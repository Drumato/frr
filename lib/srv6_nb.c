/*
 * FRRouting SRv6 Northbound Information
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
#include "lib/srv6_nb.h"
#include "libfrr.h"

/* clang-format off */
const struct frr_yang_module_info frr_srv6_info = {
	.name = "frr-srv6",
	.nodes = {
		{
			.xpath = NULL,
		},
	}
};

