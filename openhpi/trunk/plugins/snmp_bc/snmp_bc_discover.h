/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef SNMP_BC_DISCOVER_H
#define SNMP_BC_DISCOVER_H

SaErrorT snmp_bc_discover_resources(void *hnd);

SaErrorT snmp_bc_discover(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root);

SaErrorT snmp_bc_discover_rsa(struct oh_handler_state *handle,
			      SaHpiEntityPathT *ep_root);

SaErrorT snmp_bc_discover_sensors(struct oh_handler_state *handle,
				  struct snmp_bc_sensor *sensor_array,
				  struct oh_event *res_oh_event);

SaErrorT snmp_bc_discover_controls(struct oh_handler_state *handle,
				   struct snmp_bc_control *control_array,
				   struct oh_event *res_oh_event);

SaErrorT snmp_bc_discover_inventories(struct oh_handler_state *handle,
				      struct snmp_bc_inventory *inventory_array,
				      struct oh_event *res_oh_event);

SaErrorT snmp_bc_create_resourcetag(SaHpiTextBufferT *buffer,
				    const char *str,
				    SaHpiEntityLocationT location);

SaHpiBoolT rdr_exists(struct snmp_session *ss,
		      const char *oid,
		      unsigned int na,
		      int write_only);
#endif
