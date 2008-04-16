/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
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
 *
 */

#include "oh_client_connxs.h"
extern "C"
{
#include "oh_client_conf.h"
#include <config.h>
}

GHashTable *domains = NULL;
GHashTable *sessions = NULL;
GStaticRecMutex sessions_sem = G_STATIC_REC_MUTEX_INIT;
static SaHpiSessionIdT next_client_sid = 1;

static void __destroy_client_connx(gpointer data)
{
        struct oh_client_session *client_session = (struct oh_client_session *)data;

        g_hash_table_destroy(client_session->connxs);
        g_free(client_session);
}

static int init(void)
{
        // Initialize GLIB thread engine
	if (!g_thread_supported()) {
        	g_thread_init(NULL);
        }
        
        // Create session table.
	if (!sessions) {
		sessions = g_hash_table_new_full(
			g_int_hash, 
                        g_int_equal,
                        NULL, 
                        __destroy_client_connx
                );
	}

        if (!domains) { // Create domain table
                domains = g_hash_table_new_full(
                        g_int_hash, g_int_equal,
                        NULL, g_free
                );
                /* TODO: Check to see if a default domain exists, if not, add it */
                return oh_load_client_config(OH_CLIENT_DEFAULT_CONF, domains);
        }

        return 0;
}

/*----------------------------------------------------------------------------*/
/* oh_create_connx                                                            */
/*----------------------------------------------------------------------------*/

pcstrmsock oh_create_connx(SaHpiDomainIdT did)
{
	const char      *host, *portstr;
	int	       	port;
        pcstrmsock      pinst = NULL;
        struct oh_domain_conf *domain_conf = NULL;

        /* TODO: Use env vars for overriding default domain in conf file */
        /*
        host = getenv("OPENHPI_DAEMON_HOST");
        if (host == NULL) {
                host = "localhost";
        }
        portstr = getenv("OPENHPI_DAEMON_PORT");
        if (portstr == NULL) {
                port =  4743;
        }
        else {
                port =  atoi(portstr);
        }
        */
        init(); /* Initialize library - Will run only once */

        g_static_rec_mutex_lock(&sessions_sem);
	pinst = new cstrmsock;
        if (!pinst) {
                g_static_rec_mutex_unlock(&sessions_sem);
                return pinst;
        }

        domain_conf = (struct oh_domain_conf *)g_hash_table_lookup(domains, did);
        if (!domain_conf) {
                delete pinst;
                g_static_rec_mutex_unlock(&sessions_sem);
                return NULL;
        }
        g_static_rec_mutex_unlock(&sessions_sem);
        
	if (pinst->Open(domain_conf->host, domain_conf->port)) {
		err("oh_create_connx: Could not open client socket"
		    "\nPossibly, the OpenHPI daemon has not been started.");
                delete pinst;
		return NULL;
	}
	dbg("oh_create_connx: Client instance created");
	return pinst;
}


/*----------------------------------------------------------------------------*/
/* oh_delete_connx                                                            */
/*----------------------------------------------------------------------------*/

void oh_delete_connx(pcstrmsock pinst)
{
	if (pinst) {
                pinst->Close();
                dbg("oh_delete_connx: Connection closed and deleted");
                delete pinst;
        }
}


/*----------------------------------------------------------------------------*/
/* oh_add_connx - with helper functions: __destroy_table, __delete_connx      */
/*----------------------------------------------------------------------------*/
static void __delete_connx(gpointer data)
{
        pcstrmsock pinst = (pcstrmsock)data;

        oh_delete_connx(pinst);
}

SaHpiSessionIdT oh_add_connx(SaHpiSessionIdT sid, pcstrmsock pinst)
{
        GHashTable *connxs = NULL;
        pthread_t thread_id;
        struct oh_client_session *client_session;
        
        if (SessionId == 0 || pinst == NULL)
		return 0;

        client_session = (struct oh_client_session *)g_malloc0(sizeof(struct oh_client_session));
        
        g_static_rec_mutex_lock(&sessions_sem);
        // Create connections table for new session.
        connxs = g_hash_table_new_full(g_int_hash, g_int_equal,
                                       g_free, __delete_connx);
        // Map connection to thread id
        thread_id = pthread_self();
        g_hash_table_insert(connxs, g_memdup(&thread_id, sizeof(pthread_t)),
                            pinst);
        // Map connecitons table to session id
        client_session->dsid = sid;
        client_session->csid = next_client_sid++;
        client_session->connxs = connxs;
        g_hash_table_insert(sessions, &client_session->csid, client_session);
        g_static_rec_mutex_unlock(&sessions_sem);

        return client_session->csid;
}


/*----------------------------------------------------------------------------*/
/* oh_remove_connxs                                                           */
/*----------------------------------------------------------------------------*/

SaHpiBoolT oh_remove_connxs(SaHpiSessionIdT SessionId)
{        
        if (SessionId == 0)
		return SAHPI_TRUE;

        g_static_rec_mutex_lock(&sessions_sem);
        // Since we used g_hash_table_new_full to create the tables,
        // this will remove the connection hash table and all of its entries also.
        g_hash_table_remove(sessions, &SessionId);

        return SAHPI_FALSE;
}

/*----------------------------------------------------------------------------*/
/* oh_remove_connx                                                            */
/*----------------------------------------------------------------------------*/

SaHpiBoolT oh_remove_connx(SaHpiSessionIdT SessionId)
{
        pthread_t thread_id = pthread_self();
        struct oh_client_session *client_session = NULL;

        if (SessionId == 0)
                return SAHPI_TRUE;

        g_static_rec_mutex_lock(&sessions_sem);
        client_session = (struct oh_client_session *)g_hash_table_lookup(sessions, &SessionId);
        if (!client_session) {
                g_static_rec_mutex_unlock(&sessions_sem);
                err("ERROR: Did not find connx for sid %d", SessionId);
                return SAHPI_FALSE;
        }
        
        g_hash_table_remove(client_session->connxs, &thread_id);

        g_static_rec_mutex_unlock(&sessions_sem);

        return SAHPI_FALSE;
}

/*----------------------------------------------------------------------------*/
/* oh_get_connx                                                               */
/*----------------------------------------------------------------------------*/

pcstrmsock oh_get_connx(SaHpiSessionIdT SessionId)
{
        pthread_t thread_id = pthread_self();
        struct oh_client_session *client_session = NULL;
        pcstrmsock pinst = NULL;

	if (SessionId == 0)
		return FALSE;
		
	init(); /* Initialize library - Will run only once */

        // Look up connection table. If it exists, look up connection.
        // if there is not connection, create one on-the-fly.
        g_static_rec_mutex_lock(&sessions_sem);
        client_session = (struct oh_client_session *)g_hash_table_lookup(sessions, &SessionId);
        if (client_session) {
                pinst = (pcstrmsock)g_hash_table_lookup(client_session->connxs, &thread_id);

                if (!pinst) {
                        pinst = oh_create_connx(/*TODO: put domain id here*/);
                        if (pinst) {
                        	g_hash_table_insert(client_session->connxs, 
                                		    g_memdup(&thread_id,
                                		     sizeof(pthread_t)),
                                		    pinst);
				dbg("We are inserting a new connection"
				    " in conns table");
                        }
                }


        }
        g_static_rec_mutex_unlock(&sessions_sem);

        if (pinst) {
            return pinst;
        } else {
            return FALSE;
        }
}
