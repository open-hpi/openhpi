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
 * and Telco and IBM's RSA (Integrated and separate adapter models).
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
 * The RSA Integrated model has the following entity hierarchy:
 *
 *  {CHASSIS,X}
 *      |
 *      +-- {PROCESSOR,[1-8]}        (CPUs)
 *      |
 *      +-- {DISK_BAY,[1-4]}         (DASDs)
 *      |
 *      +-- {FAN,[1-8]}              (Fans)
 *
 * Differences between the models are discovered dynamically by this
 * plugin at run-time during resource discovery.
 *************************************************************************/

#ifndef __BC_RESOURCES_H
#define __BC_RESOURCES_H

/* Start HPI location numbers from 1 */
#define SNMP_BC_HPI_LOCATION_BASE 1

/* IBM Manufacturing Number */
#define IBM_MANUFACTURING_ID 2

/* Maximum OID string length */
#define SNMP_BC_MAX_OID_LENGTH 50

/* OIDs to determine platform types */
#define SNMP_BC_PLATFORM_OID_BC   ".1.3.6.1.4.1.2.3.51.2.2.7.1.0" /* BCI System Health */
#define SNMP_BC_PLATFORM_OID_BCT  ".1.3.6.1.4.1.2.3.51.2.2.9.1.0" /* BCT System Health */
#define SNMP_BC_PLATFORM_OID_RSA  ".1.3.6.1.4.1.2.3.51.1.2.7.1.0" /* RSA System Health */

/* Run-time variables to distinguish platform types */
#define SNMP_BC_PLATFORM_BCT   0x0001
#define SNMP_BC_PLATFORM_BC    0x0002
#define SNMP_BC_PLATFORM_RSA   0x0004
#define SNMP_BC_PLATFORM_ALL   0xFFFF

/* Resource indexes to snmp_rpt array in discovery */
#define BCT_RPT_ENTRY_CHASSIS 0

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

typedef enum {
	RSA_RPT_ENTRY_CHASSIS = 0,
	RSA_RPT_ENTRY_CPU,
	RSA_RPT_ENTRY_DASD,
	RSA_RPT_ENTRY_FAN
} RSARptEntryT;

/* Maximum number of RSA resources */
#define RSA_MAX_CPU    8
#define RSA_MAX_FAN    8
#define RSA_MAX_DASD   4

/* Maximum entries in eventlog, approximate */
#define BC_EL_MAX_SIZE 768 /* 512 */

/* OID definitions for discovering resources.*/
#define SNMP_BC_BLADE_VECTOR        ".1.3.6.1.4.1.2.3.51.2.2.5.2.49.0"
#define SNMP_BC_BLADE_ADDIN_VECTOR  ".1.3.6.1.4.1.2.3.51.2.22.1.5.1.1.14.x"
#define SNMP_BC_FAN_VECTOR          ".1.3.6.1.4.1.2.3.51.2.2.5.2.73.0"
#define SNMP_BC_MGMNT_VECTOR        ".1.3.6.1.4.1.2.3.51.2.22.4.30.0"
#define SNMP_BC_MGMNT_ACTIVE        ".1.3.6.1.4.1.2.3.51.2.22.4.34.0"
#define SNMP_BC_MEDIATRAY_EXISTS    ".1.3.6.1.4.1.2.3.51.2.2.5.2.74.0"
#define SNMP_BC_POWER_VECTOR        ".1.3.6.1.4.1.2.3.51.2.2.5.2.89.0"
#define SNMP_BC_SWITCH_VECTOR       ".1.3.6.1.4.1.2.3.51.2.2.5.2.113.0"
#define SNMP_BC_DST                 ".1.3.6.1.4.1.2.3.51.2.4.4.2.0"
#define SNMP_BC_DST_RSA             ".1.3.6.1.4.1.2.3.51.1.4.4.2.0"
#define SNMP_BC_CPU_OID_RSA         ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.3.x"
#define SNMP_BC_DASD_OID_RSA        ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.3.x"
#define SNMP_BC_FAN_OID_RSA         ".1.3.6.1.4.1.2.3.51.1.2.3.x.0"

/* OID definitions for System Event Log */
#define SNMP_BC_DATETIME_OID      ".1.3.6.1.4.1.2.3.51.2.4.4.1.0"
#define SNMP_BC_DATETIME_OID_RSA  ".1.3.6.1.4.1.2.3.51.1.4.4.1.0"
#define SNMP_BC_SEL_INDEX_OID     ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.1"
#define SNMP_BC_SEL_INDEX_OID_RSA ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.1"
#define SNMP_BC_SEL_ENTRY_OID     ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.2"
#define SNMP_BC_SEL_ENTRY_OID_RSA ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.2"
#define SNMP_BC_SEL_CLEAR_OID     ".1.3.6.1.4.1.2.3.51.2.3.4.3.0"
#define SNMP_BC_SEL_CLEAR_OID_RSA ".1.3.6.1.4.1.2.3.51.1.3.4.3.0"

/**********************
 * Resource Definitions
 **********************/

struct ResourceMibInfo {
        const char *OidHealth;
        int   HealthyValue;
        const char *OidReset;
        const char *OidPowerState;
        const char *OidPowerOnOff;
	const char *OidUuid;
};

#define SNMP_BC_MAX_EVENTS_PER_RESOURCE 10
#define SNMP_BC_MAX_RESOURCE_EVENT_ARRAY_SIZE  (SNMP_BC_MAX_EVENTS_PER_RESOURCE + 1)
                                       /* Includes an ending NULL entry */
struct res_event_map {
        char *event;
	SaHpiBoolT event_res_failure;
	SaHpiBoolT event_res_failure_unexpected;
        SaHpiHsStateT event_state;
        SaHpiHsStateT recovery_state;
};

struct ResourceInfo {
        struct ResourceMibInfo mib;
        SaHpiHsStateT cur_state;
        struct res_event_map event_array[SNMP_BC_MAX_RESOURCE_EVENT_ARRAY_SIZE];
};

struct snmp_rpt {
        SaHpiRptEntryT rpt;
        struct ResourceInfo res_info;
        const  char *comment;
};

extern struct snmp_rpt snmp_bc_rpt_array[];
extern struct snmp_rpt snmp_bc_rpt_array_bct[];
extern struct snmp_rpt snmp_bc_rpt_array_rsa[];

/********************
 * Sensor Definitions
 ********************/

struct SnmpSensorThresholdOids {
        const char *LowMinor;
        const char *LowMajor;
        const char *LowCritical;
        const char *UpMinor;
        const char *UpMajor;
        const char *UpCritical;
        const char *PosThdHysteresis;
        const char *NegThdHysteresis;
        const char *TotalPosThdHysteresis;
        const char *TotalNegThdHysteresis;
};

struct SnmpSensorWritableThresholdOids {
        const char *LowMinor;
        const char *LowMajor;
        const char *LowCritical;
        const char *UpMinor;
        const char *UpMajor;
        const char *UpCritical;
        const char *PosThdHysteresis;
        const char *NegThdHysteresis;
};

struct SensorMibInfo {
        unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
        SaHpiBoolT write_only; /* TRUE - Write-only SNMP command */
        const char *oid;
        struct SnmpSensorThresholdOids threshold_oids;
	struct SnmpSensorWritableThresholdOids threshold_write_oids;
};

#define SNMP_BC_MAX_EVENTS_PER_SENSOR 24
#define SNMP_BC_MAX_READING_MAPS_PER_SENSOR 3

/* Includes an ending NULL entry */
#define SNMP_BC_MAX_SENSOR_EVENT_ARRAY_SIZE  (SNMP_BC_MAX_EVENTS_PER_SENSOR + 1)
#define SNMP_BC_MAX_SENSOR_READING_MAP_ARRAY_SIZE (SNMP_BC_MAX_READING_MAPS_PER_SENSOR + 1)

/* If you add to this structure, you may also have to change EventMapInfoT 
   and event discovery in snmp_bc_event.c */
struct sensor_event_map {
        char *event;
	SaHpiBoolT event_assertion;
	SaHpiBoolT event_res_failure;
	SaHpiBoolT event_res_failure_unexpected;
        SaHpiEventStateT event_state;
        SaHpiEventStateT recovery_state;
};

struct sensor_reading_map {
	int num;
	SaHpiSensorRangeT rangemap;
	SaHpiEventStateT state;
};

struct SensorInfo {
        struct SensorMibInfo mib;
        SaHpiEventStateT cur_state; /* This really records the last state read from the SEL */
	                            /* Which probably isn't the current state of the sensor */
	SaHpiBoolT sensor_enabled;
	SaHpiBoolT events_enabled;
        SaHpiEventStateT assert_mask;
	SaHpiEventStateT deassert_mask;
        struct sensor_event_map event_array[SNMP_BC_MAX_SENSOR_EVENT_ARRAY_SIZE];
	struct sensor_reading_map reading2event[SNMP_BC_MAX_SENSOR_READING_MAP_ARRAY_SIZE];
};

struct snmp_bc_sensor {
        /* Usually sensor.Num = index; index is used to search thru sensor arrays. It allows
           sensor.Num to be independent from array index (e.g. for aggregate sensors) */
	int index;
        SaHpiSensorRecT sensor;
        struct SensorInfo sensor_info;
        const char *comment;
};

struct snmp_bc_ipmi_sensor {
	const char *ipmi_tag;
	struct snmp_bc_sensor ipmi;
};

extern struct snmp_bc_sensor snmp_bc_chassis_sensors[];
extern struct snmp_bc_sensor snmp_bc_chassis_sensors_bct[];
extern struct snmp_bc_sensor snmp_bc_blade_sensors[];
extern struct snmp_bc_ipmi_sensor snmp_bc_blade_ipmi_sensors[];
extern struct snmp_bc_sensor snmp_bc_blade_addin_sensors[];
extern struct snmp_bc_sensor snmp_bc_mgmnt_sensors[];
extern struct snmp_bc_sensor snmp_bc_mediatray_sensors[];
extern struct snmp_bc_sensor snmp_bc_fan_sensors[];
extern struct snmp_bc_sensor snmp_bc_power_sensors[];
extern struct snmp_bc_sensor snmp_bc_switch_sensors[];

extern struct snmp_bc_sensor snmp_bc_chassis_sensors_rsa[];
extern struct snmp_bc_sensor snmp_bc_cpu_sensors_rsa[];
extern struct snmp_bc_sensor snmp_bc_dasd_sensors_rsa[];
extern struct snmp_bc_sensor snmp_bc_fan_sensors_rsa[];

/*********************
 * Control Definitions
 *********************/

struct ControlMibInfo {
        unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
        int write_only; /* Write-only SNMP command; 0 no; 1 yes  */
        const char *oid;
};

struct ControlInfo {
        struct ControlMibInfo mib;
	SaHpiCtrlModeT cur_mode;
};

struct snmp_bc_control {
        SaHpiCtrlRecT control;
        struct ControlInfo control_info;
        const char *comment;
};

extern struct snmp_bc_control snmp_bc_chassis_controls_bc[];
extern struct snmp_bc_control snmp_bc_chassis_controls_bct[];
extern struct snmp_bc_control snmp_bc_blade_controls[];
extern struct snmp_bc_control snmp_bc_blade_addin_controls[];
extern struct snmp_bc_control snmp_bc_mgmnt_controls[];
extern struct snmp_bc_control snmp_bc_mediatray_controls[];
extern struct snmp_bc_control snmp_bc_fan_controls[];
extern struct snmp_bc_control snmp_bc_power_controls[];
extern struct snmp_bc_control snmp_bc_switch_controls[];

extern struct snmp_bc_control snmp_bc_chassis_controls_rsa[];
extern struct snmp_bc_control snmp_bc_cpu_controls_rsa[];
extern struct snmp_bc_control snmp_bc_dasd_controls_rsa[];
extern struct snmp_bc_control snmp_bc_fan_controls_rsa[];

/***********************
 * Inventory Definitions
 ***********************/

struct SnmpInventoryOids {
        const char *OidChassisType;
        const char *OidMfgDateTime;
        const char *OidManufacturer;
        const char *OidProductName;
        const char *OidProductVersion;
        const char *OidSerialNumber;
        const char *OidPartNumber;
        const char *OidFileId;
        const char *OidAssetTag;
};

struct InventoryMibInfo {
        unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
        int write_only; /* Write-only SNMP command; 0 no; 1 yes  */
        SaHpiIdrAreaTypeT  area_type;
        struct SnmpInventoryOids oid;
};

struct InventoryInfo {
        struct InventoryMibInfo mib;
};

struct snmp_bc_inventory {
        SaHpiInventoryRecT  inventory;
        struct InventoryInfo inventory_info;
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

extern struct snmp_bc_inventory snmp_bc_chassis_inventories_rsa[];
extern struct snmp_bc_inventory snmp_bc_cpu_inventories_rsa[];
extern struct snmp_bc_inventory snmp_bc_dasd_inventories_rsa[];
extern struct snmp_bc_inventory snmp_bc_fan_inventories_rsa[];

#endif
