/*
 * Zebra SRv6 VTY functions
 * Copyright (C) 2020  Hiroki Shirokura, LINE Corporation
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

#include "memory.h"
#include "if.h"
#include "prefix.h"
#include "command.h"
#include "table.h"
#include "rib.h"
#include "nexthop.h"
#include "vrf.h"
#include "srv6.h"
#include "lib/json.h"
#include "lib/command.h"
#include "lib/northbound_cli.h"

#include "zebra/zserv.h"
#include "zebra/zebra_router.h"
#include "zebra/zebra_vrf.h"
#include "zebra/zebra_srv6.h"
#include "zebra/zebra_srv6_vty.h"
#include "zebra/zebra_rnh.h"
#include "zebra/redistribute.h"
#include "zebra/zebra_routemap.h"
#include "zebra/zebra_dplane.h"

#ifndef VTYSH_EXTRACT_PL
#include "zebra/zebra_srv6_vty_clippy.c"
#endif

#define SRV6_LOCATOR_CMD_STR \
    "Create SRv6 Locator\n" \
    "Specify locator name\n"

static int zebra_sr_config(struct vty *vty);

static struct cmd_node sr_node = {
	.name = "sr",
	.node = SEGMENT_ROUTING_NODE,
	.parent_node = CONFIG_NODE,
	.prompt = "%s(config-sr)# ",
	.config_write = zebra_sr_config,
};

static struct cmd_node srv6_node = {
	.name = "srv6",
	.node = SRV6_NODE,
	.parent_node = SEGMENT_ROUTING_NODE,
	.prompt = "%s(config-srv6)# ",
};

static struct cmd_node srv6_locs_node = {
	.name = "srv6-locators",
	.node = SRV6_LOCS_NODE,
	.parent_node = SRV6_NODE,
	.prompt = "%s(config-srv6-locators)# ",
};

static struct cmd_node srv6_loc_node = {
	.name = "srv6-locator",
	.node = SRV6_LOC_NODE,
	.parent_node = SRV6_LOCS_NODE,
	.prompt = "%s(config-srv6-locator)# "
};
DEFUN (show_srv6_locator,
       show_srv6_locator_cmd,
       "show segment-routing srv6 locator [json]",
       SHOW_STR
       "Segment Routing\n"
       "Segment Routing SRv6\n"
       "Locator Information\n"
       JSON_STR)
{
	const bool uj = use_json(argc, argv);
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();
	struct srv6_locator *locator;
	struct listnode *node;
	char str[256];
	int id;
	json_object *json = NULL;
	json_object *json_locators = NULL;
	json_object *json_locator = NULL;

	if (uj) {
		json = json_object_new_object();
		json_locators = json_object_new_array();
		json_object_object_add(json, "locators", json_locators);

		for (ALL_LIST_ELEMENTS_RO(srv6->locators, node, locator)) {
			json_locator = srv6_locator_json(locator);
			if (!json_locator)
				continue;
			json_object_array_add(json_locators, json_locator);

		}

		vty_json(vty, json);
	} else {
		vty_out(vty, "Locator:\n");
		vty_out(vty, "Name                 ID      Prefix                   Status\n");
		vty_out(vty, "-------------------- ------- ------------------------ -------\n");

		id = 1;
		for (ALL_LIST_ELEMENTS_RO(srv6->locators, node, locator)) {
			prefix2str(&locator->prefix, str, sizeof(str));
			vty_out(vty, "%-20s %7d %-24s %s\n",
				locator->name, id, str,
				locator->status_up ? "Up" : "Down");
			++id;
		}
		vty_out(vty, "\n");
	}

	return CMD_SUCCESS;
}

DEFUN (show_srv6_locator_detail,
       show_srv6_locator_detail_cmd,
       "show segment-routing srv6 locator NAME detail [json]",
       SHOW_STR
       "Segment Routing\n"
       "Segment Routing SRv6\n"
       "Locator Information\n"
       "Locator Name\n"
       "Detailed information\n"
       JSON_STR)
{
	const bool uj = use_json(argc, argv);
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();
	struct srv6_locator *locator;
	struct listnode *node;
	char str[256];
	const char *locator_name = argv[4]->arg;
	json_object *json_locator = NULL;

	if (uj) {
		locator = zebra_srv6_locator_lookup(locator_name);
		if (!locator)
			return CMD_WARNING;

		json_locator = srv6_locator_detailed_json(locator);
		vty_json(vty, json_locator);
		return CMD_SUCCESS;
	}

	for (ALL_LIST_ELEMENTS_RO(srv6->locators, node, locator)) {
		struct listnode *node;
		struct srv6_locator_chunk *chunk;

		if (strcmp(locator->name, locator_name) != 0)
			continue;

		prefix2str(&locator->prefix, str, sizeof(str));
		vty_out(vty, "Name: %s\n", locator->name);
		vty_out(vty, "Prefix: %s\n", str);
		vty_out(vty, "Function-Bit-Len: %u\n",
			locator->function_bits_length);

		vty_out(vty, "Chunks:\n");
		for (ALL_LIST_ELEMENTS_RO((struct list *)locator->chunks, node,
					  chunk)) {
			prefix2str(&chunk->prefix, str, sizeof(str));
			vty_out(vty, "- prefix: %s, owner: %s\n", str,
				zebra_route_string(chunk->proto));
		}
	}


	return CMD_SUCCESS;
}

DEFUN_NOSH (segment_routing,
            segment_routing_cmd,
            "segment-routing",
            "Segment Routing\n")
{
	vty->node = SEGMENT_ROUTING_NODE;
	return CMD_SUCCESS;
}

DEFUN_NOSH (srv6,
            srv6_cmd,
            "srv6",
            "Segment Routing SRv6\n")
{
	vty->node = SRV6_NODE;
	return CMD_SUCCESS;
}

DEFUN_YANG (no_srv6,
       no_srv6_cmd,
       "no srv6",
       NO_STR
       "Segment Routing SRv6\n")
{
	struct zebra_srv6 *srv6 = zebra_srv6_get_default();
	struct srv6_locator *locator;
	struct listnode *node, *nnode;
    char xpath[XPATH_MAXLEN];
    int rv;

    /* For consistency between global srv6 manager and yang dnode */
	for (ALL_LIST_ELEMENTS(srv6->locators, node, nnode, locator)) {
	    snprintf(xpath, sizeof(xpath),
		    "/frr-zebra-srv6:srv6/locators/locators/locator[name='%s']", locator->name);
	    nb_cli_enqueue_change(vty, xpath, NB_OP_DESTROY, NULL);
    }

    rv = nb_cli_apply_changes(vty, NULL);

	for (ALL_LIST_ELEMENTS(srv6->locators, node, nnode, locator))
		zebra_srv6_locator_delete(locator);

	return rv;
}

DEFUN_NOSH (srv6_locators,
            srv6_locators_cmd,
            "locators",
            "Segment Routing SRv6 locators\n")
{
	vty->node = SRV6_LOCS_NODE;
	return CMD_SUCCESS;
}

DEFPY_YANG_NOSH(srv6_locator,
                srv6_locator_cmd,
                "locator LOC_NAME$name",
                SRV6_LOCATOR_CMD_STR)
{
	char xpath[XPATH_MAXLEN];
	int rv;

	snprintf(xpath, sizeof(xpath),
		 "/frr-zebra-srv6:srv6/locators/locators/locator[name='%s']", name);
	nb_cli_enqueue_change(vty, xpath, NB_OP_CREATE, NULL);

	rv = nb_cli_apply_changes(vty, xpath);
	if (rv == CMD_SUCCESS)
		VTY_PUSH_XPATH(SRV6_LOC_NODE, xpath);

	return rv;
}

DEFPY_YANG(
	no_srv6_locator, 
    no_srv6_locator_cmd,
	"no locator LOC_NAME$name",
    NO_STR
    SRV6_LOCATOR_CMD_STR)
{
	char xpath[XPATH_MAXLEN];

	snprintf(xpath, sizeof(xpath),
		 "/frr-zebra-srv6:srv6/locators/locators/locator[name='%s']", name);
	nb_cli_enqueue_change(vty, xpath, NB_OP_DESTROY, NULL);

	return nb_cli_apply_changes(vty, xpath);
}

DEFPY_YANG (locator_prefix,
       locator_prefix_cmd,
       "prefix X:X::X:X/M$prefix [func-bits (16-64)$func_bit_len]",
       "Configure SRv6 locator prefix\n"
       "Specify SRv6 locator prefix\n"
       "Configure SRv6 locator function length in bits\n"
       "Specify SRv6 locator function length in bits\n")
{
    int rv;
    char node_bits_length_str[8];
    const char *loc_name = NULL;
    struct srv6_locator*locator = NULL;
    struct srv6_locator_chunk *chunk = NULL;
    struct listnode *node = NULL;

	/*
	 * TODO(slankdev): please support variable node-bit-length.
	 * In draft-ietf-bess-srv6-services-05#section-3.2.1.
	 * Locator block length and Locator node length are defined.
	 * Which are defined as "locator-len == block-len + node-len".
	 * In current implementation, node bits length is hardcoded as 24.
	 * It should be supported various val.
	 *
	 * Cisco IOS-XR support only following pattern.
	 *  (1) Teh locator length should be 64-bits long.
	 *  (2) The SID block portion (MSBs) cannot exceed 40 bits.
	 *      If this value is less than 40 bits,
	 *      user should use a pattern of zeros as a filler.
	 *  (3) The Node Id portion (LSBs) cannot exceed 24 bits.
	 */
    snprintf(node_bits_length_str, sizeof(node_bits_length_str), 
            "%d", prefix->prefixlen - 24);

    nb_cli_enqueue_change(vty, "./prefix/prefix", NB_OP_MODIFY, prefix_str);
    nb_cli_enqueue_change(vty, "./status-up", NB_OP_MODIFY, "true");
    nb_cli_enqueue_change(vty, "./block-bits-length", NB_OP_MODIFY, node_bits_length_str);
    nb_cli_enqueue_change(vty, "./node-bits-length", NB_OP_MODIFY, "24");
    nb_cli_enqueue_change(vty, "./argument-bits-length", NB_OP_MODIFY, "0");
    if (func_bit_len != 0) {
        nb_cli_enqueue_change(vty, "./function-bits-length", 
                NB_OP_MODIFY, func_bit_len_str);
    }

    loc_name = yang_dnode_get_string(
            vty->candidate_config->dnode, "%s/name", VTY_CURR_XPATH);
    rv = nb_cli_apply_changes(vty, NULL);

    locator = zebra_srv6_locator_lookup(loc_name);
    if (!locator) {
        zlog_err("locator %s is not found", loc_name);
        return CMD_WARNING;
    }

	for (ALL_LIST_ELEMENTS_RO(locator->chunks, node, chunk)) {
		uint8_t zero[16] = {0};

		if (memcmp(&chunk->prefix.prefix, zero, 16) == 0) {
			struct zserv *client;
			struct listnode *client_node;

			chunk->prefix = locator->prefix;
			for (ALL_LIST_ELEMENTS_RO(zrouter.client_list,
							client_node,
							client)) {
				struct srv6_locator *tmp;

				if (client->proto != chunk->proto)
					continue;

				srv6_manager_get_locator_chunk_call(
						&tmp, client,
						locator->name,
						VRF_DEFAULT);
			}
		}
	}

    return rv;
}

void cli_show_segment_routing_srv6(struct vty *vty, 
        const struct lyd_node *dnode, bool show_defaults) {
	vty_out(vty, "segment-routing srv6\n");
}

void cli_show_segment_routing_srv6_end(struct vty *vty, 
        const struct lyd_node *dnode) {
	vty_out(vty, "exit\n");
}

void cli_show_srv6_locators(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults) {
	vty_out(vty, " locators\n");
}

void cli_show_srv6_locators_end(struct vty *vty, 
        const struct lyd_node *dnode) {
	vty_out(vty, " exit\n");
}

// this function doesn't anything for consisutency with cisco's cli
void cli_show_srv6_locators_locators(struct vty *vty, 
        const struct lyd_node *dnode, bool show_defaults) {
}

// this function doesn't anything for consisutency with cisco's cli
void cli_show_srv6_locators_locators_end(struct vty *vty, 
        const struct lyd_node *dnode) {
}

void cli_show_srv6_locator(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults)
{
	const char *loc_name = NULL;

	loc_name = yang_dnode_get_string(dnode, "./name");
	vty_out(vty, "  locator %s\n", loc_name);
}

void cli_show_srv6_locator_end(struct vty *vty, const struct lyd_node *dnode)
{ vty_out(vty, "  exit\n");
}

// this function doesn't anything for consisutency to cisco's cli
void cli_show_srv6_locator_prefix_container(struct vty *vty, 
        const struct lyd_node *dnode, bool show_defaults) {
}

// this function doesn't anything for consisutency to cisco's cli
void cli_show_srv6_locator_prefix_container_end(struct vty *vty, 
        const struct lyd_node *dnode) {
}

void cli_show_srv6_locator_prefix(struct vty *vty, 
        const struct lyd_node *dnode, bool show_defaults) {
    struct prefix_ipv6 prefix;
	char prefix_str[256];
    uint8_t function_bits_length;

    yang_dnode_get_prefix(&prefix, dnode, ".");
	prefix2str(&prefix, prefix_str, sizeof(prefix_str));
    function_bits_length = yang_dnode_get_uint8(
            dnode, "../../function-bits-length");
	vty_out(vty, "   prefix %s func-bits %d\n", 
            prefix_str, function_bits_length);
}

void cli_show_srv6_locator_prefix_end(struct vty *vty, 
        const struct lyd_node *dnode) {
	vty_out(vty, "   exit\n");
}

static int zebra_sr_config(struct vty *vty)
{
	int write = 0;
	struct lyd_node *dnode;

	if (zebra_srv6_is_enable()) {
		dnode = yang_dnode_get(running_config->dnode, "/frr-zebra-srv6:srv6");
		if (dnode) {
			nb_cli_show_dnode_cmds(vty, dnode, false);
			write++;
		}
	}

	return write;
}

void zebra_srv6_vty_init(void)
{
	/* Install nodes and its default commands */
	install_node(&sr_node);
	install_node(&srv6_node);
	install_node(&srv6_locs_node);
	install_node(&srv6_loc_node);
	install_default(SEGMENT_ROUTING_NODE);
	install_default(SRV6_NODE);
	install_default(SRV6_LOCS_NODE);
	install_default(SRV6_LOC_NODE);

	/* Command for change node */
	install_element(CONFIG_NODE, &segment_routing_cmd);
	install_element(SEGMENT_ROUTING_NODE, &srv6_cmd);
	install_element(SEGMENT_ROUTING_NODE, &no_srv6_cmd);
	install_element(SRV6_NODE, &srv6_locators_cmd);
	install_element(SRV6_LOCS_NODE, &srv6_locator_cmd);
	install_element(SRV6_LOCS_NODE, &no_srv6_locator_cmd);

	/* Command for configuration */
	install_element(SRV6_LOC_NODE, &locator_prefix_cmd);

	/* Command for operation */
	install_element(VIEW_NODE, &show_srv6_locator_cmd);
	install_element(VIEW_NODE, &show_srv6_locator_detail_cmd);
}
