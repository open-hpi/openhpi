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
 *	W. David Ashley <dashley@us.ibm.com>
 *
 */

#ifndef SNMP_RSA_INVENTORY_H
#define SNMP_RSA_INVENTORY_H

#include <snmp_rsa.h>

/* 
 * Functions prototype
 */

SaErrorT get_inventory_data(	void *hnd, SaHpiRdrT *rdr,
				struct RSA_InventoryInfo *s,
				SaHpiInventoryDataT *l_data,
				SaHpiInventGeneralDataT *working,
				SaHpiUint32T  *vpdrecordlength);
int snmp_rsa_get_inventory_info(void *hnd, SaHpiResourceIdT id,
                                SaHpiEirIdT num,
                                SaHpiInventoryDataT *data);
int snmp_rsa_get_inventory_size(void *hnd, SaHpiResourceIdT id,
                               SaHpiEirIdT num,  /* yes, they don't call it a
                                                  * num, but it still is one
                                                 */
                               SaHpiUint32T *size);
int snmp_rsa_set_inventory_info(void *hnd, SaHpiResourceIdT id,
                               SaHpiEirIdT num,
                               const SaHpiInventoryDataT *data);

/*
SaErrorT find_inventories(	struct snmp_rsa_hnd *custom_handle,
				struct snmp_rsa_inventory * rdr_array,
				struct oh_event *e,
				GSList *tmpqueue,
				RPTable *tmpcache);

*/

#endif
