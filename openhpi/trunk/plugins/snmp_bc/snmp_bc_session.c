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
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>
#include <sim_init.h>

/**
 * snmp_bc_open:
 * @handler_config: Pointer to hash table (passed by infrastructure)
 *
 * Open an SNMP BladeCenter/RSA plugin handler instance.
 *
 * Returns:
 * Plugin handle - normal operation.
 * NULL - on error.
 **/
void *snmp_bc_open(GHashTable *handler_config)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
        char *hostname, *version, *sec_level, *authtype, *user, *pass, *community;
        char *root_tuple;

        root_tuple = (char *)g_hash_table_lookup(handler_config, "entity_root");
        if (!root_tuple) {
                dbg("Cannot find \"entity_root\" configuration parameter.");
                return NULL;
        }

        hostname = (char *)g_hash_table_lookup(handler_config, "host");
        if (!hostname) {
                dbg("Cannot find \"host\" configuration parameter.");
                return NULL;
        }

        handle = (struct oh_handler_state *)g_malloc0(sizeof(struct oh_handler_state));
        custom_handle = (struct snmp_bc_hnd *)g_malloc0(sizeof(struct snmp_bc_hnd));
        if (!handle || !custom_handle) {
                dbg("Out of memory.");
                return NULL;
        }

        handle->data = custom_handle;
        handle->config = handler_config;

        /* Initialize RPT cache */
        handle->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));
        oh_init_rpt(handle->rptcache);
	 
        /* Initialize event log cache */
	/* FIXME:: RSA has 512 here */
        handle->elcache = oh_el_create(OH_EL_MAX_SIZE);
	handle->elcache->gentimestamp = FALSE;

	/* Initialize "String to Event" mapping hash table */
	/* FIXME:: Add RSA initialization here */
	if (errlog2event_hash_use_count == 0) {
		if (errlog2event_hash_init(&errlog2event_hash, bc_eventxml)) {
			dbg("Out of memory.");
			return NULL;
		}
	}
	errlog2event_hash_use_count++;
	
	/* Initialize "Event Number to HPI Event" mapping hash table */
	if (event2hpi_hash_init(handle)) {
		dbg("Out of memory.");
		return NULL;
	}
  
	/* Initialize simulator tables */
	if (is_simulator()) {
		custom_handle->ss = NULL;
		sim_init();
	}
	else {
		/* Initialize SNMP library */
		init_snmp("oh_snmp_bc");
		snmp_sess_init(&(custom_handle->session));
		custom_handle->session.peername = hostname;
		/* Set retries - based on testing with BC/BCT MM SNMP V3 agent */
		custom_handle->session.retries = 3;
		version = (char *)g_hash_table_lookup(handle->config, "version");
		if (!version) {
			dbg("Cannot find \"version\" configuration parameter.");
			return NULL;
		}
		sec_level = (char *)g_hash_table_lookup(handle->config, "security_level");
		authtype = (char *)g_hash_table_lookup(handle->config, "auth_type");
		user = (char *)g_hash_table_lookup(handle->config, "security_name");
		pass = (char *)g_hash_table_lookup(handle->config, "passphrase");
		community = (char *)g_hash_table_lookup(handle->config, "community");
		
		/* Configure SNMP V3 session */
		if (!strcmp(version, "3")) {
			if (!user) {
				dbg("Cannot find \"security_name\" configuration parameter.");
				return NULL;
			}
			custom_handle->session.version = SNMP_VERSION_3;
			custom_handle->session.securityName = user;
			custom_handle->session.securityNameLen = strlen(user);
			custom_handle->session.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
			
			if (!strncmp(sec_level, "auth", 4)) { /* If using password */
				if (!pass) {
					dbg("Cannot find \"passphrase\" configuration parameter.");
					return NULL;
				}
				
				custom_handle->session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
				if (!authtype || !strcmp(authtype,"MD5")) {
					custom_handle->session.securityAuthProto = usmHMACMD5AuthProtocol;
					custom_handle->session.securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
				} else if (!strcmp(authtype,"SHA")) {
					custom_handle->session.securityAuthProto = usmHMACSHA1AuthProtocol;
					custom_handle->session.securityAuthProtoLen = USM_AUTH_PROTO_SHA_LEN;
				} else {
					dbg("Unrecognized authenication type=%s.", authtype); 
					return NULL;
				}
				
				/* FIXME:: What do snmp_perror and snmp_log buy us? Below as well */
				custom_handle->session.securityAuthKeyLen = USM_AUTH_KU_LEN;
				if (generate_Ku(custom_handle->session.securityAuthProto,
						custom_handle->session.securityAuthProtoLen,
						(u_char *) pass, strlen(pass),
						custom_handle->session.securityAuthKey,
						&(custom_handle->session.securityAuthKeyLen)) != SNMPERR_SUCCESS) {
					snmp_perror("snmp_bc");
					snmp_log(LOG_ERR,
						 "Error generating Ku from authentication passphrase.\n");
					dbg("Unable to establish SNMP authnopriv session.");
					return NULL;
				}
				
				if (!strcmp(sec_level, "authPriv")) { /* if using encryption */
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
						dbg("Unable to establish SNMP authpriv session.");
						return NULL;
					}
					
				}
			}                                
                /* Configure SNMP V1 session */
		} else if (!strcmp(version, "1")) { 
			if (!community) {
				dbg("Cannot find \"community\" configuration parameter.");
				return NULL;
			}
			custom_handle->session.version = SNMP_VERSION_1;
			custom_handle->session.community = community;
			custom_handle->session.community_len = strlen(community);
		} else {
			dbg("Unrecognized SNMP version=%s.", version);
			return NULL;
		}
                
		/* Windows32 specific net-snmp initialization (noop on unix) */
		SOCK_STARTUP;

		custom_handle->ss = snmp_open(&(custom_handle->session));

		if (!custom_handle->ss) {
			snmp_perror("ack");
			snmp_log(LOG_ERR, "Something horrible happened!!!\n");
		 	dbg("Unable to open SNMP session.");
			return NULL;
		}
	}

	/* Determine platform type and daylight savings time */
	{
		const char *oid;
		struct snmp_value get_value;
		SaErrorT err;

		err = snmp_get(custom_handle->ss, SNMP_BC_PLATFORM_OID_RSA, &get_value);
		if (err == SA_OK) {
			custom_handle->platform = SNMP_BC_PLATFORM_RSA;
		}
		else {
			err = snmp_get(custom_handle->ss, SNMP_BC_PLATFORM_OID_BCT, &get_value);
			if (err == SA_OK) {
				trace("Found BC-T\n");
				custom_handle->platform = SNMP_BC_PLATFORM_BCT;
			}
			else {
				if (err ==  SA_ERR_HPI_NOT_PRESENT) {
					trace("Found BC-E\n");
					custom_handle->platform = SNMP_BC_PLATFORM_BC;
				}
				else {
					dbg("Cannot read model type=%s; Error=%d.",
					      SNMP_BC_PLATFORM_OID_BCT, err);
					return NULL;
				}
			}
		}

		/* DST */
		if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) { oid = SNMP_BC_DST_RSA; }
		else { oid = SNMP_BC_DST; }

		err = snmp_get(custom_handle->ss, oid, &get_value);
		if (err == SA_OK) {
			strcpy(custom_handle->handler_timezone, get_value.string);
		}
		else {
			dbg("Cannot read DST=%s; Error=%d.", oid, get_value.type);
			return NULL;
		}
	}

	if (is_simulator()) {
		sim_banner(custom_handle);
	}
	
        return handle;
}

/**
 * snmp_bc_close: 
 * @hnd: Pointer to handler structure.
 * 
 * Close an SNMP BladeCenter/RSA plugin handler instance.
 *
 * Returns:
 * Void
 **/
void snmp_bc_close(void *hnd)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        oh_el_close(handle->elcache);

	if (is_simulator()) {
		sim_close();
	}
	else {
		struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

		/* FIXME:: Should we free handle->config - same question on A.1.1 code? */
		snmp_close(custom_handle->ss);
		/* Windows32 specific net-snmp cleanup (noop on unix) */
		SOCK_CLEANUP;
	}

	/* Cleanup event2hpi hash table */
	event2hpi_hash_free(handle);

	/* Cleanup str2event hash table */
	/* FIXME:: Add RSA free here */
	errlog2event_hash_use_count--;
	if (errlog2event_hash_use_count == 0) {
		errlog2event_hash_free(&errlog2event_hash);
	}
}
