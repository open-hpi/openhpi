/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     Kevin Gao <kevin.gao@linux.intel.com>
 *     Rusty Lynch <rusty.lynch@linux.intel.com>
 *     Tariq Shureih <tariq.shureih@intel.com>
 *     Racing Guo  <racing.guo@intel.com>
 *     Andy Cress  <andrew.r.cress@intel.com> (watchdog)
 */

#include "ipmi.h"

void ipmi_connection_handler (ipmi_domain_t	*domain,
			      int		err,
			      unsigned int	conn_num,
			      unsigned int	port_num,
			      int		still_connected,
			      void		*cb_data)
{
  	struct ohoi_handler	*ipmi_handler = cb_data;

	dbg("connection handler called");

	dbg("Domain error code: %d", err);

	if (err) {
	  	dbg("Failed to connect to IPMI domain");
		ipmi_handler->connected = 0;
	} else {
	  	dbg("IPMI domain Connection success");
		ipmi_handler->connected = 1;
	}
	if (!still_connected) {
		dbg("All IPMI connections down\n");
		ipmi_handler->connected = 0;
	}
}
