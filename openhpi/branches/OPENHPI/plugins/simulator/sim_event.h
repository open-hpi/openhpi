#ifndef _INC_SIM_EVENT_
#define _INC_SIM_EVENT_

struct fe_handler{
        volatile int done;
        struct oh_handler_state *ohh;
        GSList * ids;
        pthread_t tid;
};

struct fe_handler *fhs_event_init(struct oh_handler_state *hnd);
void fhs_event_finish(struct fe_handler *feh);

#endif
