/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTRESOURCELITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <string.h>

#include <SaHpi.h>

#include "codec.h"
#include "handler.h"
#include "log.h"
#include "resource.h"
#include "structs.h"
#include "timers.h"
#include "utils.h"
#include "vars.h"


#include <oh_error.h>
namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static void MakeDefaultRptEntry( const SaHpiEntityPathT& ep,
                                 SaHpiRptEntryT& rpte )
{
    static const SaHpiGuidT guid = { 0xFB, 0x2B, 0x5D, 0xD5, 0x4E, 0x7D, 0x49, 0xF5,
                                     0x93, 0x97, 0xC2, 0xFE, 0xC2, 0x1B, 0x40, 0x10 };
    SaHpiEntityPathT ep2 = ep;
    SaHpiResourceIdT rid = oh_uid_from_entity_path( &ep2 );

    //rpte.EntryId                = SAHPI_ENTRY_UNSPECIFIED;
    rpte.EntryId                = rid; // NB! This is OpenHPI-specific
    rpte.ResourceId             = rid;

    SaHpiResourceInfoT& ri = rpte.ResourceInfo;
    ri.ResourceRev              = 0;
    ri.SpecificVer              = 0;
    ri.DeviceSupport            = 0;
    ri.ManufacturerId           = 0;
    ri.ProductId                = 0;
    ri.FirmwareMajorRev         = 42;
    ri.FirmwareMinorRev         = 43;
    ri.AuxFirmwareRev           = 44;

    memcpy( &ri.Guid, &guid, sizeof(guid) );
    // just a trick to make unique GUID
    memcpy( &ri.Guid, &rid, sizeof(rid) );

    rpte.ResourceEntity         = ep;
    rpte.ResourceCapabilities   = SAHPI_CAPABILITY_RESOURCE |
                                  SAHPI_CAPABILITY_FRU |
                                  SAHPI_CAPABILITY_MANAGED_HOTSWAP |
                                  SAHPI_CAPABILITY_POWER |
                                  SAHPI_CAPABILITY_RESET |
                                  SAHPI_CAPABILITY_RDR;
    rpte.HotSwapCapabilities    = SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED;
    rpte.ResourceSeverity       = SAHPI_INFORMATIONAL;
    rpte.ResourceFailed         = SAHPI_FALSE;
    FormatHpiTextBuffer( rpte.ResourceTag, "res-%u", (unsigned int)(rid) );
}


/**************************************************************
 * class cResource
 *************************************************************/
cResource::cResource( cHandler& handler,
                      const SaHpiEntityPathT& ep )

    : cObject( AssembleResourceObjectName( ep ), SAHPI_FALSE ),
      cInstruments( handler, *this ),
      m_handler( handler ),
      m_log( 0 )
{
    MakeDefaultRptEntry( ep, m_rpte );
    m_failed             = m_rpte.ResourceFailed;
    m_new_failed         = m_rpte.ResourceFailed;
    m_ae_timeout         = SAHPI_TIMEOUT_IMMEDIATE;
    m_prev_hs_state      = SAHPI_HS_STATE_NOT_PRESENT;
    m_hs_state           = SAHPI_HS_STATE_INACTIVE;
    m_new_hs_state       = SAHPI_HS_STATE_INACTIVE;
    m_hs_ind_state       = SAHPI_HS_INDICATOR_OFF;
    m_load_id.LoadNumber = SAHPI_LOAD_ID_DEFAULT;
    m_rst_state          = SAHPI_RESET_DEASSERT;
    m_pwr_state          = SAHPI_POWER_OFF;

    m_pwr_cycle_cnt      = 0;
}

cResource::~cResource()
{
    delete m_log;
    m_log = 0;
    m_handler.CancelTimer( this );
    SetVisible( false );
}

const SaHpiRptEntryT& cResource::GetRptEntry() const
{
    return m_rpte;
}

SaHpiResourceIdT cResource::GetResourceId() const
{
    return m_rpte.ResourceId;
}

const SaHpiEntityPathT& cResource::GetEntityPath() const
{
    return m_rpte.ResourceEntity;
}

bool cResource::IsFailed() const
{
    return m_failed != SAHPI_FALSE;
}

void cResource::UpdateCaps( SaHpiCapabilitiesT caps )
{
    m_rpte.ResourceCapabilities |= caps;
}

cLog * cResource::GetLog() const
{
    return m_log;
}

// HPI interface
SaErrorT cResource::SetTag( const SaHpiTextBufferT& tag )
{
    m_rpte.ResourceTag = tag;

    return SA_OK;
}

SaErrorT cResource::SetSeverity( SaHpiSeverityT sev )
{
    m_rpte.ResourceSeverity = sev;

    return SA_OK;
}

SaErrorT cResource::CancelHsPolicy( const SaHpiTimeoutT& timeout )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }
    switch ( m_hs_state ) {
        case SAHPI_HS_STATE_INSERTION_PENDING:
        case SAHPI_HS_STATE_EXTRACTION_PENDING:
            break;
        default:
            return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_handler.CancelTimer( this );

    return SA_OK;
}

SaErrorT cResource::GetAutoExtractTimeout( SaHpiTimeoutT& timeout ) const
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    timeout = m_ae_timeout;

    return SA_OK;
}

SaErrorT cResource::SetAutoExtractTimeout( const SaHpiTimeoutT& timeout )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    const SaHpiHsCapabilitiesT hs_caps = m_rpte.HotSwapCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }
    if ( ( hs_caps & SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY ) != 0 ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    m_ae_timeout = timeout;

    return SA_OK;
}

SaErrorT cResource::GetHsState( SaHpiHsStateT& state ) const
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_FRU ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    state = m_hs_state;

    return SA_OK;
}

SaErrorT cResource::SetHsState( SaHpiHsStateT state )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    switch ( m_hs_state ) {
        case SAHPI_HS_STATE_INSERTION_PENDING:
        case SAHPI_HS_STATE_EXTRACTION_PENDING:
            break;
        default:
            return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_handler.CancelTimer( this );
    m_new_hs_state = state;
    CommitChanges();

    return SA_OK;
}

SaErrorT cResource::RequestHsAction( SaHpiHsActionT action )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }


    SaHpiTimeoutT ait, aet, t;
    GetTimeouts( ait, aet );

    if ( m_hs_state == SAHPI_HS_STATE_INACTIVE ) {
        if ( action != SAHPI_HS_ACTION_INSERTION ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
        m_new_hs_state = SAHPI_HS_STATE_INSERTION_PENDING;
        t = ait;
    } else if ( m_hs_state == SAHPI_HS_STATE_ACTIVE ) {
        if ( action != SAHPI_HS_ACTION_EXTRACTION ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
        m_new_hs_state = SAHPI_HS_STATE_EXTRACTION_PENDING;
        t = aet;
    } else {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    CommitChanges();
    m_handler.SetTimer( this, t );

    return SA_OK;
}

SaErrorT cResource::GetHsIndicatorState( SaHpiHsIndicatorStateT& state ) const
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    const SaHpiHsCapabilitiesT hs_caps = m_rpte.HotSwapCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }
    if ( ( hs_caps & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    state = m_hs_ind_state;

    return SA_OK;
}

SaErrorT cResource::SetHsIndicatorState( const SaHpiHsIndicatorStateT& state )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    const SaHpiHsCapabilitiesT hs_caps = m_rpte.HotSwapCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }
    if ( ( hs_caps & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    m_hs_ind_state = state;

    return SA_OK;
}

SaErrorT cResource::GetPowerState( SaHpiPowerStateT& state )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_POWER ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_pwr_cycle_cnt > 0 ) {
        --m_pwr_cycle_cnt;
        if ( m_pwr_cycle_cnt == 0 ) {
            if ( m_pwr_state == SAHPI_POWER_ON ) {
                m_pwr_state = SAHPI_POWER_OFF;
            } else if ( m_pwr_state == SAHPI_POWER_OFF ) {
                m_pwr_state = SAHPI_POWER_ON;
            }
        }
    }

    state = m_pwr_state;

    return SA_OK;
}

SaErrorT cResource::SetPowerState( const SaHpiPowerStateT& state )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_POWER ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }
    if ( state == SAHPI_POWER_CYCLE ) {
        if ( m_pwr_state == SAHPI_POWER_ON ) {
            m_pwr_state = SAHPI_POWER_OFF;
        } else if ( m_pwr_state == SAHPI_POWER_OFF ) {
            m_pwr_state = SAHPI_POWER_ON;
        }
        m_pwr_cycle_cnt = PwrCycleDuration;
    } else {
        m_pwr_state     = state;
        m_pwr_cycle_cnt = 0;
    }

    return SA_OK;
}

SaErrorT cResource::ControlParm( SaHpiParmActionT action )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_CONFIGURATION ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return SA_OK;
}

SaErrorT cResource::GetLoadId( SaHpiLoadIdT& load_id ) const
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_LOAD_ID ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    load_id = m_load_id;

    return SA_OK;
}

SaErrorT cResource::SetLoadId( const SaHpiLoadIdT& load_id )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_LOAD_ID ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    m_load_id = load_id;

    return SA_OK;
}

SaErrorT cResource::GetResetState( SaHpiResetActionT& action ) const
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_RESET ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    action = m_rst_state;

    return SA_OK;
}

SaErrorT cResource::SetResetState( const SaHpiResetActionT& action )
{
    const SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    if ( ( caps & SAHPI_CAPABILITY_RESET ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( ( action == SAHPI_COLD_RESET ) || ( action == SAHPI_WARM_RESET ) ) {
        if ( m_rst_state == SAHPI_RESET_ASSERT ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
        m_rst_state = SAHPI_RESET_DEASSERT;
    } else {
        m_rst_state = action;
    }

    return SA_OK;
}


// Event generation
void cResource::PostEvent( SaHpiEventTypeT type,
                           const SaHpiEventUnionT& data,
                           SaHpiSeverityT severity,
                           const InstrumentList& updates,
                           const InstrumentList& removals ) const
{
    if ( m_log ) {
        const cInstrument * instr = 0;
        if ( !updates.empty() ) {
            instr = updates.front();
        } else if ( !removals.empty() ) {
            instr = removals.front();
        }
        if ( instr ) {
            const SaHpiRdrT& rdr = instr->GetRdr();
            m_log->AddEntry( type, data, severity, &rdr, &m_rpte );
        } else {
            m_log->AddEntry( type, data, severity, 0, &m_rpte );
        }
    }
    if ( !IsVisible() ) {
        return;
    }
    m_handler.PostEvent( type, data, severity, this, updates, removals );
}

void cResource::PostResourceEvent( SaHpiResourceEventTypeT type ) const
{
    SaHpiEventUnionT data;
    data.ResourceEvent.ResourceEventType = type;

    // attach all instruments
    InstrumentList updates;
    if ( type == SAHPI_RESE_RESOURCE_ADDED ) {
        GetAllInstruments( updates );
    }

    SaHpiSeverityT severity;
    switch ( type ) {
        case SAHPI_RESE_RESOURCE_FAILURE:
        case SAHPI_RESE_RESOURCE_RESTORED:
        case SAHPI_RESE_RESOURCE_REMOVED:
            severity = m_rpte.ResourceSeverity;
            break;
        default:
            severity = SAHPI_INFORMATIONAL;
    }

    PostEvent( SAHPI_ET_RESOURCE, data, severity, updates );
}

void cResource::PostHsEvent( SaHpiHsStateT current, SaHpiHsStateT prev ) const
{
    SaHpiEventUnionT data;
    SaHpiHotSwapEventT& hse = data.HotSwapEvent;
    hse.HotSwapState         = current;
    hse.PreviousHotSwapState = prev;
    hse.CauseOfStateChange   = SAHPI_HS_CAUSE_AUTO_POLICY; // TODO

    // attach all instruments
    InstrumentList updates;
    if ( prev == SAHPI_HS_STATE_NOT_PRESENT ) {
        if ( current != SAHPI_HS_STATE_NOT_PRESENT ) {
            GetAllInstruments( updates );
        }
    }

    // TODO severity
    PostEvent( SAHPI_ET_HOTSWAP, data, SAHPI_INFORMATIONAL, updates );
}


// cObject virtual functions
void cResource::BeforeVisibilityChange()
{
    cObject::BeforeVisibilityChange();

    if ( IsVisible() ) {
        // Resource disappears
        if ( m_rpte.ResourceCapabilities & SAHPI_CAPABILITY_FRU ) {
            PostHsEvent( SAHPI_HS_STATE_NOT_PRESENT, m_hs_state );
        } else {
            PostResourceEvent( SAHPI_RESE_RESOURCE_REMOVED );
        }
    }
}

void cResource::AfterVisibilityChange()
{
    if ( IsVisible() ) {
        // Resource appears
        if ( m_rpte.ResourceCapabilities & SAHPI_CAPABILITY_FRU ) {
            PostHsEvent( m_hs_state, SAHPI_HS_STATE_NOT_PRESENT );
        } else {
            PostResourceEvent( SAHPI_RESE_RESOURCE_ADDED );
        }
    }

    cObject::AfterVisibilityChange();
}

void cResource::GetNewNames( cObject::NewNames& names ) const
{
    cObject::GetNewNames( names );
    names.push_back( "log" );

    cInstruments::GetNewNames( names );
}

bool cResource::CreateChild( const std::string& name )
{
    bool rc;

    rc = cObject::CreateChild( name );
    if ( rc ) {
        return true;
    }
    if ( name == cLog::classname ) {
        CreateLog();
        return true;
    }
    rc = CreateInstrument( name );
    if ( rc ) {
        return true;
    }

    return false;
}

bool cResource::RemoveChild( const std::string& name )
{
    bool rc;

    rc = cObject::RemoveChild( name );
    if ( rc ) {
        return true;
    }
    if ( name == cLog::classname ) {
        RemoveLog();
        return true;
    }
    rc = RemoveInstrument( name );
    if ( rc ) {
        return true;
    }

    return false;
}

void cResource::GetChildren( Children& children ) const
{
    cObject::GetChildren( children );
    if ( m_log ) {
        children.push_back( m_log );
    }
    cInstruments::GetChildren( children );
}

void cResource::GetVars( cVars& vars )
{
    cObject::GetVars( vars );

    SaHpiCapabilitiesT caps = m_rpte.ResourceCapabilities;
    SaHpiHsCapabilitiesT hs_caps = m_rpte.HotSwapCapabilities;
    bool fru  = ( caps & SAHPI_CAPABILITY_FRU );
    bool hs   = fru && ( caps & SAHPI_CAPABILITY_MANAGED_HOTSWAP );
    bool ind  = hs && ( hs_caps & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED );
    bool load = ( caps & SAHPI_CAPABILITY_LOAD_ID );
    bool rst  = ( caps & SAHPI_CAPABILITY_RESET );
    bool pwr  = ( caps & SAHPI_CAPABILITY_POWER );

    Structs::GetVars( m_rpte, vars );
    vars << "ResourceFailed"
         << dtSaHpiBoolT
         << DATA( m_failed, m_new_failed )
         << VAR_END();
    vars << IF( hs )
         << "AutoExtractTimeout"
         << dtSaHpiTimeoutT
         << DATA( m_ae_timeout )
         << VAR_END();
    vars << IF( fru )
         << "PreviousHotSwapState"
         << dtSaHpiHsStateT
         << DATA( m_prev_hs_state )
         << READONLY()
         << VAR_END();
    vars << IF( fru )
         << "HotSwapState"
         << dtSaHpiHsStateT
         << DATA( m_hs_state, m_new_hs_state )
         << VAR_END();
    vars << IF( ind )
         << "HotSwapIndicatorState"
         << dtSaHpiHsIndicatorStateT
         << DATA( m_hs_ind_state )
         << VAR_END();
    if ( load ) {
        Structs::GetVars( m_load_id, vars );
    }
    vars << IF( rst )
         << "ResetState"
         << dtSaHpiResetActionT
         << DATA( m_rst_state )
         << VAR_END();
    vars << IF( pwr )
         << "PowerState"
         << dtSaHpiPowerStateT
         << DATA( m_pwr_state )
         << VAR_END();
}

void cResource::BeforeVarSet( const std::string& var_name )
{
    cObject::BeforeVarSet( var_name );

    m_new_failed   = m_failed;
    m_new_hs_state = m_hs_state;
}

void cResource::AfterVarSet( const std::string& var_name )
{
    cObject::AfterVarSet( var_name );

    if ( var_name.find( "RptEntry." ) == 0 ) {
        // RPT Entry was changed
        PostResourceEvent( SAHPI_RESE_RESOURCE_UPDATED );
    }
    if ( var_name == "PowerState" ) {
        m_pwr_cycle_cnt = 0;
    }

    CommitChanges();
}

void cResource::CreateLog()
{
    if ( m_log == 0 ) {
        m_log = new cLog();
        m_rpte.ResourceCapabilities |= cLog::RequiredResourceCap();
        PostResourceEvent( SAHPI_RESE_RESOURCE_UPDATED );
    }
}

void cResource::RemoveLog()
{
    if ( m_log ) {
        delete m_log;
        m_log = 0;
        m_rpte.ResourceCapabilities &= ~cLog::RequiredResourceCap();
        PostResourceEvent( SAHPI_RESE_RESOURCE_UPDATED );
    }
}

void cResource::TimerEvent()
{
    if ( m_hs_state == SAHPI_HS_STATE_INSERTION_PENDING ) {
        m_new_hs_state = SAHPI_HS_STATE_ACTIVE;
    } else if ( m_hs_state == SAHPI_HS_STATE_EXTRACTION_PENDING ) {
        m_new_hs_state = SAHPI_HS_STATE_INACTIVE;
    }

    CommitChanges();
}


void cResource::GetTimeouts( SaHpiTimeoutT& ai_timeout,
                             SaHpiTimeoutT& ae_timeout ) const
{
    SaHpiHsCapabilitiesT hs_caps = m_rpte.HotSwapCapabilities;
    if ( hs_caps & SAHPI_HS_CAPABILITY_AUTOINSERT_IMMEDIATE ) {
        ai_timeout = SAHPI_TIMEOUT_IMMEDIATE;
    } else {
        ai_timeout = m_handler.GetAutoInsertTimeout();
    }
    ae_timeout = m_ae_timeout;
}

void cResource::CommitChanges()
{
    if ( m_failed != m_new_failed ) {
        m_failed              = m_new_failed;
        m_rpte.ResourceFailed = m_failed;

        PostResourceEvent( ( m_failed == SAHPI_FALSE ) ?
                           SAHPI_RESE_RESOURCE_RESTORED :
                           SAHPI_RESE_RESOURCE_FAILURE );
    }

    if ( m_hs_state != m_new_hs_state ) {
        m_prev_hs_state = m_hs_state;
        m_hs_state      = m_new_hs_state;

        PostHsEvent( m_hs_state, m_prev_hs_state );
    }

    SaHpiTimeoutT ait, aet, t;
    if ( m_hs_state == SAHPI_HS_STATE_INSERTION_PENDING ) {
        GetTimeouts( t, aet );
        m_handler.SetTimer( this, t );
    } else if ( m_hs_state == SAHPI_HS_STATE_EXTRACTION_PENDING ) {
        GetTimeouts( ait, t );
        m_handler.SetTimer( this, t );
    }
}


}; // namespace TA

