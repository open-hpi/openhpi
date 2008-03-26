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
 *      Sudhakar Rao <sudhakara-d-v.rao@hp.com>
 */

#include "hpi_test.h"

int main(int argc, char **argv)
{
        int number_resources=0;
        SaHpiResetActionT resetOption;
        SaErrorT rv;
        SaHpiSessionIdT sessionid;
        SaHpiResourceIdT resourceid;
        SaHpiResourceIdT resourceid_list[RESOURCE_CAP_LENGTH] = {0};
        SaHpiResetActionT ResetAction;
        SaHpiCapabilitiesT capability = SAHPI_CAPABILITY_RESET;
        int choice=0;

        printf("saHpiResourceResetStateSet: Test for hpi reset state "
               "set function\n");

        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
        if (rv != SA_OK) {
                printf("saHpiSessionOpen failed with error: %s\n",
                       oh_lookup_error(rv));
                return rv;
        }

        /* Discover the resources with IDR capability */
        printf("\nListing the resource with reset capability \n");
        rv = discover_resources(sessionid, capability, resourceid_list,
                                &number_resources);
        if (rv != SA_OK) {
                exit(-1);
        }

        printf("\nPlease enter the resource id: ");
        scanf("%d", &resourceid);

        printf("Please enter the reset type \n");
        printf("Press 1 for COLD RESET\n");
        printf("Press 2 for WARM RESET\n");
        printf("Press 3 for RESET ASSERT\n");
        printf("Press 4 for RESET DEASSERT\n");
        printf("Please enter your option:");
        scanf("%d",&choice);
        resetOption=(SaHpiResetActionT)choice;
        switch (resetOption) {
                case 1:
                        ResetAction=SAHPI_COLD_RESET;
                        break;

                case 2:
                        ResetAction=SAHPI_WARM_RESET;
                        break;

                case 3:
                        ResetAction=SAHPI_RESET_ASSERT;
                        break;

                case 4:
                        ResetAction=SAHPI_RESET_DEASSERT;
                        break;

                default :
                        printf("Wrong option. Please enter the correct "
                               "reset action\n");
                        exit (-1);
        }

        rv = saHpiResourceResetStateSet(sessionid, resourceid, ResetAction);
        if (rv != SA_OK) {
                printf("saHpiResourceResetStateSet failed with error: %s\n",
                       oh_lookup_error(rv));
                printf("Test case - FAIL\n");
        }
        else {
                printf("Test case - PASS\n");
        }
        rv = saHpiSessionClose(sessionid);
        return 0;
}
