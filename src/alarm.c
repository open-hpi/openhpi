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
#include <oh_config.h>
#include <oh_error.h>
#include <oh_utils.h>

static void __update_dat(struct oh_domain *d)
{
        struct timeval tv;

        if (!d) return;

        gettimeofday(&tv, NULL);

        d->dat.update_count++;
        d->dat.update_timestamp = (SaHpiTimeT) tv.tv_sec * 1000000000 + tv.tv_usec * 1000;
}

static GSList *__get_alarm_node(struct oh_domain *d,
                               SaHpiAlarmIdT *aid,
                               SaHpiSeverityT *severity,
                               SaHpiStatusCondTypeT *type,
                               SaHpiResourceIdT *rid,
                               SaHpiManufacturerIdT *mid,
                               SaHpiSensorNumT *num,
                               SaHpiEventStateT *state,
                               SaHpiBoolT unacknowledged,
                               int get_next)
{
        GSList *alarms = NULL;

        if (!d) return NULL;

        if (aid) {
                if (*aid == SAHPI_FIRST_ENTRY)
                        get_next = 1;
                else if (*aid == SAHPI_LAST_ENTRY) {
                        /* Just return the last node,
                           if not getting next alarm. */
                        if (get_next)
                                return NULL;
                        else
                                return g_slist_last(d->dat.list);
                }
        }


        for (alarms = d->dat.list; alarms; alarms = alarms->next) {
                SaHpiAlarmT *alarm = alarms->data;
                if (alarm &&
                    (aid ? (get_next ? alarm->AlarmId > *aid : alarm->AlarmId == *aid) : 1) &&
                    (severity ? (*severity != SAHPI_ALL_SEVERITIES ? alarm->Severity == *severity : 1) : 1) &&
                    (type ? alarm->AlarmCond.Type == *type : 1) &&
                    (rid ? alarm->AlarmCond.ResourceId == *rid: 1) &&
                    (mid ? alarm->AlarmCond.Mid == *mid : 1) &&
                    (num ? alarm->AlarmCond.SensorNum == *num : 1) &&
                    (state ? alarm->AlarmCond.EventState == *state : 1) &&
                    (unacknowledged ? !alarm->Acknowledged : 1)) {
                        return alarms;
                }
        }

        return NULL;
}

static SaHpiUint32T __count_alarms(struct oh_domain *d,
                                   SaHpiStatusCondTypeT *type,
                                   SaHpiSeverityT sev)
{
        GSList *alarms = NULL;
        SaHpiUint32T count = 0;

        if (!d) return 0;

        if (!type && sev == SAHPI_ALL_SEVERITIES)
                return g_slist_length(d->dat.list);
        else {
                for (alarms = d->dat.list; alarms; alarms = alarms->next) {
                        SaHpiAlarmT *alarm = alarms->data;
                        if (alarm &&
                            (type ? alarm->AlarmCond.Type == *type : 1) &&
                            (sev == SAHPI_ALL_SEVERITIES ? 1 : alarm->Severity == sev)) {
                                count++;
                        }
                }
        }

        return count;
}

/**
 * oh_add_alarm
 * @d:
 * @alarm:
 *
 * Return value:
 **/
SaHpiAlarmT *oh_add_alarm(struct oh_domain *d, SaHpiAlarmT *alarm)
{
        struct timeval tv1;
        SaHpiAlarmT *a = NULL;
        struct oh_global_param param = { .type = OPENHPI_DAT_SIZE_LIMIT };

        if (!d) {
                dbg("NULL domain pointer passed.");
                return NULL;
        }

        if (oh_get_global_param(&param))
                param.u.dat_size_limit = OH_MAX_DAT_SIZE_LIMIT;

        if (param.u.dat_size_limit != OH_MAX_DAT_SIZE_LIMIT &&
            g_slist_length(d->dat.list) >= param.u.dat_size_limit) {
                dbg("DAT for domain %d is overflowed", d->id);
                d->dat.overflow = SAHPI_TRUE;
                return NULL;
        } else if (alarm && alarm->AlarmCond.Type == SAHPI_STATUS_COND_TYPE_USER) {
                param.type = OPENHPI_DAT_USER_LIMIT;
                if (oh_get_global_param(&param))
                        param.u.dat_user_limit = OH_MAX_DAT_USER_LIMIT;

                if (param.u.dat_user_limit != OH_MAX_DAT_USER_LIMIT &&
                    __count_alarms(d,
                                   &alarm->AlarmCond.Type,
                                   SAHPI_ALL_SEVERITIES) >= param.u.dat_user_limit) {
                        dbg("DAT for domain %d has reached its user alarms limit", d->id);
                        return NULL;
                }
        }

        a = g_new0(SaHpiAlarmT, 1);
        /* Is this 'if' below needed? g_new will stop program
         * on a failed malloc anyway. Just in case...
         */
        if (!a) return NULL;

        if (alarm) /* Copy contents of optional alarm reference */
                memcpy(a, alarm, sizeof(SaHpiAlarmT));
        a->AlarmId = ++(d->dat.next_id);
        gettimeofday(&tv1, NULL);
        a->Timestamp = (SaHpiTimeT) tv1.tv_sec * 1000000000 + tv1.tv_usec * 1000;
        a->AlarmCond.DomainId = d->id;
        d->dat.list = g_slist_append(d->dat.list, a);

        /* Set alarm id and timestamp info in alarm reference */
        if (alarm) {
                alarm->AlarmId = a->AlarmId;
                alarm->Timestamp = a->Timestamp;
        }

        __update_dat(d);

        return a;
}

/**
 * oh_get_alarm
 * @d:
 * @aid:
 * @severity:
 * @type:
 * @rid:
 * @mid:
 * @num:
 * @state:
 * @unacknowledged:
 * @get_next:
 *
 * Return value:
 **/
SaHpiAlarmT *oh_get_alarm(struct oh_domain *d,
                          SaHpiAlarmIdT *aid,
                          SaHpiSeverityT *severity,
                          SaHpiStatusCondTypeT *type,
                          SaHpiResourceIdT *rid,
                          SaHpiManufacturerIdT *mid,
                          SaHpiSensorNumT *num,
                          SaHpiEventStateT *state,
                          SaHpiBoolT unacknowledged,
                          int get_next)
{
        GSList *alarm_node = NULL;

        if (!d) return NULL;

        alarm_node = __get_alarm_node(d, aid, severity, type, rid, mid, num,
                                      state, unacknowledged, get_next);
        if (!alarm_node) return NULL;

        return alarm_node->data;
}

/**
 * oh_remove_alarm
 * @d:
 * @severity:
 * @type:
 * @rid:
 * @mid:
 * @num:
 * @state:
 * @deassert_mask:
 * @multi:
 *
 * Return value:
 **/
SaErrorT oh_remove_alarm(struct oh_domain *d,
                         SaHpiSeverityT *severity,
                         SaHpiStatusCondTypeT *type,
                         SaHpiResourceIdT *rid,
                         SaHpiManufacturerIdT *mid,
                         SaHpiSensorNumT *num,
                         SaHpiEventStateT *state,
                         SaHpiEventStateT *deassert_mask,
                         int multi)
{
        GSList *alarm_node = NULL;
        SaHpiAlarmT *alarm = NULL;
	SaHpiAlarmIdT saw_aid = SAHPI_FIRST_ENTRY; /* Set to zero */
        struct oh_global_param param = { .type = OPENHPI_DAT_SIZE_LIMIT };

        if (!d) return SA_ERR_HPI_INVALID_PARAMS;

        do {
                alarm_node = __get_alarm_node(d, NULL, severity, type, rid, mid,
                                              num, state, 0, 0);
                if (alarm_node) alarm = alarm_node->data;
		else break;

		if (alarm->AlarmId == saw_aid) break; /* Avoid infinite loop case */

		saw_aid = alarm->AlarmId;

                if (alarm &&
                    (deassert_mask ? *deassert_mask & alarm->AlarmCond.EventState : 1)) {
                        d->dat.list = g_slist_delete_link(d->dat.list, alarm_node);
                        g_free(alarm);
                }

                alarm_node = NULL;
                alarm = NULL;
        } while (multi);

        __update_dat(d);
        if (!oh_get_global_param(&param)) { /* Reset overflow flag if not overflowed */
                if (param.u.dat_size_limit != OH_MAX_DAT_SIZE_LIMIT &&
                    g_slist_length(d->dat.list) < param.u.dat_size_limit)
                        d->dat.overflow = SAHPI_FALSE;
        }

        return SA_OK;
}

/**
 * oh_close_alarmtable
 * @d:
 *
 * Return value:
 **/
SaErrorT oh_close_alarmtable(struct oh_domain *d)
{
        SaErrorT error = SA_OK;

        if (!d) return SA_ERR_HPI_INVALID_PARAMS;

        error = oh_remove_alarm(d, NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL, 1);
        d->dat.next_id = 0;
        d->dat.update_count = 0;
        d->dat.update_timestamp = SAHPI_TIME_UNSPECIFIED;

        return error;
}

/**
 * oh_count_alarms
 * @d:
 * @sev:
 *
 * Return value:
 **/
SaHpiUint32T oh_count_alarms(struct oh_domain *d, SaHpiSeverityT sev)
{
        SaHpiUint32T count = 0;

        count = __count_alarms(d, NULL, sev);

        return count;
}

static void oh_detect_oem_event_alarm(struct oh_domain *d, SaHpiEventT *event)
{
        SaHpiAlarmT *a = NULL;
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_OEM;

        if (!d || !event) return;

        /* Search for possible oem alarm, if severity is "non-alarming" */
        if (event->Severity > SAHPI_MINOR) {
                oh_remove_alarm(d, NULL, &type, &event->Source,
                                &event->EventDataUnion.OemEvent.MId,
                                NULL, NULL, NULL, 1);
                return;
        }

        /* Severity is "alarming". Add/Create OEM alarm */
        a = oh_add_alarm(d, NULL);
        if (!a) goto done;
        a->Severity = event->Severity;
        a->AlarmCond.Type = SAHPI_STATUS_COND_TYPE_OEM;
        oh_entity_path_lookup(&event->Source, &a->AlarmCond.Entity);
        a->AlarmCond.ResourceId = event->Source;
        a->AlarmCond.Mid = event->EventDataUnion.OemEvent.MId;
        memcpy(&a->AlarmCond.Data,
               &event->EventDataUnion.OemEvent.OemEventData,
               sizeof(SaHpiTextBufferT));
done:
        return;
}

static void oh_detect_resource_event_alarm(struct oh_domain *d, SaHpiEventT *event)
{
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_RESOURCE;
        SaHpiAlarmT *a = NULL;

        if (!d || !event) return;

        /* Search for possible resource alarm, if event is not a resource failure */
        if (event->EventDataUnion.ResourceEvent.ResourceEventType != SAHPI_RESE_RESOURCE_FAILURE) {
                oh_remove_alarm(d, NULL, &type, &event->Source, NULL,
                                NULL, NULL, NULL, 1);
                return;
        }

        /* Failed resource. Add/Create resource alarm if severity is "alarming" */
        if (event->Severity <= SAHPI_MINOR) {
                a = oh_add_alarm(d, NULL);
                if (!a) goto done;
                a->Severity = event->Severity;
                a->AlarmCond.Type = SAHPI_STATUS_COND_TYPE_RESOURCE;
                oh_entity_path_lookup(&event->Source, &a->AlarmCond.Entity);
                a->AlarmCond.ResourceId = event->Source;
        }
done:
        return;
}

static void oh_detect_sensor_event_alarm(struct oh_domain *d, SaHpiEventT *event)
{
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_SENSOR;
        SaHpiAlarmT *a = NULL;

        if (!d || !event) return;

        if (!event->EventDataUnion.SensorEvent.Assertion) {
                /* Check for possible sensor alarm removals,
                   since sensor is not asserted. */
                oh_remove_alarm(d, NULL, &type, &event->Source, NULL,
                                &event->EventDataUnion.SensorEvent.SensorNum,
                                &event->EventDataUnion.SensorEvent.EventState,
                                NULL, 1);
        } else if (event->Severity <= SAHPI_MINOR &&
                   event->EventDataUnion.SensorEvent.Assertion) {
                /* Add sensor alarm to dat, since event is severe
                   enough and is asserted. */
                a = oh_add_alarm(d, NULL);
                if (!a) goto done;
                a->Severity = event->Severity;
                a->AlarmCond.Type = SAHPI_STATUS_COND_TYPE_SENSOR;
                oh_entity_path_lookup(&event->Source, &a->AlarmCond.Entity);
                a->AlarmCond.ResourceId = event->Source;
                a->AlarmCond.SensorNum = event->EventDataUnion.SensorEvent.SensorNum;
                a->AlarmCond.EventState = event->EventDataUnion.SensorEvent.EventState;
        }
done:
        return;
}

static void oh_remove_resource_alarms(struct oh_domain *d, SaHpiResourceIdT rid, int all)
{
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_RESOURCE;

        if (!d || !rid) return;

        oh_remove_alarm(d, NULL, &type, &rid, NULL,
                        NULL, NULL, NULL, 1);
        if (all) {
                type = SAHPI_STATUS_COND_TYPE_OEM;
                oh_remove_alarm(d, NULL, &type, &rid, NULL,
                                NULL, NULL, NULL, 1);
                type = SAHPI_STATUS_COND_TYPE_SENSOR;
                oh_remove_alarm(d, NULL, &type, &rid, NULL,
                                NULL, NULL, NULL, 1);
        }

        return;
}

static void oh_detect_hpi_alarm(struct oh_domain *d, SaHpiEventT *event)
{
        if (!d || !event) return;

        switch (event->EventType) {
                case SAHPI_ET_OEM:
                        oh_detect_oem_event_alarm(d, event);
                        break;
                case SAHPI_ET_RESOURCE:
                        oh_detect_resource_event_alarm(d, event);
                        break;
                case SAHPI_ET_SENSOR_ENABLE_CHANGE:
                case SAHPI_ET_SENSOR:
                        oh_detect_sensor_event_alarm(d, event);
                        break;
                default:;
        }

        return;
}

static void oh_detect_resource_alarm(struct oh_domain *d, SaHpiRptEntryT *res)
{
        SaHpiAlarmT *a = NULL;
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_SENSOR;

        if (!d || !res) return;

        /* Check possible alarms for removal, if resource is not failed. */
        if (!res->ResourceFailed || res->ResourceSeverity > SAHPI_MINOR) {
                oh_remove_alarm(d, NULL, &type, &res->ResourceId, NULL,
                                NULL, NULL, NULL, 1);
        } else if (res->ResourceSeverity <= SAHPI_MINOR && res->ResourceFailed) {
                /* Otherwise, if sev is "alarming" and resource failed, create alarm. */
                a = oh_add_alarm(d, NULL);
                if (!a) goto done;
                a->Severity = res->ResourceSeverity;
                a->AlarmCond.Type = SAHPI_STATUS_COND_TYPE_RESOURCE;
                oh_entity_path_lookup(&res->ResourceId, &a->AlarmCond.Entity);
                a->AlarmCond.ResourceId = res->ResourceId;
                a->AlarmCond.Mid = res->ResourceInfo.ManufacturerId;
                memcpy(&a->AlarmCond.Data, &res->ResourceTag, sizeof(SaHpiTextBufferT));
        }
done:
        return;
}

/**
 * oh_detect_event_alarm
 * @d:
 * @e:
 *
 * Return value:
 **/
SaErrorT oh_detect_event_alarm(struct oh_event *e)
{
        struct oh_domain *d = NULL;

        if (!e || !e->did) return SA_ERR_HPI_INVALID_PARAMS;

        d = oh_get_domain(e->did);
        if (!d) return SA_ERR_HPI_INVALID_DOMAIN;

        switch (e->type) {
                case OH_ET_HPI:
                        oh_detect_hpi_alarm(d, &e->u.hpi_event.event);
                        break;
                case OH_ET_RESOURCE:
                        oh_detect_resource_alarm(d, &e->u.res_event.entry);
                        break;
                case OH_ET_RESOURCE_DEL:
                        oh_remove_resource_alarms(d, e->u.res_event.entry.ResourceId, 1);
                        break;
                default:;
        }
        oh_release_domain(d);
        return SA_OK;
}

/**
 * oh_detect_res_sev_alarm
 * @d:
 * @res:
 * @new_sev:
 *
 * Return value:
 **/
SaErrorT oh_detect_res_sev_alarm(SaHpiDomainIdT did,
                                 SaHpiResourceIdT rid,
                                 SaHpiSeverityT new_sev)
{
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *res = NULL;

        if (!did || !rid) return SA_ERR_HPI_INVALID_PARAMS;

        d = oh_get_domain(did);
        if (!d) return SA_ERR_HPI_INVALID_DOMAIN;

        res = oh_get_resource_by_id(&d->rpt, rid);
        if (!res) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        if (res->ResourceSeverity <= SAHPI_MINOR && new_sev > SAHPI_MINOR)
                oh_remove_resource_alarms(d, res->ResourceId, 0);

        oh_release_domain(d);
        return SA_OK;
}


/**
 * oh_detect_sensor_alarm
 * @d:
 * @rid:
 * @num:
 * @enable:
 *
 * Return value:
 **/
SaErrorT oh_detect_sensor_enable_alarm(SaHpiDomainIdT did,
                                       SaHpiResourceIdT rid,
                                       SaHpiSensorNumT num,
                                       SaHpiBoolT enable)
{
        struct oh_domain *d = NULL;
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_SENSOR;
        SaErrorT error = SA_OK;

        if (!did || !rid) return SA_ERR_HPI_INVALID_PARAMS;

        /* Only need to scan alarm table if enable is false */
        if (enable) return SA_OK;

        d = oh_get_domain(did);
        if (!d) return SA_ERR_HPI_INVALID_DOMAIN;

        /* Enable is false, so scan alarm table and remove any matching sensor alarms */
        error = oh_remove_alarm(d, NULL, &type, &rid, NULL,
                                &num, NULL, NULL, 1);

        oh_release_domain(d);
        return error;
}

/**
 * oh_detect_sensor_mask_alarm
 * @d
 * @rid
 * @num
 * @action
 * @deassert_mask
 *
 *
 * Return value:
 **/
SaErrorT oh_detect_sensor_mask_alarm(SaHpiDomainIdT did,
                                     SaHpiResourceIdT rid,
                                     SaHpiSensorNumT num,
                                     SaHpiSensorEventMaskActionT action,
                                     SaHpiEventStateT deassert_mask)
{
        struct oh_domain *d = NULL;
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_SENSOR;
        SaErrorT error = SA_OK;

        if (!did || !rid) return SA_ERR_HPI_INVALID_PARAMS;

        if (action == SAHPI_SENS_ADD_EVENTS_TO_MASKS)
                return SA_OK;

        if (action != SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS)
                return SA_ERR_HPI_INVALID_PARAMS;

        d = oh_get_domain(did);
        if (!d) return SA_ERR_HPI_INVALID_DOMAIN;

        /* Find matching sensor alarms and compare alarm's state with
           the deassert mask. If deassert for that state is being disabled
           on the sensor, then remove the alarm.
        */
        error = oh_remove_alarm(d, NULL, &type, &rid, NULL,
                                &num, NULL, &deassert_mask, 1);

        oh_release_domain(d);
        return error;
}
