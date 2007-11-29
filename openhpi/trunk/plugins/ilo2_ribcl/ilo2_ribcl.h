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
 *     Shuah Khan <shuah.khan@hp.com>
 *     Richard White <richard.white@hp.com>
 */
#ifndef _INC_ILO2_RIBCL_H_
#define _INC_ILO2_RIBCL_H_

/***************
 * This header file contains all of the iLO2 RIBCL internal data structure
 * defnitions. This file inteded to be included in all of the iLO2 RIBCL
 * source files.
***************/
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>
#include <oh_handler.h>

#include "ilo2_ribcl_cmnds.h"

/* The macro ILO2_RIBCL_SIMULATE_iLO2_RESPONSE is used to conditionally
 * compile code which reads a simulated iLO2 response from a local
 * file rather than communicating with an actual iLO2. This is used for
 * testing when specific responses are needed, or in an environment where
 * an iLO2 is not available. The filename for the response for each command
 * is specified in the openhpi.conf configuration file. The currently supported
 * names are:
 *	"discovery_responsefile" - file used by ilo2_ribcl_do_discovery().
 */ 
/* #define ILO2_RIBCL_SIMULATE_iLO2_RESPONSE */

/* 
 * ILO2 RIBCL User name
 * 	Maximum length - 39
 * 	any combination of printable characters
 * 	case sensitive
 * 	must not be blank
 */
#define ILO2_RIBCL_USER_NAME_MAX_LEN	39
#define ILO2_RIBCL_USER_NAME_MIN_LEN	1
/* 
 * ILO2 RIBCL password
 * 	Maximum length - 39
 * 	any combination of printable characters
 * 	case sensitive
 * 	Minimum length - 0
 * 	Minimum default length - 8
 */
#define ILO2_RIBCL_PASSWORD_MAX_LEN	39
#define ILO2_RIBCL_PASSWORD_MIN_LEN	1

/*
 * ILO2 RIBCL Hostname Length defines
 */
#define ILO2_HOST_NAME_MAX_LEN 256
/* A valid IP addr will have a min. len of 7 4 digts + 3 decimal points */
#define ILO2_HOST_NAME_MIN_LEN 7

/* Minimum length port string */
#define ILO2_MIN_PORT_STR_LEN	1

/*
 * RIBCL command and response buffer length - static for now.
 * Change to dynamic if need be.
 */
#define ILO2_RIBCL_BUFFER_LEN	4096

/*
 * Power and reset status definitions
*/
#define ILO2_RIBCL_POWER_OFF	0
#define ILO2_RIBCL_POWER_ON	1
#define ILO2_RIBCL_RESET_SUCCESS	1	
#define ILO2_RIBCL_RESET_FAILED	0	

/* 
 * get_event return value when there are events pending to be processed.
 * OpenHPI framework doesn't provide mnemonic for this return value.
*/
#define ILO2_RIBCL_EVENTS_PENDING	1
/* 
 * Resource doesn't support managed hot swap. OpenHPI fails to define
 * mnemonic for this value
*/
#define ILO2_RIBCL_MANAGED_HOTSWAP_CAP_FALSE	0

/*
 * The following data structures are used to save the discovery data. 
 * This data cache will be used to implement APIs that require state
 * information and additional information on components such as speed,
 * and control capabilities. The index into this field, rid, and hotswap
 * state of each component will be added to the RPT private data cache
 * via oh_add_resource() when each component is discovered.
*/
/* Discover states for a resource */
enum ir_discoverstate { BLANK=0, OK=1, FAILED=2, REMOVED=3};

/* Values for flags fields in DiscoveryData structures */
#define IR_DISCOVERED		0x01
#define IR_EXISTED		0x02
#define IR_FAILED		0x04
#define IR_SPEED_UPDATED	0x08

typedef struct ir_cpudata {
	unsigned int cpuflags;
	enum ir_discoverstate dstate;
	char *label;
} ir_cpudata_t;

typedef struct ir_memdata {
	unsigned int memflags;
	enum ir_discoverstate dstate;
	char *label;
	char *memsize;
	char *speed;
} ir_memdata_t;

typedef struct ir_fandata{
	unsigned int fanflags;
	enum ir_discoverstate dstate;
	char *label;
	char *zone;
	char *status;
	int speed;
	char *speedunit;
} ir_fandata_t;

typedef struct ir_psudata{
	unsigned int psuflags;
	enum ir_discoverstate dstate;
	char *label;
	char *status;
} ir_psudata_t;

typedef struct ir_vrmdata{
	unsigned int vrmflags;
	enum ir_discoverstate dstate;
	char *label;
	char *status;
} ir_vrmdata_t;

typedef struct ir_tsdata{
	unsigned int tsflags;
	char *label;
	char *location;
	char *status;
	char *reading;
	char *readingunits;
} ir_tsdata_t;

#define ILO2_RIBCL_DISCOVER_CPU_MAX 16
#define ILO2_RIBCL_DISCOVER_MEM_MAX 16
#define ILO2_RIBCL_DISCOVER_FAN_MAX 16
#define ILO2_RIBCL_DISCOVER_PSU_MAX 8
#define ILO2_RIBCL_DISCOVER_VRM_MAX 8
#define ILO2_RIBCL_DISCOVER_TS_MAX  16

#define ILO2_RIBCL_CHASSIS_INDEX    -1;	/* Index is not aplicable to chassis */
 
typedef struct ilo2_ribcl_DiscoveryData {
	char *product_name;
	char *serial_number;
	ir_cpudata_t cpudata[ ILO2_RIBCL_DISCOVER_CPU_MAX+1];
	ir_memdata_t memdata[ ILO2_RIBCL_DISCOVER_MEM_MAX+1];
	ir_fandata_t fandata[ ILO2_RIBCL_DISCOVER_FAN_MAX+1];
	ir_psudata_t psudata[ ILO2_RIBCL_DISCOVER_PSU_MAX+1];
	ir_vrmdata_t vrmdata[ ILO2_RIBCL_DISCOVER_VRM_MAX+1];
	ir_tsdata_t tsdata[ ILO2_RIBCL_DISCOVER_TS_MAX+1];
} ilo2_ribcl_DiscoveryData_t; 

/* iLO2 RIBCL plug-in handler structure */
typedef struct ilo2_ribcl_handler {
	char *entity_root;
	int first_discovery_done;

	/* Storehouse for data obtained during discovery */
	ilo2_ribcl_DiscoveryData_t DiscoveryData;

	/* RIBCL data */
	char *user_name;
	char *password;

	/* iLO2 hostname and port number information */
	char *ilo2_hostport;

#ifdef ILO2_RIBCL_SIMULATE_iLO2_RESPONSE
	/* Discovery response file for testing */
	char *discovery_responsefile;
#endif /* ILO2_RIBCL_SIMULATE_iLO2_RESPONSE */

	/* SSL connection status */
	void *ssl_ctx;
	/* SSL connection handler pointer */
	void *ssl_handler;

	/* Commands customized with the login and password for this system */
	char *ribcl_xml_cmd[ IR_NUM_COMMANDS];

	GSList *eventq;                 /* Event queue cache */
 
} ilo2_ribcl_handler_t;

/* iLO2 RIBCL private resource data */
typedef struct ilo2_ribcl_resource_info {
	SaHpiResourceIdT rid;
	SaHpiHsStateT fru_cur_state;	/* current simple hotswap state of
					   FRU resources */
	int disc_data_idx;		/* resource index into the Discovery
					   Data Cache */
} ilo2_ribcl_resource_info_t;
  

/*****************************
	Prototypes for iLO2 RIBCL plug-in ABI functions
*****************************/

/* The following fucntions are defined in ilo2_ribcl.c */
extern void *ilo2_ribcl_open(GHashTable *, unsigned int , oh_evt_queue *);
extern void ilo2_ribcl_close(void *);
extern SaErrorT ilo2_ribcl_discover_resources(void *);
extern SaErrorT ilo2_ribcl_get_event(void *);

/* The following functions are defined in ilo2_ribcl_reset.c */
extern SaErrorT ilo2_ribcl_get_reset_state(void *, SaHpiResourceIdT,
	SaHpiResetActionT *);
extern SaErrorT ilo2_ribcl_set_reset_state(void *hnd, SaHpiResourceIdT rid,
				 SaHpiResetActionT act);

/* The following functions are defined in ilo2_ribcl_reset.c */
extern SaErrorT ilo2_ribcl_get_power_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiPowerStateT *state);
extern SaErrorT ilo2_ribcl_set_power_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiPowerStateT state);

/* The following functions are defined in ilo2_ribcl_rpt.c */

extern SaErrorT ilo2_ribcl_set_resource_severity(void *, SaHpiResourceIdT,
	SaHpiSeverityT);
extern SaErrorT ilo2_ribcl_set_resource_tag(void *, SaHpiResourceIdT,
	SaHpiTextBufferT *);

#endif /* _INC_ILO2_RIBCL_H_ */
