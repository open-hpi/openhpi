/*
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


#include <assert.h>
#include <stdlib.h>
#include <string.h>
extern "C" {
#include "SaHpi.h"
}
#include <openhpi.h>
#include <oh_utils.h>
#include "marshal_hpi.h"
#include "client_config.h"
#include "openhpiclient.h"


static void RemoteClientClose( void *hnd );
static SaErrorT RemoteClientDiscoverResources( void *hnd );


class cRrdMap
{
public:
  SaHpiResourceIdT m_id;
  SaHpiResourceIdT m_remote_id;
  bool             m_mark;

  cRrdMap( SaHpiResourceIdT id, SaHpiResourceIdT rid )
    : m_id( id ), m_remote_id( rid )
  {}
};


class cRemoteClientConfig
{
public:
  cOpenHpiClientConf m_config;
  SaHpiSessionIdT    m_session_id; // session id for default domain
  GList             *m_resource_map; // map remote resource ids <-> resource ids
  SaHpiEntityPathT   m_entity_root;
  SaHpiUint32T       m_update_count;
  
  cRemoteClientConfig( const char *server, 
		       int port, int debug, int max_outstanding,
		       unsigned int ti,
		       const SaHpiEntityPathT &ep )
    : m_resource_map( 0 ), m_entity_root( ep ),
      m_update_count( 0 )
  {
    memset( &m_config, 0, sizeof( cOpenHpiClientConf ) );    
    strcpy( m_config.m_server, server );
    m_config.m_port  = port;
    m_config.m_debug = debug;
    m_config.m_max_outstanding = max_outstanding; // must be <= 256
    m_config.m_timeout = ti;
  }

  ~cRemoteClientConfig()
  {
    while( m_resource_map )
       {
	 cRrdMap *rm = (cRrdMap *)m_resource_map->data;
	 m_resource_map = g_list_remove( m_resource_map, rm );
	 delete rm;
       }
  }

  void AppendEntityRoot( SaHpiEntityPathT &ep )
  {
    ep_concat( &ep, &m_entity_root );
  }

  bool MapIdToRemoteId( SaHpiResourceIdT id, SaHpiResourceIdT &rid )
  {
    for( GList *list = m_resource_map; list; list = g_list_next( list ) )
       {
	 cRrdMap *rm = (cRrdMap *)list->data;

	 if ( rm->m_id == id )
	    {
	      rid = rm->m_remote_id;
	      return true;
	    }
       }

    return false;
  }

  cRrdMap *FindRemoteId( SaHpiResourceIdT rid )
  {
    for( GList *list = m_resource_map; list; list = g_list_next( list ) )
       {
	 cRrdMap *rm = (cRrdMap *)list->data;

	 if ( rm->m_remote_id == rid )
	      return rm;
       }

    return 0;
  }

  bool MapRemoteIdToId( SaHpiResourceIdT rid, SaHpiResourceIdT &id )
  {
    for( GList *list = m_resource_map; list; list = g_list_next( list ) )
       {
	 cRrdMap *rm = (cRrdMap *)list->data;

	 if ( rm->m_remote_id == rid )
	    {
	      id = rm->m_id;
	      return true;
	    }
       }

    return false;
  }

  void AddMapping( SaHpiResourceIdT id, SaHpiResourceIdT rid )
  {
    cRrdMap *rm = new cRrdMap( id, rid );
    rm->m_mark = true;
    m_resource_map = g_list_append( m_resource_map, rm );
  }

  void ClearMarks()
  {
    for( GList *list = m_resource_map; list; list = g_list_next( list ) )
       {
	 cRrdMap *rm = (cRrdMap *)list->data;
	 rm->m_mark = false;
       }
  }

  void CheckForResource( oh_handler_state *handler )
  {
    SaHpiRptInfoT info;
  
    SaErrorT rv = RemoteClientRptInfoGet( &m_config, m_session_id, &info );

    if ( rv != SA_OK )
       {
	 dbg( "cannot SaHpiRptInfoGet: %d!", rv );
	 return;
       }

    if ( m_update_count == info.UpdateCount )
	 return;

    // update rpt
    rv = RemoteClientDiscoverResources( handler );

    if ( rv == SA_OK )
	 m_update_count = info.UpdateCount;
  }
};


// ABI Interface functions
static void *
RemoteClientOpen( GHashTable *handler_config )
{
  if ( !handler_config )
     {
       dbg( "No config file provided.....ooops!" );
       return 0;
     }

  const char *er = (const char *)g_hash_table_lookup( handler_config,
						      "entity_root" );

  if ( !er )
     {
       dbg( "entity_root is missing in config file" );
       return false;
     }

  SaHpiEntityPathT entity_root;

  if ( string2entitypath( er, &entity_root ) )
     {
       dbg( "cannot understand entity_root !" );
       return false;
     }

  char *server = (char *)g_hash_table_lookup( handler_config, "host" );

  if ( !server )
     {
       dbg( "No host given !" );
       return 0;
     }

  int port = dDefaultDaemonPort;
  
  char *port_str = (char *)g_hash_table_lookup( handler_config, "port" );

  if ( port_str )
       port = atoi( port_str );

  int debug = 0;

  char *debug_str = (char *)g_hash_table_lookup( handler_config, "debug" );

  if ( debug_str )
       debug = atoi( debug_str );

  int max_outstanding = dDefaultMaxOutstanding;
  
  char *mo = (char *)g_hash_table_lookup( handler_config, "max_outstanding" );
 
  if ( mo )
     {
       max_outstanding = atoi( mo );

       if ( max_outstanding < 1 )
	    max_outstanding = 1;
       
       if ( max_outstanding > 250 )
	    max_outstanding = 250;
     }

  unsigned int ti = dDefaultMessageTimeout;

  char *t = (char *)g_hash_table_lookup( handler_config, "timeout" );
  
  if ( t )
       ti = atoi( t );

  cRemoteClientConfig *config = new cRemoteClientConfig( server,
							 port, debug,
							 max_outstanding, ti,
							 entity_root );

  if ( !config )
     {
       dbg( "out of space !" );
       return 0;
     }

  SaHpiVersionT version;

  SaErrorT rv = RemoteClientInitialize( &config->m_config, &version );

  if ( rv != SA_OK )
     {
       dbg( "cannot connect to daemon !" );

       delete config;
       return 0;
     }

  rv = RemoteClientSessionOpen( &config->m_config, SAHPI_DEFAULT_DOMAIN_ID, &config->m_session_id, 0 );

  if ( rv != SA_OK )
     {
       dbg( "cannot open session for default domain !" );
       RemoteClientFinalize( &config->m_config );
       delete config;

       return 0;
     }

  rv = RemoteClientSubscribe( &config->m_config, config->m_session_id, SAHPI_TRUE );
 
  if ( rv != SA_OK )
     {
       dbg( "cannot subscribe !" );
       RemoteClientSessionClose( &config->m_config, config->m_session_id );
       RemoteClientFinalize( &config->m_config );
       delete config;

       return 0;
     }

  // allocate handler
  struct oh_handler_state *handler = (struct oh_handler_state *)g_malloc0(
				     sizeof( struct oh_handler_state ) );
  handler->eventq   = 0;
  handler->data     = config;
  handler->rptcache = (RPTable *)g_malloc0( sizeof( RPTable ) );
  handler->config   = handler_config;

  config->CheckForResource( handler );

  return handler;
}


static void
RemoteClientClose( void *hnd )
{
  dbg( "RemoteClientClose" );

  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig *config = (cRemoteClientConfig *)handler->data;

  RemoteClientUnsubscribe( &config->m_config, config->m_session_id );
  RemoteClientSessionClose( &config->m_config, config->m_session_id );
  RemoteClientFinalize( &config->m_config );

  delete config;
  g_free( handler->rptcache );
  g_free( handler );
}


static SaErrorT
RemoteClientGetEvent( void *hnd, struct oh_event *e, 
		      struct timeval *timeout )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig *config = (cRemoteClientConfig *)handler->data;

  if ( g_slist_length( handler->eventq ) > 0 )
     {
       memcpy( e, handler->eventq->data, sizeof( oh_event ) );
       g_free( handler->eventq->data );
       handler->eventq = g_slist_remove_link( handler->eventq, handler->eventq );
       return 1;
     }

  // check rpt for new resources
  config->CheckForResource( handler );

  SaHpiEventT    ev;
  SaHpiRdrT      rdr;
  SaHpiRptEntryT rpt_entry;

  SaErrorT rv = RemoteClientEventGet( &config->m_config, config->m_session_id, 0,
				      &ev, &rdr, &rpt_entry );

  if ( rv !=  SA_OK )
     {
       if ( rv != SA_ERR_HPI_TIMEOUT )
       dbg( "SaHpiGetEvent: %d", rv );

       return 0;
     }

  if ( config->MapRemoteIdToId( ev.Source, ev.Source ) == false )
     {
       dbg( "cannot find remote resource id %d", ev.Source );
       return 0;
     }

  memset( e, 0, sizeof( oh_event ) );
  e->type = oh_event::OH_ET_HPI;
  e->u.hpi_event.parent = rpt_entry.ResourceId;
  e->u.hpi_event.id     = rdr.RecordId; // mapping of id is missing !!!
  e->u.hpi_event.event = ev;

  return 1;
}


static SaErrorT
RemoteClientDiscoverResources( void *hnd )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig *config = (cRemoteClientConfig *)handler->data;

  SaErrorT rv = RemoteClientResourcesDiscover( &config->m_config, config->m_session_id );

  if ( rv )
       return rv;

  config->ClearMarks();

  // read resources
  SaHpiEntryIdT next = SAHPI_FIRST_ENTRY;
  do
     {
       SaHpiEntryIdT current = next;
       SaHpiRptEntryT entry;

       rv = RemoteClientRptEntryGet( &config->m_config, config->m_session_id, 
				     current, &next, &entry );

       if ( rv != SA_OK )
	  {
	    if ( current != SAHPI_FIRST_ENTRY )
	       {
		 dbg( "saHpiRptEntryGet: %d", rv );
		 return rv;
	       }

	    dbg( "empty RPT" );
	    break;
	  }

       cRrdMap *rm = config->FindRemoteId( entry.ResourceId );

       if ( rm )
	    rm->m_mark = true;
       else
	  {
	    // add new resource
	    SaHpiResourceIdT rid = entry.ResourceId;

	    config->AppendEntityRoot( entry.ResourceEntity );

	    SaHpiResourceIdT id = oh_uid_from_entity_path( &entry.ResourceEntity );
	    entry.ResourceId = id;

	    // create mapping
	    config->AddMapping( id, rid );

	    struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

	    if ( !e )
	       {
		 dbg( "out of space !" );
		 return SA_ERR_HPI_OUT_OF_SPACE;
	       }

	    memset( e, 0, sizeof( struct oh_event ) );
	    e->type = oh_event::OH_ET_RESOURCE;

	    e->u.res_event.entry = entry;

	    // add the entity to the resource cache
	    int r = oh_add_resource( handler->rptcache,
				     &(e->u.res_event.entry), 0, 0 );
	    assert( r == 0 );

	    handler->eventq = g_slist_append( handler->eventq, e );

	    // add rdrs
	    SaHpiEntryIdT next_rdr = SAHPI_FIRST_ENTRY;

	    do
	       {
		 SaHpiEntryIdT current_rdr = next_rdr;
		 SaHpiRdrT     rdr;

		 rv = RemoteClientRdrGet( &config->m_config, config->m_session_id, rid, current_rdr, 
					  &next_rdr, &rdr );

		 if ( rv != SA_OK )
		      return rv;

		 // create rdrs
		 config->AppendEntityRoot( rdr.Entity );

		 // create event
		 e = (oh_event *)g_malloc0( sizeof( oh_event ) );

		 if ( !e )
		    {
		      dbg( "out of space" );
		      return SA_ERR_HPI_OUT_OF_SPACE;
		    }

		 memset( e, 0, sizeof( struct oh_event ) );

		 e->type = oh_event::OH_ET_RDR;

		 // create rdr
		 e->u.rdr_event.rdr = rdr;

		 r = oh_add_rdr( handler->rptcache,
				 entry.ResourceId,
				 &e->u.rdr_event.rdr, 0, 0 );
		 assert( r == 0 );

		 handler->eventq = g_slist_append( handler->eventq, e );
	       }
	    while( next_rdr != SAHPI_LAST_ENTRY );
	  }
     }
  while( next != SAHPI_LAST_ENTRY );

  // remove old resources
  for( GList *list = config->m_resource_map; list;  )
     {
       GList *n = g_list_next( list );

       cRrdMap *rm = (cRrdMap *)list->data;

       if ( rm->m_mark )
	  {
	    list = n;
	    continue;
	  }

       // remove resource from local cache
       int r = oh_remove_resource( handler->rptcache, rm->m_id );
       assert( r == 0 );

       config->m_resource_map = g_list_remove( config->m_resource_map, rm );

       // create remove event
       oh_event *e = (oh_event *)g_malloc0( sizeof( oh_event ) );

       if ( !e )
	  {
	    dbg( "out of space" );
	    delete rm;
	    return  SA_ERR_HPI_OUT_OF_SPACE;
	  }

       memset( e, 0, sizeof( struct oh_event ) );
       e->type = oh_event::OH_ET_RESOURCE_DEL;
       e->u.res_event.entry.ResourceId = rm->m_id;

       handler->eventq = g_slist_append( handler->eventq, e );

       delete rm;

       list = n;
     }

  return SA_OK;
}


static SaErrorT
RemoteClientSetResourceSeverity( void            *hnd,
				 SaHpiResourceIdT id,
				 SaHpiSeverityT   sev )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientResourceSeveritySet( &config->m_config, config->m_session_id,
						 rid, sev );

  return rv;
}


static SaErrorT
RemoteClientGetSensorData( void                *hnd,
			   SaHpiResourceIdT     id,
			   SaHpiSensorNumT      num,
			   SaHpiSensorReadingT *data )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientSensorReadingGet( &config->m_config, config->m_session_id,
					      rid, num, data );

  return rv;
}


static SaErrorT
RemoteClientGetSensorThresholds( void                   *hnd,
				 SaHpiResourceIdT        id,
				 SaHpiSensorNumT         num,
				 SaHpiSensorThresholdsT *thres )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientSensorThresholdsGet( &config->m_config, config->m_session_id,
						 rid, num, thres );

  return rv;
}


static SaErrorT
RemoteClientSetSensorThresholds( void *hnd,
                         SaHpiResourceIdT id,
                         SaHpiSensorNumT  num,
                         const SaHpiSensorThresholdsT *thres )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaHpiSensorThresholdsT *th = const_cast<SaHpiSensorThresholdsT *>( thres );

  SaErrorT rv = RemoteClientSensorThresholdsSet( &config->m_config, config->m_session_id,
						 rid, num, th );

  return rv;
}


static SaErrorT
RemoteClientGetSensorEventEnables( void *hnd, 
                           SaHpiResourceIdT        id,
                           SaHpiSensorNumT         num,
                           SaHpiSensorEvtEnablesT *enables )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientSensorEventEnablesGet( &config->m_config, config->m_session_id,
						   rid, num,  enables );

  return rv;
}


static SaErrorT
RemoteClientSetSensorEventEnables( void *hnd,
                           SaHpiResourceIdT         id,
                           SaHpiSensorNumT          num,
                           const SaHpiSensorEvtEnablesT *enables )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaHpiSensorEvtEnablesT *en = const_cast<SaHpiSensorEvtEnablesT *>( enables );

  SaErrorT rv = RemoteClientSensorEventEnablesSet( &config->m_config, config->m_session_id,
						   rid, num,  en );

  return rv;
}


static SaErrorT
RemoteClientGetControlState( void *hnd, SaHpiResourceIdT id,
		     SaHpiCtrlNumT num,
		     SaHpiCtrlStateT *state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientControlStateGet( &config->m_config, config->m_session_id,
					     rid, num,  state );

  return rv;
}


static SaErrorT
RemoteClientSetControlState( void *hnd, SaHpiResourceIdT id,
		     SaHpiCtrlNumT num,
		     SaHpiCtrlStateT *state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientControlStateSet( &config->m_config, config->m_session_id,
					     rid, num,  state );

  return rv;
}


static SaErrorT
RemoteClientGetInventorySize( void *hnd, SaHpiResourceIdT id,
			      SaHpiEirIdT num,
			      SaHpiUint32T *size )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;
  unsigned char            data[128*1024];

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientEntityInventoryDataRead( &config->m_config, config->m_session_id,
						     rid, num, 128*1024, (SaHpiInventoryDataT *)(void *)data, size );

  return rv;
}


static SaErrorT
RemoteClientGetInventoryInfo( void *hnd, SaHpiResourceIdT id,
			      SaHpiEirIdT num,
			      SaHpiInventoryDataT *data )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;
  SaHpiUint32T             actual_size;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientEntityInventoryDataRead( &config->m_config, config->m_session_id,
						     rid, num, 128*1024, data, &actual_size );

  return rv;
}


static SaErrorT
RemoteClientGetSelInfo( void            *hnd,
			SaHpiResourceIdT id,
			SaHpiSelInfoT   *info )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientEventLogInfoGet( &config->m_config, config->m_session_id,
					     rid, info );

  return rv;
}


static SaErrorT
RemoteClientSetSelTime( void *hnd, SaHpiResourceIdT id, SaHpiTimeT t )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientEventLogTimeSet( &config->m_config, config->m_session_id,
					     rid, t );

  return rv;
}


static SaErrorT
RemoteClientAddSelEntry( void *hnd, SaHpiResourceIdT id,
			 const SaHpiSelEntryT *Event )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaHpiSelEntryT ev = *Event;

  if ( config->MapIdToRemoteId( Event->Event.Source, ev.Event.Source ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientEventLogEntryAdd( &config->m_config, config->m_session_id,
					      rid, &ev );

  return rv;
}


static SaErrorT
RemoteClientDelSelEntry( void *hnd, SaHpiResourceIdT id,
                 SaHpiSelEntryIdT sid )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientEventLogEntryDelete( &config->m_config, config->m_session_id,
						 rid, sid );

  return rv;
}


static SaErrorT
RemoteClientGetSelEntry( void *hnd, SaHpiResourceIdT id,
			 SaHpiSelEntryIdT current,
			 SaHpiSelEntryIdT *prev,
			 SaHpiSelEntryIdT *next,
			 SaHpiSelEntryT *entry )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaHpiRdrT      rdr;
  SaHpiRptEntryT rpt_entry;

  SaErrorT rv = RemoteClientEventLogEntryGet( &config->m_config, config->m_session_id,
					      rid, current, prev, next, entry,
					      &rdr, &rpt_entry );

  if ( rv != SA_OK )
       return rv;

  if ( config->MapRemoteIdToId( entry->Event.Source, entry->Event.Source ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  return rv;
}


static SaErrorT
RemoteClientClearSel( void *hnd, SaHpiResourceIdT id )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientEventLogClear( &config->m_config,
					   config->m_session_id, rid );

  return rv;
}


static SaErrorT
RemoteClientGetHotswapState( void *hnd, SaHpiResourceIdT id,
			     SaHpiHsStateT *state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientHotSwapStateGet( &config->m_config,
					     config->m_session_id,
					     rid, state );

  return rv;
}


static SaErrorT
RemoteClientSetHotswapState( void *hnd, SaHpiResourceIdT id, 
			     SaHpiHsStateT state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv;

  if ( state == SAHPI_HS_STATE_ACTIVE_HEALTHY )
       rv = RemoteClientResourceActiveSet( &config->m_config,
					   config->m_session_id,
					   rid );
  else
       rv =  RemoteClientResourceInactiveSet( &config->m_config,
					      config->m_session_id,
					      rid );

  return rv;
}


static SaErrorT
RemoteClientRequestHotswapAction( void *hnd, SaHpiResourceIdT id, 
				  SaHpiHsActionT act )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientHotSwapActionRequest( &config->m_config,
						  config->m_session_id,
						  rid, act );

  return rv;
}


static SaErrorT
RemoteClientGetPowerState( void *hnd, SaHpiResourceIdT id, 
			   SaHpiHsPowerStateT *state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientResourcePowerStateGet( &config->m_config,
						   config->m_session_id,
						   rid, state );

  return rv;
}


static SaErrorT
RemoteClientSetPowerState( void *hnd, SaHpiResourceIdT id, 
			   SaHpiHsPowerStateT state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientResourcePowerStateSet( &config->m_config,
						   config->m_session_id,
						   rid, state );

  return rv;
}


static SaErrorT
RemoteClientGetIndicatorState( void *hnd, SaHpiResourceIdT id, 
			       SaHpiHsIndicatorStateT *state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientHotSwapIndicatorStateGet( &config->m_config,
						      config->m_session_id,
						      rid, state );

  return rv;
}


static SaErrorT
RemoteClientSetIndicatorState( void *hnd, SaHpiResourceIdT id,
			       SaHpiHsIndicatorStateT state )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientHotSwapIndicatorStateSet( &config->m_config,
						      config->m_session_id,
						      rid, state );

  return rv;
}


static SaErrorT
RemoteClientControlParm( void *hnd,
                 SaHpiResourceIdT id,
                 SaHpiParmActionT act )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientParmControl( &config->m_config,
					 config->m_session_id,
					 rid, act );

  return rv;
}


static SaErrorT
RemoteClientGetResetState( void *hnd, SaHpiResourceIdT id, 
                   SaHpiResetActionT *act )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientResourceResetStateGet( &config->m_config,
						   config->m_session_id,
						   rid, act );

  return rv;
}


static SaErrorT
RemoteClientSetResetState( void *hnd,
                   SaHpiResourceIdT id,
                   SaHpiResetActionT act )
{
  struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
  cRemoteClientConfig     *config = (cRemoteClientConfig *)handler->data;
  SaHpiResourceIdT         rid;

  if ( config->MapIdToRemoteId( id, rid ) == false )
       return SA_ERR_HPI_INVALID_RESOURCE;

  SaErrorT rv = RemoteClientResourceResetStateSet( &config->m_config,
						   config->m_session_id,
						   rid, act );

  return rv;
}


static struct oh_abi_v2 oh_remote_client_plugin;
static bool             oh_remote_client_plugin_init = false;

// use c binding
extern "C" {
int
get_interface( void **pp, const uuid_t uuid )
{
  if ( uuid_compare( uuid, UUID_OH_ABI_V2 ) != 0 )
     {
       *pp = NULL;
       return -1;
     }

  if ( !oh_remote_client_plugin_init )
     {
       memset( &oh_remote_client_plugin, 0, sizeof(oh_abi_v2) );

       oh_remote_client_plugin.del_sel_entry            = RemoteClientDelSelEntry;
       
       oh_remote_client_plugin.get_sensor_data          = RemoteClientGetSensorData;
       
       oh_remote_client_plugin.get_inventory_size       = RemoteClientGetInventorySize;
       oh_remote_client_plugin.get_inventory_info       = RemoteClientGetInventoryInfo;
     }

  *pp = &oh_remote_client_plugin;
  return 0;
}

void * oh_open (GHashTable *) __attribute__ ((weak, alias("RemoteClientOpen")));

void * oh_close (void *) __attribute__ ((weak, alias("RemoteClientClose")));

void * oh_get_event (void *, struct oh_event *) 
                __attribute__ ((weak, alias("RemoteClientGetEvent")));
		
void * oh_discover_resources (void *) 
                __attribute__ ((weak, alias("RemoteClientDiscoverResources")));
		
void * oh_set_resource_severity (void *, SaHpiResourceIdT, SaHpiSeverityT) 
                __attribute__ ((weak, alias("RemoteClientSetResourceSeverity")));

void * oh_get_el_info (void *, SaHpiResourceIdT, SaHpiEventLogInfoT *) 
                __attribute__ ((weak, alias("RemoteClientGetSelInfo")));
		
void * oh_set_el_time (void *, SaHpiResourceIdT, const SaHpiEventT *) 
                __attribute__ ((weak, alias("RemoteClientSetSelTime")));
		
void * oh_add_el_entry (void *, SaHpiResourceIdT, const SaHpiEventT *) 
                __attribute__ ((weak, alias("RemoteClientAddSelEntry")));
		
void * oh_get_el_entry (void *, SaHpiResourceIdT, SaHpiEventLogEntryIdT,
                       SaHpiEventLogEntryIdT *, SaHpiEventLogEntryIdT *,
                       SaHpiEventLogEntryT *, SaHpiRdrT *, SaHpiRptEntryT  *) 
                __attribute__ ((weak, alias("RemoteClientGetSelEntry")));
		       
void * oh_clear_el (void *, SaHpiResourceIdT) 
                __attribute__ ((weak, alias("RemoteClientClearSel")));

void * oh_get_sensor_reading (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiSensorReadingT *, 
			     SaHpiEventStateT    *) 
                __attribute__ ((weak, alias("dummy_get_sensor_reading")));
		  	     
void * oh_get_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 SaHpiSensorThresholdsT *) 
                __attribute__ ((weak, alias("RemoteClientGetSensorThresholds")));
		
void * oh_set_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 const SaHpiSensorThresholdsT *) 
                __attribute__ ((weak, alias("RemoteClientSetSensorThresholds")));
		
void * oh_get_sensor_event_enables (void *, SaHpiResourceIdT,
                                    SaHpiSensorNumT,
                                    SaHpiBoolT *) 
                __attribute__ ((weak, alias("RemoteClientGetSensorEventEnables")));

void * oh_set_sensor_event_enables (void *, SaHpiResourceIdT id, SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak, alias("RemoteClientSetSensorEventEnables")));

void * oh_get_control_state (void *, SaHpiResourceIdT, SaHpiCtrlNumT,
                             SaHpiCtrlModeT *, SaHpiCtrlStateT *)
                __attribute__ ((weak, alias("RemoteClientGetControlState")));
	       
void * oh_set_control_state (void *, SaHpiResourceIdT,SaHpiCtrlNumT,
                             SaHpiCtrlModeT, SaHpiCtrlStateT *)
                __attribute__ ((weak, alias("RemoteClientSetControlState")));

void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
                __attribute__ ((weak, alias("RemoteClientGetHotswapState")));

void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
                __attribute__ ((weak, alias("RemoteClientSetHotswapState")));
	       
void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
                __attribute__ ((weak, alias("RemoteClientRequestHotswapAction")));
	       
void * oh_get_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT *)
                __attribute__ ((weak, alias("RemoteClientGetPowerState")));
	       
void * oh_set_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT)
                __attribute__ ((weak, alias("RemoteClientSetPowerState")));

void * oh_get_indicator_state (void *, SaHpiResourceIdT, 
                               SaHpiHsIndicatorStateT *)
                __attribute__ ((weak, alias("RemoteClientGetIndicatorState")));
	       			       
void * oh_set_indicator_state (void *, SaHpiResourceIdT, 
                               SaHpiHsIndicatorStateT)
                __attribute__ ((weak, alias("RemoteClientSetIndicatorState")));
	       
void * oh_control_parm (void *, SaHpiResourceIdT, SaHpiParmActionT)
                __attribute__ ((weak, alias("RemoteClientControlParm")));

void * oh_get_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT *)
                __attribute__ ((weak, alias("RemoteClientGetResetState")));
		
void * oh_set_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT)
                __attribute__ ((weak, alias("RemoteClientSetResetState")));

}
