/**
 * TODO: license	
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Watchdog Resource Data Records 			      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_WATCHDOGRDR_H
#define HPI_WATCHDOGRDR_H

/*
** When the "Watchdog" capability is set in a resource, a watchdog with an
** identifier of SAHPI_DEFAULT_WATCHDOG_NUM is required. All watchdogs must be
** represented in the RDR repository with an SaHpiWatchdogRecT, including the
** watchdog with an identifier of SAHPI_DEFAULT_WATCHDOG_NUM.
*/
typedef struct {
	SaHpiWatchdogNumT WatchdogNum;
	SaHpiUint32T Oem;
} SaHpiWatchdogRecT;

#endif /* HPI_WATCHDOGRDR_H */


