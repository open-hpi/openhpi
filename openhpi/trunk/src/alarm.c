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
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#include <string.h>
#include <sys/time.h>
#include <oh_alarm.h>
#include <oh_error.h>
#include <oh_domain.h>
#include <oh_utils.h>

static void oh_detect_oem_event_alarm(SaHpiDomainIdT did, SaHpiEventT *event)
{
        struct oh_domain *d = NULL;
        struct timeval tv1;
        GList *alarms = NULL;
        SaHpiAlarmT *a = NULL;
        
        d = oh_get_domain(did);
        if (!d) {
                dbg("Domain not found! Can't process alarm trigger.");
                return;
        }
        
        /* Search for possible oem alarm, if severity is "non-alarming" */
        if (event->Severity > SAHPI_MINOR) {
                for (alarms = d->dat.list; alarms; alarms = alarms->next) {
                        SaHpiAlarmT *alarm = alarms->data;
                        if (alarm &&
                            alarm->AlarmCond.Type == SAHPI_STATUS_COND_TYPE_OEM &&
                            alarm->AlarmCond.ResourceId == event->Source &&
                            alarm->AlarmCond.Mid == event->EventDataUnion.OemEvent.MId) {
                                /* Found. Remove alarm */
                                GList *node = alarms->prev;
                                g_free(alarms->data);
                                d->dat.list = g_list_delete_link(d->dat.list, alarms);
                                if (node) alarms = node;
                                else { alarms = d->dat.list; continue; }
                        }
                }
                oh_release_domain(d);
                return;
        }
        
        /* Severity is "alarming". Add/Create OEM alarm */
        a = g_new0(SaHpiAlarmT, 1);
        a->AlarmId = ++(d->dat.next_id);
        gettimeofday(&tv1, NULL);
        a->Timestamp = (SaHpiTimeT) tv1.tv_sec * 1000000000 + tv1.tv_usec * 1000;
        a->Severity = event->Severity;
        a->AlarmCond.Type = SAHPI_STATUS_COND_TYPE_OEM;
        oh_entity_path_lookup(&event->Source, &a->AlarmCond.Entity);
        a->AlarmCond.DomainId = SAHPI_UNSPECIFIED_DOMAIN_ID;
        a->AlarmCond.ResourceId = event->Source;
        a->AlarmCond.Mid = event->EventDataUnion.OemEvent.MId;
        memcpy(&a->AlarmCond.Data, &event->EventDataUnion.OemEvent.OemEventData, sizeof(SaHpiTextBufferT));
        d->dat.list = g_list_append(d->dat.list, a);
        
        oh_release_domain(d);
        return;
}

static void oh_detect_resource_event_alarm(SaHpiDomainIdT did, SaHpiEventT *event)
{
        struct oh_domain *d = NULL;
        struct timeval tv1;
        GList *alarms = NULL;
        SaHpiAlarmT *a = NULL;
        
        d = oh_get_domain(did);
        if (!d) {
                dbg("Domain not found! Can't process alarm trigger.");
                return;
        }
        
        /* Search for possible resource alarm, if event is not a resource failure */
        if (event->EventDataUnion.ResourceEvent.ResourceEventType != SAHPI_RESE_RESOURCE_FAILURE) {
                for (alarms = d->dat.list; alarms; alarms = alarms->next) {
                        SaHpiAlarmT *alarm = alarms->data;
                        if (alarm &&
                            alarm->AlarmCond.Type == SAHPI_STATUS_COND_TYPE_RESOURCE &&
                            alarm->AlarmCond.ResourceId == event->Source) {
                                /* Found. Remove alarm */
                                GList *node = alarms->prev;
                                g_free(alarms->data);
                                d->dat.list = g_list_delete_link(d->dat.list, alarms);
                                if (node) alarms = node;
                                else { alarms = d->dat.list; continue; }
                        }
                }
                oh_release_domain(d);
                return;
        }
        
        /* Failed resource. Add/Create resource alarm if severity is "alarming" */
        if (event->Severity <= SAHPI_MINOR) {
                a = g_new0(SaHpiAlarmT, 1);
                a->AlarmId = ++(d->dat.next_id);
                gettimeofday(&tv1, NULL);
                a->Timestamp = (SaHpiTimeT) tv1.tv_sec * 1000000000 + tv1.tv_usec * 1000;
                a->Severity = event->Severity;
                a->AlarmCond.Type = SAHPI_STATUS_COND_TYPE_RESOURCE;
                oh_entity_path_lookup(&event->Source, &a->AlarmCond.Entity);
                a->AlarmCond.DomainId = SAHPI_UNSPECIFIED_DOMAIN_ID;
                a->AlarmCond.ResourceId = event->Source;
                d->dat.list = g_list_append(d->dat.list, a);
        }
        
        oh_release_domain(d);
        return;
}

static void oh_detect_sensor_event_alarm(SaHpiDomainIdT did, SaHpiEventT *event)
{
        return;
}

static void oh_remove_resource_alarms(SaHpiDomainIdT did, SaHpiResourceIdT rid, int all)
{
        struct oh_domain *d = NULL;
        GList *alarms = NULL;
        
        d = oh_get_domain(did);
        if (!d) {
                dbg("Domain not found! Can't process alarm trigger.");
                return;
        }
        
        for (alarms = d->dat.list; alarms; alarms = alarms->next) {
                SaHpiAlarmT *alarm = alarms->data;
                if (alarm &&
                    alarm->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER &&
                    ((all) ? alarm->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER : alarm->AlarmCond.Type == SAHPI_STATUS_COND_TYPE_RESOURCE) &&
                    alarm->AlarmCond.ResourceId == rid) {
                        /* Found. Remove alarm */
                        GList *node = alarms->prev;
                        g_free(alarms->data);
                        d->dat.list = g_list_delete_link(d->dat.list, alarms);
                        if (node) alarms = node;
                        else { alarms = d->dat.list; continue; }
                }
        }
        oh_release_domain(d);
        return;
}

static void oh_detect_hpi_alarm(SaHpiDomainIdT did, SaHpiEventT *event)
{        
        if (!event) {
                dbg("Invalid Parameters - NULL event passed.");
                return;
        }
        
        switch (event->EventType) {
                case SAHPI_ET_OEM:
                        oh_detect_oem_event_alarm(did, event);
                        break;                        
                case SAHPI_ET_RESOURCE:
                        oh_detect_resource_event_alarm(did, event);
                        break;
                case SAHPI_ET_SENSOR_ENABLE_CHANGE:
                case SAHPI_ET_SENSOR:
                        oh_detect_sensor_event_alarm(did, event);
                        break;
                default:;
        }

        return;
}

static void oh_detect_resource_alarm(SaHpiDomainIdT did, SaHpiRptEntryT *res)
{
        struct oh_domain *d = NULL;
        struct timeval tv1;
        GList *alarms = NULL;
        SaHpiAlarmT *a = NULL;
        
        d = oh_get_domain(did);
        if (!d) {
                dbg("Domain not found! Can't process alarm trigger.");
                return;
        }
        
        /* Check possible alarms for removal, if resource is not failed. */
        if (!res->ResourceFailed || res->ResourceSeverity > SAHPI_MINOR) {
                for (alarms = d->dat.list; alarms; alarms = alarms->next) {
                        SaHpiAlarmT *alarm = alarms->data;
                        if (alarm &&
                            alarm->AlarmCond.Type == SAHPI_STATUS_COND_TYPE_RESOURCE &&
                            alarm->AlarmCond.ResourceId == res->ResourceId) {
                                /* Found. Remove alarm */
                                GList *node = alarms->prev;
                                g_free(alarms->data);
                                d->dat.list = g_list_delete_link(d->dat.list, alarms);
                                if (node) alarms = node;
                                else { alarms = d->dat.list; continue; }
                        }
                }
        } else if (res->ResourceSeverity <= SAHPI_MINOR && res->ResourceFailed) {
                /* Otherwise, if sev is "alarming" and resource failed, create alarm. */
                a = g_new0(SaHpiAlarmT, 1);
                a->AlarmId = ++(d->dat.next_id);
                gettimeofday(&tv1, NULL);
                a->Timestamp = (SaHpiTimeT) tv1.tv_sec * 1000000000 + tv1.tv_usec * 1000;
                a->Severity = res->ResourceSeverity;
                a->AlarmCond.Type = SAHPI_STATUS_COND_TYPE_RESOURCE;
                oh_entity_path_lookup(&res->ResourceId, &a->AlarmCond.Entity);
                a->AlarmCond.DomainId = SAHPI_UNSPECIFIED_DOMAIN_ID;
                a->AlarmCond.ResourceId = res->ResourceId;
                a->AlarmCond.Mid = res->ResourceInfo.ManufacturerId;
                memcpy(&a->AlarmCond.Data, &res->ResourceTag, sizeof(SaHpiTextBufferT));
                d->dat.list = g_list_append(d->dat.list, a);
        }
        
        oh_release_domain(d);
        return;
}

/**
 * oh_detect_event_alarm
 * @did:
 * @e:
 *
 * Return value:
 **/
SaErrorT oh_detect_event_alarm(SaHpiDomainIdT did, struct oh_event *e)
{
        if (!did || !e) return SA_ERR_HPI_INVALID_PARAMS;
        
        switch (e->type) {                
                case OH_ET_HPI:
                        oh_detect_hpi_alarm(did, &e->u.hpi_event.event);
                        break;
                case OH_ET_RESOURCE:
                        oh_detect_resource_alarm(did, &e->u.res_event.entry);
                        break;
                case OH_ET_RESOURCE_DEL:
                        oh_remove_resource_alarms(did, e->u.res_event.entry.ResourceId, 1);
                        break;
                default:;
        }
        
        return SA_OK;
}

/**
 * oh_detect_res_sev_alarm
 * @did:
 * @res:
 * @sev:
 *
 * Return value:
 **/
SaErrorT oh_detect_res_sev_alarm(SaHpiDomainIdT did,
                                 SaHpiRptEntryT *res,
                                 SaHpiSeverityT new_sev)
{
        if (!did || !res) return SA_ERR_HPI_INVALID_PARAMS;
        
        if (res->ResourceSeverity <= SAHPI_MINOR && new_sev > SAHPI_MINOR)
                oh_remove_resource_alarms(did, res->ResourceId, 0);
        
        return SA_OK;
}
 

/**
 * oh_detect_sensor_alarm
 * @did:
 * @rdr:
 * @enable:
 *
 * Return value:
 **/
SaErrorT oh_detect_sensor_alarm(SaHpiDomainIdT did,
                                SaHpiRdrT *rdr,
                                SaHpiBoolT enable)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * oh_detect_sensor_mask_alarm
 * @did:
 * @rdr:
 * @state:
 *
 * Return value:
 **/
SaErrorT oh_detect_sensor_mask_alarm(SaHpiDomainIdT did,
                                     SaHpiRdrT *rdr,
                                     SaHpiEventStateT state)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}
