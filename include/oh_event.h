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

#ifndef OH_EVENT_H
#define OH_EVENT_H

#include <SaHpi.h>
#include <rpt_utils.h>

#ifdef __cplusplus
/*extern "C" {*/
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
        /*This is resource id which the event belongs to */
        SaHpiResourceIdT parent;
	/*This is rdr id which the event relates*/
        SaHpiEntryIdT id;
	
	/* XXX: upper layer will fill some fields which does not
	 * owned by plugins (ResourceId etc.). */
	SaHpiEventT event;
};

/*
 * The session event contains rpt entry and rdr.
 * The same as oh_hpi_event but with full data.
 */
struct oh_session_event {
        SaHpiEventT event;
        SaHpiRdrT rdr;
        SaHpiRptEntryT rpt_entry;
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
        struct oh_handler* from;
        oh_event_type type;
        oh_event_union u;
};

extern GAsyncQueue *oh_process_q;

/* function definitions */
int oh_event_init(void);
int oh_event_final(void);
unsigned int get_log_severity(char *);
SaErrorT get_events(RPTable *rpt);
SaErrorT harvest_events(void);
SaErrorT process_events(RPTable *rpt);

#ifdef __cplusplus
/*}*/
#endif
        
#endif /* OH_EVENT_H */

