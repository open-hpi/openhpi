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
 *      W. David Ashley <dashley@us.ibm.com>
 *
 */

#include <SaHpi.h>

#include <openhpi.h>
#include <snmp_util.h>
#include <rsa_resources.h>
#include <snmp_rsa_utils.h>
#include <snmp_rsa.h>
#include <snmp_rsa_discover.h>
#include <snmp_rsa_inventory.h>

/**
 * find_inventories:
 * @rdr_array: 
 * @e: 
 * @tmpqueue: 
 * @tmpcache
 *
 * Return value: 
 **/

/*
SaErrorT find_inventories(	struct snmp_rsa_hnd *custom_handle,
				struct snmp_rsa_inventory *rdr_array,
				struct oh_event * e,
				GSList *tmpqueue, RPTable *tmpcache) 
{
        int j;

        for (j=0; rdr_array[j].rsa_inventory_info.mib.oid.OidManufacturer != NULL; j++) { 
                e = snmp_rsa_discover_inventories(custom_handle->ss, 
                                                 e->u.res_event.entry.ResourceEntity -- parent_ep, 
                                                 &rdr_array[j]); 
                if(e != NULL) { 
                        struct RSA_InventoryInfo *rsa_data = g_memdup(&(rdr_array[j].rsa_inventory_info), 
                                                                    sizeof(struct RSA_InventoryInfo)); 
                        oh_add_rdr(tmpcache, e->u.res_event.entry.ResourceId -- rid,
					&(e->u.rdr_event.rdr), rsa_data, 0); 
                        tmpqueue = g_slist_append(tmpqueue, e); 
                } 
        } 
	
	return SA_OK;
}
*/ 

/**
 * get_inventory_data:
 * @hnd: 
 * @event: 
 * @timeout: 
 *
 * Return value: 
 **/ 
SaErrorT get_inventory_data(	void *hnd, SaHpiRdrT *rdr, 
				struct RSA_InventoryInfo *s,
		 		SaHpiInventoryDataT *l_data, 
				SaHpiInventGeneralDataT *working,
				SaHpiUint32T  *vpdrecordlength) 
{

	int rc;
	SaHpiUint8T  str_len;
        gchar *oid = NULL;
	uint         *base_ptr;
	char conv_string[20];
	size_t       base_len; 
        size_t       total_len;
        struct snmp_value get_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_rsa_hnd *custom_handle = (struct snmp_rsa_hnd *)handle->data;

	base_len  = sizeof(SaHpiTextTypeT) +sizeof(SaHpiLanguageT) +sizeof(SaHpiUint8T);
	base_ptr = (uint *)l_data + sizeof(SaHpiInventoryDataT) + 
				sizeof(l_data->DataRecords[0]) + sizeof(SaHpiInventDataRecordT);

       	str_len = 0; 
       	total_len = str_len; 
       	if(s->mib.oid.OidMfgDateTime != NULL) { 
               	oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidMfgDateTime); 
               	if(oid == NULL) { 
               		working->MfgDateTime =  (SaHpiTimeT) SAHPI_TIME_UNSPECIFIED; 
                       	dbg("NULL SNMP OID returned for MfgDateTime\n"); 
               	} else { 
                       	rc = snmp_get(custom_handle->ss, oid, &get_value); 
                       	if( (rc != 0) | 
                         		!((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                               	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                       	} else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                               	working->MfgDateTime = 
                                       	g_strtod((char *)get_value.string, NULL); 
                       	} else { 
                               	dbg("%s Invalid type for Manufacturer inventory data\n",oid); 
                       	} 
               	} 
       	} else { 
              		working->MfgDateTime =  (SaHpiTimeT) SAHPI_TIME_UNSPECIFIED; 
       	} 

	/*
	 *
	 */
       	total_len += str_len; 
       	str_len = 0; 
       	working->Manufacturer = (SaHpiTextBufferT *) base_ptr; 
       	working->Manufacturer->DataLength = str_len; 
       	working->Manufacturer->DataType = SAHPI_TL_TYPE_LANGUAGE; 
       	working->Manufacturer->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidManufacturer != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidManufacturer); 
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for Manufacturer\n"); 
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                                str_len = strlen((char *) get_value.string); 
                                working->Manufacturer->DataLength = str_len; 
                                strcpy((char *)working->Manufacturer->Data, (char *)get_value.string); 
                        } else { 
                                dbg("%s Invalid type for Manufacturer intory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        base_ptr = base_ptr + base_len + str_len; 
        working->ProductName = (SaHpiTextBufferT *)base_ptr; 
        total_len += str_len + base_len; 
        str_len = 0; 
	working->ProductName->DataLength = str_len; 
	working->ProductName->DataType = SAHPI_TL_TYPE_LANGUAGE; 
	working->ProductName->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidProductName != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidProductName); 
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for ProductName\n"); 
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                                str_len = strlen((char *) get_value.string); 
                                working->ProductName->DataLength = str_len; 
                                strcpy((char *)working->ProductName->Data, (char *)get_value.string); 
                        } else { 
                                dbg("%s Invalid type for ProductName inventory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        base_ptr = base_ptr + base_len + str_len; 
        working->ProductVersion = (SaHpiTextBufferT *)base_ptr; 
        total_len += str_len + base_len; 
        str_len = 0; 
	working->ProductVersion->DataLength = str_len; 
	working->ProductVersion->DataType = SAHPI_TL_TYPE_LANGUAGE; 
	working->ProductVersion->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidProductVersion != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidProductVersion); 
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for ProductVersion \n"); 
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_INTEGER )) { 
				int i = 0; 
				do { 
					conv_string[i++] = get_value.integer % 10 + '0'; 
				} while ((get_value.integer /= 10) > 0); 
				conv_string[i] = '\0'; 
                                str_len = sizeof(conv_string); 
                                working->ProductVersion->DataLength = str_len; 
                                strcpy((char *)working->ProductVersion->Data, (char *)(conv_string)); 
                        } else { 
                                dbg("%s Invalid type for ProductVersion inventory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        base_ptr = base_ptr + base_len + str_len; 
        working->ModelNumber = (SaHpiTextBufferT *) base_ptr; 
        total_len += str_len + base_len; 
        str_len = 0; 
        working->ModelNumber->DataLength = str_len; 
	working->ModelNumber->DataType = SAHPI_TL_TYPE_LANGUAGE; 
	working->ModelNumber->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidModelNumber != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidModelNumber); 
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for ModelNumber \n"); 
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                                str_len = strlen((char *) get_value.string); 
                                working->ModelNumber->DataLength = str_len; 
                                strcpy((char *)working->ModelNumber->Data, (char *)get_value.string); 
                        } else { 
                                dbg("%s Invalid type for ModelNumber inventory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        base_ptr = base_ptr + base_len + str_len; 
        working->SerialNumber = (SaHpiTextBufferT *)base_ptr; 
        total_len += str_len + base_len; 
        str_len = 0; 
	working->SerialNumber->DataLength = str_len; 
	working->SerialNumber->DataType = SAHPI_TL_TYPE_LANGUAGE; 
	working->SerialNumber->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidSerialNumber != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidSerialNumber); 
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for OidSerialNumber \n"); 
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                                str_len = strlen((char *) get_value.string); 
                                working->SerialNumber->DataLength = str_len; 
                                strcpy((char *)working->SerialNumber->Data, (char *)get_value.string);
                        } else { 
                                dbg("%s Invalid type for SerialNumber inventory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        base_ptr = base_ptr + base_len + str_len; 
        working->PartNumber = (SaHpiTextBufferT *)base_ptr; 
        total_len += str_len + base_len; 
        str_len = 0; 
	working->PartNumber->DataLength = str_len; 
	working->PartNumber->DataType = SAHPI_TL_TYPE_LANGUAGE; 
	working->PartNumber->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidPartNumber != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidPartNumber);
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for OidPartNumber \n"); 
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                                str_len = strlen((char *) get_value.string); 
                                working->PartNumber->DataLength = str_len; 
                                strcpy((char *)working->PartNumber->Data, (char *)get_value.string); 
                        } else { 
                                dbg("%s Invalid type for PartNumber inventory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        base_ptr = base_ptr + base_len + str_len; 
        working->FileId = (SaHpiTextBufferT *)base_ptr;
        total_len += str_len + base_len; 
        str_len = 0; 
	working->FileId->DataLength = str_len; 
	working->FileId->DataType = SAHPI_TL_TYPE_LANGUAGE;
	working->FileId->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidFileId != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidFileId);
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for OidFileId \n");
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                                str_len = strlen((char *) get_value.string); 
                                working->FileId->DataLength = str_len; 
                                strcpy((char *)working->FileId->Data, (char *)get_value.string); 
                        } else { 
                                dbg("%s Invalid type for FileId inventory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        base_ptr = base_ptr + base_len + str_len; 
        working->AssetTag = (SaHpiTextBufferT *)base_ptr;
        total_len += str_len + base_len; 
        str_len = 0; 
        working->AssetTag->DataLength = str_len; 
        working->AssetTag->DataType = SAHPI_TL_TYPE_LANGUAGE; 
        working->AssetTag->Language = SAHPI_LANG_ENGLISH; 
        if(s->mib.oid.OidAssetTag != NULL) { 
                oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidAssetTag); 
                if(oid == NULL) { 
                        dbg("NULL SNMP OID returned for OidAssetTag \n"); 
                } else { 
                        rc = snmp_get(custom_handle->ss, oid, &get_value); 
                        if( (rc != 0) | 
                          !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { 
                                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); 
                        } else if((rc == 0) && (get_value.type == ASN_OCTET_STR )) { 
                                str_len = strlen((char *) get_value.string); 
                                working->AssetTag->DataLength = str_len; 
                                strcpy((char *)working->AssetTag->Data, (char *)get_value.string); 
                        } else { 
                                dbg("%s Invalid type for AssetTag inventory data\n",oid); 
                        } 
                } 
        } 

	/*
	 *
	 */
        total_len += str_len + base_len; 
        str_len = 0; 
	working->CustomField[0] = NULL; 
	*vpdrecordlength = total_len;

	return SA_OK;

}

