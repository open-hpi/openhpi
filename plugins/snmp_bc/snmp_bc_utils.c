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
 *      Chris Chia <cchia@users.sf.net>
 */

#include <uuid/uuid.h>
#include <snmp_bc_plugin.h>
#include <snmp_bc_utils.h>

SaErrorT snmp_bc_get_guid(struct snmp_bc_hnd *custom_handle,
                          char *oid,
                          SaHpiGuidT *guid)
{
        SaErrorT status;
        struct  snmp_value get_value;
        gchar  *UUID = NULL, *BC_UUID = NULL;
        gchar **tmpstr = NULL;
        const   gchar  *BCUUID_delimiter1 = " ";
        const   gchar  *BCUUID_delimiter2 = "-";
        const   gchar  *UUID_delimiter    = "-";
        const   gchar  *NA   = "NOT AVAILABLE";    // not case sensitive
        guint   UUID_cnt = 0, i = 0;
        uuid_t  UUID_val;

        if ( (custom_handle == NULL) || (oid == NULL) || (guid == NULL)) {
                dbg("Error: found NULL pointer  handle %p  oid %p  guid %p\n",
                    custom_handle,oid,guid);
                status = SA_ERR_HPI_INVALID_PARAMS;
                goto CLEANUP;
        }
        status = snmp_bc_snmp_get(custom_handle, oid, &get_value);
        if(( status != SA_OK) || (get_value.type != ASN_OCTET_STR)) {
                dbg("Error: snmp_get failed rc=%x oid=%s type=%d.\n", 
                        status, oid, get_value.type);
                if ( status != SA_ERR_HPI_BUSY)  status = SA_ERR_HPI_NO_RESPONSE;
                goto CLEANUP;
        }

        dbg("BC_UUID string %s\n", get_value.string);
        // rid lead+trail blanks
        BC_UUID = g_strstrip(g_strdup(get_value.string));
        if (BC_UUID == NULL || BC_UUID[0] == '\0') {
                dbg("Error: BC_UUID string is a NULL string\n");
                status = SA_ERR_HPI_ERROR;
                goto CLEANUP;
        }
        if (g_ascii_strcasecmp( BC_UUID, NA ) == 0) {
                dbg("UUID is N/A %s, set GUID to zeros\n", BC_UUID);
                for ( i=0; i<16; i++ ) UUID_val[i] = 0;
                memmove ( guid, &UUID_val, sizeof(uuid_t));
                status = SA_OK;
                goto CLEANUP;
        }
        // separate substrings
        tmpstr = g_strsplit(BC_UUID, BCUUID_delimiter1, -1);
        for ( UUID_cnt=0; tmpstr[UUID_cnt] != NULL; UUID_cnt++ );
        dbg("number of BC_UUID substrings = %d, strings =", UUID_cnt);
        for (i=0; i<UUID_cnt; i++) dbg(" %s", tmpstr[i]); dbg("\n");
        if ( UUID_cnt == 0 ) {
                dbg("Error: zero length UUID string\n");
                status = SA_ERR_HPI_ERROR;
                goto CLEANUP;
        }
        if ( UUID_cnt == 1 ) { // check with second possible substring delimiter
                tmpstr = g_strsplit(BC_UUID, BCUUID_delimiter2, -1);
                for ( UUID_cnt=0; ; UUID_cnt++ ) {
                        if ( tmpstr[UUID_cnt] == NULL ) break;
                }
                dbg("number of BC_UUID substrings = %d, strings =", UUID_cnt);
                for (i=0; i<UUID_cnt; i++) dbg(" %s", tmpstr[i]); dbg("\n");
                if ( UUID_cnt == 0 ) {
                        dbg("Error: zero length UUID string\n");
                        status = SA_ERR_HPI_ERROR;
                        goto CLEANUP;
                }
        }
        if ( UUID_cnt == UUID_SUBSTRINGS_CNT1 ) {
                // BladeCenter UUID has 8 four character strings 4-4-4-4-4-4-4-4
                // convert to insudtry standard UUID 8-4-4-4-12 string
                UUID = g_strconcat( tmpstr[0], tmpstr[1], UUID_delimiter,   
                                    tmpstr[2], UUID_delimiter,             
                                    tmpstr[3], UUID_delimiter,            
                                    tmpstr[4], UUID_delimiter,           
                                    tmpstr[5], tmpstr[6], tmpstr[7], NULL );
                if (UUID == NULL) {
                        dbg("Error: bad UUID string");
                        status = SA_ERR_HPI_ERROR;
                        goto CLEANUP;
                }
                dbg("UUID string %s\n", UUID);
                // convert UUID string to numberic UUID value
                if ( (status = uuid_parse(UUID, UUID_val)) ) {
                        dbg("Error: failed parsing UUID string %d\n", status);
                        status = SA_ERR_HPI_ERROR;
                        goto CLEANUP;
                }       
                dbg("GUID value  ");
                for (i=0; i<16; i++) { dbg("%02x", UUID_val[i]);} dbg("\n");
                memmove ( guid, &UUID_val, sizeof(uuid_t));
                status = SA_OK;
        }
        else if ( UUID_cnt == UUID_SUBSTRINGS_CNT2 ) {
                // Got a 5 substring case, just put in the delimiter
                UUID = g_strconcat( tmpstr[0], UUID_delimiter,   
                                    tmpstr[1], UUID_delimiter,             
                                    tmpstr[2], UUID_delimiter,            
                                    tmpstr[3], UUID_delimiter,           
                                    tmpstr[4], NULL );
                if (UUID == NULL) {
                        dbg("Error: bad UUID string");
                        status = SA_ERR_HPI_ERROR;
                        goto CLEANUP;
                }
                dbg("UUID string %s\n", UUID);
                // convert UUID string to numberic UUID value
                if ( (status = uuid_parse(UUID, UUID_val)) ) {
                        dbg("Error: failed parsing UUID string %d\n", status);
                        status = SA_ERR_HPI_ERROR;
                        goto CLEANUP;
                }       
                dbg("GUID value  ");
                for (i=0; i<16; i++) { dbg("%02x", UUID_val[i]);} dbg("\n");
                memmove ( guid, &UUID_val, sizeof(uuid_t));
                status = SA_OK;
        }
        else {  // non standard case unsupported
                dbg("Error: non standard UUID string\n");
                status = SA_ERR_HPI_ERROR;
        }

  CLEANUP:
        g_free(UUID);
        g_free(BC_UUID);
        g_strfreev(tmpstr);
                                                                                             
        dbg("get_guid exit status %x\n",status);
        return(status);
} /* End of snmp_bc_get_guid */

