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
 * This header file contains prototypes to stub out Infra-structure Resource calls.
 * This file is used in "unit testing" the various plugin interface functions
 */

#ifndef TSTUBS_RES_H
#define TSTUBS_RES_H

void *oh_get_resource_data(RPTable *table,
			   SaHpiResourceIdT rid);

SaHpiRptEntryT *oh_get_resource_by_id(RPTable *table,
				      SaHpiResourceIdT rid);

#endif
