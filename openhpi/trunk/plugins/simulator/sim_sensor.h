#ifndef _INC_SENSOR_H_
#define _INC_SENSOR_H_

int generate_sensor_event(SaHpiRptEntryT *rpt,
                          SaHpiRdrT *rdr,
                          SaHpiSensorReadingT *reading,
                          SaHpiSensorThresholdsT *thres,
                          SaHpiSensorEvtEnablesT *enables,
                          SaHpiEventT *old_ev,
                          SaHpiEventT *new_ev);

#endif
