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
 *      W. David Ashley <dashley@us.ibm.com>
 */

#ifndef SNMP_RSA_DISCOVER_H
#define SNMP_RSA_DISCOVER_H

/* Resource discovery prototypes */
struct oh_event * snmp_rsa_discover_chassis(SaHpiEntityPathT *ep);
struct oh_event * snmp_rsa_discover_cpu(SaHpiEntityPathT *ep, int mmnum);
struct oh_event * snmp_rsa_discover_dasd(SaHpiEntityPathT *ep, int mmnum);

/* RDR discovery prototypes */
struct oh_event * snmp_rsa_discover_controls(struct snmp_session *ss,
					    SaHpiEntityPathT parent_ep,
					    const struct snmp_rsa_control *control);

struct oh_event * snmp_rsa_discover_sensors(struct snmp_session *ss,
                                           SaHpiEntityPathT parent_ep,
                                           const struct snmp_rsa_sensor *sensor);

struct oh_event * snmp_rsa_discover_inventories(struct snmp_session *ss,
                                           SaHpiEntityPathT parent_ep,
                                           const struct snmp_rsa_inventory *inventory);

#endif /* SNMP_RSA_DISCOVER_H */
