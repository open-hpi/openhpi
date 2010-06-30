/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 */

#include "hpi_test.h"

int powerSubsystemResId = -1;

/*
 * This function opens the HPI session, discovers the resources which matches
 * the requested capability and sends the list of resource ids and resource
 * names.
 *
 * If the none of the resources matches to requested capability,
 * then SA_ERR_HPI_NOT_PRESENT error will be return.
 * On sucess SA_OK will be return.
 */
SaErrorT discover_resources(SaHpiSessionIdT sessionid,
                            SaHpiCapabilitiesT capability,
                            SaHpiResourceIdT *resourceid_list,
                            int *number_resources)
{
        int count=0;
        SaErrorT rv;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;

        rv = saHpiDiscover(sessionid);
        if (rv != SA_OK) {
                printf("saHpiDiscover failed with error: %s\n",
                       oh_lookup_error(rv));
                return rv;
        }

        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {
                rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid,
                                      &rptentry);
                if (rv != SA_OK) {
                        printf("RptEntryGet failed with error: %s\n",
                               oh_lookup_error(rv));
                        return rv;
                }

                if (rptentry.ResourceCapabilities & capability) {
                        resourceid_list[count] = rptentry.ResourceId;
                        printf("\tResource-id = %d, Resource Name = %s\n",
                               rptentry.ResourceId, rptentry.ResourceTag.Data);
                        if (strcmp(rptentry.ResourceTag.Data,
                                   "Power Subsystem") == 0)
                          powerSubsystemResId = rptentry.ResourceId;
                        count++;
                }
                rptentryid = nextrptentryid;
        }

        if (count == 0)
                return SA_ERR_HPI_NOT_PRESENT;

        *number_resources = count;
        return SA_OK;
}
