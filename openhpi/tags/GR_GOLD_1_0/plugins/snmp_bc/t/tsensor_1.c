/* -*- linux-c -*-
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
 *     Chris Chia <cchia@users.sf.net>
 */

/**********************************************************************
 * NOTES: 
 * Using pre-defined sensor information in snmp_bc_chassis_sensors[]
 * in bc_resources.c.
 ***********************************************************************/

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <SaHpi.h>
#include <snmp_util.h>
#include <bc_resources.h>
#include <sim_resources.h>
#include <oh_error.h>

static  SaErrorT  TestSensor(SaHpiSessionIdT, SaHpiResourceIdT, SaHpiSensorRecT *);
char *rtypes[5] = {"None    ", "Control ", "Sensor  ", "Invent  ", "Watchdog"};
#define TEST_SENSOR_STR "  50.00 Centigrade"
#define TEST_SENSOR_STR_VAL  50.0
//#define DBG_PRINT
#ifdef DBG_PRINT 
#define  DBG    printf
#define PrintSensor( SensorRead )                                               \
    DBG("\t SensorReadingFormats: %d,\t Raw: %d\n",                             \
           SensorRead.ValuesPresent, SensorRead.Raw);                           \
    DBG("\t Interpreted..Type: %d,\t\t Interpreted.Value: %5.2f\n",             \
           SensorRead.Interpreted.Type, SensorRead.Interpreted.Value.SensorFloat32);\
    DBG("\t EventStatus.SensorStatus: %d,\t EventStatus.EventStatus: %d\n",     \
           SensorRead.EventStatus.SensorStatus, SensorRead.EventStatus.EventStatus);                    
#else
#define DBG(format, ...)
#define PrintSensor( SensorRead )
#endif

int main(int argc, char **argv)
{
        SaErrorT               rc;
	SaHpiEntryIdT          rptid, next_rptid;
        SaHpiRdrT              rdr;
	SaHpiRptEntryT         rpt;
	SaHpiResourceIdT       chassis_rid=0;
        SaHpiSessionIdT        sessionid;
        SaHpiVersionT          hpiVer;

	/* Setup Infra-structure */
        rc = saHpiInitialize(&hpiVer);
        if (rc != SA_OK) {
                DBG("Error! saHpiInitialize: err=%d\n", rc);
                return -1;
        }
        rc = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &sessionid, NULL);
        if (rc != SA_OK) {
	  DBG("Error! saHpiSessionOpen: rc=%d\n", rc);
	  return -1;
        }
        rc = saHpiResourcesDiscover(sessionid);
        if (rc != SA_OK) {
	  DBG("Error! saHpiResourcesDiscover: rc=%d\n", rc);
	  return -1;
        }
	/* Find first SEL capable Resource - assume its the chassis */
	rptid = SAHPI_FIRST_ENTRY;
	while ((rc == SA_OK) && (rptid != SAHPI_LAST_ENTRY)) {
		rc = saHpiRptEntryGet(sessionid, rptid, &next_rptid, &rpt);
		if (rc != SA_OK) {
			DBG("Error! saHpiRptEntryGet: rc=%d\n", rc);
			return -1;
		}
		if ((rpt.ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
			chassis_rid = rpt.ResourceId;
			break;
		}
		else {
			rptid = next_rptid;
			continue;
		}
	}
	if (chassis_rid == 0) {
		DBG("Error! saHpiRptEntryGet couldn't find SEL RID\n");
		return -1;
	}
        /* Walk the RPT entry RDR list and do sensor test */
        rptid = SAHPI_FIRST_ENTRY;
        rpt.ResourceTag.Data[rpt.ResourceTag.DataLength] = 0;
        DBG("RPTEntry[%d] tag: %s\n", chassis_rid, rpt.ResourceTag.Data);
        while ((rc == SA_OK) && (rptid != SAHPI_LAST_ENTRY)) {
                rc = saHpiRdrGet(sessionid, chassis_rid, rptid, &next_rptid, &rdr);
                if (rc == SA_OK) {
                        if (rdr.RdrType == SAHPI_SENSOR_RDR) {
                                if((rdr.RdrTypeUnion.SensorRec.DataFormat.ReadingFormats &
                                    SAHPI_SRF_RAW) ||
                                   (rdr.RdrTypeUnion.SensorRec.DataFormat.ReadingFormats &
                                    SAHPI_SRF_INTERPRETED)) {
                                        rdr.IdString.Data[rdr.IdString.DataLength] = 0;
                                        DBG("    RDR[%6d]: %s %s\n", rdr.RecordId,
                                                rtypes[rdr.RdrType], rdr.IdString.Data);
                                        rc = TestSensor(sessionid, chassis_rid,
                                                   &rdr.RdrTypeUnion.SensorRec);
                                        if (rc != SA_OK) {
                                                DBG("Error: TestSensor failed rc %d\n",rc);
                                                return -1;
                                        }
                                }
                        }
                        rptid = next_rptid;
                } 
                else {
                        DBG("Error: saHpiRdrGet failed with rc=%d\n", rc);
        	        return -1;
                }
        }
        rc = saHpiSessionClose(sessionid);
        if (rc != SA_OK) {
	  DBG("Error! saHpiSessionClose: rc=%d\n", rc);
	  return -1;
        }
        rc = saHpiFinalize();
        if (rc != SA_OK) {
	  DBG("Error! saHpiFinalize: rc=%d\n", rc);
	  return -1;
        }
        return 0;
}


static  SaErrorT  TestSensor(SaHpiSessionIdT SessionID, SaHpiResourceIdT ResourceID,
                             SaHpiSensorRecT *SensorRec)
{
        uint                   i;
        SaHpiSensorNumT        SensorNum;
        SaHpiUint32T           SensorTestVal;
        SaErrorT               rc;
        char                  *SensorOID;
	char                  *hash_key;
	SnmpMibInfoT          *hash_value;
        SaHpiSensorReadingT    SensorRead;

        for (i=0; ; i++) {
                SensorNum = snmp_bc_chassis_sensors[i].sensor.Num;
                if (SensorNum == 0) {
                        DBG("Error: can\'t find sensor number %d in table\n",
                             SensorRec->Num);
                        return -1;
                }
                if (SensorNum == SensorRec->Num) {
                        DBG("found OID %s at sensor table entry %d\n",
                             snmp_bc_chassis_sensors[i].bc_sensor_info.mib.oid, i);
                        SensorOID = 
                          g_strdup(snmp_bc_chassis_sensors[i].bc_sensor_info.mib.oid);
                        if (!SensorOID) {
                                DBG("Error: copying Sensor OID failed\n");
                                return -1;
                        }
                        break;
                }
        }
	/* If test OID not already in sim hash table; create it */
	if (!g_hash_table_lookup_extended(sim_hash, SensorOID,
		        (gpointer)&hash_key, (gpointer)&hash_value)) {

		hash_key = g_strdup(SensorOID);
		if (!hash_key) {
			DBG("Error: oid key=%s malloc failed\n", SensorOID);
			return -1;
		}
		
		hash_value = g_malloc0(sizeof(SnmpMibInfoT));
		if (!hash_value) {
			DBG("Error: hash value for oid=%s malloc failed\n", SensorOID);
			return -1;
		}
                g_hash_table_insert(sim_hash, hash_key, hash_value);
	}

        if (SensorRec->DataFormat.ReadingFormats & SAHPI_SRF_RAW) {
                if (SensorRec->DataFormat.Range.Max.ValuesPresent) {
                        SensorTestVal = SensorRec->DataFormat.Range.Max.Raw;
                  	hash_value->value.integer = SensorTestVal;
                	g_hash_table_insert(sim_hash, hash_key, hash_value);
                        rc = saHpiSensorReadingGet(SessionID, ResourceID, 
                                                   SensorRec->Num, &SensorRead);
                	if (rc != SA_OK) {
                		DBG("Error! saHpiSensorReadingGet failed, rc=%d\n", rc);
                		return -1;
                        }
                        else {
                                PrintSensor(SensorRead);
                          	if (!(SensorRead.Raw == SensorTestVal)) {
                          		DBG("Error: sensor value read %d != value set %d\n",
                                            SensorRead.Raw, SensorTestVal);
                          		return -1;
                          	}
                        }

                        SensorTestVal = SensorRec->DataFormat.Range.Max.Raw + 1;
                  	hash_value->value.integer = SensorTestVal;
                	g_hash_table_insert(sim_hash, hash_key, hash_value);
                        rc = saHpiSensorReadingGet(SessionID, ResourceID, 
                                                   SensorRec->Num, &SensorRead);
                	if (rc == SA_OK) {
                		DBG("Error! saHpiSensorReadingGet did not fail with an");
                		DBG(" sensor value too large\n"); 
                		return -1;
                        }
                }
                if (SensorRec->DataFormat.Range.Min.ValuesPresent) {
                        SensorTestVal = SensorRec->DataFormat.Range.Min.Raw;
                  	hash_value->value.integer = SensorTestVal;
                	g_hash_table_insert(sim_hash, hash_key, hash_value);
                        rc = saHpiSensorReadingGet(SessionID, ResourceID, 
                                                   SensorRec->Num, &SensorRead);
                	if (rc != SA_OK) {
                		DBG("Error! saHpiSensorReadingGet failed, rc=%d\n", rc);
                		return -1;
                        }
                        else {
                                PrintSensor(SensorRead);
                          	if (!(SensorRead.Raw == SensorTestVal)) {
                          		DBG("Error: sensor value read %d != value set %d\n",
                                            SensorRead.Raw, SensorTestVal);
                          		return -1;
                          	}
                        }
                        SensorTestVal = SensorRec->DataFormat.Range.Min.Raw - 1;
                  	hash_value->value.integer = SensorTestVal;
                	g_hash_table_insert(sim_hash, hash_key, hash_value);
                        rc = saHpiSensorReadingGet(SessionID, ResourceID, 
                                                   SensorRec->Num, &SensorRead);
                	if (rc == SA_OK) {
                                PrintSensor(SensorRead);
                		DBG("Error! saHpiSensorReadingGet did not fail with an");
                		DBG(" sensor value too small\n"); 
                		return -1;
                        }
                }
                if (SensorRec->DataFormat.Range.Nominal.ValuesPresent) {
                        SensorTestVal = SensorRec->DataFormat.Range.Nominal.Raw;
                  	hash_value->value.integer = SensorTestVal;
                	g_hash_table_insert(sim_hash, hash_key, hash_value);
                        rc = saHpiSensorReadingGet(SessionID, ResourceID, 
                                                   SensorRec->Num, &SensorRead);
                	if (rc != SA_OK) {
                		DBG("Error! saHpiSensorReadingGet failed, rc=%d\n", rc);
                		return -1;
                        }
                        else {
                                PrintSensor(SensorRead);
                          	if (!(SensorRead.Raw == SensorTestVal)) {
                          		DBG("Error: sensor value read %d != value set %d\n",
                                            SensorRead.Raw, SensorTestVal);
                          		return -1;
                          	}
                        }
                }
        	
        }
        if (SensorRec->DataFormat.ReadingFormats & SAHPI_SRF_INTERPRETED) {
        	strcpy(hash_value->value.string, TEST_SENSOR_STR);
        	g_hash_table_insert(sim_hash, hash_key, hash_value);
        
                rc = saHpiSensorReadingGet(SessionID, ResourceID, SensorRec->Num, &SensorRead);
        	if (rc != SA_OK) {
        		DBG("Error! saHpiSensorReadingGet failed, rc = %d\n", rc);
        		return -1;
                }
        	
                PrintSensor(SensorRead);
        	if ((SensorRead.Interpreted.Value.SensorFloat32 != TEST_SENSOR_STR_VAL)) {
        		DBG("Error! value read %5.2f != value set %5.2f\n",
                             SensorRead.Interpreted.Value.SensorFloat32, TEST_SENSOR_STR_VAL);
        		return -1;
        	}
        }
        return SA_OK;	
}
