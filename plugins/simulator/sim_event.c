#include <stdio.h>
#include <stdlib.h>
#include <fam.h>
#include <pthread.h>
#include <dirent.h>

#include <SaHpi.h>
#include <openhpi.h>

#include "sim_util.h"
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
        SaHpiRptEntryT *rpt;
        DIR *pdir;
        struct dirent *pd;
        char  *path, *root_path;
        int len;
        

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
  
        rpt = g_malloc0(sizeof(*rpt));
#if 0
        parse rpt entry(contain entity path, then calulate resource id)
        
        insert resource event into event queue;
#else
        printf("add resource:%s\n", fe->filename);
#endif
        feh->ids = sim_util_add_res_id(feh->ids, fe->filename, rpt->EntryId, 
                                       &rpt->ResourceEntity);
        for (pd = readdir(pdir); pd; pd = readdir(pdir)) {
                DIR *tmp;
                unsigned int index;

                if (str2uint32(pd->d_name, &index)) continue;
#if 0
                insert rdr event into event queue; 
#else
                printf("add rdr:%s\n", pd->d_name);
#endif
                sprintf(path, "%s/%s/%s/sensor", root_path, res, pd->d_name); 
                tmp = opendir(path);
                if (tmp) {
                        FAMMonitorDirectory(fe->fc, path, &fr, (void *)req_rdr);
                        sim_util_add_rdr_id(feh->ids, fe->filename, fr.reqnum, index);
                        printf("monitor sensor:%s\n", pd->d_name);
                        close(tmp);
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
#if 0
        insert resource event and rdr event to event queue
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
        sim_util_free_res_id(feh->ids, fe->filename);
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
#if 0
        free all resource id and rdr id;
#endif
        return 0;
}

struct fe_handler* fhs_event_init(struct oh_handler_state *hnd)
{

        pthread_t tid;
        int retval;     
        struct fe_handler *feh;

        feh = (struct fe_handler *)malloc(sizeof(*feh));
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
        free(feh);
}

#ifdef UNIT_TEST
int main()
{
        struct fe_handler *feh;

        feh = fhs_event_init(NULL);
        while(1);
        fhs_event_finish(feh);
        return 0;
}
/*
 *   How to do module test
 *   gcc  -DUNIT_TEST -g -I. -I../../..  -I../../../include
 *   -I/usr/include/glib-1.2 -I/usr/lib/glib/include 
 *   -lfam -lglib -lpthread  sim_event.c
 */
#endif
