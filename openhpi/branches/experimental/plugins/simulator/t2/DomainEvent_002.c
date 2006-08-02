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
*     W. David Ashley <dashley@us.ibm.com>
*/

/*
 * This is the same test as t1/sim_sanity_049.c
 */

#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include <../sim_injector_ext.h>

/*
 * The following values are assumptions and may not be valid for your running
 * instance of OpenHPI.
 */
#define PLUGINNAME "simulator"
#define NEWDOMAIN 2


/*
 * Insert sensor event message
 */
int main(int argc, char **argv)
{
        key_t ipckey;
        int msgqueid;
        SIM_MSG_QUEUE_BUF buf;
        size_t n = 0;  // size of the data in the msg buf
        char *txtptr = buf.mtext;

        /* get the  queue */
        ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
        msgqueid = msgget(ipckey, IPC_CREAT | 0660);
        if (msgqueid == -1) {
            return -1;
        }

        /* fill out the message */
        buf.mtype = SIM_MSG_DOMAIN_EVENT;
        *txtptr = '\0';
        sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, PLUGINNAME);
        n += strlen(txtptr) + 1;
        txtptr = buf.mtext + n;
        if (n > SIM_MSG_QUEUE_BUFSIZE) {
            return -1;
        }
        sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MAJOR);
        n += strlen(txtptr) + 1;
        txtptr = buf.mtext + n;
        if (n > SIM_MSG_QUEUE_BUFSIZE) {
            return -1;
        }
        sprintf(txtptr, "%s=%d", SIM_MSG_DOMAIN_TYPE, SAHPI_DOMAIN_REF_REMOVED);
        n += strlen(txtptr) + 1;
        txtptr = buf.mtext + n;
        if (n > SIM_MSG_QUEUE_BUFSIZE) {
            return -1;
        }
        sprintf(txtptr, "%s=%d", SIM_MSG_DOMAIN_ID, NEWDOMAIN);
        n += strlen(txtptr) + 1;
        txtptr = buf.mtext + n;
        if (n > SIM_MSG_QUEUE_BUFSIZE) {
            return -1;
        }
        *txtptr = '\0'; // terminate buf with a zero-length string
        n++;

        /* send the msg */
        if (msgsnd(msgqueid, &buf, n, 0)) {
            return -1;
        }

    return 0;
}
