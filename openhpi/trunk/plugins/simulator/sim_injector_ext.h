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
    SIM_MSG_SENSOR_EVENT = 1
} SIM_MSG_TYPE;

// the following are name constants for the msg buf name/value pairs
#define SIM_MSG_HANDLER_NAME "handler_name"
#define SIM_MSG_EVENT_TYPE "event_type"
#define SIM_MSG_RESOURCE_ID "resource_id"
#define SIM_MSG_EVENT_SEVERITY "event_severity"
// sensor event constants
#define SIM_MSG_SENSOR_NUM "sensor_num"
#define SIM_MSG_SENSOR_TYPE "sensor_type"
#define SIM_MSG_SENSOR_EVENT_CATEGORY "sensor_category"
#define SIM_MSG_SENSOR_ASSERTION "sensor_assertion"
#define SIM_MSG_SENSOR_EVENT_STATE "sensor_state"





#endif //__SIM_INJECTOR_EXT_H
