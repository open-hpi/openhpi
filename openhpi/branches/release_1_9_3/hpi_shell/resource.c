/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Aaron  Chen <yukun.chen@intel.com>
 *     Nick   Yin  <hu.yin@intel.com>
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <printevent_utils.h>
#include "hpi_cmd.h"
#include "resource.h"


static void print_rpt_entry(SaHpiRptEntryT rptentry){
	char str[32];

	printf("EntryId: %d\n",rptentry.EntryId);
	printf("ResourceId: %d\n",rptentry.ResourceId);
//	printf("ResourceEntity: ");
//	printf("ResourceInfo:" );
	printf("Resource Capability: %d\n",rptentry.ResourceCapabilities);

	
	severity2str(str,32,rptentry.ResourceSeverity);
	printf("Severity: %s\n",str);
	printf("DomainId: %d\n",rptentry.DomainId);
	printf("ResourceTag: %s\n", (char *)rptentry.ResourceTag.Data);
}

static void print_sensor_rdr(SaHpiSensorRecT rdr){
	char str[32];

	printf("Sensor Num:%d",rdr.Num);

	sensor_type2str(str,32,rdr.Type);
	printf("Sensor Type:%s",str);

	printf("Category:%d",rdr.Category);
	//printf("EventCtrl",rdr.);
	//printf("Events",rdr.);
	//printf("Ignore",rdr.);
	//printf("DataFormat",rdr.);
	//printf("Threshold:",rdr.);
	//printf("OEM",rdr.);
}


static void print_control_rdr(SaHpiCtrlRecT rdr){
	char str[32];
	printf("Control Num:%d\n",rdr.Num);
	
	ctrl_output_type2str(str,32,rdr.OutputType);
	printf("Control Output Type:%s\n",str);

	ctrl_type2str(str,32,rdr.Type);
	printf("Control Type:%s\n",str);

//	printf("Type Union",rdr.);
//	printf("Oem",rdr.);
}

static void print_inventory_rdr(SaHpiInventoryRecT rdr){
	printf("EirId: %d\n",rdr.EirId);
	printf("Oem:%d\n",rdr.Oem);
}

static void print_watchdog_rdr(SaHpiWatchdogRecT rdr){

	printf("WatchdogNum:%d\n",rdr.WatchdogNum);
	printf("Oem:%d\n",rdr.Oem);
}
#if 0
static void rpt_cap2str (SaHpiCapabilitiesT ResourceCapabilities)
{
        printf("\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_DOMAIN)
                printf("\tSAHPI_CAPABILITY_DOMAIN\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_RESOURCE)
                printf("\tSAHPI_CAPABILITY_RESOURCE\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_SEL)
                printf("\tSAHPI_CAPABILITY_SEL\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)
                printf("\tSAHPI_CAPABILITY_EVT_DEASSERTS\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_AGGREGATE_STATUS)
                printf("\tSAHPI_CAPABILITY_AGGREGATE_STATUS\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION)
                printf("\tSAHPI_CAPABILITY_CONFIGURATION\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)
                printf("\tSAHPI_CAPABILITY_MANAGED_HOTSWAP\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)
                printf("\tSAHPI_CAPABILITY_WATCHDOG\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)
                printf("\tSAHPI_CAPABILITY_CONTROL\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_FRU)
                printf("\tSAHPI_CAPABILITY_FRU\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)
                printf("\tSAHPI_CAPABILITY_INVENTORY_DATA\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_RDR)
                printf("\tSAHPI_CAPABILITY_RDR\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)
                printf("\tSAHPI_CAPABILITY_SENSOR\n");

}

static void display_textbuffer(SaHpiTextBufferT string)
{
        int i;
        switch(string.DataType) {
        case SAHPI_TL_TYPE_BINARY:
                for (i = 0; i < string.DataLength; i++)
                        printf("%x", string.Data[i]);
                break;
        case SAHPI_TL_TYPE_BCDPLUS:
                for (i = 0; i < string.DataLength; i++)
                        printf("%c", string.Data[i]);
                break;
        case SAHPI_TL_TYPE_ASCII6:
                for (i = 0; i < string.DataLength; i++)
                        printf("%c", string.Data[i]);
                break;
        case SAHPI_TL_TYPE_LANGUAGE:
                for (i = 0; i < string.DataLength; i++)
                        printf("%c", string.Data[i]);
                break;
        default:
                printf("Invalid string data type=%d", string.DataType);
        }
        printf("\n");
}
#endif

static void print_rdr(SaHpiRdrT rdr){
	switch(rdr.RdrType){
		case SAHPI_CTRL_RDR:
			print_control_rdr(rdr.RdrTypeUnion.CtrlRec);
			break;
		case SAHPI_SENSOR_RDR:
			print_sensor_rdr(rdr.RdrTypeUnion.SensorRec);
			break;
		case SAHPI_INVENTORY_RDR:
			print_inventory_rdr(rdr.RdrTypeUnion.InventoryRec);
			break;
		case SAHPI_WATCHDOG_RDR:
			print_watchdog_rdr(rdr.RdrTypeUnion.WatchdogRec);
			break;
		default:
			printf("Unsupported Rdr Type");
	}
	printf("\n");
}

int sa_list_res(void){

        SaErrorT ret;
        SaHpiRptInfoT rptinfo;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiResourceIdT resourceid;

        ret = saHpiRptInfoGet(sessionid,&rptinfo);
        printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n\n",
               rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
        
        // walk the RPT list 
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((ret == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {
                ret = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                                                
                if (ret == SA_OK) {
                        
                        resourceid = rptentry.ResourceId;
                        //rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0; 
                        /*printf("RPTEntry[%d] tag: %s\n",
                               resourceid,rptentry.ResourceTag.Data);
			*/
			/* print_rpt_entry(rptentry); */
			printf("Resource Id: %d, Resource Tag: %s\n", 
					rptentry.ResourceId, (char *)rptentry.ResourceTag.Data);
                        rptentryid = nextrptentryid;
                }
		else{
			printf("Fail to show RPT\n");
			return -1;
		}

        }

	return 0;
}


int sa_show_rpt(SaHpiResourceIdT resourceid){
	
        SaErrorT ret;
        SaHpiRptEntryT rptentry;

        ret = saHpiRptEntryGetByResourceId(sessionid,resourceid,&rptentry);
        if ( ret ==SA_OK ) 
		print_rpt_entry(rptentry);
	else{
		printf("Fail to show RPT entry of resource %d\n",resourceid); 
		return -1;
	}

	return SA_OK;
}

int sa_show_rdr(SaHpiResourceIdT resourceid){

        SaErrorT               rc = 0;
	SaHpiEntryIdT          entryid, next_entryid;
        SaHpiRdrT              rdr;

        entryid = SAHPI_FIRST_ENTRY;
        while ((rc == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) {
                rc = saHpiRdrGet(sessionid, resourceid, entryid, &next_entryid, &rdr);
                if (rc == SA_OK) {
			print_rdr(rdr);
                        entryid = next_entryid;
                } 
                else {
        	        return -1;
                }
	}
	return SA_OK;
}

int sa_discover(void) 
{
	SaErrorT        ret;
	SaHpiRptInfoT   rpt_info_before;
	SaHpiRptInfoT   rpt_info_after;
	char 		timestr[40];
#if 0
	char            str[32];
	SaHpiEntryIdT   current;
	SaHpiEntryIdT   next;
	SaHpiRptEntryT  entry;
#endif

        ret = saHpiResourcesDiscover(sessionid);
        if (SA_OK != ret) {
                printf("saHpiResourcesDiscover failed\n");
                return ret;
        }
        /* grab copy of the update counter before traversing RPT */
        ret = saHpiRptInfoGet(sessionid, &rpt_info_before);
        if (SA_OK != ret) {
                printf("saHpiRptInfoGet failed\n");
                return ret;
        }
	saftime2str(rpt_info_before.UpdateTimestamp, timestr, 40);
        printf("RptInfo: UpdateCount = %d, UpdateTime = %s\n",
		rpt_info_before.UpdateCount, timestr);

#if 0
        printf("Scanning RPT...\n");
        next = SAHPI_FIRST_ENTRY;
        do {
                current = next;
                ret = saHpiRptEntryGet(sessionid, current, &next, &entry);
                if (SA_OK != ret) {
                        if (current != SAHPI_FIRST_ENTRY) {
                                printf("saHpiRptEntryGet failed\n");
                                return ret;
                        } else {
                                printf("Empty RPT\n");
                                break;
                        }
                }

                printf("***Records:\n");
                printf("%s\n", (char *)entry.ResourceTag.Data);
                printf("Entry ID: %d\n", (int) entry.EntryId);
                printf("Resource ID: %d\n", (int) entry.ResourceId);
                printf("Domain ID: %d\n", (int) entry.DomainId);
                printf("Revision: %c\n", entry.ResourceInfo.ResourceRev);
                printf("Version: %c\n", entry.ResourceInfo.SpecificVer);
                printf("Device Support: %d\n", entry.ResourceInfo.DeviceSupport);
                printf("Manufacturer ID: %d\n", (int) entry.ResourceInfo.ManufacturerId);
                printf("Product ID: %d\n", (int) entry.ResourceInfo.ProductId);
                printf("Firmware Major, Minor, Aux: %d %d %d\n",
                       entry.ResourceInfo.FirmwareMajorRev,
                       entry.ResourceInfo.FirmwareMinorRev,
                       entry.ResourceInfo.AuxFirmwareRev);
		severity2str(str,32,entry.ResourceSeverity);
                printf("Severity: %s\n",str);

                rpt_cap2str(entry.ResourceCapabilities);

		oh_print_ep(&entry.ResourceEntity, 0);
                printf("\tResourceTag: ");
                       display_textbuffer(entry.ResourceTag);

                if (entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
                        sa_show_rdr(entry.ResourceId);

                // if (entry.ResourceCapabilities & SAHPI_CAPABILITY_SEL)
                //         list_sel(sessionid, entry.ResourceId);
                printf("\tEntryId: %d\n", next);
        } while (next != SAHPI_LAST_ENTRY);

        printf("SAHPI_LAST_ENTRY\n");
#endif

        /* wait for update in RPT */
        while (1) {
                ret = saHpiRptInfoGet(sessionid, &rpt_info_after);
                if (SA_OK != ret) {
                        printf("saHpiRptInfoGet failed\n");
                        return ret;
                }
                if (rpt_info_before.UpdateCount != rpt_info_after.UpdateCount) {
                        rpt_info_before = rpt_info_after;
                } else
                        break;
        };

        return SA_OK;
}

struct{
    char *str;
    SaHpiEntityTypeT type;
} entity_types[] = {
     {"SAHPI_ENT_IPMI_GROUP", SAHPI_ENT_IPMI_GROUP},
     {"SAHPI_ENT_UNSPECIFIED", SAHPI_ENT_UNSPECIFIED },
     {"SAHPI_ENT_OTHER", SAHPI_ENT_OTHER},
     {"SAHPI_ENT_UNKNOWN", SAHPI_ENT_UNKNOWN},
     {"SAHPI_ENT_PROCESSOR", SAHPI_ENT_PROCESSOR},
     {"SAHPI_ENT_DISK_BAY", SAHPI_ENT_DISK_BAY},
     {"SAHPI_ENT_PERIPHERAL_BAY", SAHPI_ENT_PERIPHERAL_BAY},
     {"SAHPI_ENT_SYS_MGMNT_MODULE", SAHPI_ENT_SYS_MGMNT_MODULE},
     {"SAHPI_ENT_SYSTEM_BOARD", SAHPI_ENT_SYSTEM_BOARD},
     {"SAHPI_ENT_MEMORY_MODULE", SAHPI_ENT_MEMORY_MODULE},
     {"SAHPI_ENT_PROCESSOR_MODULE", SAHPI_ENT_PROCESSOR_MODULE},
     {"SAHPI_ENT_POWER_SUPPLY", SAHPI_ENT_POWER_SUPPLY},
     {"SAHPI_ENT_ADD_IN_CARD", SAHPI_ENT_ADD_IN_CARD},
     {"SAHPI_ENT_FRONT_PANEL_BOARD", SAHPI_ENT_FRONT_PANEL_BOARD},
     {"SAHPI_ENT_BACK_PANEL_BOARD", SAHPI_ENT_BACK_PANEL_BOARD},
     {"SAHPI_ENT_POWER_SYSTEM_BOARD", SAHPI_ENT_POWER_SYSTEM_BOARD},
     {"SAHPI_ENT_DRIVE_BACKPLANE", SAHPI_ENT_DRIVE_BACKPLANE},
     {"SAHPI_ENT_SYS_EXPANSION_BOARD", SAHPI_ENT_SYS_EXPANSION_BOARD},
     {"SAHPI_ENT_OTHER_SYSTEM_BOARD", SAHPI_ENT_OTHER_SYSTEM_BOARD},
     {"SAHPI_ENT_PROCESSOR_BOARD", SAHPI_ENT_PROCESSOR_BOARD},
     {"SAHPI_ENT_POWER_UNIT", SAHPI_ENT_POWER_UNIT},
     {"SAHPI_ENT_POWER_MODULE", SAHPI_ENT_POWER_MODULE},
     {"SAHPI_ENT_POWER_MGMNT", SAHPI_ENT_POWER_MGMNT},
     {"SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD", SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD},
     {"SAHPI_ENT_SYSTEM_CHASSIS", SAHPI_ENT_SYSTEM_CHASSIS},
     {"SAHPI_ENT_SUB_CHASSIS", SAHPI_ENT_SUB_CHASSIS},
     {"SAHPI_ENT_OTHER_CHASSIS_BOARD", SAHPI_ENT_OTHER_CHASSIS_BOARD},
     {"SAHPI_ENT_DISK_DRIVE_BAY", SAHPI_ENT_DISK_DRIVE_BAY},
     {"SAHPI_ENT_PERIPHERAL_BAY_2", SAHPI_ENT_PERIPHERAL_BAY_2},
     {"SAHPI_ENT_DEVICE_BAY", SAHPI_ENT_DEVICE_BAY},
     {"SAHPI_ENT_COOLING_DEVICE", SAHPI_ENT_COOLING_DEVICE},
     {"SAHPI_ENT_COOLING_UNIT", SAHPI_ENT_COOLING_UNIT},
     {"SAHPI_ENT_INTERCONNECT", SAHPI_ENT_INTERCONNECT},
     {"SAHPI_ENT_MEMORY_DEVICE", SAHPI_ENT_MEMORY_DEVICE},
     {"SAHPI_ENT_SYS_MGMNT_SOFTWARE", SAHPI_ENT_SYS_MGMNT_SOFTWARE},
     {"SAHPI_ENT_BIOS", SAHPI_ENT_BIOS},
     {"SAHPI_ENT_OPERATING_SYSTEM", SAHPI_ENT_OPERATING_SYSTEM},
     {"SAHPI_ENT_SYSTEM_BUS", SAHPI_ENT_SYSTEM_BUS},
     {"SAHPI_ENT_GROUP", SAHPI_ENT_GROUP},
     {"SAHPI_ENT_REMOTE", SAHPI_ENT_REMOTE},
     {"SAHPI_ENT_EXTERNAL_ENVIRONMENT", SAHPI_ENT_EXTERNAL_ENVIRONMENT},
     {"SAHPI_ENT_BATTERY", SAHPI_ENT_BATTERY},
     {"SAHPI_ENT_CHASSIS_SPECIFIC", SAHPI_ENT_CHASSIS_SPECIFIC},
     {"SAHPI_ENT_BOARD_SET_SPECIFIC", SAHPI_ENT_BOARD_SET_SPECIFIC},
     {"SAHPI_ENT_OEM_SYSINT_SPECIFIC", SAHPI_ENT_OEM_SYSINT_SPECIFIC},
     {"SAHPI_ENT_ROOT", SAHPI_ENT_ROOT },
     {"SAHPI_ENT_RACK", SAHPI_ENT_RACK },
     {"SAHPI_ENT_SUBRACK", SAHPI_ENT_SUBRACK},
     {"SAHPI_ENT_COMPACTPCI_CHASSIS", SAHPI_ENT_COMPACTPCI_CHASSIS},
     {"SAHPI_ENT_ADVANCEDTCA_CHASSIS", SAHPI_ENT_ADVANCEDTCA_CHASSIS},
     {"SAHPI_ENT_SYSTEM_SLOT", SAHPI_ENT_SYSTEM_SLOT},
     {"SAHPI_ENT_SBC_BLADE", SAHPI_ENT_SBC_BLADE},
     {"SAHPI_ENT_IO_BLADE", SAHPI_ENT_IO_BLADE},
     {"SAHPI_ENT_DISK_BLADE", SAHPI_ENT_DISK_BLADE},
     {"SAHPI_ENT_DISK_DRIVE", SAHPI_ENT_DISK_DRIVE},
     {"SAHPI_ENT_FAN", SAHPI_ENT_FAN},
     {"SAHPI_ENT_POWER_DISTRIBUTION_UNIT", SAHPI_ENT_POWER_DISTRIBUTION_UNIT},
     {"SAHPI_ENT_SPEC_PROC_BLADE", SAHPI_ENT_SPEC_PROC_BLADE},
     {"SAHPI_ENT_IO_SUBBOARD", SAHPI_ENT_IO_SUBBOARD},
     {"SAHPI_ENT_SBC_SUBBOARD", SAHPI_ENT_SBC_SUBBOARD},
     {"SAHPI_ENT_ALARM_MANAGER", SAHPI_ENT_ALARM_MANAGER},
     {"SAHPI_ENT_ALARM_MANAGER_BLADE", SAHPI_ENT_ALARM_MANAGER_BLADE},
     {"SAHPI_ENT_SUBBOARD_CARRIER_BLADE", SAHPI_ENT_SUBBOARD_CARRIER_BLADE},
};
STR_TO_SAHPI_TYPE_FUN(str2entity_type, SaHpiEntityTypeT, entity_types)
SAHPI_TYPE_TO_STR_FUN(entity_type2str, SaHpiEntityTypeT, entity_types)

struct{
    char *str;
    SaHpiSeverityT  type;
} severity_types[] = {
   {"SAHPI_CRITICAL", SAHPI_CRITICAL},
   {"SAHPI_MAJOR", SAHPI_MAJOR},
   {"SAHPI_MINOR", SAHPI_MINOR},
   {"SAHPI_INFORMATIONAL", SAHPI_INFORMATIONAL},
   {"SAHPI_OK", SAHPI_OK},
   {"SAHPI_DEBUG", SAHPI_DEBUG},
};
STR_TO_SAHPI_TYPE_FUN(str2severity, SaHpiSeverityT, severity_types)
SAHPI_TYPE_TO_STR_FUN(severity2str, SaHpiSeverityT, severity_types)

struct{
    char *str;
    SaHpiRdrTypeT  type;
} rdr_types[] = {
   {"SAHPI_NO_RECORD", SAHPI_NO_RECORD},
   {"SAHPI_CTRL_RDR", SAHPI_CTRL_RDR},
   {"SAHPI_SENSOR_RDR", SAHPI_SENSOR_RDR},
   {"SAHPI_INVENTORY_RDR", SAHPI_INVENTORY_RDR},
   {"SAHPI_WATCHDOG_RDR", SAHPI_WATCHDOG_RDR},
};
STR_TO_SAHPI_TYPE_FUN(str2rdr_type, SaHpiRdrTypeT, rdr_types)

struct{
    char *str;
    SaHpiSensorTypeT  type;
} sensor_types[] = {
   {"SAHPI_TEMPERATURE", SAHPI_TEMPERATURE},
   {"SAHPI_VOLTAGE", SAHPI_VOLTAGE},
   {"SAHPI_CURRENT", SAHPI_CURRENT},
   {"SAHPI_FAN", SAHPI_FAN},
   {"SAHPI_PHYSICAL_SECURITY", SAHPI_PHYSICAL_SECURITY},
   {"SAHPI_PLATFORM_VIOLATION", SAHPI_PLATFORM_VIOLATION},
   {"SAHPI_PROCESSOR", SAHPI_PROCESSOR},
   {"SAHPI_POWER_SUPPLY", SAHPI_POWER_SUPPLY},
   {"SAHPI_POWER_UNIT", SAHPI_POWER_UNIT},
   {"SAHPI_COOLING_DEVICE", SAHPI_COOLING_DEVICE},
   {"SAHPI_OTHER_UNITS_BASED_SENSOR", SAHPI_OTHER_UNITS_BASED_SENSOR},
   {"SAHPI_MEMORY", SAHPI_MEMORY},
   {"SAHPI_DRIVE_SLOT", SAHPI_DRIVE_SLOT},
   {"SAHPI_POST_MEMORY_RESIZE", SAHPI_POST_MEMORY_RESIZE},
   {"SAHPI_SYSTEM_FW_PROGRESS", SAHPI_SYSTEM_FW_PROGRESS},
   {"SAHPI_EVENT_LOGGING_DISABLED", SAHPI_EVENT_LOGGING_DISABLED},
   {"SAHPI_RESERVED1", SAHPI_RESERVED1},
   {"SAHPI_SYSTEM_EVENT", SAHPI_SYSTEM_EVENT},
   {"SAHPI_CRITICAL_INTERRUPT", SAHPI_CRITICAL_INTERRUPT},
   {"SAHPI_BUTTON", SAHPI_BUTTON},
   {"SAHPI_MODULE_BOARD", SAHPI_MODULE_BOARD},
   {"SAHPI_MICROCONTROLLER_COPROCESSOR", SAHPI_MICROCONTROLLER_COPROCESSOR},
   {"SAHPI_ADDIN_CARD", SAHPI_ADDIN_CARD},
   {"SAHPI_CHASSIS", SAHPI_CHASSIS},
   {"SAHPI_CHIP_SET", SAHPI_CHIP_SET},
   {"SAHPI_OTHER_FRU", SAHPI_OTHER_FRU},
   {"SAHPI_CABLE_INTERCONNECT", SAHPI_CABLE_INTERCONNECT},
   {"SAHPI_TERMINATOR", SAHPI_TERMINATOR},
   {"SAHPI_SYSTEM_BOOT_INITIATED", SAHPI_SYSTEM_BOOT_INITIATED},
   {"SAHPI_BOOT_ERROR", SAHPI_BOOT_ERROR},
   {"SAHPI_OS_BOOT", SAHPI_OS_BOOT},
   {"SAHPI_OS_CRITICAL_STOP", SAHPI_OS_CRITICAL_STOP},
   {"SAHPI_SLOT_CONNECTOR", SAHPI_SLOT_CONNECTOR},
   {"SAHPI_SYSTEM_ACPI_POWER_STATE", SAHPI_SYSTEM_ACPI_POWER_STATE},
   {"SAHPI_RESERVED2", SAHPI_RESERVED2},
   {"SAHPI_PLATFORM_ALERT", SAHPI_PLATFORM_ALERT},
   {"SAHPI_ENTITY_PRESENCE", SAHPI_ENTITY_PRESENCE},
   {"SAHPI_MONITOR_ASIC_IC", SAHPI_MONITOR_ASIC_IC},
   {"SAHPI_LAN", SAHPI_LAN},
   {"SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH", SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH},
   {"SAHPI_BATTERY", SAHPI_BATTERY},
   {"SAHPI_OPERATIONAL", SAHPI_OPERATIONAL},
   {"SAHPI_OEM_SENSOR", SAHPI_OEM_SENSOR}, 
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_type, SaHpiSensorTypeT, sensor_types)
SAHPI_TYPE_TO_STR_FUN(sensor_type2str, SaHpiEntityTypeT, entity_types)

struct{
    char *str;
    SaHpiSensorEventCtrlT  type;
} event_ctrl_types[] = {
   {"SAHPI_SEC_PER_EVENT", SAHPI_SEC_PER_EVENT},
   {"SAHPI_SEC_ENTIRE_SENSOR", SAHPI_SEC_ENTIRE_SENSOR},
   {"SAHPI_SEC_GLOBAL_DISABLE", SAHPI_SEC_GLOBAL_DISABLE},
   {"SAHPI_SEC_NO_EVENTS", SAHPI_SEC_NO_EVENTS},    
};
STR_TO_SAHPI_TYPE_FUN(str2event_ctrl_type, SaHpiSensorEventCtrlT, event_ctrl_types)

struct{
    char *str;
    SaHpiSensorSignFormatT  type;
} sign_fmt_types[] = {
   {"SAHPI_SDF_UNSIGNED", SAHPI_SDF_UNSIGNED},
   {"SAHPI_SDF_1S_COMPLEMENT", SAHPI_SDF_1S_COMPLEMENT},
   {"SAHPI_SDF_2S_COMPLEMENT", SAHPI_SDF_2S_COMPLEMENT},
};
STR_TO_SAHPI_TYPE_FUN(str2sign_fmt_type, SaHpiSensorSignFormatT, sign_fmt_types)

struct{
    char *str;
    SaHpiSensorUnitsT  type;
} sensor_unit_types[] = {
   {"SAHPI_SU_UNSPECIFIED", SAHPI_SU_UNSPECIFIED},
   {"SAHPI_SU_DEGREES_C", SAHPI_SU_DEGREES_C},
   {"SAHPI_SU_DEGREES_F", SAHPI_SU_DEGREES_F},
   {"SAHPI_SU_DEGREES_K", SAHPI_SU_DEGREES_K},
   {"SAHPI_SU_VOLTS", SAHPI_SU_VOLTS},
   {"SAHPI_SU_AMPS", SAHPI_SU_AMPS},
   {"SAHPI_SU_WATTS", SAHPI_SU_WATTS},
   {"SAHPI_SU_JOULES", SAHPI_SU_JOULES},
   {"SAHPI_SU_COULOMBS", SAHPI_SU_COULOMBS},
   {"SAHPI_SU_VA", SAHPI_SU_VA},
   {"SAHPI_SU_NITS", SAHPI_SU_NITS},
   {"SAHPI_SU_LUMEN", SAHPI_SU_LUMEN},
   {"SAHPI_SU_LUX", SAHPI_SU_LUX},
   {"SAHPI_SU_CANDELA", SAHPI_SU_CANDELA},
   {"SAHPI_SU_KPA", SAHPI_SU_KPA},
   {"SAHPI_SU_PSI", SAHPI_SU_PSI},
   {"SAHPI_SU_NEWTON", SAHPI_SU_NEWTON},
   {"SAHPI_SU_CFM", SAHPI_SU_CFM},
   {"SAHPI_SU_RPM", SAHPI_SU_RPM},
   {"SAHPI_SU_HZ", SAHPI_SU_HZ},
   {"SAHPI_SU_MICROSECOND", SAHPI_SU_MICROSECOND},
   {"SAHPI_SU_MILLISECOND", SAHPI_SU_MILLISECOND},
   {"SAHPI_SU_SECOND", SAHPI_SU_SECOND},
   {"SAHPI_SU_MINUTE", SAHPI_SU_MINUTE},
   {"SAHPI_SU_HOUR", SAHPI_SU_HOUR},
   {"SAHPI_SU_DAY", SAHPI_SU_DAY},
   {"SAHPI_SU_WEEK", SAHPI_SU_WEEK},
   {"SAHPI_SU_MIL", SAHPI_SU_MIL},
   {"SAHPI_SU_INCHES", SAHPI_SU_INCHES},
   {"SAHPI_SU_FEET", SAHPI_SU_FEET},
   {"SAHPI_SU_CU_IN", SAHPI_SU_CU_IN},
   {"SAHPI_SU_CU_FEET", SAHPI_SU_CU_FEET},
   {"SAHPI_SU_MM", SAHPI_SU_MM},
   {"SAHPI_SU_CM", SAHPI_SU_CM},
   {"SAHPI_SU_M", SAHPI_SU_M},
   {"SAHPI_SU_CU_CM", SAHPI_SU_CU_CM},
   {"SAHPI_SU_CU_M", SAHPI_SU_CU_M},
   {"SAHPI_SU_LITERS", SAHPI_SU_LITERS},
   {"SAHPI_SU_FLUID_OUNCE", SAHPI_SU_FLUID_OUNCE},
   {"SAHPI_SU_RADIANS", SAHPI_SU_RADIANS},
   {"SAHPI_SU_STERADIANS", SAHPI_SU_STERADIANS},
   {"SAHPI_SU_REVOLUTIONS", SAHPI_SU_REVOLUTIONS},
   {"SAHPI_SU_CYCLES", SAHPI_SU_CYCLES},
   {"SAHPI_SU_GRAVITIES", SAHPI_SU_GRAVITIES},
   {"SAHPI_SU_OUNCE", SAHPI_SU_OUNCE},
   {"SAHPI_SU_POUND", SAHPI_SU_POUND},
   {"SAHPI_SU_FT_LB", SAHPI_SU_FT_LB},
   {"SAHPI_SU_OZ_IN", SAHPI_SU_OZ_IN},
   {"SAHPI_SU_GAUSS", SAHPI_SU_GAUSS},
   {"SAHPI_SU_GILBERTS", SAHPI_SU_GILBERTS},
   {"SAHPI_SU_HENRY", SAHPI_SU_HENRY},
   {"SAHPI_SU_MILLIHENRY", SAHPI_SU_MILLIHENRY},
   {"SAHPI_SU_FARAD", SAHPI_SU_FARAD},
   {"SAHPI_SU_MICROFARAD", SAHPI_SU_MICROFARAD},
   {"SAHPI_SU_OHMS", SAHPI_SU_OHMS},
   {"SAHPI_SU_SIEMENS", SAHPI_SU_SIEMENS},
   {"SAHPI_SU_MOLE", SAHPI_SU_MOLE},
   {"SAHPI_SU_BECQUEREL", SAHPI_SU_BECQUEREL},
   {"SAHPI_SU_PPM", SAHPI_SU_PPM},
   {"SAHPI_SU_RESERVED", SAHPI_SU_RESERVED},
   {"SAHPI_SU_DECIBELS", SAHPI_SU_DECIBELS},
   {"SAHPI_SU_DBA", SAHPI_SU_DBA},
   {"SAHPI_SU_DBC", SAHPI_SU_DBC},
   {"SAHPI_SU_GRAY", SAHPI_SU_GRAY},
   {"SAHPI_SU_SIEVERT", SAHPI_SU_SIEVERT},
   {"SAHPI_SU_COLOR_TEMP_DEG_K", SAHPI_SU_COLOR_TEMP_DEG_K},
   {"SAHPI_SU_BIT", SAHPI_SU_BIT},
   {"SAHPI_SU_KILOBIT", SAHPI_SU_KILOBIT},
   {"SAHPI_SU_MEGABIT", SAHPI_SU_MEGABIT},
   {"SAHPI_SU_GIGABIT", SAHPI_SU_GIGABIT},
   {"SAHPI_SU_BYTE", SAHPI_SU_BYTE},
   {"SAHPI_SU_KILOBYTE", SAHPI_SU_KILOBYTE},
   {"SAHPI_SU_MEGABYTE", SAHPI_SU_MEGABYTE},
   {"SAHPI_SU_GIGABYTE", SAHPI_SU_GIGABYTE},
   {"SAHPI_SU_WORD", SAHPI_SU_WORD},
   {"SAHPI_SU_DWORD", SAHPI_SU_DWORD},
   {"SAHPI_SU_QWORD", SAHPI_SU_QWORD},
   {"SAHPI_SU_LINE", SAHPI_SU_LINE},
   {"SAHPI_SU_HIT", SAHPI_SU_HIT},
   {"SAHPI_SU_MISS", SAHPI_SU_MISS},
   {"SAHPI_SU_RETRY", SAHPI_SU_RETRY},
   {"SAHPI_SU_RESET", SAHPI_SU_RESET},
   {"SAHPI_SU_OVERRUN", SAHPI_SU_OVERRUN},
   {"SAHPI_SU_UNDERRUN", SAHPI_SU_UNDERRUN},
   {"SAHPI_SU_COLLISION", SAHPI_SU_COLLISION},
   {"SAHPI_SU_PACKETS", SAHPI_SU_PACKETS},
   {"SAHPI_SU_MESSAGES", SAHPI_SU_MESSAGES},
   {"SAHPI_SU_CHARACTERS", SAHPI_SU_CHARACTERS},
   {"SAHPI_SU_ERRORS", SAHPI_SU_ERRORS},
   {"SAHPI_SU_CORRECTABLE_ERRORS", SAHPI_SU_CORRECTABLE_ERRORS},
   {"SAHPI_SU_UNCORRECTABLE_ERRORS", SAHPI_SU_UNCORRECTABLE_ERRORS}, 
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_unit, SaHpiSensorUnitsT, sensor_unit_types)

struct{
    char *str;
    SaHpiSensorModUnitUseT  type;
} unit_uses[] = {
   {"SAHPI_SMUU_NONE", SAHPI_SMUU_NONE},
   {"SAHPI_SMUU_BASIC_OVER_MODIFIER", SAHPI_SMUU_BASIC_OVER_MODIFIER},
   {"SAHPI_SMUU_BASIC_TIMES_MODIFIER", SAHPI_SMUU_BASIC_TIMES_MODIFIER},
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_mod_unit_use, SaHpiSensorModUnitUseT, unit_uses)

struct{
    char *str;
    SaHpiSensorLinearizationT type;
} sensor_line_types[] = {
   {"SAHPI_SL_LINEAR", SAHPI_SL_LINEAR},
   {"SAHPI_SL_LN", SAHPI_SL_LN},
   {"SAHPI_SL_LOG10", SAHPI_SL_LOG10},
   {"SAHPI_SL_LOG2", SAHPI_SL_LOG2},
   {"SAHPI_SL_E", SAHPI_SL_E},
   {"SAHPI_SL_EXP10", SAHPI_SL_EXP10},
   {"SAHPI_SL_EXP2", SAHPI_SL_EXP2},
   {"SAHPI_SL_1OVERX", SAHPI_SL_1OVERX},
   {"SAHPI_SL_SQRX", SAHPI_SL_SQRX},
   {"SAHPI_SL_CUBEX", SAHPI_SL_CUBEX},
   {"SAHPI_SL_SQRTX", SAHPI_SL_SQRTX},
   {"SAHPI_SL_CUBERTX", SAHPI_SL_CUBERTX},
   {"SAHPI_SL_NONLINEAR", SAHPI_SL_NONLINEAR},
   {"SAHPI_SL_OEM", SAHPI_SL_OEM},
   {"SAHPI_SL_UNSPECIFIED", SAHPI_SL_UNSPECIFIED},
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_line, SaHpiSensorLinearizationT, 
                      sensor_line_types)

struct{
    char *str;
    SaHpiSensorInterpretedTypeT type;
} interpreted_types[] = {
   {"SAHPI_SENSOR_INTERPRETED_TYPE_UINT8",
    SAHPI_SENSOR_INTERPRETED_TYPE_UINT8},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_UINT16",
    SAHPI_SENSOR_INTERPRETED_TYPE_UINT16},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_UINT32",
    SAHPI_SENSOR_INTERPRETED_TYPE_UINT32},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_INT8",
    SAHPI_SENSOR_INTERPRETED_TYPE_INT8},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_INT16",
    SAHPI_SENSOR_INTERPRETED_TYPE_INT16},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_INT32",
    SAHPI_SENSOR_INTERPRETED_TYPE_INT32},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32",
    SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER",
    SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER}, 
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_interpreted_type, SaHpiSensorInterpretedTypeT, 
                      interpreted_types)

SAHPI_TYPE_TO_STR_FUN(sensor_interpreted_type2str, SaHpiSensorInterpretedTypeT, 
                      interpreted_types)

struct{
	char *str;
	SaHpiCtrlOutputTypeT type;
}ctrl_output_types[] = {
   {"SAHPI_CTRL_GENERIC",
    SAHPI_CTRL_GENERIC},
   {"SAHPI_CTRL_LED",
    SAHPI_CTRL_LED},
   {"SAHPI_CTRL_FAN_SPEED",
    SAHPI_CTRL_FAN_SPEED},
   {"SAHPI_CTRL_DRY_CONTACT_CLOSURE",
    SAHPI_CTRL_DRY_CONTACT_CLOSURE},
   {"SAHPI_CTRL_POWER_SUPPLY_INHIBIT",
    SAHPI_CTRL_POWER_SUPPLY_INHIBIT},
   {"SAHPI_CTRL_AUDIBLE",
    SAHPI_CTRL_AUDIBLE},
   {"SAHPI_CTRL_FRONT_PANEL_LOCKOUT",
    SAHPI_CTRL_FRONT_PANEL_LOCKOUT},
   {"SAHPI_CTRL_POWER_INTERLOCK",
    SAHPI_CTRL_POWER_INTERLOCK},
   {"SAHPI_CTRL_POWER_STATE",
    SAHPI_CTRL_POWER_STATE},
   {"SAHPI_CTRL_LCD_DISPLAY",
    SAHPI_CTRL_LCD_DISPLAY},
   {"SAHPI_CTRL_OEM",
    SAHPI_CTRL_OEM},
};
STR_TO_SAHPI_TYPE_FUN(str2ctrl_output_type, SaHpiCtrlOutputTypeT, 
                      ctrl_output_types)

SAHPI_TYPE_TO_STR_FUN(ctrl_output_type2str, SaHpiCtrlOutputTypeT, 
                      ctrl_output_types)


struct{
	char *str;
	SaHpiCtrlTypeT type;
}ctrl_types[] = {
   {"SAHPI_CTRL_TYPE_DIGITAL",
    SAHPI_CTRL_TYPE_DIGITAL},
   {"SAHPI_CTRL_TYPE_DISCRETE",
    SAHPI_CTRL_TYPE_DISCRETE},
   {"SAHPI_CTRL_TYPE_ANALOG",
    SAHPI_CTRL_TYPE_ANALOG},
   {"SAHPI_CTRL_TYPE_STREAM",
    SAHPI_CTRL_TYPE_STREAM},
   {"SAHPI_CTRL_TYPE_TEXT",
    SAHPI_CTRL_TYPE_TEXT},
   {"SAHPI_CTRL_TYPE_OEM",
    SAHPI_CTRL_TYPE_OEM},
};
STR_TO_SAHPI_TYPE_FUN(str2ctrl_type, SaHpiCtrlTypeT, 
                      ctrl_types)

SAHPI_TYPE_TO_STR_FUN(ctrl_type2str, SaHpiCtrlTypeT, 
                      ctrl_types)
