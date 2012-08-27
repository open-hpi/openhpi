/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
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

#include <algorithm>
#include <string>
#include <vector>

#include <SaHpi.h>

#include "bank.h"
#include "codec.h"
#include "fumi.h"
#include "structs.h"
#include "utils.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultFumiRec( SaHpiFumiNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiFumiRecT& rec = data.FumiRec;

    rec.Num        = num;
    rec.AccessProt = SAHPI_FUMI_PROT_LOCAL;
    rec.Capability = SAHPI_FUMI_CAP_ROLLBACK |
                     SAHPI_FUMI_CAP_BACKUP |
                     SAHPI_FUMI_CAP_TARGET_VERIFY |
                     SAHPI_FUMI_CAP_TARGET_VERIFY_MAIN |
                     SAHPI_FUMI_CAP_COMPONENTS |
                     SAHPI_FUMI_CAP_AUTOROLLBACK |
                     SAHPI_FUMI_CAP_AUTOROLLBACK_CAN_BE_DISABLED;
    rec.NumBanks   = 0;
    rec.Oem        = 0;

    return data;
}

static SaHpiFumiSpecInfoT MakeDefaultSpecInfo()
{
    SaHpiFumiSpecInfoT spec_info;

    spec_info.SpecInfoType = SAHPI_FUMI_SPEC_INFO_SAF_DEFINED;
    spec_info.SpecInfoTypeUnion.SafDefined.SpecID = SAHPI_FUMI_SPEC_HPM1;
    spec_info.SpecInfoTypeUnion.SafDefined.RevisionID = 0;

    return spec_info;
}

static SaHpiFumiServiceImpactDataT MakeDefaultServiceImpact()
{
    SaHpiFumiServiceImpactDataT service_impact;

    service_impact.NumEntities = 0;
    for ( size_t i = 0; i < SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
        MakeUnspecifiedHpiEntityPath( service_impact.ImpactedEntities[i].ImpactedEntity );
        service_impact.ImpactedEntities[i].ServiceImpact = SAHPI_FUMI_PROCESS_NONDEGRADING;
    }

    return service_impact;
}


/**************************************************************
 * Functors for working with banks
 *************************************************************/
struct BankDeleter
{
    void operator ()( cBank * bank )
    {
        delete bank;
    }
};

struct ObjectCollector
{
    explicit ObjectCollector( cObject::Children& _children )
        : children( _children )
    {
        // empty
    }

    void operator ()( cBank * bank )
    {
        if ( bank ) {
            children.push_back( bank );
        }
    }

    cObject::Children& children;
};


/**************************************************************
 * class cFumi
 *************************************************************/
const std::string cFumi::classname( "fumi" );

cFumi::cFumi( cHandler& handler, cResource& resource, SaHpiFumiNumT num )
    : cInstrument( handler,
                   resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_FUMI_RDR,
                   MakeDefaultFumiRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.FumiRec ),
      m_spec_info( MakeDefaultSpecInfo() ),
      m_service_impact( MakeDefaultServiceImpact() ),
      m_auto_rb_disabled( SAHPI_FALSE )
{
    m_next.pass.activate = SAHPI_TRUE;

    // Create logical bank
    m_banks.push_back( new cBank( m_handler, *this, 0 ) );
}

cFumi::~cFumi()
{
    BankDeleter deleter;
    std::for_each( m_banks.begin(), m_banks.end(), deleter );
    m_banks.clear();
}


SaHpiFumiCapabilityT cFumi::Capabilities() const
{
    return m_rec.Capability;
}

bool cFumi::CheckProtocol( const std::string& proto ) const
{
    if ( proto == "tftp" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_TFTP ) != 0;
    }
    if ( proto == "ftp" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_FTP ) != 0;
    }
    if ( proto == "http" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_HTTP ) != 0;
    }
    if ( proto == "https" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_HTTP ) != 0;
    }
    if ( proto == "ldap" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_LDAP ) != 0;
    }
    if ( proto == "file" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_LOCAL ) != 0;
    }
    if ( proto == "local" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_LOCAL ) != 0;
    }
    if ( proto == "nfs" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_NFS ) != 0;
    }
    if ( proto == "dbaccess" ) {
        return ( m_rec.AccessProt & SAHPI_FUMI_PROT_DBACCESS ) != 0;
    }

    return false;
}

cBank * cFumi::GetBank( SaHpiBankNumT bnum ) const
{
    return ( bnum < m_banks.size() ) ? m_banks[bnum] : 0;
}

bool cFumi::IsAutoRollbackDisabled() const
{
    return ( m_auto_rb_disabled != SAHPI_FALSE );
}

bool cFumi::ShallNextActivationPass() const
{
    return ( m_next.pass.activate != SAHPI_FALSE );
}

void cFumi::PostEvent( SaHpiBankNumT bnum, SaHpiFumiUpgradeStatusT status )
{
    SaHpiEventUnionT data;
    SaHpiFumiEventT& fe = data.FumiEvent;

    fe.FumiNum       = m_rec.Num;
    fe.BankNum       = bnum;
    fe.UpgradeStatus = status;

    cInstrument::PostEvent( SAHPI_ET_FUMI, data, SAHPI_INFORMATIONAL );
}


// HPI interface
SaErrorT cFumi::GetSpecInfo( SaHpiFumiSpecInfoT& specinfo ) const
{
    specinfo = m_spec_info;
    return SA_OK;
}

SaErrorT cFumi::GetServiceImpact( SaHpiFumiServiceImpactDataT& data ) const
{
    data = m_service_impact;
    return SA_OK;
}

SaErrorT cFumi::SetBootOrder( SaHpiBankNumT bnum, SaHpiUint32T position )
{
    if ( ( m_rec.Capability & SAHPI_FUMI_CAP_BANKREORDER ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    if ( bnum == 0 ) {
        return SA_ERR_HPI_INVALID_DATA;
    }
    if ( bnum >= m_banks.size() ) {
        return SA_ERR_HPI_INVALID_DATA;
    }
    if ( ( position == 0 ) || ( position >= m_banks.size() ) ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    std::vector<uint16_t> sorted;
    for ( size_t i = 1, n = m_banks.size(); i < n; ++i ) {
        if ( i != bnum ) {
            sorted.push_back( MakeUint16T( m_banks[i]->Position(), i ) );
        }
    }
    std::sort( sorted.begin(), sorted.end() );

    uint8_t new_pos = 1;
    for ( size_t i = 0, n = sorted.size(); i < n; ++i ) {
        if ( new_pos == position ) {
            ++new_pos;
        }
        m_banks[GetLowByte( sorted[i] )]->SetPosition( new_pos );
        ++new_pos;
    }
    m_banks[bnum]->SetPosition( position );

    return SA_OK;
}

SaErrorT cFumi::GetAutoRollbackDisabled( SaHpiBoolT& disable ) const
{
    if ( ( m_rec.Capability & SAHPI_FUMI_CAP_AUTOROLLBACK ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    disable = m_auto_rb_disabled;

    return SA_OK;
}

SaErrorT cFumi::SetAutoRollbackDisabled( SaHpiBoolT disable )
{
    if ( ( m_rec.Capability & SAHPI_FUMI_CAP_AUTOROLLBACK ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }
    if ( ( m_rec.Capability & SAHPI_FUMI_CAP_AUTOROLLBACK_CAN_BE_DISABLED ) == 0 ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    m_auto_rb_disabled = disable;

    return SA_OK;

}

SaErrorT cFumi::StartActivation( SaHpiBoolT logical )
{
    if ( logical == SAHPI_FALSE ) {

        if ( m_banks.size() < 2 ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }

        const size_t n = m_banks.size();
        for ( size_t pos = 1; pos < n; ++pos ) {
            size_t id;
            for ( id = 1; id < n; ++id ) {
                if ( m_banks[id]->Position() == pos ) {
                    const SaHpiFumiBankStateT st = m_banks[id]->State();
                    if ( ( st == SAHPI_FUMI_BANK_VALID ) || ( st == SAHPI_FUMI_BANK_ACTIVE ) ) {
                        break;
                    }
                }
            }
            if ( id < n ) {
                return m_banks[id]->StartActivation( m_next.pass.activate );
            }
        }

        return SA_ERR_HPI_INVALID_REQUEST;

    } else {
        return m_banks[0]->StartActivation( m_next.pass.activate );
    }
}


// cObject virtual functions
void cFumi::GetNB( std::string& nb ) const
{
    cObject::GetNB( nb );
    nb += "- Test Agent supports creation of a bank with\n";
    nb += "    id == current number of banks.\n";
    nb += "- Test Agent supports removal of a bank with.\n";
    nb += "    id == (current number of banks - 1).\n";
    nb += "- Test Agent does not support Logical Bank (id == 0) removal.\n";
    nb += "- Be careful when removing a bank:\n";
    nb += "-- BankNum in FUMI RDR is not updated in that case.\n";
    nb += "-- Any FUMI API directed to the removed bank will fail.\n";
    nb += "-- Any FUMI asynchronous operation on the bank can fail or cause crash.\n";
}

void cFumi::GetNewNames( cObject::NewNames& names ) const
{
    cObject::GetNewNames( names );
    names.push_back( cBank::classname + "-XXX" );
}

bool cFumi::CreateChild( const std::string& name )
{
    bool rc;

    rc = cObject::CreateChild( name );
    if ( rc ) {
        return true;
    }

    std::string cname;
    SaHpiUint32T id;
    rc = DisassembleNumberedObjectName( name, cname, id );
    if ( !rc ) {
        return false;
    }

    if ( cname != cBank::classname ) {
        return false;
    }

    if ( id != m_banks.size() ) {
        return false;
    }

    m_banks.push_back( new cBank( m_handler, *this, id ) );
    HandleRdrChange( "Rdr.FumiRec.NumBanks" );

    return true;
}

bool cFumi::RemoveChild( const std::string& name )
{
    bool rc;

    rc = cObject::RemoveChild( name );
    if ( rc ) {
        return true;
    }

    std::string cname;
    SaHpiUint32T id;
    rc = DisassembleNumberedObjectName( name, cname, id );
    if ( !rc ) {
        return false;
    }

    if ( ( id + 1 ) != m_banks.size() ) {
        return false;
    }
    // Do not allow removing logical bank
    if ( id == 0 ) {
        return false;
    }

    delete m_banks[id];
    m_banks[id] = 0;
    m_banks.resize( id );

    return true;
}

void cFumi::GetChildren( Children& children ) const
{
    cObject::GetChildren( children );

    ObjectCollector collector( children );
    std::for_each( m_banks.begin(), m_banks.end(), collector );
}

void cFumi::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );
    Structs::GetVars( m_spec_info, vars );
    Structs::GetVars( m_service_impact, vars );

    vars << "AutoRollbackDisabled"
         << dtSaHpiBoolT
         << DATA( m_auto_rb_disabled )
         << VAR_END();
    vars << "Next.Pass.Activate"
         << dtSaHpiBoolT
         << DATA( m_next.pass.activate )
         << VAR_END();
}


// Handling RDR changes
void cFumi::UpdateRdr( const std::string& field_name,
                       SaHpiRdrTypeUnionT& data )
{
    cInstrument::UpdateRdr( field_name, data );

    if ( field_name == "Rdr.FumiRec.NumBanks" ) {
        data.FumiRec.NumBanks = m_banks.size() - 1;
    }
}


}; // namespace TA

