/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <map>

#include <glib.h>

#include <SaHpi.h>

#include <oh_error.h>
#include <oh_utils.h>

#include "codec.h"
#include "handler.h"
#include "resource.h"
#include "timers.h"
#include "vars.h"

#include "sahpi_wrappers.h"


namespace TA {


/**************************************************************
 * Helpers
 *************************************************************/


/**************************************************************
 * class cHandler
 *************************************************************/
cHandler::cHandler( unsigned int id,
                    unsigned short port,
                    oh_evt_queue& eventq )

    : cObject( "root" ),
      cConsole( *this, port, *this ),
      m_id( id ),
      m_eventq( eventq ),
      m_ai_timeout( SAHPI_TIMEOUT_IMMEDIATE )
{
    wrap_g_static_mutex_init( &m_lock );
}

cHandler::~cHandler()
{
    Resources::const_iterator iter = m_resources.begin();
    Resources::const_iterator end  = m_resources.end();
    for ( ; iter != end; ++iter ) {
        cResource * r = iter->second;
        delete r;
    }
    m_resources.clear();

    wrap_g_static_mutex_free_clear( &m_lock );
}

bool cHandler::Init()
{
    bool rc;

    rc = cConsole::Init();
    if ( !rc ) {
        CRIT( "cannot initialize console" );
        return false;
    }
    rc = cTimers::Start();
    if ( !rc ) {
        CRIT( "cannot start timers" );
        return false;
    }

    return true;
}

void cHandler::Lock()
{
    wrap_g_static_mutex_lock( &m_lock );

}

void cHandler::Unlock()
{
    wrap_g_static_mutex_unlock( &m_lock );
}

cResource * cHandler::GetResource( SaHpiResourceIdT rid ) const
{
    Resources::const_iterator iter = m_resources.find( rid );
    if ( iter != m_resources.end() ) {
        cResource * r = iter->second;
        return r;
    }

    return 0;
}

SaErrorT cHandler::RemoveFailedResource( SaHpiResourceIdT rid )
{
    cResource * r = GetResource( rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    if ( !r->IsFailed() ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_resources.erase( rid );
    delete r;

    return SA_OK;
}

SaHpiTimeoutT cHandler::GetAutoInsertTimeout() const
{
    return m_ai_timeout;
}

SaErrorT cHandler::SetAutoInsertTimeout( SaHpiTimeoutT t )
{
    m_ai_timeout = t;

    return SA_OK;
}

void cHandler::PostEvent( SaHpiEventTypeT type,
                          const SaHpiEventUnionT& data,
                          SaHpiSeverityT severity,
                          const cResource * r,
                          const InstrumentList& updates,
                          const InstrumentList& removals ) const

{
    if ( !IsVisible() ) {
        return;
    }

    struct oh_event * e     = oh_new_event();
    e->hid                  = m_id;
    e->event.Source         = r ? r->GetResourceId() :
                                  SAHPI_UNSPECIFIED_RESOURCE_ID;
    e->event.EventType      = type;
    oh_gettimeofday( &e->event.Timestamp );
    e->event.Severity       = severity;
    e->event.EventDataUnion = data;
    e->resource.ResourceId  = SAHPI_UNSPECIFIED_RESOURCE_ID;
    e->resource.ResourceCapabilities = 0;

    if ( r ) {
        e->resource = r->GetRptEntry();
    }

    InstrumentList::const_iterator i, end;
    for ( i = updates.begin(), end = updates.end(); i != end; ++i ) {
        const SaHpiRdrT& rdr = (*i)->GetRdr();
        void * copy = g_memdup( &rdr, sizeof(rdr) );
        e->rdrs = g_slist_append( e->rdrs, copy );
    }
    for ( i = removals.begin(), end  = removals.end(); i != end; ++i ) {
        const SaHpiRdrT& rdr = (*i)->GetRdr();
        void * copy = g_memdup( &rdr, sizeof(rdr) );
        e->rdrs_to_remove = g_slist_append( e->rdrs_to_remove, copy );
    }

    oh_evt_queue_push( &m_eventq, e );
}

void cHandler::GetNewNames( cObject::NewNames& names ) const
{
    cObject::GetNewNames( names );
    names.push_back( "Any Valid Entity Path" );
}

bool cHandler::CreateChild( const std::string& name )
{
    bool rc;

    rc = cObject::CreateChild( name );
    if ( rc ) {
        return true;
    }

    SaHpiEntityPathT ep;
    rc = DisassembleResourceObjectName( name, ep );
    if ( !rc ) {
        return false;
    }

    cResource * r = new cResource( *this, ep );
    m_resources[r->GetResourceId()] = r;

    return true;
}

bool cHandler::RemoveChild( const std::string& name )
{
    bool rc;

    rc = cObject::RemoveChild( name );
    if ( rc ) {
        return true;
    }

    cObject * obj = cObject::GetChild( name );
    if ( !obj ) {
        return false;
    }

    cResource * r = static_cast<cResource *>(obj);
    size_t n = m_resources.erase( r->GetResourceId() );
    if ( n == 0 ) {
        return false;
    }

    delete r;

    return true;
}


void cHandler::GetChildren( Children& children ) const
{
    cObject::GetChildren( children );

    Resources::const_iterator iter = m_resources.begin();
    Resources::const_iterator end  = m_resources.end();
    for ( ; iter != end; ++iter ) {
        cResource * r = iter->second;
        children.push_back( r );
    }
}

void cHandler::GetVars( cVars& vars )
{
    cObject::GetVars( vars );

    vars << "AutoInsertTimeout"
         << dtSaHpiTimeoutT
         << DATA( m_ai_timeout )
         << VAR_END();
}


}; // namespace TA

