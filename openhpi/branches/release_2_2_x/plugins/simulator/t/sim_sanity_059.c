/*      -*- linux-c -*-
*
*(C) Copyright IBM Corp. 2005
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
* file and program are licensed under a BSD style license.  See
* the Copying file included with the OpenHPI distribution for
* full licensing terms.
*
* Authors:
*     W. david Ashley <dashley@us.ibm.com>
*/

#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include <../sim_injector_ext.h>


/**
 * Run a series of sanity tests on the simulator
 * Return 0 on success, otherwise return -1
 **/


/**
 * This function is standalone so you can see how to inject events from a
 * completely diferent process than the one OpenHPI is running in.
 * Notice that no simulator plugin entry points are used in this function.
 **/

static int inject_event(char *plugin_name) {
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_RDR_ADD_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, plugin_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_TYPE, SAHPI_SENSOR_RDR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_RDR_ENTITYPATH,
            "{SYSTEM_CHASSIS, 1}");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_FRU, SAHPI_FALSE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_RDR_IDSTRING, "My ID");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_NUM, 14);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_TYPE, SAHPI_TEMPERATURE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_CATEGORY, SAHPI_EC_THRESHOLD);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_ENABLECTRL, SAHPI_FALSE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_EVENTCTRL, SAHPI_SEC_READ_ONLY);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_EVENTSTATE, SAHPI_ES_LOWER_MAJOR | SAHPI_ES_UPPER_MAJOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_SUPPORTED, SAHPI_TRUE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_READINGTYPE, SAHPI_SENSOR_READING_TYPE_INT64);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_BASEUNITS, SAHPI_SU_DEGREES_F);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_MODUNITS, SAHPI_SU_DEGREES_F);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_MODUSE, SAHPI_SMUU_NONE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_PERCENT, SAHPI_FALSE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_ACCURACY, 0);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_RANGE_FLAGS, SAHPI_SRF_MAX);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_RANGE_MAX_SUPPORTED, SAHPI_TRUE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_RANGE_MAX_TYPE, SAHPI_SENSOR_READING_TYPE_INT64);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_DATA_RANGE_MAX_VALUE, 120);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_THRESHOLD_ACCESSIBLE, SAHPI_TRUE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_THRESHOLD_READ, SAHPI_STM_UP_CRIT);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_THRESHOLD_WRITE, SAHPI_STM_UP_CRIT);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_THRESHOLD_NONLINEAR, SAHPI_FALSE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RDR_SENSOR_OEM, 0);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;
 }


int main(int argc, char **argv)
{
    SaHpiSessionIdT sid = 0;
    SaErrorT rc = SA_OK;
    int retc;

    rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
	if(rc != SA_OK)
		return -1;

	rc = saHpiDiscover(sid);
	if (rc != SA_OK)
		return -1;

    /* inject an event */
    retc = inject_event("simulator");
    if (retc != 0) {
        return -1;
    }

    /* sleep so the msg thread gets a chance to process the msg */
    g_usleep(100000);

    saHpiSessionClose(sid);

    return 0;
}
