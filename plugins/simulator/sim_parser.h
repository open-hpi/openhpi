#ifndef _INC_SIM_PARSER
#define _INC_SIM_PARSER

int sim_parser_get_rpt(char *filename, SaHpiRptEntryT *rpt);
int sim_parser_get_rdr(char *filename, SaHpiRdrT *rdr);
int sim_parser_get_sensor_enables(char *filename, SaHpiSensorEvtEnablesT *enables);
int sim_parser_set_sensor_enables(char *filename, const SaHpiSensorEvtEnablesT *enables);
int sim_parser_get_sensor_reading(char *filename, SaHpiSensorReadingT *reading);
int sim_parser_set_sensor_reading(char *filename, const SaHpiSensorReadingT *reading);
int sim_parser_get_sensor_thres(char *filename, SaHpiSensorThresholdsT *thres);
int sim_parser_set_sensor_thres(char *filename, const SaHpiSensorThresholdsT *thres);

#endif
