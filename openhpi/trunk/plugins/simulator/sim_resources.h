/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *	  Christina Hernandez <hernanc@us.ibm.com>
 *        W. David Ashley <dashley@us.ibm.com>
 */


#ifndef __SIM_RESOURCES_H
#define __SIM_RESOURCES_H

/* Start HPI location numbers from 0 */
#define SIM_HPI_LOCATION_BASE 0

/* IBM Manufacturing Number */
/* Corresponding to change made in snmp_bc_resources.h */
#define IBM_MANUFACTURING_ID 20944

/* Maximum OID string length */
// #define DUMMY_MAX_OID_LENGTH 50

/* OIDs to determine platform types */
// #define DUMMY_PLATFORM_OID_BC   ".1.3.6.1.4.1.2.3.51.2.2.7.1.0" /* BCI System Health */
// #define DUMMY_PLATFORM_OID_BCT  ".1.3.6.1.4.1.2.3.51.2.2.9.1.0" /* BCT System Health */
// #define DUMMY_PLATFORM_OID_RSA  ".1.3.6.1.4.1.2.3.51.1.2.7.1.0" /* RSA System Health */

/* Run-time variables to distinguish platform types */
// #define DUMMY_PLATFORM_BCT   0x0001
// #define DUMMY_PLATFORM_BC    0x0002
// #define DUMMY_PLATFORM_RSA   0x0004
// #define DUMMY_PLATFORM_ALL   0xFFFF

/* Resource indexes to snmp_rpt array in discovery */
// #define BCT_RPT_ENTRY_CHASSIS 0

// typedef enum {
//         BC_RPT_ENTRY_CHASSIS = 0,
//         BC_RPT_ENTRY_MGMNT_MODULE,
//         BC_RPT_ENTRY_SWITCH_MODULE,
//         BC_RPT_ENTRY_BLADE,
//         BC_RPT_ENTRY_BLADE_EXPANSION_CARD,
//         BC_RPT_ENTRY_MEDIA_TRAY,
//         BC_RPT_ENTRY_BLOWER_MODULE,
//         BC_RPT_ENTRY_POWER_MODULE
// } BCRptEntryT;

// typedef enum {
// 	RSA_RPT_ENTRY_CHASSIS = 0,
// 	RSA_RPT_ENTRY_CPU,
//	RSA_RPT_ENTRY_DASD,
// 	RSA_RPT_ENTRY_FAN
// } RSARptEntryT;

/* Maximum number of RSA resources */
// #define RSA_MAX_CPU    8
// #define RSA_MAX_FAN    8
// #define RSA_MAX_DASD   4

/* Maximum entries in eventlog */
#define SIM_EL_MAX_SIZE 256

/**********************
 * Resource Definitions
 **********************/

// struct ResourceMibInfo {
//         const char *OidHealth;
//         int   HealthyValue;
//         const char *OidReset;
//         const char *OidPowerState;
//         const char *OidPowerOnOff;
// 	const char *OidUuid;
// };

#define SIM_MAX_EVENTS_PER_RESOURCE 10
#define SIM_MAX_RESOURCE_EVENT_ARRAY_SIZE (SIM_MAX_EVENTS_PER_RESOURCE + 1)
                                       /* Includes an ending NULL entry */
/*struct res_event_map {
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
};*/

struct sim_rpt {
        SaHpiRptEntryT rpt;
//        struct ResourceInfo res_info;
        const  char *comment;
};

extern struct sim_rpt sim_rpt_array[];
// extern struct dummy_rpt dummy_rpt_array_bct[];
// extern struct dummy_rpt dummy_rpt_array_rsa[];

SaErrorT sim_create_resourcetag(SaHpiTextBufferT *buffer, const char *str, SaHpiEntityLocationT loc);
#endif

