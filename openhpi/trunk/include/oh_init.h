/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#ifndef OH_INIT_H
#define OH_INIT_H

#include <SaHpi.h>

/**********************************************************************
 *
 *  Finalize and Initialize were dropped in HPI B.  We keep
 *  them as a hold over for now, however once new plugin
 *  and handler utilities are provided, we'll rewrite them to
 *  be cleaner.
 *
 *********************************************************************/

SaErrorT oh_initialize(void);
/*SaErrorT oh_finalize(void);*/
SaErrorT oh_initialized(void);

#endif /* OH_INIT_H */
