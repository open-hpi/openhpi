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
#include "dimi.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultDimiRec( SaHpiDimiNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiDimiRecT& rec = data.DimiRec;

    rec.DimiNum = num;
    rec.Oem         = 0;

    return data;
}


/**************************************************************
 * class cDimi
 *************************************************************/
const std::string cDimi::classname( "dimi" );

cDimi::cDimi( cResource& resource, SaHpiDimiNumT num )
    : cInstrument( resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_DIMI_RDR,
                   MakeDefaultDimiRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.DimiRec )
{
    // empty
}

cDimi::~cDimi()
{
    // empty
}


// HPI interface
// TODO

// cObject virtual functions
void cDimi::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );

    // TODO
}


}; // namespace TA

