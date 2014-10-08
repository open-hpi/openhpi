/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2006
 * (C) Copyright Pigeon Point Systems. 2010
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
 *	    Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#ifndef __EVENT_UTILS_H
#define __EVENT_UTILS_H

#ifndef __OH_UTILS_H
#warning *** Include oh_utils.h instead of individual utility header files ***
#endif

#include <SaHpi.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/***
 * Instructions for using oh_event
 *********************************
 * oh_event is primarily used by the plugins to report HPI events.
 *
 * Required Fields:
 * .hid, .event
 *
 * Optional Fields:
 * .resource, .rdrs
 *
 *
 * Any Resource oh_events: 
 * Use RESOURCE_UPDATED event for updating RPT entry.
 *
 * FRU Resource oh_events:
 * If reporting a resource, the plugin sets the appropiate event in .event.
 * For example, if reporting a new FRU resource, then the event should be
 * a hotswap type showing the correct hotswap state (any except NOT_PRESENT)
 * indicating the library that it should add it to the RPT.
 * If the plugin needs to report an extracted FRU, then the hotswap
 * event has to show the NOT_PRESENT current state, indicating to the library
 * that it should remove it from the RPT.
 * Hotswap events must have their accompaining resource set its capability bit
 * for FRU to 1 or it will be dropped by the infrastructure.
 * The .resource.ResourceCapabilities field can be zero.
 * If so, the RPT will not be updated, but the SaHpiEventT will be passed
 * on to the session queues and domain event log normally.
 *
 * Non-FRU Resource oh_events:
 * For adding or updating Non-FRU resources, the .event should be a resource
 * type HPI event and the ResourceEventType should be RESOURCE_ADDED or
 * RESOURCE_RESTORED. The resource itself should have its capability bit for
 * FRU set to zero or the event will be dropped by the infrastructure.
 * Use RESOURCE_REMOVED for removing Non-FRU resource from the RPT.
 * If a resource is failed, then the oh_event should
 * have a resource event type with the resource state as RESOURCE_FAILED.
 * The .resource field should have the RPT entry for resource in question.
 * This is used by the infrastructure to update the RPT and mark the resource
 * as failed (ResourceFailed == True).
 * The .resource.ResourceCapabilities field can be zero. If so,
 * the RPT will not be updated, but the SaHpiEventT will be passed on to the
 * session queues and domain event log normally.
 *
 * RDRs:
 * If the event is for a resource, be it FRU or Non-FRU, and the resource did
 * not previously exist in the RPT for the domain, then the .rdrs field is
 * scanned for valid SaHpiRdrTs (RdrType != SAHPI_NO_RECORD) objects and each
 * one is added as an rdr for the resource to the RPT. If the resource is
 * already in the RPT, then the rdrs field will be ignored for all
 * event except RESOURCE_UPDATED.
 * In addition to updating RPT entry, the RESOURCE_UPDATED event 
 * can be used for adding, updating or removing RDRs.
 *
 * Other event types:
 * If the event is of type SENSOR, SENSOR_ENABLE_CHANGE, WATCHDOG, or OEM, then
 * The .resource field is scanned for a valid resource to use as reference for
 * the domain event log. Also, the .rdrs field is scanned for exactly one
 * SaHpiRdrT to be used as reference for the domain event log and session event
 * queue. If multiple rdrs are passed for these event types, only the first one
 * will be used.
 **/

struct oh_event {
        unsigned int hid; /* handler id for the event */
        SaHpiEventT event;
        /* If no resource, ResourceCapabilities must be 0 */
        SaHpiRptEntryT resource;
        GSList *rdrs;
        GSList *rdrs_to_remove;
};

typedef GAsyncQueue oh_evt_queue;

#define oh_new_event() g_new0(struct oh_event, 1)
void oh_event_free(struct oh_event *e, int only_rdrs);
struct oh_event *oh_dup_event(struct oh_event *old_event);
void oh_evt_queue_push(oh_evt_queue *equeue, gpointer data);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_UTILS_H */

