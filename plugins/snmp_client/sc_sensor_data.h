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
 
/************************************************************************/
/* Range Reading  Tables			 	                */
/************************************************************************/

SaErrorT populate_range_max(struct snmp_client_hnd *custom_handle, 
			    SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
			    int num_rdrs, 
			    SaHpiUint8T rr_num);

SaErrorT populate_range_min(struct snmp_client_hnd *custom_handle, 
			    SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
			    int num_sen_rdrs, 
			    SaHpiUint8T rr_num);

SaErrorT populate_range_nominal(struct snmp_client_hnd *custom_handle, 
				SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				int num_sen_rdrs, 
				SaHpiUint8T rr_num);

SaErrorT populate_range_normal_max(struct snmp_client_hnd *custom_handle, 
				   SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				   int num_sen_rdrs, 
				   SaHpiUint8T rr_num);

SaErrorT populate_range_normal_min(struct snmp_client_hnd *custom_handle, 
				   SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				   int num_sen_rdrs, 
				   SaHpiUint8T rr_num);
/************************************************************************/
/* Threshold Tables							*/
/************************************************************************/

SaErrorT populate_thld_low_crit(struct snmp_client_hnd *custom_handle, 
				SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				int num_sen_rdrs, 
				SaHpiUint8T rr_num);

SaErrorT populate_thld_low_major(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num);

SaErrorT populate_thld_low_minor(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num);

SaErrorT populate_thld_up_critical(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num);

SaErrorT populate_thld_up_major(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num);

SaErrorT populate_thld_up_minor(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num);

SaErrorT populate_thld_pos_hysteresis(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num);

SaErrorT populate_thld_neg_hysteresis(struct snmp_client_hnd *custom_handle, 
				 SaHpiRdrTypeUnionT *sahpi_sensor_cap, 
				 int num_sen_rdrs, 
				 SaHpiUint8T thd_num);

oid **get_thold_table_oids(int index);

SaErrorT get_sensor_threshold_data(struct snmp_client_hnd *custom_handle,
				   oid **oid_ptr, 
				   oid *indices, 
				   SaHpiSensorReadingT *reading);


	     
