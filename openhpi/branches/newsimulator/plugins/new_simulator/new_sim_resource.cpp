/** 
 * @file    new_sim_resource.cpp
 *
 * The file includes a class for handling resources:
 * NewSimulatorResource
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
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 * 
 **/
 
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <assert.h>

#include "new_sim_resource.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorResource::NewSimulatorResource( NewSimulatorDomain *domain )
                     : m_domain( domain ), 
                       m_is_fru( false ),
                       m_policy_canceled( true ),
                       m_oem( 0 ), 
                       m_current_control_id( 0 ),
                       m_populate( false ) {

   m_extract_timeout = Domain()->ExtractTimeout();
   for( int i = 0; i < 256; i++ ) m_sensor_num[i] = -1;
   m_power_state = SAHPI_POWER_OFF;
   memset( &m_rpt_entry, 0, sizeof( SaHpiRptEntryT ));
   m_hotswap_indicator = SAHPI_HS_INDICATOR_OFF;
   m_reset_state = SAHPI_RESET_DEASSERT;
}


/**
 * Destructor
 **/
NewSimulatorResource::~NewSimulatorResource() {}

/**
 * Return the domain to which the resource is mapped
 * 
 * @return pointer on domain
 **/
NewSimulatorDomain *NewSimulatorResource::Domain() const {
   return m_domain;
}

/**
 * @deprecated Create a Sensor number
 * 
 * @param num sensor number
 * @return sensor number to be used in HPI
 **/
int NewSimulatorResource::CreateSensorNum( SaHpiSensorNumT num ) {
   int v = num;

   if ( m_sensor_num[v] != -1 ) {
      for( int i = 0xff; i >= 0; i-- )
         if ( m_sensor_num[i] == -1 ) {
            v = i;
            break;
         }
         if ( m_sensor_num[v] != -1 ) {
            assert( 0 );
            return -1;
         }
   }

   m_sensor_num[v] = num;

   return v;
}

/**
 * Set the resource information
 * 
 * @param resinfo resource information to be set in the resource
 **/
void NewSimulatorResource::SetResourceInfo( SaHpiResourceInfoT resinfo ) {
	
   memcpy( &m_rpt_entry.ResourceInfo, &resinfo, sizeof( SaHpiResourceInfoT ) );
}


/**
 * Destroy the resource and clean up
 * 
 * @return success
 **/
bool NewSimulatorResource::Destroy() {
   SaHpiRptEntryT *rptentry;
   stdlog << "removing resource: " << m_entity_path << ").\n";

   while( Num() ) {
      NewSimulatorRdr *rdr = GetRdr( 0 );
      RemRdr( rdr );
      delete rdr;
   }
  
   rptentry = oh_get_resource_by_id( Domain()->GetHandler()->rptcache, m_rpt_entry.ResourceId );
   if ( !rptentry ) {
      stdlog << "Can't find resource in plugin cache !\n";
   } else {
      // create remove event
      oh_event *e = (oh_event *)g_malloc0( sizeof( oh_event ) );
  
      // remove sensors only if resource is FRU
      if ( rptentry->ResourceCapabilities & SAHPI_CAPABILITY_FRU ) {
         e->event.EventType = SAHPI_ET_HOTSWAP;
         if (e->resource.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
            e->event.EventDataUnion.HotSwapEvent.HotSwapState = SAHPI_HS_STATE_NOT_PRESENT;
            e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = SAHPI_HS_STATE_NOT_PRESENT;
         } else {
            e->event.EventDataUnion.HotSwapEvent.HotSwapState = SAHPI_HS_STATE_NOT_PRESENT;
            e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
         }
      } else {
            e->event.EventType = SAHPI_ET_RESOURCE;
            e->event.EventDataUnion.ResourceEvent.ResourceEventType = SAHPI_RESE_RESOURCE_FAILURE;
            rptentry->ResourceFailed = SAHPI_TRUE;
      }
  
      e->event.Source = rptentry->ResourceId;
      oh_gettimeofday(&e->event.Timestamp);
      e->event.Severity = rptentry->ResourceSeverity;
      e->resource = *rptentry;
      stdlog << "NewSimulatorResource::Destroy OH_ET_RESOURCE_DEL Event resource " << m_rpt_entry.ResourceId << "\n";
      Domain()->AddHpiEvent( e );

      // remove resource from local cache
      int rv = oh_remove_resource( Domain()->GetHandler()->rptcache, m_rpt_entry.ResourceId );

      if ( rv != 0 ) {
         stdlog << "Can't remove resource from plugin cache !\n";
      }
   }
  
   m_domain->RemResource( this );

   delete this;
   
   return true;
}


/**
 * Find a rdr entry with the correct type and number
 * 
 * @param type type of rdr entry
 * @param num number of rdr entry
 * 
 * @return pointer on the rdr object 
 **/
NewSimulatorRdr *NewSimulatorResource::FindRdr( SaHpiRdrTypeT type, unsigned int num) {
   for( int i = 0; i < NumRdr(); i++ ) {
      NewSimulatorRdr *r = GetRdr( i );

      if ( r->Type() == type && r->Num()  == num )
	      return r;
   }

   return 0;
}


/**
 * Add a rdr entry
 * 
 * @param rdr pointer on the rdr to be added
 * @return success
 **/
bool NewSimulatorResource::AddRdr( NewSimulatorRdr *rdr ) {
   stdlog << "adding rdr: " << rdr->EntityPath();
   stdlog << " " << rdr->Num();
   stdlog << " " << rdr->IdString() << "\n";

   // set resource
   rdr->Resource() = this;

   // add rdr to resource
   Add( rdr );

   return true;
}


/**
 * Remove a rdr entry
 * 
 * @param rdr pointer on the rdr to be deleted
 * @return success
 **/
bool NewSimulatorResource::RemRdr( NewSimulatorRdr *rdr ) {
   int idx = Find( rdr );
  
   if ( idx == -1 ) {
      stdlog << "user requested removal of a control"
                " from a resource, but the control was not there !\n";
      return false;
   }

   Rem( idx );

   return true;
}


/**
 * Check Watchdog Timer 
 * 
 * NewSimulatorWatchdog:CheckWatchdogTimer is called and if it returns true a true is returned.
 * 
 * @return true if one watchdog timer is running
 */
bool NewSimulatorResource::CheckWatchdogTimer() {
   for( int i = 0; i < NumRdr(); i++ ) {
      NewSimulatorRdr *r = GetRdr( i );

      if ( r->Type() == SAHPI_WATCHDOG_RDR ) {
         NewSimulatorWatchdog *wdt = dynamic_cast<NewSimulatorWatchdog *>( r );
         if ( wdt->CheckWatchdogTimer() )
            return true;
      }
   }
   
   return false;
}
/**
 * Initialize a new resource 
 * 
 * @param entry address of RptEntry structure
 * @return success
 **/
bool NewSimulatorResource::Create( SaHpiRptEntryT &entry ) {

   stdlog << "DBG: Resource::Create: " << m_entity_path << ".\n";
   stdlog << "DBG: Should be checked\n";
   
   entry.EntryId = 0;

   // resource info
   SaHpiResourceInfoT &info = entry.ResourceInfo;

   memset( &info, 0, sizeof( SaHpiResourceInfoT ) );

   entry.ResourceEntity = m_entity_path;
   entry.ResourceId     = oh_uid_from_entity_path( &entry.ResourceEntity );

   entry.ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE;

//   if ( m_sel )
//     entry.ResourceCapabilities |= SAHPI_CAPABILITY_EVENT_LOG;

   if ( m_is_fru == true ) {
        entry.ResourceCapabilities |= SAHPI_CAPABILITY_FRU;
   }

   entry.HotSwapCapabilities = 0;
   entry.ResourceSeverity = SAHPI_OK;
   entry.ResourceFailed = SAHPI_FALSE;
   entry.ResourceTag = ResourceTag();

   return true;
}


/**
 * Popolate the resource
 * 
 * @return success
 **/
bool NewSimulatorResource::Populate() {
   stdlog << "DBG: Start Populate()\n";
   
   if ( m_populate == false ) {
      stdlog << "DBG: populate resource: " << EntityPath() << ".\n";

      // Work with some rpt_entries 
      m_rpt_entry.ResourceTag = ResourceTag();
      if (m_rpt_entry.ResourceCapabilities & SAHPI_CAPABILITY_FRU) {
         m_is_fru = true;
      } else { 
         m_is_fru = false;
      }
         
      struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );
      // Get own ResourceId
      e->resource.ResourceEntity = m_entity_path;
      m_rpt_entry.ResourceEntity = m_entity_path;
      m_rpt_entry.ResourceId = oh_uid_from_entity_path( &(e->resource.ResourceEntity) );

      memcpy(&e->resource, &m_rpt_entry, sizeof (SaHpiRptEntryT));

      // entry.ResourceEntity       = m_entity_path;
      // entry.ResourceId           = oh_uid_from_entity_path( &entry.ResourceEntity );
      // if ( Create( e->resource ) == false ) {
      //    g_free( e );
      //    return false;
      // }

      // assign the hpi resource id to ent, so we can find
      // the resource for a given entity
      // m_resource_id = e->resource.ResourceId;

      // add the resource to the resource cache
      int rv = oh_add_resource( Domain()->GetHandler()->rptcache,
                                 &(e->resource), this, 1 );

      if ( rv != 0 ) {
         stdlog << "Can't add resource to plugin cache !\n";
         g_free( e );
         return false;
      }

      for( int i = 0; i < NumRdr(); i++ ) {
         NewSimulatorRdr *rdr = GetRdr( i );
         if ( rdr->Populate(&e->rdrs) == false ) return false;
      }

      // find updated resource in plugin cache
      SaHpiRptEntryT *resource = oh_get_resource_by_id( Domain()->GetHandler()->rptcache,
                                                        m_rpt_entry.ResourceId );
      if (!resource) return false;

      // Update resource in event accordingly
      memcpy(&e->resource, resource, sizeof (SaHpiRptEntryT));

      if (e->resource.ResourceCapabilities & SAHPI_CAPABILITY_FRU) {
         e->event.EventType = SAHPI_ET_HOTSWAP;

         if (e->resource.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
            SaHpiHsStateT hpi_state = GetHpiState();
            e->event.EventDataUnion.HotSwapEvent.HotSwapState = hpi_state;
            e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = hpi_state;
            stdlog << "NewSimulatorResource::Populate SAHPI_ET_HOTSWAP Managed FRU Event resource " << m_rpt_entry.ResourceId << " State " << hpi_state << "\n";
         } else {
            e->event.EventDataUnion.HotSwapEvent.HotSwapState = SAHPI_HS_STATE_ACTIVE;
            e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
            stdlog << "NewSimulatorResource::Populate SAHPI_ET_HOTSWAP FRU Event resource " << m_rpt_entry.ResourceId << "\n";
         }
      } else {
         e->event.EventType = SAHPI_ET_RESOURCE;
         e->event.EventDataUnion.ResourceEvent.ResourceEventType = SAHPI_RESE_RESOURCE_ADDED;
         stdlog << "NewSimulatorResource::Populate SAHPI_ET_RESOURCE Event resource " << m_rpt_entry.ResourceId << "\n";
      }

      e->event.Source = e->resource.ResourceId;
      e->event.Severity = e->resource.ResourceSeverity;
      oh_gettimeofday(&e->event.Timestamp);

      Domain()->AddHpiEvent( e );
  
      m_populate = true;
   }

   return true;
}


/**
 * Activate the resource
 *
 * @todo enhance the method by using timer to allow interruption
 * (hotswap policy cancel)
 *  
 * @return success
 **/
bool NewSimulatorResource::Activate() {
   //TODO: Fix the activation of a resource
   // Change the state properly
   // Allow to interrupt the activation by using a sleep time
   
   struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

   // TODO: Check when this method is called and what is expected 
   if ( Create( e->resource ) == false ) {
      g_free( e );
      return false;
   }

   m_hotswap_state = SAHPI_HS_STATE_ACTIVE;
   
   if (e->resource.ResourceCapabilities & SAHPI_CAPABILITY_FRU) {
      e->event.EventType = SAHPI_ET_HOTSWAP;
      if (e->resource.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
         SaHpiHsStateT hpi_state = GetHpiState();
         e->event.EventDataUnion.HotSwapEvent.HotSwapState = hpi_state;
         e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = hpi_state;
         stdlog << "NewSimulatorResource::Populate SAHPI_ET_HOTSWAP Managed FRU Event resource " << m_rpt_entry.ResourceId << " State " << hpi_state << "\n";
      } else {
         e->event.EventDataUnion.HotSwapEvent.HotSwapState = SAHPI_HS_STATE_ACTIVE;
         e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
         stdlog << "NewSimulatorResource::Populate SAHPI_ET_HOTSWAP FRU Event resource " << m_rpt_entry.ResourceId << "\n";
      }
   } else {
      e->event.EventType = SAHPI_ET_RESOURCE;
      e->event.EventDataUnion.ResourceEvent.ResourceEventType = SAHPI_RESE_RESOURCE_ADDED;
      stdlog << "NewSimulatorResource::Populate SAHPI_ET_RESOURCE Event resource " << m_rpt_entry.ResourceId << "\n";
   }

   e->event.Source = e->resource.ResourceId;
   e->event.Severity = e->resource.ResourceSeverity;
   oh_gettimeofday(&e->event.Timestamp);

   Domain()->AddHpiEvent( e );
   
   return true;
}


/**
 * Deactivation of the resource
 *
 * @todo implement deactivation process
 *  
 * @return success
 **/
void NewSimulatorResource::Deactivate() {
    // TODO: Do the same with the deactivation 
 	// Send a first hpi event wait to receive such an event
 	// Do the next step
 	
 	stdlog << "DBG: Resource.Deactivate is called but nothing is done --> TODO!\n";
 
}


/**
 * Set HPI state of resource 
 *
 * @todo implement the proper generation of events
 * 
 * @param new_hs_state hotswap state to be set
 * @return success
 **/
SaHpiHsStateT NewSimulatorResource::SetHpiState( SaHpiHsStateT new_hs_state ) {
   // TODO: Change it properly -- is new
   // Send the correct events
   
   m_hotswap_state = new_hs_state;
   stdlog << "DBG: Resource.SetHpiState is called with state " << new_hs_state << " \n";
   
   return m_hotswap_state;
}


/**
 * Dump the information of the resource 
 *
 * @param dump address of logfile
 **/
void NewSimulatorResource::Dump( NewSimulatorLog &dump ) const {
	
   dump << "Resource " << m_rpt_entry.ResourceId << " " << m_entity_path << "\n";
   dump << "   EntryId              " << m_rpt_entry.EntryId << "\n";
   dump << "   ResourceId           " << m_rpt_entry.ResourceId << "\n";
   dump << "   ResourceInfo\n";
   dump << "      ResourceRev       " << m_rpt_entry.ResourceInfo.ResourceRev << "\n";
   dump << "      SpecificVer       " << m_rpt_entry.ResourceInfo.SpecificVer << "\n";
   dump << "      DeviceSupport     " << m_rpt_entry.ResourceInfo.DeviceSupport << "\n";
   dump << "      ManufacturerId    " << m_rpt_entry.ResourceInfo.ManufacturerId << "\n";
   dump << "      ProductId         " << m_rpt_entry.ResourceInfo.ProductId << "\n";
   dump << "      FirmwareMajorRev  " << m_rpt_entry.ResourceInfo.FirmwareMajorRev << "\n";
   dump << "      FirmwareMinorRev  " << m_rpt_entry.ResourceInfo.FirmwareMinorRev << "\n";
   dump << "      AuxFirmwareRev    " << m_rpt_entry.ResourceInfo.AuxFirmwareRev << "\n";
   dump << "   ResourceEntity " << m_rpt_entry.ResourceEntity << "\n";
   dump << "   ResourceCapabilities " << m_rpt_entry.ResourceCapabilities << "\n";
   dump << "   HotSwapCapabilities  " << m_rpt_entry.HotSwapCapabilities << "\n";
   dump << "   ResourceSeverity     " << m_rpt_entry.ResourceSeverity << "\n";
   dump << "   ResourceFailed       " << m_rpt_entry.ResourceFailed  << "\n";
   dump << "   ResourceTag          " << m_rpt_entry.ResourceTag << "\n";
   dump << "------------------------\n";

}
