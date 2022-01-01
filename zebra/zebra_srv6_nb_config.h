/*
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
#ifndef _FRR_ZEBRA_SRV6_NB_CONFIG_H_
#define _FRR_ZEBRA_SRV6_NB_CONFIG_H_

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator
 */
int nb_lib_srv6_locator_create(struct nb_cb_create_args *args);
int nb_lib_srv6_locator_destroy(struct nb_cb_destroy_args *args);
const void *nb_lib_srv6_locator_get_next(struct nb_cb_get_next_args *args);
int nb_lib_srv6_locator_get_keys(struct nb_cb_get_keys_args *args);
const void *nb_lib_srv6_locator_lookup_entry(struct nb_cb_lookup_entry_args *args);

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/prefix/prefix
 */
int nb_lib_srv6_locator_prefix_modify(struct nb_cb_modify_args *args);
int nb_lib_srv6_locator_prefix_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/status-up
 */
int nb_lib_srv6_locator_status_up_modify(struct nb_cb_modify_args *args);
int nb_lib_srv6_locator_status_up_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/block-bits-length
 */
int nb_lib_srv6_locator_block_bits_length_modify(
        struct nb_cb_modify_args *args);
int nb_lib_srv6_locator_block_bits_length_destroy(
        struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/node-bits-length
 */
int nb_lib_srv6_locator_node_bits_length_modify(
        struct nb_cb_modify_args *args);
int nb_lib_srv6_locator_node_bits_length_destroy(
        struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/function-bits-length
 */
int nb_lib_srv6_locator_function_bits_length_modify(
        struct nb_cb_modify_args *args);
int nb_lib_srv6_locator_function_bits_length_destroy(
        struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/frr-zebra-srv6:srv6/locators/locators/locator/argument-bits-length
 */
int nb_lib_srv6_locator_argument_bits_length_modify(
        struct nb_cb_modify_args *args);
int nb_lib_srv6_locator_argument_bits_length_destroy(
        struct nb_cb_destroy_args *args);

#endif

