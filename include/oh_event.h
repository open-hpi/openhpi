/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <sdague@users.sf.net>
 *
 */

#ifndef __OH_EVENT_H
#define __OH_EVENT_H

#include <SaHpi.h>
#include <oh_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The event is used for plugin to report its resources.
 *  For OpenHPI >= 2.0 we use the full structure for add
 *  and delete to handle the hotswaping away issue
 */
struct oh_resource_event {
        SaHpiRptEntryT entry;
};

/* 
 * The event is used for plugin to report its RDRs in resource.
 */
struct oh_rdr_event {
        SaHpiResourceIdT parent;
	SaHpiRdrT rdr;
};

/*
 * The event is used for plugin to notify HPI events
 */
struct oh_hpi_event {
        /* Resource Associated with event */
        SaHpiRptEntryT res;
	/* RDR Associated with event */
        SaHpiRdrT rdr;
        /* the real event */
        SaHpiEventT event;
};

/* 
 * This is the main event structure. It is used for plugin report
 * its discovery about new resource/rdr or what happend on resource
 */

typedef enum {
        OH_ET_NONE = 0, /* if this is set the event is invalid */
        OH_ET_RESOURCE, 
        OH_ET_RESOURCE_DEL,
        OH_ET_RDR,
        OH_ET_RDR_DEL,
        OH_ET_HPI
} oh_event_type;

typedef union {
        struct oh_resource_event res_event;
        struct oh_rdr_event	 rdr_event;
        struct oh_hpi_event	 hpi_event;
} oh_event_union;

struct oh_event {
        SaHpiDomainIdT did; /* domain id for the event */
        struct oh_handler* from; /* handler for the event */
        oh_event_type type;
        oh_event_union u;
};

extern GAsyncQueue *oh_process_q;

/* function definitions */
int oh_event_init(void);
int oh_event_final(void);
unsigned int get_log_severity(char *severity);
SaErrorT get_events(void);
SaErrorT harvest_events(void);
SaErrorT process_events(void);

#ifdef __cplusplus
}
#endif
        
#endif /* __OH_EVENT_H */

