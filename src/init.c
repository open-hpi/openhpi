/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2006
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
 *      David Judkovics <djudkovi@us.ibm.com>
 *
 */

#include <oh_init.h>
#include <oh_initialize.h>
#include <oh_config.h>
#include <oh_plugin.h>
#include <oh_domain.h>
#include <oh_session.h>
#include <oh_threaded.h>
#include <oh_error.h>
#include <oh_lock.h>
#include <oh_utils.h>

#include <oh_config.h>


/**
 * _init
 *
 * Returns: 0 on success otherwise an error code
 **/
int _init(void)
{
        SaErrorT rval;
        SaHpiDomainCapabilitiesT capabilities = 0X00000001;
        SaHpiTextBufferT tag;

        data_access_lock();

        /* Initialize thread engine */
        oh_threaded_init();

        /* Initialize uid_utils */
        rval = oh_uid_initialize();
        if( (rval != SA_OK) && (rval != SA_ERR_HPI_ERROR) ) {
                dbg("Unique ID intialization failed.");
                data_access_unlock();
                return rval;
        }
        trace("Initialized UID.");

        /* Initialize handler table */
        oh_handlers.table = g_hash_table_new(g_int_hash, g_int_equal);
        trace("Initialized handler table");

        /* Initialize domain table */
        oh_domains.table = g_hash_table_new(g_int_hash, g_int_equal);
        trace("Initialized domain table");

        /* Create first domain */
        oh_init_textbuffer(&tag);
        oh_append_textbuffer(&tag,"First Domain");
        if (!oh_create_domain(capabilities, SAHPI_TIMEOUT_IMMEDIATE, &tag)) {
                data_access_unlock();
                dbg("Could not create first domain!");
                return SA_ERR_HPI_ERROR;
        }
        trace("Created first domain");

        /* Initialize session table */
        oh_sessions.table = g_hash_table_new(g_int_hash, g_int_equal);
        trace("Initialized session table");

        /* load plugins and start discovery threads if not a daemon */
        if ( _initialize(FALSE) == SA_ERR_HPI_ERROR) return SA_ERR_HPI_ERROR;

        data_access_unlock();

        /*
        * HACK: Wait a second before returning
        * to give the threads time to populate the RPT
        */
        struct timespec waittime = { .tv_sec = 1, .tv_nsec = 1000L};
        nanosleep(&waittime, NULL);

        /* Do not use SA_OK here in case it is ever changed to something
         * besides zero, The runtime stuff depends on zero being returned here
         * in order for the shared library to be completely initialized.
         */
        return 0;
}

/**
 * _fini
 *
 * Returns: always returns 0
 **/
int _fini(void)
{

        data_access_lock();

        oh_close_handlers();

        data_access_unlock();

        return 0;

}

