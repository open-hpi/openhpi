#include <stdio.h>
#include <stdlib.h>
#include <fam.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <uid_utils.h>

#include "sim_util.h"
#include "sim_parser.h"
#include "sim_event.h"


static const unsigned int req_res = 0;
static const unsigned int req_rdr = 1;

static int str2uint32(char *str, SaHpiUint32T *val)
{
#define BYTES_NUM 8
        int i, len;
        SaHpiUint32T  value = 0;

        len = strlen(str);
        if (len > BYTES_NUM || len <= 0)  return -1;
        for (i = 0; i< len; i++) {
                if ((str[i] >= '0') && (str[i] <= '9')) {
                        value = (str[i] - '0') + value*16;
                }else if ((str[i] >= 'a') && (str[i] <= 'f')) {
                        value = (str[i] - 'a'+ 10) + value*16;
                }else {
                        return -1;
                }
        }
        *val = value;
        return 0;
}

static int fhs_event_add_resource(struct fe_handler *feh, char *res, FAMEvent *fe)
{
        FAMRequest fr;
        DIR *pdir;
        struct dirent *pd;
        char  *path, *root_path;
        int len;
        struct oh_event *event;

#ifndef UNIT_TEST
        root_path = g_hash_table_lookup(feh->ohh->config, "root_path");
#else
        root_path = "/home/guorj/HPI/openhpi/src/plugins/simulator/test/resources";
#endif
        len = strlen(root_path) + strlen(fe->filename) + 30;
        path = g_malloc(len);

        sprintf(path, "%s/%s", root_path, fe->filename);
        pdir = opendir(path);
        if (!pdir) {
            printf("%s is not directory\n", path);
            g_free(path);
            return -1;
        }

#if 1
        event = g_malloc0(sizeof(*event));
        event->type = OH_ET_RESOURCE;
        sprintf(path, "%s/%s/rpt", root_path, fe->filename);
        sim_parser_get_rpt(path, &event->u.res_event.entry);
        event->u.res_event.entry.ResourceId = oh_uid_from_entity_path(
                                     &event->u.res_event.entry.ResourceEntity);
        sim_util_insert_event(feh->ohh->eventq, event);
#endif

        feh->ids = sim_util_add_res_id(feh->ids, fe->filename,
                                       event->u.res_event.entry.ResourceId, 
                                       &event->u.res_event.entry.ResourceEntity);
        for (pd = readdir(pdir); pd; pd = readdir(pdir)) {
                DIR *tmp;
                unsigned int index;

                if (str2uint32(pd->d_name, &index)) continue;
#if 1
                event = g_malloc0(sizeof(*event));
                event->type = OH_ET_RDR;
                sprintf(path, "%s/%s/%s/rdr", root_path, fe->filename, pd->d_name);
                sim_parser_get_rdr(path, &event->u.rdr_event.rdr);
                event->u.res_event.entry.ResourceId = oh_uid_from_entity_path(
                                     &event->u.res_event.entry.ResourceEntity);
                sim_util_insert_event(feh->ohh->eventq, event);
                printf("add rdr:%s\n", pd->d_name);
#endif
                sprintf(path, "%s/%s/%s/sensor", root_path, res, pd->d_name); 
                tmp = opendir(path);
                if (tmp) {
                        FAMMonitorDirectory(fe->fc, path, &fr, (void *)req_rdr);
                        sim_util_add_rdr_id(feh->ids, fe->filename, fr.reqnum, index);
                        printf("monitor sensor:%s\n", pd->d_name);
                        closedir(tmp);
                }
        }
        closedir(pdir);
        g_free(path);
        return 0;
}
static void fhs_event_remove_resource(struct fe_handler *feh, FAMEvent *fe)
{
        struct sim_rdr_id   *rdr_id;
        int index;
        struct oh_event *event;
        struct sim_res_id *id;
#if 1
        event = g_malloc0(sizeof(*event));
        event->type = OH_ET_RESOURCE_DEL;
        id = sim_util_get_res_id_by_reqnum(feh->ids, fe->fr.reqnum);
        event->u.res_del_event.resource_id = id->res_id;
        sim_util_insert_event(feh->ohh->eventq, event);
#endif
        index = 0;
        rdr_id = sim_util_get_rdr_id(feh->ids, fe->filename, index);

        while (rdr_id) {
             FAMRequest fr;
             fr.reqnum = rdr_id->reqnum;
             FAMCancelMonitor(fe->fc, &fr);  
             index++;
             rdr_id = sim_util_get_rdr_id(feh->ids, fe->filename, index);
        }
        feh->ids = sim_util_free_res_id(feh->ids, fe->filename);
        printf("remove resource and sensor:%s, code:%d\n", fe->filename, fe->code);
        return;
}

static void fhs_event_sensor_update(struct fe_handler *feh, FAMEvent *fe)
{
        struct sim_res_id  *res_id;
#if 0
        insert event into event queue if threshold event happens
#else
        res_id = sim_util_get_res_id_by_reqnum(feh->ids, fe->fr.reqnum);
        if (res_id == NULL) 
                return;
        printf("update sensor:%s, code:%d\n", fe->filename, fe->code);
#endif
        return;
}


#define IS_DIR(name)  ((*(name)=='/')?1:0)
static void* fhs_event_process(void *data)
{
        struct fe_handler *feh = (struct fe_handler*)data;
        FAMEvent fe;
        FAMRequest fr;
        FAMConnection fc;
        char*  root_path;

#ifndef UNIT_TEST 
        root_path = g_hash_table_lookup(feh->ohh->config, "root_path");
#else
        root_path = "/home/guorj/HPI/openhpi/src/plugins/simulator/test/resources";
#endif

        FAMOpen(&fc);
        FAMMonitorDirectory(&fc, root_path, &fr, (void*)req_res);

        while(!feh->done) {   
                FAMNextEvent(&fc, &fe);
                if ((fe.userdata == (void *)req_res) &&
                    ((fe.code == FAMCreated) || (fe.code == FAMExists))) {
                        if (!IS_DIR(fe.filename))
                                fhs_event_add_resource(feh, fe.filename, &fe);
                }else if ((fe.userdata == (void*) req_res) && (fe.code == FAMDeleted)) {
                     	if (!IS_DIR(fe.filename))
                             	fhs_event_remove_resource(feh, &fe);
                      	else
                  		printf("faint! why delete root path\n");
                }else if (fe.userdata == (void *)req_rdr) {
                        if (((fe.code == FAMChanged) || (fe.code == FAMExists))&&
                            ((!strcmp(fe.filename, "reading"))||
                             (!strcmp(fe.filename, "thres")))) {
                                fhs_event_sensor_update(feh, &fe);
                        }
                }
        } 
        FAMClose(&fc);
#if 1
        feh->ids = sim_util_free_all_res_id(feh->ids);   
#endif
        return 0;
}

struct fe_handler* fhs_event_init(struct oh_handler_state *hnd)
{
        int retval;     
        struct fe_handler *feh;

        feh = (struct fe_handler *)g_malloc0(sizeof(*feh));
        if (feh == NULL)
                return NULL;
        feh->ohh = hnd;
        retval = pthread_create(&feh->tid, NULL, fhs_event_process, feh);
        if (retval) {
                free(feh);
                return NULL;
        }
        return feh;
}

void fhs_event_finish(struct fe_handler *feh)
{     
        feh->done = 1;
        pthread_join(feh->tid, NULL);
        g_free(feh);
}
