/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <sdague@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef SNMP_BC_DISCOVER_H
#define SNMP_BC_DISCOVER_H

/* Resource discovery prototypes */
struct oh_event * snmp_bc_discover_blade(struct oh_handler_state *handle, char *blade_vector, SaHpiEntityPathT *ep, int bladenum);
struct oh_event * snmp_bc_discover_blade_addin(struct oh_handler_state *handle, char *blade_vector, SaHpiEntityPathT *ep, int bladenum);
struct oh_event * snmp_bc_discover_chassis(struct oh_handler_state *handle, char *blade_vector, SaHpiEntityPathT *ep);
struct oh_event * snmp_bc_discover_fan(struct oh_handler_state *handle, char *fan_vector, SaHpiEntityPathT *ep, int fannum);
struct oh_event * snmp_bc_discover_mediatray(struct oh_handler_state *handle, long exists, SaHpiEntityPathT *ep, int mtnum);
struct oh_event * snmp_bc_discover_mgmnt(struct oh_handler_state *handle, char *mm_vector, SaHpiEntityPathT *ep, int mmnum);
struct oh_event * snmp_bc_discover_power(struct oh_handler_state *handle, char *power_vector, SaHpiEntityPathT *ep, int powernum);
struct oh_event * snmp_bc_discover_switch(struct oh_handler_state *handle, char *switch_vector, SaHpiEntityPathT *ep, int switchnum);

/* RDR discovery prototypes */
struct oh_event * snmp_bc_discover_controls(struct oh_handler_state *handle,
					    SaHpiEntityPathT parent_ep,
					    const struct snmp_bc_control *control);

struct oh_event * snmp_bc_discover_sensors(struct oh_handler_state *handle,
                                           SaHpiEntityPathT parent_ep,
                                           const struct snmp_bc_sensor *sensor);

struct oh_event * snmp_bc_discover_inventories(struct oh_handler_state *handle,
                                           SaHpiEntityPathT parent_ep,
                                           const struct snmp_bc_inventory *inventory);

#endif
