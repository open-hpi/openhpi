/**
 * TODO: License
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

