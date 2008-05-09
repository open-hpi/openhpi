/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2008
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 *  Authors:
 *  Suntrupth S Yadav <suntrupth@in.ibm.com>
 */
 

#ifndef __SIM_DIMI_H
#define __SIM_DIMI_H

struct sim_dimi {
        SaHpiDimiRecT dimirec;
        SaHpiDimiInfoT info;
        SaHpiDimiTestT test;
        const char *comment;
};

extern struct sim_dimi sim_dimi_info[];
    
#endif
