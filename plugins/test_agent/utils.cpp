/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTUTILSLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <stddef.h>
#include <string.h>

#include <algorithm>
#include <cstdio>

#include "utils.h"


namespace TA {


/**************************************************************
 * Entity Path Helpers
 *************************************************************/
void MakeUnspecifiedHpiEntityPath( SaHpiEntityPathT& ep )
{
    for ( size_t i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
        ep.Entry[i].EntityType     = SAHPI_ENT_UNSPECIFIED;
        ep.Entry[i].EntityLocation = 0;
    }
}


/**************************************************************
 * Text Buffer Helpers
 *************************************************************/
void MakeHpiTextBuffer( SaHpiTextBufferT& tb, const char * s )
{
    if ( s ) {
        MakeHpiTextBuffer( tb, s, strlen( s ) );
    }
}

void MakeHpiTextBuffer( SaHpiTextBufferT& tb, const char * s, size_t size )
{
    tb.DataType   = SAHPI_TL_TYPE_TEXT;
    tb.Language   = SAHPI_LANG_ENGLISH;
    if ( s ) {
        tb.DataLength = std::min<size_t>( size, SAHPI_MAX_TEXT_BUFFER_LENGTH );
        if ( tb.DataLength != 0 ) {
            memcpy( tb.Data, s, tb.DataLength );
        } else {
            tb.Data[0] = '\0';
        }
    } else {
        tb.DataLength = 0;
    }
}

void MakeHpiTextBuffer( SaHpiTextBufferT& tb, char c, size_t size )
{
    if ( size > 0 ) {
        tb.DataType   = SAHPI_TL_TYPE_TEXT;
        tb.Language   = SAHPI_LANG_ENGLISH;
        tb.DataLength = std::min<size_t>( size, SAHPI_MAX_TEXT_BUFFER_LENGTH );
        memset( &tb.Data[0], c, tb.DataLength );
    } else {
        tb.DataLength = 0;
    }
}

void FormatHpiTextBuffer( SaHpiTextBufferT& tb, const char * fmt, ... )
{
    va_list ap;

    va_start(ap, fmt );
    vFormatHpiTextBuffer( tb, fmt, ap );
    va_end( ap );
}

void vFormatHpiTextBuffer( SaHpiTextBufferT& tb, const char * fmt, va_list ap )
{
    char * p = reinterpret_cast<char*>( &(tb.Data[0]) );

    tb.DataType   = SAHPI_TL_TYPE_TEXT;
    tb.Language   = SAHPI_LANG_ENGLISH;
    int len = vsnprintf( p, SAHPI_MAX_TEXT_BUFFER_LENGTH, fmt, ap );
    tb.DataLength = ( len < 0 ) ? 0 : (SaHpiUint8T)(len);
}

void AppendToTextBuffer( SaHpiTextBufferT& tb, const SaHpiTextBufferT& appendix )
{
    if ( tb.DataType != appendix.DataType ) {
        return;
    }
    if ( tb.Language != appendix.Language ) {
        return;
    }
    size_t len = std::min<size_t>( SAHPI_MAX_TEXT_BUFFER_LENGTH - tb.DataLength,
                                   appendix.DataLength );
    if ( len > 0 ) {
        memcpy( &tb.Data[tb.DataLength], &appendix.Data[0], len );
    }
    tb.DataLength += len;
}


}; // namespace TA

