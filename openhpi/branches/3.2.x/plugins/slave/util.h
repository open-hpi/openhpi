/*      -*- linux-c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <glib.h>

#include <SaHpi.h>


namespace Slave {


/**************************************************************
 * Utility Functions
 *************************************************************/

bool IsLeavingEvent( const SaHpiEventT& );

bool IsUpdateEvent( const SaHpiEventT& );

bool IsEntityPathValid( const SaHpiEntityPathT& ep );

bool IsRptEntryValid( const SaHpiRptEntryT& rpte );

bool IsRdrValid( const SaHpiRdrT& rdr );


/**************************************************************
 * Various HPI Data structures translation
 * Slave -> Master
 *************************************************************/
void TranslateEntityPath( SaHpiEntityPathT& ep,
                          const SaHpiEntityPathT& root );

void TranslateDimiTest( SaHpiDimiTestT& dt,
                        const SaHpiEntityPathT& root );

void TranslateFumiServiceImpact( SaHpiFumiServiceImpactDataT& fsi,
                                 const SaHpiEntityPathT& root );

void TranslateEvent( SaHpiEventT& event,
                     SaHpiResourceIdT master_rid );

void TranslateRptEntry( SaHpiRptEntryT& rpte,
                        SaHpiResourceIdT master_rid,
                        const SaHpiEntityPathT& root );

void TranslateRdr( SaHpiRdrT& rdr,
                   const SaHpiEntityPathT& root );

void TranslateRdrs( const GSList * rdr_list,
                    const SaHpiEntityPathT& root );

void TranslateAnnouncement( SaHpiAnnouncementT& a,
                            SaHpiResourceIdT master_rid,
                            const SaHpiEntityPathT& root );


}; // namespace Slave


#endif // __UTIL_H__

