/*
 * Copyright (c) 2003, Service Availability Forum
 * All rights reserved.
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
 * Neither the name of the Service Availalability Forum nor the names 
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
 */


/*******************************************************************************
********************************************************************************
********** 							      **********
********** Entity Inventory Data 				      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_EID_H
#define HPI_EID_H

/*
** These structures are used to read and write inventory data to entity inventory
** repositories within a resource.
*/

/*
** Entity Inventory Repository ID
** Identifier for an entity inventory repository.
*/
typedef SaHpiUint8T SaHpiEirIdT;

#define SAHPI_DEFAULT_INVENTORY_ID (SaHpiEirIdT)0x00

/* Data Validity */
typedef enum {
	SAHPI_INVENT_DATA_VALID,
	SAHPI_INVENT_DATA_INVALID,
	SAHPI_INVENT_DATA_OVERFLOW
} SaHpiInventDataValidityT;

/* Inventory Record definitions */
typedef enum {
	SAHPI_INVENT_RECTYPE_INTERNAL_USE = 0xB0,
	SAHPI_INVENT_RECTYPE_CHASSIS_INFO,
	SAHPI_INVENT_RECTYPE_BOARD_INFO,
	SAHPI_INVENT_RECTYPE_PRODUCT_INFO,
	SAHPI_INVENT_RECTYPE_OEM = 0xC0
} SaHpiInventDataRecordTypeT;

typedef enum {
	SAHPI_INVENT_CTYP_OTHER = 1,
	SAHPI_INVENT_CTYP_UNKNOWN,
	SAHPI_INVENT_CTYP_DESKTOP,
	SAHPI_INVENT_CTYP_LOW_PROFILE_DESKTOP,
	SAHPI_INVENT_CTYP_PIZZA_BOX,
	SAHPI_INVENT_CTYP_MINI_TOWER,
	SAHPI_INVENT_CTYP_TOWER,
	SAHPI_INVENT_CTYP_PORTABLE,
	SAHPI_INVENT_CTYP_LAPTOP,
	SAHPI_INVENT_CTYP_NOTEBOOK,
	SAHPI_INVENT_CTYP_HANDHELD,
	SAHPI_INVENT_CTYP_DOCKING_STATION,
	SAHPI_INVENT_CTYP_ALLINONE,
	SAHPI_INVENT_CTYP_SUBNOTEBOOK,
	SAHPI_INVENT_CTYP_SPACE_SAVING,
	SAHPI_INVENT_CTYP_LUNCH_BOX,
	SAHPI_INVENT_CTYP_MAIN_SERVER,
	SAHPI_INVENT_CTYP_EXPANSION,
	SAHPI_INVENT_CTYP_SUBCHASSIS,
	SAHPI_INVENT_CTYP_BUS_EXPANSION_CHASSIS,
	SAHPI_INVENT_CTYP_PERIPHERAL_CHASSIS,
	SAHPI_INVENT_CTYP_RAID_CHASSIS,
	SAHPI_INVENT_CTYP_RACKMOUNT
} SaHpiInventChassisTypeT;

typedef struct {
	SaHpiUint8T Data[1]; /* Variable length opaque data */
} SaHpiInventInternalUseDataT;

typedef struct {
	SaHpiTimeT MfgDateTime; /* May be set to
				 * SAHPI_TIME_UNSPECIFIED
				 * if manufacturing
				 * date/time not available */
	SaHpiTextBufferT *Manufacturer;
	SaHpiTextBufferT *ProductName;
	SaHpiTextBufferT *ProductVersion;
	SaHpiTextBufferT *ModelNumber;
	SaHpiTextBufferT *SerialNumber;
	SaHpiTextBufferT *PartNumber;
	SaHpiTextBufferT *FileId;
	SaHpiTextBufferT *AssetTag;
	SaHpiTextBufferT *CustomField[1]; /* Variable number of fields,
					   * last is NULL */
} SaHpiInventGeneralDataT;

typedef struct {
	SaHpiInventChassisTypeT Type; /* Type of chassis */
	SaHpiInventGeneralDataT GeneralData;
} SaHpiInventChassisDataT;

typedef struct {
	SaHpiManufacturerIdT MId;/* OEM Manuf. ID */
	SaHpiUint8T Data[1]; /* Variable length data, defined by OEM,
			      * Length derived from DataLength in
			      * SaHpiInventDataRecordT structure:
			      * DataLength - 4(because DataLength
			      * includes the MId) */
} SaHpiInventOemDataT;

typedef union {
	SaHpiInventInternalUseDataT InternalUse;
	SaHpiInventChassisDataT ChassisInfo;
	SaHpiInventGeneralDataT BoardInfo;
	SaHpiInventGeneralDataT ProductInfo;
	SaHpiInventOemDataT OemData;
} SaHpiInventDataUnionT;

typedef struct {
	SaHpiInventDataRecordTypeT RecordType;
	SaHpiUint32T DataLength; /* Length of Data field for
			          * this record */
	SaHpiInventDataUnionT RecordData; /* Variable length data */
} SaHpiInventDataRecordT;

typedef struct {
	SaHpiInventDataValidityT Validity; /* Indication as to whether data
					    * Returned by
					    * saHpiEntityInventoryDataRead() is
					    * complete and valid. Unless this
					    * flag indicates valid data,
					    * saHpiEntityInventoryDataWrite() will
					    * not take any actions except to
					    * return an error.*/
	SaHpiInventDataRecordT *DataRecords[1]; /* Array of pointers to inventory
						 * Data Records. Variable
						 * number of entries. Last
						 * entry is NULL. */
} SaHpiInventoryDataT;

#endif /* HPI_EID_H */

