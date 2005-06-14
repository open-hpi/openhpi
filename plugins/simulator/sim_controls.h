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


#ifndef __SIM_CONTROLS_H
#define __SIM_CONTROLS_H


/*******************
Control Definitions
*********************/

struct sim_control {
        int index;
	SaHpiCtrlRecT control;
	SaHpiCtrlModeT cur_mode;
	const char *comment;
};

extern struct sim_control sim_chassis_controls[];
extern struct sim_control sim_cpu_controls[];
extern struct sim_control sim_dasd_controls[];
extern struct sim_control sim_fan_controls[];

SaErrorT sim_discover_controls(struct oh_handler_state * state);

#endif

