/** 
 * @file    new_sim.cpp
 *
 * The file includes the interface to the openhpi abi, an 
 * implementation of the class NewSimulator and some HPI API 
 * functions.
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.1
 * @date    2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 * 
 * The new Simulator plugin is adapted from the ipmidirect plugin.
 * Thanks to \n
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>\n
 *     Pierre Sangouard  <psangouard@eso-tech.com>\n
 *     Andy Cress        <arcress@users.sourceforge.net>
 *  
 **/

#include <netdb.h>
#include <errno.h>

#include <oh_error.h>

#include "new_sim.h"
#include "new_sim_utils.h"

/// Definition of eventlog size
#define NEWSIM_EVENTLOG_ENTRIES 256

/**
 * @fn static NewSimulator *VerifyNewSimulator( void *hnd )
 * 
 * Function for verification of a handler. It returns a pointer on 
 * the equivalent NewSimulator object.
 * 
 * @param hnd pointer on a oh_handler
 * @return pointer on a NewSimulator object
 **/
static NewSimulator *VerifyNewSimulator( void *hnd ) {
   if (!hnd)
      return 0;
      
   oh_handler_state *handler = (oh_handler_state *)hnd;
   NewSimulator *newsim = (NewSimulator *)handler->data;

   if ( !newsim ) {
      return 0;
   }

   if ( !newsim->CheckMagic() ) {
      return 0;
   }

   if ( !newsim->CheckHandler( handler ) ) {
      return 0;
   }

   return newsim;
}


/**
 * 
 * Function for verification of handler and the sensor data in the cache.
 * It returns a pointer on the equivalent NewSimulatorSensor object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param num sensor id 
 * @param newsim pointer on the address of a newsim object
 * @return pointer on NewSimulatorSensor object equivalent to the input
 **/
static NewSimulatorSensor *VerifySensorAndEnter( void *hnd, 
                                                  SaHpiResourceIdT rid, 
                                                  SaHpiSensorNumT num,
                                                  NewSimulator *&newsim ) {
   newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) return 0;

   newsim->IfEnter();

   SaHpiRdrT *rdr = oh_get_rdr_by_type( newsim->GetHandler()->rptcache,
                                        rid, SAHPI_SENSOR_RDR, num );
   if ( !rdr ) {
     newsim->IfLeave();
     return 0;
   }

   NewSimulatorSensor *sensor = (NewSimulatorSensor *)oh_get_rdr_data( newsim->GetHandler()->rptcache,
                                                        rid, rdr->RecordId );
   if ( !sensor ) {
      newsim->IfLeave();
      return 0;
   }

   if ( !newsim->VerifySensor( sensor ) ) {
      newsim->IfLeave();
      return 0;
   }

   return sensor;
}


/**
 * Function for verification of handler and the control data in the cache.
 * It returns a pointer on the equivalent NewSimulatorControl object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param num control id 
 * @param newsim pointer on the address of a newsim object
 * @return pointer on NewSimulatorControl object equivalent to the input
 **/
static NewSimulatorControl *VerifyControlAndEnter( void *hnd, 
                                                    SaHpiResourceIdT rid, 
                                                    SaHpiCtrlNumT num,
                                                    NewSimulator *&newsim ) {
  newsim = VerifyNewSimulator( hnd );

  if ( !newsim ) return 0;

  newsim->IfEnter();
  
  SaHpiRdrT *rdr = oh_get_rdr_by_type( newsim->GetHandler()->rptcache,
                                       rid, SAHPI_CTRL_RDR, num );
  if ( !rdr ) {
    newsim->IfLeave();
    return 0;
  }
  
  NewSimulatorControl *control = (NewSimulatorControl *)oh_get_rdr_data( newsim->GetHandler()->rptcache,
                                                           rid, rdr->RecordId );
  if ( !control ) {
    newsim->IfLeave();
    return 0;
  }

  if ( !newsim->VerifyControl( control ) ) {
    newsim->IfLeave();
    return 0;
  }

  return control;
}

/**
 * Function for verification of handler and the annunciator data in the cache.
 * It returns a pointer on the equivalent NewSimulatorAnnunciator object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param num annunciator id 
 * @param newsim pointer on the address of a newsim object
 * @return pointer on NewSimulatorAnnunciator object equivalent to the input
 **/
static NewSimulatorAnnunciator *VerifyAnnunciatorAndEnter( void *hnd, 
                                                  SaHpiResourceIdT rid, 
                                                  SaHpiAnnunciatorNumT num,
                                                  NewSimulator *&newsim ) {
   newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) return 0;

   newsim->IfEnter();

   SaHpiRdrT *rdr = oh_get_rdr_by_type( newsim->GetHandler()->rptcache,
                                        rid, SAHPI_ANNUNCIATOR_RDR, num );
   if ( !rdr ) {
     newsim->IfLeave();
     return 0;
   }

   NewSimulatorAnnunciator *annunciator = 
          (NewSimulatorAnnunciator *)oh_get_rdr_data( newsim->GetHandler()->rptcache,
                                                      rid, rdr->RecordId );
   if ( !annunciator ) {
      newsim->IfLeave();
      return 0;
   }

   if ( !newsim->VerifyAnnunciator( annunciator ) ) {
      newsim->IfLeave();
      return 0;
   }

   return annunciator;
}

/**
 * Function for verification of handler and resource data in the cache.
 * It returns a pointer on the equivalent NewSimulatorResource object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param newsim pointer on the address of a newsim object
 * @return pointer on NewSimulatorAnnunciator object equivalent to the input
 **/
static NewSimulatorResource *VerifyResourceAndEnter( void *hnd, SaHpiResourceIdT rid, 
                                                      NewSimulator *&newsim ) {
   newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) return 0;
     
   newsim->IfEnter();

   NewSimulatorResource *res = (NewSimulatorResource *)oh_get_resource_data( newsim->GetHandler()->rptcache, rid );

   if ( !res ) {
      newsim->IfLeave();
      return 0;
   }

   if ( !newsim->VerifyResource( res ) ) {
       newsim->IfLeave();
       return 0;
   }

   return res;
}

/**
 * Function for verification of handler and inventory data in the cache.
 * It returns a pointer on the equivalent NewSimulatorInventory object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param idrid inventory id
 * @param newsim pointer on the address of a newsim object
 * 
 * @return pointer on NewSimulatorInventory object equivalent to the input
 **/
static NewSimulatorInventory * VerifyInventoryAndEnter( void *hnd, SaHpiResourceIdT rid, 
                                                         SaHpiIdrIdT idrid,
                                                         NewSimulator *&newsim ) {
  newsim = VerifyNewSimulator( hnd );

  if ( !newsim ) return 0;

  newsim->IfEnter();

  SaHpiRdrT *rdr = oh_get_rdr_by_type( newsim->GetHandler()->rptcache,
                                       rid, SAHPI_INVENTORY_RDR, idrid );
  if ( !rdr ) {
    newsim->IfLeave();
    return 0;
  }

  NewSimulatorInventory *inv = (NewSimulatorInventory *)oh_get_rdr_data( newsim->GetHandler()->rptcache,
                                                                         rid, rdr->RecordId );
  if ( !inv ) {
    newsim->IfLeave();
    return 0;
  }

  if ( !newsim->VerifyInventory( inv ) ) {
    newsim->IfLeave();
    return 0;
  }

  return inv;
}

/**
 * Function for verification of handler and watchdog data in the cache.
 * It returns a pointer on the equivalent NewSimulatorWatchdog object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param num watchdog number
 * @param newsim pointer on the address of a newsim object
 * 
 * @return pointer on NewSimulatorWatchdog object equivalent to the input
 **/
static NewSimulatorWatchdog *VerifyWatchdogAndEnter( void *hnd, SaHpiResourceIdT rid, 
                                                      SaHpiWatchdogNumT num,
                                                      NewSimulator *&newsim ) {
  newsim = VerifyNewSimulator( hnd );

  if ( !newsim ) return 0;

  newsim->IfEnter();

  SaHpiRdrT *rdr = oh_get_rdr_by_type( newsim->GetHandler()->rptcache,
                                       rid, SAHPI_WATCHDOG_RDR, num );
  if ( !rdr ) {
    newsim->IfLeave();
    return 0;
  }

  NewSimulatorWatchdog *watchdog = (NewSimulatorWatchdog *)oh_get_rdr_data( newsim->GetHandler()->rptcache,
                                                                            rid, rdr->RecordId );
  if ( !watchdog ) {
    newsim->IfLeave();
    return 0;
  }

  if ( !newsim->VerifyWatchdog( watchdog ) ) {
    newsim->IfLeave();
    return 0;
  }

  return watchdog;
}


/**
 * Function for verification of handler and fumi data in the cache.
 * It returns a pointer on the equivalent NewSimulatorFumi object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param num fumi number
 * @param newsim pointer on the address of a newsim object
 * 
 * @return pointer on NewSimulatorFumi object equivalent to the input
 **/
static NewSimulatorFumi *VerifyFumiAndEnter( void *hnd, SaHpiResourceIdT rid, 
                                              SaHpiFumiNumT num,
                                              NewSimulator *&newsim ) {
  newsim = VerifyNewSimulator( hnd );

  if ( !newsim ) return 0;

  newsim->IfEnter();

  SaHpiRdrT *rdr = oh_get_rdr_by_type( newsim->GetHandler()->rptcache,
                                       rid, SAHPI_FUMI_RDR, num );
  if ( !rdr ) {
    newsim->IfLeave();
    return 0;
  }

  NewSimulatorFumi *fumi = (NewSimulatorFumi *)oh_get_rdr_data( newsim->GetHandler()->rptcache,
                                                                rid, rdr->RecordId );
  if ( !fumi ) {
    newsim->IfLeave();
    return 0;
  }

  if ( !newsim->VerifyFumi( fumi ) ) {
    newsim->IfLeave();
    return 0;
  }

  return fumi;
}


/**
 * Function for verification of handler and dimi data in the cache.
 * It returns a pointer on the equivalent NewSimulatorDimi object.\n
 * 
 * @param hnd pointer on a oh_handler
 * @param rid resource id
 * @param num dimi number
 * @param newsim pointer on the address of a newsim object
 * 
 * @return pointer on NewSimulatorDimi object equivalent to the input
 **/
static NewSimulatorDimi *VerifyDimiAndEnter( void *hnd, SaHpiResourceIdT rid, 
                                              SaHpiDimiNumT num,
                                              NewSimulator *&newsim ) {
  newsim = VerifyNewSimulator( hnd );

  if ( !newsim ) return 0;

  newsim->IfEnter();

  SaHpiRdrT *rdr = oh_get_rdr_by_type( newsim->GetHandler()->rptcache,
                                       rid, SAHPI_DIMI_RDR, num );
  if ( !rdr ) {
    newsim->IfLeave();
    return 0;
  }

  NewSimulatorDimi *dimi = (NewSimulatorDimi *)oh_get_rdr_data( newsim->GetHandler()->rptcache,
                                                                rid, rdr->RecordId );
  if ( !dimi ) {
    newsim->IfLeave();
    return 0;
  }

  if ( !newsim->VerifyDimi( dimi ) ) {
    newsim->IfLeave();
    return 0;
  }

  return dimi;
}



/*******************************************************
 * 
 * 
 * @name New Plugin interface
 * Implementation of the alias definitions. Inside the functions the corresponding object methods
 * are called. 
 *
 */
//@{
// new plugin_loader
extern "C" {

/**
 * Alias for @ref öh_open(), implemented by @ref NewSimulatorOpen().
 **/
static void * NewSimulatorOpen( GHashTable *, unsigned int, oh_evt_queue * ) __attribute__((used));

/**
 * @fn NewSimulatorOpen( GHashTable *handler_config, unsigned int hid, 
 *                                     oh_evt_queue *eventq )
 * 
 * First function which is called at start of the plugin. It opens the 
 * logfiles, allocates and initializes the handler and a NewSim object.\n
 * For reading the configuration data the method NewSimulator::IfOpen is called.
 * 
 * @param handler_config pointer on the oh hash table with the configuration data
 * @param hid id of the handler
 * @param eventq pointer on a eventqueue
 * 
 * @return pointer on handler if everything works successfully.  
 **/
static void * NewSimulatorOpen( GHashTable *handler_config, unsigned int hid, 
                                  oh_evt_queue *eventq ) {
   // open log
   const char  *logfile = 0;
   int          max_logfiles = 10;
   char         *tmp;
   int          lp = dIpmiLogPropNone;

   dbg( "NewSimulatorOpen" );

   if ( !handler_config ) {
      err( "No config file provided.....ooops!" );
      return 0;
   }

   logfile = (char *)g_hash_table_lookup( handler_config, "logfile" );
   tmp = (char *)g_hash_table_lookup( handler_config, "logfile_max" );

   if ( tmp ) max_logfiles = atoi( tmp );

   tmp = (char *)g_hash_table_lookup( handler_config, "logflags" );

   if ( tmp ) {
      if ( strstr( tmp, "StdOut" ) || strstr( tmp, "stdout" ) )
         lp |= dIpmiLogStdOut;

      if ( strstr( tmp, "StdError" ) || strstr( tmp, "stderr" ) )
         lp |= dIpmiLogStdErr;

      if ( strstr( tmp, "File" ) || strstr( tmp, "file" ) ) {
         lp |= dIpmiLogLogFile;
         if ( logfile == 0 )
            logfile = dDefaultLogfile;
      }
   }

   stdlog.Open( lp, logfile, max_logfiles );
   stdlog.Time( true );

   // create domain
   NewSimulator *newsim = new NewSimulator;

   // allocate handler
   oh_handler_state *handler = (oh_handler_state *)g_malloc0( sizeof( oh_handler_state ) );

   if ( !handler ) {
      err("cannot allocate handler");
      delete newsim;
      stdlog.Close();

      return 0;
   }

   handler->data     = newsim;
   handler->rptcache = (RPTable *)g_malloc0( sizeof( RPTable ) );
   if ( !handler->rptcache ) {
      err("cannot allocate RPT cache");
      g_free( handler );
      delete newsim;
      stdlog.Close();

      return 0;
   }

   /* initialize the event log */
   handler->elcache = oh_el_create(NEWSIM_EVENTLOG_ENTRIES);
   if (!handler->elcache) {
      err("Event log creation failed");
      g_free(handler->rptcache);
      g_free(handler);
      delete newsim;
      stdlog.Close();
      
      return NULL;
   }
        
   handler->config   = handler_config;
   handler->hid = hid;
   handler->eventq = eventq;

   newsim->SetHandler( handler );

   if ( !newsim->IfOpen( handler_config ) ) {
      newsim->IfClose();
      delete newsim;
      oh_flush_rpt( handler->rptcache );
      g_free( handler->rptcache );
      g_free( handler );
      stdlog.Close();

     return 0;
   }

   return handler;
}

/**
 * Alias for @ref öh_close(), implemented by @ref NewSimulatorClose().
 **/
static void NewSimulatorClose( void * ) __attribute__((used));

/**
 * @relate NewSimulatorClose
 * Close the plugin and clean up the allocated memory. 
 *  
 * @param hnd pointer on handler
 **/
static void NewSimulatorClose( void *hnd ) {
  dbg( "NewSimulatorClose" );

  NewSimulator *newsim = VerifyNewSimulator( hnd );

  if ( !newsim ) {
     return;
  }
/* Commenting this code block due to the multi-domain changes
 * in the infrastructure.
 * (Renier Morales 11/21/06)
  if ( ipmi->DomainId() != oh_get_default_domain_id() )
  {
      stdlog << "Releasing domain id " << ipmi->DomainId() << "\n";

      SaErrorT rv = oh_request_domain_delete( ipmi->HandlerId(), ipmi->DomainId() );

      if ( rv != SA_OK )
          stdlog << "oh_request_domain_delete error " << rv << "\n";
  }*/

  newsim->IfClose();
  newsim->CheckLock();
  delete newsim;
  
  oh_handler_state *handler = (oh_handler_state *)hnd;

  if ( handler->rptcache ) {
     oh_flush_rpt( handler->rptcache );
     g_free( handler->rptcache );
  }

  g_free( handler );

  // close logfile
  stdlog.Close();
}


/**
 * Interface for GetEvent. 
 * Inside the function the method NewSimulator::IfGetEvent is called. 
 *  
 * @param hnd pointer on handler
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetEvent( void * ) __attribute__((used));

static SaErrorT NewSimulatorGetEvent( void *hnd ) {
   dbg( "NewSimulatorGetEvent" );
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );
   struct oh_event event;

   if ( !newsim ) {
     return SA_ERR_HPI_INTERNAL_ERROR;
   }

   // there is no need to get a lock because
   // the event queue has its own lock
   SaErrorT rv = newsim->IfGetEvent( &event );

   return rv;
}


/**
 * Interface for DiscoverResource. 
 * Inside the function the method NewSimulator::IfDiscoverResources is called. 
 *  
 * @param hnd pointer on handler
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorDiscoverResources( void * ) __attribute__((used));

static SaErrorT NewSimulatorDiscoverResources( void *hnd ) {
   dbg( "NewSimulatorDiscoverResources" );
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   stdlog << "DBG: new_sim.cpp::NewSimulatorDiscoverResources let's go: " << hnd << "\n";

   SaErrorT rv = newsim->IfDiscoverResources();

   return rv;
}


/**
 * Interface for SetResourceTag. 
 * Inside the function the method NewSimulator::IfSetResourceTag is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param tag pointer on the SaHpiTextBufferT to be set in a resource
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetResourceTag( void *, SaHpiResourceIdT, SaHpiTextBufferT * ) __attribute__((used));

static SaErrorT NewSimulatorSetResourceTag( void *hnd, SaHpiResourceIdT id, 
                                             SaHpiTextBufferT *tag ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
     return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfSetResourceTag( res, tag );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetResourceSeverity. 
 * Inside the function the method NewSimulator::IfSetResourceSeverity is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param sev severity to be set in a resource
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetResourceSeverity( void *, SaHpiResourceIdT, SaHpiSeverityT ) __attribute__((used));

static SaErrorT NewSimulatorSetResourceSeverity( void *hnd, SaHpiResourceIdT id, 
                                                  SaHpiSeverityT sev ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
     return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfSetResourceSeverity( res, sev );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetSensorReading. 
 * Inside the function the method NewSimulatorSensor::GetSensorReading is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param data pointer on data structure to be filled
 * @param state pointer on state structure to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetSensorReading( void *,
                                               SaHpiResourceIdT id,
                                               SaHpiSensorNumT num,
                                               SaHpiSensorReadingT *data,
                                               SaHpiEventStateT *state ) __attribute__((used));

static SaErrorT NewSimulatorGetSensorReading( void *hnd,
                                               SaHpiResourceIdT id,
                                               SaHpiSensorNumT num,
                                               SaHpiSensorReadingT *data,
                                               SaHpiEventStateT *state ) {
   NewSimulator *newsim = 0;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
       return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = sensor->GetSensorReading( *data, *state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetSensorThresholds. 
 * Inside the function the method NewSimulatorSensorThreshold::GetThresholds is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param thres pointer on structure to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetSensorThresholds( void *hnd,
                                          SaHpiResourceIdT,
                                          SaHpiSensorNumT,
                                          SaHpiSensorThresholdsT * ) __attribute__((used));

static SaErrorT NewSimulatorGetSensorThresholds( void *hnd,
                                                  SaHpiResourceIdT id,
                                                  SaHpiSensorNumT  num,
                                                  SaHpiSensorThresholdsT *thres ) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = SA_ERR_HPI_INVALID_PARAMS;

   NewSimulatorSensorThreshold *t = dynamic_cast<NewSimulatorSensorThreshold *>( sensor );

   if ( t )
      rv = t->GetThresholds( *thres );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetSensorThresholds. 
 * Inside the function the method NewSimulatorSensorThreshold::SetThresholds is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param thres pointer on structure to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetSensorThresholds( void *,
                                                  SaHpiResourceIdT,
                                                  SaHpiSensorNumT,
                                                  const SaHpiSensorThresholdsT * ) __attribute__((used));

static SaErrorT NewSimulatorSetSensorThresholds( void *hnd,
                                                  SaHpiResourceIdT id,
                                                  SaHpiSensorNumT  num,
                                                  const SaHpiSensorThresholdsT *thres ) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = SA_ERR_HPI_INVALID_PARAMS;

   NewSimulatorSensorThreshold *t = dynamic_cast<NewSimulatorSensorThreshold *>( sensor );

   if ( t )
      rv = t->SetThresholds( *thres );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for GetSensorEnable. 
 * Inside the function the method NewSimulatorSensor::GetEnable is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param enable pointer on Bool to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetSensorEnable( void *,
                                              SaHpiResourceIdT,
                                              SaHpiSensorNumT,
                                              SaHpiBoolT * ) __attribute__((used));

static SaErrorT NewSimulatorGetSensorEnable( void *hnd,
                                              SaHpiResourceIdT id,
                                              SaHpiSensorNumT  num,
                                              SaHpiBoolT       *enable ) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = sensor->GetEnable( *enable );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetSensorEnable. 
 * Inside the function the method NewSimulatorSensor::SetEnable is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param enable Bool to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetSensorEnable( void *,
                                              SaHpiResourceIdT,
                                              SaHpiSensorNumT,
                                              SaHpiBoolT ) __attribute__((used));

static SaErrorT NewSimulatorSetSensorEnable( void *hnd,
                                              SaHpiResourceIdT id,
                                              SaHpiSensorNumT  num,
                                              SaHpiBoolT       enable ) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = sensor->SetEnable( enable );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetSensorEventEnables. 
 * Inside the function the method NewSimulatorSensor::GetEventEnables is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param enables pointer on Bool to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetSensorEventEnables( void *,
                                                    SaHpiResourceIdT,
                                                    SaHpiSensorNumT,
                                                    SaHpiBoolT * ) __attribute__((used));

static SaErrorT NewSimulatorGetSensorEventEnables( void *hnd,
                                                    SaHpiResourceIdT id,
                                                    SaHpiSensorNumT  num,
                                                    SaHpiBoolT       *enables ) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = sensor->GetEventEnables( *enables );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for SetSensorEventEnables. 
 * Inside the function the method NewSimulatorSensor::SetEventEnables is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param enables on Bool to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetSensorEventEnables( void *,
                                                    SaHpiResourceIdT,
                                                    SaHpiSensorNumT,
                                                    SaHpiBoolT ) __attribute__((used));

static SaErrorT NewSimulatorSetSensorEventEnables( void *hnd,
                                                    SaHpiResourceIdT id,
                                                    SaHpiSensorNumT  num,
                                                    SaHpiBoolT       enables ) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = sensor->SetEventEnables( enables );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetSensorEventMasks. 
 * Inside the function the method NewSimulatorSensor::GetEventMasks is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param AssertEventMask pointer on assertion mask to be filled
 * @param DeassertEventMask pointer on deassertion mask to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetSensorEventMasks( void *,
                                                  SaHpiResourceIdT,
                                                  SaHpiSensorNumT,
                                                  SaHpiEventStateT *,
                                                  SaHpiEventStateT * ) __attribute__((used));

static SaErrorT NewSimulatorGetSensorEventMasks( void *hnd,
                                                  SaHpiResourceIdT id,
                                                  SaHpiSensorNumT  num,
                                                  SaHpiEventStateT *AssertEventMask,
                                                  SaHpiEventStateT *DeassertEventMask) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = sensor->GetEventMasks( *AssertEventMask, *DeassertEventMask );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetSensorEventMasks. 
 * Inside the function the method NewSimulatorSensor::SetEventMasks is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of sensor
 * @param AssertEventMask assertion mask to be set
 * @param DeassertEventMask deassertion mask to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetSensorEventMasks( void *,
                                                  SaHpiResourceIdT,
                                                  SaHpiSensorNumT,
                                                  SaHpiSensorEventMaskActionT,
                                                  SaHpiEventStateT,
                                                  SaHpiEventStateT ) __attribute__((used));

static SaErrorT NewSimulatorSetSensorEventMasks( void *hnd,
                                                  SaHpiResourceIdT id,
                                                  SaHpiSensorNumT  num,
                                                  SaHpiSensorEventMaskActionT act,
                                                  SaHpiEventStateT AssertEventMask,
                                                  SaHpiEventStateT DeassertEventMask) {
   NewSimulator *newsim;
   NewSimulatorSensor *sensor = VerifySensorAndEnter( hnd, id, num, newsim );

   if ( !sensor )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = sensor->SetEventMasks( act, AssertEventMask, DeassertEventMask );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for GetControlState. 
 * Inside the function the method NewSimulatorControl::GetState is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of control
 * @param mode pointer on mode to be filled
 * @param state pointer on state to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetControlState( void *, SaHpiResourceIdT,
                                              SaHpiCtrlNumT,
                                              SaHpiCtrlModeT *,
                                              SaHpiCtrlStateT * ) __attribute__((used));

static SaErrorT NewSimulatorGetControlState( void *hnd, SaHpiResourceIdT id,
                                              SaHpiCtrlNumT num,
                                              SaHpiCtrlModeT *mode,
                                              SaHpiCtrlStateT *state ) {
   NewSimulator *newsim;
   NewSimulatorControl *control = VerifyControlAndEnter( hnd, id, num, newsim );

   if ( !control )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = control->GetState( *mode, *state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetControlState. 
 * Inside the function the method NewSimulatorControl::SetState is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of control
 * @param mode mode to be set
 * @param state pointer on state to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetControlState( void *, SaHpiResourceIdT,
                                              SaHpiCtrlNumT,
                                              SaHpiCtrlModeT,
                                              SaHpiCtrlStateT * ) __attribute__((used));

static SaErrorT NewSimulatorSetControlState( void *hnd, SaHpiResourceIdT id,
                                              SaHpiCtrlNumT num,
                                              SaHpiCtrlModeT mode,
                                              SaHpiCtrlStateT *state ) {
   NewSimulator *newsim;
   NewSimulatorControl *control = VerifyControlAndEnter( hnd, id, num, newsim );

   if ( !control )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = control->SetState( mode, *state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetNextAnnouncement. 
 * Inside the function the method NewSimulatorAnnunciator::GetNextAnnouncement is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of annunciator
 * @param severity severity flag
 * @param unAckOnly unacknowledge flag
 * @param ann pointer on announcement to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetNextAnnouncement(void *, 
                                       SaHpiResourceIdT, 
                                       SaHpiAnnunciatorNumT, 
                                       SaHpiSeverityT, 
                                       SaHpiBoolT, 
                                       SaHpiAnnouncementT *) __attribute__((used));
 
static SaErrorT NewSimulatorGetNextAnnouncement(void *hnd, SaHpiResourceIdT id,
                                       SaHpiAnnunciatorNumT num,
                                       SaHpiSeverityT       severity,
                                       SaHpiBoolT           unAckOnly,
                                       SaHpiAnnouncementT   *ann) {
   NewSimulator *newsim;
   NewSimulatorAnnunciator *annunciator = VerifyAnnunciatorAndEnter( hnd, id, num, newsim );

   if ( !annunciator )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = annunciator->GetNextAnnouncement( severity, unAckOnly, *ann );

   newsim->IfLeave();

   return rv;
}
                                       

/**
 * Interface for GetAnnouncement. 
 * Inside the function the method NewSimulatorAnnunciator::GetAnnouncement is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of annunciator
 * @param entryId number of announcement to be got
 * @param ann pointer on announcement to be filled
 * 
 * @return HPI error code
 **/                                    
static SaErrorT NewSimulatorGetAnnouncement(void *, SaHpiResourceIdT, 
                                       SaHpiAnnunciatorNumT, 
                                       SaHpiEntryIdT, 
                                       SaHpiAnnouncementT *) __attribute__((used));
 
static SaErrorT NewSimulatorGetAnnouncement(void *hnd, SaHpiResourceIdT id, 
                                       SaHpiAnnunciatorNumT  num, 
                                       SaHpiEntryIdT         entryId, 
                                       SaHpiAnnouncementT    *ann) {

   NewSimulator *newsim;
   NewSimulatorAnnunciator *annunciator = VerifyAnnunciatorAndEnter( hnd, id, num, newsim );

   if ( !annunciator )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = annunciator->GetAnnouncement( entryId, *ann );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for AckAnnouncement. 
 * Inside the function the method NewSimulatorAnnunciator::SetAcknowledge is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of annunciator
 * @param entryId number of announcement to be acknowledged
 * @param severity of announcements to be acknowledged
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorAckAnnouncement(void *, SaHpiResourceIdT, 
                                       SaHpiAnnunciatorNumT, 
                                       SaHpiEntryIdT, 
                                       SaHpiSeverityT) __attribute__((used));
 
static SaErrorT NewSimulatorAckAnnouncement(void *hnd, SaHpiResourceIdT id, 
                                       SaHpiAnnunciatorNumT  num, 
                                       SaHpiEntryIdT         entryId, 
                                       SaHpiSeverityT        severity) {

   NewSimulator *newsim;
   NewSimulatorAnnunciator *annunciator = VerifyAnnunciatorAndEnter( hnd, id, num, newsim );

   if ( !annunciator )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = annunciator->SetAcknowledge( entryId, severity );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for AddAnnouncement. 
 * Inside the function the method NewSimulatorAnnunciator::AddAnnouncement is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of annunciator
 * @param ann pointer on announcement to be added
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorAddAnnouncement(void *, SaHpiResourceIdT, 
                                       SaHpiAnnunciatorNumT, 
                                       SaHpiAnnouncementT *) __attribute__((used));

static SaErrorT NewSimulatorAddAnnouncement(void *hnd, SaHpiResourceIdT id, 
                                       SaHpiAnnunciatorNumT  num, 
                                       SaHpiAnnouncementT    *ann) {
   NewSimulator *newsim;
   NewSimulatorAnnunciator *annunciator = VerifyAnnunciatorAndEnter( hnd, id, num, newsim );

   if ( !annunciator )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = annunciator->AddAnnouncement( *ann );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for DelAnnouncement. 
 * Inside the function the method NewSimulatorAnnunciator::DelAnnouncement is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of annunciator
 * @param entryId number of announcement to be deleted
 * @param severity of announcements to be deleted
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorDelAnnouncement(void *, SaHpiResourceIdT, 
                                       SaHpiAnnunciatorNumT, 
                                       SaHpiEntryIdT, 
                                       SaHpiSeverityT) __attribute__((used));

static SaErrorT NewSimulatorDelAnnouncement(void *hnd, SaHpiResourceIdT id, 
                                       SaHpiAnnunciatorNumT  num, 
                                       SaHpiEntryIdT         entryId, 
                                       SaHpiSeverityT        severity) {

   NewSimulator *newsim;
   NewSimulatorAnnunciator *annunciator = VerifyAnnunciatorAndEnter( hnd, id, num, newsim );

   if ( !annunciator )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = annunciator->DeleteAnnouncement( entryId, severity );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetAnnMode. 
 * Inside the function the method NewSimulatorAnnunciator::GetMode is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of annunciator
 * @param mode pointer on mode to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetAnnMode(void *, SaHpiResourceIdT, 
                                       SaHpiAnnunciatorNumT,
                                       SaHpiAnnunciatorModeT *) __attribute__((used));

static SaErrorT NewSimulatorGetAnnMode(void *hnd, SaHpiResourceIdT id, 
                                       SaHpiAnnunciatorNumT  num, 
                                       SaHpiAnnunciatorModeT *mode) {
                                       	
   NewSimulator *newsim;
   NewSimulatorAnnunciator *annunciator = VerifyAnnunciatorAndEnter( hnd, id, num, newsim );

   if ( !annunciator )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = annunciator->GetMode( *mode );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetAnnMode. 
 * Inside the function the method NewSimulatorAnnunciator::SetMode is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of annunciator
 * @param mode mode to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetAnnMode(void *, SaHpiResourceIdT, 
                                       SaHpiAnnunciatorNumT,
                                       SaHpiAnnunciatorModeT) __attribute__((used));

static SaErrorT NewSimulatorSetAnnMode(void *hnd, SaHpiResourceIdT id, 
                                       SaHpiAnnunciatorNumT  num,
                                       SaHpiAnnunciatorModeT mode) {
                                       	
   NewSimulator *newsim;
   NewSimulatorAnnunciator *annunciator = VerifyAnnunciatorAndEnter( hnd, id, num, newsim );

   if ( !annunciator )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = annunciator->SetMode( mode );

   newsim->IfLeave();

   return rv;
}

 
/**
 * Interface for GetIdrInfo 
 * Inside the function the method NewSimulatorInventory::GetIdrInfo is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param idrinfo info record to be filled
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorGetIdrInfo( void *,
                                         SaHpiResourceIdT,
                                         SaHpiIdrIdT,
                                         SaHpiIdrInfoT * ) __attribute__((used));

static SaErrorT NewSimulatorGetIdrInfo( void *hnd,
                                         SaHpiResourceIdT id,
                                         SaHpiIdrIdT idrid,
                                         SaHpiIdrInfoT *idrinfo ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->GetIdrInfo( *idrinfo );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetIdrAreaHeader
 * Inside the function the method NewSimulatorInventory::GetAreaHeader is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param areatype type of area to be found
 * @param areaid id of area to be found
 * @param nextareaid id of next area
 * @param header header information to be filled
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorGetIdrAreaHeader( void *,
                                               SaHpiResourceIdT,
                                               SaHpiIdrIdT,
                                               SaHpiIdrAreaTypeT,
                                               SaHpiEntryIdT,
                                               SaHpiEntryIdT *,
                                               SaHpiIdrAreaHeaderT * ) __attribute__((used));

static SaErrorT NewSimulatorGetIdrAreaHeader( void *hnd,
                                               SaHpiResourceIdT id,
                                               SaHpiIdrIdT idrid,
                                               SaHpiIdrAreaTypeT areatype,
                                               SaHpiEntryIdT areaid,
                                               SaHpiEntryIdT *nextareaid,
                                               SaHpiIdrAreaHeaderT *header ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->GetAreaHeader( areatype, areaid, *nextareaid, *header );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for AddIdrArea
 * Inside the function the method NewSimulatorInventory::AddArea is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param areatype type of area to be added
 * @param areaid id of area which is added
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorAddIdrArea( void *,
                                         SaHpiResourceIdT,
                                         SaHpiIdrIdT,
                                         SaHpiIdrAreaTypeT,
                                         SaHpiEntryIdT * ) __attribute__((used));

static SaErrorT NewSimulatorAddIdrArea( void *hnd,
                                         SaHpiResourceIdT id,
                                         SaHpiIdrIdT idrid,
                                         SaHpiIdrAreaTypeT areatype,
                                         SaHpiEntryIdT *areaid ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->AddArea( areatype, *areaid );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for AddIdrAreaById
 * Inside the function the method NewSimulatorInventory::AddAreaById is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param areatype type of area to be added
 * @param areaid id of area to use
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorAddIdrAreaById( void *,
                                         SaHpiResourceIdT,
                                         SaHpiIdrIdT,
                                         SaHpiIdrAreaTypeT,
                                         SaHpiEntryIdT ) __attribute__((used));

static SaErrorT NewSimulatorAddIdrAreaById( void *hnd,
                                         SaHpiResourceIdT id,
                                         SaHpiIdrIdT idrid,
                                         SaHpiIdrAreaTypeT areatype,
                                         SaHpiEntryIdT areaid ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->AddAreaById( areatype, areaid );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for DelIdrArea
 * Inside the function the method NewSimulatorInventory::DeleteArea is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param areaid id of area to be deleted
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorDelIdrArea( void *,
                                         SaHpiResourceIdT,
                                         SaHpiIdrIdT,
                                         SaHpiEntryIdT ) __attribute__((used));

static SaErrorT NewSimulatorDelIdrArea( void *hnd,
                                         SaHpiResourceIdT id,
                                         SaHpiIdrIdT idrid,
                                         SaHpiEntryIdT areaid ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->DeleteArea( areaid );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetIdrField
 * Inside the function the method NewSimulatorInventory::GetField is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param areaid id of area
 * @param fieldtype type of the field
 * @param fieldid id of the field
 * @param nextfieldid id of next field
 * @param field pointer on field record to be filled
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorGetIdrField( void *,
                                          SaHpiResourceIdT,
                                          SaHpiIdrIdT,
                                          SaHpiEntryIdT,
                                          SaHpiIdrFieldTypeT,
                                          SaHpiEntryIdT,
                                          SaHpiEntryIdT *,
                                          SaHpiIdrFieldT * ) __attribute__((used));

static SaErrorT NewSimulatorGetIdrField( void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiIdrIdT idrid,
                                          SaHpiEntryIdT areaid,
                                          SaHpiIdrFieldTypeT fieldtype,
                                          SaHpiEntryIdT fieldid,
                                          SaHpiEntryIdT *nextfieldid,
                                          SaHpiIdrFieldT *field ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->GetField( areaid, fieldtype, fieldid, *nextfieldid, *field );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for AddIdrField
 * Inside the function the method NewSimulatorInventory::AddField is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param field pointer on field record to add
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorAddIdrField( void *,
                                          SaHpiResourceIdT,
                                          SaHpiIdrIdT,
                                          SaHpiIdrFieldT * ) __attribute__((used));

static SaErrorT NewSimulatorAddIdrField( void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiIdrIdT idrid,
                                          SaHpiIdrFieldT *field ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->AddField( *field );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for AddIdrFieldById
 * Inside the function the method NewSimulatorInventory::AddFieldById is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param field pointer on field record to add
 * 
 * @return HPI error code
 **/             
static SaErrorT NewSimulatorAddIdrFieldById( void *,
                                          SaHpiResourceIdT,
                                          SaHpiIdrIdT,
                                          SaHpiIdrFieldT * ) __attribute__((used));

static SaErrorT NewSimulatorAddIdrFieldById( void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiIdrIdT idrid,
                                          SaHpiIdrFieldT *field ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->AddFieldById( *field );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for SetIdrField
 * Inside the function the method NewSimulatorInventory::SetField is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param field pointer on field record to be set
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorSetIdrField( void *,
                                          SaHpiResourceIdT,
                                          SaHpiIdrIdT,
                                          SaHpiIdrFieldT * ) __attribute__((used));

static SaErrorT NewSimulatorSetIdrField( void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiIdrIdT idrid,
                                          SaHpiIdrFieldT *field ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->SetField( *field );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for DelIdrField
 * Inside the function the method NewSimulatorInventory::DeleteField is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param idrid number of inventory
 * @param areaid id of area
 * @param fieldid id of field to be deleted
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorDelIdrField( void *,
                                          SaHpiResourceIdT,
                                          SaHpiIdrIdT,
                                          SaHpiEntryIdT,
                                          SaHpiEntryIdT ) __attribute__((used));

static SaErrorT NewSimulatorDelIdrField( void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiIdrIdT idrid,
                                          SaHpiEntryIdT areaid,
                                          SaHpiEntryIdT fieldid ) {
   NewSimulator *newsim = 0;
   NewSimulatorInventory *inv = VerifyInventoryAndEnter( hnd, id, idrid, newsim );

   if ( !inv )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = inv->DeleteField( areaid, fieldid );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for GetSelInfo
 * Inside the function the method NewSimulatorEventLog::IfELGetInfo is called.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * @param info pointer on event log structure to be filled
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorGetSelInfo( void *,
                                         SaHpiResourceIdT,
                                         SaHpiEventLogInfoT * ) __attribute__((used));

static SaErrorT NewSimulatorGetSelInfo( void *hnd,
                                         SaHpiResourceIdT id,
                                         SaHpiEventLogInfoT   *info ) {

   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELGetInfo((struct oh_handler_state *)hnd, info);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetSelTime
 * Inside the function the method NewSimulatorEventLog::IfELSetTime is called.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * @param t time to be set
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorSetSelTime( void *, SaHpiResourceIdT, SaHpiTimeT ) __attribute__((used));

static SaErrorT NewSimulatorSetSelTime( void *hnd, SaHpiResourceIdT id, SaHpiTimeT t ) {
   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELSetTime((struct oh_handler_state *)hnd, t);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for AddSelEntry
 * Inside the function the method NewSimulatorEventLog::IfELAddEntry is called.
 * A resource event log isn't supported at the moment.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * @param Event pointer on event to be added
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorAddSelEntry( void *, SaHpiResourceIdT, const SaHpiEventT * ) __attribute__((used));

static SaErrorT NewSimulatorAddSelEntry( void *hnd, SaHpiResourceIdT id,
                                          const SaHpiEventT *Event ) {

   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELAddEntry((struct oh_handler_state *)hnd, Event);

   newsim->IfLeave();

   return rv;
   
}


/**
 * Interface for GetSelEntry
 * Inside the function the method NewSimulatorEventLog::IfELGetEntry is called.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * @param current Identifier of event log entry to retrieve
 * @param prev Event Log entry identifier for the previous entry
 * @param next Event Log entry identifier for the next entry
 * @param entry Pointer to retrieved Event Log entry
 * @param rdr Pointer to structure to receive resource data record
 * @param rptentry Pointer to structure to receive RPT entry
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorGetSelEntry( void *hnd, SaHpiResourceIdT,
                                          SaHpiEventLogEntryIdT,
                                          SaHpiEventLogEntryIdT *, 
                                          SaHpiEventLogEntryIdT *,
                                          SaHpiEventLogEntryT *,
                                          SaHpiRdrT *,
                                          SaHpiRptEntryT * ) __attribute__((used));

static SaErrorT NewSimulatorGetSelEntry( void *hnd, SaHpiResourceIdT id,
                                          SaHpiEventLogEntryIdT current,
                                          SaHpiEventLogEntryIdT *prev, 
                                          SaHpiEventLogEntryIdT *next,
                                          SaHpiEventLogEntryT *entry,
                                          SaHpiRdrT *rdr,
                                          SaHpiRptEntryT *rptentry ) {
   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELGetEntry((struct oh_handler_state *)hnd,
                                 current, prev, next, entry, rdr, rptentry);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for ClearSel
 * Inside the function the method NewSimulatorEventLog::IfELClear is called.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorClearSel( void *, SaHpiResourceIdT ) __attribute__((used));

static SaErrorT NewSimulatorClearSel( void *hnd, SaHpiResourceIdT id ) {
   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELClear((struct oh_handler_state *)hnd);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetSelState
 * Inside the function the method NewSimulatorEventLog::IfELSetState is called.
 * A resource event log isn't supported at the moment.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * @param state Event Log state to be set
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorSetSelState(void *, SaHpiResourceIdT, 
                                         SaHpiBoolT) __attribute__((used));

static SaErrorT NewSimulatorSetSelState( void *hnd, SaHpiResourceIdT id,
                                          SaHpiBoolT state ) {
   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELSetState((struct oh_handler_state *)hnd, state);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetSelState
 * Inside the function the method NewSimulatorEventLog::IfELGetState is called.
 * A resource event log isn't supported at the moment.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * @param state pointer to the current event log enable state
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorGetSelState(void *, SaHpiResourceIdT, 
                                         SaHpiBoolT *) __attribute__((used));

static SaErrorT NewSimulatorGetSelState( void *hnd, SaHpiResourceIdT id,
                                          SaHpiBoolT *state ) {
   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELGetState((struct oh_handler_state *)hnd, state);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetSelCapability
 * Inside the function the method NewSimulatorEventLog::IfELGetCaps is called.
 * A resource event log isn't supported at the moment.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * @param caps pointer to the current event log enable state
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorGetSelCapability(void *, SaHpiResourceIdT, 
                             SaHpiEventLogCapabilitiesT *) __attribute__((used));

static SaErrorT NewSimulatorGetSelCapability( void *hnd, SaHpiResourceIdT id,
                                          SaHpiEventLogCapabilitiesT *caps ) {
   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELGetCaps((struct oh_handler_state *)hnd, caps);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for ResetSelOverflow
 * Inside the function the method NewSimulatorEventLog::IfELOverflow is called.
 * A resource event log isn't supported at the moment.
 *
 * @param hnd pointer on handler
 * @param id resource id
 * 
 * @return HPI error code
 **/ 
static SaErrorT NewSimulatorResetSelOverflow(void *, SaHpiResourceIdT) 
                                                           __attribute__((used));

static SaErrorT NewSimulatorResetSelOverflow( void *hnd, SaHpiResourceIdT id ) {

   SaErrorT rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
      return SA_ERR_HPI_INTERNAL_ERROR;
   }

   rv = newsim->IfELOverflow((struct oh_handler_state *)hnd);

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for HotswapPolicyCancel. 
 * Inside the function the method NewSimulatorResource::HotswapPolicyCancel()
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param state Pointer on state variable to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorHotswapPolicyCancel( void *, SaHpiResourceIdT,
                                                  SaHpiTimeoutT ) __attribute__((used));

static SaErrorT NewSimulatorHotswapPolicyCancel( void *hnd, SaHpiResourceIdT id,
                                                  SaHpiTimeoutT timeout) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = res->HotswapPolicyCancel();

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetHotswapState. 
 * Inside the function the method NewSimulatorResource::GetHotswapState()
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param state Pointer on state variable to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetHotswapState( void *, SaHpiResourceIdT ,
                                              SaHpiHsStateT * ) __attribute__((used));

static SaErrorT NewSimulatorGetHotswapState( void *hnd, SaHpiResourceIdT id,
                                              SaHpiHsStateT *state ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
     return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = res->GetHotswapState( *state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetHotswapState. 
 * Inside the function the method in dependency of the given state
 * NewSimulatorResource::SetStateActive() or NewSimulatorResource::SetStateInactive()
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param state HotSwap state to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetHotswapState( void *, SaHpiResourceIdT,
                                              SaHpiHsStateT ) __attribute__((used));

static SaErrorT NewSimulatorSetHotswapState( void *hnd, SaHpiResourceIdT id,
                                              SaHpiHsStateT state ) {
   SaErrorT rv = SA_OK;
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   rv = SA_ERR_HPI_INTERNAL_ERROR;
   
   if ( state == SAHPI_HS_STATE_ACTIVE ) {
      rv = res->SetStateActive();

   } else if ( state == SAHPI_HS_STATE_INACTIVE ) {
      rv = res->SetStateInactive();

   }
   
   if ( rv == SA_ERR_HPI_INTERNAL_ERROR )
      err( "It looks like the plugin got an invalid state for SetHotswapState.");

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for RequestHotswapAction. 
 * Inside the function the method NewSimulatorResource::RequestHotswapAction()
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param act action requested
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorRequestHotswapAction( void *, SaHpiResourceIdT,
                                                   SaHpiHsActionT ) __attribute__((used));

static SaErrorT NewSimulatorRequestHotswapAction( void *hnd, SaHpiResourceIdT id,
                                                   SaHpiHsActionT act ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = res->RequestHotswapAction( act );

   newsim->IfLeave();

   return rv;
}



/**
 * Interface for GetWatchdogInfo. 
 * Inside the function the method NewSimulatorWatchdog::GetWatchdogInfo
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of watchdog
 * @param watchdog watchdog information record to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetWatchdogInfo(void *,
                                             SaHpiResourceIdT,
                                             SaHpiWatchdogNumT,
                                             SaHpiWatchdogT *) __attribute__((used));

static SaErrorT NewSimulatorGetWatchdogInfo(void *hnd,
                                             SaHpiResourceIdT  id,
                                             SaHpiWatchdogNumT num,
                                             SaHpiWatchdogT    *watchdog) {
   NewSimulator *newsim = 0;
   NewSimulatorWatchdog *wd = VerifyWatchdogAndEnter( hnd, id, num, newsim );
   if ( !wd ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = wd->GetWatchdogInfo( *watchdog );
   newsim->IfLeave();
   return rv; 
}


/**
 * Interface for SetWatchdogInfo. 
 * Inside the function the method NewSimulatorWatchdog::SetWatchdogInfo
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of watchdog
 * @param watchdog record with the information which should be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetWatchdogInfo(void *,
                                             SaHpiResourceIdT,
                                             SaHpiWatchdogNumT,
                                             SaHpiWatchdogT *) __attribute__((used));

static SaErrorT NewSimulatorSetWatchdogInfo(void *hnd,
                                             SaHpiResourceIdT  id,
                                             SaHpiWatchdogNumT num,
                                             SaHpiWatchdogT    *watchdog) {
   NewSimulator *newsim = 0;
   NewSimulatorWatchdog *wd = VerifyWatchdogAndEnter( hnd, id, num, newsim );
   if ( !wd ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = wd->SetWatchdogInfo( *watchdog );
   newsim->IfLeave();
   return rv; 
}


/**
 * Interface for ResetWatchdog. 
 * Inside the function the method NewSimulatorWatchdog::ResetWatchdog
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of watchdog
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorResetWatchdog(void *,
                                           SaHpiResourceIdT,
                                           SaHpiWatchdogNumT) __attribute__((used));

static SaErrorT NewSimulatorResetWatchdog(void *hnd,
                                           SaHpiResourceIdT  id,
                                           SaHpiWatchdogNumT num) {
   NewSimulator *newsim = 0;
   NewSimulatorWatchdog *wd = VerifyWatchdogAndEnter( hnd, id, num, newsim );
   if ( !wd ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = wd->ResetWatchdog();
   newsim->IfLeave();
   return rv; 
}


/**
 * Interface for FumiSpecInfoGet. 
 * Inside the function the method NewSimulatorFumi::GetSpecInfo
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of fumi
 * @param spec pointer to the location to store spec information
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetFumiSpec(void *, 
                                         SaHpiResourceIdT, 
                                         SaHpiFumiNumT, 
                                         SaHpiFumiSpecInfoT *) __attribute__((used));

static SaErrorT NewSimulatorGetFumiSpec(void *hnd, 
                                         SaHpiResourceIdT    id, 
                                         SaHpiFumiNumT       num, 
                                         SaHpiFumiSpecInfoT *spec) {
   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetSpecInfo( *spec );
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiServiceImpactGet. 
 * Inside the function the method NewSimulatorFumi::GetImpact
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num number of fumi
 * @param impact pointer to the location to store service impact information
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetFumiServImpact(void *, 
                                               SaHpiResourceIdT, 
                                               SaHpiFumiNumT, 
                                               SaHpiFumiServiceImpactDataT *) __attribute__((used));

static SaErrorT NewSimulatorGetFumiServImpact(void *hnd, 
                                               SaHpiResourceIdT            id, 
                                               SaHpiFumiNumT               num, 
                                               SaHpiFumiServiceImpactDataT *impact) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetImpact( *impact );
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiSourceSet. 
 * Inside the function the method NewSimulatorFumi::SetSource
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param src Text buffer containing URI of the source
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetFumiSource(void *, 
                                           SaHpiResourceIdT, 
                                           SaHpiFumiNumT, 
                                           SaHpiBankNumT, 
                                           SaHpiTextBufferT *) __attribute__((used));
                                           
static SaErrorT NewSimulatorSetFumiSource(void             *hnd, 
                                           SaHpiResourceIdT  id, 
                                           SaHpiFumiNumT     num, 
                                           SaHpiBankNumT     bank, 
                                           SaHpiTextBufferT  *src) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->SetSource( bank, *src );
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiSourceInfoValidateStart. 
 * Inside the function the method NewSimulatorFumi::ValidateSource
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorValidateFumiSource(void *, 
                                                SaHpiResourceIdT, 
                                                SaHpiFumiNumT, 
                                                SaHpiBankNumT) __attribute__((used));

static SaErrorT NewSimulatorValidateFumiSource(void            *hnd, 
                                                SaHpiResourceIdT id, 
                                                SaHpiFumiNumT    num, 
                                                SaHpiBankNumT    bank) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->ValidateSource( bank );
   newsim->IfLeave();
   return rv;                                                	
}


/**
 * Interface for FumiSourceInfoGet. 
 * Inside the function the method NewSimulatorFumi::GetSource
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param src pointer to the location to store source information
 * 
 * @return HPI error code
 **/           
static SaErrorT NewSimulatorGetFumiSource(void *, 
                                           SaHpiResourceIdT, 
                                           SaHpiFumiNumT, 
                                           SaHpiBankNumT, 
                                           SaHpiFumiSourceInfoT *) __attribute__((used));
                                           
static SaErrorT NewSimulatorGetFumiSource(void                *hnd, 
                                           SaHpiResourceIdT     id, 
                                           SaHpiFumiNumT        num, 
                                           SaHpiBankNumT        bank, 
                                           SaHpiFumiSourceInfoT *src) {
                                           	
   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetSource( bank, *src );
   newsim->IfLeave();
   return rv;  
}
                                           

/**
 * Interface for FumiSourceComponentInfoGet. 
 * Inside the function the method NewSimulatorFumi::GetComponentSource
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param comp identifier of the component
 * @param next pointer to store the next component identifier
 * @param inf pointer to the location to store component information
 * 
 * @return HPI error code
 **/                                         
static SaErrorT NewSimulatorGetFumiSourceComponent(void *, 
                             SaHpiResourceIdT, 
                             SaHpiFumiNumT, 
                             SaHpiBankNumT, 
                             SaHpiEntryIdT, 
                             SaHpiEntryIdT *, 
                             SaHpiFumiComponentInfoT *) __attribute__((used));

static SaErrorT NewSimulatorGetFumiSourceComponent(void            *hnd, 
                                                    SaHpiResourceIdT id, 
                                                    SaHpiFumiNumT    num, 
                                                    SaHpiBankNumT    bank, 
                                                    SaHpiEntryIdT    comp, 
                                                    SaHpiEntryIdT    *next, 
                                            SaHpiFumiComponentInfoT  *inf) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetComponentSource( bank, comp, *next, *inf );
   newsim->IfLeave();
   return rv;                                                    	
}                                                 


/**
 * Interface for FumiTargetInfoGet. 
 * Inside the function the method NewSimulatorFumi::GetTarget
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param trg pointer to the location to store target information
 * 
 * @return HPI error code
 **/                                               
static SaErrorT NewSimulatorGetFumiTarget(void *, 
                                           SaHpiResourceIdT, 
                                           SaHpiFumiNumT, 
                                           SaHpiBankNumT, 
                                           SaHpiFumiBankInfoT *) __attribute__((used));

static SaErrorT NewSimulatorGetFumiTarget(void              *hnd, 
                                           SaHpiResourceIdT   id, 
                                           SaHpiFumiNumT      num, 
                                           SaHpiBankNumT      bank, 
                                           SaHpiFumiBankInfoT *trg) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetTarget( bank, *trg );
   newsim->IfLeave();
   return rv;                                            	
}                                          


/**
 * Interface for FumiTargetComponentInfoGet. 
 * Inside the function the method NewSimulatorFumi::GetComponentTarget
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param comp identifier of the component
 * @param next pointer to store the next component identifier
 * @param inf pointer to the location to store component information
 * 
 * @return HPI error code
 **/                                      
static SaErrorT NewSimulatorGetFumiTargetComponent(void *, 
                                                    SaHpiResourceIdT, 
                                                    SaHpiFumiNumT, 
                                                    SaHpiBankNumT, 
                                                    SaHpiEntryIdT, 
                                                    SaHpiEntryIdT *, 
                                                    SaHpiFumiComponentInfoT *) __attribute__((used));
                                                    
 static SaErrorT NewSimulatorGetFumiTargetComponent(void                  *hnd, 
                                                    SaHpiResourceIdT        id, 
                                                    SaHpiFumiNumT           num, 
                                                    SaHpiBankNumT           bank, 
                                                    SaHpiEntryIdT           comp, 
                                                    SaHpiEntryIdT           *next, 
                                                    SaHpiFumiComponentInfoT *inf) {
                                                    	
   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetComponentTarget( bank, comp, *next, *inf );
   newsim->IfLeave();
   return rv;  
}
                                                    

/**
 * Interface for FumiLogicalTargetInfoGet. 
 * Inside the function the method NewSimulatorFumi::GetTargetLogical
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param trg pointer to the location to store target information
 * 
 * @return HPI error code
 **/                                                                                                      
static SaErrorT NewSimulatorGetFumiLogicalTarget(void *, 
                                                  SaHpiResourceIdT, 
                                                  SaHpiFumiNumT, 
                                                  SaHpiFumiLogicalBankInfoT *) __attribute__((used));

static SaErrorT NewSimulatorGetFumiLogicalTarget(void            *hnd, 
                                                  SaHpiResourceIdT id,
                                                  SaHpiFumiNumT    num, 
                                         SaHpiFumiLogicalBankInfoT *trg) {
                                                  	
   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetTargetLogical( *trg );
   newsim->IfLeave();
   return rv;  
}


/**
 * Interface for FumiLogicalTargetComponentInfoGet. 
 * Inside the function the method NewSimulatorFumi::GetComponentTargetLogical
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param comp identifier of the component
 * @param next pointer to store the next component identifier
 * @param inf pointer to the location to store logical component information
 * 
 * @return HPI error code
 **/                                                                                                   
static SaErrorT NewSimulatorGetFumiLogicalTargetComponent(void *, 
                      SaHpiResourceIdT, 
                      SaHpiFumiNumT, 
                      SaHpiEntryIdT, 
                      SaHpiEntryIdT *, 
                      SaHpiFumiLogicalComponentInfoT *) __attribute__((used));
                                                           
static SaErrorT NewSimulatorGetFumiLogicalTargetComponent(void         *hnd, 
                                                         SaHpiResourceIdT id, 
                                                         SaHpiFumiNumT  num, 
                                                         SaHpiEntryIdT  comp, 
                                                         SaHpiEntryIdT  *next, 
                                         SaHpiFumiLogicalComponentInfoT *inf) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetComponentTargetLogical( comp, *next, *inf );
   newsim->IfLeave();
   return rv;                                          	
}


/**
 * Interface for FumiBackupStart. 
 * Inside the function the method NewSimulatorFumi::StartBackup
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * 
 * @return HPI error code
 **/                  
static SaErrorT NewSimulatorStartFumiBackup(void *, 
                                             SaHpiResourceIdT, 
                                             SaHpiFumiNumT) __attribute__((used));
                                             
static SaErrorT NewSimulatorStartFumiBackup(void            *hnd, 
                                             SaHpiResourceIdT id, 
                                             SaHpiFumiNumT    num) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->StartBackup();
   newsim->IfLeave();
   return rv;                                            	
}
                                             

/**
 * Interface for FumiBankBootOrderSet. 
 * Inside the function the method NewSimulatorFumi::SetOrder
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param pos new position
 * 
 * @return HPI error code
 **/                                            
static SaErrorT NewSimulatorSetFumiBankOrder(void *, 
                                              SaHpiResourceIdT, 
                                              SaHpiFumiNumT, 
                                              SaHpiBankNumT, 
                                              SaHpiUint32T) __attribute__((used));

static SaErrorT NewSimulatorSetFumiBankOrder(void            *hnd, 
                                              SaHpiResourceIdT id, 
                                              SaHpiFumiNumT    num, 
                                              SaHpiBankNumT    bank, 
                                              SaHpiUint32T     pos) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->SetOrder( bank, pos );
   newsim->IfLeave();
   return rv; 
}
  
                                              
/**
 * Interface for FumiBankCopyStart. 
 * Inside the function the method NewSimulatorFumi::CopyBank
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param dest bank number of destination
 * 
 * @return HPI error code
 **/                                              
static SaErrorT NewSimulatorStartFumiBankCopy(void *, 
                                               SaHpiResourceIdT, 
                                               SaHpiFumiNumT, 
                                               SaHpiBankNumT, 
                                               SaHpiBankNumT) __attribute__((used));
                                               
static SaErrorT NewSimulatorStartFumiBankCopy(void             *hnd, 
                                               SaHpiResourceIdT  id, 
                                               SaHpiFumiNumT     num, 
                                               SaHpiBankNumT     bank, 
                                               SaHpiBankNumT     dest) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->CopyBank( bank, dest );
   newsim->IfLeave();
   return rv;
}
 
                                              
/**
 * Interface for FumiInstallStart. 
 * Inside the function the method NewSimulatorFumi::Install
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * 
 * @return HPI error code
 **/                                                                                         
static SaErrorT NewSimulatorStartFumiInstall(void *, 
                                              SaHpiResourceIdT, 
                                              SaHpiFumiNumT, 
                                              SaHpiBankNumT) __attribute__((used));
                                              
static SaErrorT NewSimulatorStartFumiInstall(void             *hnd, 
                                              SaHpiResourceIdT  id, 
                                              SaHpiFumiNumT     num, 
                                              SaHpiBankNumT     bank) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->Install( bank );
   newsim->IfLeave();
   return rv;   
}


/**
 * Interface for FumiUpgradeStatusGet. 
 * Inside the function the method NewSimulatorFumi::GetStatus
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * @param status pointer to location to store upgrade status
 * 
 * @return HPI error code
 **/       
static SaErrorT NewSimulatorGetFumiStatus(void *, 
                                           SaHpiResourceIdT, 
                                           SaHpiFumiNumT, 
                                           SaHpiBankNumT, 
                                           SaHpiFumiUpgradeStatusT *) __attribute__((used));
                                           
static SaErrorT NewSimulatorGetFumiStatus(void                   *hnd, 
                                           SaHpiResourceIdT        id, 
                                           SaHpiFumiNumT           num, 
                                           SaHpiBankNumT           bank, 
                                           SaHpiFumiUpgradeStatusT *status) {
                                           	
   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetStatus( bank, *status );
   newsim->IfLeave();
   return rv;   
}


/**
 * Interface for FumiTargetVerifyStart. 
 * Inside the function the method NewSimulatorFumi::VerifyTarget
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * 
 * @return HPI error code
 **/                                           
static SaErrorT NewSimulatorStartFumiVerification(void *, 
                                                   SaHpiResourceIdT, 
                                                   SaHpiFumiNumT, 
                                                   SaHpiBankNumT) __attribute__((used));
                                                   
static SaErrorT NewSimulatorStartFumiVerification(void            *hnd, 
                                                   SaHpiResourceIdT id, 
                                                   SaHpiFumiNumT    num, 
                                                   SaHpiBankNumT    bank) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->VerifyTarget( bank );
   newsim->IfLeave();
   return rv; 
}

/**
 * Interface for TargetVerifyMainStart. 
 * Inside the function the method NewSimulatorFumi::VerifyTargetMain
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * 
 * @return HPI error code
 **/                             
static SaErrorT NewSimulatorStartFumiVerificationMain(void *, 
                                                       SaHpiResourceIdT, 
                                                       SaHpiFumiNumT) __attribute__((used));

static SaErrorT NewSimulatorStartFumiVerificationMain(void            *hnd, 
                                                       SaHpiResourceIdT id, 
                                                       SaHpiFumiNumT    num) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->VerifyTargetMain();
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiUpgradeCancel. 
 * Inside the function the method NewSimulatorFumi::CancelUpgrade
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * 
 * @return HPI error code
 **/                                          
static SaErrorT NewSimulatorCancelFumiUpgrade(void *, 
                                               SaHpiResourceIdT, 
                                               SaHpiFumiNumT, 
                                               SaHpiBankNumT) __attribute__((used));
                                               
static SaErrorT NewSimulatorCancelFumiUpgrade(void            *hnd, 
                                               SaHpiResourceIdT id, 
                                               SaHpiFumiNumT    num, 
                                               SaHpiBankNumT    bank) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->CancelUpgrade( bank );
   newsim->IfLeave();
   return rv; 
}

/**
 * Interface for FumiAutoRollbackDisableGet. 
 * Inside the function the method NewSimulatorFumi::GetRollbackFlag
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param rollb pointer to location to store information about rollback status
 * 
 * @return HPI error code
 **/  
static SaErrorT NewSimulatorGetFumiRollback(void *, 
                                             SaHpiResourceIdT, 
                                             SaHpiFumiNumT, 
                                             SaHpiBoolT *) __attribute__((used));
                                             
static SaErrorT NewSimulatorGetFumiRollback(void            *hnd, 
                                             SaHpiResourceIdT id, 
                                             SaHpiFumiNumT    num, 
                                             SaHpiBoolT       *rollb) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->GetRollbackFlag( *rollb );
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiAutoRollbackDisableSet. 
 * Inside the function the method NewSimulatorFumi::SetRollbackFlag
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param rollb information about rollback status
 * 
 * @return HPI error code
 **/                                           
static SaErrorT NewSimulatorSetFumiRollback(void *, 
                                             SaHpiResourceIdT, 
                                             SaHpiFumiNumT, 
                                             SaHpiBoolT) __attribute__((used));

static SaErrorT NewSimulatorSetFumiRollback(void            *hnd, 
                                             SaHpiResourceIdT id, 
                                             SaHpiFumiNumT    num, 
                                             SaHpiBoolT       rollb) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->SetRollbackFlag( rollb );
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiRollbackStart. 
 * Inside the function the method NewSimulatorFumi::Rollback
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * 
 * @return HPI error code
 **/                          
static SaErrorT NewSimulatorStartFumiRollback(void *, 
                                               SaHpiResourceIdT, 
                                               SaHpiFumiNumT) __attribute__((used));

static SaErrorT NewSimulatorStartFumiRollback(void            *hnd, 
                                             SaHpiResourceIdT id, 
                                             SaHpiFumiNumT    num) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->Rollback();
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiActivate. 
 * Inside the function the method NewSimulatorFumi::ActivateOld
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * 
 * @return HPI error code
 **/                         
static SaErrorT NewSimulatorActivateFumi(void *, 
                                          SaHpiResourceIdT, 
                                          SaHpiFumiNumT) __attribute__((used));

static SaErrorT NewSimulatorActivateFumi(void            *hnd, 
                                          SaHpiResourceIdT id, 
                                          SaHpiFumiNumT    num) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->Activate();
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiActivateStart. 
 * Inside the function the method NewSimulatorFumi::Activate
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param log indicates a logical bank or boot order activation
 * 
 * @return HPI error code
 **/        
static SaErrorT NewSimulatorStartFumiActivation(void *, 
                                                 SaHpiResourceIdT, 
                                                 SaHpiFumiNumT, 
                                                 SaHpiBoolT) __attribute__((used));

static SaErrorT NewSimulatorStartFumiActivation(void            *hnd, 
                                                 SaHpiResourceIdT id, 
                                                 SaHpiFumiNumT    num,
                                                 SaHpiBoolT       log) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->Activate( log );
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for FumiCleanup. 
 * Inside the function the method NewSimulatorFumi::Cleanup
 * is called. 
 *  
 * @param hnd pointer on handler
 * @param id resource id
 * @param num fumi number
 * @param bank bank number
 * 
 * @return HPI error code
 **/                    
static SaErrorT NewSimulatorCleanupFumi(void *, 
                                         SaHpiResourceIdT, 
                                         SaHpiFumiNumT, 
                                         SaHpiBankNumT) __attribute__((used));
                                         
static SaErrorT NewSimulatorCleanupFumi(void            *hnd, 
                                         SaHpiResourceIdT id, 
                                         SaHpiFumiNumT    num,
                                         SaHpiBankNumT    bank) {

   NewSimulator *newsim = 0;
   NewSimulatorFumi *fumi = VerifyFumiAndEnter( hnd, id, num, newsim );
   if ( !fumi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = fumi->Cleanup( bank );
   newsim->IfLeave();
   return rv;
}


/**
 * Interface for DimiInfoGet. 
 * Inside the function the method NewSimulatorDimi::GetInfo is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param num dimi number
 * @param info pointer on dimi info to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetDimiInfo( void *, 
                                          SaHpiResourceIdT, 
                                          SaHpiDimiNumT, 
                                          SaHpiDimiInfoT *)  __attribute__((used));

static SaErrorT NewSimulatorGetDimiInfo( void            *hnd, 
                                          SaHpiResourceIdT id, 
                                          SaHpiDimiNumT    num, 
                                          SaHpiDimiInfoT   *info) {

   NewSimulator *newsim = 0;
   NewSimulatorDimi *dimi = VerifyDimiAndEnter( hnd, id, num, newsim );
   if ( !dimi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = dimi->GetInfo( *info );
   newsim->IfLeave();

   return rv;
}


/**
 * Interface for DimiTestInfoGet. 
 * Inside the function the method NewSimulatorDimi::GetTestInfo is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param num dimi number
 * @param tnum dimi test number
 * @param tinfo pointer on dimi test info to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetDimiTestInfo( void *, 
                                      SaHpiResourceIdT, 
                                      SaHpiDimiNumT,
                                      SaHpiDimiTestNumT, 
                                      SaHpiDimiTestT *)  __attribute__((used));
                                      
static SaErrorT NewSimulatorGetDimiTestInfo( void     *hnd, 
                                      SaHpiResourceIdT  id, 
                                      SaHpiDimiNumT     num,
                                      SaHpiDimiTestNumT tnum, 
                                      SaHpiDimiTestT    *tinfo) {

   NewSimulator *newsim = 0;
   NewSimulatorDimi *dimi = VerifyDimiAndEnter( hnd, id, num, newsim );
   if ( !dimi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = dimi->GetTestInfo( tnum, *tinfo );
   newsim->IfLeave();

   return rv;
}


/**
 * Interface for DimiTestReadinessGet. 
 * Inside the function the method NewSimulatorDimi::GetReadiness is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param num dimi number
 * @param tnum dimi test number
 * @param tready pointer on dimi test readiness info to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetDimiTestReadiness( void *, 
                                     SaHpiResourceIdT, 
                                     SaHpiDimiNumT,
                                     SaHpiDimiTestNumT, 
                                     SaHpiDimiReadyT *)  __attribute__((used));

static SaErrorT NewSimulatorGetDimiTestReadiness( void *hnd, 
                                     SaHpiResourceIdT    id, 
                                     SaHpiDimiNumT       num,
                                     SaHpiDimiTestNumT   tnum, 
                                     SaHpiDimiReadyT     *tready) {

   NewSimulator *newsim = 0;
   NewSimulatorDimi *dimi = VerifyDimiAndEnter( hnd, id, num, newsim );
   if ( !dimi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = dimi->GetReadiness( tnum, *tready );
   newsim->IfLeave();

   return rv;
}


/**
 * Interface for DimiTestStart. 
 * Inside the function the method NewSimulatorDimi::StartTest is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param num dimi number
 * @param tnum dimi test number
 * @param tnump number of variable parameters
 * @param params pointer to array containing variable parameters
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorStartDimiTest( void *, 
                        SaHpiResourceIdT, 
                        SaHpiDimiNumT, 
                        SaHpiDimiTestNumT,
                        SaHpiUint8T, 
                        SaHpiDimiTestVariableParamsT *)  __attribute__((used));

static SaErrorT NewSimulatorStartDimiTest( void    *hnd, 
                        SaHpiResourceIdT             id, 
                        SaHpiDimiNumT                num, 
                        SaHpiDimiTestNumT            tnum,
                        SaHpiUint8T                  tnump, 
                        SaHpiDimiTestVariableParamsT *params) {

   NewSimulator *newsim = 0;
   NewSimulatorDimi *dimi = VerifyDimiAndEnter( hnd, id, num, newsim );
   if ( !dimi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = dimi->StartTest( tnum, tnump, params );
   newsim->IfLeave();

   return rv;
}


/**
 * Interface for DimiTestCancel. 
 * Inside the function the method NewSimulatorDimi::CancelTest is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param num dimi number
 * @param tnum dimi test number
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorCancelDimiTest( void *, 
                                      SaHpiResourceIdT, 
                                      SaHpiDimiNumT, 
                                      SaHpiDimiTestNumT) __attribute__((used));

static SaErrorT NewSimulatorCancelDimiTest( void      *hnd, 
                                      SaHpiResourceIdT  id, 
                                      SaHpiDimiNumT     num, 
                                      SaHpiDimiTestNumT tnum) {

   NewSimulator *newsim = 0;
   NewSimulatorDimi *dimi = VerifyDimiAndEnter( hnd, id, num, newsim );
   if ( !dimi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = dimi->CancelTest( tnum );
   newsim->IfLeave();

   return rv;
}


/**
 * Interface for DimiTestStatusGet. 
 * Inside the function the method NewSimulatorDimi::GetStatus is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param num dimi number
 * @param tnum dimi test number
 * @param tperc pointer to location to store percentage of test completed
 * @param tstatus pointer to location to store the status
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetDimiTestStatus( void *, 
                              SaHpiResourceIdT, 
                              SaHpiDimiNumT, 
                              SaHpiDimiTestNumT, 
                              SaHpiDimiTestPercentCompletedT *,
                              SaHpiDimiTestRunStatusT *) __attribute__((used));

static SaErrorT NewSimulatorGetDimiTestStatus( void        *hnd, 
                              SaHpiResourceIdT               id, 
                              SaHpiDimiNumT                  num, 
                              SaHpiDimiTestNumT              tnum, 
                              SaHpiDimiTestPercentCompletedT *tperc,
                              SaHpiDimiTestRunStatusT        *tstatus) {

   NewSimulator *newsim = 0;
   NewSimulatorDimi *dimi = VerifyDimiAndEnter( hnd, id, num, newsim );
   if ( !dimi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = dimi->GetStatus( tnum, *tperc, *tstatus );
   newsim->IfLeave();

   return rv;
}

/**
 * Interface for DimiTestResultsGet. 
 * Inside the function the method NewSimulatorDimi::GetResults is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param num dimi number
 * @param tnum dimi test number
 * @param tresults pointer to location to store results from last run
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetDimiTestResults( void *, 
                                SaHpiResourceIdT, 
                                SaHpiDimiNumT, 
                                SaHpiDimiTestNumT, 
                                SaHpiDimiTestResultsT *) __attribute__((used));

static SaErrorT NewSimulatorGetDimiTestResults( void *hnd, 
                                SaHpiResourceIdT       id, 
                                SaHpiDimiNumT          num, 
                                SaHpiDimiTestNumT      tnum, 
                                SaHpiDimiTestResultsT  *tresults) {

   NewSimulator *newsim = 0;
   NewSimulatorDimi *dimi = VerifyDimiAndEnter( hnd, id, num, newsim );
   if ( !dimi ) return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = dimi->GetResults( tnum, *tresults );
   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetAutoInsertTimeout. 
 * Inside the function the method NewSimulator::IfSetAutoInsertTimeout is called. 
 *  
 * @param hnd pointer on handler
 * @param timeout timeout value to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetAutoInsertTimeout( void *, 
                                         SaHpiTimeoutT ) __attribute__((used));

static SaErrorT NewSimulatorSetAutoInsertTimeout( void *hnd, SaHpiTimeoutT  timeout) {
   NewSimulator *newsim = VerifyNewSimulator( hnd );

   if ( !newsim ) {
       return SA_ERR_HPI_INTERNAL_ERROR;
   }

   SaErrorT rv = newsim->IfSetAutoInsertTimeout( timeout );

   return rv;
}


/**
 * Interface for GetAutoExtractTimeout. 
 * Inside the function the method NewSimulatorResource::GetAutoExtractTimeout is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param timeout pointer to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetAutoExtractTimeout( void *, SaHpiResourceIdT,
                                                    SaHpiTimeoutT * ) __attribute__((used));

static SaErrorT NewSimulatorGetAutoExtractTimeout( void *hnd, SaHpiResourceIdT id,
                                                    SaHpiTimeoutT *timeout ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = res->GetAutoExtractTimeout( *timeout );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetAutoExtractTimeout. 
 * 
 * Inside the function the method NewSimulatorResource::SetAutoExtractTimeout is called. 
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param timeout timeout value to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetAutoExtractTimeout( void *, SaHpiResourceIdT,
                                                    SaHpiTimeoutT ) __attribute__((used));

static SaErrorT NewSimulatorSetAutoExtractTimeout( void *hnd, SaHpiResourceIdT id,
                                                    SaHpiTimeoutT timeout ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = res->SetAutoExtractTimeout( timeout );

   newsim->IfLeave();

   return rv;
}

/**
 * Interface for GetPowerState. 
 * Inside the function the method NewSimulator::IfGetPowerState is called. 
 * 
 * @todo  It should be implemented inside class NewSimulatorResource instead 
 * of class NewSimulator.
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param state pointer on state structure to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetPowerState( void *, SaHpiResourceIdT,
                                            SaHpiPowerStateT * ) __attribute__((used));

static SaErrorT NewSimulatorGetPowerState( void *hnd, SaHpiResourceIdT id,
                                            SaHpiPowerStateT *state ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfGetPowerState( res, *state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetPowerState. 
 * Inside the function the method NewSimulator::IfSetPowerState is called. 
 * 
 * @todo  It should be implemented inside class NewSimulatorResource instead 
 * of class NewSimulator.
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param state structure to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetPowerState( void *, SaHpiResourceIdT,
                                            SaHpiPowerStateT ) __attribute__((used));

static SaErrorT NewSimulatorSetPowerState( void *hnd, SaHpiResourceIdT id,
                                            SaHpiPowerStateT state ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfSetPowerState( res, state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetIndicatorState. 
 * Inside the function the method NewSimulator::IfGetIndicatorState is called. 
 * 
 * @todo  It should be implemented inside class NewSimulatorResource instead 
 * of class NewSimulator.
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param state pointer on structure to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetIndicatorState( void *, SaHpiResourceIdT,
                                                SaHpiHsIndicatorStateT * ) __attribute__((used));

static SaErrorT NewSimulatorGetIndicatorState( void *hnd, SaHpiResourceIdT id,
                                                SaHpiHsIndicatorStateT *state ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfGetIndicatorState( res, *state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetIndicatorState. 
 * Inside the function the method NewSimulator::IfSetIndicatorState is called. 
 * 
 * @todo  It should be implemented inside class NewSimulatorResource instead 
 * of class NewSimulator.
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param state structure to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetIndicatorState( void *, SaHpiResourceIdT,
                                                SaHpiHsIndicatorStateT ) __attribute__((used));

static SaErrorT NewSimulatorSetIndicatorState( void *hnd, SaHpiResourceIdT id,
                                                SaHpiHsIndicatorStateT state ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfSetIndicatorState( res, state );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for ControlParm. 
 * Inside the function the method NewSimulator::IfControlParm is called. 
 * 
 * @todo  It should be implemented inside class NewSimulatorResource instead 
 * of class NewSimulator.
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param act structure of ParmAction to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorControlParm( void *,
                                          SaHpiResourceIdT,
                                          SaHpiParmActionT ) __attribute__((used));

static SaErrorT NewSimulatorControlParm( void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiParmActionT act ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfControlParm( res, act );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for GetResetState. 
 * Inside the function the method NewSimulator::IfGetResetState is called. 
 * 
 * @todo  It should be implemented inside class NewSimulatorResource instead 
 * of class NewSimulator.
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param act structure of ResetAction to be filled
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorGetResetState( void *, SaHpiResourceIdT,
                                            SaHpiResetActionT * ) __attribute__((used));

static SaErrorT NewSimulatorGetResetState( void *hnd, SaHpiResourceIdT id,
                                            SaHpiResetActionT *act ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfGetResetState( res, *act );

   newsim->IfLeave();

   return rv;
}


/**
 * Interface for SetResetState. 
 * Inside the function the method NewSimulator::IfSetResetState is called. 
 * 
 * @todo  It should be implemented inside class NewSimulatorResource instead 
 * of class NewSimulator.
 * 
 * @param hnd pointer on handler
 * @param id resource id
 * @param act structure of ResetAction to be set
 * 
 * @return HPI error code
 **/
static SaErrorT NewSimulatorSetResetState( void *,
                                            SaHpiResourceIdT,
                                            SaHpiResetActionT ) __attribute__((used));

static SaErrorT NewSimulatorSetResetState( void *hnd,
                                            SaHpiResourceIdT id,
                                            SaHpiResetActionT act ) {
   NewSimulator *newsim = 0;
   NewSimulatorResource *res = VerifyResourceAndEnter( hnd, id, newsim );

   if ( !res )
      return SA_ERR_HPI_NOT_PRESENT;

   SaErrorT rv = newsim->IfSetResetState( res, act );

   newsim->IfLeave();

   return rv;
}

} // new plugin_loader
//@}

/*
 * End of new plugin 
 ************************************************************************************/


/** 
 * 
 * 
 * @name Plugin interface
 * Defining alias names for the abi functions, If someone has an idea how the link
 * between both plugin interfaces can be documented, be free and give me a hint.
 * 
 */
//@{
extern "C" {

/// Alias definition
void * oh_open (GHashTable *, unsigned int, oh_evt_queue *) 
                __attribute__ ((weak, alias("NewSimulatorOpen")));

/// Alias definition
void * oh_close (void *) __attribute__ ((weak, alias("NewSimulatorClose")));

/// Alias definition
void * oh_get_event (void *)
                __attribute__ ((weak, alias("NewSimulatorGetEvent")));

/// Alias definition
void * oh_discover_resources (void *)
                __attribute__ ((weak, alias("NewSimulatorDiscoverResources")));

/// Alias definition
void * oh_set_resource_tag (void *, SaHpiResourceIdT, SaHpiTextBufferT *)
                __attribute__ ((weak, alias("NewSimulatorSetResourceTag")));

/// Alias definition
void * oh_set_resource_severity (void *, SaHpiResourceIdT, SaHpiSeverityT)
                __attribute__ ((weak, alias("NewSimulatorSetResourceSeverity")));

/// Alias definition
void * oh_get_el_info (void *, SaHpiResourceIdT, SaHpiEventLogInfoT *)
                __attribute__ ((weak, alias("NewSimulatorGetSelInfo")));

/// Alias definition
void * oh_set_el_time (void *, SaHpiResourceIdT, const SaHpiEventT *)
                __attribute__ ((weak, alias("NewSimulatorSetSelTime")));

/// Alias definition
void * oh_add_el_entry (void *, SaHpiResourceIdT, const SaHpiEventT *)
                __attribute__ ((weak, alias("NewSimulatorAddSelEntry")));

/// Alias definition
void * oh_get_el_entry (void *, SaHpiResourceIdT, SaHpiEventLogEntryIdT,
                       SaHpiEventLogEntryIdT *, SaHpiEventLogEntryIdT *,
                       SaHpiEventLogEntryT *, SaHpiRdrT *, SaHpiRptEntryT  *)
                __attribute__ ((weak, alias("NewSimulatorGetSelEntry")));

/// Alias definition
void * oh_clear_el (void *, SaHpiResourceIdT)
                __attribute__ ((weak, alias("NewSimulatorClearSel")));

/// Alias definition
void * oh_set_el_state (void *, SaHpiResourceIdT, SaHpiBoolT)
		        __attribute__ ((weak, alias("NewSimulatorSetSelState")));

/// Alias definition
void * oh_get_el_state (void *, SaHpiResourceIdT, SaHpiBoolT *)
		        __attribute__ ((weak, alias("NewSimulatorGetSelState")));

/// Alias definition		        
void * oh_get_el_caps (void *, SaHpiResourceIdT, SaHpiEventLogCapabilitiesT *)
	            __attribute__ ((weak, alias("NewSimulatorGetSelCapability")));

/// Alias definition
void * oh_reset_el_overflow (void *, SaHpiResourceIdT)
                __attribute__ ((weak, alias("NewSimulatorResetSelOverflow")));
                
/// Alias definition
void * oh_get_sensor_reading (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiSensorReadingT *,
                             SaHpiEventStateT    *)
                __attribute__ ((weak, alias("NewSimulatorGetSensorReading")));

/// Alias definition
void * oh_get_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("NewSimulatorGetSensorThresholds")));

/// Alias definition
void * oh_set_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 const SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("NewSimulatorSetSensorThresholds")));

/// Alias definition
void * oh_get_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT *)
                __attribute__ ((weak, alias("NewSimulatorGetSensorEnable")));

/// Alias definition
void * oh_set_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT)
                __attribute__ ((weak, alias("NewSimulatorSetSensorEnable")));

/// Alias definition
void * oh_get_sensor_event_enables (void *, SaHpiResourceIdT,
                                    SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak, alias("NewSimulatorGetSensorEventEnables")));

/// Alias definition
void * oh_set_sensor_event_enables (void *, SaHpiResourceIdT id, SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak, alias("NewSimulatorSetSensorEventEnables")));

/// Alias definition
void * oh_get_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiEventStateT *, SaHpiEventStateT *)
                __attribute__ ((weak, alias("NewSimulatorGetSensorEventMasks")));

/// Alias definition
void * oh_set_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiSensorEventMaskActionT,
                                  SaHpiEventStateT,
                                  SaHpiEventStateT)
                __attribute__ ((weak, alias("NewSimulatorSetSensorEventMasks")));

/// Alias definition
void * oh_get_control_state (void *, SaHpiResourceIdT, SaHpiCtrlNumT,
                             SaHpiCtrlModeT *, SaHpiCtrlStateT *)
                __attribute__ ((weak, alias("NewSimulatorGetControlState")));

/// Alias definition
void * oh_set_control_state (void *, SaHpiResourceIdT,SaHpiCtrlNumT,
                             SaHpiCtrlModeT, SaHpiCtrlStateT *)
                __attribute__ ((weak, alias("NewSimulatorSetControlState")));

/// Alias definition
void * oh_get_idr_info (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrInfoT *)
                __attribute__ ((weak, alias("NewSimulatorGetIdrInfo")));

/// Alias definition
void * oh_get_idr_area_header (void *, SaHpiResourceIdT, SaHpiIdrIdT,
                                SaHpiIdrAreaTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                                SaHpiIdrAreaHeaderT)
                __attribute__ ((weak, alias("NewSimulatorGetIdrAreaHeader")));

/// Alias definition
void * oh_add_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
                        SaHpiEntryIdT *)
                __attribute__ ((weak, alias("NewSimulatorAddIdrArea")));

/// Alias definition
void * oh_add_idr_area_id (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
                        SaHpiEntryIdT)
                __attribute__ ((weak, alias("NewSimulatorAddIdrAreaById")));
                
/// Alias definition
void * oh_del_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT)
                __attribute__ ((weak, alias("NewSimulatorDelIdrArea")));

/// Alias definition
void * oh_get_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldT)
                __attribute__ ((weak, alias("NewSimulatorGetIdrField")));

/// Alias definition
void * oh_add_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT *)
                __attribute__ ((weak, alias("NewSimulatorAddIdrField")));

/// Alias definition
void * oh_add_idr_field_id (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT *)
                __attribute__ ((weak, alias("NewSimulatorAddIdrFieldById")));
                
/// Alias definition
void * oh_set_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
                __attribute__ ((weak, alias("NewSimulatorSetIdrField")));

/// Alias definition
void * oh_del_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiEntryIdT)
                __attribute__ ((weak, alias("NewSimulatorDelIdrField")));

/// Alias definition
void * oh_hotswap_policy_cancel (void *, SaHpiResourceIdT, SaHpiTimeoutT)
                __attribute__ ((weak, alias("NewSimulatorHotswapPolicyCancel")));

/// Alias definition
void * oh_set_autoinsert_timeout (void *, SaHpiTimeoutT)
                __attribute__ ((weak, alias("NewSimulatorSetAutoInsertTimeout")));

/// Alias definition
void * oh_get_autoextract_timeout (void *, SaHpiResourceIdT, SaHpiTimeoutT *)
                __attribute__ ((weak, alias("NewSimulatorGetAutoExtractTimeout")));

/// Alias definition
void * oh_set_autoextract_timeout (void *, SaHpiResourceIdT, SaHpiTimeoutT)
                __attribute__ ((weak, alias("NewSimulatorSetAutoExtractTimeout")));

/// Alias definition
void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
                __attribute__ ((weak, alias("NewSimulatorGetHotswapState")));

/// Alias definition
void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
                __attribute__ ((weak, alias("NewSimulatorSetHotswapState")));

/// Alias definition
void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
                __attribute__ ((weak, alias("NewSimulatorRequestHotswapAction")));


/// Alias definition
void * oh_get_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT *)
                __attribute__ ((weak, alias("NewSimulatorGetPowerState")));

/// Alias definition
void * oh_set_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT)
                __attribute__ ((weak, alias("NewSimulatorSetPowerState")));

/// Alias definition
void * oh_get_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT *)
                __attribute__ ((weak, alias("NewSimulatorGetIndicatorState")));

/// Alias definition
void * oh_set_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT)
                __attribute__ ((weak, alias("NewSimulatorSetIndicatorState")));

/// Alias definition
void * oh_control_parm (void *, SaHpiResourceIdT, SaHpiParmActionT)
                __attribute__ ((weak, alias("NewSimulatorControlParm")));

/// Alias definition
void * oh_get_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT *)
                __attribute__ ((weak, alias("NewSimulatorGetResetState")));

/// Alias definition
void * oh_set_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT)
                __attribute__ ((weak, alias("NewSimulatorSetResetState")));

/// Alias definition
void * oh_get_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("NewSimulatorGetWatchdogInfo")));

/// Alias definition
void * oh_set_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("NewSimulatorSetWatchdogInfo")));

/// Alias definition
void * oh_reset_watchdog (void *, SaHpiResourceIdT , SaHpiWatchdogNumT )
                __attribute__ ((weak, alias("NewSimulatorResetWatchdog")));

/// Alias definition
void * oh_get_next_announce(void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT, 
                            SaHpiSeverityT, SaHpiBoolT, SaHpiAnnouncementT *)
               __attribute__ ((weak, alias("NewSimulatorGetNextAnnouncement")));

/// Alias definition       
void * oh_get_announce(void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT, 
                        SaHpiEntryIdT, SaHpiAnnouncementT *)
               __attribute__ ((weak, alias("NewSimulatorGetAnnouncement")));

/// Alias definition
void * oh_ack_announce(void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT, 
                        SaHpiEntryIdT, SaHpiSeverityT)
               __attribute__ ((weak, alias("NewSimulatorAckAnnouncement")));

/// Alias definition              
void * oh_add_announce(void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT, 
                        SaHpiAnnouncementT *)
               __attribute__ ((weak, alias("NewSimulatorAddAnnouncement")));

/// Alias definition
void * oh_del_announce(void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT, 
                        SaHpiEntryIdT, SaHpiSeverityT)
               __attribute__ ((weak, alias("NewSimulatorDelAnnouncement")));

/// Alias definition             
void * oh_get_annunc_mode(void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                          SaHpiAnnunciatorModeT *)
               __attribute__ ((weak, alias("NewSimulatorGetAnnMode")));

/// Alias definition               
void * oh_set_annunc_mode(void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                          SaHpiAnnunciatorModeT)
               __attribute__ ((weak, alias("NewSimulatorSetAnnMode")));

/// Alias definition               
void * oh_get_fumi_spec(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                         SaHpiFumiSpecInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiSpec")));

/// Alias definition               
void * oh_get_fumi_service_impact(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                                  SaHpiFumiServiceImpactDataT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiServImpact")));

/// Alias definition               
void * oh_set_fumi_source(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                          SaHpiBankNumT, SaHpiTextBufferT *)
               __attribute__ ((weak, alias("NewSimulatorSetFumiSource")));

/// Alias definition               
void * oh_validate_fumi_source(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBankNumT)
               __attribute__ ((weak, alias("NewSimulatorValidateFumiSource")));
                              
/// Alias definition               
void * oh_get_fumi_source(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBankNumT,
                           SaHpiFumiSourceInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiSource")));

/// Alias definition               
void * oh_get_fumi_source_component(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                                    SaHpiBankNumT, SaHpiEntryIdT,
                                    SaHpiEntryIdT *, SaHpiFumiComponentInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiSourceComponent")));

/// Alias definition               
void * oh_get_fumi_target(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                           SaHpiBankNumT, SaHpiFumiBankInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiTarget")));
               
/// Alias definition               
void * oh_get_fumi_target_component(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                                    SaHpiBankNumT, SaHpiEntryIdT,
                                    SaHpiEntryIdT *, SaHpiFumiComponentInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiTargetComponent")));   

/// Alias definition               
void * oh_get_fumi_logical_target(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                                   SaHpiFumiLogicalBankInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiLogicalTarget")));

/// Alias definition               
void * oh_get_fumi_logical_target_component(void *, SaHpiResourceIdT, SaHpiFumiNumT,
                                             SaHpiEntryIdT, SaHpiEntryIdT *, 
                                             SaHpiFumiLogicalComponentInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiLogicalTargetComponent")));

/// Alias definition               
void * oh_start_fumi_backup(void *, SaHpiResourceIdT, SaHpiFumiNumT)
               __attribute__ ((weak, alias("NewSimulatorStartFumiBackup")));

/// Alias definition               
void * oh_set_fumi_bank_order(void *, SaHpiResourceIdT, SaHpiFumiNumT,
                               SaHpiBankNumT, SaHpiUint32T)
               __attribute__ ((weak, alias("NewSimulatorSetFumiBankOrder")));

/// Alias definition               
void * oh_start_fumi_bank_copy(void *, SaHpiResourceIdT, SaHpiFumiNumT,
                                SaHpiBankNumT, SaHpiBankNumT)
               __attribute__ ((weak, alias("NewSimulatorStartFumiBankCopy")));

/// Alias definition               
void * oh_start_fumi_install(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBankNumT)
               __attribute__ ((weak, alias("NewSimulatorStartFumiInstall")));

/// Alias definition               
void * oh_get_fumi_status(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBankNumT,
                           SaHpiFumiUpgradeStatusT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiStatus")));

/// Alias definition               
void * oh_start_fumi_verify(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBankNumT)
               __attribute__ ((weak, alias("NewSimulatorStartFumiVerification")));

/// Alias definition               
void * oh_start_fumi_verify_main(void *, SaHpiResourceIdT, SaHpiFumiNumT)
               __attribute__ ((weak, alias("NewSimulatorStartFumiVerificationMain")));

/// Alias definition               
void * oh_cancel_fumi_upgrade(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBankNumT)
               __attribute__ ((weak, alias("NewSimulatorCancelFumiUpgrade")));

/// Alias definition
void * oh_get_fumi_autorollback_disable(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                                         SaHpiBoolT *)
               __attribute__ ((weak, alias("NewSimulatorGetFumiRollback")));

/// Alias definition
void * oh_set_fumi_autorollback_disable(void *, SaHpiResourceIdT, SaHpiFumiNumT, 
                                         SaHpiBoolT)
               __attribute__ ((weak, alias("NewSimulatorSetFumiRollback")));

/// Alias definition
void * oh_start_fumi_rollback(void *, SaHpiResourceIdT, SaHpiFumiNumT)
               __attribute__ ((weak, alias("NewSimulatorStartFumiRollback")));

/// Alias definition
void * oh_activate_fumi(void *, SaHpiResourceIdT, SaHpiFumiNumT)
               __attribute__ ((weak, alias("NewSimulatorActivateFumi")));

/// Alias definition
void * oh_start_fumi_activate(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBoolT)
               __attribute__ ((weak, alias("NewSimulatorStartFumiActivation")));

/// Alias definition
void * oh_cleanup_fumi(void *, SaHpiResourceIdT, SaHpiFumiNumT, SaHpiBankNumT)
               __attribute__ ((weak, alias("NewSimulatorCleanupFumi")));

/// Alias definition
void * oh_get_dimi_info(void *, SaHpiResourceIdT, SaHpiDimiNumT, SaHpiDimiInfoT *)
               __attribute__ ((weak, alias("NewSimulatorGetDimiInfo")));

/// Alias definition
void * oh_get_dimi_test(void *, SaHpiResourceIdT, SaHpiDimiNumT,
                         SaHpiDimiTestNumT, SaHpiDimiTestT *)
               __attribute__ ((weak, alias("NewSimulatorGetDimiTestInfo")));

/// Alias definition
void * oh_get_dimi_test_ready(void *, SaHpiResourceIdT, SaHpiDimiNumT,
                               SaHpiDimiTestNumT, SaHpiDimiReadyT *)
               __attribute__ ((weak, alias("NewSimulatorGetDimiTestReadiness")));

/// Alias definition
void * oh_start_dimi_test(void *, SaHpiResourceIdT, SaHpiDimiNumT, SaHpiDimiTestNumT,
                           SaHpiUint8T, SaHpiDimiTestVariableParamsT *)
               __attribute__ ((weak, alias("NewSimulatorStartDimiTest")));

/// Alias definition
void * oh_cancel_dimi_test(void *, SaHpiResourceIdT, SaHpiDimiNumT, 
                            SaHpiDimiTestNumT)
               __attribute__ ((weak, alias("NewSimulatorCancelDimiTest")));

/// Alias definition
void * oh_get_dimi_test_status(void *, SaHpiResourceIdT, SaHpiDimiNumT, 
                                SaHpiDimiTestNumT, 
                                SaHpiDimiTestPercentCompletedT *,
                                SaHpiDimiTestRunStatusT *)
               __attribute__ ((weak, alias("NewSimulatorGetDimiTestStatus")));
               
/// Alias definition
void * oh_get_dimi_test_results(void *, SaHpiResourceIdT, SaHpiDimiNumT, 
                                 SaHpiDimiTestNumT, SaHpiDimiTestResultsT *)
               __attribute__ ((weak, alias("NewSimulatorGetDimiTestResults")));

}               
//@}


/*
 * Isn't needed at the moment
static unsigned int GetIntNotNull( GHashTable *handler_config, const char *str, 
                                      unsigned int def = 0 ) {
  const char *value = (const char *)g_hash_table_lookup(handler_config, str );

  if ( !value )
       return def;

  unsigned int v = strtol( value, 0, 0 );

  if ( v == 0 )
       return def;

  return v;
}

static SaHpiTimeoutT GetTimeout( GHashTable *handler_config, const char *str, 
                                  SaHpiTimeoutT def ) {
  const char *value = (const char *)g_hash_table_lookup(handler_config, str );

  if ( !value )
       return def;

  int v = strtol( value, 0, 0 );

  if ( v == 0 )
       return SAHPI_TIMEOUT_IMMEDIATE;

  if ( v == -1 )
       return SAHPI_TIMEOUT_BLOCK;

  SaHpiTimeoutT timeout = v * 1000000000;

  return timeout;
}
*/



/*****************************************************************
 * Implementation of class NewSimulator
 */

/**
 * Constructor
 **/
NewSimulator::NewSimulator() : m_magic( dNewSimulatorMagic ), m_handler( 0 ) {}

/**
 * Destructor
 **/
NewSimulator::~NewSimulator() {}

/**
 * Set handler pointer
 * 
 * @param handler pointer on plugin handler
 **/
void NewSimulator::SetHandler( oh_handler_state *handler ) {
   m_handler = handler;
}


/**
 * Add an HPI event in the queue 
 * 
 * @param event pointer on event to be added
 **/
void NewSimulator::AddHpiEvent( oh_event *event ) {
   m_event_lock.Lock();

   if ( m_handler ) {
     event->hid = m_handler->hid;
     oh_evt_queue_push(m_handler->eventq, event);
   }

   m_event_lock.Unlock();
}


/**
 * Return the root entity path 
 * 
 * @return address of root entity path
 **/
const NewSimulatorEntityPath &NewSimulator::EntityRoot() {
   return m_entity_root;
}


/**
 * Return pointer on the plugin handler 
 * 
 * @return pointer on plugin handler
 **/
oh_handler_state *NewSimulator::GetHandler() {
  return m_handler;
}


/**
 * Return the rpt entry of a resource 
 * 
 * @param rid resource id to be found
 * @return pointer on plugin handler
 **/
SaHpiRptEntryT * NewSimulator::FindResource( SaHpiResourceIdT rid ) {
  if ( m_handler ) {
      return oh_get_resource_by_id( m_handler->rptcache, rid);
  } else {
      return 0;
  }
}


/**
 * Interface function Enter - only a lock is set
 **/
void NewSimulator::IfEnter() {
	
  ReadLock();
}

/**
 * Interface function Leave - only a lock is unset
 **/
void NewSimulator::IfLeave() {
  ReadUnlock();
}

/**
 * Get the parameter from the configuration file. At the moment it
 * is only a dummy method, since the filename param and entity root
 * are read inside NewSimulator::Init().
 * 
 * @param handler_config pointer on the configuration hash table
 * @return true
 **/
bool NewSimulator::GetParams( GHashTable *handler_config ) {

  return true;
}


/**
 * Interface Open. The parameters entity root and filename are read from 
 * the hash table and it is tried to open the file by generating a new 
 * NewSimulatorFile object and calling NewSimulatorFile::Open().\n
 * The Initializiation itself is done inside NewSimulatorDomain::Init() which is 
 * called inside this method.
 * 
 * @param  handler_config pointer on the configuration hash table
 * @return error code of initialization
 **/
bool NewSimulator::IfOpen( GHashTable *handler_config ) {
	
   stdlog << "DBG: We are inside IfOpen\n";
   const char *entity_root = (const char *)g_hash_table_lookup( handler_config, "entity_root" );

   if ( !entity_root ) {
      err( "entity_root is missing in config file" );
      return false;
   }

   if ( !m_entity_root.FromString( entity_root ) ) {
      err( "cannot decode entity path string" );
      return false;
   }

   const char *filename = (const char *)g_hash_table_lookup( handler_config, "file" );
   
   if ( !filename ) {
   	  err("file is missing in config file" );
   	  return false;
   }
   
   NewSimulatorFile *simfile = new NewSimulatorFile( filename, m_entity_root );
   
   if ( !simfile ) {
      stdlog << "NewSimulator cannot alloc File object !\n";
      return false;
   }

   bool rv = simfile->Open();
   
   if ( rv == false ) {
      stdlog << "File open connection fails !\n";
      delete simfile;
      return false;
   }

   if ( !Init( simfile ) ) {
      IfClose();
      return false;
   }

   return true;
}

/**
 * Interface Close - deletion of file object
 **/
void NewSimulator::IfClose() {
  Cleanup();

  if ( m_file ) {
    delete m_file;
    m_file = 0;
  }
}

/**
 * Interface GetEvent
 * m_event_lock is set and unset.
 * It is also used to reduce the keep alive interval time
 * 
 * @param event pointer on oh_event
 * @return 0
 **/
int NewSimulator::IfGetEvent( oh_event *event ) {
   int rv = 0;
   
   m_event_lock.Lock();
   m_event_lock.Unlock();

   return rv;
}


/**
 * Interface Discover - check whether all resources are
 * discovered.
 * m_initial_discover_lock is set and unset.
 * 
 * @return SA_OK
 * 
 **/
SaErrorT NewSimulator::IfDiscoverResources() {
  dbg( "NewSimulator::IfDiscoverResources");
  stdlog << "DBG: NewSimulator::IfDiscoverResources ...\n";

  bool loop;

  do {
     usleep( 10000 );

     m_initial_discover_lock.Lock();
     loop = m_initial_discover ? true : false;
     m_initial_discover_lock.Unlock();
  } while( loop );

  stdlog << "DBG: Return simple OK\n";
  return SA_OK;
}


/** 
 * HPI function saHpiResourceTagSet()
 * 
 * See also the description of the function inside the specification or header file.
 * The resource tag is set.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param ent pointer on NewSimulatorResource object for which the resource tag should be set
 * @param tag pointer on SaHpiTextBufferT with tag information to be set
 *  
 * @return HPI error code 
 **/
 SaErrorT NewSimulator::IfSetResourceTag( NewSimulatorResource *ent, SaHpiTextBufferT *tag ) {
  // change tag in plugin cache
  SaHpiRptEntryT *rptentry = oh_get_resource_by_id( ent->Domain()->GetHandler()->rptcache,
                                                    ent->ResourceId() );
  if ( !rptentry )
     return SA_ERR_HPI_NOT_PRESENT;

  memcpy(&rptentry->ResourceTag, tag, sizeof(SaHpiTextBufferT));

  oh_add_resource(ent->Domain()->GetHandler()->rptcache, rptentry, ent, 1);

  return SA_OK;
}


/** 
 * HPI function saHpiResourceSeveritySet()
 * 
 * See also the description of the function inside the specification or header file.
 * The resource severity is set.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param ent pointer on NewSimulatorResource object for which severity should be set
 * @param sev severity to be set
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfSetResourceSeverity( NewSimulatorResource *ent, SaHpiSeverityT sev ) {
  // change severity in plugin cache
  SaHpiRptEntryT *rptentry = oh_get_resource_by_id( ent->Domain()->GetHandler()->rptcache,
                                                    ent->ResourceId() );
  if ( !rptentry )
     return SA_ERR_HPI_NOT_PRESENT;

  rptentry->ResourceSeverity = sev;

  oh_add_resource(ent->Domain()->GetHandler()->rptcache, rptentry, ent, 1);

  return SA_OK;
}

/** 
 * HPI function saHpiAutoInsertTimeoutSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Set the insertion timeout value
 * 
 * @todo it fits better to implement it in class NewSimulatorDomain
 * 
 * @param timeout timeout value to be set
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfSetAutoInsertTimeout( SaHpiTimeoutT timeout ) {

  InsertTimeout() = timeout;

  return SA_OK;
}



/** 
 * HPI function saHpiResourcePowerStateGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Get the power state of a resource.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param res pointer on NewSimulatorResource to be used
 * @param state address of power state to be filled
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfGetPowerState( NewSimulatorResource *res, SaHpiPowerStateT &state ) {

   state = res->PowerState();

   return SA_OK;
}


/** 
 * HPI function saHpiResourcePowerStateSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Set the power state of a resource.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param res pointer on NewSimulatorResource to be used
 * @param state power state to be set
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfSetPowerState( NewSimulatorResource *res, SaHpiPowerStateT state ) {
   SaHpiPowerStateT origState;
   
   if (!(res->ResourceCapabilities() & SAHPI_CAPABILITY_POWER))
      return SA_ERR_HPI_CAPABILITY;
   
   origState = res->PowerState();

   /// @todo Generate some proper events for the power state transition
   if (state == SAHPI_POWER_CYCLE) {
      /// @todo: Depentend on the state of the resource send the transition event

      return SA_OK;

   } else if (state > SAHPI_POWER_STATE_MAX_VALID) {
      return SA_ERR_HPI_INVALID_PARAMS;

   } else {
      if (state != origState) {
      	// TODO: Generate proper events
        res->PowerState() = state;

      } else {
      	return SA_OK;
      }
   }  
   return SA_OK;
}

/** 
 * HPI function saHpiHotSwapIndicatorStateGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Get the indicator state of a resource.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param res pointer on NewSimulatorResource to be used
 * @param state address of indicator state to be filled
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfGetIndicatorState( NewSimulatorResource *res, SaHpiHsIndicatorStateT &state ) {
   
   if (!(res->ResourceCapabilities() & SAHPI_CAPABILITY_MANAGED_HOTSWAP) ||
       !(res->HotSwapCapabilities() & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED))
      return SA_ERR_HPI_CAPABILITY;

   state = res->HSIndicator();
   
   return SA_OK;
}


/** 
 * HPI function saHpiHotSwapIndicatorStateSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Get the indicator state of a resource.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param res pointer on NewSimulatorResource to be used
 * @param state indicator state to be set
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfSetIndicatorState( NewSimulatorResource *res, SaHpiHsIndicatorStateT state ) {
   
   if (!(res->ResourceCapabilities() & SAHPI_CAPABILITY_MANAGED_HOTSWAP) ||
       !(res->HotSwapCapabilities() & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED))
      return SA_ERR_HPI_CAPABILITY;
   
   if ( state > SAHPI_HS_INDICATOR_STATE_MAX_VALID )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   res->HSIndicator() = state;
   
   return SA_OK;
}


/** 
 * HPI function saHpiResourceResetStateGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Get the reset state of a resource.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param res pointer on NewSimulatorResource to be used
 * @param state address of reset state to be filled
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfGetResetState( NewSimulatorResource *res, SaHpiResetActionT &state ) {
   
   if (!(res->ResourceCapabilities() & SAHPI_CAPABILITY_RESET))
      return SA_ERR_HPI_CAPABILITY; 
  
   state = SAHPI_RESET_DEASSERT;

   return SA_OK;
}


/** 
 * HPI function saHpiResourceResetStateSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Get the reset state of a resource.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource
 * 
 * @param res pointer on NewSimulatorResource to be used
 * @param state Reset action
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfSetResetState( NewSimulatorResource *res, SaHpiResetActionT state ) {
   
   if (!(res->ResourceCapabilities() & SAHPI_CAPABILITY_RESET))
      return SA_ERR_HPI_CAPABILITY;
       
   if (state > SAHPI_RESET_MAX_VALID)
      return SA_ERR_HPI_INVALID_PARAMS;
      
   if ( (res->ResetState() == SAHPI_RESET_ASSERT) &&
        ((state == SAHPI_COLD_RESET) || (state == SAHPI_WARM_RESET)) )
      return SA_ERR_HPI_INVALID_REQUEST;

   if ( (state == SAHPI_COLD_RESET) || (state == SAHPI_WARM_RESET) ) {
      /// @todo Send proper transition events as if a reset was done
      return SA_OK;
   
   } else {
      res->ResetState() = state;
      return SA_OK;
   }
  
}

/** 
 * HPI function saHpiParmControl()
 * 
 * See also the description of the function inside the specification or header file.
 * Conrol the parameter of a resource.
 * 
 * @todo it fits better to implement it in class NewSimulatorResource - at the moment 
 * it is only a method stub 
 * 
 * @param res pointer on NewSimulatorResource to be used
 * @param act parm action to be done
 *  
 * @return HPI error code 
 **/
SaErrorT NewSimulator::IfControlParm( NewSimulatorResource * /*res*/, SaHpiParmActionT act )
{
  /// @todo implementation
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

