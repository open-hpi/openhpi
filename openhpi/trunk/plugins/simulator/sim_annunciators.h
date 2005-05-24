#ifndef __SIM_ANNUNCIATORS_H
#define __SIM_ANNUNCIATORS_H

#include <sim_init.h>
#include <sim_resources.h>
#include <SaHpi.h>


/*******************
Annunciator Definitions
*********************/

#define ANNUN_MAX       1
#define ANNOUN_MAX      1

struct sim_annunc {
        SaHpiAnnunciatorNumT    Num;
        SaHpiAnnunciatorModeT   mode;
        int                     count;
        SaHpiAnnouncementT      def_announs[ANNOUN_MAX];
        SaHpiAnnouncementT      *announs;
};


extern struct sim_annunc sim_annunc_rdrs[];

SaErrorT sim_discover_annunc(RPTable *rpt);
SaErrorT new_annunc(RPTable *rptcache, SaHpiResourceIdT ResId, int Index);
int sim_get_next_annunc_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type);


#endif

