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

extern int powerSubsystemResId;

int main(int argc, char **argv)
{
        int number_resources = 0;
        SaErrorT rv;
        SaHpiSessionIdT sessionid;
        SaHpiResourceIdT resourceid;
        SaHpiResourceIdT resourceid_list[RESOURCE_CAP_LENGTH] = {0};
        SaHpiCtrlNumT controlNumber = OA_SOAP_RES_CNTRL_NUM;
        SaHpiCtrlModeT controlMode;
        SaHpiCtrlStateT controlState;
        SaHpiCapabilitiesT capability = SAHPI_CAPABILITY_CONTROL;
        char *mode = NULL;

        printf("saHpiControlGet: Test for hpi control get function\n");
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
        if (rv != SA_OK) {
                printf("saHpiSessionOpen failed with error: %s\n",
                       oh_lookup_error(rv));
                return rv;
        }

        /* Discover the resources with control capability */
        printf("\nListing the resource with control capability\n");
        rv = discover_resources(sessionid, capability, resourceid_list,
                                &number_resources);
        if (rv != SA_OK) {
                exit(-1);
        }

        if (powerSubsystemResId < 0) {
          printf("Error: could not find the C7000 Power Subsystem resource. \
Exiting test...\n");
          exit(-1);
        }

        printf("\nPlease enter the resource id: ");
        scanf("%d", &resourceid);

	printf("\nSupported controls on the resource are:");
        if (resourceid == powerSubsystemResId) {
	  printf("\nPower mode control(Press 3)");
	  printf("\nDynamic power mode control(Press 4)");
	  printf("\nPower limit mode control(Press 5)");
	  printf("\nStatic power limit control(Press 6)");
	  printf("\nDynamic power cap control(Press 7)");
	  printf("\nDerated circuit cap control(Press 8)");
	  printf("\nRated circuit cap control(Press 9)");
        }
        else {
	  printf("\nUID LED control(Press 0)");
	  printf("\nPower control(Press 1)");
        }
	printf("\nEnter your option:");

	scanf("%d", &controlNumber);

        rv = saHpiControlGet(sessionid, resourceid, controlNumber,
                             &controlMode, &controlState);
        if (rv != SA_OK) {
                printf("saHpiControlGet failed with error: %s\n",
                       oh_lookup_error(rv));
                printf("Test case - FAIL\n");
        }
        else {
                mode = oh_lookup_ctrlmode(controlMode);
                if (mode == NULL) {
                        printf("Unknown control mode");
                        printf("Test case - Fail\n");
                }
                else {
                        printf("Control mode = %s\n",mode);
                        rv = oh_print_ctrlstate(&controlState,6);
                        if (rv != SA_OK) {
                                printf("Test case - FAIL\n");
                        }
                }
                printf("Test case - PASS\n");
        }

        rv = saHpiSessionClose(sessionid);
        return 0;
}
