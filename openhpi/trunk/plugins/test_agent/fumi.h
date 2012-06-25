/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTFUMILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef FUMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define FUMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>
#include <vector>

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cFumi
 *************************************************************/
class cBank;

class cFumi : public cInstrument
{
public:

    static const std::string classname;

    explicit cFumi( cHandler& handler, cResource& resource, SaHpiFumiNumT num );
    virtual ~cFumi();

public:

    SaHpiFumiCapabilityT Capabilities() const;
    bool CheckProtocol( const std::string& proto ) const;
    cBank * GetBank( SaHpiBankNumT bnum ) const;
    bool IsAutoRollbackDisabled() const;
    bool ShallNextActivationPass() const;
    void PostEvent( SaHpiBankNumT bnum, SaHpiFumiUpgradeStatusT status );

public:  // HPI interface

    SaErrorT GetSpecInfo( SaHpiFumiSpecInfoT& specinfo ) const;
    SaErrorT GetServiceImpact( SaHpiFumiServiceImpactDataT& data ) const;
    SaErrorT SetBootOrder( SaHpiBankNumT bnum, SaHpiUint32T position );
    SaErrorT GetAutoRollbackDisabled( SaHpiBoolT& disable ) const;
    SaErrorT SetAutoRollbackDisabled( SaHpiBoolT disable );
    SaErrorT StartActivation( SaHpiBoolT logical );

protected: // cObject virtual functions

    virtual void GetNB( std::string& nb ) const;
    virtual void GetNewNames( cObject::NewNames& names ) const;
    virtual bool CreateChild( const std::string& name );
    virtual bool RemoveChild( const std::string& name );
    virtual void GetChildren( cObject::Children& children ) const;
    virtual void GetVars( cVars& vars );

private: // Handling RDR changes

    virtual void UpdateRdr( const std::string& field_name,
                            SaHpiRdrTypeUnionT& data );

private:

    cFumi( const cFumi& );
    cFumi& operator =( const cFumi& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_FUMI;
    }

private: // data

    const SaHpiFumiRecT&        m_rec;
    SaHpiFumiSpecInfoT          m_spec_info;
    SaHpiFumiServiceImpactDataT m_service_impact;
    SaHpiBoolT                  m_auto_rb_disabled;
    std::vector<cBank*>         m_banks;

    struct
    {
        struct
        {
            SaHpiBoolT activate;
        } pass;
    } m_next;
};


}; // namespace TA


#endif // FUMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

