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
