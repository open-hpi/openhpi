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

struct ControlMibInfo {
	unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
	int write_only; /* Write-only command; 0 no; 1 yes  */
};

struct ControlInfo {
	struct ControlMibInfo mib;
	SaHpiCtrlModeT cur_mode;
};

struct sim_control {
	SaHpiCtrlRecT control;
	struct ControlInfo control_info;
	const char *comment;
};

extern struct sim_control sim_controls[];

SaErrorT sim_discover_controls(RPTable *rpt);
int sim_get_next_control_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type);


#endif

