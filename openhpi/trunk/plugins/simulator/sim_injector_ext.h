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
    SIM_MSG_RESOURCE_EVENT
} SIM_MSG_TYPE;

// the following are name constants for the msg buf name/value pairs
// required by all message types
#define SIM_MSG_HANDLER_NAME "handler_name"
#define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_EVENT_SEVERITY "event_severity"
// required sensor event constants
#define SIM_MSG_SENSOR_NUM "sensor_num"
#define SIM_MSG_SENSOR_TYPE "sensor_type"
#define SIM_MSG_SENSOR_EVENT_CATEGORY "sensor_category"
#define SIM_MSG_SENSOR_ASSERTION "sensor_assertion"
#define SIM_MSG_SENSOR_EVENT_STATE "sensor_state"
// optional sensor event constants
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
#define SIM_MSG_WATCHDOG_NUM "watchdog_num"
#define SIM_MSG_WATCHDOG_ACTION "watchdog_action"
#define SIM_MSG_WATCHDOG_PRETIMER_ACTION "watchdog_pretimer_action"
#define SIM_MSG_WATCHDOG_TIMER_USE "watchdog_timer_use"
// required software event constants
#define SIM_MSG_SW_MID "sw_mid"
#define SIM_MSG_SW_EVENT_TYPE "sw_event_type"
#define SIM_MSG_SW_EVENT_DATA "sw_event_data"
// required oem event constants
#define SIM_MSG_OEM_MID "oem_mid"
#define SIM_MSG_OEM_EVENT_DATA "oem_event_data"
// required oem event constants
#define SIM_MSG_USER_EVENT_DATA "user_event_data"
// required resource event constants
#define SIM_MSG_RESOURCE_EVENT_TYPE "resource_event_type"





#endif //__SIM_INJECTOR_EXT_H
