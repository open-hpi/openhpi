/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
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
 *      Steve Sherman  <stevees@us.ibm.com>
 */

#include <SaHpi.h>

#include <snmp_util.h>
#include <snmp_bc_session.h>
#include <snmp_bc.h>
#include <rpt_utils.h>
#include <sel_utils.h>
#include <bc_resources.h>
#include <bc_str2event.h>
#include <snmp_bc_event.h>


int is_simulator(void);
int sim_banner(struct snmp_bc_hnd *);
int sim_init(void);
int sim_close(void);

extern unsigned int str2event_use_count;  
/**
 * snmp_bc_open: open snmp blade center plugin
 * @handler_config: hash table passed by infrastructure
 **/

void *snmp_bc_open(GHashTable *handler_config)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
        char *hostname, *version, *sec_level, *authtype, *user, *pass, *community;
        char *root_tuple;
	SaErrorT rc;

        root_tuple = (char *)g_hash_table_lookup(handler_config, "entity_root");
        if(!root_tuple) {
                dbg("ERROR: Cannot open snmp_bc plugin. No entity root found in configuration.");
                return NULL;
        }

        hostname = (char *)g_hash_table_lookup(handler_config, "host");
        if (!hostname) {
                dbg("ERROR: Cannot open snmp_bc plugin. No hostname found in configuration.");
                return NULL;
        }

        handle = (struct oh_handler_state *)g_malloc0(sizeof(struct oh_handler_state));
        custom_handle =
                (struct snmp_bc_hnd *)g_malloc0(sizeof(struct snmp_bc_hnd));
        if(!handle || !custom_handle) {
                dbg("Could not allocate memory for handle or custom_handle.");
                return NULL;
        }

        handle->data = custom_handle;
        handle->config = handler_config;

        /* Initialize RPT cache */
        handle->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));
 
        /* Initialize SEL cache */
        handle->selcache = oh_sel_create(OH_SEL_MAX_SIZE);
	handle->selcache->gentimestamp = FALSE;

	/* Initialize String-to-Event hash table */
	if (str2event_use_count == 0) {  
		if (str2event_hash_init()) {
			dbg("Couldn't initialize str2event hash table.");
			return NULL;
		}
		str2event_use_count++;
	} else 
		str2event_use_count++;
	
	/* Initialize BC_Event_Number-to-HPI_Event hash table */
	if (event2hpi_hash_init(handle)) {
		dbg("Couldn't initialize event2hpi hash table.");
		return NULL;
	}
       
	/* Initialize simulator tables */
	if (is_simulator()) {
		custom_handle->ss = NULL;
		sim_init();
	}
	else {
		/* Initialize snmp library */
		init_snmp("oh_snmp_bc");        
		snmp_sess_init(&(custom_handle->session)); /* Setting up all defaults for now. */
		/* Set snmp agent's hostname */
		custom_handle->session.peername = hostname;
		custom_handle->session.retries  = 3;	/* set retries to 3.   */ 
							/* From testing with mm snmpv3 agent */

		/* Get config parameters */
		version = (char *)g_hash_table_lookup(handle->config, "version");
		if (!version) {
			dbg("Cannot open snmp_bc. Need version configuration parameter.");
			return NULL;
		}
		sec_level = (char *)g_hash_table_lookup(handle->config, "security_level");
		authtype = (char *)g_hash_table_lookup(handle->config, "auth_type");
		user = (char *)g_hash_table_lookup(handle->config, "security_name");
		pass = (char *)g_hash_table_lookup(handle->config, "passphrase");
		community = (char *)g_hash_table_lookup(handle->config, "community");
		
		/* Configure snmp session */
		if (!strcmp(version,"3")) { /* if snmp version 3*/
			if (!user) {
				dbg("Cannot open snmp_bc. Need security_name configuration parameter.");
				return NULL;
			}
			custom_handle->session.version = SNMP_VERSION_3;
			custom_handle->session.securityName = user;
			custom_handle->session.securityNameLen = strlen(user);
			custom_handle->session.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
			
			if (!strncmp(sec_level,"auth",4)) { /* if using password */
				if (!pass) {
					dbg("Cannot open snmp_bc. Need passphrase configuration parameter.");
					return NULL;
				}
				
				custom_handle->session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
				if (!authtype || !strcmp(authtype,"MD5")) {
					custom_handle->session.securityAuthProto = usmHMACMD5AuthProtocol;
					custom_handle->session.securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
				} else if (!strcmp(authtype,"SHA")) {
					custom_handle->session.securityAuthProto = usmHMACSHA1AuthProtocol;
					custom_handle->session.securityAuthProtoLen = USM_AUTH_PROTO_SHA_LEN;
				} else {dbg("Cannot open snmp_bc. Wrong auth_type."); return NULL;}
				
				custom_handle->session.securityAuthKeyLen = USM_AUTH_KU_LEN;
				if (generate_Ku(custom_handle->session.securityAuthProto,
						custom_handle->session.securityAuthProtoLen,
						(u_char *) pass, strlen(pass),
						custom_handle->session.securityAuthKey,
						&(custom_handle->session.securityAuthKeyLen)) != SNMPERR_SUCCESS) {
					snmp_perror("snmp_bc");
					snmp_log(LOG_ERR,
						 "Error generating Ku from authentication passphrase.\n");
					dbg("Unable to establish snmp authnopriv session.");
					return NULL;
				}
				
				if (!strcmp(sec_level,"authPriv")) { /* if using encryption */
					custom_handle->session.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
					custom_handle->session.securityPrivProto = usmDESPrivProtocol;
					custom_handle->session.securityPrivProtoLen = USM_PRIV_PROTO_DES_LEN;
					custom_handle->session.securityPrivKeyLen = USM_PRIV_KU_LEN;
					if (generate_Ku(custom_handle->session.securityAuthProto,
							custom_handle->session.securityAuthProtoLen,
							(u_char *) pass, strlen(pass),
							custom_handle->session.securityPrivKey,
							&(custom_handle->session.securityPrivKeyLen)) != SNMPERR_SUCCESS) {
						snmp_perror("snmp_bc");
						snmp_log(LOG_ERR,
							 "Error generating Ku from private passphrase.\n");
						dbg("Unable to establish snmp authpriv session.");
						return NULL;
					}
					
				}
			}                                
		} else if (!strcmp(version,"1")) { /* if using snmp version 1 */
			if (!community) {
				dbg("Cannot open snmp_bc. Need community configuration parameter.");
				return NULL;
			}
			custom_handle->session.version = SNMP_VERSION_1;
			custom_handle->session.community = community;
			custom_handle->session.community_len = strlen(community);
		} else {
			dbg("Cannot open snmp_bc. Unrecognized version parameter %s",version);
			return NULL;
		}
                
		/* windows32 specific net-snmp initialization (is a noop on unix) */
		SOCK_STARTUP;

		custom_handle->ss = snmp_open(&(custom_handle->session));

		if(!custom_handle->ss) {
			snmp_perror("ack");
			snmp_log(LOG_ERR, "something horrible happened!!!\n");
			dbg("Unable to open snmp session.");
			return NULL;
		}
	}

	/* Set BladeCenter Type */
	{
		struct snmp_value get_value;

		if (snmp_get(custom_handle->ss,SNMP_BC_TIME_DST,&get_value) == SA_OK) 
			strcpy(custom_handle->handler_timezone, get_value.string);
		else { 
		
			dbg("SNMP could not read BC DST %s; Type=%d.\n", 
			    SNMP_BC_TIME_DST, get_value.type);
			return NULL;
		}
		
		rc = snmp_get(custom_handle->ss, BC_TELCO_SYSTEM_HEALTH_STAT_OID, &get_value); 
		if (rc == SA_OK) {
			strcpy(custom_handle->bc_type, SNMP_BC_PLATFORM_BCT);
		} else if (( rc == SA_SNMP_NOSUCHOBJECT) || (rc == SA_SNMP_NOSUCHINSTANCE)) { 
			strcpy(custom_handle->bc_type, SNMP_BC_PLATFORM_BC);
		} else {		 

			dbg("SNMP could not read %s; Type=%d.\n", 
			    SNMP_BC_BLADECENTER_TYPE, get_value.type);
			return NULL;
		}

	}

	if (is_simulator) 
		sim_banner(custom_handle);
	

        return handle;
}

/**
 * snmp_bc_close: shut down plugin connection
 * @hnd: a pointer to the snmp_bc_hnd struct that contains
 * a pointer to the snmp session and another to the configuration
 * data.
 **/

void snmp_bc_close(void *hnd)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        oh_sel_close(handle->selcache);

	if (is_simulator()) {
		sim_close();
	}
	else {
		struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

		/* Should we free handle->config? */
		/* windows32 specific net-snmp cleanup (is a noop on unix) */
		snmp_close(custom_handle->ss);
		SOCK_CLEANUP;
	}

	/* Cleanup event2hpi hash table */
	event2hpi_hash_free(handle);

	/* Cleanup str2event hash table */
	str2event_use_count--;
	if (str2event_use_count == 0) 
		str2event_hash_free();

}
