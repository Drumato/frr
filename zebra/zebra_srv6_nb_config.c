/*
 * Zebra SRv6 definitions
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
#include "zebra/zebra_srv6.h"
#include "zebra/zebra_srv6_nb_config.h"

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator
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
		loc = srv6_locator_alloc(loc_name);
		chunk = srv6_locator_chunk_alloc();
		chunk->proto = 0;
		listnode_add(loc->chunks, chunk);
    }

	zebra_srv6_locator_add(loc);
	nb_running_set_entry(args->dnode, loc);

	return NB_OK;
}

/* 
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator
 */
int nb_lib_srv6_locator_destroy(struct nb_cb_destroy_args *args)
{
	struct srv6_locator *loc;
	const char *loc_name;

    if (args->event != NB_EV_APPLY) return NB_OK;

	loc_name = yang_dnode_get_string(args->dnode, "./name");
	loc = zebra_srv6_locator_lookup(loc_name);
	if (!loc) {
        return NB_ERR_NOT_FOUND;
    }

	loc = nb_running_unset_entry(args->dnode);
	zebra_srv6_locator_delete(loc);

	return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/prefix/prefix
 */
int nb_lib_srv6_locator_prefix_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY) return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    yang_dnode_get_prefix(&locator->prefix, args->dnode, NULL);

    return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/prefix/prefix
 */
int nb_lib_srv6_locator_prefix_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/status-up
 */
int nb_lib_srv6_locator_status_up_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->status_up = yang_dnode_get_bool(args->dnode, NULL);

    return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/status-up
 */
int nb_lib_srv6_locator_status_up_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/block-bits-length
 */
int nb_lib_srv6_locator_block_bits_length_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->block_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/block-bits-length
 */
int nb_lib_srv6_locator_block_bits_length_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/node-bits-length
 */
int nb_lib_srv6_locator_node_bits_length_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->node_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/node-bits-length
 */
int nb_lib_srv6_locator_node_bits_length_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/function-bits-length
 */
int nb_lib_srv6_locator_function_bits_length_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->function_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/function-bits-length
 */
int nb_lib_srv6_locator_function_bits_length_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/argument-bits-length
 */
int nb_lib_srv6_locator_argument_bits_length_modify(struct nb_cb_modify_args *args) {
	struct srv6_locator *locator;

    if (args->event != NB_EV_APPLY)
        return NB_OK;

    locator = nb_running_get_entry(args->dnode, NULL, true);
    locator->argument_bits_length = yang_dnode_get_uint8(args->dnode, NULL);

    return NB_OK;
}

/*
 * XPath: /frr-zebra-srv6:srv6/locators/locators/locator/argument-bits-length
 */
int nb_lib_srv6_locator_argument_bits_length_destroy(struct nb_cb_destroy_args *args)
{
    /* All the work is done in the apply_finish */
	return NB_OK;
}

