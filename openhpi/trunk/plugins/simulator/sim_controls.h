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
 *      Sean Dague
 *      Christina Hernandez
 *
 */


#ifndef __SIM_CONTROLS_H
#define __SIM_CONTROLS_H

#include <sim_init.h>
#include <sim_resources.h>
#include <SaHpi.h>


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

extern struct sim_control sim_control_rdrs[];

extern struct sim_control sim_chassis_controls_bc[];
extern struct sim_control sim_chassis_controls_bct[];
extern struct sim_control sim_blade_controls[];
extern struct sim_control sim_blade_addin_controls[];
extern struct sim_control sim_mgmnt_controls[];
extern struct sim_control sim_mediatray_controls[];
extern struct sim_control sim_fan_controls[];
extern struct sim_control sim_power_controls[];
extern struct sim_control sim_switch_controls[];

extern struct sim_control sim_chassis_controls_rsa[];
extern struct sim_control sim_cpu_controls_rsa[];
extern struct sim_control sim_dasd_controls_rsa[];
extern struct sim_control sim_fan_controls_rsa[];

SaErrorT sim_discover_controls(RPTable *rpt);
SaErrorT new_controls(RPTable *rptcache, SaHpiResourceIdT ResId, int Index);
int sim_get_next_control_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type);


#endif

