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

#ifndef _SNMP_CLIENT_RES_H_
#define _SNMP_CLIENT_RES_H_

#define MAX_LEN_OID 128

#if 0
#define CHECK_END(a) ((a != SNMP_ENDOFMIBVIEW) &&  \
                  (a != SNMP_NOSUCHOBJECT) &&  \
                  (a != SNMP_NOSUCHINSTANCE))? 1:0 
#endif		  

int get_sahpi_table_entries(RPTable *temp_rptable, 
                            struct oh_handler_state *handle, 
                            int num_entries );

int get_sahpi_rdr_table( RPTable *temp_rptable,
                         struct oh_handler_state *handle, 
                         int num_entries );
#if 0
int snmp_get_bulk( struct snmp_session *ss, 
                          const char *bulk_objid, 
                          struct snmp_pdu *bulk_pdu, 
                          struct snmp_pdu **bulk_response );
#endif


// New Mib 02/09/04
#define HPI_MIB 1,3,6,1,4,1,18568,1,1,1
#define HPI_MIB_OID_LEN 10



/**************************************/
/*      saHpiEntity                   */
/**************************************/
#define NUM_RES_INDICES 3

#define SA_HPI_ENTRY_COUNT_OID_LEN                              HPI_MIB_OID_LEN + 3
#define SA_HPI_ENTRY_OID_LEN                                    HPI_MIB_OID_LEN + 3

#define SA_HPI_ENTRY_TABLE_VARIABLE_OID_LENGTH                  HPI_MIB_OID_LEN + 4
#define SA_HPI_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH             SA_HPI_ENTRY_TABLE_VARIABLE_OID_LENGTH + NUM_RES_INDICES
                                                        
#ifdef _SNMP_CLIENT_C_
oid sa_hpi_entry_count[] =                              { HPI_MIB, 1, 1, 0 };
oid sa_hpi_entry[] =                                    { HPI_MIB, 1, 4, 1 };
oid sa_hpi_domain_id[] =                                { HPI_MIB, 1, 4, 1, 1 };
oid sa_hpi_entry_id[] =                                 { HPI_MIB, 1, 4, 1, 2 };
oid sa_hpi_resource_id[] =                              { HPI_MIB, 1, 4, 1, 3 };
oid sa_hpi_resource_entity_path[] =                     { HPI_MIB, 1, 4, 1, 4 };
oid sa_hpi_resource_capabilities[] =                    { HPI_MIB, 1, 4, 1, 5 };
oid sa_hpi_resource_severity[] =                        { HPI_MIB, 1, 4, 1, 6 };
oid sa_hpi_resource_info_resource_rev[] =               { HPI_MIB, 1, 4, 1, 7 };
oid sa_hpi_domain_resource_info_specific_ver[] =        { HPI_MIB, 1, 4, 1, 8 };
oid sa_hpi_domain_resource_info_device_support[] =      { HPI_MIB, 1, 4, 1, 9 };
oid sa_hpi_domain_resource_info_manufactuer_id[] =      { HPI_MIB, 1, 4, 1, 10 };
oid sa_hpi_domain_resource_info_product_id[] =          { HPI_MIB, 1, 4, 1, 11 };
oid sa_hpi_domain_resource_info_firmware_major_rev[] =  { HPI_MIB, 1 ,4, 1, 12 };
oid sa_hpi_domain_resource_info_firmware_minor_rev[] =  { HPI_MIB, 1, 4, 1, 13 };
oid sa_hpi_domain_resource_info_aux_firmware_rev[] =    { HPI_MIB, 1, 4, 1, 14 };
oid sa_hpi_domain_resource_tag_text_type[] =            { HPI_MIB, 1, 4, 1, 15 };
oid sa_hpi_domain_resource_tag_text_language[] =        { HPI_MIB, 1, 4, 1, 16 };
oid sa_hpi_domain_resource_tag[] =                      { HPI_MIB, 1, 4, 1, 17 };
oid sa_hpi_domain_param_control[] =                     { HPI_MIB, 1, 4, 1, 18 };
oid sa_hpi_domain_clear_events[] =                      { HPI_MIB, 1, 4, 1, 19 };
oid sa_hpi_domain_event_log_time[] =                    { HPI_MIB, 1, 4, 1, 20 };
oid sa_hpi_domain_event_log_state[] =                   { HPI_MIB, 1, 4, 1, 21 };
#else
extern oid sa_hpi_entry_count[];                             
extern oid sa_hpi_entry[];                                   
extern oid sa_hpi_domain_id[];                       
extern oid sa_hpi_entry_id[];   
extern oid sa_hpi_resource_id[];                             
extern oid sa_hpi_resource_entity_path[];                    
extern oid sa_hpi_resource_capabilities[];                   
extern oid sa_hpi_resource_severity[];               
extern oid sa_hpi_resource_info_resource_rev[];              
extern oid sa_hpi_domain_resource_info_specific_ver[];      
extern oid sa_hpi_domain_resource_info_device_support[];    
extern oid sa_hpi_domain_resource_info_manufactuer_id[];    
extern oid sa_hpi_domain_resource_info_product_id[];
extern oid sa_hpi_domain_resource_info_firmware_major_rev[];
extern oid sa_hpi_domain_resource_info_firmware_minor_rev[];
extern oid sa_hpi_domain_resource_info_aux_firmware_rev[]; 
extern oid sa_hpi_domain_resource_tag_text_type[];           
extern oid sa_hpi_domain_resource_tag_text_language[];      
extern oid sa_hpi_domain_resource_tag[];                     
extern oid sa_hpi_domain_param_control[];                    
extern oid sa_hpi_domain_clear_events[];                     
extern oid sa_hpi_domain_event_log_time[];                   
extern oid sa_hpi_domain_event_log_state[];                  
#endif

/**************************************/
/*      saHpiEntity                   */
/**************************************/
#define NUM_RDR_INDICES 4

#define SA_HPI_RDR_ENTRY_COUNT_OID_LEN                  HPI_MIB_OID_LEN + 3 /* TODO ask konrad why need .0 on end */
#define SA_HPI_RDR_ENTRY_OID_LEN                        HPI_MIB_OID_LEN + 3

#define SA_HPI_RDR_ENTRY_TABLE_VARIABLE_OID_LENGTH      HPI_MIB_OID_LEN + 4

#define SA_HPI_RDR_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH \
        SA_HPI_RDR_ENTRY_TABLE_VARIABLE_OID_LENGTH + NUM_RDR_INDICES

#ifdef _SNMP_CLIENT_C_
oid sa_hpi_rdr_count[] =                { HPI_MIB, 3, 1, 0 };
oid sa_hpi_rdr_entry[] =                { HPI_MIB, 3, 2, 1 }; 
oid sa_hpi_rdr_record_id[] =            { HPI_MIB, 3, 2, 1, 1 };
oid sa_hpi_rdr_type[] =                 { HPI_MIB, 3, 2, 1, 2 };
oid sa_hpi_rdr_entity_path[] =          { HPI_MIB, 3, 2, 1, 3 };
#else
extern oid sa_hpi_rdr_count[];
extern oid sa_hpi_rdr_entry[];                  
extern oid sa_hpi_rdr_record_id[];                      
extern oid sa_hpi_rdr_type[];                   
extern oid sa_hpi_rdr_entity_path[];            
#endif


/**************************************/
/*      saHpiCtrl                     */
/**************************************/
#define NUM_CTRL_INDICES 3

#define SA_HPI_CTRL_COUNT_OID_LEN                       HPI_MIB_OID_LEN + 3 
#define SA_HPI_CTRL_ENTRY_OID_LEN                       HPI_MIB_OID_LEN + 3

#define SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH     HPI_MIB_OID_LEN + 4

#define SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH        \
        SA_HPI_CTRL_ENTRY_TABLE_VARIABLE_OID_LENGTH + NUM_CTRL_INDICES

#ifdef _SNMP_CLIENT_C_
oid sa_hpi_ctrl_count[] =               { HPI_MIB, 3, 3, 0 };
oid sa_hpi_ctrl_entry[] =               { HPI_MIB, 3, 4, 1 }; 
oid sa_hpi_ctrl_num[] =                 { HPI_MIB, 3, 4, 1, 1 };
oid sa_hpi_ctrl_output_type[] =         { HPI_MIB, 3, 4, 1, 2 };
oid sa_hpi_ctrl_ignore[] =              { HPI_MIB, 3, 4, 1, 3 };
oid sa_hpi_ctrl_type[] =                { HPI_MIB, 3, 4, 1, 4 };
oid sa_hpi_ctrl_state[] =               { HPI_MIB, 3, 4, 1, 5 };
oid sa_hpi_ctrl_attr[] =                { HPI_MIB, 3, 4, 1, 6 };
oid sa_hpi_ctrl_oem[] =                 { HPI_MIB, 3, 4, 1, 7 };
oid sa_hpi_ctrl_txt_type[] =            { HPI_MIB, 3, 4, 1, 8 };
oid sa_hpi_ctrl_txt_lang[] =            { HPI_MIB, 3, 4, 1, 9 };
oid sa_hpi_ctrl_txt[] =                 { HPI_MIB, 3, 4, 1, 10 };
oid sa_hpi_ctrl_RDR[] =                 { HPI_MIB, 3, 4, 1, 11 };
#else
extern oid sa_hpi_ctrl_count[];
extern oid sa_hpi_ctrl_entry[];                         
extern oid sa_hpi_ctrl_num[];                           
extern oid sa_hpi_ctrl_output_type[];                   
extern oid sa_hpi_ctrl_ignore[];                
extern oid sa_hpi_ctrl_type[];           
extern oid sa_hpi_ctrl_state[];           
extern oid sa_hpi_ctrl_attr[];            
extern oid sa_hpi_ctrl_oem[];             
extern oid sa_hpi_ctrl_txt_type[];        
extern oid sa_hpi_ctrl_txt_lang[];        
extern oid sa_hpi_ctrl_txt[];         
extern oid sa_hpi_ctrl_RDR[];                   
#endif


/**************************************/
/*      saHpiSensor                   */
/**************************************/
#define SAHPI_SENSOR_READING_THD_LOW_CRITICAL_TABLE   0
#define SAHPI_SENSOR_READING_THD_LOW_MAJORL_TABLE     1
#define SAHPI_SENSOR_READING_THD_LOW_MINORL_TABLE     2
#define SAHPI_SENSOR_READING_THD_UP_CRITICAL_TABLE    3
#define SAHPI_SENSOR_READING_THD_UP_MAJOR_TABLE       4
#define SAHPI_SENSOR_READING_THD_UP_MINOR_TABLE       5
#define SAHPI_SENSOR_READING_THD_POS_HYSTERESIS_TABLE 6 
#define SAHPI_SENSOR_READING_THD_NEG_HYSTERESIS_TABLE 7

#define THOLD_IS_READABLE       0
#define THOLD_WRITEABLE       	1
#define THOLD_FIXED       	2
#define THOLD_VALUES_PRESENT   	3
#define THOLD_RAW       	4
#define THOLD_INTREPRETED     	5

#define NUM_THRESHOLD_TABLES 8

#define NUM_SEN_INDICES 3

#define SA_HPI_SEN_COUNT_OID_LEN                        HPI_MIB_OID_LEN + 3 
#define SA_HPI_SEN_ENTRY_OID_LEN                        HPI_MIB_OID_LEN + 3

#define SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH      HPI_MIB_OID_LEN + 4

#define SA_HPI_SEN_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH \
        SA_HPI_SEN_ENTRY_TABLE_VARIABLE_OID_LENGTH + NUM_SEN_INDICES
	
/* saHpiSensorReading<>Table */
#define SA_HPI_SEN_READING_MAX_ENTRY_OID_LEN                        HPI_MIB_OID_LEN + 4
#define SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH      HPI_MIB_OID_LEN + 5

#define SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH \
        SA_HPI_SEN_READING_MAX_ENTRY_TABLE_VARIABLE_OID_LENGTH + NUM_SEN_INDICES


#ifdef _SNMP_CLIENT_C_                  
oid sa_hpi_sen_count[] =                { HPI_MIB, 3, 5, 0 };
oid sa_hpi_sen_entry[] =                { HPI_MIB, 3, 6, 1 }; 
oid sa_hpi_sen_index[] =                { HPI_MIB, 3, 6, 1, 1 };
oid sa_hpi_sen_type[] =                 { HPI_MIB, 3, 6, 1, 2 };
oid sa_hpi_sen_category[] =             { HPI_MIB, 3, 6, 1, 3 };
oid sa_hpi_sen_evts_cat_ctrl[] =        { HPI_MIB, 3, 6, 1, 4 };
oid sa_hpi_sen_evts_supported[] =       { HPI_MIB, 3, 6, 1, 5 };
oid sa_hpi_sen_status[] =               { HPI_MIB, 3, 6, 1, 6 };
oid sa_hpi_sen_assert_evts[] =          { HPI_MIB, 3, 6, 1, 7 };
oid sa_hpi_sen_deassert_evts[] =        { HPI_MIB, 3, 6, 1, 8 };
oid sa_hpi_sen_ignore[] =               { HPI_MIB, 3, 6, 1, 9 };
oid sa_hpi_sen_reading_formats[] =      { HPI_MIB, 3, 6, 1, 10 };
oid sa_hpi_sen_is_numeric[] =           { HPI_MIB, 3, 6, 1, 11 };
oid sa_hpi_sen_sign_foramt[] =          { HPI_MIB, 3, 6, 1, 12 };
oid sa_hpi_sen_base_units[] =           { HPI_MIB, 3, 6, 1, 13 };
oid sa_hpi_sen_modifier_units[] =       { HPI_MIB, 3, 6, 1, 14 };
oid sa_hpi_sen_modifier_use[] =         { HPI_MIB, 3, 6, 1, 15 };
oid sa_hpi_sen_factors_static[] =       { HPI_MIB, 3, 6, 1, 16 };
oid sa_hpi_sen_factors[] =              { HPI_MIB, 3, 6, 1, 17 };
oid sa_hpi_sen_factors_linearization[] ={ HPI_MIB, 3, 6, 1, 18 };
oid sa_hpi_sen_percentage[] =           { HPI_MIB, 3, 6, 1, 19 };
oid sa_hpi_sen_range_flags[] =       	{ HPI_MIB, 3, 6, 1, 20 };
oid sa_hpi_sen_has_thresholds[] =       { HPI_MIB, 3, 6, 1, 21 };
oid sa_hpi_sen_thd_cap[] =             	{ HPI_MIB, 3, 6, 1, 22 };
oid sa_hpi_sen_oem[] =               	{ HPI_MIB, 3, 6, 1, 23 };

/* saHpiSensorReadingCurrentTable */
oid sa_hpi_sensor_reading_current_entry[] =		{ HPI_MIB, 3, 7, 1, 1 };
oid sa_hpi_sensor_reading_current_values_present[] =	{ HPI_MIB, 3, 7, 1, 1, 1 };
oid sa_hpi_sensor_reading_current_raw[] =		{ HPI_MIB, 3, 7, 1, 1, 2 };
oid sa_hpi_sensor_reading_current_intrepreted[] =	{ HPI_MIB, 3, 7, 1, 1, 3 };
oid sa_hpi_sensor_reading_current_status[] =		{ HPI_MIB, 3, 7, 1, 1, 4 };
oid sa_hpi_sensor_reading_current_evt_status[] =	{ HPI_MIB, 3, 7, 1, 1, 5 };

/* saHpiSensorReadingMaxTable */
oid sa_hpi_sensor_reading_max_entry[] =		{ HPI_MIB, 3, 7, 2, 1 };
oid sa_hpi_sensor_reading_max_values_present[] ={ HPI_MIB, 3, 7, 2, 1, 1 };
oid sa_hpi_sensor_reading_max_raw[] =		{ HPI_MIB, 3, 7, 2, 1, 2 };
oid sa_hpi_sensor_reading_max_intrepreted[] =	{ HPI_MIB, 3, 7, 2, 1, 3 };
oid sa_hpi_sensor_reading_max_status[] =	{ HPI_MIB, 3, 7, 2, 1, 4 };
oid sa_hpi_sensor_reading_max_evt_status[] =	{ HPI_MIB, 3, 7, 2, 1, 5 };

/* saHpiSensorReadingMinTable */
oid sa_hpi_sensor_reading_min_entry[] =		{ HPI_MIB, 3, 7, 3, 1 };
oid sa_hpi_sensor_reading_min_values_present[] ={ HPI_MIB, 3, 7, 3, 1, 1 };
oid sa_hpi_sensor_reading_min_raw[] =		{ HPI_MIB, 3, 7, 3, 1, 2 };
oid sa_hpi_sensor_reading_min_intrepreted[] =	{ HPI_MIB, 3, 7, 3, 1, 3 };
oid sa_hpi_sensor_reading_min_status[] =	{ HPI_MIB, 3, 7, 3, 1, 4 };
oid sa_hpi_sensor_reading_min_evt_status[] =	{ HPI_MIB, 3, 7, 3, 1, 5 };

/* saHpiSensorReadingNominalTable */
oid sa_hpi_sensor_reading_nominal_entry[] =		{ HPI_MIB, 3, 7, 4, 1 };
oid sa_hpi_sensor_reading_nominal_values_present[] =	{ HPI_MIB, 3, 7, 4, 1, 1 };
oid sa_hpi_sensor_reading_nominal_raw[] =		{ HPI_MIB, 3, 7, 4, 1, 2 };
oid sa_hpi_sensor_reading_nominal_intrepreted[] =	{ HPI_MIB, 3, 7, 4, 1, 3 };
oid sa_hpi_sensor_reading_nominal_status[] =		{ HPI_MIB, 3, 7, 4, 1, 4 };
oid sa_hpi_sensor_reading_nominal_evt_status[] =	{ HPI_MIB, 3, 7, 4, 1, 5 };

/* saHpiSensorReadingNormalMaxTable */
oid sa_hpi_sensor_reading_normal_max_entry[] =		{ HPI_MIB, 3, 7, 5, 1 };
oid sa_hpi_sensor_reading_normal_max_values_present[] =	{ HPI_MIB, 3, 7, 5, 1, 1 };
oid sa_hpi_sensor_reading_normal_max_raw[] =		{ HPI_MIB, 3, 7, 5, 1, 2 };
oid sa_hpi_sensor_reading_normal_max_intrepreted[] =	{ HPI_MIB, 3, 7, 5, 1, 3 };
oid sa_hpi_sensor_reading_normal_max_status[] =        	{ HPI_MIB, 3, 7, 5, 1, 4 };
oid sa_hpi_sensor_reading_normal_max_evt_status[] =	{ HPI_MIB, 3, 7, 5, 1, 5 };

/* saHpiSensorReadingNormalMinTable */
oid sa_hpi_sensor_reading_normal_min_entry[] =		{ HPI_MIB, 3, 7, 6, 1 };
oid sa_hpi_sensor_reading_normal_min_values_present[] =	{ HPI_MIB, 3, 7, 6, 1, 1 };
oid sa_hpi_sensor_reading_normal_min_raw[] =		{ HPI_MIB, 3, 7, 6, 1, 2 };
oid sa_hpi_sensor_reading_normal_min_intrepreted[] =	{ HPI_MIB, 3, 7, 6, 1, 3 };
oid sa_hpi_sensor_reading_normal_min_status[] =        	{ HPI_MIB, 3, 7, 6, 1, 4 };
oid sa_hpi_sensor_reading_normal_min_evt_status[] =	{ HPI_MIB, 3, 7, 6, 1, 5 };

/* saHpiSensorReadingThdLowCriticalTable */
oid sa_hpi_sensor_thd_low_critical_entry[] =		{ HPI_MIB, 3, 7, 7, 1 };
oid sa_hpi_sensor_thd_low_critical_calls_readable[] =	{ HPI_MIB, 3, 7, 7, 1, 1 };
oid sa_hpi_sensor_thd_low_critical_calls_writeable[] =  { HPI_MIB, 3, 7, 7, 1, 2 };
oid sa_hpi_sensor_thd_low_critical_calls_fixed[] =	{ HPI_MIB, 3, 7, 7, 1, 3 };
oid sa_hpi_sensor_thd_low_critical_valuespresent[] =    { HPI_MIB, 3, 7, 7, 1, 4 };
oid sa_hpi_sensor_thd_low_critical_raw[] =		{ HPI_MIB, 3, 7, 7, 1, 5 };
oid sa_hpi_sensor_thd_low_critical_interpreted[] =	{ HPI_MIB, 3, 7, 7, 1, 6 };

/* saHpiSensorReadingThdLowMajorlTable */
oid sa_hpi_sensor_thd_low_major_entry[] =		{ HPI_MIB, 3, 7, 8, 1 };
oid sa_hpi_sensor_thd_low_major_calls_readable[] =	{ HPI_MIB, 3, 7, 8, 1, 1 };
oid sa_hpi_sensor_thd_low_major_calls_writeable[] =  	{ HPI_MIB, 3, 7, 8, 1, 2 };
oid sa_hpi_sensor_thd_low_major_calls_fixed[] =		{ HPI_MIB, 3, 7, 8, 1, 3 };
oid sa_hpi_sensor_thd_low_major_valuespresent[] =    	{ HPI_MIB, 3, 7, 8, 1, 4 };
oid sa_hpi_sensor_thd_low_major_raw[] =			{ HPI_MIB, 3, 7, 8, 1, 5 };
oid sa_hpi_sensor_thd_low_major_interpreted[] =		{ HPI_MIB, 3, 7, 8, 1, 6 };

/* saHpiSensorReadingThdLowMinorlTable */
oid sa_hpi_sensor_thd_low_minor_entry[] =		{ HPI_MIB, 3, 7, 9, 1 };
oid sa_hpi_sensor_thd_low_minor_calls_readable[] =	{ HPI_MIB, 3, 7, 9, 1, 1 };
oid sa_hpi_sensor_thd_low_minor_calls_writeable[] =  	{ HPI_MIB, 3, 7, 9, 1, 2 };
oid sa_hpi_sensor_thd_low_minor_calls_fixed[] =		{ HPI_MIB, 3, 7, 9, 1, 3 };
oid sa_hpi_sensor_thd_low_minor_valuespresent[] =    	{ HPI_MIB, 3, 7, 9, 1, 4 };
oid sa_hpi_sensor_thd_low_minor_raw[] =			{ HPI_MIB, 3, 7, 9, 1, 5 };
oid sa_hpi_sensor_thd_low_minor_interpreted[] =		{ HPI_MIB, 3, 7, 9, 1, 6 };

/* saHpiSensorReadingThdUpCriticallTable */
oid sa_hpi_sensor_thd_up_critical_entry[] =		{ HPI_MIB, 3, 7, 10, 1 };
oid sa_hpi_sensor_thd_up_critical_calls_readable[] =	{ HPI_MIB, 3, 7, 10, 1, 1 };
oid sa_hpi_sensor_thd_up_critical_calls_writeable[] =  	{ HPI_MIB, 3, 7, 10, 1, 2 };
oid sa_hpi_sensor_thd_up_critical_calls_fixed[] =	{ HPI_MIB, 3, 7, 10, 1, 3 };
oid sa_hpi_sensor_thd_up_critical_valuespresent[] =    	{ HPI_MIB, 3, 7, 10, 1, 4 };
oid sa_hpi_sensor_thd_up_critical_raw[] =		{ HPI_MIB, 3, 7, 10, 1, 5 };
oid sa_hpi_sensor_thd_up_critical_interpreted[] =	{ HPI_MIB, 3, 7, 10, 1, 6 };

/* saHpiSensorReadingThdUpMajorlTable */
oid sa_hpi_sensor_thd_up_major_entry[] =		{ HPI_MIB, 3, 7, 11, 1 };
oid sa_hpi_sensor_thd_up_major_calls_readable[] =	{ HPI_MIB, 3, 7, 11, 1, 1 };
oid sa_hpi_sensor_thd_up_major_calls_writeable[] =  	{ HPI_MIB, 3, 7, 11, 1, 2 };
oid sa_hpi_sensor_thd_up_major_calls_fixed[] =		{ HPI_MIB, 3, 7, 11, 1, 3 };
oid sa_hpi_sensor_thd_up_major_valuespresent[] =    	{ HPI_MIB, 3, 7, 11, 1, 4 };
oid sa_hpi_sensor_thd_up_major_raw[] =			{ HPI_MIB, 3, 7, 11, 1, 5 };
oid sa_hpi_sensor_thd_up_major_interpreted[] =		{ HPI_MIB, 3, 7, 11, 1, 6 };

/* saHpiSensorReadingThdUpMinorlTable */
oid sa_hpi_sensor_thd_up_minor_entry[] =		{ HPI_MIB, 3, 7, 12, 1 };
oid sa_hpi_sensor_thd_up_minor_calls_readable[] =	{ HPI_MIB, 3, 7, 12, 1, 1 };
oid sa_hpi_sensor_thd_up_minor_calls_writeable[] =  	{ HPI_MIB, 3, 7, 12, 1, 2 };
oid sa_hpi_sensor_thd_up_minor_calls_fixed[] =		{ HPI_MIB, 3, 7, 12, 1, 3 };
oid sa_hpi_sensor_thd_up_minor_valuespresent[] =    	{ HPI_MIB, 3, 7, 12, 1, 4 };
oid sa_hpi_sensor_thd_up_minor_raw[] =			{ HPI_MIB, 3, 7, 12, 1, 5 };
oid sa_hpi_sensor_thd_up_minor_interpreted[] =		{ HPI_MIB, 3, 7, 12, 1, 6 };

/* saHpiSensorReadingThdPosHysteresislTable */
oid sa_hpi_sensor_thd_pos_hysteresis_entry[] =		{ HPI_MIB, 3, 7, 13, 1 };
oid sa_hpi_sensor_thd_pos_hysteresis_calls_readable[] =	{ HPI_MIB, 3, 7, 13, 1, 1 };
oid sa_hpi_sensor_thd_pos_hysteresis_calls_writeable[] ={ HPI_MIB, 3, 7, 13, 1, 2 };
oid sa_hpi_sensor_thd_pos_hysteresis_calls_fixed[] =	{ HPI_MIB, 3, 7, 13, 1, 3 };
oid sa_hpi_sensor_thd_pos_hysteresis_valuespresent[] =  { HPI_MIB, 3, 7, 13, 1, 4 };
oid sa_hpi_sensor_thd_pos_hysteresis_raw[] =		{ HPI_MIB, 3, 7, 13, 1, 5 };
oid sa_hpi_sensor_thd_pos_hysteresis_interpreted[] =	{ HPI_MIB, 3, 7, 13, 1, 6 };

/* saHpiSensorReadingThdNegHysteresislTable */
oid sa_hpi_sensor_thd_neg_hysteresis_entry[] =		{ HPI_MIB, 3, 7, 14, 1 };
oid sa_hpi_sensor_thd_neg_hysteresis_calls_readable[] =	{ HPI_MIB, 3, 7, 14, 1, 1 };
oid sa_hpi_sensor_thd_neg_hysteresis_calls_writeable[] ={ HPI_MIB, 3, 7, 14, 1, 2 };
oid sa_hpi_sensor_thd_neg_hysteresis_calls_fixed[] =	{ HPI_MIB, 3, 7, 14, 1, 3 };
oid sa_hpi_sensor_thd_neg_hysteresis_valuespresent[] =  { HPI_MIB, 3, 7, 14, 1, 4 };
oid sa_hpi_sensor_thd_neg_hysteresis_raw[] =		{ HPI_MIB, 3, 7, 14, 1, 5 };
oid sa_hpi_sensor_thd_neg_hysteresis_interpreted[] =	{ HPI_MIB, 3, 7, 14, 1, 6 };

#else
extern oid sa_hpi_sen_count[];
extern oid sa_hpi_sen_entry[];
extern oid sa_hpi_sen_index[];
extern oid sa_hpi_sen_type[];
extern oid sa_hpi_sen_category[];
extern oid sa_hpi_sen_evts_cat_ctrl[];
extern oid sa_hpi_sen_evts_supported[];
extern oid sa_hpi_sen_status[];
extern oid sa_hpi_sen_assert_evts[];
extern oid sa_hpi_sen_deassert_evts[];
extern oid sa_hpi_sen_ignore[];
extern oid sa_hpi_sen_reading_formats[];
extern oid sa_hpi_sen_is_numeric[];
extern oid sa_hpi_sen_sign_foramt[];
extern oid sa_hpi_sen_base_units[];       
extern oid sa_hpi_sen_modifier_units[];       
extern oid sa_hpi_sen_modifier_use[];        
extern oid sa_hpi_sen_factors_static[];       
extern oid sa_hpi_sen_factors[];        
extern oid sa_hpi_sen_factors_linearization[];
extern oid sa_hpi_sen_percentage[]; 
extern oid sa_hpi_sen_range_flags[];
extern oid sa_hpi_sen_has_thresholds[];
extern oid sa_hpi_sen_thd_cap[];
extern oid sa_hpi_sen_oem[]; 

/* saHpiSensorReadingCurrentTable */
extern oid sa_hpi_sensor_reading_current_entry[];             
extern oid sa_hpi_sensor_reading_current_values_present[];    
extern oid sa_hpi_sensor_reading_current_raw[];               
extern oid sa_hpi_sensor_reading_current_intrepreted[];       
extern oid sa_hpi_sensor_reading_current_status[];           
extern oid sa_hpi_sensor_reading_current_evt_status[];        

/* saHpiSensorReadingMaxTable */
extern oid sa_hpi_sensor_reading_max_entry[];		
extern oid sa_hpi_sensor_reading_max_values_present[];	
extern oid sa_hpi_sensor_reading_max_raw[];		
extern oid sa_hpi_sensor_reading_max_intrepreted[];	
extern oid sa_hpi_sensor_reading_max_status[];		
extern oid sa_hpi_sensor_reading_max_evt_status[];

/* saHpiSensorReadingMinTable */
extern oid sa_hpi_sensor_reading_min_entry[];		
extern oid sa_hpi_sensor_reading_min_values_present[];	
extern oid sa_hpi_sensor_reading_min_raw[];		
extern oid sa_hpi_sensor_reading_min_intrepreted[];	
extern oid sa_hpi_sensor_reading_min_status[];		
extern oid sa_hpi_sensor_reading_min_evt_status[];

/* saHpiSensorReadingNominalTable */
extern oid sa_hpi_sensor_reading_nominal_entry[];		
extern oid sa_hpi_sensor_reading_nominal_values_present[];	
extern oid sa_hpi_sensor_reading_nominal_raw[];		
extern oid sa_hpi_sensor_reading_nominal_intrepreted[];	
extern oid sa_hpi_sensor_reading_nominal_status[];		
extern oid sa_hpi_sensor_reading_nominal_evt_status[];

/* saHpiSensorReadingNormalMaxTable */
extern oid sa_hpi_sensor_reading_normal_max_entry[];		
extern oid sa_hpi_sensor_reading_normal_max_values_present[];	
extern oid sa_hpi_sensor_reading_normal_max_raw[];		
extern oid sa_hpi_sensor_reading_normal_max_intrepreted[];	
extern oid sa_hpi_sensor_reading_normal_max_status[];		
extern oid sa_hpi_sensor_reading_normal_max_evt_status[];

/* saHpiSensorReadingNormalMinTable */
extern oid sa_hpi_sensor_reading_normal_min_entry[];		
extern oid sa_hpi_sensor_reading_normal_min_values_present[];	
extern oid sa_hpi_sensor_reading_normal_min_raw[];		
extern oid sa_hpi_sensor_reading_normal_min_intrepreted[];	
extern oid sa_hpi_sensor_reading_normal_min_status[];		
extern oid sa_hpi_sensor_reading_normal_min_evt_status[];

/* saHpiSensorReadingThdLowCriticalTable */
extern oid sa_hpi_sensor_thd_low_critical_entry[];		
extern oid sa_hpi_sensor_thd_low_critical_calls_readable[];	
extern oid sa_hpi_sensor_thd_low_critical_calls_writeable[];  
extern oid sa_hpi_sensor_thd_low_critical_calls_fixed[];	
extern oid sa_hpi_sensor_thd_low_critical_valuespresent[];    
extern oid sa_hpi_sensor_thd_low_critical_raw[];		
extern oid sa_hpi_sensor_thd_low_critical_interpreted[];	

/* saHpiSensorReadingThdLowMajorlTable */
extern oid sa_hpi_sensor_thd_low_major_entry[];		
extern oid sa_hpi_sensor_thd_low_major_calls_readable[];	
extern oid sa_hpi_sensor_thd_low_major_calls_writeable[];  	
extern oid sa_hpi_sensor_thd_low_major_calls_fixed[];		
extern oid sa_hpi_sensor_thd_low_major_valuespresent[];    	
extern oid sa_hpi_sensor_thd_low_major_raw[];			
extern oid sa_hpi_sensor_thd_low_major_interpreted[];		

/* saHpiSensorReadingThdLowMinorlTable */
extern oid sa_hpi_sensor_thd_low_minor_entry[];		
extern oid sa_hpi_sensor_thd_low_minor_calls_readable[];	
extern oid sa_hpi_sensor_thd_low_minor_calls_writeable[];  	
extern oid sa_hpi_sensor_thd_low_minor_calls_fixed[];		
extern oid sa_hpi_sensor_thd_low_minor_valuespresent[];    	
extern oid sa_hpi_sensor_thd_low_minor_raw[];			
extern oid sa_hpi_sensor_thd_low_minor_interpreted[];		

/* saHpiSensorReadingThdUpCriticallTable */
extern oid sa_hpi_sensor_thd_up_critical_entry[];		
extern oid sa_hpi_sensor_thd_up_critical_calls_readable[];	
extern oid sa_hpi_sensor_thd_up_critical_calls_writeable[];  	
extern oid sa_hpi_sensor_thd_up_critical_calls_fixed[];	
extern oid sa_hpi_sensor_thd_up_critical_valuespresent[];    	
extern oid sa_hpi_sensor_thd_up_critical_raw[];		
extern oid sa_hpi_sensor_thd_up_critical_interpreted[];	

/* saHpiSensorReadingThdUpMajorlTable */
extern oid sa_hpi_sensor_thd_up_major_entry[];		
extern oid sa_hpi_sensor_thd_up_major_calls_readable[];	
extern oid sa_hpi_sensor_thd_up_major_calls_writeable[];  	
extern oid sa_hpi_sensor_thd_up_major_calls_fixed[];		
extern oid sa_hpi_sensor_thd_up_major_valuespresent[];    	
extern oid sa_hpi_sensor_thd_up_major_raw[];			
extern oid sa_hpi_sensor_thd_up_major_interpreted[];		

/* saHpiSensorReadingThdUpMinorlTable */
extern oid sa_hpi_sensor_thd_up_minor_entry[];		
extern oid sa_hpi_sensor_thd_up_minor_calls_readable[];	
extern oid sa_hpi_sensor_thd_up_minor_calls_writeable[];  	
extern oid sa_hpi_sensor_thd_up_minor_calls_fixed[];		
extern oid sa_hpi_sensor_thd_up_minor_valuespresent[];    	
extern oid sa_hpi_sensor_thd_up_minor_raw[];			
extern oid sa_hpi_sensor_thd_up_minor_interpreted[];		

/* saHpiSensorReadingThdPosHysteresislTable */
extern oid sa_hpi_sensor_thd_pos_hysteresis_entry[];		
extern oid sa_hpi_sensor_thd_pos_hysteresis_calls_readable[];	
extern oid sa_hpi_sensor_thd_pos_hysteresis_calls_writeable[];
extern oid sa_hpi_sensor_thd_pos_hysteresis_calls_fixed[];	
extern oid sa_hpi_sensor_thd_pos_hysteresis_valuespresent[];  
extern oid sa_hpi_sensor_thd_pos_hysteresis_raw[];		
extern oid sa_hpi_sensor_thd_pos_hysteresis_interpreted[];	

/* saHpiSensorReadingThdNegHysteresislTable */
extern oid sa_hpi_sensor_thd_neg_hysteresis_entry[];		
extern oid sa_hpi_sensor_thd_neg_hysteresis_calls_readable[];	
extern oid sa_hpi_sensor_thd_neg_hysteresis_calls_writeable[];
extern oid sa_hpi_sensor_thd_neg_hysteresis_calls_fixed[];	
extern oid sa_hpi_sensor_thd_neg_hysteresis_valuespresent[];  
extern oid sa_hpi_sensor_thd_neg_hysteresis_raw[];		
extern oid sa_hpi_sensor_thd_neg_hysteresis_interpreted[];	

#endif

/**************************************/
/*      saHpiInventory                */
/**************************************/
#define NUM_INV_INDICES 4

#define SA_HPI_INV_COUNT_OID_LEN                        HPI_MIB_OID_LEN + 3 
#define SA_HPI_INV_ENTRY_OID_LEN                        HPI_MIB_OID_LEN + 3

#define SA_HPI_INV_ENTRY_TABLE_VARIABLE_OID_LENGTH      HPI_MIB_OID_LEN + 4

#define SA_HPI_INV_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH \
        SA_HPI_INV_ENTRY_TABLE_VARIABLE_OID_LENGTH + NUM_RDR_INDICES


#ifdef _SNMP_CLIENT_C_                  
oid sa_hpi_inv_count[] =                { HPI_MIB, 3, 8, 0 };
oid sa_hpi_inv_entry[] =                { HPI_MIB, 3, 9, 1 }; 
oid sa_hpi_inv_eir_id[] =               { HPI_MIB, 3, 9, 1, 1 };
oid sa_hpi_inv_index[] =                { HPI_MIB, 3, 9, 1, 2 };
oid sa_hpi_inv_custom_field[] =         { HPI_MIB, 3, 9, 1, 16 };
oid sa_hpi_inv_oem[] =                  { HPI_MIB, 3, 9, 1, 17 };
#else
extern oid sa_hpi_inv_count[];
extern oid sa_hpi_inv_entry[];
extern oid sa_hpi_inv_eir_id[]; 
extern oid sa_hpi_inv_index[];
extern oid sa_hpi_inv_custom_field[];
extern oid sa_hpi_inv_oem[];
#endif

/**************************************/
/*      saHpiWatchDog                 */
/**************************************/
#define NUM_WD_INDICES 3

#define SA_HPI_WD_COUNT_OID_LEN                         HPI_MIB_OID_LEN + 3 
#define SA_HPI_WD_ENTRY_OID_LEN                         HPI_MIB_OID_LEN + 3

#define SA_HPI_WD_ENTRY_TABLE_VARIABLE_OID_LENGTH       HPI_MIB_OID_LEN + 4

#define SA_HPI_WD_ENTRY_TABLE_VARIABLE_FULL_OID_LENGTH  \
        SA_HPI_WD_ENTRY_TABLE_VARIABLE_OID_LENGTH + NUM_RDR_INDICES

#ifdef _SNMP_CLIENT_C_                  
oid sa_hpi_wd_count[] =                 { HPI_MIB, 3, 10, 0 };
oid sa_hpi_wd_entry[] =                 { HPI_MIB, 3, 11, 1 }; 
#else
extern oid sa_hpi_wd_count[];
extern oid sa_hpi_wd_entry[];
#endif

#define NUM_REPITIONS   240

#define NUM_RDR_TYPES   5

#endif /* SNMP_CLIENT_RES_H */
