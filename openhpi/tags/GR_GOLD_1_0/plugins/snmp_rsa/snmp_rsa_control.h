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

#ifndef SNMP_RSA_CONTROL_H
#define SNMP_RSA_CONTROL_H

SaErrorT snmp_rsa_get_control_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiCtrlNumT num,
				   SaHpiCtrlStateT *state);

SaErrorT snmp_rsa_set_control_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiCtrlNumT num,
				   SaHpiCtrlStateT *state);

#endif /* SNMP_RSA_CONTROL_H */
