/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors: David Judkovics
 *     
 *     
 *     
 */

#include "openhpid.h"


/********************************************************************/
/* init_openhpid                                                    */
/********************************************************************/
int init_openhpid(void) 
{
        SaErrorT err;

        dbg("\nsaHpiInitialize\n");
        err = saHpiInitialize(&version);
        if (SA_OK != err) {
                dbg("saHpiInitialize Failed: %d", err);
                exit(-1);
        }

        /* every domain requires a new session */
        dbg("\nsaHpiInitialize\n");
        err = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
        if (SA_OK != err) {
                dbg("saHpiSessionOpen Failed: %d", err);
                return err;
        }

        return 0;
    
}

/********************************************************************/
/* close_openhpid                                                   */
/********************************************************************/
int close_openhpid(void) 
{
        SaErrorT err;

        dbg("\nsaHpiFinalize\n");
        err = saHpiFinalize();
        if (SA_OK != err) {
                dbg("saHpiFinalize Failed: %d", err);
                exit(-1);
        }
        return 0;
    
}

/********************************************************************/
/* open_msg                                                         */
/********************************************************************/
int open_msg(char * readbuffer, int sockfd) 
{

        SaErrorT rval;

        OPEN_MSG_STR *open_msg_ptr;

        OPEN_MSG_STR open_msg_response = {.header.msg_type = OPEN, 
                                          .header.msg_length = sizeof(OPEN_MSG_STR),
                                          .text_message = "New and Imporved OPEN response"};

       open_msg_ptr = (OPEN_MSG_STR *)readbuffer;

       dbg("OPEN text message: %s\n", open_msg_ptr->text_message);

       if ((rval = send_msg((char *)&open_msg_response, sizeof(OPEN_MSG_STR), sockfd)) < 0) 
               dbg("open_msg Failed: %d", rval);
    
       return(rval);
}


/********************************************************************/
/* discover_resources_msg                                                         */
/********************************************************************/
int discover_resources_msg(char * readbuffer, int sockfd) 
{
        DISCOVER_RESOURCES_MSG_STR discover_resources_msg;

        SaHpiEntryIdT   current;
        SaHpiEntryIdT   next = SAHPI_FIRST_ENTRY;
        SaHpiRptEntryT  entry;

        SaHpiRptInfoT   rpt_info;

        int i = 0;  /* simple counter */
    
        discover_resources_msg.header.msg_type = DISCOVER_RESOURCES;
        discover_resources_msg.header.msg_length = sizeof(DISCOVER_RESOURCES_MSG_STR);

        err = saHpiResourcesDiscover(session_id);
        if (SA_OK != err) {
                dbg("saHpiResourcesDiscover Failed:%d", err);
        }
        else {
                /* from here the number of RPT entries are found and it is not necessary to have a variable spin loop */
                err = saHpiRptInfoGet(session_id, &rpt_info);
                if (SA_OK != err) {
                        dbg("saHpiRptInfoGet Failed: %d", err);
                        return err;
                }
                do {
                        current = next;
                        err = saHpiRptEntryGet(session_id, current, &next, &entry);
                        if (SA_OK != err) {
                                if (current != SAHPI_FIRST_ENTRY) {
                                        dbg("saHpiRptEntryGet Failed: %d", err);     
                                        return err;
                                } else {
                                        dbg("Empty RPT\n");
                                        return SA_OK;
                                }
                        }
            
                        discover_resources_msg.oh_event_arry[i].type = OH_ET_RESOURCE;
                        discover_resources_msg.oh_event_arry[i].u.res_event.id.ptr = NULL;
		 //       discover_resources_msg.oh_event_arry[i].u.res_event.domain_id = entry.DomainId;
                        memcpy(&discover_resources_msg.oh_event_arry[i].u.res_event.entry, 
                               &entry, 
                               sizeof(entry));         

                        i++;
                } while ( next != SAHPI_LAST_ENTRY );

                discover_resources_msg.num_resource_events = i;
        }

        discover_resources_msg.error = SA_OK;

        send_msg((char *)&discover_resources_msg,discover_resources_msg.header.msg_length, sockfd);

        return(SA_OK);
}
