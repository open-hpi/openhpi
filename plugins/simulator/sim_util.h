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

struct sim_rdr_id {
   int reqnum;
   SaHpiEntryIdT  rdr_id;
};

struct sim_res_id {
    SaHpiResourceIdT res_id;
    char *file;
    SaHpiEntityPathT epath;
    GSList *rdr_list;
};

GSList *sim_util_add_res_id(GSList *ids,
                            char *file,
                            SaHpiResourceIdT res_id,
                            SaHpiEntityPathT *epath);
struct sim_res_id *sim_util_get_res_id_by_reqnum(GSList *ids, int reqnum);
GSList* sim_util_free_res_id(GSList* ids, char *file);
GSList* sim_util_free_all_res_id(GSList *ids);

void sim_util_add_rdr_id(GSList *ids,
                         char *file,
                         int reqnum,
                         SaHpiEntryIdT  rdr_id);
struct sim_rdr_id *sim_util_get_rdr_id(GSList *ids,
                                       char *file,
                                       int index);

int sim_util_get_rdr_by_sensornum(RPTable *table,
                                  SaHpiResourceIdT res_id,
                                  SaHpiSensorNumT num,
                                  SaHpiEntryIdT *eid);
int sim_util_get_res_id(RPTable *table, char *filename, SaHpiResourceIdT *rid);
char *sim_util_get_sensor_dir(struct oh_handler_state *inst, 
                              SaHpiResourceIdT id,
                              SaHpiSensorNumT num);
char *sim_util_get_rdr_dir(struct oh_handler_state *inst,
                           SaHpiResourceIdT res_id, 
                           SaHpiEntryIdT rd_id);

int sim_util_insert_event(GSList *eventq, struct oh_event *event);
int sim_util_remove_event(GSList *eventq, struct oh_event *event);
#endif
