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

#include <SaHpi.h>
#include "simple_config.h"
#include "marshal_hpi.h"
#include "connection.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


#define OH_CLIENT_DEFAULT_CONF "/etc/openhpi/openhpiclient.conf"


typedef struct
{
  char m_server[dConfigStringMaxLength];
  int  m_port;
  int  m_debug;
} cOpenHpiClientConf;


static cClientConnection  *client_connection = 0;
static int                 num_sessions = 0;
static cOpenHpiClientConf  config;
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


static int
OpenConnection( void )
{
  assert( client_connection == 0 );

  char *openhpi_client_conf = getenv( "OPENHPICLIENT_CONF" );

  if ( openhpi_client_conf == 0 )
       openhpi_client_conf = OH_CLIENT_DEFAULT_CONF;

  ConfigRead( openhpi_client_conf, config_table, &config );

  client_connection = ClientConnectionOpen( config.m_server, config.m_port );

  if ( !client_connection )
       return 0;

  return 1;
}


static void
CloseConnection( void )
{
  assert( client_connection );
  ClientConnectionClose( client_connection );
  client_connection = 0;
}


#define CheckSession() \
  do                                            \
     {                                          \
       if ( !client_connection )                \
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
       int r = ClientConnectionWriteMsg( client_connection, &request_header, \
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


static int initialize = 0;


SaErrorT SAHPI_API 
saHpiInitialize( SAHPI_OUT SaHpiVersionT *HpiImplVersion )
{
  if ( initialize )
       return SA_ERR_HPI_DUPLICATE;

  initialize = 1;

  *HpiImplVersion = SAHPI_INTERFACE_VERSION;

  return SA_OK;
}


SaErrorT SAHPI_API
saHpiFinalize ( void )
{
  if ( num_sessions )
       return SA_ERR_HPI_BUSY;
 
  initialize = 0;

  return SA_OK;
}


SaErrorT SAHPI_API
saHpiSessionOpen( SAHPI_IN  SaHpiDomainIdT   DomainId,
		  SAHPI_OUT SaHpiSessionIdT *SessionId,
		  SAHPI_IN  void            *SecurityParams )
{
  if ( num_sessions == 0 )
     {
       if ( !OpenConnection() )
	    return SA_ERR_HPI_INITIALIZING;
     }

  num_sessions++;

  PreMarshal( eFsaHpiSessionOpen );

  HpiMarshalRequest1( hm, request, &DomainId );

  Send();

  HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, SessionId );

  PostMarshal();

  if ( err != SA_OK )
     {
       num_sessions--;
   
       assert( num_sessions >= 0 );

       if ( num_sessions < 0 )
	    num_sessions = 0;
  
       if ( num_sessions == 0 )
	    CloseConnection();
     }

  return err;
}


SaErrorT SAHPI_API
saHpiSessionClose( SAHPI_IN SaHpiSessionIdT SessionId )
{
  CheckSession();

  PreMarshal( eFsaHpiSessionClose );
  HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  num_sessions--;

  assert( num_sessions >= 0 );

  if ( num_sessions < 0 )
       num_sessions = 0;

  if ( num_sessions == 0 )
       CloseConnection();

  return err;
}


SaErrorT SAHPI_API
saHpiResourcesDiscover( SAHPI_IN SaHpiSessionIdT SessionId )
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
saHpiRptInfoGet( SAHPI_IN SaHpiSessionIdT SessionId,
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
saHpiRptEntryGet( SAHPI_IN SaHpiSessionIdT SessionId,
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
saHpiRptEntryGetByResourceId( SAHPI_IN  SaHpiSessionIdT  SessionId,
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
saHpiResourceSeveritySet( SAHPI_IN  SaHpiSessionIdT  SessionId,
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
saHpiResourceTagSet( SAHPI_IN  SaHpiSessionIdT    SessionId,
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
saHpiResourceIdGet( SAHPI_IN  SaHpiSessionIdT   SessionId,
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
saHpiEntitySchemaGet( SAHPI_IN  SaHpiSessionIdT     SessionId,
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
saHpiEventLogInfoGet( SAHPI_IN  SaHpiSessionIdT  SessionId,
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
saHpiEventLogEntryGet( SAHPI_IN    SaHpiSessionIdT     SessionId,
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
saHpiEventLogEntryAdd( SAHPI_IN SaHpiSessionIdT      SessionId,
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
saHpiEventLogEntryDelete( SAHPI_IN SaHpiSessionIdT      SessionId,
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
saHpiEventLogClear( SAHPI_IN  SaHpiSessionIdT   SessionId,
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
saHpiEventLogTimeGet( SAHPI_IN  SaHpiSessionIdT  SessionId,
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
saHpiEventLogTimeSet( SAHPI_IN SaHpiSessionIdT   SessionId,
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
saHpiEventLogStateGet( SAHPI_IN  SaHpiSessionIdT  SessionId,
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
saHpiEventLogStateSet( SAHPI_IN SaHpiSessionIdT   SessionId,
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
saHpiSubscribe( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiUnsubscribe( SAHPI_IN SaHpiSessionIdT SessionId )
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
saHpiEventGet( SAHPI_IN    SaHpiSessionIdT      SessionId,
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
saHpiRdrGet( SAHPI_IN  SaHpiSessionIdT       SessionId,
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
saHpiSensorReadingGet( SAHPI_IN  SaHpiSessionIdT      SessionId,
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
saHpiSensorReadingConvert( SAHPI_IN  SaHpiSessionIdT      SessionId,
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
saHpiSensorThresholdsGet( SAHPI_IN  SaHpiSessionIdT        SessionId,
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
saHpiSensorThresholdsSet( SAHPI_IN  SaHpiSessionIdT        SessionId,
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
saHpiSensorTypeGet( SAHPI_IN  SaHpiSessionIdT     SessionId,
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
saHpiSensorEventEnablesGet( SAHPI_IN  SaHpiSessionIdT         SessionId,
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
saHpiSensorEventEnablesSet( SAHPI_IN SaHpiSessionIdT        SessionId,
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
saHpiControlTypeGet( SAHPI_IN  SaHpiSessionIdT  SessionId,
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
saHpiControlStateGet( SAHPI_IN    SaHpiSessionIdT  SessionId,
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
saHpiControlStateSet( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiEntityInventoryDataRead( SAHPI_IN    SaHpiSessionIdT         SessionId,
			      SAHPI_IN    SaHpiResourceIdT        ResourceId,
			      SAHPI_IN    SaHpiEirIdT             EirId,
			      SAHPI_IN    SaHpiUint32T            BufferSize,
			      SAHPI_OUT   SaHpiInventoryDataT     *InventData,
			      SAHPI_OUT   SaHpiUint32T            *ActualSize )
{
  return SA_ERR_HPI_UNSUPPORTED_API;
}


SaErrorT SAHPI_API
saHpiEntityInventoryDataWrite( SAHPI_IN SaHpiSessionIdT          SessionId,
			       SAHPI_IN SaHpiResourceIdT         ResourceId,
			       SAHPI_IN SaHpiEirIdT              EirId,
			       SAHPI_IN SaHpiInventoryDataT      *InventData )
{
  return SA_ERR_HPI_UNSUPPORTED_API;
}


SaErrorT SAHPI_API
saHpiWatchdogTimerGet( SAHPI_IN  SaHpiSessionIdT    SessionId,
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
saHpiWatchdogTimerSet( SAHPI_IN SaHpiSessionIdT    SessionId,
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
saHpiWatchdogTimerReset( SAHPI_IN SaHpiSessionIdT   SessionId,
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
saHpiHotSwapControlRequest( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiResourceActiveSet( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiResourceInactiveSet( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiAutoInsertTimeoutGet( SAHPI_IN  SaHpiSessionIdT SessionId,
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
saHpiAutoInsertTimeoutSet( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiAutoExtractTimeoutGet( SAHPI_IN  SaHpiSessionIdT   SessionId,
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
saHpiAutoExtractTimeoutSet( SAHPI_IN  SaHpiSessionIdT   SessionId,
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
saHpiHotSwapStateGet( SAHPI_IN  SaHpiSessionIdT  SessionId,
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
saHpiHotSwapActionRequest( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiResourcePowerStateGet( SAHPI_IN  SaHpiSessionIdT     SessionId,
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
saHpiResourcePowerStateSet( SAHPI_IN SaHpiSessionIdT     SessionId,
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
saHpiHotSwapIndicatorStateGet( SAHPI_IN  SaHpiSessionIdT         SessionId,
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
saHpiHotSwapIndicatorStateSet( SAHPI_IN SaHpiSessionIdT         SessionId,
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
saHpiParmControl( SAHPI_IN SaHpiSessionIdT  SessionId,
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
saHpiResourceResetStateGet( SAHPI_IN SaHpiSessionIdT    SessionId,
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
saHpiResourceResetStateSet( SAHPI_IN SaHpiSessionIdT  SessionId,
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
