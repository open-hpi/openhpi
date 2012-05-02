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

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cFumi
 *************************************************************/
class cFumi : public cInstrument
{
public:

    static const std::string classname;

    explicit cFumi( cResource& resource, SaHpiFumiNumT num );
    virtual ~cFumi();

public:  // HPI interface

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );

private:

    cFumi( const cFumi& );
    cFumi& operator =( const cFumi& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_FUMI;
    }

private: // data

    const SaHpiFumiRecT& m_rec;
};


}; // namespace TA


#endif // FUMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

