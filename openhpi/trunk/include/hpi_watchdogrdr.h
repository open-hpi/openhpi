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


