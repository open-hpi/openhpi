/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003
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
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <oh_utils.h>
#include <snmp_util.h>

#include <snmp_client.h>
#include <snmp_client_res.h>
#include <snmp_client_utils.h>
#include <sc_sensor_data.h>

#include <netinet/in.h>

SaErrorT populate_range_max(struct snmp_client_hnd *custom_handle, 
			    SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
			    int num_rdrs, 
			    SaHpiUint8T rr_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorReadingT *sen_max_reading = 
		g_malloc0(rr_num * sizeof(*sen_max_reading));
	if (!sen_max_reading){
		dbg("ERROR: in populate_range_max(), allocating sen_max_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # of range readings of type max, rr_num %d *****" ,rr_num);

        /* SA_HPI_SENSOR_READING_MAX_VALUE_PRESENT */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     sa_hpi_sensor_reading_max_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_max_reading[i].ValuesPresent =
                                        (SaHpiSensorReadingFormatsT)SNMP_ENUM_ADJUST(*vars->val.integer);
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_MAX_VALUE_PRESENT:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MAX_RAW */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_max_values_present, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_UNSIGNED ) 
				sen_max_reading[i].Raw =
                                        (SaHpiSensorReadingFormatsT)*vars->val.integer; 
                        else
                                dbg("SA_HPI_SENSOR_READING_MAX_RAW:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MAX_INTERPRETED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_max_raw, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) { 
				if(vars->val_len <= SAHPI_SENSOR_BUFFER_LENGTH) 
					memcpy(&sen_max_reading[i].Interpreted.Value.SensorBuffer,
					       vars->val.string,
					       vars->val_len);
                        } else
                                dbg("SA_HPI_SENSOR_READING_MAX_INTERPRETED:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MAX_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_max_intrepreted, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) 
				sen_max_reading[i].EventStatus.SensorStatus =
					(SaHpiSensorStatusT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                dbg("SA_HPI_SENSOR_READING_MAX_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MAX_EVENT_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_max_status, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) 
				build_state_value(vars->val.string,
						  vars->val_len,
						  &sen_max_reading[i].EventStatus.EventStatus);
                        else
                                dbg("SA_HPI_SENSOR_READING_MAX_EVENT_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SENSOR READINGS MAX  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** Adding sen_max_reading[] to cap[]             	      **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_rdrs; i++) {
                        if (sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_MAX ) {
                                if (ii >= rr_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered MAX READING RANGE data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }

                                sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Max = 
					sen_max_reading[ii++];
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_max_reading cache */
        if (sen_max_reading) 
                g_free(sen_max_reading);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_range_min(struct snmp_client_hnd *custom_handle, 
			    SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
			    int num_sen_rdrs, 
			    SaHpiUint8T rr_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorReadingT *sen_min_reading = 
		g_malloc0(rr_num * sizeof(*sen_min_reading));
	if (!sen_min_reading){
		dbg("ERROR: in populate_range_max(), allocating sen_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # of range readings of type min, rr_num %d *****" ,rr_num);

        /* SA_HPI_SENSOR_READING_MIN_VALUE_PRESENT */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     sa_hpi_sensor_reading_min_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_min_reading[i].ValuesPresent =
                                        (SaHpiSensorReadingFormatsT)SNMP_ENUM_ADJUST(*vars->val.integer);
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_MIN_VALUE_PRESENT:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MIN_RAW */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_min_values_present, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_UNSIGNED ) 
				sen_min_reading[i].Raw =
                                        (SaHpiSensorReadingFormatsT)*vars->val.integer; 
                        else
                                dbg("SA_HPI_SENSOR_READING_MIN_RAW:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MIN_INTERPRETED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_min_raw, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) { 
				if(vars->val_len <= SAHPI_SENSOR_BUFFER_LENGTH) 
					memcpy(&sen_min_reading[i].Interpreted.Value.SensorBuffer,
					       vars->val.string,
					       vars->val_len);
                        } else
                                dbg("SA_HPI_SENSOR_READING_MIN_INTERPRETED:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MIN_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_min_intrepreted, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) 
				sen_min_reading[i].EventStatus.SensorStatus =
					(SaHpiSensorStatusT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                dbg("SA_HPI_SENSOR_READING_MIN_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_MIN_EVENT_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_min_status, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) 
				build_state_value(vars->val.string,
						  vars->val_len,
						  &sen_min_reading[i].EventStatus.EventStatus);
                        else
                                dbg("SA_HPI_SENSOR_READING_MIN_EVENT_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SENSOR READINGS MIN  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** Adding sen_min_reading[] to cap[]             	 **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_MIN ) {
                                if (ii >= rr_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered MIN READING RANGE data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Min = 
					sen_min_reading[ii++];
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_min_reading cache */
        if (sen_min_reading) 
                g_free(sen_min_reading);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_range_nominal(struct snmp_client_hnd *custom_handle, 
				SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				int num_sen_rdrs, 
				SaHpiUint8T rr_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorReadingT *sen_nominal_reading = 
		g_malloc0(rr_num * sizeof(*sen_nominal_reading));
	if (!sen_nominal_reading){
		dbg("ERROR: in populate_range_max(), allocating sen_nominal_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # of range readings of type nominal, rr_num %d *****" ,rr_num);

        /* SA_HPI_SENSOR_READING_NOMINAL_VALUE_PRESENT */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     sa_hpi_sensor_reading_nominal_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_nominal_reading[i].ValuesPresent =
                                        (SaHpiSensorReadingFormatsT)SNMP_ENUM_ADJUST(*vars->val.integer);
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_NOMINAL_VALUE_PRESENT:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
  	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NOMINAL_RAW */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_nominal_values_present, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_UNSIGNED ) 
				sen_nominal_reading[i].Raw =
                                        (SaHpiSensorReadingFormatsT)*vars->val.integer; 
                        else
                                dbg("SA_HPI_SENSOR_READING_NOMINAL_RAW:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NOMINAL_INTERPRETED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_nominal_raw, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) { 
				if(vars->val_len <= SAHPI_SENSOR_BUFFER_LENGTH) 
					memcpy(&sen_nominal_reading[i].Interpreted.Value.SensorBuffer,
					       vars->val.string,
					       vars->val_len);
                        } else
                                dbg("SA_HPI_SENSOR_READING_NOMINAL_INTERPRETED:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NOMINAL_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_nominal_intrepreted, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) 
				sen_nominal_reading[i].EventStatus.SensorStatus =
					(SaHpiSensorStatusT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                dbg("SA_HPI_SENSOR_READING_NOMINAL_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NOMINAL_EVENT_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_nominal_status, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) 
				build_state_value(vars->val.string,
						  vars->val_len,
						  &sen_nominal_reading[i].EventStatus.EventStatus);
                        else
                                dbg("SA_HPI_SENSOR_READING_NOMINAL_EVENT_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SENSOR READINGS NOMINAL  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** Adding sen_nominal_reading[] to cap[]             	 **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_NOMINAL ) {
                                if (ii >= rr_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered NOMINAL READING RANGE data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Nominal = 
					sen_nominal_reading[ii++];
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_nominal_reading cache */
        if (sen_nominal_reading) 
                g_free(sen_nominal_reading);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_range_normal_max(struct snmp_client_hnd *custom_handle, 
				   SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				   int num_sen_rdrs, 
				   SaHpiUint8T rr_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorReadingT *sen_normal_max_reading = 
		g_malloc0(rr_num * sizeof(*sen_normal_max_reading));
	if (!sen_normal_max_reading){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_max_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # of range readings of type normal max, rr_num %d *****" ,rr_num);

        /* SA_HPI_SENSOR_READING_NORMAL_MAX_VALUE_PRESENT */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_reading_normal_max_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_normal_max_reading[i].ValuesPresent =
                                        (SaHpiSensorReadingFormatsT)SNMP_ENUM_ADJUST(*vars->val.integer);
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MAX_VALUE_PRESENT:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MAX_RAW */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_max_values_present, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_UNSIGNED ) 
				sen_normal_max_reading[i].Raw =
                                        (SaHpiSensorReadingFormatsT)*vars->val.integer; 
                        else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MAX_RAW:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MAX_INTERPRETED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_max_raw, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) { 
				if(vars->val_len <= SAHPI_SENSOR_BUFFER_LENGTH) 
					memcpy(&sen_normal_max_reading[i].Interpreted.Value.SensorBuffer,
					       vars->val.string,
					       vars->val_len);
                        } else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MAX_INTERPRETED:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MAX_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_max_intrepreted, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) 
				sen_normal_max_reading[i].EventStatus.SensorStatus =
					(SaHpiSensorStatusT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MAX_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MAX_EVENT_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_max_status, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) 
				build_state_value(vars->val.string,
						  vars->val_len,
						  &sen_normal_max_reading[i].EventStatus.EventStatus);
                        else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MAX_EVENT_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SENSOR READINGS NORMAL_MAX  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** Adding sen_normal_max_reading[] to cap[]             	 **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX ) {
                                if (ii >= rr_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered NORMAL_MAX READING RANGE data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.DataFormat.Range.NormalMax = 
					sen_normal_max_reading[ii++];
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_max_reading cache */
        if (sen_normal_max_reading) 
                g_free(sen_normal_max_reading);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_range_normal_min(struct snmp_client_hnd *custom_handle, 
				   SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				   int num_sen_rdrs, 
				   SaHpiUint8T rr_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorReadingT *sen_normal_min_reading = 
		g_malloc0(rr_num * sizeof(*sen_normal_min_reading));
	if (!sen_normal_min_reading){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # of range readings of type normal max, rr_num %d *****" ,rr_num);

        /* SA_HPI_SENSOR_READING_NORMAL_MIN_VALUE_PRESENT */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_reading_normal_min_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_normal_min_reading[i].ValuesPresent =
                                        (SaHpiSensorReadingFormatsT)SNMP_ENUM_ADJUST(*vars->val.integer);
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MIN_VALUE_PRESENT:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MIN_RAW */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_min_values_present, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_UNSIGNED ) 
				sen_normal_min_reading[i].Raw =
                                        (SaHpiSensorReadingFormatsT)*vars->val.integer; 
                        else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MIN_RAW:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MIN_INTERPRETED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_min_raw, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) { 
				if(vars->val_len <= SAHPI_SENSOR_BUFFER_LENGTH) 
					memcpy(&sen_normal_min_reading[i].Interpreted.Value.SensorBuffer,
					       vars->val.string,
					       vars->val_len);
                        } else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MIN_INTERPRETED:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MIN_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_min_intrepreted, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_INTEGER ) 
				sen_normal_min_reading[i].EventStatus.SensorStatus =
					(SaHpiSensorStatusT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MIN_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_NORMAL_MIN_EVENT_STATUS */
	build_res_oid(anOID, 
		      sa_hpi_sensor_reading_normal_min_status, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     rr_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < rr_num; i++) {
                        if ( vars->type == ASN_OCTET_STR ) 
				build_state_value(vars->val.string,
						  vars->val_len,
						  &sen_normal_min_reading[i].EventStatus.EventStatus);
                        else
                                dbg("SA_HPI_SENSOR_READING_NORMAL_MIN_EVENT_STATUS:something terrible has happened");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SENSOR READINGS NORMAL_MIN  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** Adding sen_normal_min_reading[] to cap[]             	 **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN ) {
                                if (ii >= rr_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered NORMAL_MIN READING RANGE data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.DataFormat.Range.NormalMin = 
					sen_normal_min_reading[ii++];
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_normal_min_reading) 
                g_free(sen_normal_min_reading);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_thld_low_crit(struct snmp_client_hnd *custom_handle, 
				   SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				   int num_sen_rdrs, 
				   SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_low_crit, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_LOW_CRITICAL_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_low_critical_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_CRIT : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_CRITICAL_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_LOW_CRITICAL_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_low_critical_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_CRIT : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_CRITICAL_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_LOW_CRITICAL_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_low_critical_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_CRIT : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_CRITICAL_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_LOW_CRITICAL  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** LOW_CRITICAL: AND'ING sen_defn[] to sahpi_sensor_cap[]**");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered SA_HPI_SENSOR_READING_THD_LOW_CRITICAL data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_thld_low_major(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_low_major, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_LOW_MAJOR_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_low_major_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_MAJOR : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_MAJOR_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_LOW_MAJOR_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_low_major_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_MAJOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_MAJOR_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_LOW_MAJOR_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_low_major_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_MAJOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_MAJOR_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_LOW_MAJOR  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** LOW_MAJOR: AND'ING sen_defn[] to sahpi_sensor_cap[]   **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered SA_HPI_SENSOR_READING_THD_LOW_MAJOR data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_thld_low_minor(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_low_minor, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_LOW_MINOR_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_low_minor_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_MINOR : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_MINOR_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_LOW_MINOR_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_low_minor_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_MINOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_MINOR_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_LOW_MINOR_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_low_minor_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_MINOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_LOW_MINOR_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_LOW_MINOR  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** LOW_MINOR: AND'ING sen_defn[] to sahpi_sensor_cap[]   **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered SA_HPI_SENSOR_READING_THD_LOW_MINOR data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);

	sc_free_pdu(&get_cap_response);

	return(status);
}


SaErrorT populate_thld_up_critical(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_up_critical, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_UP_CRITICAL_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_up_critical_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_CRIT : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_CRITICAL_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_UP_CRITICAL_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_up_critical_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_CRIT : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_CRITICAL_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
		sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_UP_CRITICAL_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_up_critical_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_CRIT : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_CRITICAL_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_UP_CRITICAL  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** UP_CRITICAL: AND'ING sen_defn[] to sahpi_sensor_cap[]   **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered SA_HPI_SENSOR_READING_THD_UP_CRITICAL data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_thld_up_major(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_up_major, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_UP_MAJOR_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_up_major_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_MAJOR : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_MAJOR_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_UP_MAJOR_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_up_major_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_MAJOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_MAJOR_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_UP_MAJOR_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_up_major_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_MAJOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_MAJOR_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_UP_MAJOR  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** UP_MAJOR: AND'ING sen_defn[] to sahpi_sensor_cap[]   **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered SA_HPI_SENSOR_READING_THD_UP_MAJOR data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);

	sc_free_pdu(&get_cap_response);

	return(status);
}


SaErrorT populate_thld_up_minor(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_up_minor, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_UP_MINOR_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_up_minor_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_MINOR : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_MINOR_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_UP_MINOR_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_up_minor_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_MINOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_MINOR_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_UP_MINOR_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_up_minor_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_MINOR : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_UP_MINOR_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_UP_MINOR  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** UP_MINOR: AND'ING sen_defn[] to sahpi_sensor_cap[]   **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
                                        dbg("Number of RDRs of type SENSOR exceeds discovered SA_HPI_SENSOR_READING_THD_UP_MINOR data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);

	sc_free_pdu(&get_cap_response);

	return(status);
}

SaErrorT populate_thld_pos_hysteresis(struct snmp_client_hnd *custom_handle, 
				      SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				      int num_sen_rdrs, 
				      SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_pos_hysteresis, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_POS_HYSTERESIS_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_pos_hysteresis_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_HYSTERESIS : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_POS_HYSTERESIS_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_POS_HYSTERESIS_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_pos_hysteresis_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_HYSTERESIS : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_POS_HYSTERESIS_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_POS_HYSTERESIS_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_pos_hysteresis_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_UP_HYSTERESIS : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_POS_HYSTERESIS_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_POS_HYSTERESIS  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** POS_HYSTERESIS: AND'ING sen_defn[] to sahpi_sensor_cap[]   **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
					dbg("num_rdrs exceeds NEG_HYSTERESIS data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);
	
	sc_free_pdu(&get_cap_response);

	return(status);
}


SaErrorT populate_thld_neg_hysteresis(struct snmp_client_hnd *custom_handle, 
				      SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				      int num_sen_rdrs, 
				      SaHpiUint8T thd_num)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;

        oid anOID[MAX_OID_LEN];
        oid *indices;

 	/* allocate memory for temp resources indices storage */
	SaHpiSensorThdDefnT *sen_defn = 
		g_malloc0(thd_num * sizeof(*sen_defn));
	if (!sen_defn){
		dbg("ERROR: in populate_range_max(), allocating sen_normal_min_reading"); 
		return(SA_ERR_HPI_ERROR);
	} 

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in populate_range_max(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

dbg("**** # entrys for populate_thld_neg_hysteresis, thd_num %d *****" ,thd_num);

        /* SA_HPI_SENSOR_READING_THD_NEG_HYSTERESIS_CALLS_READABLE */
        snmp_status = snmp_getn_bulk(custom_handle->ss,
                                     sa_hpi_sensor_thd_neg_hysteresis_entry, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);

        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER ) {
                                
				sen_defn[i].ReadThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_HYSTERESIS : SAHPI_FALSE;
				
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                dbg("SA_HPI_SENSOR_READING_THD_NEG_HYSTERESIS_CALLS_READABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_NEG_HYSTERESIS_CALLS_WRITEABLE */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_neg_hysteresis_calls_readable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].WriteThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_HYSTERESIS : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_NEG_HYSTERESIS_CALLS_WRITEABLE:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_THD_NEG_HYSTERESIS_CALLS_FIXED */
	build_res_oid(anOID, 
		      sa_hpi_sensor_thd_neg_hysteresis_calls_writeable, 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);

        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     thd_num);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                vars = get_cap_response->variables;

                for (i = 0; i < thd_num; i++) {
                        if ( vars->type == ASN_INTEGER )
				sen_defn[i].FixedThold = 
					(*vars->val.integer == 1) ? SAHPI_STM_LOW_HYSTERESIS : SAHPI_FALSE;
                        else
                                dbg("SA_HPI_SENSOR_READING_THD_NEG_HYSTERESIS_CALLS_FIXED:something terrible has happened");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
	sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_READING_THD_NEG_HYSTERESIS  add to SaHpiRdrTypeUnionT *cap*/
	if(status == SA_OK) {
		dbg("***********************************************************");
		dbg("** NEG_HYSTERESIS: AND'ING sen_defn[] to sahpi_sensor_cap[]   **");
		dbg("***********************************************************");
                int ii = 0;
                for( i = 0; i < num_sen_rdrs; i++) {
                        if ( sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold  ) {
                                if (ii >= thd_num) {
					dbg("num_rdrs exceeds NEG_HYSTERESIS data");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.ReadThold |=  
					sen_defn[ii].ReadThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.WriteThold |= 
					sen_defn[ii].WriteThold;
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.FixedThold |= 
					sen_defn[ii].FixedThold;
				ii++;
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary sen_normal_min_reading cache */
        if (sen_defn) 
                g_free(sen_defn);

	sc_free_pdu(&get_cap_response);

	return(status);
}


/*****************************************************************************/
/*****************************************************************************/
/**** 									   ***/
/**** 									   ***/
/**** 	abi support functions 						   ***/
/**** 									   ***/
/**** 									   ***/
/**** 									   ***/
/**** 									   ***/
/**** 									   ***/
/*****************************************************************************/
/*****************************************************************************/
oid **get_thold_table_oids(int index)
{
	static oid *sensor_thold_oid_table_index[NUM_THRESHOLD_TABLES][6] = {
		/* saHpiSensorReadingThdLowCriticalTable */
		{
			sa_hpi_sensor_thd_low_critical_calls_readable,	
			sa_hpi_sensor_thd_low_critical_calls_writeable,  
			sa_hpi_sensor_thd_low_critical_calls_fixed,	
			sa_hpi_sensor_thd_low_critical_valuespresent,    
			sa_hpi_sensor_thd_low_critical_raw,		
			sa_hpi_sensor_thd_low_critical_interpreted
		},
		/* saHpiSensorReadingThdLowMajorlTable */
		{
			sa_hpi_sensor_thd_low_major_calls_readable,	
			sa_hpi_sensor_thd_low_major_calls_writeable,  	
			sa_hpi_sensor_thd_low_major_calls_fixed,		
			sa_hpi_sensor_thd_low_major_valuespresent,    	
			sa_hpi_sensor_thd_low_major_raw,			
			sa_hpi_sensor_thd_low_major_interpreted		
		},
		/* saHpiSensorReadingThdLowMinorlTable */
		{
			sa_hpi_sensor_thd_low_minor_calls_readable,	
			sa_hpi_sensor_thd_low_minor_calls_writeable,  	
			sa_hpi_sensor_thd_low_minor_calls_fixed,		
			sa_hpi_sensor_thd_low_minor_valuespresent,    	
			sa_hpi_sensor_thd_low_minor_raw,			
			sa_hpi_sensor_thd_low_minor_interpreted
		},
		/* saHpiSensorReadingThdUpCriticallTable */
		{
			sa_hpi_sensor_thd_up_critical_calls_readable,	
			sa_hpi_sensor_thd_up_critical_calls_writeable,  	
			sa_hpi_sensor_thd_up_critical_calls_fixed,	
			sa_hpi_sensor_thd_up_critical_valuespresent,    	
			sa_hpi_sensor_thd_up_critical_raw,		
			sa_hpi_sensor_thd_up_critical_interpreted	
		},
		/* saHpiSensorReadingThdUpMajorlTable */
		{
			sa_hpi_sensor_thd_up_major_calls_readable,
			sa_hpi_sensor_thd_up_major_calls_writeable,  	
			sa_hpi_sensor_thd_up_major_calls_fixed,		
			sa_hpi_sensor_thd_up_major_valuespresent,    	
			sa_hpi_sensor_thd_up_major_raw,			
			sa_hpi_sensor_thd_up_major_interpreted
		},
		/* saHpiSensorReadingThdUpMinorlTable */
		{
			sa_hpi_sensor_thd_up_minor_calls_readable,	
			sa_hpi_sensor_thd_up_minor_calls_writeable,  	
			sa_hpi_sensor_thd_up_minor_calls_fixed,		
			sa_hpi_sensor_thd_up_minor_valuespresent,    	
			sa_hpi_sensor_thd_up_minor_raw,			
			sa_hpi_sensor_thd_up_minor_interpreted		
		},
		/* saHpiSensorReadingThdPosHysteresislTable */
		{
			sa_hpi_sensor_thd_pos_hysteresis_calls_readable,	
			sa_hpi_sensor_thd_pos_hysteresis_calls_writeable,
			sa_hpi_sensor_thd_pos_hysteresis_calls_fixed,	
			sa_hpi_sensor_thd_pos_hysteresis_valuespresent,  
			sa_hpi_sensor_thd_pos_hysteresis_raw,		
			sa_hpi_sensor_thd_pos_hysteresis_interpreted	
		},
		/* saHpiSensorReadingThdNegHysteresislTable */
		{
			sa_hpi_sensor_thd_neg_hysteresis_calls_readable,	
			sa_hpi_sensor_thd_neg_hysteresis_calls_writeable,
			sa_hpi_sensor_thd_neg_hysteresis_calls_fixed,	
			sa_hpi_sensor_thd_neg_hysteresis_valuespresent,  
			sa_hpi_sensor_thd_neg_hysteresis_raw,		
			sa_hpi_sensor_thd_neg_hysteresis_interpreted
		}
	
	};

	return(sensor_thold_oid_table_index[index]);
}


SaErrorT get_sensor_threshold_data(struct snmp_client_hnd *custom_handle,
			  oid **oid_ptr, 
			  oid *indices, 
			  SaHpiSensorReadingT *reading)
{
	SaErrorT status = SA_OK;
	struct snmp_value get_value;
	int is_readable = 0;
        
	oid anOID[MAX_OID_LEN];

	/* THOLD_IS_READABLE */
	build_res_oid(anOID, 
		      oid_ptr[THOLD_IS_READABLE], 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);	
	status  = snmp_get2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &get_value);

	if( (status == SA_OK) && (get_value.type == ASN_INTEGER) )
		is_readable = (get_value.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
	else
		dbg("get_sensor_threshold_data: error getting THOLD_IS_READABLE ");

	/* if the threshold MIB OIDS are not readable return */
	if (is_readable == SAHPI_FALSE) {
		dbg("RAW and INTERPRETED MIB vars not readable");
		return status;
	}

	/* VALUES_PRESENT */
	build_res_oid(anOID, 
		      oid_ptr[THOLD_VALUES_PRESENT], 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);	
	status  = snmp_get2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &get_value);

	if( (status == SA_OK) && (get_value.type == ASN_INTEGER) )
		reading->ValuesPresent = 
			(SaHpiSensorReadingFormatsT)SNMP_ENUM_ADJUST(get_value.integer);
	else
		dbg("get_sensor_threshold_data: error getting VALUES_PRESENT "); 

	/* RAW_READING */
	if ((reading->ValuesPresent & SAHPI_SRF_RAW) && (status == SA_OK) ) {
		
		build_res_oid(anOID, 
			      oid_ptr[THOLD_RAW], 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status  = snmp_get2(custom_handle->ss,
				    anOID, 
				    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				    &get_value);

		if( (status == SA_OK) && (get_value.type == ASN_UNSIGNED) ) {
			reading->Raw = (SaHpiUint32T)get_value.integer;
		} else {
			dbg("get_sensor_threshold_data: error getting");
			dbg(" RAW_READING ");
		}

	} 

	/* INTERPRETED_READING */
	if( (reading->ValuesPresent & SAHPI_SRF_INTERPRETED) && (status == SA_OK) ) {
		build_res_oid(anOID, 
			      oid_ptr[THOLD_INTREPRETED], 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status  = snmp_get2(custom_handle->ss,
				    anOID, 
				    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				    &get_value);

		if( (status == SA_OK) && (get_value.type == ASN_OCTET_STR) ) {
			if (get_value.str_len <= SAHPI_SENSOR_BUFFER_LENGTH) {
				memcpy(&reading->Interpreted.Value, 
				       get_value.string, 
				       get_value.str_len);
				reading->Interpreted.Type = SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER;
			} else
				dbg("ERROR: get_sensor_threshold_data() buff exceeds max allowed");

		} else 
			dbg("get_sensor_threshold_data: INTERPRETED_READING");
	} 

	return(status);

}

SaErrorT set_sensor_threshold_data(struct snmp_client_hnd *custom_handle,
				   oid **oid_ptr, 
				   oid *indices, 
				   const SaHpiSensorReadingT *writing)
{
	SaErrorT status = SA_OK;
	struct snmp_value get_value,
			  set_value;
	int is_writable = SAHPI_FALSE;
        
	oid anOID[MAX_OID_LEN];

	/* THOLD_WRITEABLE */
	build_res_oid(anOID, 
		      oid_ptr[THOLD_WRITEABLE], 
		      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
		      indices, 
		      NUM_SEN_INDICES);	
	status  = snmp_get2(custom_handle->ss,
			    anOID, 
			    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
			    &get_value);

	if( (status == SA_OK) && (get_value.type == ASN_INTEGER) )
		is_writable = (get_value.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
	else
		dbg("get_sensor_threshold_data: error getting THOLD_WRITEABLE ");

	/* if the threshold MIB OIDS are not writable return */
	if (is_writable == SAHPI_FALSE) {
		dbg("RAW and INTERPRETED MIB vars not writable");
		return status;
	}

	/* RAW_READING */
	if ( (writing->ValuesPresent & SAHPI_SRF_RAW) && 
	     (status == SA_OK)) {

		memset(&set_value, 0, sizeof(set_value));
		set_value.integer = (SaHpiUint32T)writing->Raw;
		set_value.type = ASN_UNSIGNED;
		set_value.str_len = sizeof(unsigned int);
		
		build_res_oid(anOID, 
			      oid_ptr[THOLD_RAW], 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);

		status  = snmp_set2(custom_handle->ss,
				    anOID, 
				    SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				    &set_value);
	} 

	/* INTERPRETED_READING */
	if( (writing->ValuesPresent & SAHPI_SRF_INTERPRETED) && 
	    (status == SA_OK) ) {

		memset(&set_value, 0, sizeof(set_value));
		memcpy(&set_value.string, 
		       &writing->Interpreted.Value.SensorBuffer, 
		       SAHPI_SENSOR_BUFFER_LENGTH);
		set_value.str_len = SAHPI_SENSOR_BUFFER_LENGTH;
		set_value.type = ASN_OCTET_STR;

		build_res_oid(anOID, 
			      oid_ptr[THOLD_INTREPRETED], 
			      SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
			      indices, 
			      NUM_SEN_INDICES);
	
		status  = snmp_set2(custom_handle->ss,
				   anOID, 
				   SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH,
				   &set_value);
	} 

	return(status);

}











