/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTANNUNCIATORLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef ANNUNCIATOR_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define ANNUNCIATOR_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cAnnunciator
 *************************************************************/
class cAnnunciator : public cInstrument
{
public:

    static const std::string classname;

    explicit cAnnunciator( cResource& resource, SaHpiAnnunciatorNumT num );
    virtual ~cAnnunciator();

public:  // HPI interface

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );

private:

    cAnnunciator( const cAnnunciator& );
    cAnnunciator& operator =( const cAnnunciator& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_ANNUNCIATOR;
    }

private: // data

    const SaHpiAnnunciatorRecT& m_rec;
};


}; // namespace TA


#endif // ANNUNCIATOR_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

