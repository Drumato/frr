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

#ifndef _ZEBRA_SRV6_VTY_H
#define _ZEBRA_SRV6_VTY_H

extern void zebra_srv6_vty_init(void);

/* Optional 'cli_show' callbacks. */
void cli_show_segment_routing_srv6(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults);
void cli_show_segment_routing_srv6_end(struct vty *vty, const struct lyd_node *dnode);
void cli_show_srv6_locators(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults);
void cli_show_srv6_locators_end(struct vty *vty, const struct lyd_node *dnode);
void cli_show_srv6_locators_locators(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults);
void cli_show_srv6_locators_locators_end(struct vty *vty, const struct lyd_node *dnode);
void cli_show_srv6_locator(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults);
void cli_show_srv6_locator_end(struct vty *vty, const struct lyd_node *dnode);
void cli_show_srv6_locator_prefix_container(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults);

void cli_show_srv6_locator_prefix_container_end(struct vty *vty, const struct lyd_node *dnode);
void cli_show_srv6_locator_prefix(struct vty *vty, const struct lyd_node *dnode,
			bool show_defaults);
void cli_show_srv6_locator_prefix_end(struct vty *vty, const struct lyd_node *dnode);

#endif /* _ZEBRA_SRV6_VTY_H */
