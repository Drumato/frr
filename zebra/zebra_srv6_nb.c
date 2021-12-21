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
#include "zebra/zebra_srv6_nb.h"
#include "zebra/zebra_srv6_nb_config.h"
#include "zebra/zebra_srv6_vty.h"
#include "zebra/zebra_srv6.h"

/* prototypes */

/* clang-format off */
const struct frr_yang_module_info frr_zebra_srv6_info = {
	.name = "frr-zebra-srv6",
	.nodes = {
		{
			.xpath = "/frr-zebra-srv6:srv6",
			.cbs = {
				.cli_show = cli_show_segment_routing_srv6,
				.cli_show_end = cli_show_segment_routing_srv6_end,
			},
			.priority = NB_DFLT_PRIORITY - 1,
		},
		{
			.xpath = "/frr-zebra-srv6:srv6/locators",
			.cbs = {
				.cli_show = cli_show_srv6_locators,
				.cli_show_end = cli_show_srv6_locators_end,
			},
		},
		{
			.xpath = "/frr-zebra-srv6:srv6/locators/locators",
			.cbs = {
				.cli_show = cli_show_srv6_locators_locators,
				.cli_show_end = cli_show_srv6_locators_locators_end,
			},
		},
		{
			.xpath = "/frr-zebra-srv6:srv6/locators/locators/locator",
			.cbs = {
				.cli_show = cli_show_srv6_locator,
				.cli_show_end = cli_show_srv6_locator_end,
				.create = nb_lib_srv6_locator_create,
				.destroy = nb_lib_srv6_locator_destroy,
			},
		},
		{
            .xpath = "/frr-zebra-srv6:srv6/locators/locators/locator/prefix",
            .cbs = {
                .cli_show = cli_show_srv6_locator_prefix_container,
                .cli_show_end = cli_show_srv6_locator_prefix_container_end,
            },
        },
		{
            .xpath = "/frr-zebra-srv6:srv6/locators/locators/locator/prefix/prefix",
            .cbs = {
                .cli_show = cli_show_srv6_locator_prefix,
                .cli_show_end = cli_show_srv6_locator_prefix_end,
                .modify = nb_lib_srv6_locator_prefix_modify,
                .destroy = nb_lib_srv6_locator_prefix_destroy,
            },
        },
		{
            .xpath = "/frr-zebra-srv6:srv6/locators/locators/locator/status-up",
            .cbs = {
                .modify = nb_lib_srv6_locator_status_up_modify,
                .destroy = nb_lib_srv6_locator_status_up_destroy,
            },
        },
		{
            .xpath = "/frr-zebra-srv6:srv6/locators/locators/locator/block-bits-length",
            .cbs = {
                .modify = nb_lib_srv6_locator_block_bits_length_modify,
                .destroy = nb_lib_srv6_locator_block_bits_length_destroy,
            },
        },
		{
            .xpath = "/frr-zebra-srv6:srv6/locators/locators/locator/node-bits-length",
            .cbs = {
                .modify = nb_lib_srv6_locator_node_bits_length_modify,
                .destroy = nb_lib_srv6_locator_node_bits_length_destroy,
            },
        },
		{
            .xpath = "/frr-zebra-srv6:srv6/locators/locators/locator/function-bits-length",
            .cbs = {
                .modify = nb_lib_srv6_locator_function_bits_length_modify,
                .destroy = nb_lib_srv6_locator_function_bits_length_destroy,
            },
        },
		{
            .xpath = "/frr-zebra-srv6:srv6/locators/locators/locator/argument-bits-length",
            .cbs = {
                .modify = nb_lib_srv6_locator_argument_bits_length_modify,
                .destroy = nb_lib_srv6_locator_argument_bits_length_destroy,
            },
        },

		{
			.xpath = NULL,
		},
	}
};

