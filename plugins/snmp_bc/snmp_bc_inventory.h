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
 *	peter d phan  <pdphan@sourceforge.net>
 *
 *	02/16/2004 pdphan Split from snmp_bc.h for ease of test
 *
 */

#ifndef SNMP_BC_INVENTORY_H
#define SNMP_BC_INVENTORY_H

#include <snmp_bc.h>

/* 
 * Functions prototype
 */

SaErrorT get_inventory_data(	void *hnd, SaHpiRdrT *rdr,
				struct BC_InventoryInfo *s,
				SaHpiInventoryDataT *l_data,
				SaHpiInventGeneralDataT *working,
				SaHpiUint32T  *vpdrecordlength);

/*
SaErrorT find_inventories(	struct snmp_bc_hnd *custom_handle,
				struct snmp_bc_inventory * rdr_array,
				struct oh_event *e,
				GSList *tmpqueue,
				RPTable *tmpcache);

*/

#endif
