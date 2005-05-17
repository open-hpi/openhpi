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
#include <glib.h>

#define OH_MAX_EVT_QUEUE_LIMIT 0

#ifdef __cplusplus
extern "C" {
#endif

/* Event utility macros */
#define oh_new_event() g_new0(SaHpiEventT, 1)
#define oh_dup_event(old) g_memdup(old, sizeof(*old))
#define oh_copy_event(new, old) memcpy(new, old, sizeof(*new))
#define oh_dup_oh_event(old) g_memdup(old, sizeof(*old))
#define oh_copy_oh_event(new, old) memcpy(new, old, sizeof(*new))

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
 * its discovery about new resource/rdr or what happened on resource
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
        struct oh_rdr_event      rdr_event;
        struct oh_hpi_event      hpi_event;
} oh_event_union;

struct oh_event {
        SaHpiDomainIdT did; /* domain id for the event */
        unsigned int hid; /* handler id for the event */
        oh_event_type type;
        oh_event_union u;
};

extern GAsyncQueue *oh_process_q;

/* function definitions */
int oh_event_init(void);
int oh_start_event_thread(void);
int oh_event_final(void);
gboolean oh_run_threaded(void);
SaErrorT oh_get_events(void);
SaErrorT oh_harvest_events(void);
SaErrorT oh_process_events(void);

#ifdef __cplusplus
}
#endif

#endif /* __OH_EVENT_H */

