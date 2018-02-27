/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
 * (C) Copyright Pigeon Point Systems. 2010
 * (C) Copyright Nokia Siemens Networks 2010
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
 *      Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 */

#include <glib.h>

#include <SaHpi.h>

#include "conf.h"
#include "init.h"
#include "session.h"
#include "sahpi_wrappers.h"


SaErrorT ohc_init(void)
{
    static SaHpiBoolT initialized = SAHPI_FALSE;

    if (initialized != SAHPI_FALSE) {
        return SA_OK;
    }
    initialized = SAHPI_TRUE;

    // Initialize GLIB thread engine
    if (g_thread_supported() == FALSE) {
        wrap_g_thread_init(0);
    }

    ohc_conf_init();
    ohc_sess_init();

    return SA_OK;
}

SaErrorT ohc_finit(void)
{
    SaErrorT rv;

    rv = ohc_sess_close_all();
    if ( rv != SA_OK ) {
        return rv;
    }

    return SA_OK;
}

