#include <stdlib.h>
#include <stdio.h>
#include <SaHpi.h>
#include <unistd.h>
#include <string.h>

/* debug macros */
#define warn(str) fprintf(stderr,"%s: " str "\n", __FUNCTION__)
#define error(str, e) fprintf(stderr,str ": %s\n", get_error_string(e))

/* Function prototypes */
SaErrorT discover_domain(SaHpiDomainIdT, SaHpiSessionIdT, SaHpiRptEntryT);
const char * get_error_string(SaErrorT);
void display_entity_capabilities(SaHpiCapabilitiesT);
const char * severity2str(SaHpiSeverityT);
const char * type2string(SaHpiEntityTypeT type);
const char * rdrtype2str(SaHpiRdrTypeT type);
const char * rpt_cap2str(SaHpiCapabilitiesT ResourceCapabilities);
const char * get_sensor_type(SaHpiSensorTypeT type);
const char * get_sensor_category(SaHpiEventCategoryT category);
void list_rdr(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id);
void display_id_string(SaHpiTextBufferT string);

/**
 * main: main program loop
 *
 *
 *
 * Return value: int 0
 **/
int main(int arc, const char *argv[])
{
        SaErrorT 		err;
        SaHpiVersionT		version;
        SaHpiSessionIdT 	session_id;
        SaHpiRptEntryT		entry;
        //SaHpiSelInfoT		Info;
        
	
	/* First step in HPI and openhpi */
        err = saHpiInitialize(&version);
        if (SA_OK != err) {
                error("saHpiInitialize", err);
                exit(-1);
        }
        
	/* Every domain requires a new session */
	/* This example is for one domain, one session */
        err = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
        if (SA_OK != err) {
                error("saHpiSessionOpen", err);
                return err;
        }
        
        err = discover_domain(SAHPI_DEFAULT_DOMAIN_ID, session_id, entry);
        if (SA_OK != err) {
                warn("an error was encountered, results may be incomplete");
        }

        err = saHpiFinalize();
        if (SA_OK != err) {
                error("saHpiFinalize", err);
                exit(-1);

	}
	return 0;
}

SaErrorT discover_domain(SaHpiDomainIdT domain_id, SaHpiSessionIdT session_id, SaHpiRptEntryT entry)
{
	
	SaErrorT	err;
	SaHpiRptInfoT	rpt_info_before;
	SaHpiRptInfoT   rpt_info_after;
	SaHpiEntryIdT	current;
	SaHpiEntryIdT   next;

        err = saHpiResourcesDiscover(session_id);
        if (SA_OK != err) {
                error("saHpiResourcesDiscover", err);
                return err;
        }
        
        /* grab copy of the update counter before traversing RPT */
        err = saHpiRptInfoGet(session_id, &rpt_info_before);
        if (SA_OK != err) {
                error("saHpiRptInfoGet", err);
                return err;
        }
        
        warn("Scanning RPT...");
        next = SAHPI_FIRST_ENTRY;
        do {
                int i;
                current = next;
                err = saHpiRptEntryGet(session_id, current, &next, &entry);
                if (SA_OK != err) {
                        if (current != SAHPI_FIRST_ENTRY) {
                                error("saHpiRptEntryGet", err);
                                return err;
                        } else {
                                warn("Empty RPT\n");
                                break;
                        }
                }

		printf("***Records:\n");
                printf("%s\n", (char *)entry.ResourceTag.Data);
                printf("Entry ID: %x\n", (int) entry.EntryId);
                printf("Resource ID: %x\n", (int) entry.ResourceId);
                printf("Domain ID: %x\n", (int) entry.DomainId);
                printf("Revision: %c\n", entry.ResourceInfo.ResourceRev);
                printf("Version: %c\n", entry.ResourceInfo.SpecificVer);
                printf("Severity: %s\n",severity2str(entry.ResourceSeverity));
		printf("Resource Capability: %s\n", rpt_cap2str(entry.ResourceCapabilities));

                printf("Entity Path:\n");
                for ( i=0; i<SAHPI_MAX_ENTITY_PATH; i++ )
                {
                        SaHpiEntityT tmp = entry.ResourceEntity.Entry[i];
                        if (tmp.EntityType <= SAHPI_ENT_UNSPECIFIED)
                                break;

                        printf("\t{%s, %i}\n", type2string(tmp.EntityType),
                               tmp.EntityInstance);
                }

                if (entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
                        list_rdr(session_id, entry.ResourceId);
                
#if 0
                /* if the resource is also a domain, then 
                 * traverse its RPT */        
                if (entry.ResourceCapabilities & SAHPI_CAPABILITY_DOMAIN) {
                        err = discover_domain(entry.DomainId);
                        if (SA_OK != err)
                                return err;
                }
#endif
        } while (next != SAHPI_LAST_ENTRY);

	printf("SAHPI_LAST_ENTRY\n");
		
        /* wait for update in RPT */
        while (1) {
                err = saHpiRptInfoGet(session_id, &rpt_info_after);
                if (SA_OK != err) {
                        error("saHpiRptInfoGet", err);
                        return err;
                }
                if (rpt_info_before.UpdateCount != rpt_info_after.UpdateCount) {
                        rpt_info_before = rpt_info_after;
                } else
			break;
        };

	
        return SA_OK;
}

const char * severity2str(SaHpiSeverityT severity)
{
        switch (severity) {
        case SAHPI_CRITICAL:
                return "CRITICAL";
        case SAHPI_MAJOR:
                return "MAJOR";
        case SAHPI_MINOR:
                return "MINOR";
        case SAHPI_INFORMATIONAL:
                return "INFORMATIONAL";               
        case SAHPI_OK:
                return "OK";
        case SAHPI_DEBUG:
                return "DEBUG";
        default:
                return "UNKNOWN SEVERITY";
        }
}

const char * get_error_string(SaErrorT error)
{
        switch(error) {
                case SA_ERR_HPI_ERROR:
                        return "SA_ERR_HPI_ERROR";
                case SA_ERR_HPI_UNSUPPORTED_API:
                        return "SA_ERR_UNSUPPORTED_API";
                case SA_ERR_HPI_BUSY:
                        return "SA_ERR_HPI_BUSY";
                case SA_ERR_HPI_INVALID:
                        return "SA_ERR_HPI_INVALID";
                case SA_ERR_HPI_INVALID_CMD:
                        return "SA_ERR_HPI_INVALID_CMD";
                case SA_ERR_HPI_TIMEOUT:
                        return "SA_ERR_HPI_TIMEOUT";
                case SA_ERR_HPI_OUT_OF_SPACE:
                        return "SA_ERR_HPI_OUT_OF_SPACE";
                case SA_ERR_HPI_DATA_TRUNCATED:
                        return "SA_ERR_HPI_DATA_TRUNCATED";
                case SA_ERR_HPI_DATA_LEN_INVALID:
                        return "SA_ERR_HPI_DATA_LEN_INVALID";
                case SA_ERR_HPI_DATA_EX_LIMITS:
                        return "SA_ERR_HPI_DATA_EX_LIMITS";
                case SA_ERR_HPI_INVALID_PARAMS:
                        return "SA_ERR_HPI_INVALID_PARAMS";
                case SA_ERR_HPI_INVALID_DATA:
                        return "SA_ERR_HPI_INVALID_DATA";
                case SA_ERR_HPI_NOT_PRESENT:
                        return "SA_ERR_HPI_NOT_PRESENT";
                case SA_ERR_HPI_INVALID_DATA_FIELD:
                        return "SA_ERR_HPI_INVALID_DATA_FIELD";
                case SA_ERR_HPI_INVALID_SENSOR_CMD:
                        return "SA_ERR_HPI_INVALID_SENSOR_CMD";
                case SA_ERR_HPI_NO_RESPONSE:
                        return "SA_ERR_HPI_NO_RESPONSE";
                case SA_ERR_HPI_DUPLICATE:
                        return "SA_ERR_HPI_DUPLICATE";
                case SA_ERR_HPI_INITIALIZING:
                        return "SA_ERR_HPI_INITIALIZING";
                case SA_ERR_HPI_UNKNOWN:
                        return "SA_ERR_HPI_UNKNOWN";
                case SA_ERR_HPI_INVALID_SESSION:
                        return "SA_ERR_HPI_INVALID_SESSION";
                case SA_ERR_HPI_INVALID_RESOURCE:
                        return "SA_ERR_HPI_INVALID_RESOURCE";
                case SA_ERR_HPI_INVALID_REQUEST:
                        return "SA_ERR_HPI_INVALID_REQUEST";
                case SA_ERR_HPI_ENTITY_NOT_PRESENT:
                        return "SA_ERR_HPI_ENTITY_NOT_PRESENT";
                case SA_ERR_HPI_UNINITIALIZED:
                        return "SA_ERR_HPI_UNINITIALIZED";
                default:
                        return "(invalid error code)";
        }
}

const char * type2string(SaHpiEntityTypeT type)
{
        switch(type) {
        case SAHPI_ENT_UNSPECIFIED:
                return "SAHPI_ENT_UNSPECIFIED";
        case SAHPI_ENT_OTHER:
                return "SAHPI_ENT_OTHER";
        case SAHPI_ENT_UNKNOWN:
                return "SAHPI_ENT_UNKNOWN";
        case SAHPI_ENT_PROCESSOR:
                return "SAHPI_ENT_PROCESSOR";
        case SAHPI_ENT_DISK_BAY:
                return "SAHPI_ENT_DISK_BAY";
        case SAHPI_ENT_PERIPHERAL_BAY:
                return "SAHPI_ENT_PERIPHERAL_BAY";
        case SAHPI_ENT_SYS_MGMNT_MODULE:
                return "SAHPI_ENT_SYS_MGMNT_MODULE";
        case SAHPI_ENT_SYSTEM_BOARD:
                return "SAHPI_ENT_SYSTEM_BOARD";
        case SAHPI_ENT_MEMORY_MODULE:
                return "SAHPI_ENT_MEMORY_MODULE";
        case SAHPI_ENT_PROCESSOR_MODULE:
                return "SAHPI_ENT_PROCESSOR_MODULE";
        case SAHPI_ENT_POWER_SUPPLY:
                return "SAHPI_ENT_POWER_SUPPLY";
        case SAHPI_ENT_ADD_IN_CARD:
                return "SAHPI_ENT_ADD_IN_CARD";
        case SAHPI_ENT_FRONT_PANEL_BOARD:
                return "SAHPI_ENT_FRONT_PANEL_BOARD";
        case SAHPI_ENT_BACK_PANEL_BOARD:
                return "SAHPI_ENT_BACK_PANEL_BOARD";
        case SAHPI_ENT_POWER_SYSTEM_BOARD:
                return "SAHPI_ENT_POWER_SYSTEM_BOARD";
        case SAHPI_ENT_DRIVE_BACKPLANE:
                return "SAHPI_ENT_DRIVE_BACKPLANE";
        case SAHPI_ENT_SYS_EXPANSION_BOARD:
                return "SAHPI_ENT_SYS_EXPANSION_BOARD";
        case SAHPI_ENT_OTHER_SYSTEM_BOARD:
                return "SAHPI_ENT_OTHER_SYSTEM_BOARD";
        case SAHPI_ENT_PROCESSOR_BOARD:
                return "SAHPI_ENT_PROCESSOR_BOARD";
        case SAHPI_ENT_POWER_UNIT:
                return "SAHPI_ENT_POWER_UNIT";
        case SAHPI_ENT_POWER_MODULE:
                return "SAHPI_ENT_POWER_MODULE";
        case SAHPI_ENT_POWER_MGMNT:
                return "SAHPI_ENT_POWER_MGMNT";
        case SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD:
                return "SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD";
        case SAHPI_ENT_SYSTEM_CHASSIS:
                return "SAHPI_ENT_SYSTEM_CHASSIS";
        case SAHPI_ENT_SUB_CHASSIS:
                return "SAHPI_ENT_SUB_CHASSIS";
        case SAHPI_ENT_OTHER_CHASSIS_BOARD:
                return "SAHPI_ENT_OTHER_CHASSIS_BOARD";
        case SAHPI_ENT_DISK_DRIVE_BAY:
                return "SAHPI_ENT_DISK_DRIVE_BAY";
        case SAHPI_ENT_PERIPHERAL_BAY_2:
                return "SAHPI_ENT_PERIPHERAL_BAY_2";
        case SAHPI_ENT_DEVICE_BAY:
                return "SAHPI_ENT_DEVICE_BAY";
        case SAHPI_ENT_COOLING_DEVICE:
                return "SAHPI_ENT_COOLING_DEVICE";
        case SAHPI_ENT_COOLING_UNIT:
                return "SAHPI_ENT_COOLING_UNIT";
        case SAHPI_ENT_INTERCONNECT:
                return "SAHPI_ENT_INTERCONNECT";
        case SAHPI_ENT_MEMORY_DEVICE:
                return "SAHPI_ENT_MEMORY_DEVICE";
        case SAHPI_ENT_SYS_MGMNT_SOFTWARE:
                return "SAHPI_ENT_SYS_MGMNT_SOFTWARE";
        case SAHPI_ENT_BIOS:
                return "SAHPI_ENT_BIOS";
        case SAHPI_ENT_OPERATING_SYSTEM:
                return "SAHPI_ENT_OPERATING_SYSTEM";
        case SAHPI_ENT_SYSTEM_BUS:
                return "SAHPI_ENT_SYSTEM_BUS";
        case SAHPI_ENT_GROUP:
                return "SAHPI_ENT_GROUP";
        case SAHPI_ENT_REMOTE:
                return "SAHPI_ENT_REMOTE";
        case SAHPI_ENT_EXTERNAL_ENVIRONMENT:
                return "SAHPI_ENT_EXTERNAL_ENVIRONMENT";
        case SAHPI_ENT_BATTERY:
                return "SAHPI_ENT_BATTERY";
        case SAHPI_ENT_CHASSIS_SPECIFIC:
                return "SAHPI_ENT_CHASSIS_SPECIFIC";
        case SAHPI_ENT_BOARD_SET_SPECIFIC:
                return "SAHPI_ENT_BOARD_SET_SPECIFIC";
        case SAHPI_ENT_OEM_SYSINT_SPECIFIC:
                return "SAHPI_ENT_OEM_SYSINT_SPECIFIC";
        case SAHPI_ENT_ROOT:
                return "SAHPI_ENT_ROOT";
        case SAHPI_ENT_RACK:
                return "SAHPI_ENT_RACK";
        case SAHPI_ENT_SUBRACK:
                return "SAHPI_ENT_SUBRACK";
        case SAHPI_ENT_COMPACTPCI_CHASSIS:
                return "SAHPI_ENT_COMPACTPCI_CHASSIS";
        case SAHPI_ENT_ADVANCEDTCA_CHASSIS:
                return "SAHPI_ENT_ADVANCEDTCA_CHASSIS";
        case SAHPI_ENT_SYSTEM_SLOT:
                return "SAHPI_ENT_SYSTEM_SLOT";
        case SAHPI_ENT_SBC_BLADE:
                return "SAHPI_ENT_SBC_BLADE";
        case SAHPI_ENT_IO_BLADE:
                return "SAHPI_ENT_IO_BLADE";
        case SAHPI_ENT_DISK_BLADE:
                return "SAHPI_ENT_DISK_BLADE";
        case SAHPI_ENT_DISK_DRIVE:
                return "SAHPI_ENT_DISK_DRIVE";
        case SAHPI_ENT_FAN:
                return "SAHPI_ENT_FAN";
        case SAHPI_ENT_POWER_DISTRIBUTION_UNIT:
                return "SAHPI_ENT_POWER_DISTRIBUTION_UNIT";
        case SAHPI_ENT_SPEC_PROC_BLADE:
                return "SAHPI_ENT_SPEC_PROC_BLADE";
        case SAHPI_ENT_IO_SUBBOARD:
                return "SAHPI_ENT_IO_SUBBOARD";
        case SAHPI_ENT_SBC_SUBBOARD:
                return "SAHPI_ENT_SBC_SUBBOARD";
        case SAHPI_ENT_ALARM_MANAGER:
                return "SAHPI_ENT_ALARM_MANAGER";
        case SAHPI_ENT_ALARM_MANAGER_BLADE:
                return "SAHPI_ENT_ALARM_MANAGER_BLADE";
        case SAHPI_ENT_SUBBOARD_CARRIER_BLADE:
                return "SAHPI_ENT_SUBBOARD_CARRIER_BLADE";
        default:
                return "(invalid entity type)";
        }
        return "\0";
}

const char * rdrtype2str(SaHpiRdrTypeT type)
{
        switch (type) {
        case SAHPI_NO_RECORD:
                return "SAHPI_NO_RECORD";
        case SAHPI_CTRL_RDR:
                return "SAHPI_CTRL_RDR";
        case SAHPI_SENSOR_RDR:
                return "SAHPI_SENSOR_RDR";
        case SAHPI_INVENTORY_RDR:
                return "SAHPI_INVENTORY_RDR";
        case SAHPI_WATCHDOG_RDR:
                return "SAHPI_WATCHDOG_RDR";
        default:
                return "(invalid rdr type)";
        }
        return "\0";
}

void list_rdr(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id)
{
        SaErrorT             err;
        SaHpiEntryIdT        current_rdr;
        SaHpiEntryIdT        next_rdr;
        SaHpiRdrT            rdr;

        printf("RDR Info:\n");
        next_rdr = SAHPI_FIRST_ENTRY;
        do {
                int i;
                current_rdr = next_rdr;
                err = saHpiRdrGet(session_id, resource_id, current_rdr, 
                                &next_rdr, &rdr);
                if (SA_OK != err) {
                        if (current_rdr == SAHPI_FIRST_ENTRY)
                                printf("Empty RDR table\n");
                        else
                                error("saHpiRdrGet", err);
                        return;                        
                }
                
                printf("\tRecordId: %x\n", rdr.RecordId);
                printf("\tRdrType: %s\n", rdrtype2str(rdr.RdrType));
		
		if (rdr.RdrType == SAHPI_SENSOR_RDR)
		{
			SaHpiSensorReadingT	reading;
			SaHpiSensorTypeT	type;
			SaHpiSensorNumT		num;
			SaHpiEventCategoryT	category;
			SaHpiSensorThresholdsT	thres;
			//SaHpiSensorReadingT	converted;
			
			SaErrorT val;
			
			num = rdr.RdrTypeUnion.SensorRec.Num;
			
			val = saHpiSensorTypeGet(session_id, resource_id, num, &type, &category);
			
			printf("\tSensor num: %i\n\tType: %s\n", num, get_sensor_type(type)); 
			printf("\tCategory: %s\n", get_sensor_category(category)); 

			err = saHpiSensorReadingGet(session_id, resource_id, num, &reading);
			if (err != SA_OK) {
				printf("Error reading sensor data {sensor, %d}", num);
				break;
			} else {
				if (reading.ValuesPresent & SAHPI_SRF_RAW) {
					printf("\tValues Present: RAW\n");
					printf("\t\tRaw value: %d\n", reading.Raw);
					
					err = saHpiSensorThresholdsGet(session_id, resource_id, num, &thres);
					if (err != SA_OK) {
						printf("Error reading sensor thresholds {sensor, %d}\n", num);
						continue;
					}
				}
				
				
				if (reading.ValuesPresent & SAHPI_SRF_INTERPRETED)
					printf("\tValues Present: Interpreted\n");
				if (reading.ValuesPresent & SAHPI_SRF_EVENT_STATE)
					printf("\tValues Present: Event State\n");
					
			}
			
				
		}

                printf("\tEntity: \n");
                for ( i=0; i<SAHPI_MAX_ENTITY_PATH; i++)
                {
                        SaHpiEntityT tmp = rdr.Entity.Entry[i];
                        if (tmp.EntityType <= SAHPI_ENT_UNSPECIFIED)
                                break;
                                printf("\t\t{%s, %i}\n", 
                                type2string(tmp.EntityType),
                                tmp.EntityInstance);
                }
                printf("\tIdString: ");
                       display_id_string(rdr.IdString);
        }while(next_rdr != SAHPI_LAST_ENTRY);
}

void display_id_string(SaHpiTextBufferT string)
{
        int i;
        switch(string.DataType) {
        case SAHPI_TL_TYPE_ASCII6:
                for (i = 0; i < string.DataLength; i++)
                        printf("%c", string.Data[i]);
                break;
        default:
                printf("Unsupported string type");
        }
        printf("\n");
}

const char * rpt_cap2str (SaHpiCapabilitiesT ResourceCapabilities)
{
	switch (ResourceCapabilities) {
		case SAHPI_CAPABILITY_DOMAIN:
			return("SAHPI_CAPABILITY_DOMAIN");
		case SAHPI_CAPABILITY_RESOURCE:
			return("SAHPI_CAPABILITY_RESOURCE");
		case SAHPI_CAPABILITY_SEL:
			return("SAHPI_CAPABILITY_SEL");
		case SAHPI_CAPABILITY_EVT_DEASSERTS:
			return("SAHPI_CAPABILITY_EVT_DEASSERTS");
		case SAHPI_CAPABILITY_AGGREGATE_STATUS:
			return("#define SAHPI_CAPABILITY_AGGREGATE_STATUS");
		case SAHPI_CAPABILITY_CONFIGURATION:
			return("SAHPI_CAPABILITY_CONFIGURATION");
		case SAHPI_CAPABILITY_MANAGED_HOTSWAP:
			return("SAHPI_CAPABILITY_MANAGED_HOTSWAP");
		case SAHPI_CAPABILITY_WATCHDOG:
			return("SAHPI_CAPABILITY_WATCHDOG");
		case SAHPI_CAPABILITY_CONTROL:
			return("SAHPI_CAPABILITY_CONTROL");
		case SAHPI_CAPABILITY_FRU:
			return("SAHPI_CAPABILITY_FRU");
		case SAHPI_CAPABILITY_INVENTORY_DATA:
			return("SAHPI_CAPABILITY_INVENTORY_DATA");
		case SAHPI_CAPABILITY_RDR:
			return("SAHPI_CAPABILITY_RDR");
		case SAHPI_CAPABILITY_SENSOR:
			return("SAHPI_CAPABILITY_SENSOR");
		default:
			return("Unknown Capabilities");
	}
	return("\n");
}

const char * get_sensor_type(SaHpiSensorTypeT type) 
{
	switch(type) {
		case SAHPI_TEMPERATURE:
			return "TEMPERATURE SENSOR";
		case SAHPI_VOLTAGE:
			return "VOLTAGE SENSOR";
		case SAHPI_FAN:
			return "FAN SENSOR";
		case SAHPI_PROCESSOR:
			return "PROCESSOR";
		case SAHPI_POWER_SUPPLY:
			return "POWER SUPPLY";
		case SAHPI_POWER_UNIT:
			return "POWER UNIT";
		case SAHPI_COOLING_DEVICE:
			return "COOLING DEVICE";
		case SAHPI_MEMORY:
			return "MEMORY";
		case SAHPI_OTHER_UNITS_BASED_SENSOR:
			return "OTHER UNITS BASED SENSOR";
		case SAHPI_LAN:
			return "Lan Sensor";
    		case SAHPI_PHYSICAL_SECURITY:
			return("SAHPI_PHYSICAL_SECURITY"); 
    		case SAHPI_PLATFORM_VIOLATION:
			return("SAHPI_PLATFORM_VIOLATION"); 
    		case SAHPI_DRIVE_SLOT:
			return("SAHPI_DRIVE_SLOT"); 
    		case SAHPI_POST_MEMORY_RESIZE:
			return("SAHPI_POST_MEMORY_RESIZE"); 
    		case SAHPI_SYSTEM_FW_PROGRESS:
			return("SAHPI_SYSTEM_FW_PROGRESS"); 
    		case SAHPI_EVENT_LOGGING_DISABLED:
			return("SAHPI_EVENT_LOGGING_DISABLED"); 
    		case SAHPI_RESERVED1:
			return("SAHPI_RESERVED1"); 
    		case SAHPI_SYSTEM_EVENT:
			return("SAHPI_SYSTEM_EVENT"); 
    		case SAHPI_CRITICAL_INTERRUPT:
			return("SAHPI_CRITICAL_INTERRUPT"); 
    		case SAHPI_BUTTON:
			return("SAHPI_BUTTON"); 
    		case SAHPI_MODULE_BOARD:
			return("SAHPI_MODULE_BOARD"); 
    		case SAHPI_MICROCONTROLLER_COPROCESSOR:
			return("SAHPI_MICROCONTROLLER_COPROCESSOR"); 
    		case SAHPI_ADDIN_CARD:
			return("SAHPI_ADDIN_CARD"); 
    		case SAHPI_CHASSIS:
			return("SAHPI_CHASSIS"); 
    		case SAHPI_CHIP_SET:
			return("SAHPI_CHIP_SET"); 
    		case SAHPI_OTHER_FRU:
			return("SAHPI_OTHER_FRU"); 
    		case SAHPI_CABLE_INTERCONNECT:
			return("SAHPI_CABLE_INTERCONNECT"); 
    		case SAHPI_TERMINATOR:
			return("SAHPI_TERMINATOR"); 
    		case SAHPI_SYSTEM_BOOT_INITIATED:
			return("SAHPI_SYSTEM_BOOT_INITIATED"); 
    		case SAHPI_BOOT_ERROR:
			return("SAHPI_BOOT_ERROR"); 
    		case SAHPI_OS_BOOT:
			return("SAHPI_OS_BOOT"); 
    		case SAHPI_OS_CRITICAL_STOP:
			return("SAHPI_OS_CRITICAL_STOP"); 
    		case SAHPI_SLOT_CONNECTOR:
			return("SAHPI_SLOT_CONNECTOR"); 
    		case SAHPI_SYSTEM_ACPI_POWER_STATE:
			return("SAHPI_SYSTEM_ACPI_POWER_STATE"); 
    		case SAHPI_RESERVED2:
			return("SAHPI_RESERVED2"); 
    		case SAHPI_PLATFORM_ALERT:
			return("SAHPI_PLATFORM_ALERT"); 
    		case SAHPI_ENTITY_PRESENCE:
			return("SAHPI_ENTITY_PRESENCE"); 
    		case SAHPI_MONITOR_ASIC_IC:
			return("SAHPI_MONITOR_ASIC_IC"); 
    		case SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH:
			return("SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH"); 
    		case SAHPI_BATTERY:
			return("SAHPI_BATTERY"); 
    		case SAHPI_OEM_SENSOR && SAHPI_OEM_SENSOR == 0xC:
			return("SAHPI_OEM_SENSOR=0xC"); 
		default:
			return "Other";
	}

return "\0";

}

const char * get_sensor_category (SaHpiEventCategoryT category)
{
	switch (category)
	{
		case SAHPI_EC_UNSPECIFIED:
			return "SAHPI_EC_UNSPECIFIED";
		case SAHPI_EC_THRESHOLD:
			return "SAHPI_EC_THRESHOLD"; 
		case SAHPI_EC_USAGE:
			return "SAHPI_EC_USAGE"; 
		case SAHPI_EC_STATE:
			return "SAHPI_EC_STATE"; 
		case SAHPI_EC_PRED_FAIL:
			return "SAHPI_EC_PRED_FAIL"; 
		case SAHPI_EC_LIMIT:
			return "SAHPI_EC_LIMIT";
		case SAHPI_EC_PERFORMANCE:
			return "SAHPI_EC_PERFORMANCE"; 
		case SAHPI_EC_SEVERITY:
		   	return "SAHPI_EC_SEVERITY"; 
		case SAHPI_EC_PRESENCE:
			return "SAHPI_EC_PRESENSE";
		case SAHPI_EC_ENABLE:
			return "SAHPI_EC_ENABLE";
		case SAHPI_EC_AVAILABILITY:
			return "SAHPI_EC_AVAILABILITY";
		case SAHPI_EC_REDUNDANCY:
			return "SAHPI_EC_REDUNDANCY";
		case SAHPI_EC_USER:
			return "SAHPI_EC_USER";
		case SAHPI_EC_GENERIC:
			return "SAHPI_EC_GENERIC";
			
                default:
			return "Unknown Category";
			
	}
	return("\0");
}
