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

#include "annunciator.h"
#include "codec.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultAnnunciatorRec( SaHpiAnnunciatorNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiAnnunciatorRecT& rec = data.AnnunciatorRec;

    rec.AnnunciatorNum  = num;
    rec.AnnunciatorType = SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE;
    rec.ModeReadOnly    = SAHPI_FALSE;
    rec.MaxConditions   = 0;
    rec.Oem             = 0;

    return data;
}


/**************************************************************
 * class cAnnunciator
 *************************************************************/
const std::string cAnnunciator::classname( "ann" );

cAnnunciator::cAnnunciator( cResource& resource, SaHpiAnnunciatorNumT num )
    : cInstrument( resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_ANNUNCIATOR_RDR,
                   MakeDefaultAnnunciatorRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.AnnunciatorRec )
{
    // empty
}

cAnnunciator::~cAnnunciator()
{
    // empty
}


// HPI interface
// TODO

// cObject virtual functions
void cAnnunciator::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );

    // TODO
}


}; // namespace TA

