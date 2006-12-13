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
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef __SNMP_BC_SESSION_H
#define __SNMP_BC_SESSION_H

/**
 * This handle is unique per instance of this plugin. SNMP session data is 
 * stored in the handle along with config file data.
 **/

void *snmp_bc_open(GHashTable *handler_config);
void snmp_bc_close(void *hnd);

#endif
