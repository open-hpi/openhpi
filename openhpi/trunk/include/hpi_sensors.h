/**
 * TODO: License
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Sensors 						      **********
**********							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_SENSORS_H
#define HPI_SENSORS_H

/* Sensor Number */
typedef SaHpiUint8T SaHpiSensorNumT;

/* Type of Sensor */
typedef enum {
	SAHPI_TEMPERATURE = 0x01,
	SAHPI_VOLTAGE,
	SAHPI_CURRENT,
	SAHPI_FAN,
	SAHPI_PHYSICAL_SECURITY,
	SAHPI_PLATFORM_VIOLATION,
	SAHPI_PROCESSOR,
	SAHPI_POWER_SUPPLY,
	SAHPI_POWER_UNIT,
	SAHPI_COOLING_DEVICE,
	SAHPI_OTHER_UNITS_BASED_SENSOR,
	SAHPI_MEMORY,
	SAHPI_DRIVE_SLOT,
	SAHPI_POST_MEMORY_RESIZE,
	SAHPI_SYSTEM_FW_PROGRESS,
	SAHPI_EVENT_LOGGING_DISABLED,
	SAHPI_RESERVED1,
	SAHPI_SYSTEM_EVENT,
	SAHPI_CRITICAL_INTERRUPT,
	SAHPI_BUTTON,
	SAHPI_MODULE_BOARD,
	SAHPI_MICROCONTROLLER_COPROCESSOR,
	SAHPI_ADDIN_CARD,
	SAHPI_CHASSIS,
	SAHPI_CHIP_SET,
	SAHPI_OTHER_FRU,
	SAHPI_CABLE_INTERCONNECT,
	SAHPI_TERMINATOR,
	SAHPI_SYSTEM_BOOT_INITIATED,
	SAHPI_BOOT_ERROR,
	SAHPI_OS_BOOT,
	SAHPI_OS_CRITICAL_STOP,
	SAHPI_SLOT_CONNECTOR,
	SAHPI_SYSTEM_ACPI_POWER_STATE,
	SAHPI_RESERVED2,
	SAHPI_PLATFORM_ALERT,
	SAHPI_ENTITY_PRESENCE,
	SAHPI_MONITOR_ASIC_IC,
	SAHPI_LAN,
	SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH,
	SAHPI_BATTERY,
	SAHPI_OPERATIONAL = 0xA0,
	SAHPI_OEM_SENSOR=0xC0
} SaHpiSensorTypeT;

/*
** Interpreted Sensor Reading Type
**
** These definitions list the available data types that can be
** used for interpreted sensor readings. Interpreted sensor readings are provided
** because typically sensors measure their associated entities in a way that is
** not human readable/understandable. For example a fan sensor may measure the
** number of ticks that it takes a fan blade to move passed a sensor. The human
** readable reading type would be revolutions per minute (RPM).
*/

#define SAHPI_SENSOR_BUFFER_LENGTH 32

typedef enum {
	SAHPI_SENSOR_INTERPRETED_TYPE_UINT8,
	SAHPI_SENSOR_INTERPRETED_TYPE_UINT16,
	SAHPI_SENSOR_INTERPRETED_TYPE_UINT32,
	SAHPI_SENSOR_INTERPRETED_TYPE_INT8,
	SAHPI_SENSOR_INTERPRETED_TYPE_INT16,
	SAHPI_SENSOR_INTERPRETED_TYPE_INT32,
	SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
	SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER /* 32 byte array */
} SaHpiSensorInterpretedTypeT;

typedef union {
	SaHpiUint8T SensorUint8;
	SaHpiUint16T SensorUint16;
	SaHpiUint32T SensorUint32;
	SaHpiInt8T SensorInt8;
	SaHpiInt16T SensorInt16;
	SaHpiInt32T SensorInt32;
	SaHpiFloat32T SensorFloat32;
	SaHpiUint8T SensorBuffer[SAHPI_SENSOR_BUFFER_LENGTH];
} SaHpiSensorInterpretedUnionT;

typedef struct {
	SaHpiSensorInterpretedTypeT Type;
	SaHpiSensorInterpretedUnionT Value;
} SaHpiSensorInterpretedT;

/*
** Sensor Status
**
** The sensor status structure is used to determine if sensor scanning is
** enabled and if events are enabled. If events are enabled, the structure will
** have valid data for the outstanding sensor event states.
*/
typedef SaHpiUint8T SaHpiSensorStatusT;

#define SAHPI_SENSTAT_EVENTS_ENABLED (SaHpiSensorStatusT)0x80
#define SAHPI_SENSTAT_SCAN_ENABLED (SaHpiSensorStatusT)0x40
#define SAHPI_SENSTAT_BUSY (SaHpiSensorStatusT)0x20

typedef struct {
	SaHpiSensorStatusT SensorStatus;
	SaHpiEventStateT EventStatus;
} SaHpiSensorEvtStatusT;

/* Sensor Event Enables */
typedef struct {
	SaHpiSensorStatusT SensorStatus;
	SaHpiEventStateT AssertEvents;
	SaHpiEventStateT DeassertEvents;
} SaHpiSensorEvtEnablesT;

/*
** Sensor Reading
**
** The sensor reading type is the data structure returned from a call to get
** sensor reading. The structure is also used when setting and getting sensor
** threshold values and reporting sensor ranges.
** Each sensor may support one or more of raw, interpreted, or event status
** representations of the sensor data. For analog sensors the raw value is the
** raw value from the sensor (such as ticks per fan blade) and the interpreted
** value is the raw value converted in to a usable format (such as RPM). The
** interpreted value can be calculated by the HPI implementation using the
** sensor factors or by another OEM means.
*/
typedef SaHpiUint8T SaHpiSensorReadingFormatsT;
#define SAHPI_SRF_RAW (SaHpiSensorReadingFormatsT)0x01
#define SAHPI_SRF_INTERPRETED (SaHpiSensorReadingFormatsT)0x02
#define SAHPI_SRF_EVENT_STATE (SaHpiSensorReadingFormatsT)0x04

typedef struct {
	SaHpiSensorReadingFormatsT ValuesPresent;
	SaHpiUint32T Raw;
	SaHpiSensorInterpretedT Interpreted;
	SaHpiSensorEvtStatusT EventStatus;
} SaHpiSensorReadingT;

/*
** Threshold Values
** This structure encompasses all of the thresholds that can be set.
*/
typedef struct {
	SaHpiSensorReadingT LowCritical; /* Lower Critical Threshold */
	SaHpiSensorReadingT LowMajor; /* Lower Major Threshold */
	SaHpiSensorReadingT LowMinor; /* Lower Minor Threshold */
	SaHpiSensorReadingT UpCritical; /* Upper critical Threshold */
	SaHpiSensorReadingT UpMajor; /* Upper major Threshold */
	SaHpiSensorReadingT UpMinor; /* Upper minor Threshold */
	SaHpiSensorReadingT PosThdHysteresis; /* Positive Threshold Hysteresis */
	SaHpiSensorReadingT NegThdHysteresis; /* Negative Threshold Hysteresis */
}SaHpiSensorThresholdsT;


#endif /* HPI_SENSORS_H */

