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
 * Authors:
 *      David Judkovics <djudkovi@us.ibm.com>
 */
 
/* used for defining and externing the arrays that hold oids */
#define _SNMP_CLIENT_C_ 
 
#include <SaHpi.h>
#include <openhpi.h>

#include <oh_utils.h>
#include <snmp_util.h>
#include <snmp_client.h>
#include <snmp_client_res.h>
#include <snmp_client_utils.h>
#include <sc_sensor_data.h>

#include <string.h>

/**
 * snmp_client_open: open snmp blade center plugin
 * @handler_config: hash table passed by infrastructure
 **/

static void *snmp_client_open(GHashTable *handler_config)
{
        struct oh_handler_state *handle;

        struct snmp_client_hnd *custom_handle;
        
	char *root_tuple;

	/* snmpv3 sercurity */
	char *version = NULL;
	char *security_name = NULL;
	char *security_level = NULL;
	char *authpassphrase = NULL;
	char *privpassphrase = NULL;

        root_tuple = (char *)g_hash_table_lookup(handler_config, "entity_root");
        if(!root_tuple) {
                dbg("ERROR: Cannot open snmp_client plugin. No entity root found in configuration.");
                return NULL;
        }
        
        handle = 
		(struct oh_handler_state *)g_malloc0(sizeof(*handle));
        custom_handle =
                (struct snmp_client_hnd *)g_malloc0(sizeof(*custom_handle));

        if(!handle || !custom_handle) {
                dbg("Could not allocate memory for handle or custom_handle.");
                return NULL;
        }
        handle->data = custom_handle;
        
        handle->config = handler_config;

        /* Initialize RPT cache */
        handle->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));

        /* Initialize snmp library */
        init_snmp("oh_snmp_client");

        snmp_sess_init(&(custom_handle->session)); /* Setting up all defaults for now. */
        custom_handle->session.peername = (char *)g_hash_table_lookup(handle->config, "host");

	/* snmp version */
        version = (char *)g_hash_table_lookup(handle->config, "version");

	/* Use SNMPv3 to talk to the server */
	if (!strncmp(version, "3", 1)) {

		if(!(security_name = (char *)g_hash_table_lookup(handle->config, "security_name"))){
			dbg("MISSING security_name"); 
			return NULL;
		}
		
		if (!(security_level = (char *)g_hash_table_lookup(handle->config, "security_level"))){
			dbg("MISSING security_level");
			return NULL;
		}
		if (!(authpassphrase = (char *)g_hash_table_lookup(handle->config, "authpassphrase"))){ 
			dbg("MISSING authpassphrase");
			return NULL;
		}
	
		if (!(privpassphrase = (char *)g_hash_table_lookup(handle->config, "privpassphrase"))){
			dbg("MISSING privpassphrase");
			return NULL;
		}
	
		/* set the SNMPv3 user name */
		custom_handle->session.securityName = strdup(security_name);
		custom_handle->session.securityNameLen = strlen(custom_handle->session.securityName);
	
		/* set the SNMP version number */
		custom_handle->session.version = SNMP_VERSION_3;
		
		/* set the security level to authenticated, AND not encrypted */
		if(!strncmp(security_level, "noAuthnoPriv", sizeof("noAuthnoPriv")))
			custom_handle->session.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
		else if(!strncmp(security_level, "authNoPriv", sizeof("authNoPriv"))) 
			custom_handle->session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
		else if(!strncmp(security_level, "authPriv", sizeof("suthPriv")))
		   custom_handle->session.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
		else {
			dbg("ERROR: unsupport securtiy_level");
			return NULL;
		}
	
		/* set the authentication method to MD5 */
		if (authpassphrase) {
			custom_handle->session.securityAuthProto = usmHMACMD5AuthProtocol;
			custom_handle->session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
			custom_handle->session.securityAuthKeyLen = USM_AUTH_KU_LEN;
		    
			/* set the authentication key to a MD5 hashed version of our
			passphrase "The UCD Demo Password" (which must be at least 8
			characters long) */
			if (generate_Ku(custom_handle->session.securityAuthProto,
					custom_handle->session.securityAuthProtoLen,
					(u_char *) authpassphrase, 
					strlen(authpassphrase),
					custom_handle->session.securityAuthKey,
					&custom_handle->session.securityAuthKeyLen) != SNMPERR_SUCCESS) {
				snmp_perror("ack");
				snmp_log(LOG_ERR,
					 "Error generating Ku from AUTHENTICATION pass phrase. ");
				dbg("Error generating Ku from AUTHENTICATION pass phrase.");
			}
		}
		/* set the encryption method to DES */
		if (privpassphrase) {
			custom_handle->session.securityPrivProto = usmDESPrivProtocol;
			custom_handle->session.securityPrivProtoLen = sizeof(usmDESPrivProtocol)/sizeof(oid);
			custom_handle->session.securityPrivKeyLen = USM_PRIV_KU_LEN;
			/* generate Key */
			if (generate_Ku(custom_handle->session.securityAuthProto,
					custom_handle->session.securityAuthProtoLen,
					(u_char *) privpassphrase, 
					strlen(privpassphrase),
					custom_handle->session.securityPrivKey,
					&custom_handle->session.securityPrivKeyLen) != SNMPERR_SUCCESS) {
				snmp_perror("ack");
				snmp_log(LOG_ERR,
					 "Error generating Ku from PRIVACY pass phrase. ");
				dbg("Error generating Ku from PRIVACY pass phrase.");
			}
		}
	
	}
	
	/* Use SNMPv2c to talk to the server */
	if (!strncmp(version, "2c", 2)) {

		/* set the SNMP version number */
		custom_handle->session.version = SNMP_VERSION_2c;

		/* set the SNMPv1 community name used for authentication */
		custom_handle->session.community = 
			(char *)g_hash_table_lookup(handle->config, "community");
		custom_handle->session.community_len = 
			strlen(custom_handle->session.community);
	}
	

        /* windows32 specific net-snmp initialization (is a noop on unix) */
        SOCK_STARTUP;

        custom_handle->ss = snmp_open(&(custom_handle->session));

        if(!custom_handle->ss) {
                snmp_perror("ack");
                snmp_log(LOG_ERR, "something horrible happened!!!");
                dbg("Unable to open snmp session.");
                return NULL;
        }

        return handle;

}

/**
 * snmp_client_close: shut down plugin connection
 * @hnd: a pointer to the snmp_client_hnd struct that contains
 * a pointer to the snmp session and another to the configuration
 * data.
 **/

static void snmp_client_close(void *hnd)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_client_hnd *custom_handle =
                (struct snmp_client_hnd *)handle->data;

        snmp_close(custom_handle->ss);
        /* Should we free handle->config? */

	snmp_shutdown("oh_snmp_client");

        /* windows32 specific net-snmp cleanup (is a noop on unix) */
        SOCK_CLEANUP;
}

/**
 * snmp_client_get_event:
 * @hnd: 
 * @event: 
 * @timeout: 
 *
 * Return value: 
 **/
static int snmp_client_get_event(void *hnd, struct oh_event *event, struct timeval *timeout)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        
        if( g_slist_length(handle->eventq) > 0 ) {
                memcpy(event, handle->eventq->data, sizeof(*event));
                free(handle->eventq->data);
                handle->eventq = g_slist_remove_link(handle->eventq, handle->eventq);
                return 1;
        } else {
                return 0;
	}

	return(-1);
}


static int snmp_client_discover_resources(void *hnd)
{
	SaErrorT status = SA_OK;

	struct snmp_value get_value;

        struct oh_handler_state 
		*handle = (struct oh_handler_state *)hnd;
        struct snmp_client_hnd 
		*custom_handle = (struct snmp_client_hnd *)handle->data;
					
	RPTable *diff_rptable = g_malloc0(sizeof(*diff_rptable));

	/* Get the saHpiEntryCount */
	status = snmp_get2(custom_handle->ss,
			   sa_hpi_entry_count,
			   SA_HPI_ENTRY_COUNT_OID_LEN, 
			   &get_value);    

	/* Get the saHpiEntries, these are the resources */
	status = get_sahpi_table_entries(diff_rptable, handle, get_value.integer);

	/* Get the saHpiRdrCount */
	status = snmp_get2(custom_handle->ss,
			   sa_hpi_rdr_count,
			   SA_HPI_RDR_ENTRY_COUNT_OID_LEN, 
			   &get_value); 
	/* Get the saHpiEntries, these are the resources */
	status = get_sahpi_rdr_table(diff_rptable, handle, get_value.integer);

/* simple check */
//	handle->rptcache = ;

	process_diff_table(handle, diff_rptable);

	/* Build Events for Resources and their RDRs found in the rptcahce */
	if (status == SA_OK)
		status = eventq_event_add( handle );

        return(status);
}

/**
 * snmp_client_get_self_id:
 * @hnd: 
 * @id: 
 * 
 * Return value: 
 **/
static int snmp_client_get_self_id(void *hnd, SaHpiResourceIdT id)
{
        return -1;
}

static int snmp_client_get_sensor_data(void *hnd, SaHpiResourceIdT id,
                                       SaHpiSensorNumT num,
                                       SaHpiSensorReadingT *data)
{
	SaErrorT status = SA_OK;
	struct snmp_value get_value;

	SaHpiRdrT *rdr = NULL;

	struct rdr_data *remote_rdr_data = NULL;
        
	oid anOID[MAX_OID_LEN];
	oid indices[NUM_SEN_INDICES];

        struct oh_handler_state *handle = 
		(struct oh_handler_state *)hnd;

        struct snmp_client_hnd *custom_handle = 
		(struct snmp_client_hnd *)handle->data;

	/* clear data for fresh information */
	memset(data, 0, sizeof(*data));

        if(!(rdr = 
		oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num)) ) {
		dbg("ERROR finding rdr in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

        if (!(remote_rdr_data =
                (struct rdr_data *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId))) {
		dbg(" ERROR finding rdr_data in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

	/* INDEX   { saHpiDomainID, saHpiResourceID, saHpiSensorIndex }	*/
	indices[0] = (oid)remote_rdr_data->index.remote_domain;
	indices[1] = (oid)remote_rdr_data->index.remote_resource_id;
	indices[2] = (oid)num;

	/* VALUES_PRESENT */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_current_values_present, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_SEN_INDICES);
	
	status  = snmp_get2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &get_value);
	if( (status == SA_OK) && (get_value.type == ASN_INTEGER) )
		data->ValuesPresent = 
			(SaHpiSensorReadingFormatsT)SNMP_ENUM_ADJUST(get_value.integer);
	else
		dbg("snmp_client_get_sensor_data: error getting VALUES_PRESENT "); 

	/* RAW_READING */
	if ((rdr->RdrTypeUnion.SensorRec.ThresholdDefn.TholdCapabilities & SAHPI_SRF_RAW)
	    && (data->ValuesPresent & SAHPI_SRF_RAW) && (status == SA_OK) ) {
		
		build_res_oid(anOID, 
			      sa_hpi_sensor_reading_current_raw, 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status  = snmp_get2(custom_handle->ss,
				    anOID, 
				    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				    &get_value);

		if( (status == SA_OK) && (get_value.type == ASN_UNSIGNED) ) {
			data->Raw = (SaHpiUint32T)get_value.integer;
		} else {
			dbg("snmp_client_get_sensor_data: error getting RAW_READING");
		}

	} else {              
		if ((rdr->RdrTypeUnion.SensorRec.ThresholdDefn.TholdCapabilities & SAHPI_SRF_RAW)
		    != (data->ValuesPresent & SAHPI_SRF_RAW)) {
			dbg("RAW_READING: rdr vs. current values");
			dbg(" present RAW Flags DO NOT match"); 
			dbg("stored flags: %X, current flags: %X, status: %d",
			       rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingFormats,
			       data->ValuesPresent,
			       status);
		}
	}

	/* INTERPRETED_READING */
	if( (rdr->RdrTypeUnion.SensorRec.ThresholdDefn.TholdCapabilities & SAHPI_SRF_INTERPRETED)
	    && (data->ValuesPresent & SAHPI_SRF_INTERPRETED) 
	    && (status == SA_OK) ) {
		build_res_oid(anOID, 
			      sa_hpi_sensor_reading_current_intrepreted, 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status  = snmp_get2(custom_handle->ss,
				    anOID, 
				    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				    &get_value);
		if( (status == SA_OK) && (get_value.type == ASN_OCTET_STR) ) {
			if (get_value.str_len <= SAHPI_SENSOR_BUFFER_LENGTH) {
				memcpy(&data->Interpreted.Value, 
				       get_value.string, 
				       get_value.str_len);
				data->Interpreted.Type = SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER;
			} else
				dbg("ERROR: interpreted buff exceeds max allowed");

		} else {
			dbg("snmp_client_get_sensor_data: error getting");
			dbg(" INTERPRETED_READING ");
		}

	} else { 
		if ((rdr->RdrTypeUnion.SensorRec.ThresholdDefn.TholdCapabilities & SAHPI_SRF_INTERPRETED)
		    != (data->ValuesPresent & SAHPI_SRF_INTERPRETED)) {
			dbg("INTERPRETED_READING: rdr vs. current values");
			dbg("present RAW Flags DO NOT match");
			dbg("stored flags: %X, current flags: %X, status: %d",
			       rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingFormats,
			       data->ValuesPresent,
			       status);
		}
	}

	/* SENSOR_STATUS */
	if (status == SA_OK) {
	
		build_res_oid(anOID, 
			      sa_hpi_sensor_reading_current_status, 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status = snmp_get2(custom_handle->ss,
				   anOID, 
				   SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				   &get_value);
		if ( (status == SA_OK) && (get_value.type == ASN_INTEGER) )
			data->EventStatus.SensorStatus = 
				(SaHpiSensorStatusT)SNMP_ENUM_ADJUST(get_value.integer);
			else
				dbg("snmp_client_get_sensor_data: error getting SENSOR_STATUS ");
	}

	/* SENSOR_EVENT_STATUS */
	if (status == SA_OK) {

		build_res_oid(anOID, 
			      sa_hpi_sensor_reading_current_evt_status, 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status = snmp_get2(custom_handle->ss,
				   anOID, 
				   SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				   &get_value);
		if ( (status == SA_OK) && (get_value.type == ASN_OCTET_STR) ) {
			SaHpiTextBufferT buffer;
			SaHpiEventCategoryT cat;
			
			oh_init_textbuffer(&buffer);
			oh_append_textbuffer(&buffer, get_value.string);
			oh_encode_eventstate(&buffer,
					     &data->EventStatus.EventStatus,
					     &cat);
#if 0
			build_state_value(get_value.string,
					  get_value.str_len,
					  &data->EventStatus.EventStatus);
#endif
		}
		else
			dbg("snmp_client_get_sensor_data: error getting SENSOR_EVENT_STATUS ");
	}

	return(status);
}
 	   

static int snmp_client_get_sensor_thresholds(void *hnd, 
					     SaHpiResourceIdT id,
					     SaHpiSensorNumT num,
					     SaHpiSensorThresholdsT *thres)
{
	/* pointer to MIB OID's for given Sensor table */
	oid **oid_ptr;

	SaErrorT status = SA_OK;

	SaHpiRdrT *rdr = NULL;

	struct rdr_data *remote_rdr_data = NULL;
        
	oid indices[NUM_SEN_INDICES];

        struct oh_handler_state *handle = 
		(struct oh_handler_state *)hnd;

        struct snmp_client_hnd *custom_handle = 
		(struct snmp_client_hnd *)handle->data;

	/* clear thres for fresh information */
	memset(thres, 0, sizeof(*thres));

        if(!(rdr = 
		oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num)) ) {
		dbg("ERROR finding rdr in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

        if (!(remote_rdr_data =
                (struct rdr_data *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId))) {
		dbg(" ERROR finding rdr_data in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

	/* INDEX   { saHpiDomainID, saHpiResourceID, saHpiSensorIndex }	*/
	indices[0] = (oid)remote_rdr_data->index.remote_domain;
	indices[1] = (oid)remote_rdr_data->index.remote_resource_id;
	indices[2] = (oid)num;

	/* Check Thresholds Supprorted */
	if (rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsThreshold
	    == SAHPI_TRUE) {
							 
		/* SAHPI_SENSOR_READING_THD_LOW_CRITICAL_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_LOW_CRITICAL_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
        					  &thres->LowCritical);

		/* SAHPI_SENSOR_READING_THD_LOW_MAJORL_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_LOW_MAJORL_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->LowMajor);

		/* SAHPI_SENSOR_READING_THD_LOW_MINORL_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_LOW_MINORL_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->LowMinor);

		/* SAHPI_SENSOR_READING_THD_UP_CRITICAL_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_UP_CRITICAL_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->UpCritical);

		/* SAHPI_SENSOR_READING_THD_UP_MAJOR_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_UP_MAJOR_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->UpMajor);

		/* SAHPI_SENSOR_READING_THD_UP_MINOR_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_UP_MINOR_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->UpMinor);

		/* SAHPI_SENSOR_READING_THD_POS_HYSTERESIS_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_POS_HYSTERESIS_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->PosThdHysteresis);

		/* SAHPI_SENSOR_READING_THD_NEG_HYSTERESIS_TABLE readings */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_NEG_HYSTERESIS_TABLE);
		status = get_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->NegThdHysteresis);
	}

	return(status);
}


static int snmp_client_set_sensor_thresholds(void *hnd, SaHpiResourceIdT id,
					     SaHpiSensorNumT num,
					     const SaHpiSensorThresholdsT *thres)
{
	/* pointer to MIB OID's for given Sensor table */
	oid **oid_ptr;

	SaErrorT status = SA_OK;

	SaHpiRdrT *rdr = NULL;

	struct rdr_data *remote_rdr_data = NULL;
        
	oid indices[NUM_SEN_INDICES];

        struct oh_handler_state *handle = 
		(struct oh_handler_state *)hnd;

        struct snmp_client_hnd *custom_handle = 
		(struct snmp_client_hnd *)handle->data;

        if(!(rdr = 
		oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num)) ) {
		dbg("ERROR finding rdr in sreadingsnmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

        if (!(remote_rdr_data =
                (struct rdr_data *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId))) {
		dbg(" ERROR finding rdr_data in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

	/* INDEX   { saHpiDomainID, saHpiResourceID, saHpiSensorIndex }	*/
	indices[0] = (oid)remote_rdr_data->index.remote_domain;
	indices[1] = (oid)remote_rdr_data->index.remote_resource_id;
	indices[2] = (oid)num;

	/* Check Thresholds Supprorted */
	if (rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsThreshold
	    == SAHPI_TRUE) {
		
		/* SAHPI_SENSOR_READING_THD_LOW_CRITICAL_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_LOW_CRITICAL_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
        					  &thres->LowCritical);

		/* SAHPI_SENSOR_READING_THD_LOW_MAJORL_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_LOW_MAJORL_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->LowMajor);

		/* SAHPI_SENSOR_READING_THD_LOW_MINORL_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_LOW_MINORL_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->LowMinor);

		/* SAHPI_SENSOR_READING_THD_UP_CRITICAL_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_UP_CRITICAL_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->UpCritical);

		/* SAHPI_SENSOR_READING_THD_UP_MAJOR_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_UP_MAJOR_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->UpMajor);

		/* SAHPI_SENSOR_READING_THD_UP_MINOR_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_UP_MINOR_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->UpMinor);

		/* SAHPI_SENSOR_READING_THD_POS_HYSTERESIS_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_POS_HYSTERESIS_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->PosThdHysteresis);

		/* SAHPI_SENSOR_READING_THD_NEG_HYSTERESIS_TABLE set */ 
		oid_ptr = get_thold_table_oids(
			SAHPI_SENSOR_READING_THD_NEG_HYSTERESIS_TABLE);
		status = set_sensor_threshold_data(custom_handle, 
						  oid_ptr, 
						  indices, 
						  &thres->NegThdHysteresis);


	}
      
        return status;
}

static int snmp_client_get_sensor_event_enables(void *hnd, 
						SaHpiResourceIdT id,
						SaHpiSensorNumT num,
						SaHpiSensorEvtEnablesT *enables)
{
	SaErrorT status = SA_OK;
	struct snmp_value get_value;

	SaHpiRdrT *rdr = NULL;

	struct rdr_data *remote_rdr_data = NULL;
        
	oid anOID[MAX_OID_LEN];
	oid indices[NUM_SEN_INDICES];

        struct oh_handler_state *handle = 
		(struct oh_handler_state *)hnd;

        struct snmp_client_hnd *custom_handle = 
		(struct snmp_client_hnd *)handle->data;

	/* clear data for fresh information */
	memset(enables, 0, sizeof(*enables));

        if(!(rdr = 
		oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num)) ) {
		dbg("ERROR finding rdr in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

        if (!(remote_rdr_data =
                (struct rdr_data *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId))) {
		dbg(" ERROR finding rdr_data in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

	/* INDEX   { saHpiDomainID, saHpiResourceID, saHpiSensorIndex }	*/
	indices[0] = (oid)remote_rdr_data->index.remote_domain;
	indices[1] = (oid)remote_rdr_data->index.remote_resource_id;
	indices[2] = (oid)num;

	/* STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sen_status, 
		      SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_SEN_INDICES);
	
	status  = snmp_get2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &get_value);
	if( (status == SA_OK) && (get_value.type == ASN_INTEGER) )
		enables->SensorStatus = 
			(SaHpiSensorStatusT)SNMP_ENUM_ADJUST(get_value.integer);
	else
		dbg("snmp_client_get_sensor_data: error getting VALUES_PRESENT "); 

	/* ASSERTEVENTS */
	build_res_oid(anOID, 
		      sa_hpi_sen_assert_evts, 
		      SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_SEN_INDICES);
	
	status  = snmp_get2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &get_value);
	if( (status == SA_OK) && (get_value.type == ASN_OCTET_STR) ) {
		SaHpiTextBufferT buffer;
		SaHpiEventCategoryT cat;
		
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, get_value.string);
		oh_encode_eventstate(&buffer,
				     &enables->AssertEvents,
				     &cat);
#if 0
		build_state_value(get_value.string, 
				  get_value.str_len, 
				  &enables->AssertEvents);
#endif
	}
	else
		dbg("snmp_client_get_sensor_data: error getting ASSERTEVENTS ");

	/* DEASSERTEVENTS */
	build_res_oid(anOID, 
		      sa_hpi_sen_deassert_evts, 
		      SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_SEN_INDICES);
	
	status  = snmp_get2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &get_value);
	if( (status == SA_OK) && (get_value.type == ASN_OCTET_STR) ) {
		SaHpiTextBufferT buffer;
		SaHpiEventCategoryT cat;
		
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, get_value.string);
		oh_encode_eventstate(&buffer,
				     &enables->DeassertEvents,
				     &cat);
#if 0
		build_state_value(get_value.string, 
				  get_value.str_len,
				  &enables->DeassertEvents);
#endif
	}
	else
		dbg("snmp_client_get_sensor_data: error getting DEASSERTEVENTS ");


        return status;
}

static int snmp_client_set_sensor_event_enables(void *hnd, 
						SaHpiResourceIdT id,
						SaHpiSensorNumT num,
						const SaHpiSensorEvtEnablesT *enables)
{
	SaErrorT status = SA_OK;
	struct snmp_value set_value;

	SaHpiRdrT *rdr = NULL;

	struct rdr_data *remote_rdr_data = NULL;
        
	oid anOID[MAX_OID_LEN];
	oid indices[NUM_SEN_INDICES];

        struct oh_handler_state *handle = 
		(struct oh_handler_state *)hnd;

        struct snmp_client_hnd *custom_handle = 
		(struct snmp_client_hnd *)handle->data;

        if(!(rdr = 
		oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num)) ) {
		dbg("ERROR finding rdr in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

        if (!(remote_rdr_data =
                (struct rdr_data *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId))) {
		dbg(" ERROR finding rdr_data in snmp_client_get_sensor_data()");
		return(SA_ERR_HPI_ERROR);
	}

	/* INDEX   { saHpiDomainID, saHpiResourceID, saHpiSensorIndex }	*/
	indices[0] = (oid)remote_rdr_data->index.remote_domain;
	indices[1] = (oid)remote_rdr_data->index.remote_resource_id;
	indices[2] = (oid)num;

	/* STATUS */
	memset(&set_value, 0, sizeof(set_value));
	set_value.type = ASN_INTEGER;
	set_value.integer = enables->SensorStatus + 1;   
	set_value.str_len = sizeof(set_value.integer);

	build_res_oid(anOID, 
		      sa_hpi_sen_status, 
		      SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_SEN_INDICES);

	status  = snmp_set2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &set_value);

	/* ASSERTEVENTS */
	if (status == SA_OK) {
		SaHpiTextBufferT buffer;
		memset(&set_value, 0, sizeof(set_value));
		set_value.type = ASN_OCTET_STR;			  
		set_value.str_len = strlen(set_value.string);

		oh_decode_eventstate(enables->AssertEvents,
				     rdr->RdrTypeUnion.SensorRec.Category,
				     &buffer);
		strcpy(set_value.string, buffer.Data);
		set_value.str_len = buffer.DataLength;
#if 0
		build_state_string(rdr->RdrTypeUnion.SensorRec.Category,
				    enables->AssertEvents,
				    set_value.string, 
				    &set_value.str_len, 			
				    MAX_ASN_STR_LEN);
#endif
		
		build_res_oid(anOID, 
			      sa_hpi_sen_assert_evts, 
			      SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status  = snmp_set2(custom_handle->ss,
				    anOID, 
				    SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				    &set_value);
	}
	/* DEASSERTEVENTS */
	if (status == SA_OK) {
		SaHpiTextBufferT buffer;
		memset(&set_value, 0, sizeof(set_value));
		set_value.type = ASN_OCTET_STR;
		set_value.str_len = strlen(set_value.string);

		oh_decode_eventstate(enables->DeassertEvents,
				     rdr->RdrTypeUnion.SensorRec.Category,
				     &buffer);
		strcpy(set_value.string, buffer.Data);
		set_value.str_len = buffer.DataLength;

#if 0
		build_state_string(rdr->RdrTypeUnion.SensorRec.Category,
				    enables->DeassertEvents,
				    set_value.string, 
				    &set_value.str_len, 			
				    MAX_ASN_STR_LEN);
#endif

		build_res_oid(anOID, 
			      sa_hpi_sen_deassert_evts, 
			      SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status  = snmp_set2(custom_handle->ss,
				    anOID, 
				    SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				    &set_value);
	}

        return status;
}

static int snmp_client_get_control_state(void *hnd, 
					 SaHpiResourceIdT id,
					 SaHpiCtrlNumT num,
					 SaHpiCtrlStateT *state)
{
	SaErrorT status = SA_OK;
	struct snmp_value get_value;

	struct rdr_data *remote_rdr_data;
        
	oid anOID[MAX_OID_LEN];
	oid indices[NUM_CTRL_INDICES];

	char *data;

        struct oh_handler_state *handle = 
		(struct oh_handler_state *)hnd;

        struct snmp_client_hnd *custom_handle = 
		(struct snmp_client_hnd *)handle->data;

        SaHpiRdrT *rdr = 
		oh_get_rdr_by_type(handle->rptcache, id, SAHPI_CTRL_RDR, num);

        remote_rdr_data =
                (struct rdr_data *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);

	/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiCtrlNum } */
	indices[0] = (oid)remote_rdr_data->index.remote_domain;
	indices[1] = (oid)remote_rdr_data->index.remote_resource_id;
	indices[2] = (oid)num;

	/* get state type */
	build_res_oid(anOID, 
		      sa_hpi_ctrl_type, 
		      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_CTRL_INDICES);

	if( !snmp_get2(custom_handle->ss,
		  anOID, 
		  SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
		  &get_value) ) {

		state->Type = SNMP_ENUM_ADJUST((SaHpiCtrlTypeT)get_value.integer);

		/* get state value */
		build_res_oid(anOID, 
			      sa_hpi_ctrl_state, 
			      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_CTRL_INDICES);

		if( !snmp_get2(custom_handle->ss,
			  anOID, 
			  SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			  &get_value) ) {
			switch(state->Type) {
			case SAHPI_CTRL_TYPE_DIGITAL:
				state->StateUnion.Digital =
					 *(SaHpiCtrlStateDigitalT *)(void *)get_value.string;
				state->StateUnion.Digital = 
					SNMP_ENUM_ADJUST(ntohl(state->StateUnion.Digital));
				break;
			case SAHPI_CTRL_TYPE_DISCRETE:
				state->StateUnion.Discrete =
					*(SaHpiCtrlStateDiscreteT *)(void *)get_value.string;
				state->StateUnion.Discrete = 
					ntohl(state->StateUnion.Discrete);
				break;
			case SAHPI_CTRL_TYPE_ANALOG:
				state->StateUnion.Analog =
					*(SaHpiCtrlStateAnalogT *)(void *)get_value.string;
				state->StateUnion.Analog = 
					ntohl(state->StateUnion.Analog);
				break;
			case SAHPI_CTRL_TYPE_STREAM:  
				data = get_value.string;
				state->StateUnion.Stream.Repeat = 
                                        (*data == 1) ? SAHPI_TRUE : SAHPI_FALSE;
				data++;
				state->StateUnion.Stream.StreamLength = 
					*(SaHpiUint32T *)(void *)data;
				state->StateUnion.Stream.StreamLength = 
					ntohl(state->StateUnion.Stream.StreamLength);
				data+=sizeof(SaHpiUint32T);
				memcpy(&state->StateUnion.Stream.Stream, 
				       data, 
				       state->StateUnion.Stream.StreamLength);
				break;
			case SAHPI_CTRL_TYPE_TEXT:  
				/* get text type line number */
				state->StateUnion.Text.Line = 
					*(SaHpiTxtLineNumT *)get_value.string;

				/* get text type DateType */
				build_res_oid(anOID, 
					      sa_hpi_ctrl_txt_type, 
					      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
					      indices, 
					      NUM_CTRL_INDICES);
				status = snmp_get2(custom_handle->ss,
					       anOID, 
					       SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
					       &get_value);
				if (status != SA_OK) {
					dbg("ERROR: getting text DataType");
					break;
				}
				state->StateUnion.Text.Text.DataType =
					SNMP_ENUM_ADJUST((SaHpiTextTypeT)get_value.integer);

				/* get text type Language */
				build_res_oid(anOID, 
					      sa_hpi_ctrl_txt_lang, 
					      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
					      indices, 
					      NUM_CTRL_INDICES);
				status = snmp_get2(custom_handle->ss,
					       anOID, 
					       SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
					       &get_value);
				state->StateUnion.Text.Text.Language = 
					SNMP_ENUM_ADJUST((SaHpiLanguageT)get_value.integer);
				if (status != SA_OK) {
					dbg("ERROR: getting text Language");
					break;
				}
				/* get text type Data/Text */
					build_res_oid(anOID, 
					      sa_hpi_ctrl_txt, 
					      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
					      indices, 
					      NUM_CTRL_INDICES);
				status = snmp_get2(custom_handle->ss,
					       anOID, 
					       SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
					       &get_value);

				state->StateUnion.Text.Text.DataLength = get_value.str_len;
				memcpy(&state->StateUnion.Text.Text.Data, 
				       get_value.string, 
				       get_value.str_len);
				if (status != SA_OK)
					dbg("ERROR: getting text Data/Text");
				break;
			case SAHPI_CTRL_TYPE_OEM:        
				/* set the Mid from the State */
				memcpy(&state->StateUnion.Oem.MId, 
				       get_value.string,
				       get_value.str_len);
				state->StateUnion.Oem.MId = 
					ntohl(state->StateUnion.Oem.MId);

				/* get the Body and LengthBody from saHpiCtrlText */
				build_res_oid(anOID, 
				      sa_hpi_ctrl_txt, 
				      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
				      indices, 
				      NUM_CTRL_INDICES);
				status = snmp_get2(custom_handle->ss,
				       anOID, 
				       SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				       &get_value);
				if (status != SA_OK) {
					dbg("ERROR: getting text Data/Text");
					break;
				}
				state->StateUnion.Oem.BodyLength =
					get_value.str_len;
				memcpy(state->StateUnion.Oem.Body, 
				       get_value.string, 
				       get_value.str_len);
				break;
			default:                      
				dbg("ERROR:snmp_client_get_control_state, unknow rdr ctrl type");  
				break;
			}
		} else {
			status = SA_ERR_HPI_NO_RESPONSE;
			dbg("ERROR:failure to get rdr ctrl state");
		}
	} else {
		status = SA_ERR_HPI_NO_RESPONSE;
		dbg("ERROR: failure to get rdr ctrl type");
	}

	return(status);
}

static int snmp_client_set_control_state(void *hnd, SaHpiResourceIdT id,
					 SaHpiCtrlNumT num,
					 SaHpiCtrlStateT *state)
{

dbg("TODO: ******************* snmp_client_set_control_state() *******************");

#if 0
        gchar *oid;
	int value;
        struct snmp_value set_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_client_hnd *custom_handle = (struct snmp_client_hnd *)handle->data;
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_CTRL_RDR, num);
        struct BC_ControlInfo *s =
                (struct BC_ControlInfo *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);

	if(state->Type != rdr->RdrTypeUnion.CtrlRec.Type) {
		dbg("Control %s type %d cannot be changed",s->mib.oid,state->Type);
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	switch (state->Type) {
	case SAHPI_CTRL_TYPE_DIGITAL:

		/* More icky dependencies on SaHpiStateDigitalT enum */
		switch (state->StateUnion.Digital) {
		case SAHPI_CTRL_STATE_OFF:
			value = s->mib.digitalmap[SAHPI_CTRL_STATE_OFF];
		case SAHPI_CTRL_STATE_ON:
			value = s->mib.digitalmap[SAHPI_CTRL_STATE_ON];
		case SAHPI_CTRL_STATE_PULSE_OFF:
			value = s->mib.digitalmap[SAHPI_CTRL_STATE_PULSE_OFF];
		case SAHPI_CTRL_STATE_PULSE_ON:
			value = s->mib.digitalmap[SAHPI_CTRL_STATE_PULSE_ON];
		case SAHPI_CTRL_STATE_AUTO:
			value = s->mib.digitalmap[ELEMENTS_IN_SaHpiStateDigitalT - 1];
		default:
			dbg("Spec Change: MAX_SaHpiStateDigitalT incorrect?");
			return -1;
		}

		if(value < 0) {
			dbg("Control state %d not allowed to be set",state->StateUnion.Digital);
			return SA_ERR_HPI_INVALID_CMD;
		}

		oid = snmp_derive_objid(rdr->Entity, s->mib.oid);
		if(oid == NULL) {
			dbg("NULL SNMP OID returned for %s",s->mib.oid);
			return -1;
		}

		set_value.type = ASN_INTEGER;
		set_value.integer = value;

		if((snmp_client_set(custom_handle->ss, oid, set_value) != 0)) {
			dbg("SNMP could not set %s; Type=%d.",s->mib.oid,set_value.type);
			g_free(oid);
			return SA_ERR_HPI_NO_RESPONSE;
		}
		g_free(oid);

	case SAHPI_CTRL_TYPE_DISCRETE:
		dbg("Discrete controls not supported");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_ANALOG:
		dbg("Analog controls not supported");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_STREAM:
		dbg("Stream controls not supported");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_TEXT:
		dbg("Text controls not supported");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_OEM:	
		dbg("Oem controls not supported");
		return SA_ERR_HPI_INVALID_CMD;
        default:
		dbg("Request has invalid control state=%d", state->Type);
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	
        return SA_OK;
#endif
return(-1);
}

static int snmp_client_get_inventory_size(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num, /* yes, they don't call it a
                                                    * num, but it still is one
                                                    */
                                      SaHpiUint32T *size)
{
dbg("TODO: ******************* snmp_client_get_inventory_size() *******************");

        return -1;
}

static int snmp_client_get_inventory_info(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num,
                                      SaHpiInventoryDataT *data)
{
dbg("TODO: ******************* snmp_client_get_inventory_info() *******************");
        return -1;
}

static int snmp_client_set_inventory_info(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num,
                                      const SaHpiInventoryDataT *data)
{
dbg("TODO: ******************* snmp_client_set_inventory_info() *******************");
        return -1;
}

static int snmp_client_get_watchdog_info(void *hnd, SaHpiResourceIdT id,
                                     SaHpiWatchdogNumT num,
                                     SaHpiWatchdogT *wdt)
{
dbg("TODO: ******************* snmp_client_get_watchdog_info() *******************");
	/* Watchdog not supported */
        return SA_ERR_HPI_NOT_PRESENT;
}

static int snmp_client_set_watchdog_info(void *hnd, SaHpiResourceIdT id,
                                     SaHpiWatchdogNumT num,
                                     SaHpiWatchdogT *wdt)
{
dbg("TODO: ******************* snmp_client_set_watchdog_info() *******************");
	/* Watchdog not supported */
        return SA_ERR_HPI_NOT_PRESENT;
}

static int snmp_client_reset_watchdog(void *hnd, SaHpiResourceIdT id,
                                  SaHpiWatchdogNumT num)
{

dbg("TODO: ******************* snmp_client_reset_watchdog() *******************");
 	/* Watchdog not supported */
        return SA_ERR_HPI_NOT_PRESENT;
}

static int snmp_client_get_hotswap_state(void *hnd, SaHpiResourceIdT id,
				     SaHpiHsStateT *state)
{
dbg("TODO: ******************* snmp_client_get_hotswap_state() *******************");
        /* TODO: when is a resource ACTIVE_UNHEALTHY */
        *state = SAHPI_HS_STATE_ACTIVE_HEALTHY;

        return SA_OK;
}

static int snmp_client_set_hotswap_state(void *hnd, SaHpiResourceIdT id,
				     SaHpiHsStateT state)
{
dbg("TODO: ******************* snmp_client_set_hotswap_state() *******************");
        return -1;
}

static int snmp_client_request_hotswap_action(void *hnd, SaHpiResourceIdT id,
				          SaHpiHsActionT act)
{
dbg("TODO: ******************* snmp_client_request_hotswap_action() *******************");
        return -1;
}

static int snmp_client_get_power_state(void *hnd, SaHpiResourceIdT id,
			           SaHpiHsPowerStateT *state)
{
dbg("TODO: ******************* snmp_client_get_power_state() *******************");
        return -1;
}

static int snmp_client_set_power_state(void *hnd, SaHpiResourceIdT id,
			           SaHpiHsPowerStateT state)
{
dbg("TODO: ******************* snmp_client_set_power_state() *******************");
        return -1;
}

static int snmp_client_get_indicator_state(void *hnd, SaHpiResourceIdT id,
				       SaHpiHsIndicatorStateT *state)
{
dbg("TODO: ******************* snmp_client_get_indicator_state() *******************");
        return -1;
}

static int snmp_client_set_indicator_state(void *hnd, SaHpiResourceIdT id,
				       SaHpiHsIndicatorStateT state)
{
dbg("TODO: ******************* snmp_client_set_indicator_state() *******************");
        return -1;
}

static int snmp_client_control_parm(void *hnd, SaHpiResourceIdT id, SaHpiParmActionT act)
{
dbg("TODO: ******************* snmp_client_control_parm() *******************");
        return -1;
}

static int snmp_client_get_reset_state(void *hnd, SaHpiResourceIdT id,
			           SaHpiResetActionT *act)
{
dbg("TODO: ******************* snmp_client_get_reset_state() *******************");
        return -1;
}

static int snmp_client_set_reset_state(void *hnd, SaHpiResourceIdT id,
			           SaHpiResetActionT act)
{
dbg("TODO: ******************* snmp_client_set_reset_state() *******************");
        return -1;

}

struct oh_abi_v2 oh_snmp_client_plugin = {
        .open				= snmp_client_open,
        .close				= snmp_client_close,
        .get_event			= snmp_client_get_event,
        .discover_resources     	= snmp_client_discover_resources,
        .get_self_id			= snmp_client_get_self_id,
//        .get_sel_info			= snmp_client_get_sel_info,
        .get_sel_info			= NULL,
//        .set_sel_time			= snmp_client_set_sel_time,
        .set_sel_time			= NULL,
//        .add_sel_entry			= snmp_client_add_sel_entry,
        .add_sel_entry			= NULL,
//        .del_sel_entry			= snmp_client_del_sel_entry,
        .del_sel_entry			= NULL,
//	.get_sel_entry			= snmp_client_get_sel_entry,        
	.get_sel_entry			= NULL,
	.get_sensor_data		= snmp_client_get_sensor_data,
        .get_sensor_thresholds		= snmp_client_get_sensor_thresholds,
        .set_sensor_thresholds		= snmp_client_set_sensor_thresholds,
        .get_sensor_event_enables	= snmp_client_get_sensor_event_enables,
        .set_sensor_event_enables	= snmp_client_set_sensor_event_enables,
        .get_control_state		= snmp_client_get_control_state,
        .set_control_state		= snmp_client_set_control_state,
        .get_inventory_size		= snmp_client_get_inventory_size,
        .get_inventory_info		= snmp_client_get_inventory_info,
        .set_inventory_info		= snmp_client_set_inventory_info,
        .get_watchdog_info		= snmp_client_get_watchdog_info,
        .set_watchdog_info		= snmp_client_set_watchdog_info,
        .reset_watchdog			= snmp_client_reset_watchdog,
        .get_hotswap_state		= snmp_client_get_hotswap_state,
        .set_hotswap_state		= snmp_client_set_hotswap_state,
        .request_hotswap_action		= snmp_client_request_hotswap_action,
        .get_power_state		= snmp_client_get_power_state,
        .set_power_state		= snmp_client_set_power_state,
        .get_indicator_state		= snmp_client_get_indicator_state,
        .set_indicator_state		= snmp_client_set_indicator_state,
        .control_parm			= snmp_client_control_parm,
        .get_reset_state		= snmp_client_get_reset_state,
        .set_reset_state		= snmp_client_set_reset_state
};

int get_interface(void **pp, const uuid_t uuid)
{
        if(uuid_compare(uuid, UUID_OH_ABI_V2)==0) {
                *pp = &oh_snmp_client_plugin;
                return 0;
        }

        *pp = NULL;
        return -1;
}
