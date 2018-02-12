/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTCONTROLLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
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
#include <string>
#include <vector>

#include "codec.h"
#include "control.h"
#include "structs.h"
#include "utils.h"


namespace TA {

/**************************************************************
 * Helper functions
 *************************************************************/
static const char fillchar = 'X';

static void MakeDefaultControlState( const SaHpiRdrTypeUnionT& data,
                                     SaHpiCtrlStateT& state )
{
    state.Type = SAHPI_CTRL_TYPE_TEXT;
    state.StateUnion.Text = data.CtrlRec.TypeUnion.Text.Default;
}

static SaHpiRdrTypeUnionT MakeDefaultCtrlRec( SaHpiCtrlNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiCtrlRecT& rec    = data.CtrlRec;
    SaHpiCtrlRecTextT& tr = rec.TypeUnion.Text;

    rec.Num                    = num;
    rec.OutputType             = SAHPI_CTRL_GENERIC;
    rec.Type                   = SAHPI_CTRL_TYPE_TEXT;
    tr.MaxChars                = 10;
    tr.MaxLines                = 3;
    tr.Language                = SAHPI_LANG_ENGLISH;
    tr.DataType                = SAHPI_TL_TYPE_TEXT;
    tr.Default.Line            = SAHPI_TLN_ALL_LINES;
    tr.Default.Text.DataType   = tr.DataType;
    tr.Default.Text.Language   = tr.Language;
    tr.Default.Text.DataLength = tr.MaxLines * tr.MaxChars;
    memset( &tr.Default.Text.Data[0], fillchar, SAHPI_MAX_TEXT_BUFFER_LENGTH  );
    rec.DefaultMode.Mode       = SAHPI_CTRL_MODE_AUTO;
    rec.DefaultMode.ReadOnly   = SAHPI_FALSE;
    rec.WriteOnly              = SAHPI_FALSE;
    rec.Oem                    = 0;

    return data;
}


/**************************************************************
 * class cControl
 *************************************************************/
const std::string cControl::classname( "ctrl");
static const std::string line_name( "Line" );

cControl::cControl( cHandler& handler, cResource& resource, SaHpiCtrlNumT num )
    : cInstrument( handler,
                   resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_CTRL_RDR,
                   MakeDefaultCtrlRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.CtrlRec ),
      m_mode( m_rec.DefaultMode.Mode )
{
    MakeDefaultControlState( GetRdr().RdrTypeUnion, m_state );

    if ( m_rec.Type == SAHPI_CTRL_TYPE_TEXT ) {
        size_t nlines = m_rec.TypeUnion.Text.MaxLines;
        size_t nchars = m_rec.TypeUnion.Text.MaxChars;
        m_lines.resize( nlines );
        for ( size_t n = 0; n < nlines; ++n ) {
            MakeHpiTextBuffer( m_lines[n], fillchar, nchars );
        }
    }
}

cControl::~cControl()
{
    // empty
}


// HPI interface
SaErrorT cControl::Get( SaHpiCtrlModeT& mode, SaHpiCtrlStateT& state ) const
{
    if ( m_rec.WriteOnly != SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_CMD;
    }

    mode  = m_mode;

    if ( m_rec.Type == SAHPI_CTRL_TYPE_TEXT ) {
        size_t nlines = m_lines.size();
        size_t n = state.StateUnion.Text.Line;
        state.Type = SAHPI_CTRL_TYPE_TEXT;
        state.StateUnion.Text.Line = n;
        state.StateUnion.Text.Text.DataType = m_rec.TypeUnion.Text.DataType;
        state.StateUnion.Text.Text.Language = m_rec.TypeUnion.Text.Language;
        state.StateUnion.Text.Text.DataLength = 0;
        if ( n == SAHPI_TLN_ALL_LINES ) { // merge all lines
            for ( size_t i = 0; i < nlines; ++i ) {
                AppendToTextBuffer( state.StateUnion.Text.Text, m_lines[i] );
            }
        } else if ( n <= nlines ) {
            state.StateUnion.Text.Text = m_lines[n - 1];
        } else {
            return SA_ERR_HPI_INVALID_DATA;
        }
    } else {
        state = m_state;
    }

    return SA_OK;
}

SaErrorT cControl::Set( SaHpiCtrlModeT mode, const SaHpiCtrlStateT& state )
{
    if ( m_rec.DefaultMode.ReadOnly != SAHPI_FALSE ) {
        if ( m_mode != mode ) {
            return SA_ERR_HPI_READ_ONLY;
        }
    }
    m_mode = mode;
    if ( mode == SAHPI_CTRL_MODE_AUTO ) {
        return SA_OK;
    }

    if ( state.Type != m_rec.Type ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    SaErrorT rv = SA_OK;
    if ( m_rec.Type == SAHPI_CTRL_TYPE_DIGITAL ) {
        rv = CheckStateDigital( state.StateUnion.Digital );
    } else if ( m_rec.Type == SAHPI_CTRL_TYPE_ANALOG ) {
        rv = CheckStateAnalog( state.StateUnion.Analog );
    } else if ( m_rec.Type == SAHPI_CTRL_TYPE_STREAM ) {
        rv = CheckStateStream( state.StateUnion.Stream );
    } else if ( m_rec.Type == SAHPI_CTRL_TYPE_TEXT ) {
        rv = CheckStateText( state.StateUnion.Text );
    }

    if ( rv != SA_OK ) {
        return rv;
    }

    m_state = state;

    if ( m_rec.Type == SAHPI_CTRL_TYPE_TEXT ) {
        size_t n = state.StateUnion.Text.Line;
        if ( n == SAHPI_TLN_ALL_LINES ) {
            size_t nlines = m_lines.size();
            for ( size_t i = 0; i < nlines; ++i ) {
                m_lines[i].DataLength = 0;
            }
            m_lines[0] = state.StateUnion.Text.Text;
        } else {
            m_lines[n - 1] = state.StateUnion.Text.Text;
        }
        NormalizeLines();
    }

    if ( m_rec.Type == SAHPI_CTRL_TYPE_OEM ) {
        // Do not allow override MId
        m_state.StateUnion.Oem.MId = m_rec.TypeUnion.Oem.MId;
    }

    return SA_OK;
}


void cControl::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );
    vars << "Mode"
         << dtSaHpiCtrlModeT
         << DATA( m_mode )
         << VAR_END();
    if ( m_rec.Type == SAHPI_CTRL_TYPE_TEXT ) {
        for ( size_t i = 0, n = m_lines.size(); i < n; ++i ) {
            vars << AssembleNumberedObjectName( line_name, i + 1 )
                 << dtSaHpiTextBufferT
                 << DATA( m_lines[i] )
                 << VAR_END();
        }
    } else {
        Structs::GetVars( m_state, vars );
    }
}

void cControl::AfterVarSet( const std::string& var_name )
{
    cInstrument::AfterVarSet( var_name );

    if ( var_name.find( line_name ) == 0 ) {
        NormalizeLines();
    }
}


// Handling RDR changes
void cControl::UpdateRdr( const std::string& field_name,
                          SaHpiRdrTypeUnionT& data )
{
    cInstrument::UpdateRdr( field_name, data );

    if ( field_name == "Rdr.CtrlRec.Type" ) {
        m_state.Type = data.CtrlRec.Type;
    }
}


SaErrorT cControl::CheckStateDigital( const SaHpiCtrlStateDigitalT& ds ) const
{
    if ( m_state.StateUnion.Digital == SAHPI_CTRL_STATE_ON ) {
        if ( ds == SAHPI_CTRL_STATE_PULSE_ON ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
    }
    if ( m_state.StateUnion.Digital == SAHPI_CTRL_STATE_OFF ) {
        if ( ds == SAHPI_CTRL_STATE_PULSE_OFF ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
    }

    return SA_OK;
}

SaErrorT cControl::CheckStateAnalog( const SaHpiCtrlStateAnalogT& as ) const
{
    SaHpiCtrlStateAnalogT amax = m_rec.TypeUnion.Analog.Max;
    SaHpiCtrlStateAnalogT amin = m_rec.TypeUnion.Analog.Min;
    if ( ( as < amin ) || ( as > amax ) ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    return SA_OK;
}

SaErrorT cControl::CheckStateStream( const SaHpiCtrlStateStreamT& ss ) const
{
    if ( ss.StreamLength > SAHPI_CTRL_MAX_STREAM_LENGTH ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    return SA_OK;
}

SaErrorT cControl::CheckStateText( const SaHpiCtrlStateTextT& ts ) const
{
    const SaHpiCtrlRecTextT& tr = m_rec.TypeUnion.Text;

    if ( tr.MaxLines == 0 ) {
        // Spec does not define this
        return SA_ERR_HPI_INVALID_STATE;
    }
    if ( ts.Line != SAHPI_TLN_ALL_LINES ) {
        if ( ts.Line > tr.MaxLines ) {
            return SA_ERR_HPI_INVALID_DATA;
        }
    }

    if ( ts.Text.DataType != tr.DataType ) {
        return SA_ERR_HPI_INVALID_DATA;
    }
    if ( tr.DataType == SAHPI_TL_TYPE_UNICODE ) {
        if ( ts.Text.Language != tr.Language ) {
            return SA_ERR_HPI_INVALID_DATA;
        }
    }
    if ( tr.DataType == SAHPI_TL_TYPE_TEXT ) {
        if ( ts.Text.Language != tr.Language ) {
            return SA_ERR_HPI_INVALID_DATA;
        }
    }

    return SA_OK;
}

void cControl::NormalizeLines()
{
    size_t nlines = m_lines.size();
    size_t mc = m_rec.TypeUnion.Text.MaxChars;

    size_t n;

    // find long line
    bool found = false;
    for ( n = 0; n < nlines; ++n ) {
        SaHpiTextBufferT& line = m_lines[n];
        if ( line.DataLength > mc ) {
            found = true;
            break;
        }
    }

    // wrap long line to the next lines
    if ( found ) {
        SaHpiTextBufferT& longline = m_lines[n];
        size_t pos = mc;
        ++n;
        for ( ; ( n < nlines ) && ( pos < longline.DataLength ); ++n ) {
            size_t len = std::min( mc, longline.DataLength - pos );
            memcpy( &m_lines[n].Data[0], &longline.Data[pos], len );
            m_lines[n].DataLength = len;
            pos += len;
        }
        longline.DataLength = mc;
    }

    // append spaces to short lines
    for ( n = 0; n < nlines; ++n ) {
        SaHpiTextBufferT& line = m_lines[n];
        if ( line.DataLength >= mc ) {
            continue;
        }
        std::fill( &line.Data[line.DataLength], &line.Data[mc], ' ' );
        line.DataLength = mc;
    }
}

}; // namespace TA

