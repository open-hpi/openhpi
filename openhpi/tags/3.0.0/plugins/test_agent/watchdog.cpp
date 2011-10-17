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

#include <string>

#include "codec.h"
#include "watchdog.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultWatchdogRec( SaHpiWatchdogNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiWatchdogRecT& rec = data.WatchdogRec;

    rec.WatchdogNum = num;
    rec.Oem         = 0;

    return data;
}


/**************************************************************
 * class cWatchdog
 *************************************************************/
const std::string cWatchdog::classname( "wdt" );

cWatchdog::cWatchdog( cResource& resource, SaHpiWatchdogNumT num )
    : cInstrument( resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_WATCHDOG_RDR,
                   MakeDefaultWatchdogRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.WatchdogRec )
{
    // empty
}

cWatchdog::~cWatchdog()
{
    // empty
}


// HPI interface
// TODO

// cObject virtual functions
void cWatchdog::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );

    // TODO
}


}; // namespace TA

