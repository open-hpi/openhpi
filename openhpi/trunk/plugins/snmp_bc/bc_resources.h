/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <http://dague.net/sean>
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

/**************************************************************************
 * This header file stubs resource and RDR static infomation that are used
 * in constructing RPT and RDR for IBM's BladeCenter - models Integrated
 * and Telco.
 *
 * The BladeCenter Integrated model has the following entity hierarchy:
 *
 *  {CHASSIS,X}
 *      |
 *      +-- {SYS_MGMNT_MODULE,[1-2]} (Management Module)
 *      |
 *      +-- {INTERCONNECT,[1-4]}     (Switch Module)
 *      |
 *      +-- {SBC_BLADE,[1-14]}       (Blade)
 *      |   |
 *      |   +-- {ADD_IN_CARD,[1-14]} (Blade expansion card (BSE card))
 *      |                            (Expansion card number match's)
 *      |                            (it's parent blade's number)
 *      |
 *      +-- {PERIPHERAL_BAY,1}       (Control Panel/Media Tray)
 *      |
 *      +-- {POWER_SUPPLY,[1-4]}     (Power Module)
 *      |
 *      +-- {FAN,[1-2]}              (Blower Module)
 *
 * BladeCenter Telco has the same hierarchy and number of devices except:
 *
 *   - Telco supports 4 blower modules (instead of 2)
 *   - Telco supports 8 blades (instead of 14)
 *
 * Differences between the BladeCenter models are discovered dynamically
 * by this plugin at run-time during resource discovery.
 *************************************************************************/

#ifndef __BC_RESOURCES_H
#define __BC_RESOURCES_H

/* HPI Spec dependencies */
#define ELEMENTS_IN_SaHpiStateDigitalT 5

/* IBM Manufacturing Number */
#define IBM_MANUFACTURING_ID 2

/* BladeCenter types - run-time support for platform-specific processing */
#define SNMP_BC_BLADECENTER_TYPE ".1.3.6.1.4.1.2.3.51.2.2.21.1.1.1.0"
#define SNMP_BC_BCE_MODEL "8677"
#define SNMP_BC_BCT_MODEL_AC "8730"
#define SNMP_BC_BCT_MODEL_DC "8720"
#define SNMP_BC_PLATFORM_BCT "BCT"
#define SNMP_BC_PLATFORM_BC  "BC"

/* Resource indexes to snmp_rpt array in bc_resources.c */
typedef enum {
        BC_RPT_ENTRY_CHASSIS = 0,
        BC_RPT_ENTRY_MGMNT_MODULE,
        BC_RPT_ENTRY_SWITCH_MODULE,
        BC_RPT_ENTRY_BLADE,
        BC_RPT_ENTRY_BLADE_ADDIN_CARD,
        BC_RPT_ENTRY_MEDIA_TRAY,
        BC_RPT_ENTRY_BLOWER_MODULE,
        BC_RPT_ENTRY_POWER_MODULE
} BCRptEntryT;

/* Start HPI Instance numbers from 1 */
#define BC_HPI_INSTANCE_BASE 1

/* Character for blanking out normalized oid elements */
#define OID_BLANK_CHAR 'x'
#define OID_BLANK_STR "x"

/* OID definitions for discovering resources.*/
#define SNMP_BC_BLADE_VECTOR        ".1.3.6.1.4.1.2.3.51.2.2.5.2.49.0"
#define SNMP_BC_BLADE_ADDIN_VECTOR  ".1.3.6.1.4.1.2.3.51.2.22.1.5.1.1.14.x"
#define SNMP_BC_FAN_VECTOR          ".1.3.6.1.4.1.2.3.51.2.2.5.2.73.0"
#define SNMP_BC_MGMNT_VECTOR        ".1.3.6.1.4.1.2.3.51.2.22.4.30.0"
#define SNMP_BC_MGMNT_ACTIVE        ".1.3.6.1.4.1.2.3.51.2.22.4.34.0"
#define SNMP_BC_MEDIATRAY_EXISTS    ".1.3.6.1.4.1.2.3.51.2.2.5.2.74.0"
#define SNMP_BC_POWER_VECTOR        ".1.3.6.1.4.1.2.3.51.2.2.5.2.89.0"
#define SNMP_BC_SWITCH_VECTOR       ".1.3.6.1.4.1.2.3.51.2.2.5.2.113.0"
#define SNMP_BC_TIME_DST            ".1.3.6.1.4.1.2.3.51.2.4.4.2.0"


/* OID definitions for System Event Log */
#define BC_DATETIME_OID ".1.3.6.1.4.1.2.3.51.2.4.4.1.0"
#define BC_SEL_INDEX_OID ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.1"
#define BC_SEL_ENTRY_OID ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.2"
#define BC_CLEAR_SEL_OID ".1.3.6.1.4.1.2.3.51.2.3.4.3.0"

/* OID definitions for System Health and Summary */
#define BC_TELCO_SYSTEM_HEALTH_STAT_OID ".1.3.6.1.4.1.2.3.51.2.2.9.1.0"

/*************************************************************************
 *                   Resource Definitions
 *************************************************************************/

struct BC_ResourceMibInfo {
        const char *OidHealth;
        int   HealthyValue;
        const char *OidReset;
        const char *OidPowerState;
        const char *OidPowerOnOff;
};

#define MAX_EVENTS_PER_RESOURCE 10
#define MAX_RESOURCE_EVENT_ARRAY_SIZE  (MAX_EVENTS_PER_RESOURCE + 1)
                                       /* Includes an ending NULL entry */
struct res_event_map {
        char *event;
        SaHpiHsStateT event_state;
        SaHpiHsStateT recovery_state;
};

struct BC_ResourceInfo {
        struct BC_ResourceMibInfo mib;
        SaHpiHsStateT cur_state;
        struct res_event_map event_array[MAX_RESOURCE_EVENT_ARRAY_SIZE];
};

struct snmp_rpt {
        SaHpiRptEntryT rpt;
        struct BC_ResourceInfo bc_res_info;
        const  char *comment;
};

extern struct snmp_rpt snmp_rpt_array[];

/******************************************************************************
 *                      Sensor Definitions
 ******************************************************************************/

struct SNMPRawThresholdsOIDs {
        const char *OidLowMinor;
        const char *OidLowMajor;
        const char *OidLowCrit;
        const char *OidUpMinor;
        const char *OidUpMajor;
        const char *OidUpCrit;
        const char *OidLowHysteresis;
        const char *OidUpHysteresis;
};

struct SNMPInterpretedThresholdsOIDs {
        const char *OidLowMinor;
        const char *OidLowMajor;
        const char *OidLowCrit;
        const char *OidUpMinor;
        const char *OidUpMajor;
        const char *OidUpCrit;
        const char *OidLowHysteresis;
        const char *OidUpHysteresis;
};

struct SnmpSensorThresholdOids {
        struct SNMPRawThresholdsOIDs RawThresholds;
        struct SNMPInterpretedThresholdsOIDs InterpretedThresholds;
};

struct BC_SensorMibInfo {
        unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
        int write_only; /* Write-only SNMP command; 0 no; 1 yes  */
        int convert_snmpstr; /* -1 no conversion; else use SaHpiSensorInterpretedTypeT values */
        const char *oid;
        struct SnmpSensorThresholdOids threshold_oids;
};

#define MAX_EVENTS_PER_SENSOR 15
#define MAX_SENSOR_EVENT_ARRAY_SIZE  (MAX_EVENTS_PER_SENSOR + 1)
                                     /* Includes an ending NULL entry */

struct sensor_event_map {
        char *event;
        SaHpiEventStateT event_state;
        SaHpiEventStateT recovery_state;
};

struct BC_SensorInfo {
        struct BC_SensorMibInfo mib;
        SaHpiEventStateT cur_state;
        SaHpiSensorEvtEnablesT sensor_evt_enablement;
        struct sensor_event_map event_array[MAX_SENSOR_EVENT_ARRAY_SIZE];
};

struct snmp_bc_sensor {
        SaHpiSensorRecT sensor;
        struct BC_SensorInfo bc_sensor_info;
        const char *comment;
};

extern struct snmp_bc_sensor snmp_bc_chassis_sensors[];
extern struct snmp_bc_sensor snmp_bc_blade_sensors[];
extern struct snmp_bc_sensor snmp_bc_blade_addin_sensors[];
extern struct snmp_bc_sensor snmp_bc_mgmnt_sensors[];
extern struct snmp_bc_sensor snmp_bc_mediatray_sensors[];
extern struct snmp_bc_sensor snmp_bc_fan_sensors[];
extern struct snmp_bc_sensor snmp_bc_power_sensors[];
extern struct snmp_bc_sensor snmp_bc_switch_sensors[];

/*************************************************************************
 *                   Control Definitions
 *************************************************************************/
struct BC_ControlMibInfo {
        unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
        int write_only; /* Write-only SNMP command; 0 no; 1 yes  */
        const char *oid;
        int digitalmap[ELEMENTS_IN_SaHpiStateDigitalT];
};

struct BC_ControlInfo {
        struct BC_ControlMibInfo mib;
};

struct snmp_bc_control {
        SaHpiCtrlRecT control;
        struct BC_ControlInfo bc_control_info;
        const char *comment;
};

extern struct snmp_bc_control snmp_bc_chassis_controls[];
extern struct snmp_bc_control snmp_bc_blade_controls[];
extern struct snmp_bc_control snmp_bct_blade_controls[];
extern struct snmp_bc_control snmp_bc_blade_addin_controls[];
extern struct snmp_bc_control snmp_bc_mgmnt_controls[];
extern struct snmp_bc_control snmp_bc_mediatray_controls[];
extern struct snmp_bc_control snmp_bc_fan_controls[];
extern struct snmp_bc_control snmp_bc_power_controls[];
extern struct snmp_bc_control snmp_bc_switch_controls[];

/*************************************************************************
 *                   Inventory Definitions
 *************************************************************************/
struct SnmpInventoryOids {
        const char *OidMfgDateTime;
        const char *OidManufacturer;
        const char *OidProductName;
        const char *OidProductVersion;
        const char *OidModelNumber;
        const char *OidSerialNumber;
        const char *OidPartNumber;
        const char *OidFileId;
        const char *OidAssetTag;
};

struct BC_InventoryMibInfo {
        unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
        int write_only; /* Write-only SNMP command; 0 no; 1 yes  */
        SaHpiInventDataRecordTypeT  inventory_type;
        SaHpiInventChassisTypeT chassis_type; /* Ignore if inventory_type not CHASSIS */
        struct SnmpInventoryOids oid;
};

struct BC_InventoryInfo {
        struct BC_InventoryMibInfo mib;
};

struct snmp_bc_inventory {
        SaHpiInventoryRecT  inventory;
        struct BC_InventoryInfo bc_inventory_info;
        const char *comment;
};

extern struct snmp_bc_inventory snmp_bc_chassis_inventories[];
extern struct snmp_bc_inventory snmp_bc_fan_inventories[];
extern struct snmp_bc_inventory snmp_bc_mgmnt_inventories[];
extern struct snmp_bc_inventory snmp_bc_switch_inventories[];
extern struct snmp_bc_inventory snmp_bc_blade_inventories[];
extern struct snmp_bc_inventory snmp_bc_blade_addin_inventories[];
extern struct snmp_bc_inventory snmp_bc_mediatray_inventories[];
extern struct snmp_bc_inventory snmp_bc_power_inventories[];

#endif
