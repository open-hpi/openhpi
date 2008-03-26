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
#include <time.h>

int main(int argc, char **argv)
{
        SaErrorT rv;
        SaHpiSessionIdT sessionid;
        SaHpiResourceIdT resourceid = SAHPI_UNSPECIFIED_RESOURCE_ID;
        SaHpiTimeT time;
        struct tm t;
        time_t tt;

        memset(&t, 0, sizeof(t));

        printf("saHpiEventLogTimeSet: Test for hpi eventlog "
               "time set function\n");
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
        if (rv != SA_OK) {
                printf("\nsaHpiSessionOpen error %d\n"
                       "Unable to open the session", rv);
                exit(-1);
        }

        printf("\nPlease enter the time to be set for eventlong\n");
        printf("\tPlease enter the year [YYYY]: ");
        scanf("%d",&t.tm_year);
        // Year is calculated relative to 1900
        t.tm_year = t.tm_year - 1900;
        printf("\tPlease enter the month [MM]: ");
        scanf("%d",&t.tm_mon);
        printf("\tPlease enter the day [DD]: ");
        scanf("%d",&t.tm_mday);
        printf("\tPlease enter the hour [HH]: ");
        scanf("%d",&t.tm_hour);
        printf("\tPlease enter the minute [MM]: ");
        scanf("%d",&t.tm_min);
        printf("\tPlease enter the second [SS]: ");
        scanf("%d",&t.tm_sec);

        tt = mktime(&t);

        time = (SaHpiTimeT) tt * 1000000000;
        rv = saHpiEventLogTimeSet(sessionid, resourceid , time);
        if (rv != SA_OK) {
                printf("saHpiEventLogTimeSet returns error %s\n",
                       oh_lookup_error(rv));
                printf("Test Result - FAIL\n");
        }
        else {
                printf("Test Result - PASS\n");
        }

        rv = saHpiSessionClose(sessionid);
        return 0;
}
