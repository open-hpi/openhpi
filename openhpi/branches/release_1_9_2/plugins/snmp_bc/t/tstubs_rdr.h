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
 * This header file contains prototypes to stub out Infra-structure RDR calls.
 * This file is used in "unit testing" the various plugin interface functions
 */

#ifndef TSTUBS_RDR_H
#define TSTUBS_RDR_H

void *oh_get_rdr_data(RPTable *table,
		      SaHpiResourceIdT rid,
		      SaHpiEntryIdT rdrid);

SaHpiRdrT *oh_get_rdr_by_type(RPTable *table,
			      SaHpiResourceIdT rid,
			      SaHpiRdrTypeT type,
			      SaHpiUint8T num);

#endif
