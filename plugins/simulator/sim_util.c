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
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#include <SaHpi.h>
#include <openhpi.h>

#include "sim_util.h"


#define trace(x, ...) printf(x,  __VA_ARGS__)

static pthread_mutex_t util_mutext = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

int sim_util_rptable_init(RPTable *table)
{
        return 0;
}

int sim_util_add_resource(RPTable *table,  SaHpiRptEntryT *rpt, char *file)
{
        int retval;

        pthread_mutex_lock(&util_mutext);
        retval = oh_add_resource(table, rpt, g_strdup(file), 0);
        pthread_mutex_unlock(&util_mutext);

        return retval;
}

int sim_util_remove_resource(RPTable *table, SaHpiResourceIdT rid,
                             FAMConnection *fc)
{

        sim_rdr_id_t *sid;
        SaHpiEntryIdT eid;
        SaHpiRdrT *rdr;
      
        pthread_mutex_lock(&util_mutext);
      
        eid = SAHPI_FIRST_ENTRY;
        rdr = oh_get_rdr_next(table, rid, eid);
        for ( ; rdr; eid = rdr->RecordId, rdr = oh_get_rdr_next(table, rid, eid)) {
                sid = oh_get_rdr_data(table, rid, eid);
                if (sid){
                        if (fc) {
                                FAMRequest fr;
                                fr.reqnum = sid->reqnum;
                                FAMCancelMonitor(fc, &fr);
                        }
                        g_free(sid);
                }
        }

        oh_remove_resource(table, rid);

        pthread_mutex_unlock(&util_mutext);

        return 0;
}

int sim_util_add_rdr(RPTable *table, SaHpiResourceIdT rid, 
                     SaHpiRdrT *rdr, sim_rdr_id_t *sid)
{
        int retval;

        pthread_mutex_lock(&util_mutext);
        retval = oh_add_rdr(table, rid, rdr, sid, 1);
        pthread_mutex_unlock(&util_mutext);

        return retval;
}

sim_rdr_id_t *sim_util_get_rdr_id(RPTable *table, int reqnum)
{ 
        sim_rdr_id_t *sid, *ret_id = NULL;
        SaHpiRptEntryT *rpt;
        SaHpiRdrT   *rdr;
        SaHpiResourceIdT rid;
        SaHpiEntryIdT eid;

        pthread_mutex_lock(&util_mutext);
        rid = SAHPI_FIRST_ENTRY;
        rpt = oh_get_resource_next(table, rid);

        for ( ; rpt; rid = rpt->ResourceId, 
                     rpt = oh_get_resource_next(table, rid)) {
                
                eid = SAHPI_FIRST_ENTRY;
                rdr = oh_get_rdr_next(table, rid, eid);
                
                for ( ; rdr; eid = rdr->RecordId,
                             rdr = oh_get_rdr_next(table, rid, eid)) {
                        sid = oh_get_rdr_data(table, rid, eid);
                        if (sid) {
                                if (sid->reqnum == reqnum) {
                                      sid->eid = eid;
                                      ret_id = sid;
                                      goto out;
                                }
                        }
                }
        }
out:
        pthread_mutex_unlock(&util_mutext);
        return ret_id;
}

int sim_util_get_res_id(RPTable *table, char *filename, SaHpiResourceIdT *rid)
{
        SaHpiRptEntryT *entry;
        SaHpiResourceIdT  id;
        int retval = -1;

        pthread_mutex_lock(&util_mutext);
        for (id = SAHPI_FIRST_ENTRY, entry = oh_get_resource_next(table, id);
             entry; entry = oh_get_resource_next(table, entry->ResourceId)) {
                char* data;
                data = (char*) oh_get_resource_data(table, entry->ResourceId);
                if (data == NULL) continue;
                if (!strcmp(data, filename)) {
                        *rid = entry->ResourceId;
                        retval = 0;
                        goto out;
                }
        }
out:
        pthread_mutex_unlock(&util_mutext);
        return retval;
}

char *sim_util_get_rpt_file(struct oh_handler_state *inst,
                            SaHpiResourceIdT rid)
{
        char *str1, *str2, *str = NULL;
     
        pthread_mutex_lock(&util_mutext);

        str1 = g_hash_table_lookup(inst->config, "root_path");
        str2 = (char*) oh_get_resource_data(inst->rptcache, rid);

        if (!str1 || !str2)
                goto out;

        str = g_malloc0(strlen(str1) + strlen(str2) + 10);
        if (!str)
                goto out;

        sprintf(str, "%s/%s/rpt", str1, str2);
        trace("%s/%s/rpt\n", str1, str2);
out:
        pthread_mutex_unlock(&util_mutext);
        return str;
}

#define SIM_UTIL_GET_SENSOR_FILE(fun_name, str_x)                  \
char * fun_name(struct oh_handler_state *inst,                     \
                SaHpiResourceIdT rid,                              \
                SaHpiSensorNumT num)                               \
{                                                                  \
        char *str1, *str2, *str = NULL;                            \
        SaHpiEntryIdT eid;                                         \
        sim_rdr_id_t *sid;                                         \
                                                                   \
        pthread_mutex_lock(&util_mutext);                          \
                                                                   \
        eid = get_rdr_uid(SAHPI_SENSOR_RDR, num);                  \
        sid = oh_get_rdr_data(inst->rptcache, rid, eid);           \
                                                                   \
        str1 = g_hash_table_lookup(inst->config, "root_path");     \
        str2 = (char*) oh_get_resource_data(inst->rptcache, rid);  \
                                                                   \
        if (!str1 || !str2 || !sid)                                \
                goto out;                                          \
                                                                   \
        str = (char *)g_malloc0(strlen(str1) + strlen(str2) + 30); \
        if (!str)                                                  \
                goto out;                                          \
                                                                   \
        sprintf(str, str_x, str1, str2, sid->index);               \
        trace(str_x"\n", str1, str2, sid->index);                  \
                                                                   \
out:                                                               \
        pthread_mutex_unlock(&util_mutext);                        \
        return str;                                                \
}
SIM_UTIL_GET_SENSOR_FILE(sim_util_get_sensor_reading_file,
                         "%s/%s/%x/sensor/reading")
SIM_UTIL_GET_SENSOR_FILE(sim_util_get_sensor_thres_file,
                         "%s/%s/%x/sensor/thres")
SIM_UTIL_GET_SENSOR_FILE(sim_util_get_sensor_enables_file,
                         "%s/%s/%x/sensor/enables")


int sim_util_insert_event(GSList **eventq, struct oh_event *event)
{
        pthread_mutex_lock(&util_mutext);

        *eventq = g_slist_append(*eventq, event);
        pthread_mutex_unlock(&util_mutext);
        return 0;
}
int sim_util_remove_event(GSList **eventq, struct oh_event *event)
{
        int retval = 1;
        gpointer data;

        pthread_mutex_lock(&util_mutext);
        data = g_slist_nth_data(*eventq, 0);
        if (data == NULL)
                retval = 0;
        else {
                memcpy(event, data, sizeof(*event));
                *eventq = g_slist_remove(*eventq, data);
        }
        pthread_mutex_unlock(&util_mutext);
        return retval;
}

#ifdef UNIT_TEST
/*
  I will move test code to another file
 */

static void sim_util_res_id_print(GSList *ids)
{
        printf("--- Resouce Id ---\n");
        for ( ; ids; ids = g_slist_next(ids)) {
                struct sim_res_id *res_id;
                GSList *list;

                res_id = (struct sim_res_id *)ids->data;
                if (res_id == NULL) continue;

                printf("file:%s\nreqnum:", res_id->file);

                for (list = res_id->rdr_list; list; list = g_slist_next(list)) {
                        printf("%d,", ((struct sim_rdr_id *)(list->data))->reqnum);
                }
                printf("\n");
        }
        printf("\n");
        return;
}

int main()
{
        GSList * res_id = NULL;
	SaHpiEntityPathT epath;
        struct sim_res_id *id;

        res_id = sim_util_add_res_id(res_id, "test1", 1, &epath);
        sim_util_res_id_print(res_id);
        res_id = sim_util_add_res_id(res_id, "test2", 2, &epath);
        sim_util_res_id_print(res_id); 

        sim_util_add_rdr_id(res_id, "test1",  11, 11);
        sim_util_res_id_print(res_id);
        sim_util_add_rdr_id(res_id, "test1",  12, 12);
        sim_util_res_id_print(res_id);

        sim_util_add_rdr_id(res_id, "test2",  21, 21);
        sim_util_res_id_print(res_id);
        sim_util_add_rdr_id(res_id, "test2",  22, 22);
        sim_util_res_id_print(res_id);

        id = sim_util_get_res_id_by_reqnum(res_id, 11);
        if (id == NULL) {
                printf("cannot get correct resource id\n");
                return -1;
        }
        if (strcmp(id->file, "test1")) {
                printf("cannot get correct resource id\n");
                return -1;
        }

        id = sim_util_get_res_id_by_reqnum(res_id, 12);
        if (id == NULL) {
                printf("cannot get correct resource id\n");
                return -1;
        }
        if (strcmp(id->file, "test1")) {
                printf("cannot get correct resource id\n");
                return -1;
        }

        id = sim_util_get_res_id_by_reqnum(res_id, 21);
        if (id == NULL) {
                printf("cannot get correct resource id\n");
                return -1;
        }
        if (strcmp(id->file, "test2")) {
                printf("cannot get correct resource id\n");
                return -1;
        }

        res_id = sim_util_free_res_id(res_id, "test1");
        sim_util_res_id_print(res_id);

        id = sim_util_get_res_id_by_reqnum(res_id, 11);
        if (id != NULL) {
                printf("cannot free reource id\n");
                return -1;
        }

         
        res_id =  sim_util_free_all_res_id(res_id);
        sim_util_res_id_print(res_id);

        id = sim_util_get_res_id_by_reqnum(res_id, 21);
        if (id != NULL) {
                printf("cannot free reource id\n");
                return -1;
        }

        return 0;
}

#endif
