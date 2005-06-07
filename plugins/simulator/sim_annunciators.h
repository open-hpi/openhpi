/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *	  Christina Hernandez <hernanc@us.ibm.com>
 *        W. David Ashley <dashley@us.ibm.com>
 */


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

