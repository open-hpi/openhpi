/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 * (C) Copyright 2015-2018 Hewlett Packard Enterprise Development LP
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
 * Neither the name of the Hewlett Packard Enterprise, nor the names
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
        SaErrorT rv;
        SaHpiSessionIdT sessionid;
        SaHpiResourceIdT resourceid;
        SaHpiIdrIdT IdrId = SAHPI_DEFAULT_INVENTORY_ID;
        SaHpiIdrFieldT field;
        SaHpiResourceIdT resourceid_list[RESOURCE_CAP_LENGTH] = {0};
        SaHpiCapabilitiesT capability = SAHPI_CAPABILITY_INVENTORY_DATA;

        printf("saHpiIdrFieldAdd: Test for hpi IDR field add function\n");

        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
        if (rv != SA_OK) {
                printf("saHpiSessionOpen failed with error: %s\n",
                       oh_lookup_error(rv));
                return rv;
        }

        /* Discover the resources with IDR capability */
        printf("\nListing the resource with IDR capability \n");
        rv = discover_resources(sessionid, capability, resourceid_list,
                                &number_resources);
        if (rv != SA_OK) {
                exit(-1);
        }

        printf("\nPlease enter the resource id: ");
        scanf("%d", &resourceid);
        printf("\nPlease enter the area id: ");
        scanf("%d", &field.AreaId);

        field.Type = SAHPI_IDR_FIELDTYPE_PART_NUMBER;
        field.FieldId = SAHPI_FIRST_ENTRY;
        field.Field.DataType = SAHPI_TL_TYPE_TEXT;
        field.Field.Language = SAHPI_LANG_ENGLISH;
        field.Field.DataLength = strlen ("Test field") + 1;
        snprintf((char *)field.Field.Data, field.Field.DataLength,
                 "%s", "Test field");

        rv = saHpiIdrFieldAdd(sessionid, resourceid, IdrId, &field);
        if (rv != SA_OK) {
                printf("saHpiIdrFieldAdd failed with error: %s\n",
                       oh_lookup_error(rv));
                printf("Test case - FAIL\n");
        }
        else {
                oh_print_idrfield(&field,6);
                printf("Test case - PASS\n");
        }

        rv = saHpiSessionClose(sessionid);
        return 0;
}
