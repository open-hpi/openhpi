/** 
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: license
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Resource Data Record 				      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_RDR_H
#define HPI_RDR_H

/*
** The following describes the different types of records that exist within a
** RDR repository and the RDR super-structure to all of the specific RDR types
** (sensor, inventory data, watchdog, etc.).
*/

typedef enum {
	SAHPI_NO_RECORD,
	SAHPI_CTRL_RDR,
	SAHPI_SENSOR_RDR,
	SAHPI_INVENTORY_RDR,
	SAHPI_WATCHDOG_RDR
} SaHpiRdrTypeT;

typedef union {
	SaHpiCtrlRecT 		CtrlRec;
	SaHpiSensorRecT 	SensorRec;
	SaHpiInventoryRecT 	InventoryRec;
	SaHpiWatchdogRecT 	WatchdogRec;
} SaHpiRdrTypeUnionT;

typedef struct {
	SaHpiEntryIdT 		RecordId;
	SaHpiRdrTypeT 		RdrType;
	SaHpiEntityPathT 	Entity; /* Entity to which this RDR relates. */
	SaHpiRdrTypeUnionT 	RdrTypeUnion;
	SaHpiTextBufferT 	IdString;
} SaHpiRdrT;

#endif /* HPI_RDR_H */


