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

int main(int argc, char **argv)
{
        SaErrorT        rv = SA_OK;
        SaHpiSessionIdT sessionid;
        SaHpiResourceIdT resourceId = SAHPI_UNSPECIFIED_RESOURCE_ID;
        SaHpiEventLogEntryIdT currentId = SAHPI_FIRST_ENTRY, prevId, nextId;
        SaHpiEventLogEntryT entry;
        SaHpiRdrT rdr;
        SaHpiRptEntryT rptentry;
        SaHpiEntityPathT *ep = NULL;

        printf("saHpiEventLogEntyGet: Test for hpi eventlog "
               "entry get function\n");
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                printf("saHpiSessionOpen failed with error: %s\n",
                       oh_lookup_error(rv));
                exit(-1);
        }

        do {
                rv = saHpiEventLogEntryGet(sessionid, resourceId, currentId,
                                           &prevId, &nextId, &entry, &rdr,
                                           &rptentry);
                if (rv != SA_OK) {
                        printf("Get event log entry failed with error: %s\n",
                               oh_lookup_error(rv));
                        printf("Test case - FAIL\n");
                        exit (-1);
                }

                if (rptentry.ResourceCapabilities) {
                        ep = &rptentry.ResourceEntity;
                        oh_print_eventlogentry(&entry, ep, 6);
                }
                currentId = nextId;
        } while (nextId != SAHPI_NO_MORE_ENTRIES);

        printf("Test case - PASS\n");
        rv = saHpiSessionClose(sessionid);
        return 0;
}

