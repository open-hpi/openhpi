/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authours:
 *      Racing Guo <racing.guo@intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SaHpi.h>
#include <openhpi.h>

#include "sim_util.h"
#include "sim_sensor.h"


static int get_sensor_event_state(SaHpiSensorThdDefnT *def,
                                  SaHpiSensorSignFormatT sign,
                                  SaHpiSensorThresholdsT *thres,
                                  SaHpiSensorReadingT *reading,
                                  SaHpiEventStateT *ev_state,
                                  SaHpiSensorReadingT *trigger)
{
#define  CHECK_RAW(x)                                  \
do {                                                   \
        if (!(((x)->ValuesPresent) & SAHPI_SRF_RAW))   \
                 return -1;                            \
}while(0)

#define GET_RAW_EVENT_STATE(r1, r2, p, n, es)          \
do {                                                   \
        if (((r1)->Raw < (r2)->Raw + (p)->Raw) &&      \
            ((r1)->Raw < (r2)->Raw - (p)->Raw)) {      \
                 *ev_state = es;                       \
                 *trigger = *(r2);                     \
                 return 0;                             \
        }                                              \
} while(0)

        if (def->IsThreshold != SAHPI_TRUE)
                return -1;     

        if (def->TholdCapabilities & SAHPI_STC_RAW) {
                if (sign != SAHPI_SDF_UNSIGNED) {
                        printf("not implement for 1s and 2s type\n");
                        return -1;
                }
                CHECK_RAW(reading);
                CHECK_RAW(&thres->LowCritical);
                CHECK_RAW(&thres->LowMajor);
                CHECK_RAW(&thres->LowMinor);
                CHECK_RAW(&thres->UpCritical);
                CHECK_RAW(&thres->UpMajor);
                CHECK_RAW(&thres->UpMinor);
                CHECK_RAW(&thres->PosThdHysteresis);
                CHECK_RAW(&thres->NegThdHysteresis);
                
                GET_RAW_EVENT_STATE(reading, &thres->LowCritical,
                                    &thres->PosThdHysteresis,
                                    &thres->NegThdHysteresis,
                                    SAHPI_ES_LOWER_CRIT);

                GET_RAW_EVENT_STATE(reading, &thres->LowMajor,
                                    &thres->PosThdHysteresis,
                                    &thres->NegThdHysteresis,
                                    SAHPI_ES_LOWER_MAJOR);

                GET_RAW_EVENT_STATE(reading, &thres->UpMinor,
                                    &thres->PosThdHysteresis,
                                    &thres->NegThdHysteresis,
                                    SAHPI_ES_UPPER_MINOR);

                GET_RAW_EVENT_STATE(reading, &thres->UpCritical,
                                    &thres->PosThdHysteresis,
                                    &thres->NegThdHysteresis,
                                    SAHPI_ES_UPPER_CRIT);

                GET_RAW_EVENT_STATE(reading, &thres->UpMajor,
                                    &thres->PosThdHysteresis,
                                    &thres->NegThdHysteresis,
                                    SAHPI_ES_UPPER_MAJOR);

                GET_RAW_EVENT_STATE(reading, &thres->UpMinor,
                                    &thres->PosThdHysteresis,
                                    &thres->NegThdHysteresis,
                                    SAHPI_ES_UPPER_MINOR);
        
        }else if (def->TholdCapabilities & SAHPI_STC_INTERPRETED) {
                /* Fix Me */
                printf("don't implement for interpreted type\n");
                return -1;
        }else {
                return -1;
        }
        return -1;
}
static int generate_sensor_event(SaHpiRptEntryT *rpt,
                                 SaHpiRdrT *rdr,
                                 SaHpiSensorReadingT *reading,
                                 SaHpiSensorThresholdsT *thres,
                                 SaHpiSensorEvtEnablesT *enables,
                                 SaHpiEventT *old_ev,
                                 SaHpiEventT *new_ev)
{
       SaHpiSensorRecT * rec;
       SaHpiSensorEventT *ns_ev;

       if (rdr->RdrType != SAHPI_SENSOR_RDR) 
               return -1;

       rec = &rdr->RdrTypeUnion.SensorRec;

       /* Fix Me:
          if EventCtrl is equal to SAHPI_SEC_PER_EVENT, 
          SAHPI_SEC_GLOBAL_DISABLE or SAHPI_SEC_GLOBAL_DISABLE
       */

       /* Events not supported, therefore don't generate event */
       if (rec->EventCtrl == SAHPI_SEC_NO_EVENTS)
               return -1;

       /* Ignore sensor, therefor don't generate event */       
       if (rec->Ignore == SAHPI_TRUE)
               return -1;       
 
       if (rec->DataFormat.IsNumeric != SAHPI_TRUE)
               return -1;

       memset(new_ev, 0, sizeof(*new_ev));

       new_ev->Source = rpt->ResourceId;
       new_ev->EventType = SAHPI_ET_SENSOR;
      
       new_ev->Timestamp = SAHPI_TIME_UNSPECIFIED;
       new_ev->Severity = rpt->ResourceSeverity;
       ns_ev = &new_ev->EventDataUnion.SensorEvent;
       ns_ev->SensorNum = rec->Num;
       ns_ev->EventCategory = rec->Category;
       
       if (ns_ev->EventCategory != SAHPI_EC_THRESHOLD) {
               /*  Fix Me if event category is not threshold */
               ns_ev->Assertion = SAHPI_TRUE;
               ns_ev->EventState = 0;
       }else {

               if (!get_sensor_event_state(&rec->ThresholdDefn,
                                           rec->DataFormat.SignFormat,
                                           thres,
                                           reading,
                                           &ns_ev->EventState,
                                           &ns_ev->TriggerThreshold))
                     return -1;

               /* Fix Me */
               if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)
                       ns_ev->Assertion = SAHPI_FALSE;
               else
                       ns_ev->Assertion = SAHPI_TRUE;

               ns_ev->TriggerReading = *reading;
       }

       if ((old_ev != NULL) && (old_ev->EventType == SAHPI_ET_SENSOR))
               ns_ev->PreviousState = old_ev->EventDataUnion.SensorEvent.EventState;
       if (ns_ev->PreviousState == ns_ev->EventState)
               return -1;
       return 0;
}

int sim_sensor_update(struct oh_handler_state *inst,
                      SaHpiResourceIdT rid,
                      SaHpiSensorNumT num)
{
        SaHpiRptEntryT  rpt;
        SaHpiRdrT  rdr;
        SaHpiSensorReadingT  reading;
        SaHpiSensorThresholdsT  thres;
        SaHpiSensorEvtEnablesT  enables;
    
        struct oh_event *ev;
        int retval;

        ev = g_malloc0(sizeof(*ev));

        if (!ev)
                return -1;

        ev->type = OH_ET_HPI;
        ev->u.hpi_event.parent = rpt.ResourceId;
        ev->u.hpi_event.id = get_rdr_uid(SAHPI_SENSOR_RDR, num);
        
        retval = generate_sensor_event(&rpt, &rdr, &reading, &thres, 
                                       &enables, NULL, &ev->u.hpi_event.event);
        if (retval)
                return -1;

        sim_util_insert_event(&inst->eventq, ev);
        return 0;
}
