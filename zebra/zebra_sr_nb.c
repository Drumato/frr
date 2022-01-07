/*
 * Zebra Segment-Routing Northbound Information
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

#include "lib/northbound.h"
#include "libfrr.h"
#include "zebra/zebra_sr_nb_config.h"
#include "zebra/zebra_sr_nb.h"

/* clang-format off */
const struct frr_yang_module_info frr_zebra_sr_info = {
	.name = "frr-zebra-sr",
	.nodes = {
		{
			.xpath = "/frr-zebra-sr:sr/enable",
			.cbs = {
				.create = nb_lib_sr_enable_create,
				.destroy = nb_lib_sr_enable_destroy,
			},
			.priority = NB_DFLT_PRIORITY - 1,
		},
        {
            .xpath = "/frr-zebra-sr:sr/local-block",
            .cbs = {
                .create = nb_lib_sr_local_block_create,
                .destroy = nb_lib_sr_local_block_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/local-block/lower-bound",
            .cbs = {
                .modify = nb_lib_sr_local_block_lower_bound_modify,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/local-block/upper-bound",
            .cbs = {
                .modify = nb_lib_sr_local_block_upper_bound_modify,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/mappings/mapping",
            .cbs = {
                .create = nb_lib_sr_mapping_create,
                .destroy = nb_lib_sr_mapping_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/mappings/mapping/af",
            .cbs = {
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/mappings/mapping/ip",
            .cbs = {
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/mappings/mapping/mask",
            .cbs = {
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/mappings/mapping/sid-start",
            .cbs = {
                .modify = nb_lib_sr_mapping_sid_start_modify,
                .destroy = nb_lib_sr_mapping_sid_start_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/mappings/mapping/sid-range",
            .cbs = {
                .modify = nb_lib_sr_mapping_sid_range_modify,
                .destroy = nb_lib_sr_mapping_sid_range_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/mappings/mapping/flag-attached",
            .cbs = {
                .modify = nb_lib_sr_mapping_flag_attached_modify,
                .destroy = nb_lib_sr_mapping_flag_attached_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface",
            .cbs = {
                .create = nb_lib_sr_adjacency_sid_interface_create,
                .destroy = nb_lib_sr_adjacency_sid_interface_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/interface",
            .cbs = {
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family",
            .cbs = {
                .create = nb_lib_sr_adjacency_sid_interface_af_create,
                .destroy = nb_lib_sr_adjacency_sid_interface_af_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/address-family",
            .cbs = {
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop",
            .cbs = {
                .create = nb_lib_sr_adjacency_sid_interface_af_nexthop_create,
                .destroy = nb_lib_sr_adjacency_sid_interface_af_nexthop_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/sid-type",
            .cbs = {
                .modify = nb_lib_sr_adjacency_sid_interface_af_nexthop_sid_type_modify,
                .destroy = nb_lib_sr_adjacency_sid_interface_af_nexthop_sid_type_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/absolute-sid",
            .cbs = {
                .modify = nb_lib_sr_adjacency_sid_interface_af_nexthop_absolute_sid_modify,
                .destroy = nb_lib_sr_adjacency_sid_interface_af_nexthop_absolute_sid_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/index-sid",
            .cbs = {
                .modify = nb_lib_sr_adjacency_sid_interface_af_nexthop_index_sid_modify,
                .destroy = nb_lib_sr_adjacency_sid_interface_af_nexthop_index_sid_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/adjacency-sid/interfaces/interface/address-families/address-family/next-hops/next-hop/l2-adjacency-sid/srlb",
            .cbs = {
                .modify = nb_lib_sr_adjacency_sid_interface_af_nexthop_srlb_modify,
                .destroy = nb_lib_sr_adjacency_sid_interface_af_nexthop_srlb_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/global-block",
            .cbs = {
                .create = nb_lib_sr_global_block_create,
                .destroy = nb_lib_sr_global_block_destroy,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/global-block/lower-bound",
            .cbs = {
                .modify = nb_lib_sr_global_block_lower_bound_modify,
            },
        },
        {
            .xpath = "/frr-zebra-sr:sr/global-block/upper-bound",
            .cbs = {
                .modify = nb_lib_sr_global_block_upper_bound_modify,
            },
        },

		{
			.xpath = NULL,
		},
	}
};

