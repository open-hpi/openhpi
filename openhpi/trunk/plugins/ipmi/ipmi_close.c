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
 *     Tariq Shureih <tariq.shureih@intel.com>
 */

#include "ipmi.h"

static int flag;

static void close_done(void *cb_data)
{
		struct oh_handler_state *handler = (struct oh_handler_state *)cb_data;
		struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;

		dbg("ipmi instance: #%d, closed", ipmi_handler->ipmi_instance);

		g_free(ipmi_handler);
		g_free(handler);
		flag = 1;
}
		

void ohoi_close_connection(ipmi_domain_t *domain, void *user_data)
{
		struct oh_handler_state *handler = (struct oh_handler_state *)user_data;
		struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;

		int rv;

		flag = 0;
		rv = ipmi_close_connection(domain, close_done, handler);

		if (rv)
				dbg("ipmi_close_connection failed!");

		while (flag == 0) {
				sel_select(ipmi_handler->ohoi_sel, NULL,0,NULL,NULL);
		}
}


