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

#ifndef _INC_SIM_UTIL_
#define _INC_SIM_UTIL_
#include <fam.h>

typedef struct sim_rdr_id {
        SaHpiResourceIdT rid;
        SaHpiEntryIdT eid;
        int index;
        int reqnum;
} sim_rdr_id_t;

/* rpt table and ids */

int sim_util_rptable_init(RPTable *table);
int sim_util_add_resource(RPTable *table,  SaHpiRptEntryT *rpt, char *file);
int sim_util_remove_resource(RPTable *table, SaHpiResourceIdT rid,
                             FAMConnection *fc);
int sim_util_add_rdr(RPTable *table, SaHpiResourceIdT rid,
                    SaHpiRdrT *rdr, sim_rdr_id_t *sid);

int sim_util_get_res_id(RPTable *table, char *file, SaHpiResourceIdT *rid);
sim_rdr_id_t *sim_util_get_rdr_id(RPTable *table, int reqnum);


/* get file */

char *sim_util_get_rpt_file(struct oh_handler_state *inst,
                            SaHpiResourceIdT rid);

char *sim_util_get_sensor_reading_file(struct oh_handler_state *inst, 
                                       SaHpiResourceIdT rid,
                                       SaHpiSensorNumT num);

char *sim_util_get_sensor_thres_file(struct oh_handler_state *inst,
                                     SaHpiResourceIdT rid, 
                                     SaHpiSensorNumT num);

char *sim_util_get_sensor_enables_file(struct oh_handler_state *inst,
                                       SaHpiResourceIdT rid,
                                       SaHpiSensorNumT num);

char *sim_util_get_rdr_file(struct oh_handler_state *inst,
                                       SaHpiResourceIdT rid,
                                       SaHpiSensorNumT num);
/* for event */

int sim_util_insert_event(GSList **eventq, struct oh_event *event);
int sim_util_remove_event(GSList **eventq, struct oh_event *event);

#endif
