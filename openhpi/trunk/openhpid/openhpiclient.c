/*
 * hpi library code
 *
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#include "openhpiclient.h"
#include "marshal_hpi.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


#ifdef dOpenHpiClientWithConfig
static cOpenHpiClientConf config_data = 
{
  .m_num_sessions = 0,
  .m_client_connection = 0,
  .m_initialize = 0
};

static cOpenHpiClientConf *config = &config_data;

static cConfigEntry config_table[] =
{
  {
    .m_name = "daemon",
    .m_type = eConfigTypeString,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_server ),
    .m_default.m_string = "localhost"
  },
  {
    .m_name = "port",
    .m_type = eConfigTypeInt,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_port ),
    .m_default.m_int = dDefaultDaemonPort
  },
  {
    .m_name = "debug",
    .m_type = eConfigTypeInt,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_debug ),
    .m_default.m_int = 0xffff
  },
  {
    .m_type = eConfigTypeUnknown
  }
};
#endif


static int
OpenConnection( cOpenHpiClientConf *c )
{
  assert( c->m_client_connection == 0 );

#ifdef dOpenHpiClientWithConfig
  char *openhpi_client_conf = getenv( "OPENHPICLIENT_CONF" );

  if ( openhpi_client_conf == 0 )
       openhpi_client_conf = OH_CLIENT_DEFAULT_CONF;

  ConfigRead( openhpi_client_conf, config_table, c );
#endif

  c->m_client_connection = ClientConnectionOpen( c->m_server, c->m_port );

  if ( !c->m_client_connection )
       return 0;

  return 1;
}


static void
CloseConnection( cOpenHpiClientConf *c )
{
  assert( c->m_client_connection );
  ClientConnectionClose( c->m_client_connection );
  c->m_client_connection = 0;
}


#define CheckSession() \
  do                                            \
     {                                          \
       if ( !config->m_client_connection )      \
	    return SA_ERR_HPI_INVALID_SESSION;  \
     }                                          \
  while( 0 )


#define PreMarshal(id)   \
  cMessageHeader request_header; \
  cMessageHeader reply_header; \
  void *request;         \
  void *reply;           \
  SaErrorT err;          \
                         \
  cHpiMarshal *hm = hm = HpiMarshalFind( id ); \
  assert( hm );          \
                         \
  MessageHeaderInit( &request_header, eMhRequest, 0, id, hm->m_request_len ); \
                         \
  request = malloc( hm->m_request_len )


#define Send()       \
  do                 \
     {               \
       int r = ClientConnectionWriteMsg( config->m_client_connection, &request_header, \
                                         request, &reply_header, &reply ); \
                     \
       free( request ); \
                     \
       if ( r )      \
	    return SA_ERR_HPI_INITIALIZING; \
     }               \
  while( 0 )


#define PostMarshal() \
  if ( reply )        \
       free( reply )




SaErrorT SAHPI_API
dOpenHpiClientFunction(Initialize)dOpenHpiClientParam( SAHPI_OUT SaHpiVersionT *HpiImplVersion )
{
  if ( config->m_initialize )
       return SA_ERR_HPI_DUPLICATE;

  config->m_initialize = 1;

  *HpiImplVersion = SAHPI_INTERFACE_VERSION;

  return SA_OK;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(Finalize)dOpenHpiClientParam()
{
  if ( config->m_num_sessions )
       return SA_ERR_HPI_BUSY;

  config->m_initialize = 0;

  return SA_OK;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(SessionOpen)dOpenHpiClientParam( SAHPI_IN  SaHpiDomainIdT   DomainId,
		  SAHPI_OUT SaHpiSessionIdT *SessionId,
		  SAHPI_IN  void            *SecurityParams )
{
  if ( config->m_num_sessions == 0 )
     {
       if ( !OpenConnection( config ) )
	    return SA_ERR_HPI_INITIALIZING;
     }

  config->m_num_sessions++;

  PreMarshal( eFsaHpiSessionOpen );

  HpiMarshalRequest1( hm, request, &DomainId );

  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, SessionId );

  PostMarshal();

  if ( err != SA_OK )
     {
       config->m_num_sessions--;
   
       assert( config->m_num_sessions >= 0 );

       if ( config->m_num_sessions < 0 )
	    config->m_num_sessions = 0;
  
       if ( config->m_num_sessions == 0 )
	    CloseConnection( config );
     }

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(SessionClose)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId )
{
  CheckSession();

  PreMarshal( eFsaHpiSessionClose );
  HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  config->m_num_sessions--;

  assert( config->m_num_sessions >= 0 );

  if ( config->m_num_sessions < 0 )
       config->m_num_sessions = 0;

  if ( config->m_num_sessions == 0 )
       CloseConnection( config );

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ResourcesDiscover)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId )
{
  CheckSession();

  PreMarshal( eFsaHpiResourcesDiscover );
  HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(RptInfoGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId,
		 SAHPI_OUT SaHpiRptInfoT *RptInfo )
{
  CheckSession();

  PreMarshal( eFsaHpiRptInfoGet );
  HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, RptInfo );

  PostMarshal();

  return err;  
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(RptEntryGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId,
		  SAHPI_IN SaHpiEntryIdT EntryId,
		  SAHPI_OUT SaHpiEntryIdT *NextEntryId,
		  SAHPI_OUT SaHpiRptEntryT *RptEntry )
{
  CheckSession();

  PreMarshal( eFsaHpiRptEntryGet );
  HpiMarshalRequest2( hm, request, &SessionId, &EntryId );

  Send();

  HpiDemarshalReply2( reply_header.m_flags & dMhEndianBit, hm, reply,
                      &err, NextEntryId, RptEntry );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(RptEntryGetByResourceId)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
			      SAHPI_IN  SaHpiResourceIdT ResourceId,
			      SAHPI_OUT SaHpiRptEntryT   *RptEntry )
{
  CheckSession();

  PreMarshal( eFsaHpiRptEntryGetByResourceId );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, RptEntry );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(ResourceSeveritySet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
			  SAHPI_IN  SaHpiResourceIdT ResourceId,
			  SAHPI_IN  SaHpiSeverityT   Severity )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceSeveritySet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Severity);
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(ResourceTagSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT    SessionId,
		     SAHPI_IN  SaHpiResourceIdT   ResourceId,
		     SAHPI_IN  SaHpiTextBufferT   *ResourceTag )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceTagSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, ResourceTag );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ResourceIdGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
		    SAHPI_OUT SaHpiResourceIdT  *ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceIdGet );

  HpiMarshalRequest1( hm, request, &SessionId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, ResourceId );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EntitySchemaGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT     SessionId,
		      SAHPI_OUT SaHpiUint32T        *SchemaId )
{
  CheckSession();

  PreMarshal( eFsaHpiEntitySchemaGet );

  HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, SchemaId );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogInfoGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
		      SAHPI_IN  SaHpiResourceIdT ResourceId,
		      SAHPI_OUT SaHpiSelInfoT    *Info )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogInfoGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Info );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogEntryGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT     SessionId,
		       SAHPI_IN    SaHpiResourceIdT    ResourceId,
		       SAHPI_IN    SaHpiSelEntryIdT    EntryId,
		       SAHPI_OUT   SaHpiSelEntryIdT    *PrevEntryId,
		       SAHPI_OUT   SaHpiSelEntryIdT    *NextEntryId,
		       SAHPI_OUT   SaHpiSelEntryT      *EventLogEntry,
		       SAHPI_INOUT SaHpiRdrT           *Rdr,
		       SAHPI_INOUT SaHpiRptEntryT      *RptEntry )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogEntryGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &EntryId );

  Send();

  SaHpiRdrT rdr;
  SaHpiRptEntryT rpt_entry;

  HpiDemarshalReply5( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		     PrevEntryId, NextEntryId, EventLogEntry, 
		     &rdr, &rpt_entry );

  if ( Rdr )
       *Rdr = rdr;

  if ( RptEntry )
       *RptEntry = rpt_entry;

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogEntryAdd)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT      SessionId,
		       SAHPI_IN SaHpiResourceIdT     ResourceId,
		       SAHPI_IN SaHpiSelEntryT       *EvtEntry )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogEntryAdd );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, EvtEntry );

  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, EvtEntry );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogEntryDelete)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT      SessionId,
			  SAHPI_IN SaHpiResourceIdT     ResourceId,
			  SAHPI_IN SaHpiSelEntryIdT     EntryId )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogEntryDelete );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &EntryId );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(EventLogClear)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
		    SAHPI_IN  SaHpiResourceIdT  ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogClear );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogTimeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
		      SAHPI_IN  SaHpiResourceIdT ResourceId,
		      SAHPI_OUT SaHpiTimeT       *Time )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogTimeGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Time );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogTimeSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
		      SAHPI_IN SaHpiResourceIdT  ResourceId,
		      SAHPI_IN SaHpiTimeT        Time )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogTimeSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Time );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
		       SAHPI_IN  SaHpiResourceIdT ResourceId,
		       SAHPI_OUT SaHpiBoolT       *Enable )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogStateGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Enable );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventLogStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
		       SAHPI_IN SaHpiResourceIdT  ResourceId,
		       SAHPI_IN SaHpiBoolT        Enable )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogStateSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Enable );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(Subscribe)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
		SAHPI_IN SaHpiBoolT       ProvideActiveAlarms )
{
  CheckSession();

  PreMarshal( eFsaHpiSubscribe );

  HpiMarshalRequest2( hm, request, &SessionId, &ProvideActiveAlarms );

  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(Unsubscribe)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId )
{
  CheckSession();

  PreMarshal( eFsaHpiUnsubscribe );

  HpiMarshalRequest1( hm, request, &SessionId );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EventGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT      SessionId,
	       SAHPI_IN    SaHpiTimeoutT        Timeout,
	       SAHPI_OUT   SaHpiEventT          *Event,
	       SAHPI_INOUT SaHpiRdrT            *Rdr,
	       SAHPI_INOUT SaHpiRptEntryT       *RptEntry )
{
  CheckSession();

  PreMarshal( eFsaHpiEventGet );

  HpiMarshalRequest2( hm, request, &SessionId, &Timeout );
  
  Send();

  SaHpiRdrT      rdr;
  SaHpiRptEntryT rpt_entry;

  HpiDemarshalReply3( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      Event, &rdr, &rpt_entry );

  if ( Rdr )
       *Rdr = rdr;
  
  if ( RptEntry )
       *RptEntry = rpt_entry;

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(RdrGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT       SessionId,
	     SAHPI_IN  SaHpiResourceIdT      ResourceId,
	     SAHPI_IN  SaHpiEntryIdT         EntryId,
	     SAHPI_OUT SaHpiEntryIdT         *NextEntryId,
	     SAHPI_OUT SaHpiRdrT             *Rdr )
{
  CheckSession();

  PreMarshal( eFsaHpiRdrGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &EntryId );
  
  Send();

  HpiDemarshalReply2( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		     NextEntryId, Rdr );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(SensorReadingGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
		       SAHPI_IN  SaHpiResourceIdT     ResourceId,
		       SAHPI_IN  SaHpiSensorNumT      SensorNum,
		       SAHPI_OUT SaHpiSensorReadingT  *Reading )
{
  CheckSession();

  PreMarshal( eFsaHpiSensorReadingGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		     Reading);

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(SensorReadingConvert)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
			   SAHPI_IN  SaHpiResourceIdT     ResourceId,
			   SAHPI_IN  SaHpiSensorNumT      SensorNum,
			   SAHPI_IN  SaHpiSensorReadingT  *ReadingInput,
			   SAHPI_OUT SaHpiSensorReadingT  *ConvertedReading )
{
  CheckSession();

  PreMarshal( eFsaHpiSensorReadingConvert );

  HpiMarshalRequest4( hm, request, &SessionId, &ResourceId,
		     &SensorNum, ReadingInput );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      ConvertedReading );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(SensorThresholdsGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT        SessionId,
			  SAHPI_IN  SaHpiResourceIdT       ResourceId,
			  SAHPI_IN  SaHpiSensorNumT        SensorNum,
			  SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds )
{
  CheckSession();

  PreMarshal( eFsaHpiSensorThresholdsGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );

  Send();

  // this is to pass test sf_202.test of hpitest
  memset( SensorThresholds, 0, sizeof( SaHpiSensorThresholdsT ) );
  
  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      SensorThresholds );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(SensorThresholdsSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT        SessionId,
			  SAHPI_IN  SaHpiResourceIdT       ResourceId,
			  SAHPI_IN  SaHpiSensorNumT        SensorNum,
			  SAHPI_IN  SaHpiSensorThresholdsT *SensorThresholds )
{
  CheckSession();

  PreMarshal( eFsaHpiSensorThresholdsSet );

  HpiMarshalRequest4( hm, request, &SessionId, &ResourceId, &SensorNum, SensorThresholds );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(SensorTypeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT     SessionId,
		    SAHPI_IN  SaHpiResourceIdT    ResourceId,
		    SAHPI_IN  SaHpiSensorNumT     SensorNum,
		    SAHPI_OUT SaHpiSensorTypeT    *Type,
		    SAHPI_OUT SaHpiEventCategoryT *Category )
{
  CheckSession();

  PreMarshal( eFsaHpiSensorTypeGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );
  
  Send();

  HpiDemarshalReply2( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      Type, Category );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(SensorEventEnablesGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
			    SAHPI_IN  SaHpiResourceIdT        ResourceId,
			    SAHPI_IN  SaHpiSensorNumT         SensorNum,
			    SAHPI_OUT SaHpiSensorEvtEnablesT  *Enables )
{
  CheckSession();

  PreMarshal( eFsaHpiSensorEventEnablesGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, 
		      Enables );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(SensorEventEnablesSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT        SessionId,
			    SAHPI_IN SaHpiResourceIdT       ResourceId,
			    SAHPI_IN SaHpiSensorNumT        SensorNum,
			    SAHPI_IN SaHpiSensorEvtEnablesT *Enables )
{
  CheckSession();

  PreMarshal( eFsaHpiSensorEventEnablesSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      Enables );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ControlTypeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
		     SAHPI_IN  SaHpiResourceIdT ResourceId,
		     SAHPI_IN  SaHpiCtrlNumT    CtrlNum,
		     SAHPI_OUT SaHpiCtrlTypeT   *Type )
{
  CheckSession();

  PreMarshal( eFsaHpiControlTypeGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &CtrlNum );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		     Type );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ControlStateGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT  SessionId,
		      SAHPI_IN    SaHpiResourceIdT ResourceId,
		      SAHPI_IN    SaHpiCtrlNumT    CtrlNum,
		      SAHPI_INOUT SaHpiCtrlStateT  *CtrlState )
{
  CheckSession();

  PreMarshal( eFsaHpiControlStateGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &CtrlNum );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      CtrlState);

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ControlStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
		      SAHPI_IN SaHpiResourceIdT ResourceId,
		      SAHPI_IN SaHpiCtrlNumT    CtrlNum,
		      SAHPI_IN SaHpiCtrlStateT  *CtrlState )
{
  CheckSession();

  PreMarshal( eFsaHpiControlStateSet );

  HpiMarshalRequest4( hm, request, &SessionId, &ResourceId, &CtrlNum, CtrlState );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EntityInventoryDataRead)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT         SessionId,
			      SAHPI_IN    SaHpiResourceIdT        ResourceId,
			      SAHPI_IN    SaHpiEirIdT             EirId,
			      SAHPI_IN    SaHpiUint32T            BufferSize,
			      SAHPI_OUT   SaHpiInventoryDataT     *InventData,
			      SAHPI_OUT   SaHpiUint32T            *ActualSize )
{
  return SA_ERR_HPI_UNSUPPORTED_API;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(EntityInventoryDataWrite)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT          SessionId,
			       SAHPI_IN SaHpiResourceIdT         ResourceId,
			       SAHPI_IN SaHpiEirIdT              EirId,
			       SAHPI_IN SaHpiInventoryDataT      *InventData )
{
  return SA_ERR_HPI_UNSUPPORTED_API;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(WatchdogTimerGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT    SessionId,
		       SAHPI_IN  SaHpiResourceIdT   ResourceId,
		       SAHPI_IN  SaHpiWatchdogNumT  WatchdogNum,
		       SAHPI_OUT SaHpiWatchdogT     *Watchdog )
{
  CheckSession();

  PreMarshal( eFsaHpiWatchdogTimerGet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &WatchdogNum );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      Watchdog );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(WatchdogTimerSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT    SessionId,
		       SAHPI_IN SaHpiResourceIdT   ResourceId,
		       SAHPI_IN SaHpiWatchdogNumT  WatchdogNum,
		       SAHPI_IN SaHpiWatchdogT     *Watchdog )
{
  CheckSession();

  PreMarshal( eFsaHpiWatchdogTimerSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &WatchdogNum );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		     Watchdog );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(WatchdogTimerReset)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
			 SAHPI_IN SaHpiResourceIdT  ResourceId,
			 SAHPI_IN SaHpiWatchdogNumT WatchdogNum )
{
  CheckSession();

  PreMarshal( eFsaHpiWatchdogTimerReset );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &WatchdogNum );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(HotSwapControlRequest)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
			    SAHPI_IN SaHpiResourceIdT ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapControlRequest );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ResourceActiveSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
			SAHPI_IN SaHpiResourceIdT ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceActiveSet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ResourceInactiveSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
			  SAHPI_IN SaHpiResourceIdT ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceInactiveSet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(AutoInsertTimeoutGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT SessionId,
			   SAHPI_OUT SaHpiTimeoutT    *Timeout )
{
  CheckSession();

  PreMarshal( eFsaHpiAutoInsertTimeoutGet );

  HpiMarshalRequest1( hm, request, &SessionId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, 
		      Timeout );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(AutoInsertTimeoutSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
			   SAHPI_IN SaHpiTimeoutT    Timeout )
{
  CheckSession();

  PreMarshal( eFsaHpiAutoInsertTimeoutSet );

  HpiMarshalRequest2( hm, request, &SessionId, &Timeout );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(AutoExtractTimeoutGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
			    SAHPI_IN  SaHpiResourceIdT  ResourceId,
			    SAHPI_OUT SaHpiTimeoutT     *Timeout )
{
  CheckSession();

  PreMarshal( eFsaHpiAutoExtractTimeoutGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Timeout );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(AutoExtractTimeoutSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
			    SAHPI_IN  SaHpiResourceIdT  ResourceId,
			    SAHPI_IN  SaHpiTimeoutT     Timeout )
{
  CheckSession();

  PreMarshal( eFsaHpiAutoExtractTimeoutSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Timeout );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(HotSwapStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
		      SAHPI_IN  SaHpiResourceIdT ResourceId,
		      SAHPI_OUT SaHpiHsStateT    *State )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapStateGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      State );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(HotSwapActionRequest)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
			   SAHPI_IN SaHpiResourceIdT ResourceId,
			   SAHPI_IN SaHpiHsActionT   Action )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapActionRequest );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Action );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(ResourcePowerStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT     SessionId,
			    SAHPI_IN  SaHpiResourceIdT    ResourceId,
			    SAHPI_OUT SaHpiHsPowerStateT  *State )
{
  CheckSession();

  PreMarshal( eFsaHpiResourcePowerStateGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      State );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ResourcePowerStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT     SessionId,
			    SAHPI_IN SaHpiResourceIdT    ResourceId,
			    SAHPI_IN SaHpiHsPowerStateT  State )
{
  CheckSession();

  PreMarshal( eFsaHpiResourcePowerStateSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &State );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(HotSwapIndicatorStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
			       SAHPI_IN  SaHpiResourceIdT        ResourceId,
			       SAHPI_OUT SaHpiHsIndicatorStateT  *State )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapIndicatorStateGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      State );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(HotSwapIndicatorStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT         SessionId,
			       SAHPI_IN SaHpiResourceIdT        ResourceId,
			       SAHPI_IN SaHpiHsIndicatorStateT  State )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapIndicatorStateSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &State );

  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ParmControl)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
		  SAHPI_IN SaHpiResourceIdT ResourceId,
		  SAHPI_IN SaHpiParmActionT Action )
{
  CheckSession();

  PreMarshal( eFsaHpiParmControl );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Action );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API
dOpenHpiClientFunction(ResourceResetStateGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT    SessionId,
			    SAHPI_IN SaHpiResourceIdT   ResourceId,
			    SAHPI_OUT SaHpiResetActionT *ResetAction )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceResetStateGet );

  HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );

  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
		      ResetAction );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API 
dOpenHpiClientFunction(ResourceResetStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
			    SAHPI_IN SaHpiResourceIdT ResourceId,
			    SAHPI_IN SaHpiResetActionT ResetAction )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceResetStateSet );

  HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &ResetAction );
  
  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}
