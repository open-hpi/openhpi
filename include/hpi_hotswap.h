/**
 * TODO: License
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Hot Swap 						      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_HOTSWAP_H
#define HPI_HOTSWAP_H

/* Power State */
typedef enum {
	SAHPI_HS_POWER_OFF = 0,
	SAHPI_HS_POWER_ON,
	SAHPI_HS_POWER_CYCLE
} SaHpiHsPowerStateT;

/* Hot Swap Indicator State */
typedef enum {
	SAHPI_HS_INDICATOR_OFF = 0,
	SAHPI_HS_INDICATOR_ON
} SaHpiHsIndicatorStateT;

/* Hot Swap Action */
typedef enum {
	SAHPI_HS_ACTION_INSERTION = 0,
	SAHPI_HS_ACTION_EXTRACTION
} SaHpiHsActionT;

/* Hot Swap State */
typedef enum {
	SAHPI_HS_STATE_INACTIVE = 0,
	SAHPI_HS_STATE_INSERTION_PENDING,
	SAHPI_HS_STATE_ACTIVE_HEALTHY,
	SAHPI_HS_STATE_ACTIVE_UNHEALTHY,
	SAHPI_HS_STATE_EXTRACTION_PENDING,
	SAHPI_HS_STATE_NOT_PRESENT
} SaHpiHsStateT;

#endif /* HPI_HOTSWAP_H */

