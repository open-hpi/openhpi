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
 *	pdphan	04/06/04	Move Showset() util here
 *	renierm 04/28/04	Add checks for null pointers
 */

#include <printevent_utils.h>

/**
 *
 *
 **/
#define NEVTYPES  5
char *evtypes[NEVTYPES] = {"sensor","hotswap","watchdog","oem   ","user  "};

/**
 *
 *
 **/
#define NUM_EC  14
struct { int val; char *str;
} eventcats[NUM_EC] = {
{ /*0x00*/ SAHPI_EC_UNSPECIFIED,  "unspecified"},
{ /*0x01*/ SAHPI_EC_THRESHOLD,    "Threshold"},
{ /*0x02*/ SAHPI_EC_USAGE,        "Usage    "},
{ /*0x03*/ SAHPI_EC_STATE,        "State    "},
{ /*0x04*/ SAHPI_EC_PRED_FAIL,    "Predictive"},
{ /*0x05*/ SAHPI_EC_LIMIT,        "Limit    "},
{ /*0x06*/ SAHPI_EC_PERFORMANCE,  "Performnc"},
{ /*0x07*/ SAHPI_EC_SEVERITY,     "Severity "},
{ /*0x08*/ SAHPI_EC_PRESENCE,     "DevPresen"},
{ /*0x09*/ SAHPI_EC_ENABLE,       "DevEnable"},
{ /*0x0a*/ SAHPI_EC_AVAILABILITY, "Availabil"},
{ /*0x0b*/ SAHPI_EC_REDUNDANCY,   "Redundanc"},
{ /*0x7e*/ SAHPI_EC_USER,         "UserDefin"},
{ /*0x7f*/ SAHPI_EC_GENERIC,      "OemDefin " }};

/**
 *
 *
 **/
#define NUM_ES  6
struct { int val; char *str;
} eventstates[NUM_ES] = {
{ SAHPI_ES_LOWER_MINOR , "lo-min" },
{ SAHPI_ES_LOWER_MAJOR , "lo-maj" },
{ SAHPI_ES_LOWER_CRIT  , "lo-crt" },
{ SAHPI_ES_UPPER_MINOR , "hi-min" },
{ SAHPI_ES_UPPER_MAJOR , "hi-maj" },
{ SAHPI_ES_UPPER_CRIT  , "hi-crt" } };

/**
 *
 *
 **/
#define NGDESC   4
struct {
 unsigned char val;
 const char str[5];
} gen_desc[NGDESC] = {
 {0x00, "IPMB"},
 {0x03, "BIOS"},
 {0x20, "BMC "},
 {0x21, "SMI "} };

/**
 *
 *
 **/
#define NSDESC   13
struct {
 unsigned char s_typ;
 unsigned char s_num;
 unsigned char data1;
 unsigned char data2;
 unsigned char data3;
 char *desc;
} sens_desc[NSDESC] = {
{0x01, 0xff, 0x07, 0xff, 0xff, "Temperature Upper Non-critical" },
{0x01, 0xff, 0x09, 0xff, 0xff, "Temperature Upper Critical"},
{0x07, 0xff, 0x01, 0xff, 0xff, "Processor Thermal trip"},
{0x09, 0x01, 0xff, 0xff, 0xff, "Power Off/Down"},
{0x0f, 0x06, 0xff, 0xff, 0xff, "POST Code"},
{0x10, 0x09, 0x02, 0xff, 0xff, "EventLog Cleared"},
{0x12, 0x83, 0xff, 0xff, 0xff, "System Boot Event"},
{0x14, 0xff, 0x02, 0xff, 0xff, "Reset Button pressed"},
{0x14, 0xff, 0x00, 0xff, 0xff, "Power Button pressed"},
{0x23, 0x03, 0x01, 0xff, 0xff, "Watchdog2 Hard Reset action"},
{0x23, 0x03, 0x02, 0xff, 0xff, "Watchdog2 Power down action"},
{0xf3, 0x85, 0x01, 0xff, 0xff, "State is now OK"},
{0x20, 0x00, 0xff, 0xff, 0xff, "OS Kernel Panic"} };


/**
 *
 *
 **/
#define NUMST   0x2A
const char *sensor_types[NUMST] = {
/* 00h */ "reserved",
/* 01h */ "Temperature",
/* 02h */ "Voltage",
/* 03h */ "Current",
/* 04h */ "Fan",
/* 05h */ "Platform Chassis Intrusion",
/* 06h */ "Platform Security Violation",
/* 07h */ "Processor",
/* 08h */ "Power Supply",
/* 09h */ "Power Unit",
/* 0Ah */ "Cooling Device",
/* 0Bh */ "FRU Sensor",
/* 0Ch */ "Memory",
/* 0Dh */ "Drive Slot",
/* 0Eh */ "POST Memory Resize",
/* 0Fh */ "System Firmware",
/* 10h */ "EventLog Cleared",
/* 11h */ "Watchdog 1",
/* 12h */ "System Event",          /* offset 0,1,2 */
/* 13h */ "Critical Interrupt",    /* offset 0,1,2 */
/* 14h */ "Button",                /* offset 0,1,2 */
/* 15h */ "Board",
/* 16h */ "Microcontroller",
/* 17h */ "Add-in Card",
/* 18h */ "Chassis",
/* 19h */ "Chip Set",
/* 1Ah */ "Other FRU",
/* 1Bh */ "Cable / Interconnect",
/* 1Ch */ "Terminator",
/* 1Dh */ "System Boot Initiated",
/* 1Eh */ "Boot Error",
/* 1Fh */ "OS Boot",
/* 20h */ "OS Critical Stop",
/* 21h */ "Slot / Connector",
/* 22h */ "ACPI Power State",
/* 23h */ "Watchdog 2",
/* 24h */ "Platform Alert",
/* 25h */ "Entity Presence",
/* 26h */ "Monitor ASIC",
/* 27h */ "LAN",
/* 28h */ "Management Subsystem Health",
/* 29h */ "Battery",
};

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
        if (!interpreted_value) return;
	
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
	if (!thisEvent) return;
	
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
	SaHpiWatchdogEventT *thisWatchdogEvent;
	
	if (!thisEvent) return;
	thisWatchdogEvent = &thisEvent->EventDataUnion.WatchdogEvent;
	
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
	SaHpiSensorEventT *thisSensorEvent;
	
	if (!thisEvent) return;
	thisSensorEvent = &thisEvent->EventDataUnion.SensorEvent;

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
	SaHpiHotSwapEventT *thisHotSwapEvent;

	if (!thisEvent) return;
	thisHotSwapEvent = &thisEvent->EventDataUnion.HotSwapEvent;

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

	if (code_array) {
        	for (i = 0; i < NUM_MAX; i++) {
                	if (code == code_array[i].val) {
				str = code_array[i].str; break;
			}
        	}
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

        if (!thisEvent) return;	
	
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
}

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
	
	if (!str) return;
	
	tt = time / 1000000000;
	localtime_r(&tt, &t);
	strftime(str, size, "%b %d, %Y - %H:%M:%S", &t);
}

void ShowSel( SaHpiSelEntryT  *sel, SaHpiRdrT *rdr,
                     SaHpiRptEntryT *rptentry )
{
        unsigned char evtype;
        char timestr[256];
        time_t tt1;
        int ec, eci;
        int es, esi;
        char *srctag;
        char *rdrtag;
        const char *pstr;
        char estag[8];
        unsigned char *pd;
        int ix, i, styp;
        int outlen;
        char outbuf[132];
        char mystr[26];
        unsigned char data1, data2, data3;
		//char date[30];

        if (!sel || !rdr || !rptentry) return;
	
	/*format & print the EventLog entry*/

        if (sel->Event.Timestamp > SAHPI_TIME_MAX_RELATIVE) { /*absolute time*/
                tt1 = sel->Event.Timestamp / 1000000000;
                strftime(timestr,sizeof(timestr),"%F %T", localtime(&tt1));
        } else if (sel->Event.Timestamp > SAHPI_TIME_UNSPECIFIED) { /*invalid time*/
				printf("timestamp: %llx\n", sel->Event.Timestamp);
                strcpy(timestr,"invalid time     ");
        } else {   /*relative time*/
                tt1 = sel->Event.Timestamp / 1000000000;
                sprintf(timestr,"rel(%lx)", (unsigned long)tt1);
				//saftime2str(sel->Event.Timestamp, date, 30);
				//printf("timestamp = %s\n", date);

        }
        if (rptentry->ResourceId == sel->Event.Source)
                srctag = rptentry->ResourceTag.Data;
        else
                srctag = "unspec ";  /* SAHPI_UNSPECIFIED_RESOURCE_ID */

        evtype = sel->Event.EventType;
        if (evtype > NEVTYPES)
                evtype = NEVTYPES - 1;

        if (rdr->RdrType == SAHPI_NO_RECORD) {
                rdrtag = "rdr-unkn";
        } else {
                rdr->IdString.Data[rdr->IdString.DataLength] = 0;
                rdrtag = &rdr->IdString.Data[0];
        }
        sprintf(outbuf,"%i %s %s ", sel->EntryId,
                timestr, evtypes[evtype] );
        outlen = strlen(outbuf);
        pstr = "";

        /*
          sld: there is a lot of stuff specific to IPMI and other HPI implementations
          here.  Scrubing for HPI 1.0 only data would be a good thing soon
        */

        switch(evtype)
        {
        case SAHPI_ET_SENSOR:   /*Sensor*/
                /* decode event category */
                ec = sel->Event.EventDataUnion.SensorEvent.EventCategory;
                for (eci = 0; eci < NUM_EC; eci++)
                        if (eventcats[eci].val == ec) break;
                if (eci >= NUM_EC) eci = 0;
                /* decode event state */
                es = sel->Event.EventDataUnion.SensorEvent.EventState;
                if (eci == 1) { /*SAHPI_EC_THRESHOLD*/
                        for (esi = 0; esi < NUM_ES; esi++)
                                if (eventstates[esi].val == es) break;
                        if (esi >= NUM_ES) esi = 0;
                        strcpy(estag,eventstates[esi].str);
                } else sprintf(estag,"%02x",es);

                /* decode sensor type */
                styp = sel->Event.EventDataUnion.SensorEvent.SensorType;
                /* data3 is not specifically defined in HPI 1.0, implementation hack */
                pd = (unsigned char *)&sel->Event.EventDataUnion.SensorEvent.SensorSpecific;
                data1 = pd[0];
                data2 = pd[1];
                data3 = pd[2];
                if (styp >= NUMST) { styp = 0; }

                if (styp == 0x20) { /*OS Critical Stop*/
                        /* Show first 3 chars of panic string */
                        mystr[0] = '(';
                        mystr[1] = sel->Event.EventDataUnion.SensorEvent.SensorNum & 0x7f;
                        mystr[2] = data2 & 0x007f;
                        mystr[3] = data3 & 0x7f;
                        mystr[4] = ')';
                        mystr[5] = 0;
                        if (sel->Event.EventDataUnion.SensorEvent.SensorNum & 0x80)
                                strcat(mystr,"Oops!");
                        if (data2 & 0x80) strcat(mystr,"Int!");
                        if (data3 & 0x80) strcat(mystr,"NullPtr!");
                        pstr = mystr;
                }
                sprintf(&outbuf[outlen], "%s, %s %s %x [%02x %02x %02x] %s",
                        sensor_types[styp], eventcats[eci].str, estag,
                        sel->Event.EventDataUnion.SensorEvent.SensorNum,
                        data1, data2, data3, pstr);
                break;
        case SAHPI_ET_USER:   /*User, usu 16-byte IPMI SEL record */
                pd = &sel->Event.EventDataUnion.UserEvent.UserEventData[0];
                /* get gen_desc from offset 7 */
                for (ix = 0; ix < NGDESC; ix++)
                        if (gen_desc[ix].val == pd[7]) break;
                if (ix >= NGDESC) ix = 0;
                /* get sensor type description for misc cases */
                styp = pd[10];   /*sensor type*/
                data3 = pd[15];
                /* = *sel->Event.EventDataUnion.SensorEvent.SensorSpecific+1; */
                for (i = 0; i < NSDESC; i++) {
                        if (sens_desc[i].s_typ == styp) {
                                if (sens_desc[i].s_num != 0xff &&
                                    sens_desc[i].s_num != pd[11])
                                        continue;
                                if (sens_desc[i].data1 != 0xff &&
                                    (sens_desc[i].data1 & 0x07) != pd[13])
                                        continue;
                                if (sens_desc[i].data2 != 0xff &&
                                    sens_desc[i].data2 != pd[14])
                                        continue;
                                if (sens_desc[i].data3 != 0xff &&
                                    sens_desc[i].data3 != data3)
                                        continue;
                                /* have a match, use description */
                                pstr = (char *)sens_desc[i].desc;
                                break;
                        }
                } /*end for*/
                if (i >= NSDESC) {
                        if (styp >= NUMST) styp = 0;
                        pstr = sensor_types[styp];
                }
                sprintf(&outbuf[outlen], "%s, %s %02x %02x %02x [%02x %02x %02x]",
                        pstr, gen_desc[ix].str,
                        pd[10],pd[11],pd[12],pd[13],pd[14],data3);
                break;
        case SAHPI_ET_OEM:
                /* only go into this if it is IBM hardware, as others might use
                   the Oem field differently */
                if(sel->Event.EventDataUnion.OemEvent.MId == 2) {
                        /* sld: I'm going to printf directly, as the output buffer isn't
                           big enough for what I want to do */
                        printf("Oem Event:\n");
                        saftime2str(sel->Timestamp, timestr, 40);
                        printf("\tTimestamp: %s\n", timestr);
                        printf("\tSeverity: %d\n", sel->Event.Severity);
                        printf("\tMId:%d, Data: %s\n",
                               sel->Event.EventDataUnion.OemEvent.MId,
                               sel->Event.EventDataUnion.OemEvent.OemEventData);
                }
                break;
        default:
                pd = &sel->Event.EventDataUnion.UserEvent.UserEventData[0];
                styp = pd[10];
                data3 = pd[15];
                /* *sel->Event.EventDataUnion.SensorEvent.SensorSpecific+1 */
                if (styp >= NUMST) {
                        printf("sensor type %d >= max %d\n",styp,NUMST);
                        styp = 0;
                }
                pstr = sensor_types[styp];
                sprintf(&outbuf[outlen], "%s, %x %x, %02x %02x %02x [%02x %02x %02x/%02x]",
                        pstr, pd[0], pd[7], pd[10], pd[11], pd[12],
                        pd[13], pd[14], pd[15], data3);
                break;
        }
        printf("%s\n",outbuf);
}

/* end of file         */
