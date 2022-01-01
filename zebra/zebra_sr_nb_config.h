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
#ifndef _FRR_ZEBRA_SR_NB_CONFIG_H_
#define _FRR_ZEBRA_SR_NB_CONFIG_H_

/*
 * XPath: /frr-zebra-sr:sr/enable
 */
int nb_lib_sr_enable_create(struct nb_cb_create_args *args);
int nb_lib_sr_enable_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/mappings/mapping
 */
int nb_lib_sr_mapping_create(struct nb_cb_create_args *args);
int nb_lib_sr_mapping_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/mappings/mapping/sid-start
 */
int nb_lib_sr_mapping_sid_start_modify(struct nb_cb_modify_args *args);
int nb_lib_sr_mapping_sid_start_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/mappings/mapping/sid-range
 */
int nb_lib_sr_mapping_sid_range_modify(struct nb_cb_modify_args *args);
int nb_lib_sr_mapping_sid_range_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/mappings/mapping/flag-attached
 */
int nb_lib_sr_mapping_flag_attached_modify(struct nb_cb_modify_args *args);
int nb_lib_sr_mapping_flag_attached_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/adjacency-sid/interfaces/interface
 */
int nb_lib_sr_adjacency_sid_interface_create(struct nb_cb_create_args *args);
int nb_lib_sr_adjacency_sid_interface_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family
 */
int nb_lib_sr_adjacency_sid_interface_af_create(struct nb_cb_create_args *args);
int nb_lib_sr_adjacency_sid_interface_af_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop
 */
int nb_lib_sr_adjacency_sid_interface_af_nexthop_create(struct nb_cb_create_args *args);
int nb_lib_sr_adjacency_sid_interface_af_nexthop_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/sid-type
 */
int nb_lib_sr_adjacency_sid_interface_af_nexthop_sid_type_modify(struct nb_cb_modify_args *args);
int nb_lib_sr_adjacency_sid_interface_af_nexthop_sid_type_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/absolute-sid
 */
int nb_lib_sr_adjacency_sid_interface_af_nexthop_absolute_sid_modify(struct nb_cb_modify_args *args);
int nb_lib_sr_adjacency_sid_interface_af_nexthop_absolute_sid_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/index-sid
 */
int nb_lib_sr_adjacency_sid_interface_af_nexthop_index_sid_modify(struct nb_cb_modify_args *args);
int nb_lib_sr_adjacency_sid_interface_af_nexthop_index_sid_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/srlb
 */
int nb_lib_sr_adjacency_sid_interface_af_nexthop_srlb_modify(struct nb_cb_modify_args *args);
int nb_lib_sr_adjacency_sid_interface_af_nexthop_srlb_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/local-block
 */
int nb_lib_sr_local_block_create(struct nb_cb_create_args *args);
int nb_lib_sr_local_block_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/local-block/lower-bound
 */
int nb_lib_sr_local_block_lower_bound_modify(struct nb_cb_modify_args *args);

/*
 * XPath: /frr-zebra-sr:sr/local-block/upper-bound
 */
int nb_lib_sr_local_block_upper_bound_modify(struct nb_cb_modify_args *args);

/*
 * XPath: /frr-zebra-sr:sr/global-block
 */
int nb_lib_sr_global_block_create(struct nb_cb_create_args *args);
int nb_lib_sr_global_block_destroy(struct nb_cb_destroy_args *args);

/*
 * XPath: /frr-zebra-sr:sr/global-block/lower-bound
 */
int nb_lib_sr_global_block_lower_bound_modify(struct nb_cb_modify_args *args);

/*
 * XPath: /frr-zebra-sr:sr/global-block/upper-bound
 */
int nb_lib_sr_global_block_upper_bound_modify(struct nb_cb_modify_args *args);

#endif

