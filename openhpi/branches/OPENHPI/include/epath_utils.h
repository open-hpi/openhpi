#ifndef EPATH_UTILS_H
#define EPATH_UTILS_H

/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author:
 *     Steve Sherman <stevees@us.ibm.com>
 */

/******************************************************************************
 * DESCRIPTION:
 * Header file for epath_utils.c
 *****************************************************************************/

int string2entitypath(const gchar *epathstr,
		      SaHpiEntityPathT *epathptr);

int entitypath2string(const SaHpiEntityPathT *epathptr,
		      gchar *epathstr,
		      gint strsize);

#endif /* EPATH_UTILS_H */
