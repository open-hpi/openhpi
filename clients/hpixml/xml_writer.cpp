/*      -*- c++ -*-
 *
 * Copyright (c) 2011 by Pigeon Point Systems.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Anton Pak <avpak@pigeonpoint.com>
 *
 */

#include <stdarg.h>
#include <stdio.h>

#include "xml_writer.h"


/***************************************************
 * class cXmlWriter
 ***************************************************/
cXmlWriter::cXmlWriter( const char * ns,
                        const char * schema_location,
                        int indent_step,
                        bool use_names )
    : m_ns( ns ),
      m_schema_location( schema_location ),
      m_indent( 0 ),
      m_indent_step( indent_step ),
      m_use_names( use_names )
{
    // empty
}

void cXmlWriter::Begin( const char * name )
{
    printf( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    PrintIndent();
    printf( "<%s"
            " xmlns=\"%s\""
            " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
            " xsi:schemaLocation=\"%s\""
            ">\n",
            name,
            m_ns.c_str(),
            m_schema_location.c_str() );
    m_indent += m_indent_step;
}

void cXmlWriter::End( const char * name )
{
    EndNode( name );
}

void cXmlWriter::Node( const char * name, const char * format, ... )
{
    PrintIndent();
    printf( "<%s>", name );
    va_list ap;
    va_start( ap, format );
    vprintf( format, ap );
    va_end( ap );
    printf( "</%s>\n", name );
}

void cXmlWriter::NodeHex( const char * name, const uint8_t * data, size_t len )
{
    PrintIndent();
    printf( "<%s>", name );
    if ( data ) {
        for ( size_t i = 0; i < len; ++i ) {
            printf( "%02X", data[i] );
        }
    }
    printf( "</%s>\n", name );
}

void cXmlWriter::NodeFlags( const char * name,
                            Flags::Type flags,
                            const Flags::Names * flag_names )
{
    if ( ( !m_use_names ) || ( !flag_names ) ) {
        Node( name, "%lu", (unsigned long)flags );
        return;
    }

    PrintIndent();
    printf( "<%s>", name );
    uint8_t i;
    bool first = true;
    for ( i = 0; i < Flags::Num; ++i ) {
        Flags::Type flag = 1 << i;
        if ( flags & flag ) {
            if ( !first ) {
                printf( " " );
            }
            const char * flag_name = (*flag_names)[i];
            if ( flag_name ) {
                printf( "%s", flag_name );
            } else {
                printf( "%lu", (unsigned long)flag );
            }
            first = false;
        }
    }
    printf( "</%s>\n", name );
}


void cXmlWriter::BeginNode( const char * name )
{
    PrintIndent();
    printf( "<%s>\n", name );
    m_indent += m_indent_step;
}

void cXmlWriter::EndNode( const char * name )
{
    m_indent -= m_indent_step;
    PrintIndent();
    printf( "</%s>\n", name );
}

void cXmlWriter::PrintIndent( void )
{
    for ( int i = 0; i < m_indent; ++i ) {
        putc( ' ', stdout );
    }
}

