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

static void close_done(void *cb_data)
{
		struct oh_handler_state *handler = (struct oh_handler_state *)cb_data;
		struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
		
		dbg("IPMI connection closed");
		
		g_free(ipmi_handler);
		g_free(handler);
}
		

void ohoi_close_connection(ipmi_domain_t *domain, void *user_data)
{
		struct oh_handler_state *handler = (struct oh_handler_state *)user_data;

		int rv;

		rv = ipmi_close_connection(domain, close_done, handler);
		if (rv)
				dbg("ipmi_close_connection failed!");

}
