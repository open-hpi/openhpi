/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      W. david Ashley <dashley@us.ibm.com>
 *
 */


#ifndef __SIM_INJECTOR_H
#define __SIM_INJECTOR_H


typedef enum {
        OHPI_INJECT_EVENT = 1,
        OHPI_INJECT_RESOURCE,
        OHPI_INJECT_RDR
} SimInjectorTypeT;


RPTable * sim_inject_get_rptcache(oHpiHandlerIdT id);
SaErrorT sim_inject_resource(oHpiHandlerIdT id, void *data);
SaErrorT sim_inject_rdr(oHpiHandlerIdT id, SaHpiResourceIdT resid, void *data);
SaErrorT sim_inject_event(oHpiHandlerIdT id, void *data);

#endif /*__SIM_INJECTOR_H*/
