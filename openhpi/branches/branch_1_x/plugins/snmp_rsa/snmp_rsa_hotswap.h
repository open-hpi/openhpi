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

#ifndef SNMP_RSA_HOTSWAP_H
#define SNMP_RSA_HOTSWAP_H

SaErrorT snmp_rsa_get_hotswap_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiHsStateT *state);

SaErrorT snmp_rsa_set_hotswap_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiHsStateT state);

SaErrorT snmp_rsa_request_hotswap_action(void *hnd,
					SaHpiResourceIdT id,
					SaHpiHsActionT act);

SaErrorT snmp_rsa_get_reset_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiResetActionT *act);

SaErrorT snmp_rsa_set_reset_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiResetActionT act);

SaErrorT snmp_rsa_get_power_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiHsPowerStateT *state);

SaErrorT snmp_rsa_set_power_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiHsPowerStateT state);

SaErrorT snmp_rsa_get_indicator_state(void *hnd,
				     SaHpiResourceIdT id,
				     SaHpiHsIndicatorStateT *state);

SaErrorT snmp_rsa_set_indicator_state(void *hnd,
				     SaHpiResourceIdT id,
				     SaHpiHsIndicatorStateT state);

#endif /* SNMP_RSA_HOTSWAP_H */
