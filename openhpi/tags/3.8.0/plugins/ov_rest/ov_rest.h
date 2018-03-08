/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
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
 * Neither the name of the Hewlett Packard Enterprise, nor the names
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
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 */

#ifndef _OV_REST_H
#define _OV_REST_H

#include <unistd.h>

#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_error.h>


/* Include libraries */
#include <errno.h>
#include <stdio.h>
#include <oh_error.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define OV_REST_ENC_C7000       0
#define OV_REST_ENC_C3000       1
#define OV_REST_ENC_SYNERGY     2
#define OV_REST_MAX_ENC       20


/* Maximum Fan Zones present in different HPE Synergy enclosures */
#define OV_REST_C7000_MAX_FZ 4
#define OV_REST_C3000_MAX_FZ 1

/* Wait period for action completion */
#define OV_MAX_POWER_POLLS            10
#define OV_POWER_POLL_SLEEP_SECONDS   05

/* Enum for storing the status of the plugin */
enum ovRestPluginStatus {
        PRE_DISCOVERY = 0,
        PLUGIN_NOT_INITIALIZED = 1,
        DISCOVERY_FAIL = 2,
        DISCOVERY_COMPLETED = 3
};

typedef enum resource_presence
{
        RES_ABSENT = 0,
        RES_PRESENT= 1
} resource_presence_t;

typedef enum resourceCategory
{
	SERVER_HARDWARE = 0,
	DRIVE_ENCLOSURE = 1,
	INTERCONNECT = 2,
	SAS_INTERCONNECT = 3,
	POWER_SUPPLY = 4,
	FAN = 5,
	SYNERGY_COMPOSER = 6,
	UNSPECIFIED_RESOURCE = 7 
}resourceCategory_t;

/* Resource presence matrix per resource type */
typedef struct resource_info
{
        SaHpiInt32T max_bays;
	enum resourceCategory *type;
        enum resource_presence *presence;
        char **serialNumber;
        SaHpiResourceIdT *resource_id;
} resource_info_t;

typedef struct composer_status
{
        char serialNumber[256];
        SaHpiResourceIdT resource_id;
}resourceComposer_t;

typedef struct enclosureStatus
{	
	SaHpiResourceIdT enclosure_rid;
	SaHpiResourceIdT power_subsystem_rid;
	SaHpiResourceIdT thermal_subsystem_rid;
	SaHpiResourceIdT lcd_rid;
	SaHpiInt32T enc_type;
	char *serialNumber;
	struct resource_info composer;
	struct resource_info server;
	struct resource_info interconnect;
	struct resource_info ps_unit;
	struct resource_info fan;
	struct enclosureStatus *next; 
}resourceEnclosure_t;

/* Resource presence matrix for all FRUs in HPE Synergy */
struct ovRestResourceStatus
{	
	struct composer_status composer;
        struct enclosureStatus *enclosure;
	struct resource_info RM_server;
};

struct ov_rest_handler
{
        struct ovConnection *connection;
        enum ovRestPluginStatus status;
        struct ovRestResourceStatus ov_rest_resources;
        GThread *thread_handler;
        GMutex* mutex;
        GMutex* ov_mutex;
        struct oh_handler_state *oh_handler;
        char *ov_relese_version;
        SaHpiBoolT shutdown_event_thread;
	SaHpiInt32T current_enclosure_count;
	SaHpiInt32T max_current_enclosure_count;
	SaHpiInt32T current_rms_count;
	SaHpiInt32T max_current_rms_count;
	SaHpiInt32T discover_called_count;
	GHashTable *uri_rid;
	struct cert {
		char fCaRoot[15];
		char fSslKey[15];
		char fSslCert[15];
	} cert_t;
};

/* Structure for storing the current hotswap state of the resource */
struct ovRestHotswapState {
        SaHpiHsStateT currentHsState;
};


/* This define is the IANA-assigned private enterprise number for
 * Hewlett Packard Enterprise. A complete list of IANA numbers can be found at
 * http://www.iana.org/assignments/enterprise-numbers
 */
#define HPE_MANUFACTURING_ID 47196

/* This define is IANA-assigned private enterprise number for Cisco Systems.
 * The HPE Synergy can have interconnect blades from Cisco Systems
 */
#define CISCO_MANUFACTURING_ID 9

/* Checks for the shutdown request in event thread. On shutdown request, 
 * mutexes locked by event thread are unlocked and exits the thread. It is 
 * necessary to unlock the mutex, else g_free_mutex crahes on locked mutex
 */ 
#define OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, hnd_mutex, ov_mutex, timer) \
        { \
                if (ov_handler->shutdown_event_thread == SAHPI_TRUE) { \
                        dbg("Shutting down the OV REST event thread"); \
                        if (ov_mutex != NULL) \
                                g_mutex_unlock(ov_mutex); \
                        if (hnd_mutex != NULL) \
                                g_mutex_unlock(hnd_mutex); \
                        if (timer != NULL) \
                                g_timer_destroy(timer); \
                        g_thread_exit(NULL); \
                } \
        }

SaErrorT build_ov_rest_custom_handler(struct oh_handler_state *oh_handler);

void *ov_rest_open(GHashTable *handler_config,
                   unsigned int handler_id,
                   oh_evt_queue *eventq);
void ov_rest_close(void *oh_handler);

SaErrorT ov_rest_set_resource_tag(void *oh_handler,
                                  SaHpiResourceIdT resource_id,
                                  SaHpiTextBufferT *tag);

SaErrorT ov_rest_set_resource_severity(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiSeverityT severity);

SaErrorT ov_rest_control_parm(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiParmActionT action);

#endif
