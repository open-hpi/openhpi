/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by International Business Machines
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

#ifndef __OPENHPID_H
#define __OPENHPID_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <glib.h>

/* threading */
#include <sched.h>

/* select and accept */
#include <sys/select.h>

/* to be able to build event structure for text_remote plugin to consume */
#include <oh_plugin.h>

/* network messages */                    
#include <net_msg.h>

/* libopenhpi.s0.0 or libopenhpi.a usage */
#include <SaHpi.h>
#include <openhpi.h>


//#define SERV_TCP_PORT   6543
//#define SERV_HOST_ADDR  "127.0.0.1" /* host addr for server */

#define STACK_SIZE  2048

/* prototypes */
int init_openhpid(void);
int close_openhpid(void);
int open_msg(char * readbuffer, int sockfd);
int discover_resources_msg(char * readbuffer, int sockfd);
void answer(int newsockfd);

/* global variables */
SaErrorT 	err;
SaHpiVersionT version;      /* return value from SaHpiInitialize */
SaHpiSessionIdT session_id; /* used in init_openhpid */


#endif/*__OPENHPID_H*/




