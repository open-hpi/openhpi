#ifndef _INC_SENSOR_H_
#define _INC_SENSOR_H_

int sim_sensor_update(struct oh_handler_state *inst,
                      SaHpiResourceIdT rid,
                      SaHpiSensorNumT num);
#endif
