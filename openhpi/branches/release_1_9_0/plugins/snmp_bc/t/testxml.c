/*      -*- linux-c -*-
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
 */

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SaHpi.h>

#include <openhpi.h>
#include <com_xml2event.h>


int main(int argc, char *argv[]) {
        int rc;
        guint hash_size;

        rc = xml2event_hash_init(&bc_xml2event_hash, bc_eventxml);
        printf("\nxml2event_hash_init (BC) returned %d\n", rc);
        if (rc) {
                return 1;
        }

        hash_size = g_hash_table_size(bc_xml2event_hash);
        printf("\nThe hash table contains %d elements\n", hash_size);

        xml2event_hash_free(&bc_xml2event_hash);

        rc = xml2event_hash_init(&rsa_xml2event_hash, rsa_eventxml);
        printf("\nxml2event_hash_init (RSA) returned %d\n", rc);
        if (rc) {
                return 1;
        }

        hash_size = g_hash_table_size(rsa_xml2event_hash);
        printf("\nThe hash table contains %d elements\n", hash_size);

        xml2event_hash_free(&rsa_xml2event_hash);
        return 0;
}

