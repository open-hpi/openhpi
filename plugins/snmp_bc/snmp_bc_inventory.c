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
 *      peter d phan  <pdphan@sourceforge.net>
 *      Renier Morales <renierm@users.sf.net>
 *
 *	07/19/04 HPI-B Inventory   
 */

#include <SaHpi.h>

#include <openhpi.h>
#include <snmp_util.h>
#include <bc_resources.h>
#include <snmp_bc.h>
#include <snmp_bc_discover.h>
#include <snmp_bc_inventory.h>

/************************************************************************/
/* Inventory functions   						*/
/************************************************************************/

/**
 * snmp_bc_get_idr_info:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_get_idr_info( void *hnd,  
		SaHpiResourceIdT        ResourceId,
		SaHpiIdrIdT             IdrId,
		SaHpiIdrInfoT          *IdrInfo)
{
	SaErrorT  rv = SA_OK;
	void *i_record;

	i_record = (void *)g_malloc0(SIZE_OF_WORKING_SPACE * sizeof(guint));
 	if (!i_record) {
  		dbg("Cannot allocate working buffer memory");
		rv = SA_ERR_HPI_OUT_OF_MEMORY;
	}
	
	if (rv == SA_OK)
		rv = snmp_bc_build_idr(hnd, ResourceId, IdrId, i_record);
		
	if (rv == SA_OK) {
		memcpy(IdrInfo, i_record, sizeof(SaHpiIdrInfoT));
	}

	g_free(i_record);
	return rv;
}

/**
 * snmp_bc_get_idr_area_header:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_get_idr_area_header( void *hnd,
		SaHpiResourceIdT         ResourceId,
		SaHpiIdrIdT              IdrId,
		SaHpiIdrAreaTypeT        AreaType,
		SaHpiEntryIdT            AreaId,
		SaHpiEntryIdT           *NextAreaId,
		SaHpiIdrAreaHeaderT     *Header)
{

	SaErrorT rv = SA_OK;
	void *i_record;
	struct bc_inventory_record * thisInventoryRecordPtr;
	
	i_record = (void *)g_malloc0(SIZE_OF_WORKING_SPACE *  sizeof(guint));
 	if (!i_record) {
  		dbg("Cannot allocate working buffer memory");
		rv = SA_ERR_HPI_OUT_OF_MEMORY;
	}
	
	if (rv == SA_OK)
		rv = snmp_bc_build_idr(hnd, ResourceId, IdrId, i_record);
		
	if (rv == SA_OK) {
		thisInventoryRecordPtr = (struct bc_inventory_record *) i_record;
		if (thisInventoryRecordPtr->areaptr[0]->idrareas.AreaId == AreaId) {
			memcpy(Header, &(thisInventoryRecordPtr->areaptr[0]->idrareas), sizeof(SaHpiIdrAreaHeaderT));
			*NextAreaId = SAHPI_LAST_ENTRY;
		} else {
			rv = SA_ERR_HPI_NOT_PRESENT;
		}
	}

	g_free(i_record);
	return rv;

}


/**
 * snmp_bc_add_idr_area:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_add_idr_area( void *hnd,
		SaHpiResourceIdT         ResourceId,
		SaHpiIdrIdT              IdrId,
		SaHpiIdrAreaTypeT        AreaType,
		SaHpiEntryIdT           *AreaId)

{
	return SA_ERR_HPI_READ_ONLY;
}


/**
 * snmp_bc_del_idr_area:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_del_idr_area( void *hnd,
		SaHpiResourceIdT       ResourceId,
		SaHpiIdrIdT            IdrId,
		SaHpiEntryIdT          AreaId)
{
	return SA_ERR_HPI_READ_ONLY;
}


/**
 * snmp_bc_get_idr_field:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_get_idr_field( void *hnd,
		SaHpiResourceIdT       ResourceId,
		SaHpiIdrIdT             IdrId,
		SaHpiEntryIdT           AreaId,
		SaHpiIdrFieldTypeT      FieldType,
		SaHpiEntryIdT           FieldId,
		SaHpiEntryIdT          *NextFieldId,
		SaHpiIdrFieldT         *Field)
{
	SaErrorT rv = SA_OK;
	void *i_record;
	int i;
	SaHpiBoolT foundit = SAHPI_FALSE;
	struct bc_inventory_record * thisInventoryRecordPtr;
	
	i_record = (void *)g_malloc0(SIZE_OF_WORKING_SPACE *  sizeof(guint));
 	if (!i_record) {
  		dbg("Cannot allocate working buffer memory");
		rv = SA_ERR_HPI_OUT_OF_MEMORY;
	}
	
	if (rv == SA_OK)
		rv = snmp_bc_build_idr(hnd, ResourceId, IdrId, i_record);
		
	if (rv == SA_OK) {
		thisInventoryRecordPtr = (struct bc_inventory_record *) i_record;
		if (thisInventoryRecordPtr->areaptr[0]->idrareas.AreaId == AreaId) {
		
			/* Search for fieldId here */
			for (i=0; i < thisInventoryRecordPtr->areaptr[0]->idrareas.NumFields; i++) {
				if ((thisInventoryRecordPtr->areaptr[0]->fieldptr[i]->FieldId == FieldId) && 
					(thisInventoryRecordPtr->areaptr[0]->fieldptr[i]->Type == FieldType)){
					break;
				}
			}
			
			if (foundit) {
				memcpy(Field, thisInventoryRecordPtr->areaptr[0]->fieldptr[i], sizeof(SaHpiIdrFieldT));
				*NextFieldId = SAHPI_LAST_ENTRY;
			} else {
				rv = SA_ERR_HPI_NOT_PRESENT;
			} 
		} else {
			rv = SA_ERR_HPI_NOT_PRESENT;
		}
	}

	g_free(i_record);
	return rv;
}


/**
 * snmp_bc_add_idr_field:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_add_idr_field( void *hnd,
		SaHpiResourceIdT         ResourceId,
		SaHpiIdrIdT              IdrId,
		SaHpiIdrFieldT        *Field)
{
	return SA_ERR_HPI_READ_ONLY;
}


/**
 * snmp_bc_set_idr_field:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_set_idr_field( void *hnd,
		SaHpiResourceIdT         ResourceId,
		SaHpiIdrIdT              IdrId,
		SaHpiIdrFieldT           *Field)
{
	return SA_ERR_HPI_READ_ONLY;
}


/**
 * snmp_bc_del_idr_field:
 * @hnd:
 * @event:
 * @timeout:
 *
 * Return value:
 **/
SaErrorT snmp_bc_del_idr_field( void *hnd, 
		SaHpiResourceIdT         ResourceId,
		SaHpiIdrIdT              IdrId,
		SaHpiEntryIdT            AreaId,
		SaHpiEntryIdT            FieldId)
{
	return SA_ERR_HPI_READ_ONLY;
}


/**
 * snmp_bc_build_idr:
 * @hnd:
 * @ResourceId:
 * @i_record:
 * 	If inventory data are found, i_record has the following format
 * 	SaHpiIdrInfoT || SaHpiIdrAreaHeaderT || X * SaHpiIdrFieldT 
 *      	ChassisType;
 *       	MfgDateTime;
 *       	Manufacturer;
 *       	ProductName;
 *       	ProductVersion;
 *       	SerialNumber;
 *       	PartNumber;
 *       	FileId;
 *       	AssetTag;
 * 
 *
 * Return value:
 *		SA_OK
 *		SA_ERR_HPI_NOT_PRESENT
 *		SA_ERR_BUSY <--- pdp check
 *		SA_ERR_TIMEOUT <-- pdp check
 **/
SaErrorT snmp_bc_build_idr( void *hnd, 
		SaHpiResourceIdT         ResourceId,
		SaHpiIdrIdT              IdrId,
		void *		         i_record)
{
	SaErrorT rv = SA_OK;
	struct oh_handler_state *handle = (struct oh_handler_state *) hnd;
	struct snmp_bc_hnd *custom_handle = handle->data;
	SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
	
	gchar        *oid = NULL;
	struct snmp_value get_value;
	
	/* Local work spaces */
	struct bc_inventory_record thisInventoryRecord;
	struct bc_idr_area	   thisInventoryArea;
	struct bc_idr_area *       inventoryAreaPtr;
	SaHpiIdrFieldT *           inventoryFieldPtr;

	SaHpiIdrFieldT		thisField;
	
	
	if (rdr != NULL) {
	
		struct BC_InventoryInfo *s =
                        (struct BC_InventoryInfo *)oh_get_rdr_data(handle->rptcache, ResourceId, rdr->RecordId);

		
		thisInventoryRecord.idrinfo.IdrId = IdrId;
		thisInventoryRecord.idrinfo.UpdateCount = 0;
		thisInventoryRecord.idrinfo.ReadOnly = SAHPI_TRUE;
		thisInventoryRecord.idrinfo.NumAreas = 1; /* pdp - expand to 2 for chassis HW & Firmware */
		
		thisInventoryArea.idrareas.AreaId = 1;
		thisInventoryArea.idrareas.Type = s->mib.area_type;
		thisInventoryArea.idrareas.ReadOnly = SAHPI_TRUE;
		thisInventoryArea.idrareas.NumFields = 0; /* Increment it as we find field */
		
		inventoryAreaPtr = (struct bc_idr_area *)i_record + sizeof(struct bc_inventory_record);
		thisInventoryRecord.areaptr[0] = inventoryAreaPtr;
		memcpy((struct bc_inventory_record *)i_record, &thisInventoryRecord, sizeof(struct bc_inventory_record));
		memcpy((struct bc_idr_area *)inventoryAreaPtr, &thisInventoryRecord, sizeof(struct bc_idr_area));
		inventoryFieldPtr = (SaHpiIdrFieldT *) (inventoryAreaPtr + sizeof(struct bc_idr_area));
		
				
		thisField.AreaId = thisInventoryArea.idrareas.AreaId;
		thisField.ReadOnly = SAHPI_TRUE;
		thisField.Field.Language = SAHPI_LANG_ENGLISH; /*  SaHpiLanguageT */

		/**
		 *
		 */ 		
		thisField.FieldId = 1;
		thisField.Type = SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidChassisType != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidChassisType);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for Chassis Type\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for Chassis data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}
		
		/**
		 *
		 */ 		
		thisField.FieldId = 2;
		thisField.Type = SAHPI_IDR_FIELDTYPE_MFG_DATETIME;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidMfgDateTime != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidMfgDateTime);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for MfgDateTime\n");
				thisField.Field.DataLength = sizeof(SaHpiTimeT); /* SaHpiUint8T  */	
				thisField.Field.DataType = SAHPI_TL_TYPE_BINARY; /* SaHpiTextTypeT */
                		/* (SaHpiTimeT) thisField.Field.Data =  (SaHpiTimeT) SAHPI_TIME_UNSPECIFIED; */
                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid type for MfgDateTime inventory data\n",oid);
                        	}
                	}
                	g_free(oid);
        	} else {
			thisField.Field.DataLength = sizeof(SaHpiTimeT); /* SaHpiUint8T  */	
			thisField.Field.DataType = SAHPI_TL_TYPE_BINARY; /* SaHpiTextTypeT */
                	/*(SaHpiTimeT) thisField.Field.Data =  SAHPI_TIME_UNSPECIFIED;*/
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}
		
		/**
		 *
		 */ 		
		thisField.FieldId = 3;
		thisField.Type = SAHPI_IDR_FIELDTYPE_MANUFACTURER;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidManufacturer != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidManufacturer);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for Manufacturer\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for Manufacturer data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}
		
		/**
		 *
		 */ 		
		thisField.FieldId = 4;
		thisField.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_NAME;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidProductName != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidProductName);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for ProductName\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for ProductName data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}
						
		/**
		 *
		 */ 		
		thisField.FieldId = 5;
		thisField.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidProductVersion != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidProductVersion);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for ProductVersion\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for ProductVersion data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}

		/**
		 *
		 */ 		
		thisField.FieldId = 6;
		thisField.Type = SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidSerialNumber != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidSerialNumber);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for SerialNumber\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for SerialNumber data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}

		/**
		 *
		 */ 		
		thisField.FieldId = 7;
		thisField.Type = SAHPI_IDR_FIELDTYPE_PART_NUMBER;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidPartNumber != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidPartNumber);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for PartNumber\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for PartNumber data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}

		/**
		 *
		 */ 		
		thisField.FieldId = 8;
		thisField.Type = SAHPI_IDR_FIELDTYPE_FILE_ID;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidFileId != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidFileId);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for FileId\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for FileId data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
			inventoryAreaPtr->idrareas.NumFields++;
			inventoryFieldPtr+= sizeof(SaHpiIdrFieldT);
		}

		/**
		 *
		 */ 		
		thisField.FieldId = 9;
		thisField.Type = SAHPI_IDR_FIELDTYPE_ASSET_TAG;
		thisField.Field.DataLength = 0; /* SaHpiUint8T  */	
		if(s->mib.oid.OidAssetTag != NULL) {
			oid = snmp_derive_objid(rdr->Entity,s->mib.oid.OidAssetTag);
			if(oid == NULL) {
                        	dbg("NULL SNMP OID returned for AssetTag\n");

                	} else {
                        	rv = snmp_bc_snmp_get(custom_handle, custom_handle->ss, oid, &get_value);
                        	if((rv != SA_OK) |
                           		!((get_value.type == ASN_INTEGER) |
                           				(get_value.type == ASN_OCTET_STR))) {
                                	dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type);
                                	g_free(oid);
                                	return rv;
                        	} else if((rv == SA_OK) && (get_value.type == ASN_OCTET_STR )) {
					thisField.Field.DataLength = get_value.str_len;
					thisField.Field.DataType = SAHPI_TL_TYPE_TEXT;
					memcpy(thisField.Field.Data, get_value.string, get_value.str_len); 
                        	} else {
                                	dbg("%s Invalid data type for AssetTag data\n",oid);
                        	}
                	}
                	g_free(oid);
        	}
		
		if (thisField.Field.DataLength != 0) {
			memcpy(inventoryFieldPtr, &thisField, sizeof(SaHpiIdrFieldT));
			inventoryAreaPtr->fieldptr[inventoryAreaPtr->idrareas.NumFields] = inventoryFieldPtr;
		}

	} else {
		rv = SA_ERR_HPI_NOT_PRESENT;
	}
	return rv;
}


