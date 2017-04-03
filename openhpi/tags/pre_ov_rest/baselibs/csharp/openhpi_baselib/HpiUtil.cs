/*      -*- c# -*-
 *
 * Copyright (C) 2012, Pigeon Point Systems
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

using System;
using System.Text;


namespace org {
namespace openhpi {


/**********************************************************
 * HPI Utility Functions
 *********************************************************/
public static partial class HpiUtil
{
    /**********************************************************
     * NB: ToXXX throws FormatException if lookup fails
     *********************************************************/


    // For encoding
    private static ASCIIEncoding ascii = new ASCIIEncoding();


    /**********************************************************
     * Text Buffer Helpers
     *********************************************************/
    public static string FromSaHpiTextBufferT( SaHpiTextBufferT tb )
    {
        // NB: Only BCD+/ASCII6/ASCII(Eng)/ are now supported.
        // TODO implement further

        bool ok = false;
        ok = ok || ( tb.DataType == HpiConst.SAHPI_TL_TYPE_BCDPLUS );
        ok = ok || ( tb.DataType == HpiConst.SAHPI_TL_TYPE_ASCII6 );
        bool eng = ( tb.Language == HpiConst.SAHPI_LANG_ENGLISH );
        ok = ok || ( ( tb.DataType == HpiConst.SAHPI_TL_TYPE_TEXT ) && eng );

        string s = null;
        if ( ok ) {
            char[] data = new char[tb.DataLength];
            Array.Copy( tb.Data, data, tb.DataLength );
            s = new string( data, 0, unchecked( (int)tb.DataLength ) );
        } else {
            throw new FormatException();
        }

        return s;
    }

    public static SaHpiTextBufferT ToSaHpiTextBufferT( string s )
    {
        // NB: Only BCD+/ASCII6/ASCII(Eng)/ are now supported.
        // TODO implement further

        SaHpiTextBufferT tb = new SaHpiTextBufferT();
        tb.DataType   = HpiConst.SAHPI_TL_TYPE_TEXT;
        tb.Language   = HpiConst.SAHPI_LANG_ENGLISH;
        tb.DataLength = 0;
        tb.Data = new byte[HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH];

        byte[] encoded = ascii.GetBytes( s );
        tb.DataLength = Math.Min( encoded.Length, HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
        Array.Copy( encoded, tb.Data, tb.DataLength );

        return tb;
    }


    /**********************************************************
     * Entity Path Helpers
     *********************************************************/
    public static SaHpiEntityPathT MakeUnspecifiedSaHpiEntityPathT()
    {
        SaHpiEntityPathT ep = new SaHpiEntityPathT();
        ep.Entry = new SaHpiEntityT[HpiConst.SAHPI_MAX_ENTITY_PATH];
        for ( int i = 0; i < HpiConst.SAHPI_MAX_ENTITY_PATH; ++i ) {
            ep.Entry[i] = new SaHpiEntityT();
            ep.Entry[i].EntityType = HpiConst.SAHPI_ENT_UNSPECIFIED;
            ep.Entry[i].EntityLocation = 0;
        }
        return ep;
    }

    public static SaHpiEntityPathT MakeRootSaHpiEntityPathT()
    {
        SaHpiEntityPathT ep = MakeUnspecifiedSaHpiEntityPathT();
        ep.Entry[0].EntityType = HpiConst.SAHPI_ENT_ROOT;
        ep.Entry[0].EntityLocation = 0;
        return ep;
    }

    public static SaHpiEntityPathT CloneSaHpiEntityPathT( SaHpiEntityPathT ep )
    {
        if ( ep == null ) {
            return null;
        }

        SaHpiEntityPathT ep2 = new SaHpiEntityPathT();
        ep2.Entry = new SaHpiEntityT[HpiConst.SAHPI_MAX_ENTITY_PATH];
        for ( int i = 0; i < HpiConst.SAHPI_MAX_ENTITY_PATH; ++i ) {
            ep2.Entry[i] = new SaHpiEntityT();
            ep2.Entry[i].EntityType = ep.Entry[i].EntityType;
            ep2.Entry[i].EntityLocation = ep.Entry[i].EntityLocation;
        }
        return ep2;
    }

    private static int GetSaHpiEntityPathLength( SaHpiEntityPathT ep )
    {
        int i;
        for ( i = 0; i < HpiConst.SAHPI_MAX_ENTITY_PATH; ++i ) {
            if ( ( ep.Entry[i].EntityType == HpiConst.SAHPI_ENT_ROOT ) ) {
                if ( ep.Entry[i].EntityLocation == 0 ) {
                    break;
                }
            }
        }
        return i;
    }

    public static string FromSaHpiEntityPathT( SaHpiEntityPathT ep )
    {
        StringBuilder b = new StringBuilder();
        int l = GetSaHpiEntityPathLength( ep );
        for ( int i = ( l - 1 ); i >= 0; --i ) {
            b.AppendFormat( "{{{0},{1}}}",
                            FromSaHpiEntityTypeT( ep.Entry[i].EntityType ),
                            ep.Entry[i].EntityLocation );
        }
        return b.ToString();
    }

    public static SaHpiEntityPathT ToSaHpiEntityPathT( string s )
    {
        SaHpiEntityPathT ep = MakeRootSaHpiEntityPathT();
        if ( s.Length == 0 ) {
            return ep;
        }

        if ( s.IndexOf( '{' ) != 0 ) {
            throw new FormatException();
        }

        int i = 0;

        string[] parts = s.Substring( 1 ).Split( new Char[] { '{' } );
        Array.Reverse( parts );
        foreach ( string part in parts ) {
            if ( !part.EndsWith( "}" ) ) {
                throw new FormatException();
            }
            string[] et_el = part.Substring( 0, part.Length - 1 ).Split( new Char[] { ',' } );
            if ( et_el.Length != 2 ) {
                throw new FormatException();
            }
            ep.Entry[i].EntityType = ToSaHpiEntityTypeT( et_el[0] );
            try {
                ep.Entry[i].EntityLocation = UInt32.Parse( et_el[1] );
            } catch ( Exception ) {
                throw new FormatException();
            }
            ++i;
            if ( i == HpiConst.SAHPI_MAX_ENTITY_PATH ) {
                break;
            }
        }
        if ( i < HpiConst.SAHPI_MAX_ENTITY_PATH ) {
            ep.Entry[i].EntityType = HpiConst.SAHPI_ENT_ROOT;
            ep.Entry[i].EntityLocation = 0;
        }

        return ep;
    }
};


}; // namespace openhpi
}; // namespace org

