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
                           
/* internal prototypes */
static int read_msg(char * readbuffer, int sockfd);
static int process_msg(char * readbuffer, int sockfd);
static int send_defualt_msg(int sockfd);


/* when true client has closed socket or on socket error */
                                                                      
/* recieve data from client connection */
void answer(int newsockfd){

        static int counter;

        int num_recv;

        int socket_closed = FALSE;
        
        char * readbuffer;

        readbuffer = (char *)malloc(READ_BUFFER_SIZE);


        while (!socket_closed) {
        
                counter++;
                dbg("counter: %d",counter);

                read_msg(readbuffer, newsockfd); 

                process_msg(readbuffer, newsockfd);

                /* block for data or closed socket, if no data read socket closed end session */
                if (!(num_recv = recv(newsockfd, readbuffer, sizeof(NETWORK_HDR_STR), MSG_WAITALL | MSG_PEEK)) ) {
                        dbg("Returned from blocking read having read [%d] bytes.\n", num_recv);        
                        dbg("client closed socket");
                        socket_closed = TRUE;
                }
        };

        /* cleanup */
        free(readbuffer);
}

static int read_msg(char * readbuffer, const int sockfd) {

        /*******************************************************************/
        /* TODO:                                                           */ 
        /* really need a timeout value for these two blocking socket reads */
        /* generate read error after some reasonable amout of time         */
        /*******************************************************************/

        NETWORK_HDR_STR *netHdrPtr;

        int num_recv, remaining_bytes;

        remaining_bytes = 0;

        memset(readbuffer, 0x00, READ_BUFFER_SIZE);
    
        netHdrPtr = (NETWORK_HDR_STR *)readbuffer;

        num_recv = recv(sockfd, readbuffer, sizeof(NETWORK_HDR_STR), MSG_WAITALL);

        dbg("bytes received: %d", num_recv);
        dbg("msg_length: %d", netHdrPtr->msg_length );

        /* calulate remaining bytes and read */
        remaining_bytes = netHdrPtr->msg_length - sizeof(NETWORK_HDR_STR);

        dbg("num of bytes remaining %d", remaining_bytes);

        num_recv = recv(sockfd, readbuffer + sizeof(NETWORK_HDR_STR), remaining_bytes, MSG_WAITALL);

        dbg("bytes received: %d", num_recv);
    
        dbg("message type: %d", netHdrPtr->msg_type);

        return(num_recv);

}

static int process_msg(char * readbuffer, int sockfd) {

        NETWORK_HDR_STR *netHdrPtr;
    
        netHdrPtr = (NETWORK_HDR_STR *)readbuffer;

        switch (netHdrPtr->msg_type) {
        case OPEN:
                open_msg(readbuffer, sockfd);
                break;
        case CLOSE:
                break;
        case GET_EVENT:
                break;
        case DISCOVER_RESOURCES: 
                discover_resources_msg(readbuffer, sockfd);
                break;
        case GET_SEL_ID:       
        case GET_SEL_INFO: 
        case SET_SEL_TIME: 
        case SET_SEL_STATE: 
        case GET_SENSOR_DATA: 
        case GET_SENSOR_THRESHOLDS: 
        case SET_SENSOR_THRESHOLDS:  
        case GET_SENSOR_EVENT_ENABLES: 
        case SET_SENSOR_EVENT_ENABLES: 
        case GET_CONTROL_INFO: 
        case GET_CONTROL_STATE: 
        case SET_CONTROL_STATE: 
        case GET_INVENTORY_SIZE: 
        case GET_INVENTORY_INFO:
        case SET_INVENTORY_INFO: 
        case GET_WATCHDOG_INFO: 
        case SET_WATCHDOG_INFO: 
        case RESET_WATCHDOG: 
        case GET_HOTSWAP_STATE: 
        case SET_HOTSWAP_STATE: 
        case REQUEST_HOTSWAP_ACTION:
        case GET_POWER_STATE: 
        case SET_POWER_STATE:
        case GET_INDICATOR_STATE: 
        case SET_INDICATOR_STATE: 
        case CONTROL_PARM: 
        case GET_RESET_STATE:  
        case SET_RESET_STATE:
        default:
                send_defualt_msg(sockfd);
                break;

        }

        return(0);
}


static int send_defualt_msg (int sockfd) {

        UNKNOWN_MSG_TYPE_MSG_STR 
                default_message = { .header.msg_length = sizeof(UNKNOWN_MSG_TYPE_MSG_STR), 
                                    .header.msg_type = UNKNOWN_MSG_TYPE };

        if ( send(sockfd, &default_message, sizeof(UNKNOWN_MSG_TYPE_MSG_STR), 0) < 0 ) 
                dbg("send_defualt_msg() send error");

        return(0);
}


int send_msg(char *message, int msg_len, int sockfd) {

        int rval = -1;

        if ( send(sockfd, message, msg_len, 0) < msg_len ) 
                dbg("send_msg() send");
        else
                rval = 0;

        return(rval);
}
