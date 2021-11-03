/*
 * Zebra SRv6 definitions
 * Copyright (C) 2020  Hiroki Shirokura, LINE Corporation
 * Copyright (C) 2020  Masakazu Asama
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <zebra.h>

#include "network.h"
#include "prefix.h"
#include "stream.h"
#include "srv6.h"
#include "zebra/debug.h"
#include "zebra/zapi_msg.h"
#include "zebra/zserv.h"
#include "zebra/zebra_router.h"
#include "zebra/zebra_srv6.h"
#include "zebra/zebra_errors.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>


DEFINE_MGROUP(SRV6_MGR, "SRv6 Manager");
DEFINE_MTYPE_STATIC(SRV6_MGR, SRV6M_CHUNK, "SRv6 Manager Chunk");

/* define hooks for the basic API, so that it can be specialized or served
 * externally
 */

DEFINE_HOOK(srv6_manager_client_connect,
	    (struct zserv *client, vrf_id_t vrf_id),
	    (client, vrf_id));
DEFINE_HOOK(srv6_manager_client_disconnect,
	    (struct zserv *client), (client));
DEFINE_HOOK(srv6_manager_get_chunk,
	    (struct srv6_locator_chunk * *chunk, struct zserv *client,
	     const char *locator_name, vrf_id_t vrf_id),
	    (chunk, client, locator_name, vrf_id));
DEFINE_HOOK(srv6_manager_release_chunk,
	    (struct zserv *client,
	     const char *locator_name,
	     vrf_id_t vrf_id),
	    (client, locator_name, vrf_id));

/* define wrappers to be called in zapi_msg.c (as hooks must be called in
 * source file where they were defined)
 */

void srv6_manager_client_connect_call(struct zserv *client, vrf_id_t vrf_id)
{
	hook_call(srv6_manager_client_connect, client, vrf_id);
}

void srv6_manager_get_locator_chunk_call(struct srv6_locator_chunk **chunk,
					 struct zserv *client,
					 const char *locator_name,
					 vrf_id_t vrf_id)
{
	hook_call(srv6_manager_get_chunk, chunk, client, locator_name, vrf_id);
}

void srv6_manager_release_locator_chunk_call(struct zserv *client,
					     const char *locator_name,
					     vrf_id_t vrf_id)
{
	hook_call(srv6_manager_release_chunk, client, locator_name, vrf_id);
}

int srv6_manager_client_disconnect_cb(struct zserv *client)
{
	hook_call(srv6_manager_client_disconnect, client);
	return 0;
}

static int zebra_srv6_cleanup(struct zserv *client)
{
	return 0;
}

void zebra_srv6_locator_add(struct srv6_locator *locator)
{
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();
	struct srv6_locator *tmp;
	struct listnode *node;
	struct zserv *client;

	tmp = zebra_srv6_locator_lookup(locator->name);
	if (!tmp)
		listnode_add(srv6->locators, locator);

	/*
	 * Notify new locator info to zclients.
	 *
	 * The srv6 locators and their prefixes are managed by zserv(zebra).
	 * And an actual configuration the srv6 sid in the srv6 locator is done
	 * by zclient(bgpd, isisd, etc). The configuration of each locator
	 * allocation and specify it by zserv and zclient should be
	 * asynchronous. For that, zclient should be received the event via
	 * ZAPI when a srv6 locator is added on zebra.
	 * Basically, in SRv6, adding/removing SRv6 locators is performed less
	 * frequently than adding rib entries, so a broad to all zclients will
	 * not degrade the overall performance of FRRouting.
	 */
	for (ALL_LIST_ELEMENTS_RO(zrouter.client_list, node, client))
		zsend_zebra_srv6_locator_add(client, locator);
}

void zebra_srv6_locator_delete(struct srv6_locator *locator)
{
	struct listnode *n;
	struct srv6_locator_chunk *c;
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();
	struct zserv *client;

	/*
	 * Notify deleted locator info to zclients if needed.
	 *
	 * zclient(bgpd,isisd,etc) allocates a sid from srv6 locator chunk and
	 * uses it for its own purpose. For example, in the case of BGP L3VPN,
	 * the SID assigned to vpn unicast rib will be given.
	 * And when the locator is deleted by zserv(zebra), those SIDs need to
	 * be withdrawn. The zclient must initiate the withdrawal of the SIDs
	 * by ZEBRA_SRV6_LOCATOR_DELETE, and this notification is sent to the
	 * owner of each chunk.
	 */
	for (ALL_LIST_ELEMENTS_RO((struct list *)locator->chunks, n, c)) {
		if (c->proto == ZEBRA_ROUTE_SYSTEM)
			continue;
		client = zserv_find_client(c->proto, c->instance);
		if (!client) {
			zlog_warn(
				"%s: Not found zclient(proto=%u, instance=%u).",
				__func__, c->proto, c->instance);
			continue;
		}
		zsend_zebra_srv6_locator_delete(client, locator);
	}

	listnode_delete(srv6->locators, locator);
}

struct srv6_locator *zebra_srv6_locator_lookup(const char *name)
{
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();
	struct srv6_locator *locator;
	struct listnode *node;

	for (ALL_LIST_ELEMENTS_RO(srv6->locators, node, locator))
		if (!strncmp(name, locator->name, SRV6_LOCNAME_SIZE))
			return locator;
	return NULL;
}

struct zebra_srv6 *zebra_srv6_get_default(void)
{
	static struct zebra_srv6 srv6;
	static bool first_execution = true;

	if (first_execution) {
		first_execution = false;
		srv6.locators = list_new();
	}
	return &srv6;
}

/**
 * inherit attributes from locator to chunk
 *
 * Inherits attributes from locator to chunk.
 * @param loc SRv6 locator that is a source of shared attributes.
 * @param chunk SRv6 chunk that receives attributes from the parent locator.
 * @return void
 */
static void
inherit_attrs_from_locator_to_chunk(struct srv6_locator *loc,
				    struct srv6_locator_chunk *chunk)
{
	chunk->block_bits_length = loc->block_bits_length;
	chunk->node_bits_length = loc->node_bits_length;
	chunk->function_bits_length = loc->function_bits_length;
	chunk->argument_bits_length = loc->argument_bits_length;
	strlcpy(chunk->locator_name, loc->name, sizeof(chunk->locator_name));
	return;
}

/**
 * separate locator prefix into chunk
 *
 * currently each locator-chunk only uses /68 prefix.
 * we may need to change the ipv6-block operation for supporting
 * more flexible perchunk_length.
 * @param loc SRv6 locator that is a source of shared attributes.
 * @param chunk_index Index of locator's chunk pool.
 * @return the new chunk's prefix or NULL.
 */
static struct prefix_ipv6
separate_locator_prefix_into_chunk(struct srv6_locator *loc,
				   uint16_t chunk_index)
{
	struct prefix_ipv6 chunk_prefix = loc->prefix;
	chunk_prefix.prefixlen += loc->perchunk_bits_length;
	chunk_prefix.prefix.s6_addr16[4] = htons(chunk_index);
	return chunk_prefix;
}

/**
 * Core function, assigns srv6-locator chunks
 *
 * It first searches through the list to check if there's one available
 * (previously released). Otherwise it creates and assigns a new one
 *
 * @param proto Daemon protocol of client, to identify the owner
 * @param instance Instance, to identify the owner
 * @param session_id SessionID of client
 * @param name Name of SRv6-locator
 * @return Pointer to the assigned srv6-locator chunk,
 *         or NULL if the request could not be satisfied
 */
static struct srv6_locator_chunk *
assign_srv6_locator_chunk(uint8_t proto, uint16_t instance, uint32_t session_id,
			  const char *locator_name)
{
	bool chunk_found = false;
	struct listnode *node = NULL;
	struct srv6_locator *loc = NULL;
	struct srv6_locator_chunk *chunk = NULL;

	loc = zebra_srv6_locator_lookup(locator_name);
	if (!loc) {
		zlog_info("%s: locator %s was not found",
			  __func__, locator_name);
		return NULL;
	}

	if (list_isempty(loc->chunks) || srv6_locator_chunks_exhausted(loc)) {
		uint16_t current_chunks = listcount(loc->chunks);
		uint16_t expansion_end =
			current_chunks ? 2 * current_chunks : 1;

		// currently each locator-chunk only uses /68 prefix.
		if (expansion_end > DEFAULT_SRV6_LOCATOR_CHUNK_ENTRIES) {
			zlog_err("%s: locator %s chunk pool no longer expanded",
				 __func__, locator_name);
			return NULL;
		}

		for (uint16_t chunk_index = current_chunks;
		     chunk_index < expansion_end; chunk_index++) {
			chunk = srv6_locator_chunk_alloc();
			inherit_attrs_from_locator_to_chunk(loc, chunk);
			chunk->prefix = separate_locator_prefix_into_chunk(
				loc, chunk_index);
			listnode_add(loc->chunks, chunk);
		}
	}

	for (ALL_LIST_ELEMENTS_RO((struct list *)loc->chunks, node, chunk)) {
		if (chunk->proto != NO_PROTO && chunk->proto != proto)
			continue;
		chunk_found = true;
		break;
	}

	if (!chunk_found) {
		zlog_info("%s: locator is already owned", __func__);
		return NULL;
	}

	chunk->proto = proto;
	chunk->instance = instance;
	chunk->session_id = session_id;
	return chunk;
}

static int
zebra_srv6_manager_get_locator_chunk(struct srv6_locator_chunk **chunk,
				     struct zserv *client,
				     const char *locator_name, vrf_id_t vrf_id)
{
	int ret = 0;

	*chunk = assign_srv6_locator_chunk(client->proto, client->instance,
					   client->session_id, locator_name);

	if (!*chunk) {
		zlog_err("Unable to assign locator chunk to %s instance %u",
			 zebra_route_string(client->proto), client->instance);
		return ret;
	}

	if (IS_ZEBRA_DEBUG_PACKET)
		zlog_info("Assigned locator chunk %s to %s instance %u",
			  locator_name, zebra_route_string(client->proto),
			  client->instance);

	ret = zsend_srv6_manager_get_locator_chunk_response(client, vrf_id,
							    *chunk);
	return ret;
}

/**
 * Core function, release no longer used srv6-locator chunks
 *
 * @param proto Daemon protocol of client, to identify the owner
 * @param instance Instance, to identify the owner
 * @param session_id Zclient session ID, to identify the zclient session
 * @param locator_name SRv6-locator name, to identify the actual locator
 * @return 0 on success, -1 otherwise
 */
static int release_srv6_locator_chunk(uint8_t proto, uint16_t instance,
				      uint32_t session_id,
				      const char *locator_name)
{
	int ret = -1;
	struct listnode *node;
	struct srv6_locator_chunk *chunk;
	struct srv6_locator *loc = NULL;

	loc = zebra_srv6_locator_lookup(locator_name);
	if (!loc)
		return -1;

	if (IS_ZEBRA_DEBUG_PACKET)
		zlog_debug("%s: Releasing srv6-locator on %s", __func__,
			   locator_name);

	for (ALL_LIST_ELEMENTS_RO((struct list *)loc->chunks, node, chunk)) {
		if (chunk->proto != proto ||
		    chunk->instance != instance ||
		    chunk->session_id != session_id)
			continue;
		chunk->proto = NO_PROTO;
		chunk->instance = 0;
		chunk->session_id = 0;
		chunk->keep = 0;
		ret = 0;
		break;
	}

	if (ret != 0)
		flog_err(EC_ZEBRA_SRV6M_UNRELEASED_LOCATOR_CHUNK,
			 "%s: SRv6 locator chunk not released", __func__);

	return ret;
}

static int zebra_srv6_manager_release_locator_chunk(struct zserv *client,
						    const char *locator_name,
						    vrf_id_t vrf_id)
{
	if (vrf_id != VRF_DEFAULT) {
		zlog_err("SRv6 locator doesn't support vrf");
		return -1;
	}

	return release_srv6_locator_chunk(client->proto, client->instance,
					  client->session_id, locator_name);
}

/**
 * Release srv6-locator chunks from a client.
 *
 * Called on client disconnection or reconnection. It only releases chunks
 * with empty keep value.
 *
 * @param proto Daemon protocol of client, to identify the owner
 * @param instance Instance, to identify the owner
 * @return Number of chunks released
 */
int release_daemon_srv6_locator_chunks(struct zserv *client)
{
	int ret;
	int count = 0;
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();
	struct listnode *loc_node;
	struct listnode *chunk_node;
	struct srv6_locator *loc;
	struct srv6_locator_chunk *chunk;

	if (IS_ZEBRA_DEBUG_PACKET)
		zlog_debug("%s: Releasing chunks for client proto %s, instance %d, session %u",
			   __func__, zebra_route_string(client->proto),
			   client->instance, client->session_id);

	for (ALL_LIST_ELEMENTS_RO(srv6->locators, loc_node, loc)) {
		for (ALL_LIST_ELEMENTS_RO(loc->chunks, chunk_node, chunk)) {
			if (chunk->proto == client->proto &&
			    chunk->instance == client->instance &&
			    chunk->session_id == client->session_id &&
			    chunk->keep == 0) {
				ret = release_srv6_locator_chunk(
						chunk->proto, chunk->instance,
						chunk->session_id, loc->name);
				if (ret == 0)
					count++;
			}
		}
	}

	if (IS_ZEBRA_DEBUG_PACKET)
		zlog_debug("%s: Released %d srv6-locator chunks",
			   __func__, count);

	return count;
}

void zebra_srv6_init(void)
{
	hook_register(zserv_client_close, zebra_srv6_cleanup);
	hook_register(srv6_manager_get_chunk,
		      zebra_srv6_manager_get_locator_chunk);
	hook_register(srv6_manager_release_chunk,
		      zebra_srv6_manager_release_locator_chunk);
}

bool zebra_srv6_is_enable(void)
{
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();

	return listcount(srv6->locators);
}
