/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by IBM Corp.   
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 * 	pdphan	03/22/04	Initial code 
 *				Based on hpiEvent() by Andy Cress of Intel
 */

#include <printevent_utils.h>

/**
 *
 *
 **/
char *rdrtypes[5] = {
	"None    ",
	"Control ",
	"Sensor  ",
	"Invent  ",
	"Watchdog"};

#define HPI_NSEC_PER_SEC 1000000000LL

/**
 *
 *
 **/
#define NSU   32
char *units[NSU] = {
	"units", "deg C",     "deg F",     "deg K",     "volts", "amps",
	"watts", "joules",    "coulombs",  "va",        "nits",  "lumen",
	"lux",   "candela",   "kpa",       "psi",     "newton",  "cfm",
	"rpm",   "Hz",        "us",        "ms",      "sec",     "min",
	"hours", "days",      "weeks",     "mil",     "in",     "ft",
	"mm",    "cm"
};
 
char *code_unknown =  "Code Unknown.";
/*
 * Intepretated values for Watchdog Events
 *
 */
#define NUM_WT  7
struct code2string watchdogtime[NUM_WT] = {
	{ SAHPI_WTU_NONE , "None"} ,
	{ SAHPI_WTU_BIOS_FRB2 , "BIOS FRB2" },
	{ SAHPI_WTU_BIOS_POST , "BIOS POST" },
	{ SAHPI_WTU_OS_LOAD , "OS Clock" },
	{ SAHPI_WTU_SMS_OS , "System Management System heartbeat" },    
	{ SAHPI_WTU_OEM, "OEM Clock" },
	{ SAHPI_WTU_UNSPECIFIED , "Not Specified" }
};

/**
 *
 *
 **/
#define NUM_WD_ACTION 5
struct code2string watchdogevent[NUM_WD_ACTION] = {
	{ SAHPI_WAE_NO_ACTION ,	"No Action" },
	{ SAHPI_WAE_RESET, 	"Reset" },
	{ SAHPI_WAE_POWER_DOWN, "Power Down"},
	{ SAHPI_WAE_POWER_CYCLE, "Power Cycle"},
	{ SAHPI_WAE_TIMER_INT, "Timer Interrupt"}
}; 


/**
 *
 *
 **/
#define NUM_WD_INT 5
struct code2string watchdogpint [NUM_WD_INT] = {
	{ SAHPI_WPI_NONE , "None" },
	{ SAHPI_WPI_SMI, "SMI" },
	{ SAHPI_WPI_NMI, "NMI" },
	{ SAHPI_WPI_MESSAGE_INTERRUPT, "Message Interrupt" },
	{ SAHPI_WPI_OEM, "OEM" }
};

/**
 *
 *
 **/
#define NUM_HS 6
struct code2string hotswapstate [NUM_HS] = {
	{SAHPI_HS_STATE_INACTIVE, "Hotswap State: Inactive"},   
	{SAHPI_HS_STATE_INSERTION_PENDING, "Hotswap State: Insertion Pending"},
	{SAHPI_HS_STATE_ACTIVE_HEALTHY, "Hotswap State: Active - Healthy"},
	{SAHPI_HS_STATE_ACTIVE_UNHEALTHY, "Hotswap State: Active - Unhealthy" },
	{SAHPI_HS_STATE_EXTRACTION_PENDING, "Hotswap State: Extraction Pending"},
	{SAHPI_HS_STATE_NOT_PRESENT, "Hotswap State:  Not Present"}
};

/**
 *
 *
 **/
#define NUM_SENSORTYPE 43
struct code2string sensortype [NUM_SENSORTYPE] = {
	{SAHPI_TEMPERATURE, "Sensor type: TEMPERATURE" },
	{SAHPI_VOLTAGE, "Sensor type: VOLTAGE" },
	{SAHPI_CURRENT, "Sensor type: CURRENT" },
	{SAHPI_FAN, "Sensor type: FAN" },
	{SAHPI_PHYSICAL_SECURITY, "Sensor type: PHYSICAL_SECURITY" },
	{SAHPI_PLATFORM_VIOLATION, "Sensor type: PLATFORM_VIOLATION" },
	{SAHPI_PROCESSOR, "Sensor type: PROCESSOR" },
	{SAHPI_POWER_SUPPLY,"Sensor type: POWER_SUPPLY" }, 
	{SAHPI_POWER_UNIT, "Sensor type: POWER_UNIT" },
	{SAHPI_COOLING_DEVICE, "Sensor type: COOLING_DEVICE" },
	{SAHPI_OTHER_UNITS_BASED_SENSOR, "Sensor type: OTHER_UNITS_BASED_SENSOR" },
	{SAHPI_MEMORY, "Sensor type: MEMORY" },
	{SAHPI_DRIVE_SLOT, "Sensor type: DRIVE_SLOT" },
	{SAHPI_POST_MEMORY_RESIZE, "Sensor type: POST_MEMORY_RESIZE" },
	{SAHPI_SYSTEM_FW_PROGRESS, "Sensor type: SYSTEM_FW_PROGRES" },
	{SAHPI_EVENT_LOGGING_DISABLED, "Sensor type: EVENT_LOGGING_DISABLED" },
	{SAHPI_RESERVED1, "Sensor type: reserved" },
	{SAHPI_SYSTEM_EVENT, "Sensor type: SYSTEM_EVENT" },
	{SAHPI_CRITICAL_INTERRUPT, "Sensor type: CRITICAL_INTERRUPT" },
	{SAHPI_BUTTON, "Sensor type: BUTTON" },
	{SAHPI_MODULE_BOARD, "Sensor type: MODULE_BOARD" },
	{SAHPI_MICROCONTROLLER_COPROCESSOR, "Sensor type: MICROCONTROLLER_COPROCESSOR" },
	{SAHPI_ADDIN_CARD, "Sensor type: ADDIN_CARD" },
	{SAHPI_CHASSIS, "Sensor type: CHASSIS" },
	{SAHPI_CHIP_SET, "Sensor type: CHIP_SET" },
	{SAHPI_OTHER_FRU, "Sensor type: OTHER_FRU" },
	{SAHPI_CABLE_INTERCONNECT, "Sensor type: CABLE_INTERCONNECT" },
	{SAHPI_TERMINATOR, "Sensor type: TERMINATOR" },
	{SAHPI_SYSTEM_BOOT_INITIATED, "Sensor type: SYSTEM_BOOT_INITIATED" },
	{SAHPI_BOOT_ERROR, "Sensor type: BOOT_ERROR" },
	{SAHPI_OS_BOOT, "Sensor type: OS_BOOT" },
	{SAHPI_OS_CRITICAL_STOP, "Sensor type: OS_CRITICAL_STOP" },
	{SAHPI_SLOT_CONNECTOR, "Sensor type: SLOT_CONNECTOR" },
	{SAHPI_SYSTEM_ACPI_POWER_STATE, "Sensor type: SYSTEM_ACPI_POWER_STATE" },
	{SAHPI_RESERVED2, "Sensor type: RESERVED2" },
	{SAHPI_PLATFORM_ALERT, "Sensor type: PLATFORM_ALERT" },
	{SAHPI_ENTITY_PRESENCE, "Sensor type: ENTITY_PRESENCE" },
	{SAHPI_MONITOR_ASIC_IC, "Sensor type: MONITOR_ASIC_IC" },
	{SAHPI_LAN, "Sensor type: LAN" },
	{SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH, "Sensor type: MANAGEMENT_SUBSYSTEM_HEALTH" },
	{SAHPI_BATTERY, "Sensor type: BATTERY" },
	{SAHPI_OPERATIONAL, "Sensor type: OPERATIONAL" },
	{SAHPI_OEM_SENSOR, "Sensor type: OEM_SENSOR" }
};

/**
 *
 *
 **/
#define NUM_EVENTTYPE 5
struct code2string eventtype [NUM_EVENTTYPE] = {
	{SAHPI_ET_SENSOR, "Sensor Event"},
	{SAHPI_ET_HOTSWAP, "Hotswap Event"},
	{SAHPI_ET_WATCHDOG, "Watchdog Event"},
	{SAHPI_ET_OEM, "OEM Event"},
	{SAHPI_ET_USER, "User Event"}
};

/**
 *
 *
 **/
#define NUM_SEV 6
struct code2string severity[NUM_SEV] = {
	{SAHPI_CRITICAL, "Critical"},
	{SAHPI_MAJOR, "Major"},
	{SAHPI_MINOR, "Minor" }, 
	{SAHPI_INFORMATIONAL, "Informational" },
	{SAHPI_OK, "OK" },
	{SAHPI_DEBUG, "Debug" }
};

/**
 * printInterpretedSensor:
 * @interpreted_value: pointer to a SaHpiSensorInterpretedT structure 
 * 
 * Return value: none 
 * Exported: 	no
 **/
static void printInterpretedSensor(SaHpiSensorInterpretedT *interpreted_value)
{
        switch (interpreted_value->Type)
        {
                case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_UINT8: ");
                        printf(" %u\n",interpreted_value->Value.SensorUint8);
                        break;
                case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_UINT16: ");
                        printf(" %u\n",interpreted_value->Value.SensorUint16);
                        break;
                case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_UINT32: ");
                        printf(" %u\n",interpreted_value->Value.SensorUint32);
                        break;
                case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_INT8: ");
                        printf(" %d\n",interpreted_value->Value.SensorInt8);
                        break;
                case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_INT16: ");
                        printf(" %d\n",interpreted_value->Value.SensorInt16);
                        break;
                case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_INT32: ");
                        printf(" %d\n",interpreted_value->Value.SensorInt32);
                        break;
                case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32: ");
                        printf(" %lf\n",interpreted_value->Value.SensorFloat32);
                        break;
                case SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER:
                        printf("SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER: ");
			/* TODO: print all in array */
                        printf("TODO:  %u\n",interpreted_value->Value.SensorBuffer[0]);
                        break;
                default:
                        printf("Invalid Type for Sensor Interpreted Value.\n");
                        break;
        }

}

/**
 * showOemEvent: Parse and printf information of an OEM event
 * @thisEvent: Pointer to a SaHpiEventT structure 
 * 
 * Return value: none 
 * Exported: 	no
 **/
static void showOemEvent(SaHpiEventT *thisEvent)
{
	printf("Manufacturing Id: %i\n", thisEvent->EventDataUnion.OemEvent.MId);
	printf("OEM Event Data: \n\t%s\n", thisEvent->EventDataUnion.OemEvent.OemEventData);
	return;
}

/**
 * showWatchdogEvent: Parse and printf information of a Watchdog event
 * @thisEvent: Pointer to a SaHpiEventT structure 
 * 
 * Return value: none 
 * Exported: 	no
 **/
static void showWatchdogEvent(SaHpiEventT *thisEvent)
{
	char *str = NULL;
	SaHpiWatchdogEventT *thisWatchdogEvent = &thisEvent->EventDataUnion.WatchdogEvent;

	printf("Watchdog Num %d\n", thisWatchdogEvent->WatchdogNum);
	str = decode_enum(&watchdogevent[0],  NUM_WD_ACTION, thisWatchdogEvent->WatchdogAction);
	printf("Watchdog Action %s\n", str);
	str = decode_enum(&watchdogpint[0], NUM_WD_INT , thisWatchdogEvent->WatchdogPreTimerAction);
	printf("Watchdog Pretimer interrupt %s\n", str);
	str = decode_enum(&watchdogtime[0], NUM_WT, thisWatchdogEvent->WatchdogUse);
	printf("Watchdog Timer Use %s\n", str);
	return;
}

/**
 * showSensorEvent: Parse and printf information of a Sensor event
 * @thisEvent: Pointer to a SaHpiEventT structure 
 * 
 * Return value: none 
 * Exported: 	no
 **/
static void showSensorEvent(SaHpiEventT *thisEvent)
{

	char *str = NULL;
	SaHpiSensorEventT *thisSensorEvent = &thisEvent->EventDataUnion.SensorEvent;

	printf( "Sensor # = %2d\n", thisSensorEvent->SensorNum);
	str = decode_enum(sensortype, NUM_SENSORTYPE,thisSensorEvent->SensorType);
	printf("Sensor Type %s\n",str);
	printf("Assertion State %d\n",thisSensorEvent->Assertion);
	printf("Sensor Event State %d\n",thisSensorEvent->EventState);
	printf("Sensor Optional Data %d\n", thisSensorEvent->OptionalDataPresent);
	/*
	 * printTriggerReading(thisSensorEvent->TriggerReading);
	*/
	printf("TriggerReading.ValuesPresent %d\n",thisSensorEvent->TriggerReading.ValuesPresent);
	printf("Sensor Raw value %d\n",thisSensorEvent->TriggerReading.Raw);
	printInterpretedSensor(&thisSensorEvent->TriggerReading.Interpreted);
	printf("Sensor Event Status - Sensor Status %x\n", thisSensorEvent->TriggerReading.EventStatus.SensorStatus);
	printf("Sensor Event Status - Event Status  %x\n", thisSensorEvent->TriggerReading.EventStatus.EventStatus);
 	/* 
	 * printTriggerReading(thisSensorEvent->TriggerThreshold);
	*/
	printf("TriggerThreshold.ValuesPresent %d\n",thisSensorEvent->TriggerThreshold.ValuesPresent);
	printf("Sensor Raw value %d\n",thisSensorEvent->TriggerThreshold.Raw);
	printInterpretedSensor(&thisSensorEvent->TriggerThreshold.Interpreted);
	printf("Sensor Event Status - Sensor Status %x\n", thisSensorEvent->TriggerThreshold.EventStatus.SensorStatus);
	printf("Sensor Event Status - Event Status  %x\n", thisSensorEvent->TriggerThreshold.EventStatus.EventStatus);

	printf("Sensor Previous State %d\n",thisSensorEvent->PreviousState);
	printf("Sensor OEM %d\n",thisSensorEvent->Oem);
	printf("Sensor Specific Date %d\n",thisSensorEvent->SensorSpecific);
	
	return;
}

/**
 * showHotswapEvent: Parse and printf information of a Hotswap event
 * @thisEvent: Pointer to a SaHpiEventT structure 
 * 
 * Return value: none 
 * Exported: 	no
 **/
static void showHotswapEvent(SaHpiEventT *thisEvent)
{
	
	char *str = NULL;
	SaHpiHotSwapEventT *thisHotSwapEvent = &thisEvent->EventDataUnion.HotSwapEvent;

	str = decode_enum(hotswapstate, NUM_HS, thisHotSwapEvent->HotSwapState);
	printf("HotSwapEventState %s\n", str);
	
	str = decode_enum(hotswapstate, NUM_HS, thisHotSwapEvent->PreviousHotSwapState);
	printf("PreviousHotSwapState %s\n", str);
	return;
}
 
/**
 * decode_enum: Look up an enum in the specified table,
 *		 and return the pointer to the desciptive string  
 * @code_array: Pointer to a code2string array 
 * @NUM_MAX   : Maximum entries in the above array
 * @code      : Enum to look up
 * 
 * Return value: Pointer to a string
 * Exported: yes
 **/
char *decode_enum(struct code2string *code_array, int NUM_MAX, int code)
{
        int i;
        char *str = NULL;
        for (i = 0; i < NUM_MAX; i++) {
                if (code == code_array[i].val) { str = code_array[i].str; break;}
        }

        if (str == NULL) {
                str = code_unknown;
        }
        return(str);
}

/**
 * print_event: Parse, convert and print information of an event record
 * @thisEvent: Pointer to the event record to be processed
 * 
 * Return value: none
 * Exported: yes
 **/
void print_event(SaHpiEventT *thisEvent)
{

        char *str = NULL;
        char timestr[40];

        printf("\n***** Event\n");
        printf( "Resource ID: %d\n", thisEvent->Source);
        str = decode_enum(eventtype, NUM_EVENTTYPE,thisEvent->EventType);
        printf("Event Type: %s\n", str);
	saftime2str(thisEvent->Timestamp, timestr, 40);
        printf("Event Time Stamp: %s\n", timestr);
        str = decode_enum(severity, NUM_SEV, thisEvent->Severity);
        printf("Severity Level: %s\n", str);

        switch (thisEvent->EventType)
        {
                case SAHPI_ET_SENSOR:
                        showSensorEvent(thisEvent);
                        break;
                case SAHPI_ET_HOTSWAP:
                        showHotswapEvent(thisEvent);
                        break;
                case SAHPI_ET_WATCHDOG:
                        showWatchdogEvent(thisEvent);
                        break;
                case SAHPI_ET_USER:
                        printf("User Event Data: \n\t%s\n",
                                         thisEvent->EventDataUnion.UserEvent.UserEventData);
                        break;
                case SAHPI_ET_OEM:
                        showOemEvent(thisEvent);
                        break;
                default:
                        printf("Invalid event type reported.\n");
                        break;
        }
} /*  */


/**
 * saftime2str: Convert sahpi time to calendar date/time string        
 * @time : sahpi time to be converted                     
 * @str  : location to store the converted string
 * @size : max size of the converted string
 * 
 * Return value: none
 * Exported: yes
 **/
void saftime2str(SaHpiTimeT time, char * str, size_t size)
{
	struct tm t;
	time_t tt;
	tt = time / 1000000000;
	localtime_r(&tt, &t);
	strftime(str, size, "%b %d, %Y - %H:%M:%S", &t);
}


/* end of file         */
