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
********** Events, Part 2 					      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_EVENTS2_H
#define HPI_EVENTS2_H

/* Event Data Structures */
/*
** Sensor Optional Data
**
** Sensor events may contain optional data items passed and stored with the
** event. If these optional data items are present, they will be included with
** the event data returned in response to a saHpiEventGet() or
** saHpiEventLogEntryGet() function call. Also, the optional data items may be
** included with the event data passed to the saHpiEventLogEntryAdd() function.
**
** Specific implementations of HPI may have restrictions on how much data may
** be passed to saHpiEventLogEntryAdd(). These restrictions should be documented
** by the provider of the HPI interface.
*/
typedef enum {
	SAHPI_CRITICAL = 0,
	SAHPI_MAJOR,
	SAHPI_MINOR,
	SAHPI_INFORMATIONAL,
	SAHPI_OK,
	SAHPI_DEBUG = 0xF0
} SaHpiSeverityT;

typedef SaHpiUint8T SaHpiSensorOptionalDataT;

#define SAHPI_SOD_TRIGGER_READING (SaHpiSensorOptionalDataT)0x01
#define SAHPI_SOD_TRIGGER_THRESHOLD (SaHpiSensorOptionalDataT)0x02
#define SAHPI_SOD_OEM (SaHpiSensorOptionalDataT)0x04
#define SAHPI_SOD_PREVIOUS_STATE (SaHpiSensorOptionalDataT)0x08
#define SAHPI_SOD_SENSOR_SPECIFIC (SaHpiSensorOptionalDataT)0x10

typedef struct {
	SaHpiSensorNumT SensorNum;
	SaHpiSensorTypeT SensorType;
	SaHpiEventCategoryT EventCategory;
	SaHpiBoolT Assertion; /* TRUE = Event State asserted
			       * FALSE = deasserted */
	SaHpiEventStateT EventState; /* State being asserted
				      * deasserted */
	SaHpiSensorOptionalDataT OptionalDataPresent;
	SaHpiSensorReadingT TriggerReading; /* Reading that triggered
					     * the event */
	SaHpiSensorReadingT TriggerThreshold;
	SaHpiEventStateT PreviousState;
	SaHpiUint32T Oem;
	SaHpiUint32T SensorSpecific;
} SaHpiSensorEventT;

typedef struct {
	SaHpiHsStateT HotSwapState;
	SaHpiHsStateT PreviousHotSwapState;
} SaHpiHotSwapEventT;

typedef struct {
	SaHpiWatchdogNumT WatchdogNum;
	SaHpiWatchdogActionEventT WatchdogAction;
	SaHpiWatchdogPretimerInterruptT WatchdogPreTimerAction;
	SaHpiWatchdogTimerUseT WatchdogUse;
} SaHpiWatchdogEventT;

#define SAHPI_OEM_EVENT_DATA_SIZE 32

typedef struct {
	SaHpiManufacturerIdT MId;
	SaHpiUint8T OemEventData[SAHPI_OEM_EVENT_DATA_SIZE];
} SaHpiOemEventT;

/*
** User events may be used for storing custom events created by the application / middleware;
** eg. when injecting events into the event log using saHpiEventLogEntryAdd().
*/
#define SAHPI_USER_EVENT_DATA_SIZE 32
typedef struct {
	SaHpiUint8T UserEventData[SAHPI_USER_EVENT_DATA_SIZE];
} SaHpiUserEventT;

typedef enum {
	SAHPI_ET_SENSOR,
	SAHPI_ET_HOTSWAP,
	SAHPI_ET_WATCHDOG,
	SAHPI_ET_OEM,
	SAHPI_ET_USER
} SaHpiEventTypeT;

typedef union {
	SaHpiSensorEventT SensorEvent;
	SaHpiHotSwapEventT HotSwapEvent;
	SaHpiWatchdogEventT WatchdogEvent;
	SaHpiOemEventT OemEvent;
	SaHpiUserEventT UserEvent;
} SaHpiEventUnionT;

typedef struct {
	SaHpiResourceIdT Source;
	SaHpiEventTypeT EventType;
	SaHpiTimeT Timestamp;
	SaHpiSeverityT Severity;
	SaHpiEventUnionT EventDataUnion;
} SaHpiEventT;

#endif /* HPI_EVENTS2_H */

