/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTINSTRUMENTLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <string>

#include <oh_utils.h>

#include "instrument.h"
#include "resource.h"
#include "structs.h"
#include "utils.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static void MakeDefaultRdr( const SaHpiEntityPathT& ep,
                            SaHpiRdrTypeT type,
                            const SaHpiRdrTypeUnionT& data,
                            const std::string& name,
                            SaHpiRdrT& rdr )
{
    SaHpiInstrumentIdT instr_id = SAHPI_ENTRY_UNSPECIFIED;
    switch ( type ) {
        case SAHPI_CTRL_RDR:
            instr_id = data.CtrlRec.Num;
            break;
        case SAHPI_SENSOR_RDR:
            instr_id = data.SensorRec.Num;
            break;
        case SAHPI_INVENTORY_RDR:
            instr_id = data.InventoryRec.IdrId;
            break;
        case SAHPI_WATCHDOG_RDR:
            instr_id = data.WatchdogRec.WatchdogNum;
            break;
        case SAHPI_ANNUNCIATOR_RDR:
            instr_id = data.AnnunciatorRec.AnnunciatorNum;
            break;
        case SAHPI_DIMI_RDR:
            instr_id = data.DimiRec.DimiNum;
            break;
        case SAHPI_FUMI_RDR:
            instr_id = data.FumiRec.Num;
            break;
        default:
            instr_id = SAHPI_ENTRY_UNSPECIFIED;
    }

    rdr.RecordId     = oh_get_rdr_uid( type, instr_id );
    rdr.RdrType      = type;
    rdr.Entity       = ep;
    rdr.IsFru        = SAHPI_FALSE;
    rdr.RdrTypeUnion = data;
    MakeHpiTextBuffer( rdr.IdString, name.c_str() );
}


/**************************************************************
 * class cInstrument
 *************************************************************/
cInstrument::cInstrument( cHandler& handler,
                          cResource& resource,
                          const std::string& name,
                          SaHpiRdrTypeT type,
                          const SaHpiRdrTypeUnionT& data )
    : cObject( name, SAHPI_FALSE ),
      m_handler( handler ),
      m_resource( resource )
{
    MakeDefaultRdr( resource.GetEntityPath(),
                    type,
                    data,
                    GetName(),
                    m_rdr );
}

cInstrument::~cInstrument()
{
    SetVisible( false );
}

const SaHpiRdrT& cInstrument::GetRdr() const
{
    return m_rdr;
}


// cObject virtual functions
void cInstrument::BeforeVisibilityChange()
{
    cObject::BeforeVisibilityChange();

    if ( IsVisible() ) {
        // Instrument disappears
        PostUpdateEvent( true );
    }
}

void cInstrument::AfterVisibilityChange()
{
    if ( IsVisible() ) {
        // Instrument appears
        m_resource.UpdateCaps( RequiredResourceCap() );
        PostUpdateEvent();
    }

    cObject::AfterVisibilityChange();
}

void cInstrument::GetVars( cVars& vars )
{
    cObject::GetVars( vars );
    Structs::GetVars( m_rdr, vars );
}

void cInstrument::AfterVarSet( const std::string& var_name )
{
    cObject::AfterVarSet( var_name );

    if ( var_name.find( "Rdr." ) == 0 ) {
        // RDR has been changed
        HandleRdrChange( var_name );
    }
}


// Event generation
void cInstrument::PostEvent( SaHpiEventTypeT type,
                             const SaHpiEventUnionT& data,
                             SaHpiSeverityT severity,
                             bool remove ) const
{
    if ( !IsVisible() ) {
        return;
    }

    InstrumentList l, l_remove;
    if ( !remove ) {
        l.push_back( this );
    } else {
        l_remove.push_back( this );
    }

    m_resource.PostEvent( type, data, severity, l, l_remove );
}


void cInstrument::PostUpdateEvent( bool remove ) const
{
    SaHpiEventUnionT data;
    data.ResourceEvent.ResourceEventType = SAHPI_RESE_RESOURCE_UPDATED;
    PostEvent( SAHPI_ET_RESOURCE, data, SAHPI_INFORMATIONAL, remove );
}


// Handling RDR changes
void cInstrument::HandleRdrChange( const std::string& field_name )
{
    UpdateRdr( field_name, m_rdr.RdrTypeUnion );
    PostUpdateEvent();
}

void cInstrument::UpdateRdr( const std::string& field_name,
                             SaHpiRdrTypeUnionT& data )
{
    // extend in inherited classes
}


}; // namespace TA

