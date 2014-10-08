/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include <unistd.h>
#include <getopt.h>
#include <glib.h>

//#include "../cClass.h"
/* Values for the sensor number as defined in cClass.h */

#define OA_SOAP_RES_INV_NUM 0
#define OA_SOAP_RES_CNTRL_NUM 1
#define OA_SOAP_RES_SEN_TEMP_NUM 2
#define OA_SOAP_RES_SEN_EXH_TEMP_NUM 3
#define OA_SOAP_RES_SEN_FAN_NUM 4
#define OA_SOAP_RES_SEN_POWER_NUM 5
#define OA_SOAP_RES_SEN_PRES_NUM 6
#define OA_SOAP_RES_SEN_OPR_NUM 7

/* RecordId assignment */
#define OA_SOAP_RES_SEN_RCR_ID 1
#define OA_SOAP_RES_CTRL_RCR_ID 2

#define STRING_LENGTH 250
#define RESOURCE_CAP_LENGTH 250

SaErrorT discover_resources(SaHpiSessionIdT sessionid,
                            SaHpiCapabilitiesT capability,
                            SaHpiResourceIdT *resource_id,
                            int *resource_count);
