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

#define OH_ALL_READY (SaHpiUint8T)0xFF
#define OH_NOT_READY (SaHpiUint8T)0x00

#define OH_PLUGINS_READY (SaHpiUint8T)0x01
#define OH_HANDLERS_READY (SaHpiUint8T)0x02
#define OH_CONFIGS_READY (SaHpiUint8T)0x04
#define OH_PROCESS_Q_READY (SaHpiUint8T)0x08
#define OH_DOMAINS_READY (SaHpiUint8T)0x10
#define OH_SESSIONS_READY (SaHpiUint8T)0x20
#define OH_UID_READY (SaHpiUint8T)0x40

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
SaHpiUint8T oh_get_ready_state(void);

#endif /* OH_INIT_H */
