/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2012
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTAREALITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <stdio.h>

#include <string>

#include "bank.h"
#include "codec.h"
#include "fumi.h"
#include "handler.h"
#include "structs.h"
#include "utils.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiFumiBankInfoT MakeDefaultBankInfo( SaHpiBankNumT num )
{
    SaHpiFumiBankInfoT info;

    info.BankId       = num;
    info.BankSize     = ( num == 0 ) ? 0 : 42;
    info.Position     = num;
    info.BankState    = ( num == 0 ) ? SAHPI_FUMI_BANK_UNKNOWN : SAHPI_FUMI_BANK_VALID;
    FormatHpiTextBuffer( info.Identifier, "/banks/bank%u.img", (unsigned int)num );
    MakeHpiTextBuffer( info.Description, "Firmware" );
    MakeHpiTextBuffer( info.DateTime, "1979-06-10" );
    info.MajorVersion = 1;
    info.MinorVersion = 2;
    info.AuxVersion   = 3;

    return info;
}

static SaHpiFumiLogicalBankInfoT MakeDefaultLogicalBankInfo( SaHpiBankNumT num )
{
    SaHpiFumiLogicalBankInfoT info;

    info.FirmwarePersistentLocationCount = 3;
    info.BankStateFlags = 0;

    SaHpiFumiFirmwareInstanceInfoT& pfw = info.PendingFwInstance;
    pfw.InstancePresent = SAHPI_FALSE;
    FormatHpiTextBuffer( pfw.Identifier, "/banks/bank%u_pending.img", (unsigned int)num );
    MakeHpiTextBuffer( pfw.Description, "Firmware" );
    MakeHpiTextBuffer( pfw.DateTime, "1979-06-14" );
    pfw.MajorVersion    = 1;
    pfw.MinorVersion    = 2;
    pfw.AuxVersion      = 4;

    SaHpiFumiFirmwareInstanceInfoT& rfw = info.RollbackFwInstance;
    rfw.InstancePresent = SAHPI_FALSE;
    FormatHpiTextBuffer( rfw.Identifier, "/banks/bank%u_rollback.img", (unsigned int)num );
    MakeHpiTextBuffer( rfw.Description, "Firmware" );
    MakeHpiTextBuffer( rfw.DateTime, "1979-06-05" );
    rfw.MajorVersion    = 1;
    rfw.MinorVersion    = 2;
    rfw.AuxVersion      = 2;

    return info;
}

static SaHpiFumiSourceInfoT MakeDefaultSourceInfo()
{
    SaHpiFumiSourceInfoT info;

    MakeHpiTextBuffer( info.SourceUri, "file:///tmp/1.fw" );
    info.SourceStatus = SAHPI_FUMI_SRC_VALID;
    MakeHpiTextBuffer( info.Identifier, "" );
    MakeHpiTextBuffer( info.Description, "Firmware" );
    MakeHpiTextBuffer( info.DateTime, "1979-06-14" );
    info.MajorVersion = 1;
    info.MinorVersion = 2;
    info.AuxVersion   = 4;

    return info;
}

static void MakeDefaultComponents( SaHpiFumiComponentInfoT * components,
                                   size_t max_components )
{
    for ( size_t i = 0; i < max_components; ++i ) {
        SaHpiFumiComponentInfoT& c = components[i];
        SaHpiFumiFirmwareInstanceInfoT& fw = c.MainFwInstance;

        c.EntryId          = i;
        c.ComponentId      = i;
        fw.InstancePresent = SAHPI_TRUE;
        FormatHpiTextBuffer( fw.Identifier, "/components/component%u.img", (unsigned int)i );
        MakeHpiTextBuffer( fw.Description, "Firmware" );
        MakeHpiTextBuffer( fw.DateTime, "1979-06-10" );
        fw.MajorVersion    = 1;
        fw.MinorVersion    = 2;
        fw.AuxVersion      = 3;
        c.ComponentFlags   = 0;
    }
}

static void MakeDefaultLogicalComponents( SaHpiFumiLogicalComponentInfoT * components,
                                          size_t max_components )
{
    for ( size_t i = 0; i < max_components; ++i ) {
        SaHpiFumiLogicalComponentInfoT& c = components[i];

        c.EntryId          = i;
        c.ComponentId      = i;

        SaHpiFumiFirmwareInstanceInfoT& pfw = c.PendingFwInstance;
        pfw.InstancePresent = SAHPI_FALSE;
        FormatHpiTextBuffer( pfw.Identifier, "/components/component%u_pending.img", (unsigned int)i );
        MakeHpiTextBuffer( pfw.Description, "Firmware" );
        MakeHpiTextBuffer( pfw.DateTime, "1979-06-14" );
        pfw.MajorVersion    = 1;
        pfw.MinorVersion    = 2;
        pfw.AuxVersion      = 4;

        SaHpiFumiFirmwareInstanceInfoT& rfw = c.RollbackFwInstance;
        rfw.InstancePresent = SAHPI_FALSE;
        FormatHpiTextBuffer( rfw.Identifier, "/components/component%u_rollback.img", (unsigned int)i );
        MakeHpiTextBuffer( rfw.Description, "Firmware" );
        MakeHpiTextBuffer( rfw.DateTime, "1979-06-05" );
        rfw.MajorVersion    = 1;
        rfw.MinorVersion    = 2;
        rfw.AuxVersion      = 2;

        c.ComponentFlags   = 0;
    }
}

static void MakeDefaultSourceComponents( SaHpiFumiComponentInfoT * components,
                                         size_t max_components )
{
    for ( size_t i = 0; i < max_components; ++i ) {
        SaHpiFumiComponentInfoT& c = components[i];
        SaHpiFumiFirmwareInstanceInfoT& fw = c.MainFwInstance;

        c.EntryId          = i;
        c.ComponentId      = i;
        fw.InstancePresent = SAHPI_TRUE;
        FormatHpiTextBuffer( fw.Identifier, "/components/component%u.img", (unsigned int)i );
        MakeHpiTextBuffer( fw.Description, "Firmware" );
        MakeHpiTextBuffer( fw.DateTime, "1979-06-14" );
        fw.MajorVersion    = 1;
        fw.MinorVersion    = 2;
        fw.AuxVersion      = 4;
        c.ComponentFlags   = 0;
    }
}

static bool GetEntryIds( SaHpiEntryIdT requested_eid,
                         const SaHpiBoolT * arr_enabled,
                         size_t arr_size,
                         SaHpiEntryIdT& eid,
                         SaHpiEntryIdT& next_eid )
{
    if ( requested_eid >= arr_size ) {
        return false;
    }
    if ( requested_eid == SAHPI_FIRST_ENTRY ) {
        eid = SAHPI_LAST_ENTRY;
        for ( SaHpiEntryIdT i = 0; i < arr_size; ++i ) {
            if ( arr_enabled[i] != SAHPI_FALSE ) {
                eid = i;
                break;
            }
        }
        if ( eid == SAHPI_LAST_ENTRY ) {
            return false;
        }
    } else {
        eid = requested_eid;
        if ( arr_enabled[eid] == SAHPI_FALSE ) {
            return false;
        }
    }
    next_eid = SAHPI_LAST_ENTRY;
    for ( SaHpiEntryIdT i = eid + 1; i < arr_size; ++i ) {
        if ( arr_enabled[i] != SAHPI_FALSE ) {
            next_eid = i;
            break;
        }
    }

    return true;
}

static void ResetSourceInfo( SaHpiFumiSourceInfoT& info,
                             SaHpiFumiSourceStatusT status,
                             bool reset_uri )
{
    if ( reset_uri ) {
        MakeHpiTextBuffer( info.SourceUri, "" );
    }
    info.SourceStatus = status;
    MakeHpiTextBuffer( info.Identifier, "" );
    MakeHpiTextBuffer( info.Description, "" );
    MakeHpiTextBuffer( info.DateTime, "" );
    info.MajorVersion = 0;
    info.MinorVersion = 0;
    info.AuxVersion   = 0;
}

static void ResetComponents( SaHpiBoolT * enabled,
                             SaHpiFumiComponentInfoT * components,
                             size_t max_components )
{
    for ( size_t i = 0; i < max_components; ++i ) {
        enabled[i] = SAHPI_FALSE;

        SaHpiFumiComponentInfoT& c = components[i];
        SaHpiFumiFirmwareInstanceInfoT& fw = c.MainFwInstance;

        c.EntryId          = i;
        c.ComponentId      = i;
        fw.InstancePresent = SAHPI_FALSE;
        MakeHpiTextBuffer( fw.Identifier, "" );
        MakeHpiTextBuffer( fw.Description, "" );
        MakeHpiTextBuffer( fw.DateTime, "" );
        fw.MajorVersion    = 0;
        fw.MinorVersion    = 0;
        fw.AuxVersion      = 0;
        c.ComponentFlags   = 0;
    }
}

static bool SplitUri( const SaHpiTextBufferT& uri,
                      std::string& proto,
                      std::string& path )
{
    std::string s;
    ToTxt( Var( dtSaHpiTextBufferT, "", &uri, 0 ), s );

    size_t pos = 0, pos2;

    pos2 = s.find_first_of( ':', pos );
    if ( pos2 == std::string::npos ) {
        return false;
    }
    std::string type( s.begin() + pos, s.begin() + pos2 );
    if ( ( type != "TEXT" ) && ( type != "ASCII6" ) ) {
        return false;
    }

    pos = pos2 + 1;
    pos2 =  s.find( "://", pos );
    if ( pos2 == std::string::npos ) {
        return false;
    }
    proto.assign( s.begin() + pos, s.begin() + pos2 );

    pos = pos2 + 3;
    if ( pos >= s.size() ) {
        return false;
    }
    path.assign( s.begin() + pos, s.end() );

    return true;
}


/**************************************************************
 * class cBank
 *************************************************************/
const std::string cBank::classname( "bank" );

cBank::cBank( cHandler& handler, cFumi& fumi, SaHpiBankNumT num )
    : cObject( AssembleNumberedObjectName( classname, num ) ),
      m_handler( handler ),
      m_fumi( fumi ),
      m_info( MakeDefaultBankInfo( num ) ),
      m_logical_info( MakeDefaultLogicalBankInfo( num ) ),
      m_src_set( SAHPI_FALSE ),
      m_src_info( MakeDefaultSourceInfo() ),
      m_status( SAHPI_FUMI_OPERATION_NOTSTARTED ),
      m_verify_main( false ),
      m_copy_dest_num( 0xFFU )
{
    MakeDefaultComponents( m_components, MAX_COMPONENTS );
    MakeDefaultLogicalComponents( m_logical_components, MAX_COMPONENTS );
    MakeDefaultSourceComponents( m_src_components, MAX_COMPONENTS );

    for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
        m_enabled_components[i]     = SAHPI_FALSE;
        m_enabled_src_components[i] = SAHPI_FALSE;
    }

    m_enabled_components[2] = SAHPI_TRUE;
    m_enabled_components[5] = SAHPI_TRUE;

    m_enabled_src_components[1] = SAHPI_TRUE;
    m_enabled_src_components[3] = SAHPI_TRUE;

    m_next.action_duration = 5000000000LL; // 5 sec
    m_next.pass.validate   = SAHPI_TRUE;
    m_next.pass.install    = SAHPI_TRUE;
    m_next.pass.rollback   = SAHPI_TRUE;
    m_next.pass.backup     = SAHPI_TRUE;
    m_next.pass.copy       = SAHPI_TRUE;
    m_next.pass.verify     = SAHPI_TRUE;
    m_next.pass.verifymain = SAHPI_TRUE;
    m_next.pass.activate   = SAHPI_TRUE;
    m_next.src_fail_status = SAHPI_FUMI_SRC_UNREACHABLE;
    m_next.src_info        = MakeDefaultSourceInfo();
}

cBank::~cBank()
{
    m_handler.CancelTimer( this );
}


SaHpiUint32T cBank::Position() const
{
    return m_info.Position;
}

void cBank::SetPosition( SaHpiUint32T pos )
{
    m_info.Position = pos;
}

SaHpiFumiBankStateT cBank::State() const
{
    return m_info.BankState;
}


// HPI Interface
SaErrorT cBank::SetSource( const SaHpiTextBufferT& uri )
{
    if ( m_src_set != SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    ResetSourceInfo( m_src_info, SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED, true );
    ResetComponents( m_enabled_src_components, m_src_components, MAX_COMPONENTS );
    m_src_info.SourceUri = uri;
    m_src_set = SAHPI_TRUE;

    return SA_OK;
}

SaErrorT cBank::StartSourceValidation()
{
    if ( m_src_set == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_src_info.SourceStatus = SAHPI_FUMI_SRC_VALIDATION_INITIATED;
    ChangeStatus( SAHPI_FUMI_SOURCE_VALIDATION_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::GetSourceInfo( SaHpiFumiSourceInfoT& info ) const
{
    if ( m_src_set == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    info = m_src_info;

    return SA_OK;
}

SaErrorT cBank::GetSourceComponentInfo( SaHpiEntryIdT eid,
                                        SaHpiEntryIdT& next_eid,
                                        SaHpiFumiComponentInfoT& info ) const
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_COMPONENTS ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_src_set == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    bool rc = GetEntryIds( eid,
                           m_enabled_src_components,
                           MAX_COMPONENTS,
                           eid,
                           next_eid );
    if ( !rc ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    info = m_src_components[eid];

    return SA_OK;
}

SaErrorT cBank::GetTargetInfo( SaHpiFumiBankInfoT& info ) const
{
    info = m_info;
    return SA_OK;
}

SaErrorT cBank::GetTargetComponentInfo( SaHpiEntryIdT eid,
                                        SaHpiEntryIdT& next_eid,
                                        SaHpiFumiComponentInfoT& info ) const
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_COMPONENTS ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    bool rc = GetEntryIds( eid,
                           m_enabled_components,
                           MAX_COMPONENTS,
                           eid,
                           next_eid );
    if ( !rc ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    info = m_components[eid];

    return SA_OK;
}

SaErrorT cBank::GetLogicalTargetInfo( SaHpiFumiLogicalBankInfoT& info ) const
{
    if ( m_info.BankId != 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    info = m_logical_info;

    return SA_OK;
}

SaErrorT cBank::GetLogicalTargetComponentInfo( SaHpiEntryIdT eid,
                                               SaHpiEntryIdT& next_eid,
                                               SaHpiFumiLogicalComponentInfoT& info ) const
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_COMPONENTS ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_info.BankId != 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    bool rc = GetEntryIds( eid,
                           m_enabled_components,
                           MAX_COMPONENTS,
                           eid,
                           next_eid );
    if ( !rc ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    info = m_logical_components[eid];

    return SA_OK;
}

SaErrorT cBank::StartBackup()
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_BACKUP ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_info.BankId != 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    ChangeStatus( SAHPI_FUMI_BACKUP_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::StartCopy( SaHpiBankNumT dest_num )
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_BANKCOPY ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_info.BankId == 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }
    if ( dest_num == 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }
    if ( m_info.BankId == dest_num ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }
    cBank * dest = m_fumi.GetBank( dest_num );
    if ( !dest ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_copy_dest_num = dest_num;
    ChangeStatus( SAHPI_FUMI_BANK_COPY_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::StartInstallation()
{
    if ( m_src_set == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }
    bool ok = false;
    ok = ok || ( m_src_info.SourceStatus == SAHPI_FUMI_SRC_VALID );
    ok = ok || ( m_src_info.SourceStatus == SAHPI_FUMI_SRC_VALIDITY_UNKNOWN );
    if ( !ok ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    ChangeStatus( SAHPI_FUMI_INSTALL_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::GetUpgradeStatus( SaHpiFumiUpgradeStatusT& status ) const
{
    status = m_status;
    return SA_OK;
}

SaErrorT cBank::StartTargetVerification()
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_TARGET_VERIFY ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_src_set == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }
    bool ok = false;
    ok = ok || ( m_src_info.SourceStatus == SAHPI_FUMI_SRC_VALID );
    ok = ok || ( m_src_info.SourceStatus == SAHPI_FUMI_SRC_VALIDITY_UNKNOWN );
    if ( !ok ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    if ( m_info.BankId == 0 ) {
        if ( m_logical_info.PendingFwInstance.InstancePresent == SAHPI_FALSE ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_verify_main = false;
    ChangeStatus( SAHPI_FUMI_TARGET_VERIFY_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::StartTargetMainVerification()
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_TARGET_VERIFY_MAIN ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_info.BankId != 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    if ( m_src_set == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }
    bool ok = false;
    ok = ok || ( m_src_info.SourceStatus == SAHPI_FUMI_SRC_VALID );
    ok = ok || ( m_src_info.SourceStatus == SAHPI_FUMI_SRC_VALIDITY_UNKNOWN );
    if ( !ok ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_verify_main = true;
    ChangeStatus( SAHPI_FUMI_TARGET_VERIFY_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::CancelUpgrade()
{
    bool has = m_handler.HasTimerSet( this );
    if ( !has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_handler.CancelTimer( this );

    SaHpiFumiUpgradeStatusT new_status;

    switch ( m_status ) {
        case SAHPI_FUMI_SOURCE_VALIDATION_INITIATED:
            new_status = SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED;
            break;
        case SAHPI_FUMI_INSTALL_INITIATED:
            new_status = SAHPI_FUMI_INSTALL_CANCELLED;
            break;
        case SAHPI_FUMI_BACKUP_INITIATED:
            new_status = SAHPI_FUMI_BACKUP_CANCELLED;
            break;
        case SAHPI_FUMI_BANK_COPY_INITIATED:
            new_status = SAHPI_FUMI_BANK_COPY_CANCELLED;
            break;
        case SAHPI_FUMI_TARGET_VERIFY_INITIATED:
            new_status = SAHPI_FUMI_TARGET_VERIFY_CANCELLED;
            break;
        case SAHPI_FUMI_ACTIVATE_INITIATED:
            new_status = SAHPI_FUMI_ACTIVATE_CANCELLED;
            break;
        case SAHPI_FUMI_ROLLBACK_INITIATED:
        case SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED:
        case SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED:
            new_status = SAHPI_FUMI_ROLLBACK_CANCELLED;
            break;
        default:
            new_status = m_status;
    }

    ChangeStatus( new_status );

    return SA_OK;
}

SaErrorT cBank::StartRollback()
{
    if ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_ROLLBACK ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( m_info.BankId != 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    if ( m_logical_info.RollbackFwInstance.InstancePresent == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    ChangeStatus( SAHPI_FUMI_ROLLBACK_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::StartActivation( SaHpiBoolT pass )
{
    if ( m_info.BankId == 0 ) {
        if ( m_logical_info.PendingFwInstance.InstancePresent == SAHPI_FALSE ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
    }

    bool has = m_handler.HasTimerSet( this );
    if ( has ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_next.pass.activate = pass;
    ChangeStatus( SAHPI_FUMI_ACTIVATE_INITIATED );
    m_handler.SetTimer( this, m_next.action_duration );

    return SA_OK;
}

SaErrorT cBank::Cleanup()
{
    m_handler.CancelTimer( this );

    ResetSourceInfo( m_src_info, SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED, true );
    ResetComponents( m_enabled_src_components, m_src_components, MAX_COMPONENTS );
    m_src_set = SAHPI_FALSE;

    m_verify_main   = false;
    m_copy_dest_num = 0xFFU;

    ChangeStatus( SAHPI_FUMI_OPERATION_NOTSTARTED );

    return SA_OK;
}


// cObject virtual functions
void cBank::GetVars( cVars& vars )
{
    cObject::GetVars( vars );
    Structs::GetVars( m_info, vars );
    if ( m_info.BankId == 0 ) {
        Structs::GetVars( m_logical_info, vars );
    }

    for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
        char prefix[256];
        snprintf( &prefix[0], sizeof(prefix), "Component[%u]", (unsigned int)i );
        const std::string name( prefix );

        vars << name + ".Enabled"
             << dtSaHpiBoolT
             << DATA( m_enabled_components[i] )
             << VAR_END();

        if ( m_enabled_components[i] != SAHPI_FALSE ) {
            Structs::GetVars( name, m_components[i], vars );
            if ( m_info.BankId == 0 ) {
                Structs::GetVars( name, m_logical_components[i], vars );
            }
        }
    }

    vars << "Next.ActionDuration"
         << dtSaHpiTimeoutT
         << DATA( m_next.action_duration )
         << VAR_END();
    vars << "Next.Pass.Validate"
         << dtSaHpiBoolT
         << DATA( m_next.pass.validate )
         << VAR_END();
    vars << "Next.Pass.Install"
         << dtSaHpiBoolT
         << DATA( m_next.pass.install )
         << VAR_END();
    vars << "Next.Pass.Rollback"
         << dtSaHpiBoolT
         << DATA( m_next.pass.rollback )
         << VAR_END();
    vars << "Next.Pass.Backup"
         << dtSaHpiBoolT
         << DATA( m_next.pass.backup )
         << VAR_END();
    vars << "Next.Pass.Copy"
         << dtSaHpiBoolT
         << DATA( m_next.pass.copy )
         << VAR_END();
    vars << "Next.Pass.Verify"
         << dtSaHpiBoolT
         << DATA( m_next.pass.verify )
         << VAR_END();
    vars << "Next.Pass.Verifymain"
         << dtSaHpiBoolT
         << DATA( m_next.pass.verifymain )
         << VAR_END();
    vars << "Next.Pass.SourceFailStatus"
         << dtSaHpiFumiSourceStatusT
         << DATA( m_next.src_fail_status )
         << VAR_END();

    Structs::GetVars( "Next.SourceInfo", m_next.src_info, true, vars );
}

void cBank::AfterVarSet( const std::string& var_name )
{
    cObject::AfterVarSet( var_name );
}


// cTimerCallback virtual functions
void cBank::TimerEvent()
{
    cLocker<cHandler> al( &m_handler );

    switch ( m_status ) {
        case SAHPI_FUMI_SOURCE_VALIDATION_INITIATED:
            DoValidation();
            break;
        case SAHPI_FUMI_INSTALL_INITIATED:
            DoInstall();
            break;
        case SAHPI_FUMI_BACKUP_INITIATED:
            DoBackup();
            break;
        case SAHPI_FUMI_BANK_COPY_INITIATED:
            DoCopy();
            break;
        case SAHPI_FUMI_TARGET_VERIFY_INITIATED:
            DoVerification();
            break;
        case SAHPI_FUMI_ACTIVATE_INITIATED:
            DoActivation();
            break;
        case SAHPI_FUMI_ROLLBACK_INITIATED:
        case SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED:
        case SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED:
            DoRollback();
            break;
        default:
            break;
    }
}


void cBank::ChangeStatus( SaHpiFumiUpgradeStatusT status )
{
    if ( m_status == status ) {
        return;
    }

    m_status = status;
    if ( IsVisible() ) {
        m_fumi.PostEvent( m_info.BankId, m_status );
    }
}


void cBank::DoValidation()
{
    bool rc;
    std::string proto, path;
    rc = SplitUri( m_src_info.SourceUri, proto, path );
    if ( !rc ) {
        m_src_info.SourceStatus = SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED;
        ChangeStatus( SAHPI_FUMI_SOURCE_VALIDATION_FAILED );
        return;
    }
    rc = m_fumi.CheckProtocol( proto );
    if ( !rc ) {
        m_src_info.SourceStatus = SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED;
        ChangeStatus( SAHPI_FUMI_SOURCE_VALIDATION_FAILED );
        return;
    }

    if ( m_next.pass.validate == SAHPI_FALSE ) {
        m_src_info.SourceStatus = m_next.src_fail_status;
        ChangeStatus( SAHPI_FUMI_SOURCE_VALIDATION_FAILED );
        return;
    }

    m_next.src_info.SourceUri    = m_src_info.SourceUri;
    m_next.src_info.SourceStatus = SAHPI_FUMI_SRC_VALID;
    m_src_info = m_next.src_info;
    for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
        m_enabled_src_components[i] = m_enabled_components[i];
        SaHpiFumiComponentInfoT& c = m_src_components[i];
        c = m_components[i];
        c.MainFwInstance.DateTime     = m_next.src_info.DateTime;
        c.MainFwInstance.MajorVersion = m_next.src_info.MajorVersion;
        c.MainFwInstance.MinorVersion = m_next.src_info.MinorVersion;
        c.MainFwInstance.AuxVersion   = m_next.src_info.AuxVersion;
    }

    ChangeStatus( SAHPI_FUMI_SOURCE_VALIDATION_DONE );
}

void cBank::DoInstall()
{
    if ( m_next.pass.install == SAHPI_FALSE ) {
        SaHpiFumiUpgradeStatusT new_status = SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE;
        if ( m_info.BankId == 0 ) {
            bool has_rb_fw       = ( m_logical_info.RollbackFwInstance.InstancePresent != SAHPI_FALSE );
            bool has_autorb_cap  = ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_AUTOROLLBACK ) != 0 );
            bool autorb_disabled =  m_fumi.IsAutoRollbackDisabled();
            if ( has_rb_fw ) {
                if ( has_autorb_cap && ( !autorb_disabled ) ) {
                    new_status = SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED;
                } else {
                    new_status = SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED;
                }
            }
        }
        ChangeStatus( new_status );
        if ( new_status == SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED ) {
            m_handler.SetTimer( this, m_next.action_duration );
        }
        return;
    }

    if ( m_info.BankId == 0 ) {
        m_logical_info.PendingFwInstance.InstancePresent = SAHPI_TRUE;
        m_logical_info.PendingFwInstance.Identifier      = m_src_info.Identifier;
        m_logical_info.PendingFwInstance.Description     = m_src_info.Description;
        m_logical_info.PendingFwInstance.DateTime        = m_src_info.DateTime;
        m_logical_info.PendingFwInstance.MajorVersion    = m_src_info.MajorVersion;
        m_logical_info.PendingFwInstance.MinorVersion    = m_src_info.MinorVersion;
        m_logical_info.PendingFwInstance.AuxVersion      = m_src_info.AuxVersion;
        for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
            m_logical_components[i].PendingFwInstance = m_src_components[i].MainFwInstance;
        }
    } else {
        m_info.Identifier   = m_src_info.Identifier;
        m_info.Description  = m_src_info.Description;
        m_info.DateTime     = m_src_info.DateTime;
        m_info.MajorVersion = m_src_info.MajorVersion;
        m_info.MinorVersion = m_src_info.MinorVersion;
        m_info.AuxVersion   = m_src_info.AuxVersion;
        for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
            m_components[i].MainFwInstance = m_src_components[i].MainFwInstance;
        }
    }

    ChangeStatus( SAHPI_FUMI_INSTALL_DONE );
}

void cBank::DoBackup()
{
    if ( m_next.pass.backup == SAHPI_FALSE ) {
        ChangeStatus( SAHPI_FUMI_BACKUP_FAILED );
    }

    m_logical_info.RollbackFwInstance.InstancePresent = SAHPI_TRUE;
    m_logical_info.RollbackFwInstance.Identifier      = m_info.Identifier;
    m_logical_info.RollbackFwInstance.Description     = m_info.Description;
    m_logical_info.RollbackFwInstance.DateTime        = m_info.DateTime;
    m_logical_info.RollbackFwInstance.MajorVersion    = m_info.MajorVersion;
    m_logical_info.RollbackFwInstance.MinorVersion    = m_info.MinorVersion;
    m_logical_info.RollbackFwInstance.AuxVersion      = m_info.AuxVersion;

    for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
        m_logical_components[i].RollbackFwInstance = m_components[i].MainFwInstance;
    }

    ChangeStatus( SAHPI_FUMI_BACKUP_DONE );
}

void cBank::DoCopy()
{
    if ( m_next.pass.copy == SAHPI_FALSE ) {
        ChangeStatus( SAHPI_FUMI_BANK_COPY_FAILED );
        return;
    }

    cBank * dest = m_fumi.GetBank( m_copy_dest_num );
    if ( !dest ) {
        ChangeStatus( SAHPI_FUMI_BANK_COPY_FAILED );
        return;
    }

    dest->m_info.Identifier   = m_info.Identifier;
    dest->m_info.Description  = m_info.Description;
    dest->m_info.DateTime     = m_info.DateTime;
    dest->m_info.MajorVersion = m_info.MajorVersion;
    dest->m_info.MinorVersion = m_info.MinorVersion;
    dest->m_info.AuxVersion   = m_info.AuxVersion;
    for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
        dest->m_enabled_components[i] = m_enabled_components[i];
        dest->m_components[i] = m_components[i];
    }

    ChangeStatus( SAHPI_FUMI_BANK_COPY_DONE );
}

void cBank::DoVerification()
{
    bool fail;
    if ( m_verify_main ) {
        fail = ( m_next.pass.verifymain == SAHPI_FALSE );
    } else {
        fail = ( m_next.pass.verify == SAHPI_FALSE );
    }
    if ( fail ) {
        ChangeStatus( SAHPI_FUMI_TARGET_VERIFY_FAILED );
        return;
    }

    ChangeStatus( SAHPI_FUMI_TARGET_VERIFY_DONE );
}

void cBank::DoActivation()
{
    if ( m_next.pass.activate == SAHPI_FALSE ) {
        SaHpiFumiUpgradeStatusT new_status = SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE;
        if ( m_info.BankId == 0 ) {
            bool has_rb_fw       = ( m_logical_info.RollbackFwInstance.InstancePresent != SAHPI_FALSE );
            bool has_autorb_cap  = ( ( m_fumi.Capabilities() & SAHPI_FUMI_CAP_AUTOROLLBACK ) != 0 );
            bool autorb_disabled =  m_fumi.IsAutoRollbackDisabled();
            if ( has_rb_fw ) {
                if ( has_autorb_cap && ( !autorb_disabled ) ) {
                    new_status = SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED;
                } else {
                    new_status = SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED;
                }
            }
        }
        ChangeStatus( new_status );
        if ( new_status == SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED ) {
            m_handler.SetTimer( this, m_next.action_duration );
        }
        return;
    }

    if ( m_info.BankId == 0 ) {
        m_info.Identifier   = m_logical_info.PendingFwInstance.Identifier;
        m_info.Description  = m_logical_info.PendingFwInstance.Description;
        m_info.DateTime     = m_logical_info.PendingFwInstance.DateTime;
        m_info.MajorVersion = m_logical_info.PendingFwInstance.MajorVersion;
        m_info.MinorVersion = m_logical_info.PendingFwInstance.MinorVersion;
        m_info.AuxVersion   = m_logical_info.PendingFwInstance.AuxVersion;
        m_logical_info.PendingFwInstance.InstancePresent = SAHPI_FALSE;
        for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
            m_components[i].MainFwInstance = m_logical_components[i].PendingFwInstance;
            m_logical_components[i].PendingFwInstance.InstancePresent = SAHPI_FALSE;
        }
    }

    ChangeStatus( SAHPI_FUMI_ACTIVATE_DONE );
}

void cBank::DoRollback()
{
    if ( m_next.pass.rollback == SAHPI_FALSE ) {
        ChangeStatus( SAHPI_FUMI_ROLLBACK_FAILED );
        return;
    }

    m_info.Identifier   = m_logical_info.RollbackFwInstance.Identifier;
    m_info.Description  = m_logical_info.RollbackFwInstance.Description;
    m_info.DateTime     = m_logical_info.RollbackFwInstance.DateTime;
    m_info.MajorVersion = m_logical_info.RollbackFwInstance.MajorVersion;
    m_info.MinorVersion = m_logical_info.RollbackFwInstance.MinorVersion;
    m_info.AuxVersion   = m_logical_info.RollbackFwInstance.AuxVersion;
    m_logical_info.RollbackFwInstance.InstancePresent = SAHPI_FALSE;
    for ( size_t i = 0; i < MAX_COMPONENTS; ++i ) {
        m_components[i].MainFwInstance = m_logical_components[i].RollbackFwInstance;
        m_logical_components[i].RollbackFwInstance.InstancePresent = SAHPI_FALSE;
    }

    ChangeStatus( SAHPI_FUMI_ROLLBACK_DONE );
}


}; // namespace TA

