/**
 * TODO: License
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

