/*
 *
 * Copyright (c) 2003,2004 by FORCE Computers.
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

#include <netdb.h>
#include <errno.h>
#include <assert.h>

#include "ipmi.h"
#include "ipmi_con_lan.h"
#include "ipmi_con_smi.h"
#include "ipmi_utils.h"


#define SA_ERR_INVENT_DATA_TRUNCATED    (SaErrorT)(SA_HPI_ERR_BASE - 1000)

static cIpmi *
VerifyIpmi( void *hnd )
{
  assert( hnd );

  oh_handler_state *handler = (oh_handler_state *)hnd;
  cIpmi *ipmi = (cIpmi *)handler->data;

  if ( !ipmi )
     {
       assert( 0 );
       return 0;
     }

  if ( !ipmi->CheckMagic() )
     {
       assert( 0 );
       return 0;
     }

  if ( !ipmi->CheckHandler( handler ) )
     {
       assert( 0 );
       return 0;
     }

  return ipmi;
}


static cIpmiSensor *
VerifySensorAndEnter( void *hnd, SaHpiResourceIdT rid, SaHpiSensorNumT num,
		      cIpmi *&ipmi )
{
  ipmi = VerifyIpmi( hnd );

  if ( !ipmi )
     {
       assert( 0 );
       return 0;
     }

  ipmi->IfEnter();

  SaHpiRdrT *rdr = oh_get_rdr_by_type( ipmi->GetHandler()->rptcache,
                                       rid, SAHPI_SENSOR_RDR, num );
  if ( !rdr )
     {
       ipmi->IfLeave();
       return 0;
     }

  cIpmiSensor *sensor = (cIpmiSensor *)oh_get_rdr_data( ipmi->GetHandler()->rptcache, 
                                                        rid, rdr->RecordId );
  assert( sensor );

  if ( !ipmi->VerifySensor( sensor ) )
     {
       ipmi->IfLeave();
       return 0;
     }

  return sensor;
}


static cIpmiControl *
VerifyControlAndEnter( void *hnd, SaHpiResourceIdT rid, SaHpiCtrlNumT num,
                      cIpmi *&ipmi )
{
  ipmi = VerifyIpmi( hnd );

  if ( !ipmi )
     {
       assert( 0 );
       return 0;
     }

  ipmi->IfEnter();

  SaHpiRdrT *rdr = oh_get_rdr_by_type( ipmi->GetHandler()->rptcache,
                                       rid, SAHPI_CTRL_RDR, num );
  if ( !rdr )
     {
       ipmi->IfLeave();
       return 0;
     }

  cIpmiControl *control = (cIpmiControl *)oh_get_rdr_data( ipmi->GetHandler()->rptcache, 
                                                           rid, rdr->RecordId );
  assert( control );

  if ( !ipmi->VerifyControl( control ) )
     {
       ipmi->IfLeave();
       return 0;
     }

  return control;
}


static cIpmiInventory *
VerifyInventoryAndEnter( void *hnd, SaHpiResourceIdT rid, SaHpiEirIdT num,
                         cIpmi *&ipmi )
{
  ipmi = VerifyIpmi( hnd );

  if ( !ipmi )
     {
       assert( 0 );
       return 0;
     }

  ipmi->IfEnter();

  SaHpiRdrT *rdr = oh_get_rdr_by_type( ipmi->GetHandler()->rptcache,
                                       rid, SAHPI_INVENTORY_RDR, num );
  if ( !rdr )
     {
       ipmi->IfLeave();
       return 0;
     }

  cIpmiInventory *inv = (cIpmiInventory *)oh_get_rdr_data( ipmi->GetHandler()->rptcache, 
                                                           rid, rdr->RecordId );
  assert( inv );

  if ( !ipmi->VerifyInventory( inv ) )
     {
       ipmi->IfLeave();
       return 0;
     }

  return inv;
}


static cIpmiResource *
VerifyResourceAndEnter( void *hnd, SaHpiResourceIdT rid, cIpmi *&ipmi )
{
  ipmi = VerifyIpmi( hnd );

  if ( !ipmi )
     {
       assert( 0 );
       return 0;
     }

  ipmi->IfEnter();

  cIpmiResource *res = (cIpmiResource *)oh_get_resource_data( ipmi->GetHandler()->rptcache, rid );

  if ( !ipmi->VerifyResource( res ) )
     {
       ipmi->IfLeave();
       return 0;
     }

  return res;
}


static cIpmiSel *
VerifySelAndEnter( void *hnd, SaHpiResourceIdT rid, cIpmi *&ipmi )
{
  ipmi = VerifyIpmi( hnd );

  if ( !ipmi )
     {
       assert( 0 );
       return 0;
     }

  ipmi->IfEnter();

  cIpmiResource *res = (cIpmiResource *)oh_get_resource_data( ipmi->GetHandler()->rptcache, rid );

  if ( !ipmi->VerifyResource( res ) )
     {
       ipmi->IfLeave();
       return 0;
     }

  if ( res->FruId() || !res->Mc()->SelDeviceSupport() )
     {
       ipmi->IfLeave();
       return 0;
     }

  return res->Mc()->Sel();
}


// ABI Interface functions
static void *
IpmiOpen( GHashTable *handler_config )
{
  // open log
  const char *logfile = 0;
  int   max_logfiles = 10;
  char *tmp;
  int   lp = dIpmiLogPropNone;

  dbg( "IpmiOpen" );

  if ( !handler_config )
     {
       dbg( "No config file provided.....ooops!" );
       return 0;
     }

  logfile = (char *)g_hash_table_lookup( handler_config, "logfile" );
  tmp = (char *)g_hash_table_lookup( handler_config, "logfile_max" );

  if ( tmp )
       max_logfiles = atoi( tmp );

  tmp = (char *)g_hash_table_lookup( handler_config, "logflags" );

  if ( tmp )
     {
       if (    strstr( tmp, "StdOut" )
            || strstr( tmp, "stdout" ) )
            lp |= dIpmiLogStdOut;

       if (    strstr( tmp, "StdError" )
            || strstr( tmp, "stderr" ) )
            lp |= dIpmiLogStdErr;

       if (    strstr( tmp, "File" )
            || strstr( tmp, "file" ) )
          {
            lp |= dIpmiLogLogFile;

            if ( logfile == 0 )
                 logfile = dDefaultLogfile;
          }
     }
  else
     {
       // default
       lp = dIpmiLogStdOut;

       if ( logfile && *logfile )
            lp |= dIpmiLogLogFile;
     }

  stdlog.Open( lp, logfile, max_logfiles );
  stdlog.Time( true );

  // create domain
  cIpmi *ipmi = new cIpmi;

  // allocate handler
  oh_handler_state *handler = (oh_handler_state *)g_malloc0(
                                  sizeof( oh_handler_state ) );

  if ( !handler )
     {
       dbg("cannot allocate handler");

       delete ipmi;

       stdlog.Close();

       return 0;
     }

  handler->data     = ipmi;
  handler->rptcache = (RPTable *)g_malloc0( sizeof( RPTable ) );
  handler->config   = handler_config;

  ipmi->SetHandler( handler );

  if ( !ipmi->IfOpen( handler_config ) )
     {
       ipmi->IfClose();

       delete ipmi;

       oh_flush_rpt( handler->rptcache );
       g_free( handler->rptcache );
       g_free( handler );

       stdlog.Close();

       return 0;
     }

  return handler;
}


static void
IpmiClose( void *hnd )
{
  dbg( "IpmiClose" );

  cIpmi *ipmi = VerifyIpmi( hnd );

  ipmi->IfClose();

  assert( ipmi->CheckLock() );

  delete ipmi;

  oh_handler_state *handler = (oh_handler_state *)hnd;
  assert( handler );

  assert( handler->rptcache );
  oh_flush_rpt( handler->rptcache );
  g_free( handler->rptcache );
  g_free( handler );

  // close logfile
  stdlog.Close();
}


static SaErrorT
IpmiGetEvent( void *hnd, struct oh_event *event, 
              struct timeval *timeout )
{
  cIpmi *ipmi = VerifyIpmi( hnd );

  // there is no need to get a lock because
  // the event queue has its own lock
  SaErrorT rv = ipmi->IfGetEvent( event, *timeout );

  return rv;
}


static SaErrorT
IpmiDiscoverResources( void *hnd )
{
  cIpmi *ipmi = VerifyIpmi( hnd );

  SaErrorT rv = ipmi->IfDiscoverResources();

  return rv;
}


static SaErrorT
IpmiSetResourceSeverity( void *hnd, SaHpiResourceIdT id, SaHpiSeverityT sev )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfSetResourceSeverity( res, sev );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetSensorData( void *hnd,
                   SaHpiResourceIdT id,
                   SaHpiSensorNumT num,
                   SaHpiSensorReadingT *data )
{
  cIpmi *ipmi = 0;
  cIpmiSensor *sensor = VerifySensorAndEnter( hnd, id, num, ipmi );

  if ( !sensor )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sensor->GetData( *data );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetSensorThresholds( void                   *hnd,
                         SaHpiResourceIdT        id,
                         SaHpiSensorNumT         num,
                         SaHpiSensorThresholdsT *thres )
{
  cIpmi *ipmi;
  cIpmiSensor *sensor = VerifySensorAndEnter( hnd, id, num, ipmi );

  if ( !sensor )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = SA_ERR_HPI_INVALID_PARAMS;

  cIpmiSensorThreshold *t = dynamic_cast<cIpmiSensorThreshold *>( sensor );

  if ( t )
       rv = t->GetThresholdsAndHysteresis( *thres );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetSensorThresholds( void *hnd,
                         SaHpiResourceIdT id,
                         SaHpiSensorNumT  num,
                         const SaHpiSensorThresholdsT *thres )
{
  cIpmi *ipmi;
  cIpmiSensor *sensor = VerifySensorAndEnter( hnd, id, num, ipmi );

  if ( !sensor )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = SA_ERR_HPI_INVALID_PARAMS;

  cIpmiSensorThreshold *t = dynamic_cast<cIpmiSensorThreshold *>( sensor );

  if ( t )
       rv = t->SetThresholdsAndHysteresis( *thres );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetSensorEventEnables( void *hnd, 
                           SaHpiResourceIdT        id,
                           SaHpiSensorNumT         num,
                           SaHpiSensorEvtEnablesT *enables )
{
  cIpmi *ipmi;
  cIpmiSensor *sensor = VerifySensorAndEnter( hnd, id, num, ipmi );

  if ( !sensor )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sensor->GetEventEnables( *enables );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetSensorEventEnables( void *hnd,
                           SaHpiResourceIdT         id,
                           SaHpiSensorNumT          num,
                           const SaHpiSensorEvtEnablesT *enables )
{
  cIpmi *ipmi;
  cIpmiSensor *sensor = VerifySensorAndEnter( hnd, id, num, ipmi );

  if ( !sensor )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sensor->SetEventEnables( *enables );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetControlState( void *hnd, SaHpiResourceIdT id,
		     SaHpiCtrlNumT num,
		     SaHpiCtrlStateT *state )
{
  cIpmi *ipmi;
  cIpmiControl *control = VerifyControlAndEnter( hnd, id, num, ipmi );

  if ( !control )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = control->GetState( *state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetControlState( void *hnd, SaHpiResourceIdT id,
		     SaHpiCtrlNumT num,
		     SaHpiCtrlStateT *state )
{
  cIpmi *ipmi;
  cIpmiControl *control = VerifyControlAndEnter( hnd, id, num, ipmi );

  if ( !control )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = control->SetState( *state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetInventorySize( void *hnd, SaHpiResourceIdT id,
                      SaHpiEirIdT num,
                      SaHpiUint32T *size )
{
  cIpmi *ipmi = 0;
  cIpmiInventory *inv = VerifyInventoryAndEnter( hnd, id, num, ipmi );

  if ( !inv )
       return SA_ERR_HPI_NOT_PRESENT;

  *size = (SaHpiUint32T)inv->HpiSize();

  ipmi->IfLeave();

  return SA_OK;
}


static SaErrorT
IpmiGetInventoryInfo( void *hnd, SaHpiResourceIdT id,
                      SaHpiEirIdT num,
                      SaHpiInventoryDataT *data )
{
  cIpmi *ipmi = 0;
  cIpmiInventory *inv = VerifyInventoryAndEnter( hnd, id, num, ipmi );

  if ( !inv )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv;

  // special case data == 0
  if ( data == 0 )
     {
       assert( 0 );

       rv = SA_ERR_INVENT_DATA_TRUNCATED;
     }
  else
       rv = inv->HpiRead( *data );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetSelInfo( void *hnd,
                SaHpiResourceIdT id,
                SaHpiSelInfoT   *info )
{
  cIpmi *ipmi = 0;
  cIpmiSel *sel = VerifySelAndEnter( hnd, id, ipmi );

  if ( !sel )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sel->GetSelInfo( *info );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetSelTime( void *hnd, SaHpiResourceIdT id, SaHpiTimeT t )
{
  cIpmi *ipmi = 0;
  cIpmiSel *sel = VerifySelAndEnter( hnd, id, ipmi );

  if ( !sel )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sel->SetSelTime( t );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiAddSelEntry( void *hnd, SaHpiResourceIdT id,
                 const SaHpiSelEntryT *Event )
{
  cIpmi *ipmi = 0;
  cIpmiSel *sel = VerifySelAndEnter( hnd, id, ipmi );

  if ( !sel )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sel->AddSelEntry( *Event );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiDelSelEntry( void *hnd, SaHpiResourceIdT id,
                 SaHpiSelEntryIdT sid )
{
  cIpmi *ipmi = 0;
  cIpmiSel *sel = VerifySelAndEnter( hnd, id, ipmi );

  if ( !sel )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sel->DeleteSelEntry( sid );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetSelEntry( void *hnd, SaHpiResourceIdT id,
                 SaHpiSelEntryIdT current,
                 SaHpiSelEntryIdT *prev, SaHpiSelEntryIdT *next,
                 SaHpiSelEntryT *entry )
{
  cIpmi *ipmi = 0;
  cIpmiSel *sel = VerifySelAndEnter( hnd, id, ipmi );

  if ( !sel )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sel->GetSelEntry( current, *prev, *next, *entry );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiClearSel( void *hnd, SaHpiResourceIdT id )
{
  cIpmi *ipmi = 0;
  cIpmiSel *sel = VerifySelAndEnter( hnd, id, ipmi );

  if ( !sel )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = sel->ClearSel();

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetHotswapState( void *hnd, SaHpiResourceIdT id, 
                     SaHpiHsStateT *state )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfGetHotswapState( res, *state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetHotswapState( void *hnd, SaHpiResourceIdT id, 
                     SaHpiHsStateT state )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfSetHotswapState( res, state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiRequestHotswapAction( void *hnd, SaHpiResourceIdT id, 
                          SaHpiHsActionT act )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfRequestHotswapAction( res, act );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetPowerState( void *hnd, SaHpiResourceIdT id, 
                   SaHpiHsPowerStateT *state )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfGetPowerState( res, *state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetPowerState( void *hnd, SaHpiResourceIdT id, 
                   SaHpiHsPowerStateT state )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfSetPowerState( res, state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiGetIndicatorState( void *hnd, SaHpiResourceIdT id, 
                       SaHpiHsIndicatorStateT *state )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfGetIndicatorState( res, *state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetIndicatorState( void *hnd, SaHpiResourceIdT id,
                       SaHpiHsIndicatorStateT state )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfSetIndicatorState( res, state );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiControlParm( void *hnd,
                 SaHpiResourceIdT id,
                 SaHpiParmActionT act )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfControlParm( res, act );

  ipmi->IfLeave();

  return rv;  
}


static SaErrorT
IpmiGetResetState( void *hnd, SaHpiResourceIdT id, 
                   SaHpiResetActionT *act )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfGetResetState( res, *act );

  ipmi->IfLeave();

  return rv;
}


static SaErrorT
IpmiSetResetState( void *hnd,
                   SaHpiResourceIdT id,
                   SaHpiResetActionT act )
{
  cIpmi *ipmi = 0;
  cIpmiResource *res = VerifyResourceAndEnter( hnd, id, ipmi );

  if ( !res )
       return SA_ERR_HPI_NOT_PRESENT;

  SaErrorT rv = ipmi->IfSetResetState( res, act );

  ipmi->IfLeave();

  return rv;
}


static struct oh_abi_v2 oh_ipmi_plugin;
static bool oh_ipmi_plugin_init = false;


extern "C" {
int
get_interface( void **pp, const uuid_t uuid )
{
  if ( uuid_compare( uuid, UUID_OH_ABI_V2 ) != 0 )
     {
       *pp = NULL;
       return -1;
     }

  if ( !oh_ipmi_plugin_init )
     {
       memset( &oh_ipmi_plugin, 0, sizeof(oh_ipmi_plugin) );

       oh_ipmi_plugin.open                     = IpmiOpen;
       oh_ipmi_plugin.close                    = IpmiClose;
       oh_ipmi_plugin.get_event                = IpmiGetEvent;
       oh_ipmi_plugin.discover_resources       = IpmiDiscoverResources;
       oh_ipmi_plugin.set_resource_severity    = IpmiSetResourceSeverity;
       oh_ipmi_plugin.get_sel_info             = IpmiGetSelInfo;
       oh_ipmi_plugin.set_sel_time             = IpmiSetSelTime;
       oh_ipmi_plugin.add_sel_entry            = IpmiAddSelEntry;
       oh_ipmi_plugin.del_sel_entry            = IpmiDelSelEntry;
       oh_ipmi_plugin.get_sel_entry            = IpmiGetSelEntry;
       oh_ipmi_plugin.clear_sel                = IpmiClearSel;
       oh_ipmi_plugin.get_sensor_data          = IpmiGetSensorData;
       oh_ipmi_plugin.get_sensor_thresholds    = IpmiGetSensorThresholds;
       oh_ipmi_plugin.set_sensor_thresholds    = IpmiSetSensorThresholds;
       oh_ipmi_plugin.get_sensor_event_enables = IpmiGetSensorEventEnables;
       oh_ipmi_plugin.set_sensor_event_enables = IpmiSetSensorEventEnables;
       oh_ipmi_plugin.get_control_state        = IpmiGetControlState;
       oh_ipmi_plugin.set_control_state        = IpmiSetControlState;
       oh_ipmi_plugin.get_inventory_size       = IpmiGetInventorySize;
       oh_ipmi_plugin.get_inventory_info       = IpmiGetInventoryInfo;
       oh_ipmi_plugin.get_hotswap_state        = IpmiGetHotswapState;
       oh_ipmi_plugin.set_hotswap_state        = IpmiSetHotswapState;
       oh_ipmi_plugin.request_hotswap_action   = IpmiRequestHotswapAction;
       oh_ipmi_plugin.get_power_state          = IpmiGetPowerState;
       oh_ipmi_plugin.set_power_state          = IpmiSetPowerState;
       oh_ipmi_plugin.get_indicator_state      = IpmiGetIndicatorState;
       oh_ipmi_plugin.set_indicator_state      = IpmiSetIndicatorState;
       oh_ipmi_plugin.control_parm             = IpmiControlParm;
       oh_ipmi_plugin.get_reset_state          = IpmiGetResetState;
       oh_ipmi_plugin.set_reset_state          = IpmiSetResetState;

       oh_ipmi_plugin_init = true;
     }
 
  *pp = &oh_ipmi_plugin;
  return 0;
}

int ipmidirect_get_interface(void **pp, const uuid_t uuid) __attribute__ ((alias("get_interface")));

}


static unsigned int
GetIntNotNull( GHashTable *handler_config, const char *str, unsigned int def = 0 )
{
  const char *value = (const char *)g_hash_table_lookup(handler_config, str );

  if ( !value )
       return def;

  unsigned int v = strtol( value, 0, 0 );

  if ( v == 0 )
       return def;

  return v;
}


cIpmi::cIpmi()
  : m_magic( dIpmiMagic ),
    m_handler( 0 )
{
}


cIpmi::~cIpmi()
{
}


void
cIpmi::SetHandler( oh_handler_state *handler )
{
  assert( m_handler == 0 );
  m_handler = handler;
}


// wrapper class move async events to domain
class cIpmiConLanDomain : public cIpmiConLan
{
  cIpmiDomain *m_domain;

public:
  cIpmiConLanDomain( cIpmiDomain *domain, 
                     unsigned int timeout, int log_level,
                     struct in_addr addr, int port,
                     tIpmiAuthType auth, tIpmiPrivilege priv, 
                     char *user, char *passwd )
    : cIpmiConLan( timeout, log_level, addr, port, auth, priv, 
                   user, passwd ),
      m_domain( domain )
  {
  }

  virtual ~cIpmiConLanDomain()
  {
  }

  virtual void HandleAsyncEvent( const cIpmiAddr &addr, const cIpmiMsg &msg )
  {
    m_domain->HandleAsyncEvent( addr, msg );
  }
};


// wrapper class move async events to domain
class cIpmiConSmiDomain : public cIpmiConSmi
{
  cIpmiDomain *m_domain;

public:
  cIpmiConSmiDomain( cIpmiDomain *domain, 
                     unsigned int timeout, int log_level, int if_num )
    : cIpmiConSmi( timeout, log_level, if_num ), m_domain( domain )
  {
  }

  virtual ~cIpmiConSmiDomain()
  {
  }

  virtual void HandleAsyncEvent( const cIpmiAddr &addr, const cIpmiMsg &msg )
  {
    m_domain->HandleAsyncEvent( addr, msg );
  }
};


cIpmiCon *
cIpmi::AllocConnection( GHashTable *handler_config )
{
  // default is 5s for IPMI
  m_con_ipmi_timeout = GetIntNotNull( handler_config, "IpmiConnectionTimeout", 5000 );
  stdlog << "AllocConnection: IPMITimeout " << m_con_ipmi_timeout << " ms.\n";

   // default is 1s for ATCA systems
  m_con_atca_timeout = GetIntNotNull( handler_config, "AtcaConnectionTimeout", 1000 );
  stdlog << "AllocConnection: AtcaTimeout " << m_con_atca_timeout << " ms.\n";

  // outstanding messages 0 => read from BMC/ShMc
  m_max_outstanding = GetIntNotNull( handler_config, "MaxOutstanding", 0 );

  if ( m_max_outstanding > 256 )
       m_max_outstanding = 256;

  stdlog << "AllocConnection: Max Outstanding IPMI messages "
         << m_max_outstanding << ".\n";

  const char *name = (const char *)g_hash_table_lookup(handler_config, "name");

  if ( !name )
     {
       stdlog << "Empty parameter !\n";
       return 0;
     }

  stdlog << "IpmiAllocConnection: connection name = '" << name << "'.\n";

  if ( !strcmp( name, "lan" ) || !strcmp( name, "rmcp" ) )
     {
       const char     *addr;
       struct in_addr  lan_addr;
       int             lan_port   = dIpmiConLanStdPort;
       tIpmiAuthType   auth       = eIpmiAuthTypeNone;
       tIpmiPrivilege  priv       = eIpmiPrivilegeAdmin;
       char            user[32]   = "";
       char            passwd[32] = "";
       char           *value;
       struct hostent *ent;

       // Address
       addr = (const char *)g_hash_table_lookup(handler_config, "addr");

       if ( !addr )
          {
            stdlog << "TCP/IP address missing in config file !\n";
            return 0;
          }

       stdlog << "IpmiAllocConnection: addr = '" << addr << "'.\n";
       ent = gethostbyname( addr );

       if ( !ent )
          {
            stdlog << "Unable to resolve IPMI LAN address: " << addr << " !\n";
            return 0;
          }

       memcpy( &lan_addr, ent->h_addr_list[0], ent->h_length );
       unsigned int a = *(unsigned int *)(void *)ent->h_addr_list[0];

       stdlog << "Using host at "
              << (int)(a & 0xff) << "."
              << (int)((a >> 8 ) & 0xff) << "." 
              << (int)((a >> 16) & 0xff) << "."
              << (int)((a >> 24) & 0xff) << ".\n";

       // Port
       lan_port = GetIntNotNull( handler_config, "port", 623 );

       stdlog << "IpmiAllocConnection: port = " << lan_port << ".\n";

       // Authentication type
       value = (char *)g_hash_table_lookup( handler_config, "auth_type" );

       if ( value )
          {
            if ( !strcmp( value, "none" ) )
                 auth = eIpmiAuthTypeNone;
            else if ( !strcmp( value, "straight" ) )
                 auth = eIpmiAuthTypeStraight;
            else if ( !strcmp( value, "md2" ) )
#ifdef HAVE_OPENSSL_MD2_H
                 auth = eIpmiAuthTypeMd2;
#else
	       {
		 stdlog << "MD2 is not supported. Please install SSL and recompile.\n";
		 return 0;
	       }
#endif
            else if ( !strcmp( value, "md5" ) )
#ifdef HAVE_OPENSSL_MD5_H
                 auth = eIpmiAuthTypeMd5;
#else
	       {
		 stdlog << "MD5 is not supported. Please install SSL and recompile.\n";
		 return 0;
	       }
#endif
            else
               {
                 stdlog << "Invalid IPMI LAN authenication method '" << value << "' !\n";
                 return 0;
               }
          }

       stdlog << "IpmiAllocConnection: authority: " << value << "(" << auth << ").\n";

       // Priviledge
       value = (char *)g_hash_table_lookup(handler_config, "auth_level" );

       if ( value )
          {
            if ( !strcmp( value, "callback" ) )
                 priv = eIpmiPrivilegeCallback;
            else if ( !strcmp( value, "user" ) )
                 priv = eIpmiPrivilegeUser;
            else if ( !strcmp( value, "operator" ) )
                 priv = eIpmiPrivilegeOperator;
            else if ( !strcmp( value, "admin" ) )
                 priv = eIpmiPrivilegeAdmin;
            else
               {
                 stdlog << "Invalid authentication method '" << value << "' !\n";
                 return 0;
               }
          }

       stdlog << "IpmiAllocConnection: priviledge = " << value << "(" << priv << ").\n";

       // User Name
       value = (char *)g_hash_table_lookup( handler_config, "username" ); 

       if ( value )
            strncpy( user, value, 32);

       stdlog << "IpmiAllocConnection: user = " << user << ".\n";

       // Password
       value = (char *)g_hash_table_lookup( handler_config, "password" );

       if ( value )
            strncpy( passwd, value, 32 );

       stdlog << "IpmiAllocConnection: password = " << user << ".\n";

       return new cIpmiConLanDomain( this, m_con_ipmi_timeout, dIpmiConLogAll,
				     lan_addr, lan_port, auth, priv,
                                     user, passwd );
     }
  else if ( !strcmp( name, "smi" ) )
     {
       const char *addr = (const char *)g_hash_table_lookup(handler_config, "addr");

       int if_num = 0;

       if ( addr )
            if_num = strtol( addr, 0, 10 );

       stdlog << "IpmiAllocConnection: interface number = " << if_num << ".\n";

       return new cIpmiConSmiDomain( this, dIpmiConLogAll, m_con_ipmi_timeout, if_num );
     }

  stdlog << "Unknown connection type: " << name << " !\n";

  return 0;
}


void
cIpmi::AddHpiEvent( oh_event *event )
{
  m_event_lock.Lock();

  assert( m_handler );
  m_handler->eventq = g_slist_append( m_handler->eventq, event );

/*
  stdlog << "event ";

  switch( event->type )
     {
       case oh_event::OH_ET_RESOURCE:
	    {
	      SaHpiRptEntryT *entry = &event->u.res_event.entry;
	      stdlog << "resource: " << entry->ResourceId << " ";
	      oh_print_ep( &entry->ResourceEntity, 4 );
	    }

	    break;

       case oh_event::OH_ET_RESOURCE_DEL:
	    {
	      stdlog << "resource del: " << event->u.res_del_event.resource_id << " ";
	      SaHpiRptEntryT *entry = oh_get_resource_by_id( GetHandler()->rptcache,
							     event->u.res_del_event.resource_id );
	      assert( entry );
	      
	      oh_print_ep( &entry->ResourceEntity, 4);
	    }

	    break;

       case oh_event::OH_ET_RDR:
	    stdlog << "rdr: \n";
	    break;

       case oh_event::OH_ET_RDR_DEL:
	    stdlog << "rdr del: \n";
	    break;

       case oh_event::OH_ET_HPI:
	    stdlog << "hpi: \n";
	    break;
     }
*/

  m_event_lock.Unlock();
}


const cIpmiEntityPath &
cIpmi::EntityRoot()
{
  return m_entity_root;
}


oh_handler_state *
cIpmi::GetHandler()
{
  return m_handler;
}


SaHpiRptEntryT *
cIpmi::FindResource( SaHpiResourceIdT rid )
{
  assert( m_handler );

  return oh_get_resource_by_id( m_handler->rptcache, rid);
}


void
cIpmi::IfEnter()
{
  ReadLock();
}


void
cIpmi::IfLeave()
{
  ReadUnlock();
}


bool
cIpmi::GetParams( GHashTable *handler_config )
{
  // get mcs to scan
  char str[100];

  for( unsigned int i = 1; i < 0xf1; i++ )
     {
       sprintf( str, "MC%02x", i );

       char *value = (char *)g_hash_table_lookup( handler_config,
                                                  str );

       if ( value == 0 )
          {
            sprintf( str, "MC%02X", i );

            value = (char *)g_hash_table_lookup( handler_config, str );
          }

       if ( value == 0 )
            continue;

       unsigned int properties = 0;

       char *tokptr;
       char *tok = strtok_r( value, " \t\n", &tokptr );

       while( tok )
          {
            if ( !strcmp( tok, "initial_discover" ) )
                 properties |= dIpmiMcThreadInitialDiscover;
            else if ( !strcmp( tok, "poll_alive" ) )
                 properties |= dIpmiMcThreadPollAliveMc;
            else if ( !strcmp(tok, "poll_dead" ) )
                 properties |= dIpmiMcThreadPollDeadMc;
            else
                 stdlog << "unknown propertiy for MC " << (unsigned char)i
                        << ": " << tok << " !\n";

            tok = strtok_r( 0, " \t\n", &tokptr );
          }

       if ( properties == 0 )
            continue;

       char pp[256] = "";

       if ( properties & dIpmiMcThreadInitialDiscover )
            strcat( pp, " initial_discover" );

       if ( properties &  dIpmiMcThreadPollAliveMc )
            strcat( pp, " poll_alive" );

       if ( properties &  dIpmiMcThreadPollDeadMc )
            strcat( pp, " poll_dead" );

       stdlog << "MC " << (unsigned char)i << " properties: " << pp << ".\n";

       // NewFruInfo( i, 0, slot, entity, dIpmiMcTypeBitAll );
     }

  return true;
}


bool
cIpmi::IfOpen( GHashTable *handler_config )
{
  const char *entity_root = (const char *)g_hash_table_lookup( handler_config, "entity_root" );

  if ( !entity_root )
     {
       dbg( "entity_root is missing in config file" );
       return false;
     }

  if ( !m_entity_root.FromString( entity_root ) )
     {
       dbg( "cannot decode entity path string" );
       return false;
     }

  cIpmiCon *con = AllocConnection( handler_config );

  if ( !con )
     {
       stdlog << "IPMI cannot alloc connection !\n";
       return false;
     }

  if ( !GetParams( handler_config ) )
     {
       delete con;
       return false;
     }

  bool rv = con->Open();

  if ( rv == false )
     {
       stdlog << "IPMI open connection fails !\n";

       delete con;

       return false;
     }

  if ( !Init( con ) )
     {
       IfClose();
       return false;
     }

  return true;
}


void
cIpmi::IfClose()
{
  Cleanup();

  if ( m_con )
     {
       delete m_con;
       m_con = 0;
     }
}


int
cIpmi::IfGetEvent( oh_event *event, const timeval & /*timeout*/ )
{
  int rv = 0;

  m_event_lock.Lock();

  if ( g_slist_length( m_handler->eventq ) > 0 )
     {
       memcpy( event, m_handler->eventq->data, sizeof( oh_event ) );
       g_free( m_handler->eventq->data );
       m_handler->eventq = g_slist_remove_link( m_handler->eventq, m_handler->eventq );
       rv = 1;
     }

  m_event_lock.Unlock();

  return rv;
}


SaErrorT
cIpmi::IfDiscoverResources()
{
  dbg( "ipmidirect discover_resources");

  bool loop;

  do
     {
       usleep( 10000 );

       m_initial_discover_lock.Lock();
       loop = m_initial_discover ? true : false;
       m_initial_discover_lock.Unlock();
     }
  while( loop );

  return SA_OK;
}


SaErrorT
cIpmi::IfSetResourceSeverity( cIpmiResource *ent, SaHpiSeverityT sev )
{
  // TODO: add real functionality

  // change severity in plugin cache
  SaHpiRptEntryT *rptentry = oh_get_resource_by_id( ent->Domain()->GetHandler()->rptcache, 
                                                    ent->m_resource_id );
  assert( rptentry );

  rptentry->ResourceSeverity = sev;

  // send update event
  struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "out of space !\n";
       return SA_ERR_HPI_OUT_OF_SPACE;
     }

  memset( e, 0, sizeof( struct oh_event ) );
  e->type               = oh_event::OH_ET_RESOURCE;
  e->u.res_event.entry = *rptentry;

  AddHpiEvent( e );

  return SA_OK;
}


SaErrorT
cIpmi::IfControlParm( cIpmiResource * /*res*/, SaHpiParmActionT act )
{
  // TODO: implementation
  switch( act )
     {
       case SAHPI_DEFAULT_PARM:
            break;

       case SAHPI_SAVE_PARM:
            break;

       case SAHPI_RESTORE_PARM:
            break;
     }
  
  return SA_OK;
}
