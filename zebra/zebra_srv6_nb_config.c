/*
 * Zebra SRv6 Northbound Callbacks
 * Copyright (C) 2021  Yamato Sugawara, LINE Corporation
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

#include "lib/northbound.h"
#include "lib/srv6.h"
#include "libfrr.h"
#include "zebra/label_manager.h"
#include "zebra/zebra_srv6.h"
#include "zebra/zebra_srv6_nb_config.h"

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator
 */
int nb_lib_srv6_locator_create(struct nb_cb_create_args *args)
{
	struct srv6_locator *loc;
	struct srv6_locator_chunk *chunk;
	const char *loc_name;

    if (args->event != NB_EV_APPLY) return NB_OK;

	loc_name = yang_dnode_get_string(args->dnode, "./name");
	loc = zebra_srv6_locator_lookup(loc_name);
	if (!loc) {
        /* SRv6 manager pre-allocates one chunk for zclients */
		loc = srv6_locator_alloc(loc_name);
		chunk = srv6_locator_chunk_alloc();
		chunk->proto = NO_PROTO;
		listnode_add(loc->chunks, chunk);
    }

	zebra_srv6_locator_add(loc);
	nb_running_set_entry(args->dnode, loc);

	return NB_OK;
}

int nb_lib_srv6_locator_destroy(struct nb_cb_destroy_args *args)
{
	struct srv6_locator *loc;
	const char *loc_name;

    if (args->event != NB_EV_APPLY) return NB_OK;

	loc_name = yang_dnode_get_string(args->dnode, "./name");
	loc = zebra_srv6_locator_lookup(loc_name);
	if (!loc) {
        zlog_err("srv6_locator_destroy called but given name %s is not found", 
                loc_name);
        return NB_ERR_NOT_FOUND;
    }

	loc = nb_running_unset_entry(args->dnode);
	zebra_srv6_locator_delete(loc);

	return NB_OK;
}

const void *nb_lib_srv6_locator_get_next(struct nb_cb_get_next_args *args) {
    const struct srv6_locator *pivot_loc 
        = (struct srv6_locator *)args->list_entry;
    bool first_entry = args->list_entry == NULL;
    struct zebra_srv6 *srv6 = zebra_srv6_get_default();
    struct listnode *node;
    struct srv6_locator *loc_iter;
    struct srv6_locator *loc_next = NULL;
    bool pivot_found = false;

    if (!args->list_entry)
        return NULL;

	for (ALL_LIST_ELEMENTS_RO(srv6->locators, node, loc_iter)) {
        if (first_entry) {
            return loc_iter;
        }

        if (pivot_found) {
            loc_next = loc_iter;
            break;
        }

        if (!strncmp(pivot_loc->name, loc_iter->name, SRV6_LOCNAME_SIZE)) {
            pivot_found = true;
        }
	}

    if (!pivot_found)
        return NULL;

    return loc_next;
}

int nb_lib_srv6_locator_get_keys(struct nb_cb_get_keys_args *args) {
    const struct srv6_locator *loc = (struct srv6_locator *)args->list_entry;

    args->keys->num = 1;
    snprintf(args->keys->key[0], sizeof(args->keys->key[0]), "%s", loc->name);
    return NB_OK;

}

const void *nb_lib_srv6_locator_lookup_entry(struct nb_cb_lookup_entry_args *args){
    return zebra_srv6_locator_lookup(args->keys->key[0]);
}

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/prefix/prefix
 */
int nb_lib_srv6_locator_prefix_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY) return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    yang_dnode_get_prefix(&locator->prefix, args->dnode, NULL);

    return NB_OK;
}

int nb_lib_srv6_locator_prefix_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/status-up
 */
int nb_lib_srv6_locator_status_up_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->status_up = yang_dnode_get_bool(args->dnode, NULL);

    return NB_OK;
}

int nb_lib_srv6_locator_status_up_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/block-bits-length
 */
int nb_lib_srv6_locator_block_bits_length_modify(
        struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->block_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

int nb_lib_srv6_locator_block_bits_length_destroy(
        struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/node-bits-length
 */
int nb_lib_srv6_locator_node_bits_length_modify(
        struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->node_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

int nb_lib_srv6_locator_node_bits_length_destroy(
        struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/function-bits-length
 */
int nb_lib_srv6_locator_function_bits_length_modify(
        struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->function_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

int nb_lib_srv6_locator_function_bits_length_destroy(
        struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/argument-bits-length
 */
int nb_lib_srv6_locator_argument_bits_length_modify(
        struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->argument_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

int nb_lib_srv6_locator_argument_bits_length_destroy(
        struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

