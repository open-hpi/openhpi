#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#include <SaHpi.h>
#include <openhpi.h>

#include "sim_util.h"


GSList *sim_util_add_res_id(GSList *ids,
                            char *file,
                            SaHpiResourceIdT res_id,
                            SaHpiEntityPathT *epath)
{
        struct sim_res_id * tmp;

        tmp = g_malloc0(sizeof(*tmp));
        if (tmp == NULL)
                goto out; 

        tmp->file = g_strdup(file);
        if (tmp->file == NULL)
                goto out1;

        tmp->res_id = res_id;
        tmp->epath = *epath;

        return g_slist_append(ids, tmp);
out1:    
        g_free(tmp);
out:
        return ids;
}

struct sim_res_id *sim_util_get_res_id_by_reqnum(GSList *ids, int reqnum)
{
        for ( ; ids; ids = g_slist_next(ids)) {    
                struct sim_res_id *res_id;
                GSList *list;

                res_id = (struct sim_res_id *)ids->data;
                if (res_id == NULL) continue;
                for (list = res_id->rdr_list; list; list = g_slist_next(list)) {
                        struct sim_rdr_id *rdr_id;
                        rdr_id = (struct sim_rdr_id *)list->data;
                        if (rdr_id->reqnum == reqnum) 
                                return res_id;  
                }
        }
        return NULL;
}

GSList* sim_util_free_res_id(GSList* ids, char *file)
{
        GSList *res_list;

        for ( res_list = ids; res_list; res_list = g_slist_next(res_list)) {
                struct sim_res_id *res_id;

                res_id = (struct sim_res_id *)res_list->data;
                if (res_id == NULL) continue;
                if (!strcmp(res_id->file, file)) {
                        GSList *list;
                        
                        for (list = res_id->rdr_list; list; list = g_slist_next(list)) {
                            g_free((struct sim_rdr_id *) list->data);
                        }
                        g_slist_free(res_id->rdr_list);
                        g_free(res_id->file);
                        g_free(res_id);
                        return g_slist_remove_link(ids, res_list);
                }
        }
        return ids;
}

void sim_util_free_all_res_id(GSList *ids)
{
          for ( ; ids; ids = g_slist_next(ids)) {
                struct sim_res_id *res_id;
                GSList *list;

                res_id = (struct sim_res_id *)ids->data;
                if (res_id == NULL) continue;
                for (list = res_id->rdr_list; list; list = g_slist_next(list)) {
                     g_free((struct sim_rdr_id *) list->data);
                }
                g_slist_free(res_id->rdr_list);
                g_free(res_id->file);
                g_free(res_id);
        }
        g_slist_free(ids);
        return;
}

void sim_util_add_rdr_id(GSList *ids,
                         char *file,
                         int reqnum,
                         SaHpiEntryIdT  rid)
{
         for ( ; ids; ids = g_slist_next(ids)) {
                struct sim_res_id *res_id;

                res_id = (struct sim_res_id *)ids->data;
                if (res_id == NULL) continue;
                if (!strcmp(res_id->file, file)) {
                     struct sim_rdr_id * rdr_id;

                     rdr_id = g_malloc(sizeof(*rdr_id));
                     if (rdr_id == NULL) return;
                     rdr_id->reqnum = reqnum;
                     rdr_id->rdr_id = rid;
                     res_id->rdr_list = g_slist_append(res_id->rdr_list, rdr_id);
                }
        }  
}
struct sim_rdr_id *sim_util_get_rdr_id(GSList *ids,
                                       char *file,
                                       int index)
{
         for ( ; ids; ids = g_slist_next(ids)) {
                struct sim_res_id *res_id;
               
                res_id = (struct sim_res_id *)ids->data;
                if (res_id == NULL) continue;
                if (!strcmp(res_id->file, file))
                     return g_slist_nth_data(res_id->rdr_list, index);
        }
        return NULL;
}

int sim_util_get_rdr_by_sensornum(RPTable *table,
                                  SaHpiResourceIdT res_id,
                                  SaHpiSensorNumT num,
                                  SaHpiEntryIdT *eid)
{  
        SaHpiRdrT *rdr;

        rdr = oh_get_rdr_by_type(table, res_id, SAHPI_SENSOR_RDR, num);

        if (rdr == NULL) 
        return -1;
        *eid = rdr->RecordId;
        return 0;
}

int sim_util_get_res_id(RPTable *table, char *filename, SaHpiResourceIdT *rid)
{
        SaHpiRptEntryT *entry;
        SaHpiResourceIdT  id;

        for (id = RPT_ENTRY_BEGIN, entry = oh_get_resource_next(table, id);
             entry; entry = oh_get_resource_next(table, entry->ResourceId)) {
                char* data;
                data = (char*) oh_get_resource_data(table, entry->ResourceId);
                if (data == NULL) continue;
                if (!strcmp(data, filename)) {
                        *rid = entry->ResourceId;
                        return 0;
                }
        }
        return -1;
}

char *sim_util_get_sensor_dir(struct oh_handler_state *inst, 
                              SaHpiResourceIdT id,
                              SaHpiSensorNumT num)
{
        int retval;
        GHashTable *handler_config;
        char *str1, *str2, *str;
        int len1, len2;
        SaHpiEntryIdT entry_id;
 

        handler_config = inst->config;
        str1 = g_hash_table_lookup(handler_config, "root_path");
        if (str1 == NULL)
        return NULL;
        len1 = strlen(str1);

        str2 = (char*) oh_get_resource_data(inst->rptcache, id);
        if (str2 == NULL) return NULL;
        len2 = strlen(str2);

        retval = sim_util_get_rdr_by_sensornum(inst->rptcache, id,
                                               num, &entry_id);
        if (retval)
                return NULL;

        str = (char *)g_malloc0(len1 + len2 + 20);
        if (str == NULL) 
                return NULL;

        sprintf(str, "%s/%s/%x/sensor", str1, str2, entry_id);
        return str;
}

char* sim_util_get_rdr_dir(struct oh_handler_state *inst,
                           SaHpiResourceIdT res_id, 
                           SaHpiEntryIdT rd_id)
{   
        GHashTable *handler_config;
        char *str1, *str2, *str;
        int len1, len2;
        
        handler_config = inst->config;
        str1 = g_hash_table_lookup(handler_config, "root_path");
        if (str1 == NULL)
        return NULL;
        len1 = strlen(str1);

        str2 = (char*) oh_get_resource_data(inst->rptcache, res_id);
        if (str2 == NULL) return NULL;
        len2 = strlen(str2);

        str = (char *)g_malloc0(len1 + len2 + 20);
        if (str == NULL)
                return NULL;

        sprintf(str, "%s/%s/%x", str1, str2, rd_id);
        return str;
}

int sim_util_insert_event(GSList *eventq, struct oh_event *event)
{
        return -1;
}
int sim_util_remove_event(GSList *eventq, struct oh_event *event)
{
        return -1;
}

