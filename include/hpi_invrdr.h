/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: License
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Inventory Resource Data Records 			      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_INVRDR_H
#define HPI_INVRDR_H

/*
** All inventory data contained in an entity inventory repository
** must be represented in the RDR repository
** with an SaHpiInventoryRecT.
*/
typedef struct {
	SaHpiEirIdT EirId;
	SaHpiUint32T Oem;
} SaHpiInventoryRecT;

#endif /* HPI_INVRDR_H */


