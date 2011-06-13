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

#ifndef WATCHDOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define WATCHDOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cWatchdog
 *************************************************************/
class cWatchdog : public cInstrument
{
public:

    static const std::string classname;

    explicit cWatchdog( cResource& resource, SaHpiWatchdogNumT num );
    virtual ~cWatchdog();

public:  // HPI interface

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );

private:

    cWatchdog( const cWatchdog& );
    cWatchdog& operator =( const cWatchdog& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_WATCHDOG;
    }

private: // data

    const SaHpiWatchdogRecT& m_rec;
};


}; // namespace TA


#endif // WATCHDOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

