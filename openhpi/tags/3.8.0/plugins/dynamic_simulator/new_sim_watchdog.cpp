/** 
 * @file    new_sim_watchdog.cpp
 *
 * The file includes a class for watchdog handling:\n
 * NewSimulatorWatchdog
 * 
 * @todo maybe some watchdog actions
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
 */

#include <oh_error.h>
 
#include "new_sim_watchdog.h"
#include "new_sim_utils.h"
#include "new_sim_domain.h"
#include "new_sim_timer_thread.h"

/**
 * Constructor
 **/
NewSimulatorWatchdog::NewSimulatorWatchdog( NewSimulatorResource *res )
                    : NewSimulatorRdr( res, SAHPI_WATCHDOG_RDR ),
                      NewSimulatorTimerThread( 0 ),
                      m_state( NONE ) {

   memset( &m_wdt_rec, 0, sizeof( SaHpiWatchdogRecT ));
   memset( &m_wdt_data, 0, sizeof( SaHpiWatchdogT ));
   
}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorWatchdog::NewSimulatorWatchdog( NewSimulatorResource *res, 
                      SaHpiRdrT rdr, 
                      SaHpiWatchdogT wdt_data)
                    : NewSimulatorRdr( res, SAHPI_WATCHDOG_RDR, rdr.Entity, rdr.IsFru, rdr.IdString ),
                      NewSimulatorTimerThread(  (wdt_data.InitialCount - wdt_data.PreTimeoutInterval) ),
                      m_state( NONE ) {

   memcpy( &m_wdt_rec, &rdr.RdrTypeUnion.WatchdogRec, sizeof( SaHpiWatchdogRecT ));
   memcpy( &m_wdt_data, &wdt_data, sizeof( SaHpiWatchdogT ));
}


/**
 * Destructor
 **/
NewSimulatorWatchdog::~NewSimulatorWatchdog() {

   Stop();
}


/**
 * A rdr structure is filled with the data
 * 
 * This method is called by method NewSimulatorRdr::Populate().
 * 
 * @param resource Address of resource structure
 * @param rdr Address of rdr structure
 * 
 * @return true
 **/
bool NewSimulatorWatchdog::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {

   if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
      return false;

   /// Watchdog record
   memcpy(&rdr.RdrTypeUnion.WatchdogRec, &m_wdt_rec, sizeof(SaHpiWatchdogRecT));
   

   return true;
}


/** 
 * Dump the Watchdog information
 * 
 * @param dump address of the log
 * 
 **/
void NewSimulatorWatchdog::Dump( NewSimulatorLog &dump ) const {
   char str[256];
   IdString().GetAscii( str, 256 );

   dump << "Watchdog: " << m_wdt_rec.WatchdogNum << " " << str << "\n";
   dump << "Oem:      " << m_wdt_rec.Oem << "\n";
   dump << "Watchdog data:\n";
   dump << "Log:                " << m_wdt_data.Log << "\n";
   dump << "Running:            " << m_wdt_data.Running << "\n";
   dump << "TimerUse:           " << m_wdt_data.TimerUse << "\n";
   dump << "TimerAction:        " << m_wdt_data.TimerAction << "\n";
   dump << "PretimerInterrupt:  " << m_wdt_data.PretimerInterrupt << "\n";
   dump << "PreTimeoutInterval: " << m_wdt_data.PreTimeoutInterval << "\n";
   dump << "TimerUseExpFlags:   " << m_wdt_data.TimerUseExpFlags << "\n";
   dump << "InitialCount:       " << m_wdt_data.InitialCount << "\n";
   dump << "PresentCount:       " << m_wdt_data.PresentCount << "\n";
   
}


/**
 * Check whether the watchdog timer is running and trigger proper action 
 * 
 * @return true if thread can exit, false if not
 **/
bool NewSimulatorWatchdog::TriggerAction() {
   
   stdlog << "DBG: CheckWatchdogTimer\n";
    
   if ( m_wdt_data.Running == SAHPI_FALSE )
      return true;
      
   if ( ! m_start.IsSet() )
      return true;
   
   // Ok, we have a running wdt
   cTime now( cTime::Now() );
   now -= m_start;
   
   if ( now.GetMsec() >= m_wdt_data.InitialCount ) {
   	  
   	  if ( m_state != PRETIMEOUT )
   	     TriggerAction( PRETIMEOUT );
      TriggerAction( TIMEOUT );
      
      stdlog << "DBG: WatchdogTimer expires.\n";
      
      return true;
   }
   
   if ( now.GetMsec() >= m_wdt_data.InitialCount - m_wdt_data.PreTimeoutInterval ) {
   	
   	  TriggerAction( PRETIMEOUT );
   	  
   	  return false;
   }
   
   m_wdt_data.PresentCount = m_wdt_data.InitialCount - now.GetMsec();

   return false;
}
 
/**
 * Trigger an action, like sending an event and setting the exp_mask
 * 
 * @param state watchdog state which should be triggered 
 **/
void NewSimulatorWatchdog::TriggerAction( WdtStateT state ) {
   SaHpiWatchdogActionEventT wdtaction;
   SaHpiSeverityT sev = SAHPI_MAJOR;
   
   if ( ( state == PRETIMEOUT ) &&
         ( m_state != PRETIMEOUT ) ) {
      cTime now( cTime::Now() );
      now -= m_start;
      m_state = PRETIMEOUT;
      wdtaction = SAHPI_WAE_TIMER_INT;
      sev = SAHPI_MAJOR;
      
      m_wdt_data.PresentCount = m_wdt_data.InitialCount - now.GetMsec();
      Reset( m_wdt_data.PreTimeoutInterval );
      
      if ( m_wdt_data.Log == SAHPI_TRUE ) {
         // The next is implementation specific, HPI-B, p. 154
         // An event is generated when the pre-timer expires, unless the 
         // pre-timer interrupt action is “None” and the pre-timer interval is zero, 
         // in which case it is implementation-dependent whether an event is generated.
         if (!(    (m_wdt_data.PretimerInterrupt == SAHPI_WPI_NONE)
                && (m_wdt_data.PreTimeoutInterval == 0)  ))
            SendEvent( wdtaction, sev );
            
      }
   }
  
   if ( state == TIMEOUT ) {

      m_wdt_data.Running = SAHPI_FALSE;
      m_wdt_data.PresentCount = 0;
      m_start.Clear();
      stdlog << "DBG: Stop TimerThread due to TimerAction\n";
      Stop();
      m_state = TIMEOUT;
      
      switch ( m_wdt_data.TimerAction ) {
         case SAHPI_WA_NO_ACTION:
            sev = SAHPI_INFORMATIONAL;
            wdtaction = SAHPI_WAE_NO_ACTION;
            break;
            
         case SAHPI_WA_RESET:
            sev = SAHPI_MAJOR;
            wdtaction = SAHPI_WAE_RESET;
            break;
            
         case SAHPI_WA_POWER_DOWN:
            sev = SAHPI_MAJOR;
            wdtaction = SAHPI_WAE_POWER_DOWN;
            break;
            
         case SAHPI_WA_POWER_CYCLE:
            sev = SAHPI_MAJOR;
            wdtaction = SAHPI_WAE_POWER_CYCLE;
            break;
            
         default:
            err("Invalid TimerAction is configured inside Watchdog");
            sev = SAHPI_INFORMATIONAL;
            wdtaction = SAHPI_WAE_NO_ACTION;
            break;
      }
      
      switch ( m_wdt_data.TimerUse ) {
      	  case SAHPI_WTU_NONE:
      	  case SAHPI_WTU_UNSPECIFIED:
      	     break;
      	     
          case SAHPI_WTU_BIOS_FRB2:
             m_wdt_data.TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_BIOS_FRB2;
             break;
             
          case SAHPI_WTU_BIOS_POST:
             m_wdt_data.TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_BIOS_POST;
             break;
             
          case SAHPI_WTU_OS_LOAD:
             m_wdt_data.TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_OS_LOAD;
             break;
             
          case SAHPI_WTU_SMS_OS:
             m_wdt_data.TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_SMS_OS;
             break;
             
          case SAHPI_WTU_OEM:
             m_wdt_data.TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_OEM;
             break;
             
          default:
             err("Invalid TimerUse is configured inside Watchdog");
             break;
      }
      stdlog << "DBG: Watchdog::SendEvent if allowed\n";
      if ( m_wdt_data.Log == SAHPI_TRUE )
         SendEvent( wdtaction, sev ); 
   }
}


/**
 * Send a watchdog event
 * 
 * @param wdtaction watchdog action event flag to be set
 * @param sev severity of event to be set 
 **/
void NewSimulatorWatchdog::SendEvent( SaHpiWatchdogActionEventT wdtaction, SaHpiSeverityT sev ) {

   NewSimulatorResource *res = Resource();
   if( !res ) {
      stdlog << "DBG: Watchdog::TriggerAction: No resource !\n";
      return;
   }
  
   oh_event *e = (oh_event *)g_malloc0( sizeof( struct oh_event ) );

   e->event.EventType = SAHPI_ET_WATCHDOG;

   SaHpiRptEntryT *rptentry = oh_get_resource_by_id( res->Domain()->GetHandler()->rptcache, res->ResourceId() );
   SaHpiRdrT *rdrentry = oh_get_rdr_by_id( res->Domain()->GetHandler()->rptcache, res->ResourceId(), m_record_id );

   if ( rptentry )
      e->resource = *rptentry;
   else
      e->resource.ResourceCapabilities = 0;

   if ( rdrentry )
      e->rdrs = g_slist_append(e->rdrs, g_memdup(rdrentry, sizeof(SaHpiRdrT)));
   else
      e->rdrs = NULL;

   // hpi events
   e->event.Source    = res->ResourceId();
   e->event.EventType = SAHPI_ET_WATCHDOG;
   e->event.Severity  = sev;
  
   oh_gettimeofday(&e->event.Timestamp);
  
   SaHpiWatchdogEventT *wdte = &e->event.EventDataUnion.WatchdogEvent;
   wdte->WatchdogNum            = m_wdt_rec.WatchdogNum;
   wdte->WatchdogAction         = wdtaction;
   wdte->WatchdogPreTimerAction = m_wdt_data.PretimerInterrupt;
   wdte->WatchdogUse            = m_wdt_data.TimerUse;
  
   stdlog << "DBG: NewSimWatchdog::SendEvent Wdt for resource " << res->ResourceId() << "\n";
   res->Domain()->AddHpiEvent( e );

}
 
// Official HPI functions
/** 
 * HPI function saHpiWatchdogTimerGet
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @param watchdog address of watchdog record to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorWatchdog::GetWatchdogInfo( SaHpiWatchdogT &watchdog ) {
   
   if ( &watchdog == NULL)
      return SA_ERR_HPI_INVALID_PARAMS;
   
   memcpy( &watchdog, &m_wdt_data, sizeof( SaHpiWatchdogT ));   

   if ( m_start.IsSet() ) {
      cTime now( cTime::Now() );
      now -= m_start;
      
      if ( m_wdt_data.InitialCount < now.GetMsec() ) {
         watchdog.PresentCount = 0;
      } else {
      	 watchdog.PresentCount = m_wdt_data.InitialCount - now.GetMsec();
      }
      
      stdlog << "DBG: GetWatchdogInfo PresentCount == " << watchdog.PresentCount << "\n";
   }
   
   stdlog << "DBG: Call of GetWatchdogInfo: num " << m_wdt_rec.WatchdogNum << "\n";

   return SA_OK;
}


/** 
 * HPI function saHpiWatchdogTimerSet
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param watchdog address of watchdog record to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorWatchdog::SetWatchdogInfo( SaHpiWatchdogT &watchdog ) {
   SaHpiWatchdogExpFlagsT origFlags; 
   
   if ( &watchdog == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;

   if ( watchdog.PreTimeoutInterval > watchdog.InitialCount )
      return SA_ERR_HPI_INVALID_DATA;

   origFlags = m_wdt_data.TimerUseExpFlags;
   
   memcpy( &m_wdt_data, &watchdog, sizeof( SaHpiWatchdogT ));  

   if ( watchdog.Running == SAHPI_TRUE ) {
      if ( m_start.IsSet() ) {
         m_start = cTime::Now();
         Reset( m_wdt_data.InitialCount - m_wdt_data.PreTimeoutInterval );
         if ( !m_running )
            Start();

      } else {
      	 m_wdt_data.Running = SAHPI_FALSE;
      	 m_wdt_data.PresentCount = 0;
      }
      
   } else {
      m_start.Clear();
      Stop();
      m_wdt_data.PresentCount = 0;

   }
   
   // ClearFlags
   m_wdt_data.TimerUseExpFlags = origFlags & ~watchdog.TimerUseExpFlags;

   stdlog << "DBG: SetWatchdogInfo successfully: num " << m_wdt_rec.WatchdogNum << "\n";

   return SA_OK;
}


/** 
 * HPI function saHpiWatchdogTimerReset
 * 
 * See also the description of the function inside the specification or header file.
 * Starting or resetting a watchdog timer if it is allowed
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorWatchdog::ResetWatchdog() {

   if ( m_start.IsSet() ) {
      cTime now( cTime::Now() );
      now -= m_start;
      
      if ( now.GetMsec() > m_wdt_data.InitialCount - m_wdt_data.PreTimeoutInterval ) {
         
         stdlog << "DBG: ResetWatchdog not allowed: num " << m_wdt_rec.WatchdogNum << ":\n";
         stdlog << "DBG: Time expire in ms: " << now.GetMsec() << " > " 
                << (m_wdt_data.InitialCount - m_wdt_data.PreTimeoutInterval) << "\n";

         return SA_ERR_HPI_INVALID_REQUEST;
      }
      
      // Reset the Timer
      Reset( m_wdt_data.InitialCount - m_wdt_data.PreTimeoutInterval );
      m_start = cTime::Now();

   } else {
   	
      m_start = cTime::Now();
      // Reset the Timer
      Reset( m_wdt_data.InitialCount - m_wdt_data.PreTimeoutInterval );
      if (!m_running)
         Start();

   }

   m_wdt_data.Running = SAHPI_TRUE;  
   Domain()->SetRunningWdt( true );
   stdlog << "DBG: ResetWatchdog successfully: num " << m_wdt_rec.WatchdogNum << "\n";
   
   return SA_OK;
}
