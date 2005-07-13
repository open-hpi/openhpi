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
 *      W. David Ashley <dashley@us.ibm.com>
 */

#ifndef __SIM_WATCHDOG_H
#define __SIM_WATCHDOG_H


struct simWatchdogInfo {
      SaHpiWatchdogT watchdog;
};

struct sim_watchdog {
      SaHpiWatchdogRecT watchdogrec;
      SaHpiWatchdogT wd;
      const char *comment;
};


SaErrorT sim_get_watchdog_info(void *hnd,
			       SaHpiResourceIdT id,
			       SaHpiWatchdogNumT num,
			       SaHpiWatchdogT *wdt);

SaErrorT sim_set_watchdog_info(void *hnd,
			       SaHpiResourceIdT id,
			       SaHpiWatchdogNumT num,
			       SaHpiWatchdogT *wdt);

SaErrorT sim_reset_watchdog(void *hnd,
			    SaHpiResourceIdT id,
			    SaHpiWatchdogNumT num);


extern struct sim_watchdog sim_chassis_watchdogs[];
extern struct sim_watchdog sim_cpu_watchdogs[];
extern struct sim_watchdog sim_dasd_watchdogs[];
extern struct sim_watchdog sim_fan_watchdogs[];

SaErrorT sim_discover_watchdogs(struct oh_handler_state * state);

#endif
