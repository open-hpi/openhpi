/*      -*- linux-c -*-
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <SaHpi.h>
#include <oh_utils.h>

/* debug macros */
#define warn(str) fprintf(stderr,"%s: " str "\n", __FUNCTION__)
#define error(str, e) fprintf(stderr,str ": %s\n", get_error_string(e))

/* Function prototypes */
SaErrorT discover_domain(SaHpiDomainIdT, SaHpiSessionIdT, SaHpiRptEntryT);
const char * get_error_string(SaErrorT);
void display_entity_capabilities(SaHpiCapabilitiesT);
const char * severity2str(SaHpiSeverityT);
const char * rdrtype2str(SaHpiRdrTypeT type);
void rpt_cap2str(SaHpiCapabilitiesT ResourceCapabilities);
const char * ctrldigital2str(SaHpiCtrlStateDigitalT digstate);
const char * get_sensor_type(SaHpiSensorTypeT type);
const char * get_control_type(SaHpiCtrlTypeT type);
const char * get_sensor_category(SaHpiEventCategoryT category);
void list_sel(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id);
void list_rdr(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id);
void display_textbuffer(SaHpiTextBufferT string);
void display_oembuffer(SaHpiUint32T length, SaHpiUint8T *string);
void printreading (SaHpiSensorReadingT reading);
void time2str( SaHpiTimeT time, char *str );
const char *eventtype2str(SaHpiEventTypeT type);
const char *hotswapstate2str(SaHpiHsStateT state);


/**
 * main: main program loop
 *
 *
 *
 * Return value: int 0
 **/
int main(int arc, const char *argv[])
{
        SaErrorT 		err;
        SaHpiVersionT		version;
        SaHpiSessionIdT 	session_id;
        SaHpiRptEntryT		entry;
        //SaHpiEventLogInfoT		Info;
        
        /* Every domain requires a new session */
        /* This example is for one domain, one session */
        err = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &session_id, NULL);
        if (SA_OK != err) {
                error("saHpiSessionOpen", err);
                return err;
        }
        
        err = discover_domain(SAHPI_UNSPECIFIED_DOMAIN_ID, session_id, entry);
        if (SA_OK != err) {
                warn("an error was encountered, results may be incomplete");
        }

		printf("Check RPT again in case something updated\n");
        err = discover_domain(SAHPI_UNSPECIFIED_DOMAIN_ID, session_id, entry);
        if (SA_OK != err) {
                warn("an error was encountered, results may be incomplete");
        }

        saHpiSessionClose(session_id);

	return 0;
}

SaErrorT discover_domain(SaHpiDomainIdT domain_id, SaHpiSessionIdT session_id, SaHpiRptEntryT entry)
{
	
	SaErrorT	err;
	SaHpiEntryIdT	current;
	SaHpiEntryIdT   next;

        err = saHpiDiscover(session_id);
        if (SA_OK != err) {
                error("saHpiResourcesDiscover", err);
                return err;
        }
 	warn("list_resources: discover done");       
        
        warn("Scanning RPT...");
        next = SAHPI_FIRST_ENTRY;
        do {
                char tmp_epath[128];
                current = next;
                err = saHpiRptEntryGet(session_id, current, &next, &entry);
                if (SA_OK != err) {
                        if (current != SAHPI_FIRST_ENTRY) {
                                error("saHpiRptEntryGet", err);
                                return err;
                        } else {
                                warn("Empty RPT\n");
                                break;
                        }
                }

		printf("***Records:\n");
                printf("%s\n", (char *)entry.ResourceTag.Data);
                printf("Entry ID: %d\n", (int) entry.EntryId);
                printf("Resource ID: %d\n", (int) entry.ResourceId);
                printf("Revision: %c\n", entry.ResourceInfo.ResourceRev);
                printf("Version: %c\n", entry.ResourceInfo.SpecificVer);
		printf("Device Support: %d\n", entry.ResourceInfo.DeviceSupport);
		printf("Manufacturer ID: %d\n", (int) entry.ResourceInfo.ManufacturerId);
		printf("Product ID: %d\n", (int) entry.ResourceInfo.ProductId);
		printf("Firmware Major, Minor, Aux: %d %d %d\n", 
		       entry.ResourceInfo.FirmwareMajorRev, 
		       entry.ResourceInfo.FirmwareMinorRev,
		       entry.ResourceInfo.AuxFirmwareRev);
                printf("Severity: %s\n",severity2str(entry.ResourceSeverity));
                
                rpt_cap2str(entry.ResourceCapabilities);
                                
                printf("Entity Path:\n");
                entitypath2string(&entry.ResourceEntity, tmp_epath, sizeof(tmp_epath));
                printf("\t%s\n", tmp_epath);
                printf("\tResourceTag: ");
                       display_textbuffer(entry.ResourceTag);

                if (entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
                        list_rdr(session_id, entry.ResourceId);

                if (entry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)
                        list_sel(session_id, entry.ResourceId);

                printf("\tEntryId: %d\n", next);
        } while (next != SAHPI_LAST_ENTRY);

	printf("SAHPI_LAST_ENTRY\n");
		
        return SA_OK;
}

void list_rdr(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id)
{
        SaErrorT             	err;
        SaHpiEntryIdT        	current_rdr;
        SaHpiEntryIdT        	next_rdr;
        SaHpiRdrT            	rdr;

	SaHpiSensorReadingT	reading;
	SaHpiSensorTypeT	sensor_type;
	SaHpiSensorNumT		sensor_num;
	SaHpiEventCategoryT	category;
	SaHpiSensorThresholdsT	thres; 

	SaHpiCtrlNumT   	ctrl_num;
	SaHpiCtrlStateT 	state;
	SaHpiCtrlTypeT  	ctrl_type;

        printf("RDR Info:\n");
        next_rdr = SAHPI_FIRST_ENTRY;
        do {
                char tmp_epath[128];
                current_rdr = next_rdr;
                err = saHpiRdrGet(session_id, resource_id, current_rdr, 
                                  &next_rdr, &rdr);
                if (SA_OK != err) {
                        if (current_rdr == SAHPI_FIRST_ENTRY)
                                printf("Empty RDR table\n");
                        else
                                error("saHpiRdrGet", err);
                        return;                        
                }
                
                printf("\tRecordId: %x\n", rdr.RecordId);
                printf("\tRdrType: %s\n", rdrtype2str(rdr.RdrType));
                
                if (rdr.RdrType == SAHPI_SENSOR_RDR)
                {			
                        SaErrorT val;
                        
                        sensor_num = rdr.RdrTypeUnion.SensorRec.Num;
                        
                        val = saHpiSensorTypeGet(session_id, resource_id, 
                                                 sensor_num, &sensor_type, 
                                                 &category);
                        
                        printf("\tSensor num: %i\n\tType: %s\n", sensor_num, get_sensor_type(sensor_type)); 
                        printf("\tCategory: %s\n", get_sensor_category(category)); 
                        
                        memset(&reading, 0, sizeof(SaHpiSensorReadingT));
                        
                        err = saHpiSensorReadingGet(session_id, resource_id, sensor_num, &reading, NULL);
                        if (err != SA_OK) {
                                printf("Error=%d reading sensor data {sensor, %d}\n", err, sensor_num);
                                continue;
                        }
                        
#if 0
                        if (reading.ValuesPresent & SAHPI_SRF_RAW) {
                                printf("\tValues Present: RAW\n");
                                printf("\t\tRaw value: %d\n", reading.Raw);
                        }
                        
                        if (reading.ValuesPresent & SAHPI_SRF_INTERPRETED) {
                                printf("\tValues Present: Interpreted\n");
                                printf("\t\t");
                                interpreted2str(reading.Interpreted);
                        }
                        
                        if (reading.ValuesPresent & SAHPI_SRF_EVENT_STATE) {
                                printf("\tValues Present: Event State\n");
                        }
                        
                        if (rdr.RdrTypeUnion.SensorRec.ThresholdDefn.IsThreshold == SAHPI_TRUE) {
                                memset(&thres, 0, sizeof(SaHpiSensorThresholdsT));
                                err = saHpiSensorThresholdsGet(session_id, resource_id, sensor_num, &thres);
                                if (err != SA_OK) {
                                        printf("Error=%d reading sensor thresholds {sensor, %d}\n", err, sensor_num);
                                        continue;
                                }
                                
                                if (thres.LowCritical.ValuesPresent) {
                                        printf("\t\tThreshold:  Low Critical Values\n");
                                        printreading(thres.LowCritical);
                                }
                                if (thres.LowMajor.ValuesPresent) {
                                        printf("\t\tThreshold:  Low Major Values\n");
                                        printreading(thres.LowMajor);
                                }
                                if (thres.LowMinor.ValuesPresent) {
                                        printf("\t\tThreshold:  Low Minor Values\n");
                                        printreading(thres.LowMinor);
                                }
                                if (thres.UpCritical.ValuesPresent) {
                                        printf("\t\tThreshold:  Up Critical Values\n");
                                        printreading(thres.UpCritical);
                                }
                                if (thres.UpMajor.ValuesPresent) {
                                        printf("\t\tThreshold:  Up Major Values\n");
                                        printreading(thres.UpMajor);
                                }
                                if (thres.UpMinor.ValuesPresent) {
                                        printf("\t\tThreshold:  Up Minor Values\n");
                                        printreading(thres.UpMinor);
                                }
                                if (thres.PosThdHysteresis.ValuesPresent) {
                                        printf("\t\tThreshold:  Pos Threshold Hysteresis Values\n");
                                        printreading(thres.PosThdHysteresis);
                                }
                                if (thres.NegThdHysteresis.ValuesPresent) {
					printf("\t\tThreshold:  Neg Threshold Hysteresis Values\n");
					printreading(thres.NegThdHysteresis);
                                }
                        }
#endif
                }
                
                if (rdr.RdrType == SAHPI_CTRL_RDR)
                {    
                        ctrl_num = rdr.RdrTypeUnion.CtrlRec.Num;
                        err = saHpiControlTypeGet(session_id, resource_id, ctrl_num, &ctrl_type);
                        if (err != SA_OK) {
                                printf("Error=%d reading control type {control, %d}\n", err, ctrl_num);
                                continue;
                        }
                        printf("\tControl num: %i\n\tType: %s\n", ctrl_num, get_control_type(ctrl_type)); 
                        
                        err = saHpiControlGet(session_id, resource_id, ctrl_num, &state);
                        if (err != SA_OK) {
                                printf("Error=%d reading control state {control, %d}\n", err, ctrl_num, NULL);
                                continue;
                        }
                        if (ctrl_type != state.Type) {
                                printf("Control Type mismatch between saHpiControlTypeGet=%d and saHpiControlStateGet = %d\n", 
                                       ctrl_type, state.Type);
                        }
                        
                        switch (state.Type) {
                        case SAHPI_CTRL_TYPE_DIGITAL:
                                printf("\t\tControl Digital State: %s\n", 
                                       ctrldigital2str(state.StateUnion.Digital));
                                break;
                        case SAHPI_CTRL_TYPE_DISCRETE:
                                printf("\t\tControl Discrete State: %x\n", state.StateUnion.Discrete);
                                break;
                        case SAHPI_CTRL_TYPE_ANALOG:
                                printf("\t\tControl Analog State: %x\n", state.StateUnion.Analog);
                                break;
                        case SAHPI_CTRL_TYPE_STREAM:
                                printf("\t\tControl Stream Repeat: %d\n", state.StateUnion.Stream.Repeat);
                                printf("\t\tControl Stream Data: ");
                                display_oembuffer(state.StateUnion.Stream.StreamLength, state.StateUnion.Stream.Stream);
                                break;
                        case SAHPI_CTRL_TYPE_TEXT:
                                printf("\t\tControl Text Line Num: %c\n", state.StateUnion.Text.Line);
                                display_textbuffer(state.StateUnion.Text.Text);
                                break;
                        case SAHPI_CTRL_TYPE_OEM:
                                printf("\t\tControl OEM Manufacturer: %d\n", state.StateUnion.Oem.MId);
                                printf("\t\tControl OEM Data: ");
                                display_oembuffer((SaHpiUint32T)state.StateUnion.Oem.BodyLength, 
                                                  state.StateUnion.Oem.Body);
                                break;
                        default:
                                printf("\t\tInvalid control type (%d) from saHpiControlStateGet\n", 
                                       state.Type);
                        }
                }
                
#if 0
                if (rdr.RdrType == SAHPI_INVENTORY_RDR)
                {
                        l_eirid = rdr.RdrTypeUnion.InventoryRec.EirId;

                        l_inventdata = (SaHpiInventoryDataT *)g_malloc(l_inventsize);
                        err = saHpiEntityInventoryDataRead(session_id, resource_id,
                                                            l_eirid, l_inventsize,
                                                            l_inventdata, &l_actualsize);

			if (err != SA_OK) {
				printf("Error=%d reading inventory type {EirId, %d}\n", err, l_eirid);
				continue;
			} else if (l_inventdata->Validity ==  SAHPI_INVENT_DATA_VALID) {
                        	printf("\tFound Inventory RDR with EirId: %x\n", l_eirid);
				printf("\tRDR l_inventsize = %d, actualsize = %d\n", l_inventsize, l_actualsize);
				switch (l_inventdata->DataRecords[0]->RecordType)
				{
					case SAHPI_INVENT_RECTYPE_INTERNAL_USE:
						printf( "Internal Use\n");
						break;
					case SAHPI_INVENT_RECTYPE_PRODUCT_INFO:
						printf( "Product Info\n");
						break;
					case SAHPI_INVENT_RECTYPE_CHASSIS_INFO:
						printf( "Chassis Info\n");
						break;
					case SAHPI_INVENT_RECTYPE_BOARD_INFO:
						printf( "Board Info\n");
						break;
					case SAHPI_INVENT_RECTYPE_OEM:
						printf( "OEM Record\n");
						break;
					default:
						printf(" Invalid Invent Rec Type =%x\n",
								l_inventdata->DataRecords[0]->RecordType);
						break;
				} 

			}

                        g_free(l_inventdata);
#endif
                }

                printf("\tEntity: \n");
                entitypath2string(&rdr.Entity, tmp_epath, sizeof(tmp_epath));
                printf("\t\t%s\n", tmp_epath);
                printf("\tIdString: ");
                display_textbuffer(rdr.IdString);
                printf("\n"); /* Produce blank line between rdrs. */
        }while(next_rdr != SAHPI_LAST_ENTRY);
}

/**
 * time2str:
 * @time: HPI time
 * @str: time string
 *
 * Convert SaHpiTimeT into a string.
 **/

void time2str(SaHpiTimeT time, char *str)
{
        if (time == SAHPI_TIME_UNSPECIFIED) {
                strcpy( str, "SAHPI_TIME_UNSPECIFIED" );
                return;
        }

        int nano = time % 1000000000;

        time /= 1000000000;
        time_t t = (time_t)time;

        struct tm tm;
        localtime_r(&t, &tm);

        char s[30];
        strftime(s, 30, "%Y.%m.%d %H:%M:%S", &tm);

        sprintf(str, "%s.%09d", s, nano);
}


/**
 * list_sel:
 * @session_id: session id
 * @resource_id: resource id with capability SAHPI_CAPABILITY_EVENT_LOG
 *
 * Print out information about event log.
 **/

void list_sel(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id)
{
        char str[256];

        printf("EVENT_LOG Info:\n");

        SaHpiEventLogInfoT info;        
        SaErrorT rv = saHpiEventLogInfoGet(session_id, resource_id, &info);

        if (rv != SA_OK) {
                printf( "Error=%d reading EVENT_LOG info\n", rv);
                return;
        }

        printf("\tEntries in EVENT_LOG: %d\n", info.Entries);
        printf("\tSize: %d\n", info.Size);

        time2str(info.UpdateTimestamp, str);
        printf("\tUpdateTimestamp: %s\n", str);

        time2str(info.CurrentTime, str);
        printf("\tCurrentTime    : %s\n", str);
        printf("\tEnabled: %s\n", info.Enabled ? "true" : "false");
        printf("\tOverflowFlag: %s\n", info.OverflowFlag ? "true" : "false");
        printf("\tOverflowAction: " );

        switch(info.OverflowAction) {
        case SAHPI_EL_OVERFLOW_DROP:
                printf("SAHPI_SEL_OVERFLOW_DROP\n");
                break;

        case SAHPI_EL_OVERFLOW_OVERWRITE:
                printf("SAHPI_EL_OVERFLOW_OVERWRITE\n");
                break;

        case SAHPI_EL_OVERFLOW_WRITELAST:
                printf("SAHPI_SEL_OVERFLOW_WRITELAST\n");
                break;

        default:
                printf("unknown(0x%x)\n", info.OverflowAction);
                break;
        }

        printf("\tDeleteEntrySupported: %s\n", info.DeleteEntrySupported ? "true" : "false" );

		if (info.Entries == 0)
				return;
		//else 
        // read sel records
        SaHpiEventLogEntryIdT current = SAHPI_OLDEST_ENTRY;

        do {
                SaHpiEventLogEntryIdT prev;
                SaHpiEventLogEntryIdT next;
                SaHpiEventLogEntryT   entry;
                SaHpiRdrT        rdr;
                SaHpiRptEntryT   res;

                rv = saHpiEventLogEntryGet(session_id, resource_id, current, &prev, &next, &entry, &rdr, &res);

                if (rv != SA_OK) {
                        printf( "Error=%d reading SEL entry %d\n", rv, current);
                        return;
                }

                printf("\t\tEntry %d, prev %d, next %d\n", entry.EntryId, prev, next);
                time2str(entry.Timestamp, str);
                printf("\t\t\tTimestamp:            %s\n", str);

                SaHpiRptEntryT rres;

                rv = saHpiRptEntryGetByResourceId(session_id, entry.Event.Source, &rres );

                if ( rv != SA_OK )
                printf("\t\t\tSource:               unknown\n" );
                else {
                        entitypath2string( &rres.ResourceEntity, str, sizeof(str));
                printf("\t\t\tSource:               %s\n", str );
                }

                printf("\t\t\tEventType:            %s\n", eventtype2str(entry.Event.EventType));
                time2str(entry.Timestamp, str);
                printf("\t\t\tEvent timestamp:      %s\n", str);
                printf("\t\t\tSeverity:             %s\n", severity2str( entry.Event.Severity ) );

                switch(entry.Event.EventType) {
                case SAHPI_ET_SENSOR:
                        {
                                SaHpiSensorEventT *se = &entry.Event.EventDataUnion.SensorEvent;
                printf("\t\t\tSensorNum:            %d\n", se->SensorNum );
                printf("\t\t\tSensorType:           %s\n", get_sensor_type(se->SensorType ) );
                printf("\t\t\tEventCategory:        %s\n", get_sensor_category( se->EventCategory ) );
                printf("\t\t\tAssertion:            %s\n", se->Assertion ? "TRUE" : "FALSE" );
                        }

                        break;

                case SAHPI_ET_HOTSWAP:
                        {
                                SaHpiHotSwapEventT *he = &entry.Event.EventDataUnion.HotSwapEvent;
                printf("\t\t\tHotSwapState:         %s\n",
                                      hotswapstate2str( he->HotSwapState ) );
                printf("\t\t\tPreviousHotSwapState: %s\n",
                                        hotswapstate2str( he->PreviousHotSwapState ) );
                        }

                        break;

                case SAHPI_ET_WATCHDOG:
                        break;

                case SAHPI_ET_OEM:
                        break;

                case SAHPI_ET_USER:
                        break;
                }
                
                current = next;
        } while(current != SAHPI_NO_MORE_ENTRIES);
}


void display_oembuffer(SaHpiUint32T length, SaHpiUint8T *string)
{
        int i;

	for (i = 0; i < length; i++) {
		printf("%c", string[i]);
	}

        printf("\n");
}


void display_textbuffer(SaHpiTextBufferT string)
{
        int i;
        switch(string.DataType) {
        case SAHPI_TL_TYPE_BINARY:
                for (i = 0; i < string.DataLength; i++)
                        printf("%x", string.Data[i]);
                break;
        case SAHPI_TL_TYPE_BCDPLUS:
                for (i = 0; i < string.DataLength; i++)
                        printf("%c", string.Data[i]);
                break;
        case SAHPI_TL_TYPE_ASCII6:
                for (i = 0; i < string.DataLength; i++)
                        printf("%c", string.Data[i]);
                break;
        case SAHPI_TL_TYPE_LANGUAGE:
                for (i = 0; i < string.DataLength; i++)
                        printf("%c", string.Data[i]);
                break;
        default:
                printf("Invalid string data type=%d", string.DataType);
        }
        printf("\n");
}

void rpt_cap2str (SaHpiCapabilitiesT ResourceCapabilities)
{
        printf("\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_DOMAIN)
                printf("\tSAHPI_CAPABILITY_DOMAIN\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_RESOURCE)
                printf("\tSAHPI_CAPABILITY_RESOURCE\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_SEL)
                printf("\tSAHPI_CAPABILITY_SEL\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)
                printf("\tSAHPI_CAPABILITY_EVT_DEASSERTS\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_AGGREGATE_STATUS)
                printf("\tSAHPI_CAPABILITY_AGGREGATE_STATUS\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION)
                printf("\tSAHPI_CAPABILITY_CONFIGURATION\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)
                printf("\tSAHPI_CAPABILITY_MANAGED_HOTSWAP\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)
                printf("\tSAHPI_CAPABILITY_WATCHDOG\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)
                printf("\tSAHPI_CAPABILITY_CONTROL\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_FRU)
                printf("\tSAHPI_CAPABILITY_FRU\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)
                printf("\tSAHPI_CAPABILITY_INVENTORY_DATA\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_RDR)
                printf("\tSAHPI_CAPABILITY_RDR\n");
        if(ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)
                printf("\tSAHPI_CAPABILITY_SENSOR\n");
        
}



