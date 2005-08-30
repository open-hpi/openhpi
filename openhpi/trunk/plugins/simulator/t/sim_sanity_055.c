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
    buf.mtype = SIM_MSG_RESOURCE_ADD_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, plugin_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_RPT_ENTITYPATH,
            "{BIOS,1}{SYSTEM_CHASSIS,1}");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RPT_CAPABILITIES, SAHPI_CAPABILITY_RESOURCE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RPT_HSCAPABILITIES, 0);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_OK);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RPT_FAILED, SAHPI_FALSE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_RPT_COMMENT, "BIOS comment");
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
