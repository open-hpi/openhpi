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
 *
 */
 

#include <stdio.h>
#include <stdlib.h>
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
#include <sc_sensor_data.h>

#include <netinet/in.h>

struct {
	SaHpiUint8T max;
	SaHpiUint8T min;
	SaHpiUint8T nominal;
	SaHpiUint8T normal_max;
	SaHpiUint8T normal_min;
} range_flags_count;

static void count_range_flags(SaHpiRdrTypeUnionT *sen_cap, int num_sen_rdrs);

static SaErrorT get_ctrl_capabilities(struct snmp_client_hnd *custom_handle, 
                                      SaHpiRdrT *rdr_cache, 
                                      int *rdr_type_count,
                                      int num_rdrs);

static SaErrorT get_sensor_capabilities(struct snmp_client_hnd *custom_handle, 
                                        SaHpiRdrT *rdr_cache, 
                                        int *rdr_type_count,
                                        int num_rdrs);

static SaErrorT get_inventory_capabilities(struct snmp_client_hnd *custom_handle, 
                                      SaHpiRdrT *rdr_cache, 
                                      int *rdr_type_count,
                                      int num_rdrs);

static SaErrorT get_watchdog_capabilities(struct snmp_client_hnd *custom_handle, 
                                     SaHpiRdrT *rdr_cache, 
                                     int *rdr_type_count,
                                     int num_rdrs);

int get_sahpi_rdr_table( RPTable *temp_rptable, 
                         struct oh_handler_state *handle,  
                         int num_entries )
{
        struct snmp_pdu *pdu = NULL;
        struct snmp_pdu *response = NULL;
        SaErrorT status = SA_OK;
        int snmp_status;
        struct variable_list *vars;
        char *ep;

        int i;
        int id = 0;


        SaHpiRdrT *rdr_cache = NULL;

        struct rdr_data *remote_rdr_data = NULL;
        
        int rdr_type_count[NUM_RDR_TYPES];

        oid anOID[MAX_OID_LEN];
        oid *indices;
 
        struct snmp_client_hnd 
                *custom_handle = (struct snmp_client_hnd *)handle->data;
                    
        /* memory for temporary rdr_cache */
        rdr_cache = g_malloc0(num_entries * sizeof(*rdr_cache));
        if (!rdr_cache) {
                dbg("ERROR: in get_sahpi_rdr_table(), allocating rdr_cache");
                return(SA_ERR_HPI_ERROR);
        }

        /* memory for remote rdr data */
        remote_rdr_data = g_malloc0(num_entries * sizeof(*remote_rdr_data));
        if (!remote_rdr_data) {
                dbg("ERROR: in get_sahpi_rdr_table(), allocating remote_rdr_data"); 
                return(SA_ERR_HPI_ERROR);
        }

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_RDR_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in get_sahpi_rdr_table(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

        /* SA_HPI_RDR_RECORD_ID */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                             sa_hpi_rdr_entry, 
                             SA_HPI_RDR_ENTRY_OID_LEN, 
                             pdu, 
                             &response, 
                             num_entries);
        /* Process the response */
        if (status == SA_OK && snmp_status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        /* SUCCESS: Print the result variables */  

                /* Get the data from the response */
                vars = response->variables;
                
                for (i = 0; i < num_entries; i++) {
                        if (vars->type == ASN_UNSIGNED) {

                                /* this is the rdr recordId for the remote rdr, the local */
                                /* value will overwrite this value when oh_rdr_add() is called */
                                rdr_cache[i].RecordId = (SaHpiEntryIdT)*vars->val.integer;

                                /* save remote rdr info */
                                remote_rdr_data[i].index.remote_domain = 
                                        (SaHpiDomainIdT)vars->name[vars->name_length - 4];
                                remote_rdr_data[i].index.remote_resource_id = 
                                        (SaHpiResourceIdT)vars->name[vars->name_length - 3];
                                remote_rdr_data[i].index.remote_record_id = 
                                        (SaHpiEntryIdT)vars->name[vars->name_length - 2];
                                remote_rdr_data[i].index.remote_rdr_type = 
                                       (SaHpiRdrTypeT)vars->name[vars->name_length - 1];
                                
                                /* save the index values for this resource */
                                /* really just want the last index of the last resource */
                                /* INDEX   { saHpiDomainID, saHpiResourceID, saHpiRdrRecordId, saHpiRdrType } */
                                indices[0] = vars->name[vars->name_length - 4];
                                indices[1] = vars->name[vars->name_length - 3];
                                indices[2] = vars->name[vars->name_length - 2];
                                indices[3] = vars->name[vars->name_length - 1];  
                        } else
                                printf("SA_HPI_RDR_RECORD_ID:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, response);
        sc_free_pdu(&response);


        /* SA_HPI_RDR_TYPE */
        build_res_oid(anOID, 
                      sa_hpi_rdr_record_id, 
                      SA_HPI_RDR_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_RES_INDICES); 
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                             anOID, 
                             SA_HPI_RDR_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, 
                             pdu, 
                             &response, 
                             num_entries);
        /* Process the response */
        if (status == SA_OK && snmp_status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        /* SUCCESS: Print the result variables */  

                /* Get the data from the response */
                vars = response->variables;

                for (i = 0; i < num_entries; i++) {
                        if (vars->type == ASN_INTEGER)
                                rdr_cache[i].RdrType = 
					SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_RDR_TYPE:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, response);
        sc_free_pdu(&response);

        /* SA_HPI_RDR_ENTITY_PATH */
        build_res_oid(anOID, 
                      sa_hpi_rdr_type, 
                      SA_HPI_RDR_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_RES_INDICES); 
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                             anOID, 
                             SA_HPI_RDR_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, 
                             pdu, 
                             &response, 
                             num_entries);
        /* Process the response */
        if (status == SA_OK && snmp_status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        /* SUCCESS: Print the result variables */  

                /* Get the data from the response */
                vars = response->variables;

                for (i = 0; i < num_entries; i++) {
                        if (vars->type == ASN_OCTET_STR)  {
                                ep = g_malloc0(vars->val_len + 1);
                                memcpy(ep, vars->val.string, vars->val_len);
                                if(string2entitypath(ep, &rdr_cache[i].Entity))
                                        dbg("something terrible happened with SA_HPI_RDR_ENTITY_PATH");
                                free(ep);
                        }
                        else
                                printf("SA_HPI_RDR_ENTITY_PATH:something terrible has happened\n");
                        vars = vars->next_variable;             
                }

                display_vars(response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, response);
        sc_free_pdu(&response);

       /* count the number of different type rdrs, see SaHpiRdrTypeT */
       memset(rdr_type_count, 0, NUM_RDR_TYPES * sizeof(int));
       for (i=0; i < num_entries; i++) {
               switch (rdr_cache[i].RdrType) {
               case SAHPI_NO_RECORD:
                       rdr_type_count[SAHPI_NO_RECORD]++;
                       break;
               case SAHPI_CTRL_RDR:
                       rdr_type_count[SAHPI_CTRL_RDR]++;
                       break;
               case SAHPI_SENSOR_RDR:
                       rdr_type_count[SAHPI_SENSOR_RDR]++;
                       break;
               case SAHPI_INVENTORY_RDR:
                       rdr_type_count[SAHPI_INVENTORY_RDR]++;
                       break;
               case SAHPI_WATCHDOG_RDR:
                       rdr_type_count[SAHPI_WATCHDOG_RDR]++;
                       break;
               }
       }
       
        status = get_ctrl_capabilities(custom_handle, rdr_cache, rdr_type_count, num_entries);
                
        status = get_sensor_capabilities(custom_handle, rdr_cache, rdr_type_count, num_entries);
                
        status = get_inventory_capabilities(custom_handle, rdr_cache, rdr_type_count, num_entries);
                
        status = get_watchdog_capabilities(custom_handle, rdr_cache, rdr_type_count, num_entries);

        if(status == SA_OK) {
                /* add the rdrs to the plugin's primary rptcache, */
                /* the one maintained in oh_handler_state pointer */
                printf("************************************************************\n");
                printf("** Adding rdr_cache[] and remote_rdr_data[] to temp_table **\n");
                printf("************************************************************\n");
                /* the RDR's to the plugins rptcache */
                for ( i = 0; i < num_entries; i++) {
                        id = oh_uid_lookup(&rdr_cache[i].Entity);
                        if( id < 0 ) { 
                                dbg("error looking up uid in get_sahpi_rdr_table");  
                                status = SA_ERR_HPI_ERROR;
                                break;
                        }
                        if ( oh_add_rdr(temp_rptable, id, &rdr_cache[i], &remote_rdr_data[i], 1) )
                                dbg("oh_add_resource failed for rdr %d, in get_sahpi_rdr_table", i);
                }

        } else {
                /*free the temporary rptcahce buffer used to capture the */
                /* snmp remote resource data */
                printf("Error: freeing remote_rdr_data[] \n");
                if (remote_rdr_data) 
                        g_free(remote_rdr_data);
        }

        /* free the temporary indices */
        if(indices)             
                g_free(indices);

        /* free the temporary rdr_cache */
        if(rdr_cache)           
                g_free(rdr_cache);

        return(status);

}

static SaErrorT get_ctrl_capabilities(struct snmp_client_hnd *custom_handle, 
                                      SaHpiRdrT *rdr_cache, 
                                      int *rdr_type_count, 
                                      int num_rdrs)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        SaErrorT status = SA_OK;
        int snmp_status;
        int i = 0;
        struct variable_list *vars;
        struct snmp_value get_value;

        SaHpiRdrTypeUnionT *sahpi_ctr_cap = NULL;

        oid anOID[MAX_OID_LEN];
        oid *indices;

        /* make sure present ctrl cap count marches discovered */
        status = snmp_get2(custom_handle->ss,
                           sa_hpi_ctrl_count,
                           SA_HPI_CTRL_COUNT_OID_LEN, 
                           &get_value);
        if (get_value.integer != rdr_type_count[SAHPI_CTRL_RDR]) {
                dbg("Sever Error: SA_HPI_CTRL_COUNT value doesn't match Resource Capability Settings");
                return(SA_ERR_HPI_ERROR);
        }

        /* temp ctrl cap table, begin filling in table */
        sahpi_ctr_cap = g_malloc0(get_value.integer * sizeof(*sahpi_ctr_cap));
        if(!sahpi_ctr_cap && get_value.integer) {
                dbg("ERROR: allocating sahpi_ctr_cap");
                return(SA_ERR_HPI_ERROR);
        }

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_CTRL_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in get_sahpi_rdr_table(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }
        
        /* SA_HPI_CTRL_NUM */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     sa_hpi_ctrl_entry, 
                                     SA_HPI_CTRL_ENTRY_OID_LEN, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_UNSIGNED) {
                                sahpi_ctr_cap[i].CtrlRec.Num = (SaHpiCtrlNumT)*vars->val.integer;
                                /* save the indices here */
                                /* INDEX { saHpiDomainID, saHpiResourceID, saHpiCtrlNum } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1];    
                        } else
                                printf("SA_HPI_CNUM_CTRL_INDICESTRL_NUM:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_CTRL_OUTPUT_TYPE */
        build_res_oid(anOID, 
                      sa_hpi_ctrl_num, 
                      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_CTRL_INDICES); 
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_ctr_cap[i].CtrlRec.OutputType = 
                                        (SaHpiCtrlOutputTypeT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_CTRL_OUTPUT_TYPE:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_CTRL_IGNORE */
        build_res_oid(anOID, 
                      sa_hpi_ctrl_output_type, 
                      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_CTRL_INDICES); 
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_ctr_cap[i].CtrlRec.Ignore = 
                                        (*vars->val.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
                        else
                                printf("SA_HPI_CTRL_IGNORE:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_CTRL_TYPE */
        build_res_oid(anOID, 
                      sa_hpi_ctrl_ignore, 
                      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_CTRL_INDICES); 
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_ctr_cap[i].CtrlRec.Type = 
					(SaHpiCtrlTypeT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_CTRL_TYPE:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_CTRL_STATE */
        
        /* SA_HPI_CTRL_ATTRIBUTES */
        build_res_oid(anOID, 
                      sa_hpi_ctrl_state, 
                      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_CTRL_INDICES); 
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) { 
                        if (vars->type == ASN_OCTET_STR) {

                                /* local variables used by case statements */
                                int *data;
                                char *repeat;
                                int *stream_data;
                                char *text_info;
                                char *oem_data;

                                switch (sahpi_ctr_cap[i].CtrlRec.Type) {
                                case SAHPI_CTRL_TYPE_DIGITAL:
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Digital.Default, 
                                               vars->val.string,
                                               sizeof(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Digital.Default));
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Digital.Default = 
                                                ntohs(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Digital.Default);
                                        break;
                                case SAHPI_CTRL_TYPE_DISCRETE:
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Discrete.Default, 
                                               vars->val.string,
                                               sizeof(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Discrete.Default));
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Discrete.Default =
                                                ntohl(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Discrete.Default);
                                        break;
                                case SAHPI_CTRL_TYPE_ANALOG:
                                        data = (int *)vars->val.string;
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Min = *data;          
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Min =
                                                ntohl(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Min);
                                        data++;
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Max = *data;
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Max =
                                                ntohl(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Max);
                                        data++;
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Default = *data;
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Default = 
                                                ntohl(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Analog.Default);
                                        break;
                                case SAHPI_CTRL_TYPE_STREAM:
                                        /* set repeat */
                                        repeat = vars->val.string;
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Stream.Default.Repeat, 
                                               repeat, 
                                               sizeof(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Stream.Default.Repeat));
                                        repeat++;
                                        stream_data = (int *)repeat;
                                        /* set .StreamLength */
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Stream.Default.StreamLength,
                                               stream_data, 
                                               sizeof(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Stream.Default.StreamLength));
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Stream.Default.StreamLength = 
                                                ntohs(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Stream.Default.StreamLength);
                                        stream_data++;
                                        /* set the .Stream data */
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Stream.Default.Stream, 
                                               stream_data, 
                                               SAHPI_CTRL_MAX_STREAM_LENGTH); 
                                              break;
                                case SAHPI_CTRL_TYPE_TEXT:
                                        text_info = vars->val.string;
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.MaxChars = *text_info;
                                        text_info++;

                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.MaxLines = *text_info;
                                        text_info++;

                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Language = *(SaHpiLanguageT *)text_info;
					sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Language =
						(SaHpiLanguageT)SNMP_ENUM_ADJUST(ntohl(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Language));
                                        text_info++;

                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.DataType = *(SaHpiTextTypeT *)text_info;
					sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.DataType = 
						(SaHpiTextTypeT)SNMP_ENUM_ADJUST(ntohl(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.DataType));
                                        text_info++;

                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Default.Line = *text_info,
                                        text_info++;

                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Default.Text.DataLength =
                                                *text_info;

                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Default.Text.DataType =
                                                sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.DataType;

                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Default.Text.Language =
                                                sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Language;
                                        text_info++;

                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Default.Text.Data,
                                               text_info, 
                                               sahpi_ctr_cap[i].CtrlRec.TypeUnion.Text.Default.Text.DataLength);
                                        break;
                                case SAHPI_CTRL_TYPE_OEM:
                                        /* get Mid */
                                        oem_data = vars->val.string;
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.MId, 
                                        vars->val.string, 
                                        sizeof(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.MId));
                                        oem_data = oem_data + 
                                                sizeof(sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.MId);
                                        /* get ConfigData */
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.ConfigData, 
                                               oem_data, 
                                               SAHPI_CTRL_OEM_CONFIG_LENGTH);
                                        oem_data = oem_data + SAHPI_CTRL_OEM_CONFIG_LENGTH;
                                        /* get BodyLength */
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.Default.BodyLength = 
                                                *oem_data;
                                        oem_data++;
                                        /* set MId */
                                        sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.Default.MId = 
                                                sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.MId;
                                        /* get Body/Text data */
                                        memcpy(&sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.Default.Body,
                                               oem_data,
                                               sahpi_ctr_cap[i].CtrlRec.TypeUnion.Oem.Default.BodyLength);                                           
                                        break;
                                default:
                                        dbg("ERROR: unknown CTRL TYPE");
                                        break;
                                }
                        } else
                                printf("SA_HPI_CTRL_STATE:something terrible has happened\n");

                        vars = vars->next_variable;             
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_CTRL_OEM */
        build_res_oid(anOID, 
                      sa_hpi_ctrl_attr, 
                      SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH, 
                      indices, 
                      NUM_CTRL_INDICES); 
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     anOID, 
                                     SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
                /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_UNSIGNED)
                                sahpi_ctr_cap[i].CtrlRec.Oem = (SaHpiUint32T)*vars->val.integer;
                        else
                                printf("SA_HPI_CTRL_OEM:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_CTRL_TEXT_TYPE */
        /* SA_HPI_CTRL_TEXT_LANGUAGE */
        /* SA_HPI_CTRL_TEXT */
        /* SA_HPI_CTRL_RDR */

        /* CTRL CAPABILITIES ADD TO rdr_cache */
        if(status == SA_OK) {
                printf("***********************************************************\n");
                printf("** Adding sahpi_ctr_cap[] to rdr_cache[]                 **\n");
                printf("***********************************************************\n");
                int ii = 0;
                for( i = 0; i < num_rdrs; i++) {
                        if (rdr_cache[i].RdrType == SAHPI_CTRL_RDR ) {
                                rdr_cache[i].RdrTypeUnion.CtrlRec = sahpi_ctr_cap[ii].CtrlRec;
                                ii++;
                                if (ii > get_value.integer) {
                                        dbg("Number of RDRs of type CTRL exceeds discovered CTRL caps");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                        }
                }
        }

        /* free the temporary indices */
        if(indices)             
                g_free(indices);

        /* free the temporary rdr ctrl cap cache */
        if (sahpi_ctr_cap) 
                g_free(sahpi_ctr_cap);
                
        return(status);
}


#define sen_row_get(row_oid) \
do{ \
	build_res_oid(anOID, \
			row_oid, \
			SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH, \
			indices, \
			NUM_SEN_INDICES); \
	snmp_status = snmp_getn_bulk(custom_handle->ss, \
					anOID, \
					SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, \
					get_cap_pdu, \
					&get_cap_response, \
					get_value.integer); \
} while(0)


static SaErrorT get_sensor_capabilities(struct snmp_client_hnd *custom_handle, 
                                        SaHpiRdrT *rdr_cache, 
                                        int *rdr_type_count, 
                                        int num_rdrs)
{

        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
	int snmp_status;
        int i = 0;
        struct variable_list *vars;
        struct snmp_value get_value;

        SaHpiRdrTypeUnionT *sahpi_sensor_cap = NULL;

        oid anOID[MAX_OID_LEN];
        oid *indices;

	/* need to read the Thold MIB tables */
	int num_sen_rdrs_has_thold;
        
        /* make sure present ctrl cap count marches discovered */
        status = snmp_get2(custom_handle->ss,
                           sa_hpi_sen_count,
                           SA_HPI_SEN_COUNT_OID_LEN, 
                           &get_value); 
        if (get_value.integer != rdr_type_count[SAHPI_SENSOR_RDR]) {
                dbg("Sever Error: SA_HPI_SENSOR_COUNT value doesn't match Resource Capability Settings");
                return(SA_ERR_HPI_ERROR);
        }

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_SEN_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in get_sahpi_rdr_table(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }

        /* get SAHPI_SENSOR_RDR data */
        /* SA_HPI_SENSOR_INDEX */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     sa_hpi_sen_entry, 
                                     SA_HPI_SEN_ENTRY_OID_LEN, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                /* begin filling in table */
                sahpi_sensor_cap = g_malloc0(get_value.integer * sizeof(*sahpi_sensor_cap));

                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_UNSIGNED) {
                                sahpi_sensor_cap[i].SensorRec.Num = 
                                        (SaHpiSensorNumT)*vars->val.integer;
				/* INDEX: { saHpiDomainID, saHpiResourceID, saHpiSensorIndex } */
                                indices[0] = vars->name[vars->name_length - 3];
                                indices[1] = vars->name[vars->name_length - 2];
                                indices[2] = vars->name[vars->name_length - 1]; 
                        } else
                                printf("SA_HPI_SENSOR_INDEX:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_TYPE */
	sen_row_get(sa_hpi_sen_index);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER) 
                                sahpi_sensor_cap[i].SensorRec.Type = 
                                        (SaHpiSensorTypeT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_TYPE:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_CATEGORY */
	sen_row_get(sa_hpi_sen_type);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.Category = 
                                        (SaHpiEventCategoryT)(SNMP_ENUM_ADJUST(*vars->val.integer));
                        else
                                printf("SA_HPI_SENSOR_CATEGORY:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_EVENTS_CATEGORY_CONTROL */
	sen_row_get(sa_hpi_sen_category);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.EventCtrl = 
                                        (SaHpiSensorEventCtrlT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_EVENTS_CATEGORY_CONTROL:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_EVENT_STATE */
	sen_row_get(sa_hpi_sen_evts_cat_ctrl);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;
		/* temp storage variable for build_state_value() */ 

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_OCTET_STR) 
				build_state_value (vars->val.string,
						   vars->val_len,
						   &sahpi_sensor_cap[i].SensorRec.Events);
                        else
                                printf("SA_HPI_SENSOR_EVENT_STATE:something terrible has happened\n");
                        vars = vars->next_variable;
                
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_STATUS */
        /* SA_HPI_SENSOR_ASSERT_EVENTS*/
        /* SA_HPI_SENSOR_DEASSERT_EVENTS */

        /* SA_HPI_SENSOR_IGNORE */
	sen_row_get(sa_hpi_sen_deassert_evts);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER) 
                                sahpi_sensor_cap[i].SensorRec.Ignore = 
                                        (*vars->val.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
                        else
                                printf("SA_HPI_SENSOR_IGNORE:something terrible has happened\n");
                        vars = vars->next_variable;
                
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_READING_FORMATS */
	sen_row_get(sa_hpi_sen_ignore);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_UNSIGNED)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.ReadingFormats = 
                                        (SaHpiSensorReadingFormatsT)*vars->val.integer;
                        else
                                printf("SA_HPI_SENSOR_READING_FORMATS:something terrible has happened\n");
                        vars = vars->next_variable;
                
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_IS_NUMERIC */
	sen_row_get(sa_hpi_sen_reading_formats);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.IsNumeric = 
                                        (*vars->val.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
                        else
                                printf("SA_HPI_SENSOR_IS_NUMERIC:something terrible has happened\n");
                        vars = vars->next_variable;
                
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_SIGN_FORMAT */
	sen_row_get(sa_hpi_sen_is_numeric);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.SignFormat = 
                                        (SaHpiSensorSignFormatT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_SIGN_FORMAT:something terrible has happened\n");
                        vars = vars->next_variable;
                
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_BASE_UNITS */
	sen_row_get(sa_hpi_sen_sign_foramt);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.BaseUnits = 
                                        (SaHpiSensorUnitsT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_BASE_UNITS:something terrible has happened\n");
                        vars = vars->next_variable;
                
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_MODIFIER_UNITS */
	sen_row_get(sa_hpi_sen_base_units);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.ModifierUnits = 
                                        (SaHpiSensorUnitsT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_MODIFIER_UNITS:something terrible has happened\n");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_MODIFIER_USE */
	sen_row_get(sa_hpi_sen_modifier_units);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.ModifierUse = 
                                        (SaHpiSensorModUnitUseT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_MODIFIER_USE:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

         /* SA_HPI_SENSOR_FACTORS_STATIC */
	sen_row_get(sa_hpi_sen_modifier_use);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.FactorsStatic = 
                                        (*vars->val.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
                        else
                                printf("SA_HPI_SENSOR_FACTORS_STATIC:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_FACTORS */
	sen_row_get(sa_hpi_sen_factors_static);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                char *factors;
                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_OCTET_STR) {
                                if(sahpi_sensor_cap[i].SensorRec.DataFormat.FactorsStatic == SAHPI_TRUE) {
                                        factors = vars->val.string;
                                        /* set M_Factor */
                                        memcpy(&sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.M_Factor,
                                               factors,
                                               sizeof(SaHpiInt16T));
                                        sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.M_Factor =
                                                ntohs(sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.M_Factor);
                                        factors = factors + sizeof(SaHpiInt16T);
                                        /* set B_Factor */
                                        memcpy(&sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.B_Factor,
                                               factors,
                                               sizeof(sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.B_Factor));
                                        sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.B_Factor =
                                                ntohs(sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.B_Factor);
                                        factors = factors + sizeof(SaHpiInt16T);
                                        /* set Accuracy_Factor */
                                        memcpy(&sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.AccuracyFactor,
                                               factors,
                                               sizeof(SaHpiUint16T));
                                        sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.AccuracyFactor = 
                                                ntohs(sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.AccuracyFactor);
                                        factors = factors + sizeof(SaHpiUint16T);   
                                        /*set Tolerance */
                                        sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.ToleranceFactor =
                                                *factors;
					factors = factors + sizeof(SaHpiUint8T); 
                                        /* set Accuracy Exp*/
                                        sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.ExpA = 
                                                *factors;
					factors = factors + sizeof(SaHpiUint8T);
                                        /* set Result Exp */
                                        sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.ExpR = 
                                                *factors;
					factors = factors + sizeof(SaHpiInt8T);
                                        /* set B Exp */
                                        sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.ExpB =
                                                *factors;
					factors = factors + sizeof(SaHpiInt8T);
                                }     
                        } else
                                printf("SA_HPI_SENSOR_FACTORS:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_FACTORS_LINEARIZATION */
	sen_row_get(sa_hpi_sen_factors);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.Factors.Linearization = 
                                        (SaHpiSensorLinearizationT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_FACTORS_LINEARIZATION:something terrible has happened\n");
                        vars = vars->next_variable;
                
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_PERCENTAGE */
	sen_row_get(sa_hpi_sen_factors_linearization);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER)
                                sahpi_sensor_cap[i].SensorRec.DataFormat.Percentage = 
                                        (*vars->val.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
                        else
                                printf("SA_HPI_SENSOR_PERCENTAGE:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_RANGE_FLAGS */
	sen_row_get(sa_hpi_sen_percentage);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

		/* clear the Ranges Flags Count Structure */
                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_OCTET_STR) {
				build_flag_value(vars->val.string, 
						 vars->val_len,
						 &sahpi_sensor_cap[i].SensorRec.DataFormat.Range.Flags);
                        } else
                                printf("SA_HPI_SENSOR_RANGE_FLAGS:something terrible has happened\n");
                        vars = vars->next_variable;                
                }
		/* COUNT THE DIFFERENT OCCURANCES of TYPES OF RANGE READINGS */
		count_range_flags(sahpi_sensor_cap, get_value.integer);
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_HAS_THRESHOLD */
	num_sen_rdrs_has_thold = 0;
	sen_row_get(sa_hpi_sen_range_flags);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER) {
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold = 
                                        (*vars->val.integer == 1) ? SAHPI_TRUE : SAHPI_FALSE;
				if(sahpi_sensor_cap[i].SensorRec.ThresholdDefn.IsThreshold) 
					num_sen_rdrs_has_thold++;
                        } else
                                printf("SA_HPI_SENSOR_HAS_THRESHOLD:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

	/* SA_HPI_SENSOR_THOLD_CAPABILITIES */
	sen_row_get(sa_hpi_sen_has_thresholds);
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_INTEGER) 
                                sahpi_sensor_cap[i].SensorRec.ThresholdDefn.TholdCapabilities = 
                                        (SaHpiSensorThdCapT)SNMP_ENUM_ADJUST(*vars->val.integer);
                        else
                                printf("SA_HPI_SENSOR_THOLD_CAPABILITIES:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);
	
	/* SA_HPI_SENSOR_OEM */  
	sen_row_get(sa_hpi_sen_thd_cap);
	if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
		/* Get the data from the response */
		vars = get_cap_response->variables;
                int Oem;
                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_UNSIGNED) {
                                Oem = *vars->val.string;
                                sahpi_sensor_cap[i].SensorRec.Oem = 
                                        (SaHpiUint32T)ntohl(Oem);
                        } else
                                printf("SA_HPI_SENSOR_OEM:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);

        /* SA_HPI_SENSOR_RANGE_MAX_TABLE */
	if(range_flags_count.max)
		populate_range_max(custom_handle,
				   sahpi_sensor_cap, 
				   get_value.integer,
				   range_flags_count.max); 

        /* SA_HPI_SENSOR_RANGE_MIN_TABLE */
	if(range_flags_count.min)
		populate_range_min(custom_handle,
				   sahpi_sensor_cap, 
				   get_value.integer,
				   range_flags_count.min); 

        /* SA_HPI_SENSOR_RANGE_NOMINAL_TABLE */
	if(range_flags_count.nominal)
		populate_range_nominal(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       range_flags_count.nominal); 


	/* SA_HPI_SENSOR_RANGE_NORMAL_MAX_TABLE */
	if(range_flags_count.normal_max)
		populate_range_normal_max(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       range_flags_count.normal_max);

        
	/* SA_HPI_SENSOR_RANGE_NORMAL_MIN_TABLE */
	if(range_flags_count.normal_min)
		populate_range_normal_min(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       range_flags_count.normal_min);


	/* READ THE THOLD TABLES */
	if(num_sen_rdrs_has_thold) {

		dbg("num_sen_rdrs %d\n", (int)get_value.integer);
		dbg("num_sen_rdrs_has_thold %d\n", num_sen_rdrs_has_thold);

		populate_thld_low_crit(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);

		populate_thld_low_major(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);

		populate_thld_low_minor(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);

		populate_thld_up_critical(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);

		populate_thld_up_major(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);

		populate_thld_up_minor(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);

		populate_thld_pos_hysteresis(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);

		populate_thld_neg_hysteresis(custom_handle,
				       sahpi_sensor_cap, 
				       get_value.integer,
				       num_sen_rdrs_has_thold);
	}

	/* SENSOR CAPABILITIES ADD TO rdr_cache */
	if(status == SA_OK) {
		printf("***********************************************************\n");
		printf("** Adding sahpi_sensor_cap[] to rdr_cache[]              **\n");
		printf("***********************************************************\n");
                int ii = 0;
                for( i = 0; i < num_rdrs; i++) {
                        if (rdr_cache[i].RdrType == SAHPI_SENSOR_RDR ) {
                                rdr_cache[i].RdrTypeUnion.SensorRec = sahpi_sensor_cap[ii].SensorRec;
                                ii++;
                                if (ii > get_value.integer) {
                                        dbg("Number of RDRs of type CTRL exceeds discovered CTRL caps");
                                        status = SA_ERR_HPI_ERROR;
                                        break;
                                }
                        }
                }
	}

	/* free the temporary indices */
	if(indices)             
		g_free(indices);

        /* free the temporary rdr ctrl cap cache */
        if (sahpi_sensor_cap) 
                g_free(sahpi_sensor_cap);

        sc_free_pdu(&get_cap_response);
                
        return(status);

}

static void count_range_flags(SaHpiRdrTypeUnionT *sen_cap, int num_sen_rdrs)
{        	
	int i;
	memset(&range_flags_count, 0, sizeof(range_flags_count));

printf("num_sen_rdrs  %d\n", num_sen_rdrs);

	for(i=0; i < num_sen_rdrs; i++) {

printf(" rdr range flags %x count %d\n", sen_cap[i].SensorRec.DataFormat.Range.Flags, i);

		if (sen_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_MIN) 
			{ 
			range_flags_count.min++; 
			printf("MIN %d\n", range_flags_count.min);
			}
		if (sen_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_MAX) 
			{ 
			range_flags_count.max++; 
			printf("MAX %d\n", range_flags_count.max);
			}
		if (sen_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN) 
			{ 
			range_flags_count.normal_min++; 
			printf("NORMAL_MIN %d\n", range_flags_count.normal_min);
			}
		if (sen_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX)
			{ 
			range_flags_count.normal_max++;
			printf("NORAML_MAX %d\n", range_flags_count.normal_max);
			}
		if (sen_cap[i].SensorRec.DataFormat.Range.Flags & SAHPI_SRF_NOMINAL) 
			{
			range_flags_count.nominal++;
			printf("NOM %d\n", range_flags_count.nominal);
			}
	}
}

#define inv_row_get(row_oid) \
do{ \
	build_res_oid(anOID, \
			row_oid, \
			SA_HPI_INV_ENTRY_TABLE_VARIABLE_OID_LENGTH, \
			indices, \
			NUM_INV_INDICES); \
	snmp_status = snmp_getn_bulk(custom_handle->ss, \
					anOID, \
					SA_HPI_INV_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH, \
					get_cap_pdu, \
					&get_cap_response, \
					get_value.integer); \
} while(0)

static SaErrorT get_inventory_capabilities(struct snmp_client_hnd *custom_handle, 
                          SaHpiRdrT *rdr_cache, 
                          int *rdr_type_count, 
                          int num_rdrs)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        SaErrorT status = SA_OK;
        int snmp_status;
        unsigned int i;
        struct variable_list *vars;
        struct snmp_value get_value;

        SaHpiRdrTypeUnionT *sahpi_inventory_cap = NULL;

	oid anOID[MAX_OID_LEN];
        oid *indices;

printf("***** get_inventory_capabilities *****\n");

        /* make sure present inventory cap count marches discovered */
        status = snmp_get2(custom_handle->ss,
                           sa_hpi_inv_count,
                           SA_HPI_INV_COUNT_OID_LEN, 
                           &get_value);

printf("***** get_inventory_capabilities:snmp_get2 *****\n");

        if (get_value.integer != rdr_type_count[SAHPI_INVENTORY_RDR]) {
                dbg("Sever Error: SA_HPI_INVENTORY_COUNT value doesn't match Resource Capability Settings");
                return(SA_ERR_HPI_ERROR);
        }

        /* begin filling in table */
        sahpi_inventory_cap = g_malloc0(get_value.integer * sizeof(*sahpi_inventory_cap));
        if(!sahpi_inventory_cap && get_value.integer){
                dbg("ERROR: in get_inventory_capabilities() allocating sahpi_inventory_cap");
                return(SA_ERR_HPI_ERROR);
        }

        /* allocate memory for temp resources indices storage */
        indices = g_malloc0(NUM_INV_INDICES * sizeof(*indices));
        if (!indices){
                dbg("ERROR: in get_sahpi_rdr_table(), allocating indices"); 
                return(SA_ERR_HPI_ERROR);
        }


        /* get SAHPI_INVENTORY_RDR data */
        /* SA_HPI_INVENTORY_EIRID */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     sa_hpi_inv_entry, 
                                     SA_HPI_INV_ENTRY_OID_LEN, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);

printf("***** get_inventory_capabilities:snmp_getn_bulk *****\n");
        /* Process the response */
        if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
        /* SUCCESS: Print the result variables */  

                /* Get the data from the response */
                vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_UNSIGNED)  {

                                sahpi_inventory_cap[i].InventoryRec.EirId = 
                                        (SaHpiEirIdT)*vars->val.integer;
                                /* INDEX:  { saHpiDomainID, saHpiResourceID, */
                                /* saHpiInventoryEirId, saHpiInventoryIndex }*/
                                indices[0] = vars->name[vars->name_length - 4];
                                indices[1] = vars->name[vars->name_length - 3];
                                indices[2] = vars->name[vars->name_length - 2];
                                indices[3] = vars->name[vars->name_length - 1];

                        } else
                                printf("SA_HPI_INVENTORY_EIRID:something terrible has happened\n");
                        vars = vars->next_variable;  
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);


        /* SA_HPI_INVENTORY_OEM */ 
	inv_row_get(sa_hpi_inv_custom_field);
	if (snmp_status == STAT_SUCCESS && get_cap_response->errstat == SNMP_ERR_NOERROR) {
		/* Get the data from the response */
		vars = get_cap_response->variables;

                for (i = 0; i < get_value.integer; i++) {
                        if (vars->type == ASN_UNSIGNED)
                                sahpi_inventory_cap[i].InventoryRec.Oem = 
                                        (SaHpiUint32T)*vars->val.integer;
                        else
                                printf("SA_HPI_INVENTORY_OEM:something terrible has happened\n");
                        vars = vars->next_variable;
                }
                display_vars(get_cap_response);
        } else
                status = net_snmp_failure(custom_handle, snmp_status, get_cap_response);
        sc_free_pdu(&get_cap_response);
                

        /* INVENTORY CAPABILITIES ADD TO rdr_cache */
        if (status == SA_OK) {
                printf("***********************************************************\n");
                printf("** Adding sahpi_inventory_cap[] to rdr_cache[]           **\n");
                printf("***********************************************************\n");
                int ii = 0;
                for( i = 0; i < num_rdrs; i++) {
                        if (rdr_cache[i].RdrType == SAHPI_INVENTORY_RDR ) {
                                rdr_cache[i].RdrTypeUnion.InventoryRec = sahpi_inventory_cap[ii].InventoryRec;
                                ii++;
                                if (ii > get_value.integer) {
                                        dbg("Number of RDRs of type CTRL exceeds discovered CTRL caps");
                                        status = SAHPI_INVENTORY_RDR;
                                        break;
                                }
                        }
                }
        }

        /* free the temporary indices */
        if(indices)             
                g_free(indices);

        /* free temporary sahpi_inventory_cap buffer */
        if (sahpi_inventory_cap) 
                g_free(sahpi_inventory_cap);

        return(status);

}

static SaErrorT get_watchdog_capabilities(struct snmp_client_hnd *custom_handle, 
                          SaHpiRdrT *rdr_cache, 
                          int *rdr_type_count, 
                          int num_rdrs)
{
        struct snmp_pdu *get_cap_pdu = NULL;
        struct snmp_pdu *get_cap_response = NULL;
        int status = SA_OK;
        int snmp_status;
        struct variable_list *vars;
        struct snmp_value get_value;

        SaHpiRdrTypeUnionT *sahpi_watchdog_cap = NULL;

        /* make sure present wd cap count marches discovered */
        status = snmp_get2(custom_handle->ss,
                           sa_hpi_wd_count,
                           SA_HPI_WD_COUNT_OID_LEN, 
                           &get_value);
        if (get_value.integer != rdr_type_count[SAHPI_WATCHDOG_RDR]) {
                dbg("Sever Error: SA_HPI_WATCHDOG_COUNT value doesn't match Resource Capability Settings");
                return(SA_ERR_HPI_ERROR);
        }

        /* begin filling in table */
        sahpi_watchdog_cap = g_malloc0(get_value.integer * sizeof(*sahpi_watchdog_cap));
        if(!sahpi_watchdog_cap && get_value.integer){
                dbg("ERROR: in get_inventory_capabilities() allocating sahpi_inventory_cap");
                return(SA_ERR_HPI_ERROR);
        }

        /* get SAHPI_WATCHDOG_RDR data */
        snmp_status = snmp_getn_bulk(custom_handle->ss, 
                                     sa_hpi_wd_entry, 
                                     SA_HPI_WD_ENTRY_OID_LEN, 
                                     get_cap_pdu, 
                                     &get_cap_response, 
                                     get_value.integer);

        /* INDEX: { saHpiDomainID, saHpiResourceID, saHpiWatchdogNum } */
        vars = get_cap_response->variables;


        /* free temporary sahpi_inventory_cap buffer */
        if (sahpi_watchdog_cap) 
                g_free(sahpi_watchdog_cap);

        sc_free_pdu(&get_cap_response);

        return(status);

}









