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
 * Authors:
 *	David Judkovics <djudkovi@us.ibm.com>
 *
 */
 

#include <stdio.h>
#include <time.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <epath_utils.h>
#include <rpt_utils.h>
#include <uid_utils.h>
#include <snmp_util.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <snmp_client.h>
#include <snmp_client_res.h>
#include <snmp_client_utils.h>

#include <netinet/in.h>

typedef struct state_category_string_ 
{
    SaHpiEventCategoryT category;
    SaHpiEventStateT state;
    char *str;
} state_category_string;

static state_category_string state_string[] = {
    {SAHPI_EC_UNSPECIFIED, SAHPI_ES_UNSPECIFIED, "UNSPECIFIED"},
    {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_MINOR, "LOWER_MINOR"},
    {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_MAJOR, "LOWER_MAJOR"},
    {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_CRIT, "LOWER_CRIT"},
    {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_MINOR, "UPPER_MINOR"},
    {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_MAJOR, "UPPER_MAJOR"},
    {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_CRIT, "UPPER_CRIT"},
    {SAHPI_EC_USAGE, SAHPI_ES_IDLE, "IDLE"},
    {SAHPI_EC_USAGE, SAHPI_ES_ACTIVE, "ACTIVE"},
    {SAHPI_EC_USAGE, SAHPI_ES_BUSY, "BUSY"},
    {SAHPI_EC_STATE, SAHPI_ES_STATE_DEASSERTED, "STATE_DEASSERTED"},
    {SAHPI_EC_STATE, SAHPI_ES_STATE_ASSERTED, "STATE_ASSERTED"},
    {SAHPI_EC_PRED_FAIL, SAHPI_ES_PRED_FAILURE_DEASSERT, "PRED_FAILURE_DEASSERT"},
    {SAHPI_EC_PRED_FAIL, SAHPI_ES_PRED_FAILURE_ASSERT, "PRED_FAILURE_ASSERT"},
    {SAHPI_EC_LIMIT, SAHPI_ES_LIMIT_NOT_EXCEEDED, "LIMIT_NOT_EXCEEDED"},
    {SAHPI_EC_LIMIT, SAHPI_ES_LIMIT_EXCEEDED, "LIMIT_EXCEEDED"},
    {SAHPI_EC_PERFORMANCE, SAHPI_ES_PERFORMANCE_MET, "PERFORMANCE_MET"},
    {SAHPI_EC_PERFORMANCE, SAHPI_ES_PERFORMANCE_LAGS, "PERFORMANCE_LAGS"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_OK, "OK"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_MINOR_FROM_OK, "MINOR_FROM_OK"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_MAJOR_FROM_LESS, "MAJOR_FROM_LESS"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_CRITICAL_FROM_LESS, "CRITICAL_FROM_LESS"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_MINOR_FROM_MORE, "MINOR_FROM_MORE"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_MAJOR_FROM_CRITICAL, "MAJOR_FROM_CRITICAL"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_CRITICAL, "CRITICAL"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_MONITOR, "MONITOR"},
    {SAHPI_EC_SEVERITY, SAHPI_ES_INFORMATIONAL, "INFORMATIONAL"},
    {SAHPI_EC_PRESENCE, SAHPI_ES_ABSENT, "ABSENT"},
    {SAHPI_EC_PRESENCE, SAHPI_ES_PRESENT, "PRESENT"},
    {SAHPI_EC_ENABLE, SAHPI_ES_DISABLED, "DISABLED"},
    {SAHPI_EC_ENABLE, SAHPI_ES_ENABLED, "ENABLED"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_RUNNING, "RUNNING"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_TEST, "TEST"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_POWER_OFF, "POWER_OFF"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_ON_LINE, "ON_LINE"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_OFF_LINE, "OFF_LINE"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_OFF_DUTY, "OFF_DUTY"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_DEGRADED, "DEGRADED"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_POWER_SAVE, "POWER_SAVE"},
    {SAHPI_EC_AVAILABILITY, SAHPI_ES_INSTALL_ERROR, "INSTALL_ERROR"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_FULLY_REDUNDANT, "FULLY_REDUNDANT"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_LOST, "REDUNDANCY_LOST"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED, "REDUNDANCY_DEGRADED"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES, "REDUNDANCY_LOST_SUFFICIENT_RESOURCES"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES, "NON_REDUNDANT_SUFFICIENT_RESOURCES"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES, "NON_REDUNDANT_INSUFFICIENT_RESOURCES"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL, "REDUNDANCY_DEGRADED_FROM_FULL"},
    {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON, "REDUNDANCY_DEGRADED_FROM_NON"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_00, "STATE_00"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_01, "STATE_01"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_02, "STATE_02"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_03, "STATE_03"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_04, "STATE_04"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_05, "STATE_05"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_06, "STATE_06"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_07, "STATE_07"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_08, "STATE_08"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_09, "STATE_09"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_10, "STATE_10"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_11, "STATE_11"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_12, "STATE_12"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_13, "STATE_13"},
    {SAHPI_EC_GENERIC , SAHPI_ES_STATE_14, "STATE_14"}};

#define STATESTRING_VALUE_DELIMITER ", "
#define STATESTRING_VALUE_DELIMITER_LENGTH 2
#define STATESTRING_MAX_LENGTH 1000
#define STATESTRING_MAX_ENTRIES 63

typedef struct sensor_range_flags_
{
	SaHpiSensorRangeFlagsT flag;
	const unsigned char* str;
} sensor_range_flags;

static sensor_range_flags range_flags[] = {
  {SAHPI_SRF_MIN,"MIN"},
  {SAHPI_SRF_MAX, "MAX"},
  {SAHPI_SRF_NORMAL_MIN, "NORMAL_MIN"},
  {SAHPI_SRF_NORMAL_MAX, "NORMAL_MAX"},
  {SAHPI_SRF_NOMINAL,"NOMINAL"}};
#define FLAGSTRING_VALUE_DELIMITER ", "
#define FLAGSTRING_VALUE_DELIMITER_LENGTH 2
#define RANGE_FLAGS_LEN 5



/**
 * snmp_get: Gets a single value indicated by the objectid
 * using snmp.
 * @handle: a handle to the snmp session needed to make an
 * snmp transaction.
 * @objid: string containing the OID entry.
 * @value: the value received from snmp will be put in this union.
 *
 * In the case of multiple values being returned, the type in 'value' will be
 * ASN_NULL (0x05). Nothing else in 'value' will be filled in.
 * Use snmp_get_all for doing gets that return multiple values.
 *
 * Return value: Returns 0 if successful, -1 if there was an error.
 **/
SaErrorT snmp_get2(struct snmp_session *ss, 
	           oid *objid,
	           size_t objid_len,
                   struct snmp_value *value) 
{
        struct snmp_pdu *pdu;
        struct snmp_pdu *response;
        
        struct variable_list *vars;
        int status;
        
        /*
         * Create the PDU for the data for our request.
         */
        pdu = snmp_pdu_create(SNMP_MSG_GET);

        snmp_add_null_var(pdu, objid, objid_len);

        /*
         * Send the Request out.
         */
        status = snmp_synch_response(ss, pdu, &response);

        /*
         * Process the response.
         */
        if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
                vars = response->variables;
                value->type = vars->type;
                if (vars->next_variable != NULL) {
                        value->type = ASN_NULL;
                }/* If there are more values, set return type to null. */
                else if ( (vars->type == ASN_INTEGER) || 
			  (vars->type == ASN_COUNTER) || 
			  (vars->type == ASN_UNSIGNED) ) {
                        value->integer = *(vars->val.integer);
                } else {
                        value->str_len = vars->val_len;
                        if (value->str_len >= MAX_ASN_STR_LEN)
                                value->str_len = MAX_ASN_STR_LEN;
                        else value->string[value->str_len] = '\0';
                        
                        memcpy(value->string, vars->val.string, value->str_len);
                       
                }
		/* display data */
		printf("********************************************************\n");
			if (CHECK_END(vars->type)) { 
				print_variable(vars->name, vars->name_length, vars);  
			}  else 
				fprintf(stderr,"No idea.\n");
		printf("********************************************************\n");

        } else {
		int i;
                value->type = (u_char)0x00; /* Set return type to 0 in case of error. */
                if (status == STAT_SUCCESS) {
                        fprintf(stderr, "Error, Reason: %s\n", 
				snmp_errstring(response->errstat));
			fprintf(stderr, "objid: ");
			for(i = 0; i<objid_len; i++ )
				fprintf(stderr, "%d.", (int)objid[i]);
			fprintf(stderr, "\n");
                } else
                        snmp_sess_perror("snmpget", ss);
        }

        /* Clean up: free the response */
        if (response) snmp_free_pdu(response);

        return value->type? SA_OK : -1;
}


int snmp_getn_bulk( struct snmp_session *ss, 
		    oid *bulk_objid, 
		    size_t bulk_objid_len,
		    struct snmp_pdu *bulk_pdu, 
		    struct snmp_pdu **bulk_response,
		    int num_repetitions )
{
		int status;

	bulk_pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
 	
	bulk_pdu->non_repeaters = 0; 
	
	bulk_pdu->max_repetitions = num_repetitions;

	snmp_add_null_var(bulk_pdu, bulk_objid, bulk_objid_len);
	
	/* Send the Request out.*/
	status = snmp_synch_response(ss, bulk_pdu, bulk_response);

	return(status);

}

void build_res_oid(oid *d, oid *s, int s_len, oid *indices, int num_indices) 
{ 
	int i;

	memcpy(d, s, (s_len * sizeof(*s)));

	for (i = 0; i < num_indices; i++) {
		d[s_len++] = indices[i]; 
	}
}

SaErrorT net_snmp_failure(struct snmp_client_hnd *custom_handle, int snmp_status, 
				 struct snmp_pdu *response)
{
	if (snmp_status == STAT_SUCCESS)
		fprintf(stderr, "Error in packet, Whilst getting Resources\nReason: %s\n", snmp_errstring(response->errstat));
	else
		snmp_sess_perror("snmpget", custom_handle->ss);

	return(SA_ERR_HPI_ERROR);
}

void display_vars( struct snmp_pdu *response) 
{	
	int c = 0;
	struct variable_list *vars;

	printf("********************************************************\n");
	for(vars = response->variables; vars; vars = vars->next_variable) {
		c++;
		printf("\n**** oid count %d ******\n", c);
		if (CHECK_END(vars->type)) { 
			print_variable(vars->name, vars->name_length, vars);  
		}  else 
			fprintf(stderr,"No idea.\n");
	}
	printf("********************************************************\n");
}

int build_state_value (char *str,
		       size_t len,
		       SaHpiEventStateT *state) 
{

  int rc = SA_OK;
  char *s = NULL;
  char *delim = NULL;
  char *tok = NULL;
  int i = 0;

  s = (char *)malloc (len);
  if (s == NULL)
    return SA_ERR_HPI_ERROR;

  delim = (char *) malloc ( STATESTRING_VALUE_DELIMITER_LENGTH );
  if (delim == NULL) {
    free (s);
    return SA_ERR_HPI_ERROR;
  }

  memcpy (s, str, len);
  memcpy (delim, STATESTRING_VALUE_DELIMITER, STATESTRING_VALUE_DELIMITER_LENGTH);
  tok = strtok(s, delim);
  while (tok != NULL) {
    
    /*  snmp_log(LOG_INFO,"Tok: [%s]\n", tok); */
    for (i = 0; i <STATESTRING_MAX_ENTRIES; i++) {
      if (strncasecmp(tok, state_string[i].str,strlen(state_string[i].str)) == 0) {
	/*
	snmp_log(LOG_INFO,"Matched: %X [%s] = [%s]\n", 
		 state_string[i].state,
		 state_string[i].str,
		 tok);
	*/
	*state = *state + state_string[i].state;
      }
    }
    tok = strtok(NULL, delim);
  }
  
  free (s);
  free (delim);
  return rc;
}

int build_flag_value (char *str,
		      size_t len,
		      SaHpiSensorRangeFlagsT *flags) 
{

  int rc = SA_OK;
  char *s = NULL;
  char *delim = NULL;
  char *tok = NULL;
  int i = 0;

  s = (char *)g_malloc0(len);
  if (s == NULL)
    return SA_ERR_HPI_ERROR;

  delim = (char *)g_malloc0(FLAGSTRING_VALUE_DELIMITER_LENGTH);
  if (delim == NULL) {
    free (s);
    return SA_ERR_HPI_ERROR;
  }
  

  dbg("****** Find set Flags********");
  memcpy (s, str, len);
  memcpy (delim, FLAGSTRING_VALUE_DELIMITER, FLAGSTRING_VALUE_DELIMITER_LENGTH);
  tok = strtok(s, delim);
  while (tok != NULL) {
    for (i = 0; i < RANGE_FLAGS_LEN; i++) {
      if (strncasecmp(tok, range_flags[i].str, strlen(range_flags[i].str)) == 0) {

	printf(" Matched: %X[%s] = [%s] \n", range_flags[i].flag, range_flags[i].str, tok); 

	*flags = *flags | range_flags[i].flag;
      }
    }
    tok = strtok(NULL, delim);
  
  }			
  
  g_free (s);
  g_free (delim);

  printf("*** build_flag_value: flags %X *****\n", *flags);

  return rc;
}
