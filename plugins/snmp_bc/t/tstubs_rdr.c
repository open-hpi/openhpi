/* -*- linux-c -*-
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
 *     Steve Sherman <stevees@us.ibm.com>
 */

/*
 * These functions stub out infra-structure RDR calls. They are used to 
 * "unit test" the various plugin interface calls.
 */ 

void *oh_get_rdr_data(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
	if (ifobj_data_force_error || table == NULL) { return NULL; }
	else { return((void *)(&(((struct rdr_stub *)table)->Test.sensor_info.mib))); }
}

SaHpiRdrT *oh_get_rdr_by_type(RPTable *table, SaHpiResourceIdT rid,
                              SaHpiRdrTypeT type, SaHpiUint8T num)
{
	if (ifobj_force_error || table == NULL) { return NULL; }
	else { return (&(((struct rdr_stub *)table)->Rdr)); }
}
