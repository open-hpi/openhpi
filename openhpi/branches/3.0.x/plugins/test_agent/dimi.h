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

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cDimi
 *************************************************************/
class cDimi : public cInstrument
{
public:

    static const std::string classname;

    explicit cDimi( cResource& resource, SaHpiDimiNumT num );
    virtual ~cDimi();

public:  // HPI interface

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );

private:

    cDimi( const cDimi& );
    cDimi& operator =( const cDimi& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_DIMI;
    }

private: // data

    const SaHpiDimiRecT& m_rec;
};


}; // namespace TA


#endif // DIMI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

