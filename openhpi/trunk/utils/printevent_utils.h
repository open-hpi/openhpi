/*      -*- linux-c -*-
 *
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 *
 * Authors:
 *      peter d phan   <pdphan@users.sf.net>
 *      
 * Logs:
 */

#ifndef PRINTEVENTS_H 
#define PRINTEVENTS_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "SaHpi.h"

struct code2string {
	int val;
	char *str;
};


/* 
 * Export function prototypes
*/
void print_event(SaHpiEventT *thisEvent);
char *decode_enum(struct code2string *code_array, int NUM_MAX, int code);
void saftime2str(SaHpiTimeT time, char * str, size_t size);

#endif
