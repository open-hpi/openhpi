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

