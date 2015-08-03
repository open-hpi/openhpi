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
#include "../../../include/SaHpiOaSoap.h"

extern int powerSubsystemResId;

int main(int argc, char **argv)
{
        int number_resources = 0, modeOption;
        SaErrorT rv;
        SaHpiSessionIdT sessionid;
        SaHpiResourceIdT resourceid;
        SaHpiResourceIdT resourceid_list[RESOURCE_CAP_LENGTH] = {0};
        SaHpiCtrlNumT controlNumber = OA_SOAP_RES_CNTRL_NUM;
        SaHpiCtrlModeT controlMode;
        SaHpiCtrlStateT controlState;
        SaHpiCapabilitiesT capability = SAHPI_CAPABILITY_CONTROL;
        SaHpiCtrlTypeT control_type = SAHPI_CTRL_TYPE_DIGITAL;
        SaHpiRdrT rdr;
        SaHpiCtrlRecT *ctrl;
        SaHpiCtrlRecAnalogT *analog;

        printf("saHpiControlSet: Test for hpi control set function\n");

        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
        if (rv != SA_OK) {
                printf("saHpiSessionOpen failed with error: %s\n",
                       oh_lookup_error(rv));
                return rv;
        }

        /* Discover the resources with control capability */
        printf("\nListing the resource with control capability\n\n");
        rv = discover_resources(sessionid, capability, resourceid_list,
                                &number_resources);
        if (rv != SA_OK) {
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

        if (resourceid == powerSubsystemResId) {
          switch (controlNumber) {
            case OA_SOAP_DYNAMIC_PWR_CNTRL:
              control_type = SAHPI_CTRL_TYPE_DIGITAL;
              break;
            case OA_SOAP_PWR_MODE_CNTRL:
            case OA_SOAP_PWR_LIMIT_MODE_CNTRL:
              control_type = SAHPI_CTRL_TYPE_DISCRETE;
              break;
            case OA_SOAP_STATIC_PWR_LIMIT_CNTRL:
            case OA_SOAP_DYNAMIC_PWR_CAP_CNTRL:
            case OA_SOAP_DERATED_CIRCUIT_CAP_CNTRL:
            case OA_SOAP_RATED_CIRCUIT_CAP_CNTRL:
              control_type = SAHPI_CTRL_TYPE_ANALOG;
              break; 
          }
        }

        printf("Possible control mode options are: \n");
        printf("\tPress 1 for AUTO MODE \n");
        printf("\tPress 2 for MANUAL MODE \n");
        printf("Please enter your option: ");

        scanf("%d", &modeOption);
        if (modeOption == 1)
                controlMode=SAHPI_CTRL_MODE_AUTO;
        else if (modeOption == 2)
                controlMode=SAHPI_CTRL_MODE_MANUAL;
        else {
                printf("Wrong option selected. Exiting");
                exit (-1);
        }

        controlState.Type = control_type;

        switch (control_type) {
          case SAHPI_CTRL_TYPE_DIGITAL:
            printf("Possible control state options are: \n");
            printf("\tPress 1 for ON \n");
            printf("\tPress 2 for OFF \n");
            printf("Please enter your option: ");
            scanf("%d", &modeOption);
            switch (modeOption) {
                    case 1 :
                            controlState.StateUnion.Digital =
                              SAHPI_CTRL_STATE_ON;
                            break;
                    case 2 :
                            controlState.StateUnion.Digital =
                              SAHPI_CTRL_STATE_OFF;
                            break;
                    default :
                            printf("Wrong option selected. Exiting");
                            exit (-1);
            }

            break;
          case SAHPI_CTRL_TYPE_DISCRETE:
            printf("Possible control state options are: \n");
            if (controlNumber == OA_SOAP_PWR_MODE_CNTRL) {
              printf("\tPress 1 for C7000_PWR_NON_REDUNDANT \n");
              printf("\tPress 2 for C7000_PWR_AC_REDUNDANT \n");
              printf("\tPress 3 for C7000_PWR_SUPPLY_REDUNDANT \n");
            } else {
              printf("\tPress 0 for C7000_PWR_LIMIT_NONE \n");
              printf("\tPress 1 for C7000_PWR_LIMIT_STATIC \n");
              printf("\tPress 2 for C7000_PWR_LIMIT_DYNAMIC_CAP \n");
            }
            printf("Please enter your option: ");
            scanf("%d", &modeOption);
            controlState.StateUnion.Discrete = modeOption;
            break;
          case SAHPI_CTRL_TYPE_ANALOG:
            rv = saHpiRdrGetByInstrumentId(sessionid, resourceid,
                                           SAHPI_CTRL_RDR,
                                           controlNumber, &rdr);
            if (rv != SA_OK) {
              printf("Error: cannot get ctrl rdr by instrument id. Exiting");
              exit (-1);
            };
            ctrl = &(rdr.RdrTypeUnion.CtrlRec);
            analog = &(ctrl->TypeUnion.Analog);
            printf("Choose an analog value for the control (range = %d - %d): \
\n", analog->Min, analog->Max);
            scanf("%d", &modeOption);
            controlState.StateUnion.Analog = modeOption;
            break; 
        }

        rv = saHpiControlSet(sessionid, resourceid, controlNumber,
                             controlMode, &controlState);

        /* Print the test case status*/
        if (rv != SA_OK) {
                printf("saHpiControlSet returns %s\n",oh_lookup_error(rv));
                printf("Test case - FAIL\n");
        }
        else
                printf("Test case - PASS\n");

        rv = saHpiSessionClose(sessionid);
        return 0;
}
