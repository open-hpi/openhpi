/** 
 * @file    new_sim_hotswap.cpp
 *
 * The file includes a class for hotswap behaviour:\n
 * NewSimulatorHotSwap
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
 
#include "new_sim_hotswap.h"
#include "new_sim_utils.h"
#include "new_sim_domain.h"
#include "new_sim_timer_thread.h"

/**
 * Constructor
 **/
NewSimulatorHotSwap::NewSimulatorHotSwap( NewSimulatorResource *res )
                    : NewSimulatorTimerThread( 0 ),
                      m_insert_time( SAHPI_TIMEOUT_IMMEDIATE ),
                      m_extract_time( SAHPI_TIMEOUT_IMMEDIATE ),
                      m_running( false ),
                      m_state( SAHPI_HS_STATE_NOT_PRESENT ),
                      m_res( res ) {

}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorHotSwap::NewSimulatorHotSwap( NewSimulatorResource *res,
                      SaHpiTimeoutT insertTime,
                      SaHpiTimeoutT extractTime, 
                      SaHpiHsStateT startState )
                    : NewSimulatorTimerThread( 0 ),
                      m_insert_time( insertTime ),
                      m_extract_time( extractTime ),
                      m_running( false ),
                      m_state( startState ),
                      m_res( res ) {

}


/**
 * Destructor
 **/
NewSimulatorHotSwap::~NewSimulatorHotSwap() {

   Stop();
}


/** 
 * Dump hotswap information
 * 
 * @param dump address of the log
 * 
 **/
void NewSimulatorHotSwap::Dump( NewSimulatorLog &dump ) const {

   dump << "HotSwap data: \n";
   dump << "InsertionTimeout:   " << (long int) m_insert_time << "\n";
   dump << "ExtractionTimeout:  " << (long int) m_extract_time << "\n";
   dump << "State:              " << m_state << "\n";
}


/**
 * Set both timeout values 
 **/
void NewSimulatorHotSwap::SetTimeouts( SaHpiTimeoutT insert, SaHpiTimeoutT extract ) {
   m_insert_time = insert;
   m_extract_time = extract;
}


/**
 * Check if the hotswap policy is still valid and send event 
 * 
 * @return true if thread can exit, false if not
 **/
bool NewSimulatorHotSwap::TriggerAction() {
   SaHpiHsCauseOfStateChangeT cause    = SAHPI_HS_CAUSE_AUTO_POLICY;
   SaHpiSeverityT             severity = SAHPI_INFORMATIONAL;

   stdlog << "DBG: CheckHotSwapTimer\n";
    
   if ( m_running == SAHPI_FALSE )
      return true;
      
   if ( ! m_start.IsSet() )
      return true;
   
   // Ok, we have a running hotswap policy
   cTime now( cTime::Now() );
   now -= m_start;
   
   if ( m_state == SAHPI_HS_STATE_INSERTION_PENDING ) {
      if ( now.GetMsec() >= m_insert_time / 1000000LL ) {
      	 
   	     stdlog << "DBG: HotSwapTimer expires for Insertion.\n";
   	     SendEvent( SAHPI_HS_STATE_ACTIVE, SAHPI_HS_STATE_INSERTION_PENDING, 
   	                cause, severity );
         m_state = SAHPI_HS_STATE_ACTIVE;
         m_running = false;
         m_start.Clear();
         
         return true;
      }
   }
   
   if ( m_state == SAHPI_HS_STATE_EXTRACTION_PENDING ) {
      if ( now.GetMsec() >= m_extract_time / 1000000LL ) {
      	 
   	     stdlog << "DBG: HotSwapTimer expires for Extraction.\n";
   	     SendEvent( SAHPI_HS_STATE_INACTIVE, SAHPI_HS_STATE_EXTRACTION_PENDING,
   	                cause, severity ); 
         m_state = SAHPI_HS_STATE_INACTIVE;
         m_running = false;
         m_start.Clear();
         
         return true;
      }
   }

   err(" Timer expires but now action was defined -> Stop Timer. \n");
 
   return true;
}
 
/**
 * Check if the hotswap policy timeouts and start actions 
 * 
 * @return true if thread can exit, false if not
 **/
SaErrorT NewSimulatorHotSwap::TriggerTransition( SaHpiHsStateT state ) {
   SaHpiTimeoutT timeout;
   SaHpiHsCauseOfStateChangeT cause    = SAHPI_HS_CAUSE_AUTO_POLICY;
   SaHpiSeverityT             severity = SAHPI_INFORMATIONAL;
   
   switch ( state ) {
      case SAHPI_HS_STATE_ACTIVE:
         // First get the latest Insertion time value
         m_insert_time = m_res->Domain()->InsertTimeout();
         timeout = m_insert_time;
         break;

      case SAHPI_HS_STATE_INACTIVE:
         timeout = m_extract_time;
         break;
         
      default:
         err( "Invalid state for NewSimulatorHotSwap::TriggerTransition.");
         return SA_ERR_HPI_INTERNAL_ERROR;
   }
   
   if ( timeout == SAHPI_TIMEOUT_IMMEDIATE ) {
      stdlog << "DBG: Transition happens immediatly due to SAHPI_TIMEOUT_IMMEDIATE.\n";
   	  SendEvent( state, m_state, cause, severity );
   	  m_state = state;   
      
   } else if ( timeout == SAHPI_TIMEOUT_BLOCK ) {
      stdlog << "DBG: Transition is blocked by timeout value SAHPI_TIMEOUT_BLOCK.\n";
      
   } else if ( timeout > 0 ) {
   	  stdlog << "DBG: Transition will happen after " << (long int) timeout << " ms.\n";
   	  Reset( timeout / 1000000LL );
   	  m_start = cTime::Now();
   	  m_running = true;
   	  Start();
   	  
   } else {
      err( "Invalid timeout value inside NewSimulatorHotSwap::TriggerTransition.");
      return SA_ERR_HPI_INTERNAL_ERROR;
   }
   
   return SA_OK;
   
}


/**
 * Start a resource from NOT_PRESENT
 **/
SaErrorT NewSimulatorHotSwap::StartResource(oh_event *e) {
	
   SaErrorT                   rv       = SA_OK;
   SaHpiHsCauseOfStateChangeT cause    = SAHPI_HS_CAUSE_AUTO_POLICY;
   SaHpiSeverityT             severity = SAHPI_INFORMATIONAL;

   if ( m_res->ResourceCapabilities() & SAHPI_CAPABILITY_FRU ) {
      e->event.EventType = SAHPI_ET_HOTSWAP;
      
      if ( m_res->ResourceCapabilities() & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) {

         e->event.EventDataUnion.HotSwapEvent.HotSwapState = SAHPI_HS_STATE_INACTIVE;
         e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = SAHPI_HS_STATE_NOT_PRESENT;
      	 e->event.EventDataUnion.HotSwapEvent.CauseOfStateChange = cause;
      	 
         m_state = SAHPI_HS_STATE_INACTIVE;
      
      } else {

         e->event.EventDataUnion.HotSwapEvent.HotSwapState = SAHPI_HS_STATE_ACTIVE;
         e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = SAHPI_HS_STATE_NOT_PRESENT;
         e->event.EventDataUnion.HotSwapEvent.CauseOfStateChange = cause;
         
         m_state = SAHPI_HS_STATE_ACTIVE;
      
      }
      
   } else {
   	  // Send a Resource event
   	  e->event.EventType = SAHPI_ET_RESOURCE;
      e->event.EventDataUnion.ResourceEvent.ResourceEventType = SAHPI_RESE_RESOURCE_ADDED;
      m_state = SAHPI_HS_STATE_ACTIVE;
      
   }

   e->event.Source = e->resource.ResourceId;
   e->event.Severity = severity;
   oh_gettimeofday(&e->event.Timestamp);

   m_res->Domain()->AddHpiEvent( e );

   if ( rv == SA_OK )
      stdlog << "DBG: HotSwap::StartResource successfully.\n";

   return rv; 
}

/**
 * Send a HotSwap event
 * 
 * @param newState new HotSwap state
 * @param prevState previous HotSwap state
 * @param cause cause for the state transition
 * @param severity severity of event to be set 
 **/
void NewSimulatorHotSwap::SendEvent( SaHpiHsStateT newState, 
                                      SaHpiHsStateT prevState, 
                                      SaHpiHsCauseOfStateChangeT cause,
                                      SaHpiSeverityT severity ) {

   NewSimulatorResource *res = m_res;
   if( !res ) {
      stdlog << "DBG: HotSwap::SendEvent: No resource !\n";
      return;
   }
   oh_event *e = (oh_event *)g_malloc0( sizeof( struct oh_event ) );

   e->event.EventType = SAHPI_ET_HOTSWAP;

   SaHpiRptEntryT *rptentry = oh_get_resource_by_id( res->Domain()->GetHandler()->rptcache, res->ResourceId() );

   if ( rptentry )
      e->resource = *rptentry;
   else
      e->resource.ResourceCapabilities = 0;

   // hpi events
   e->event.Source    = res->ResourceId();
   e->event.EventType = SAHPI_ET_HOTSWAP;
   e->event.Severity  = severity;
  
   oh_gettimeofday(&e->event.Timestamp);
  
   SaHpiHotSwapEventT &he   = e->event.EventDataUnion.HotSwapEvent;
   he.HotSwapState         = newState;
   he.PreviousHotSwapState = prevState;
   he.CauseOfStateChange   = cause;
  
   stdlog << "DBG: NewSimHotSwap::Send hotswap event for resource " << res->ResourceId() << "\n";
   res->Domain()->AddHpiEvent( e );

}



// Official HPI functions
/** 
 * HPI function saHpiHotSwapPolicyCancel
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorHotSwap::CancelPolicy() {
   
   if ( !( (m_state == SAHPI_HS_STATE_INSERTION_PENDING) ||
           (m_state == SAHPI_HS_STATE_EXTRACTION_PENDING) ))
      return SA_ERR_HPI_INVALID_REQUEST;
      
   if (!(m_res->ResourceCapabilities() & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
      return SA_ERR_HPI_CAPABILITY; 

   if ( m_running )
      Stop();
   m_running = false;
   m_start.Clear();
   
   return SA_OK;
}

/** 
 * HPI function saHpiHotSwapStateGet
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @param state address of state to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorHotSwap::GetState( SaHpiHsStateT &state ) {
   
   if ( &state == NULL)
      return SA_ERR_HPI_INVALID_PARAMS;
      
   if (!(m_res->ResourceCapabilities() & SAHPI_CAPABILITY_FRU))
      return SA_ERR_HPI_CAPABILITY; 

   state = m_state; 
   
   return SA_OK;
}

/** 
 * HPI function saHpiAutoExtractTimeoutSet
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @param timeout timeout value to be set
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorHotSwap::SetExtractTimeout( SaHpiTimeoutT timeout ) {

   if ( ( timeout != SAHPI_TIMEOUT_BLOCK ) &&
        ( timeout != SAHPI_TIMEOUT_IMMEDIATE ) &&
        ( timeout <= 0 ) )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   if (!(m_res->ResourceCapabilities() & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
      return SA_ERR_HPI_CAPABILITY;
      
   if ( m_res->HotSwapCapabilities() & SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY )
      return SA_ERR_HPI_READ_ONLY;
      
   m_extract_time = timeout;
   
   return SA_OK;
   
}


/** 
 * HPI function saHpiAutoExtractTimeoutGet
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @param timeout address of timeout value to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorHotSwap::GetExtractTimeout( SaHpiTimeoutT &timeout ) {

   if ( &timeout == NULL ) 
      return SA_ERR_HPI_INVALID_PARAMS;
   
   if (!(m_res->ResourceCapabilities() & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
      return SA_ERR_HPI_CAPABILITY;
      
   timeout = m_extract_time;
   
   return SA_OK;
   
}


/** 
 * HPI function saHpiHotSwapActionRequest
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @param action Requested action
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorHotSwap::ActionRequest( SaHpiHsActionT action ) {
	
   SaHpiHsCauseOfStateChangeT cause    = SAHPI_HS_CAUSE_EXT_SOFTWARE;
   SaHpiSeverityT             severity = SAHPI_INFORMATIONAL;
   
   switch( action ) {
   
      case SAHPI_HS_ACTION_INSERTION:
         if ( m_state != SAHPI_HS_STATE_INACTIVE )
            return SA_ERR_HPI_INVALID_REQUEST;

         SendEvent( SAHPI_HS_STATE_INSERTION_PENDING, SAHPI_HS_STATE_INACTIVE,
                    cause, severity );
                    
         m_state = SAHPI_HS_STATE_INSERTION_PENDING;
         TriggerTransition( SAHPI_HS_STATE_ACTIVE );
         break;
      
      case SAHPI_HS_ACTION_EXTRACTION:
         if ( m_state != SAHPI_HS_STATE_ACTIVE )
            return SA_ERR_HPI_INVALID_REQUEST;

         SendEvent( SAHPI_HS_STATE_EXTRACTION_PENDING, SAHPI_HS_STATE_ACTIVE,
                    cause, severity );
                            
         m_state = SAHPI_HS_STATE_EXTRACTION_PENDING;
         TriggerTransition( SAHPI_HS_STATE_INACTIVE );
         break;
      
      default:
         return SA_ERR_HPI_INVALID_PARAMS;
   }
   
   return SA_OK;
}


/** 
 * HPI function saHpiHotSwapInactiveSet
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorHotSwap::SetInactive() {
   SaHpiHsCauseOfStateChangeT cause    = SAHPI_HS_CAUSE_EXT_SOFTWARE;
   SaHpiSeverityT             severity = SAHPI_INFORMATIONAL;

   // Stop Timer if it is running
   if ( m_running )
      Stop();
   m_running = false;
   m_start.Clear();
   
   switch ( m_state ) {
      case  SAHPI_HS_STATE_INSERTION_PENDING:
         SendEvent( SAHPI_HS_STATE_INACTIVE, SAHPI_HS_STATE_INSERTION_PENDING, 
   	                cause, severity );
   	     
         break;
      
      case SAHPI_HS_STATE_EXTRACTION_PENDING:
         SendEvent( SAHPI_HS_STATE_INACTIVE, SAHPI_HS_STATE_EXTRACTION_PENDING,
                    cause, severity );
         break;
      
      default:
         return SA_ERR_HPI_INVALID_REQUEST;
   }
   
   m_state = SAHPI_HS_STATE_INACTIVE;
   
   return SA_OK;
}


/** 
 * HPI function saHpiHotSwapActiveSet
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values and show the remaining time if the timer was started.
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorHotSwap::SetActive() {
   SaHpiHsCauseOfStateChangeT cause    = SAHPI_HS_CAUSE_EXT_SOFTWARE;
   SaHpiSeverityT             severity = SAHPI_INFORMATIONAL;
   
   // Stop Timer if it is running
   if ( m_running )
      Stop();
   m_running = false;
   m_start.Clear();
     
   switch ( m_state ) {
      case  SAHPI_HS_STATE_INSERTION_PENDING:
         SendEvent( SAHPI_HS_STATE_ACTIVE, SAHPI_HS_STATE_INSERTION_PENDING,
                    cause, severity ); 
      break;
      
      case SAHPI_HS_STATE_EXTRACTION_PENDING:
         SendEvent( SAHPI_HS_STATE_ACTIVE, SAHPI_HS_STATE_EXTRACTION_PENDING,
                    cause, severity );         
      break;
      
      default:
         return SA_ERR_HPI_INVALID_REQUEST;
   }
   
   m_state = SAHPI_HS_STATE_ACTIVE;
   
   return SA_OK;
}

