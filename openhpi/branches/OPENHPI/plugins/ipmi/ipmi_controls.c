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

static void get_control_state(ipmi_control_t	*control,
				void		*cb_data)
{
	//SaHpiCtrlStateT ctrl_state;
	//int rv;


	dbg("Under construction");


}

int ohoi_get_control_state(ipmi_control_id_t	control_id,
			   SaHpiCtrlStateT	*state)
{
	int rv;

	rv = ipmi_control_pointer_cb(control_id, get_control_state, state);

	if (rv) {
		dbg("Unable to retrieve control state");
		return SA_ERR_HPI_ERROR;
	}
	return 0;
}




