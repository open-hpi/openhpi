/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2005
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

#ifndef __SIM_RESOURCES_H
#define __SIM_RESOURCES_H

/* Start HPI location numbers from 1 */
#define DUMMY_HPI_LOCATION_BASE 1

/* IBM Manufacturing Number */
#define IBM_MANUFACTURING_ID 2

/* Maximum OID string length */
#define DUMMY_MAX_OID_LENGTH 50

/* OIDs to determine platform types */
#define DUMMY_PLATFORM_OID_BC   ".1.3.6.1.4.1.2.3.51.2.2.7.1.0" /* BCI System Health */
#define DUMMY_PLATFORM_OID_BCT  ".1.3.6.1.4.1.2.3.51.2.2.9.1.0" /* BCT System Health */
#define DUMMY_PLATFORM_OID_RSA  ".1.3.6.1.4.1.2.3.51.1.2.7.1.0" /* RSA System Health */

/* Run-time variables to distinguish platform types */
#define DUMMY_PLATFORM_BCT   0x0001
#define DUMMY_PLATFORM_BC    0x0002
#define DUMMY_PLATFORM_RSA   0x0004
#define DUMMY_PLATFORM_ALL   0xFFFF

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
#define DUMMY_BLADE_VECTOR        ".1.3.6.1.4.1.2.3.51.2.2.5.2.49.0"
#define DUMMY_BLADE_ADDIN_VECTOR  ".1.3.6.1.4.1.2.3.51.2.22.1.5.1.1.14.x"
#define DUMMY_FAN_VECTOR          ".1.3.6.1.4.1.2.3.51.2.2.5.2.73.0"
#define DUMMY_MGMNT_VECTOR        ".1.3.6.1.4.1.2.3.51.2.22.4.30.0"
#define DUMMY_MGMNT_ACTIVE        ".1.3.6.1.4.1.2.3.51.2.22.4.34.0"
#define DUMMY_MEDIATRAY_EXISTS    ".1.3.6.1.4.1.2.3.51.2.2.5.2.74.0"
#define DUMMY_POWER_VECTOR        ".1.3.6.1.4.1.2.3.51.2.2.5.2.89.0"
#define DUMMY_SWITCH_VECTOR       ".1.3.6.1.4.1.2.3.51.2.2.5.2.113.0"
#define DUMMY_DST                 ".1.3.6.1.4.1.2.3.51.2.4.4.2.0"
#define DUMMY_DST_RSA             ".1.3.6.1.4.1.2.3.51.1.4.4.2.0"
#define DUMMY_CPU_OID_RSA         ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.3.x"
#define DUMMY_DASD_OID_RSA        ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.3.x"
#define DUMMY_FAN_OID_RSA         ".1.3.6.1.4.1.2.3.51.1.2.3.x.0"

/* OID definitions for System Event Log */
#define DUMMY_DATETIME_OID      ".1.3.6.1.4.1.2.3.51.2.4.4.1.0"
#define DUMMY_DATETIME_OID_RSA  ".1.3.6.1.4.1.2.3.51.1.4.4.1.0"
#define DUMMY_SEL_INDEX_OID     ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.1"
#define DUMMY_SEL_INDEX_OID_RSA ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.1"
#define DUMMY_SEL_ENTRY_OID     ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.2"
#define DUMMY_SEL_ENTRY_OID_RSA ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.2"
#define DUMMY_SEL_CLEAR_OID     ".1.3.6.1.4.1.2.3.51.2.3.4.3.0"
#define DUMMY_SEL_CLEAR_OID_RSA ".1.3.6.1.4.1.2.3.51.1.3.4.3.0"

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

#define DUMMY_MAX_EVENTS_PER_RESOURCE 10
#define DUMMY_MAX_RESOURCE_EVENT_ARRAY_SIZE  (DUMMY_MAX_EVENTS_PER_RESOURCE + 1)
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
        struct res_event_map event_array[DUMMY_MAX_RESOURCE_EVENT_ARRAY_SIZE];
};

struct dummy_rpt {
        SaHpiRptEntryT rpt;
//        struct ResourceInfo res_info;
        const  char *comment;
};

extern struct dummy_rpt dummy_rpt_array[];
// extern struct dummy_rpt dummy_rpt_array_bct[];
// extern struct dummy_rpt dummy_rpt_array_rsa[];

SaErrorT dummy_create_resourcetag(SaHpiTextBufferT *buffer, const char *str, SaHpiEntityLocationT loc);
#endif

