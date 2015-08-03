/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2012
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTWATCHDOGLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <stddef.h>
#include <string>

#include <oh_utils.h>

#include "log.h"
#include "structs.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiEventLogInfoT MakeDefaultInfo()
{
    SaHpiEventLogInfoT info;

    info.Entries           = 0;
    info.Size              = 100;
    info.UserEventMaxSize  = 100;
    oh_gettimeofday( &info.UpdateTimestamp );
    oh_gettimeofday( &info.CurrentTime );
    info.Enabled           = SAHPI_TRUE;
    info.OverflowFlag      = SAHPI_FALSE;
    info.OverflowResetable = SAHPI_TRUE;
    info.OverflowAction    = SAHPI_EL_OVERFLOW_OVERWRITE;

    return info;
}

static SaHpiEventLogCapabilitiesT MakeDefaultCaps()
{
    return SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD |
           SAHPI_EVTLOG_CAPABILITY_CLEAR |
           SAHPI_EVTLOG_CAPABILITY_TIME_SET |
           SAHPI_EVTLOG_CAPABILITY_STATE_SET |
           SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET;
}


/**************************************************************
 * class cLog
 *************************************************************/
const std::string cLog::classname( "log" );
static const std::string size_name( "Info.Size" );

cLog::cLog()
    : cObject( classname ),
      m_info( MakeDefaultInfo() ),
      m_caps( MakeDefaultCaps() ),
      m_delta( 0LL ),
      m_next_eid( 1 )
{
    // empty
}

cLog::~cLog()
{
    // empty
}

void cLog::AddEntry( SaHpiEventTypeT type,
                     const SaHpiEventUnionT& data,
                     SaHpiSeverityT severity,
                     const SaHpiRdrT * rdr,
                     const SaHpiRptEntryT * rpte )
{
    if ( m_info.Enabled == SAHPI_FALSE ) {
        return;
    }

    SaHpiEventT event;
    event.Source         = ( rpte == 0 ) ? SAHPI_UNSPECIFIED_RESOURCE_ID : rpte->ResourceId;
    event.EventType      = type;
    oh_gettimeofday( &event.Timestamp );
    event.Severity       = severity;
    event.EventDataUnion = data;

    AddEntry( event, rdr, rpte );
}


// HPI interface
SaErrorT cLog::GetInfo( SaHpiEventLogInfoT& info )
{
    SyncInfo();
    info = m_info;

    return SA_OK;
}

SaErrorT cLog::GetCapabilities( SaHpiEventLogCapabilitiesT& caps ) const
{
    caps = m_caps;

    return SA_OK;
}

SaErrorT cLog::SetTime( SaHpiTimeT t )
{
    if ( ( m_caps && SAHPI_EVTLOG_CAPABILITY_TIME_SET ) == 0 ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    if ( t == SAHPI_TIME_UNSPECIFIED ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    SaHpiTimeT current;
    oh_gettimeofday( &current );
    m_delta = t - current;

    Update();

    return SA_OK;
}

SaErrorT cLog::AddEntry( const SaHpiEventT& event )
{
    if ( ( m_caps && SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD ) == 0 ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    if ( event.Source != SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if ( event.EventType != SAHPI_ET_USER ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if ( event.EventDataUnion.UserEvent.UserEventData.DataLength > m_info.UserEventMaxSize ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    bool rc = AddEntry( event, 0, 0 );
    if ( !rc ) {
        return SA_ERR_HPI_OUT_OF_SPACE;
    }

    return SA_OK;
}

SaErrorT cLog::GetEntry( SaHpiEventLogEntryIdT eid,
                   SaHpiEventLogEntryIdT& prev_eid,
                   SaHpiEventLogEntryIdT& next_eid,
                   SaHpiEventLogEntryT& entry,
                   SaHpiRdrT& rdr,
                   SaHpiRptEntryT& rpte )
{
    if ( m_entries.empty() ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    if ( eid == SAHPI_NO_MORE_ENTRIES ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    Entries::const_iterator iter;
    Entries::const_iterator begin = m_entries.begin();
    Entries::const_iterator end = m_entries.end();
    if ( eid == SAHPI_OLDEST_ENTRY ) {
        iter = begin;
    } else if ( eid == SAHPI_NEWEST_ENTRY ) {
        iter = end;
        Entries::const_iterator iter2;
        for ( iter2 = begin; iter2 != end; ++iter2 ) {
            iter = iter2;
        }
    } else {
        iter = end;
        Entries::const_iterator iter2;
        for ( iter2 = begin; iter2 != end; ++iter2 ) {
            if ( iter2->entry.EntryId == eid ) {
                iter = iter2;
                break;
            }
        }
    }
    if ( iter == end ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    prev_eid = SAHPI_NO_MORE_ENTRIES;
    Entries::const_iterator prev_iter = iter;
    --prev_iter;
    if ( prev_iter != end ) {
        prev_eid = prev_iter->entry.EntryId;
    }

    next_eid = SAHPI_NO_MORE_ENTRIES;
    Entries::const_iterator next_iter = iter;
    ++next_iter;
    if ( next_iter != end ) {
        next_eid = next_iter->entry.EntryId;
    }

    entry = iter->entry;
    rdr   = iter->rdr;
    rpte  = iter->rpte;

    return SA_OK;
}

SaErrorT cLog::Clear()
{
    if ( ( m_caps && SAHPI_EVTLOG_CAPABILITY_CLEAR ) == 0 ) {
        return SA_ERR_HPI_INVALID_CMD;
    }

    m_entries.clear();
    SyncInfo();
    Update();

    return SA_OK;
}

SaErrorT cLog::SetState( SaHpiBoolT enable )
{
    if ( ( m_caps && SAHPI_EVTLOG_CAPABILITY_STATE_SET ) == 0 ) {
        return SA_ERR_HPI_INVALID_CMD;
    }

    m_info.Enabled = enable;
    Update();

    return SA_OK;
}

SaErrorT cLog::ResetOverflow()
{
    if ( m_info.OverflowResetable == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    if ( ( m_caps && SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET ) == 0 ) {
        return SA_ERR_HPI_INVALID_CMD;
    }

    m_info.OverflowFlag = SAHPI_FALSE;
    Update();

    return SA_OK;
}


// cObject virtual functions
void cLog::GetVars( cVars& vars )
{
    cObject::GetVars( vars );

    SyncInfo();
    Structs::GetVars( m_info, vars );

    vars << "Capabilities"
         << dtSaHpiEventLogCapabilitiesT
         << DATA( m_caps )
         << VAR_END();
}

void cLog::AfterVarSet( const std::string& var_name )
{
    cObject::AfterVarSet( var_name );

    if ( var_name == size_name ) {
        if ( m_info.Size == 0 ) {
            m_entries.clear();
        } if ( m_entries.size() >= m_info.Size ) {
            if ( m_info.OverflowAction == SAHPI_EL_OVERFLOW_DROP ) {
                m_entries.resize( m_info.Size );
            } else {
                while ( m_entries.size() > m_info.Size ) {
                    m_entries.pop_front();
                }
            }
        }
    }
}


void cLog::Update()
{
    oh_gettimeofday( &m_info.UpdateTimestamp );
    m_info.UpdateTimestamp += m_delta;
}

void cLog::SyncInfo()
{
    m_info.Entries = m_entries.size();
    oh_gettimeofday( &m_info.CurrentTime );
    m_info.CurrentTime += m_delta;
    if ( ( m_info.Entries == 0 ) || ( m_info.Entries < m_info.Size ) ) {
        m_info.OverflowFlag = SAHPI_FALSE;
    }
}

bool cLog::AddEntry( const SaHpiEventT& event,
                     const SaHpiRdrT * rdr,
                     const SaHpiRptEntryT * rpte )
{
    if ( m_entries.size() >= m_info.Size ) {
        if ( m_info.OverflowAction == SAHPI_EL_OVERFLOW_DROP ) {
            return false;
        }
        if ( m_info.Size == 0 ) {
            return false;
        }
        const size_t need = m_info.Size - 1;
        while ( m_entries.size() > need ) {
            m_entries.pop_front();
        }
    }

    Entry entry;
    entry.entry.EntryId             = m_next_eid;
    entry.entry.Event               = event;
    oh_gettimeofday( &entry.entry.Timestamp );
    entry.entry.Timestamp          += m_delta;
    entry.rdr.RdrType               = SAHPI_NO_RECORD;
    if ( rdr ) {
        entry.rdr                   = *rdr;
    }
    entry.rpte.ResourceId           = SAHPI_UNSPECIFIED_RESOURCE_ID;
    entry.rpte.ResourceCapabilities = 0;
    if ( rpte ) {
        entry.rpte                  = *rpte;
    }

    m_entries.push_back( entry );

    ++m_next_eid;

    if ( m_entries.size() == m_info.Size ) {
        m_info.OverflowFlag = SAHPI_TRUE;
    }

    Update();

    return true;
}



}; // namespace TA

