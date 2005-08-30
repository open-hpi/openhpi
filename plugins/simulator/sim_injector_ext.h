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
 *      W. David Ashley <dashley@us.ibm.com>
 *
 */


#ifndef __SIM_INJECTOR_EXT_H
#define __SIM_INJECTOR_EXT_H


#define SIM_MSG_QUEUE_KEY 'I'
#define SIM_MSG_QUEUE_BUFSIZE 1024

typedef struct {
    long int mtype;
    char     mtext[SIM_MSG_QUEUE_BUFSIZE];
} SIM_MSG_QUEUE_BUF;

typedef enum {
    SIM_MSG_SENSOR_EVENT = 1,
    SIM_MSG_SENSOR_ENABLE_CHANGE_EVENT,
    SIM_MSG_HOT_SWAP_EVENT,
    SIM_MSG_WATCHDOG_EVENT,
    SIM_MSG_SW_EVENT,
    SIM_MSG_OEM_EVENT,
    SIM_MSG_USER_EVENT,
    SIM_MSG_RESOURCE_EVENT,
    SIM_MSG_DOMAIN_EVENT,
    SIM_MSG_RESOURCE_ADD_EVENT,
    SIM_MSG_RDR_ADD_EVENT
} SIM_MSG_TYPE;

// the following are name constants for the msg buf name/value pairs
// required by all message types
#define SIM_MSG_HANDLER_NAME "handler_name"
#define SIM_MSG_EVENT_SEVERITY "event_severity"
// required sensor event constants
#define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_SENSOR_NUM "sensor_num"
#define SIM_MSG_SENSOR_TYPE "sensor_type"
#define SIM_MSG_SENSOR_EVENT_CATEGORY "sensor_category"
#define SIM_MSG_SENSOR_ASSERTION "sensor_assertion"
#define SIM_MSG_SENSOR_EVENT_STATE "sensor_state"
// optional sensor event constants
// #define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_SENSOR_OPTIONAL_DATA "sensor_opt_data"
#define SIM_MSG_SENSOR_TRIGGER_READING_SUPPORTED "sensor_trigger_reading_supported"
#define SIM_MSG_SENSOR_TRIGGER_READING_TYPE "sensor_trigger_reading_type"
#define SIM_MSG_SENSOR_TRIGGER_READING "sensor_trigger_reading"
#define SIM_MSG_SENSOR_TRIGGER_THRESHOLD_SUPPORTED "sensor_trigger_threshold_supported"
#define SIM_MSG_SENSOR_TRIGGER_THRESHOLD_TYPE "sensor_trigger_threshold_type"
#define SIM_MSG_SENSOR_TRIGGER_THRESHOLD "sensor_trigger_threshold"
#define SIM_MSG_SENSOR_PREVIOUS_STATE "sensor_previous_state"
#define SIM_MSG_SENSOR_CURRENT_STATE "sensor_current_state"
#define SIM_MSG_SENSOR_OEM "sensor_oem"
#define SIM_MSG_SENSOR_SPECIFIC "sensor_specific"
// required sensor enable change event constants
// #define SIM_MSG_RESOURCE_ID "resource_id"
// #define SIM_MSG_SENSOR_NUM "sensor_num"
// #define SIM_MSG_SENSOR_TYPE "sensor_type"
// #define SIM_MSG_SENSOR_EVENT_CATEGORY "sensor_category"
#define SIM_MSG_SENSOR_ENABLE "sensor_enable"
#define SIM_MSG_SENSOR_EVENT_ENABLE "sensor_event_enable"
#define SIM_MSG_SENSOR_ASSERT_EVENT_MASK "sensor_assert_event_mask"
#define SIM_MSG_SENSOR_DEASSERT_EVENT_MASK "sensor_deassert_event_mask"
// optional sensor enable change event constants
// #define SIM_MSG_SENSOR_OPTIONAL_DATA "sensor_opt_data"
// #define SIM_MSG_SENSOR_CURRENT_STATE "sensor_current_state"
// required hot swap event constants
#define SIM_MSG_HS_STATE "hs_state"
#define SIM_MSG_HS_PREVIOUS_STATE "hs_previous_state"
// required watchdog event constants
// #define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_WATCHDOG_NUM "watchdog_num"
#define SIM_MSG_WATCHDOG_ACTION "watchdog_action"
#define SIM_MSG_WATCHDOG_PRETIMER_ACTION "watchdog_pretimer_action"
#define SIM_MSG_WATCHDOG_TIMER_USE "watchdog_timer_use"
// required software event constants
// #define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_SW_MID "sw_mid"
#define SIM_MSG_SW_EVENT_TYPE "sw_event_type"
#define SIM_MSG_SW_EVENT_DATA "sw_event_data"
// required oem event constants
// #define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_OEM_MID "oem_mid"
#define SIM_MSG_OEM_EVENT_DATA "oem_event_data"
// required user event constants
// #define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_USER_EVENT_DATA "user_event_data"
// required resource event constants
// #define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_RESOURCE_EVENT_TYPE "resource_event_type"
// required domain event constants
#define SIM_MSG_DOMAIN_TYPE "domain_type"
#define SIM_MSG_DOMAIN_ID "domain_id"
// required resource event constants
#define SIM_MSG_RPT_ENTITYPATH "rpt_entitypath"
#define SIM_MSG_RPT_CAPABILITIES "rpt_capabilities"
#define SIM_MSG_RPT_HSCAPABILITIES "rpt_hscapabilities"
// #define SIM_MSG_EVENT_SEVERITY "event_severity"
#define SIM_MSG_RPT_FAILED "rpt_failed"
#define SIM_MSG_RPT_RESTAG "rpt_restag"
#define SIM_MSG_RPT_COMMENT "rpt_comment"
// optional resource event constants
#define SIM_MSG_RPT_RESINFO_RESREV "rpt_resinfo_resrev"
#define SIM_MSG_RPT_RESINFO_SPECVER "rpt_resinfo_specver"
#define SIM_MSG_RPT_RESINFO_DEVSUPPORT "rpt_resinfo_devsupport"
#define SIM_MSG_RPT_RESINFO_MFGID "rpt_resinfo_mfgid"
#define SIM_MSG_RPT_RESINFO_PRODID "rpt_resinfo_prodid"
#define SIM_MSG_RPT_RESINFO_FIRMMAJREV "rpt_resinfo_firmmajrev"
#define SIM_MSG_RPT_RESINFO_FIRMMINREV "rpt_resinfo_firmminrev"
#define SIM_MSG_RPT_RESINFO_AUXFIRMREV "rpt_resinfo_auxfirmrev"
// required rdr event constants
#define SIM_MSG_RDR_TYPE "rdr_type"
#define SIM_MSG_RDR_ENTITYPATH "rdr_entitypath"
#define SIM_MSG_RDR_FRU "rdr_fru"
#define SIM_MSG_RDR_IDSTRING "rdr_idstring"
// required rdr annunciator constants
#define SIM_MSG_RDR_ANNUN_NUM "rdr_annum_num"
#define SIM_MSG_RDR_ANNUN_TYPE "rdr_annum_type"
#define SIM_MSG_RDR_ANNUN_MODE "rdr_annum_mode"
#define SIM_MSG_RDR_ANNUN_MAXCOND "rdr_annum_maxcond"
#define SIM_MSG_RDR_ANNUN_OEM "rdr_annum_oem"
// required rdr watchdog constants
#define SIM_MSG_RDR_WATCHDOG_NUM "rdr_watchdog_num"
#define SIM_MSG_RDR_WATCHDOG_OEM "rdr_watchdog_oem"
// required rdr idr constants
#define SIM_MSG_RDR_IDR_ID "rdr_idr_id"
#define SIM_MSG_RDR_IDR_PERSISTENT "rdr_idr_persistent"
#define SIM_MSG_RDR_IDR_OEM "rdr_idr_oem"
// required rdr ctrl constants
#define SIM_MSG_RDR_CTRL_NUM "rdr_ctrl_num"
#define SIM_MSG_RDR_CTRL_OUTTYPE "rdr_ctrl_outtype"
#define SIM_MSG_RDR_CTRL_TYPE "rdr_ctrl_type"
#define SIM_MSG_RDR_CTRL_MODE "rdr_ctrl_mode"
#define SIM_MSG_RDR_CTRL_MODE_READONLY "rdr_ctrl_mode_readonly"
#define SIM_MSG_RDR_CTRL_WRITEONLY "rdr_ctrl_writeonly"
#define SIM_MSG_RDR_CTRL_OEM "rdr_ctrl_oem"
#define SIM_MSG_RDR_CTRL_DIGITAL_DEFSTATE "rdr_ctrl_digital_defstate"
#define SIM_MSG_RDR_CTRL_DISCRETE_DEFSTATE "rdr_ctrl_discrete_defstate"
#define SIM_MSG_RDR_CTRL_ANALOG_MIN "rdr_ctrl_analog_min"
#define SIM_MSG_RDR_CTRL_ANALOG_MAX "rdr_ctrl_analog_max"
#define SIM_MSG_RDR_CTRL_ANALOG_DEFAULT "rdr_ctrl_analog_default"
#define SIM_MSG_RDR_CTRL_STREAM "rdr_ctrl_stream"




#endif //__SIM_INJECTOR_EXT_H
