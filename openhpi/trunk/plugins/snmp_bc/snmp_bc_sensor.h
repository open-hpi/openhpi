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
 * Author(s):
 *      Steve Sherman <sesherman@us.ibm.com>
 */

#ifndef __SNMP_BC_SENSOR_H
#define __SNMP_BC_SENSOR_H

/* FIXME:: ?? Can we pass handle in; instead of hnd - better error checking */
SaErrorT snmp_bc_get_sensor_reading(void *hnd,
				    SaHpiResourceIdT id,
				    SaHpiSensorNumT num,
				    SaHpiSensorReadingT *data,
				    SaHpiEventStateT *state);

SaErrorT snmp_bc_get_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       SaHpiSensorThresholdsT *thres);

SaErrorT snmp_bc_set_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       const SaHpiSensorThresholdsT *thres);

SaErrorT snmp_bc_get_sensor_enable(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiSensorNumT sensor_num,
				   SaHpiBoolT *enable);

SaErrorT snmp_bc_set_sensor_enable(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiSensorNumT sensor_num,
				   const SaHpiBoolT enable);

SaErrorT snmp_bc_get_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT id,
					 SaHpiSensorNumT num,
					 SaHpiBoolT *enables);

SaErrorT snmp_bc_set_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT id,
					 SaHpiSensorNumT num,
					 const SaHpiBoolT enables);
					  
SaErrorT snmp_bc_get_sensor_event_masks(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSensorNumT sensor_num,
					SaHpiEventStateT *AssertEventMask,
					SaHpiEventStateT *DeassertEventMask);

SaErrorT snmp_bc_set_sensor_event_masks(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSensorNumT sensor_num,
					const SaHpiEventStateT AssertEventMask,
					const SaHpiEventStateT DeassertEventMask);
/* FIXME:: Could be static */
SaErrorT snmp_bc_determine_sensor_eventstates(void *hnd,
					      SaHpiResourceIdT id,
					      SaHpiSensorNumT num,
					      SaHpiSensorReadingT *reading,
					      SaHpiEventStateT *state);
#endif
