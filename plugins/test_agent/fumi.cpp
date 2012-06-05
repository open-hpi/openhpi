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
#include "fumi.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultFumiRec( SaHpiFumiNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiFumiRecT& rec = data.FumiRec;

    rec.Num        = num;
    rec.AccessProt = SAHPI_FUMI_PROT_LOCAL | SAHPI_FUMI_PROT_TFTP;
    rec.Capability = SAHPI_FUMI_CAP_COMPONENTS;
    rec.NumBanks   = 0;
    rec.Oem        = 0;

    return data;
}


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
      m_rec( GetRdr().RdrTypeUnion.FumiRec )
{
    // empty
}

cFumi::~cFumi()
{
    // empty
}


// HPI interface
// TODO hpi interface

// cObject virtual functions
void cFumi::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );

    // TODO getvars
}


}; // namespace TA

