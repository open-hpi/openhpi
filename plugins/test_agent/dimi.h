/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTDIMILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef DIMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define DIMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>
#include <vector>

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cDimi
 *************************************************************/
class cTest;

class cDimi : public cInstrument
{
public:

    static const std::string classname;

    explicit cDimi( cHandler& handler, cResource& resource, SaHpiDimiNumT num );
    virtual ~cDimi();

public:

    cTest * GetTest( SaHpiDimiTestNumT tnum ) const;
    void PostEvent( SaHpiDimiTestNumT tnum,
                    SaHpiDimiTestRunStatusT status,
                    SaHpiDimiTestPercentCompletedT progress );

public:  // HPI interface

    SaErrorT GetInfo( SaHpiDimiInfoT& info ) const;

protected: // cObject virtual functions

    virtual void GetNB( std::string& nb ) const;
    virtual void GetNewNames( cObject::NewNames& names ) const;
    virtual bool CreateChild( const std::string& name );
    virtual bool RemoveChild( const std::string& name );
    virtual void GetChildren( cObject::Children& children ) const;

private:

    cDimi( const cDimi& );
    cDimi& operator =( const cDimi& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_DIMI;
    }

    void Update();

private: // data

    const SaHpiDimiRecT& m_rec;
    std::vector<cTest*> m_tests;
    SaHpiUint32T m_update_count;
};


}; // namespace TA


#endif // DIMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

