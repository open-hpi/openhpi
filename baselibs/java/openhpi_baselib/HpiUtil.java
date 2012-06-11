/*      -*- java -*-
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

package org.openhpi;

import java.io.UnsupportedEncodingException;
import java.lang.IllegalArgumentException;
import java.lang.Math;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import static org.openhpi.HpiDataTypes.*;


public class HpiUtil extends HpiUtilGen
{
    /**********************************************************
     * NB: ToXXX throws IllegalArgumentException if lookup fails
     *********************************************************/

    // Just to ensure nobody creates it
    private HpiUtil()
    {
        // empty
    }

    /**********************************************************
     * Text Buffer Helpers
     *********************************************************/
    public static String fromSaHpiTextBufferT( SaHpiTextBufferT tb )
    {
        // NB: Only BCD+/ASCII6/ASCII(Eng)/ are now supported.
        // TODO implement further

        boolean ok = false;
        ok = ok || ( tb.DataType == SAHPI_TL_TYPE_BCDPLUS );
        ok = ok || ( tb.DataType == SAHPI_TL_TYPE_ASCII6 );
        boolean eng = ( tb.Language == SAHPI_LANG_ENGLISH );
        ok = ok || ( ( tb.DataType == SAHPI_TL_TYPE_TEXT ) && eng );

        String s = null;
        if ( ok ) {
            try {
                s = new String( tb.Data, 0, (int)tb.DataLength, "US-ASCII" );
            } catch ( UnsupportedEncodingException e ) {
                throw new IllegalArgumentException();
            }
        } else {
            throw new IllegalArgumentException();
        }

        return s;
    }

    public static SaHpiTextBufferT toSaHpiTextBufferT( String s )
    {
        // NB: Only BCD+/ASCII6/ASCII(Eng)/ are now supported.
        // TODO implement further

        SaHpiTextBufferT tb = new SaHpiTextBufferT();
        tb.DataType   = SAHPI_TL_TYPE_TEXT;
        tb.Language   = SAHPI_LANG_ENGLISH;
        tb.DataLength = 0;
        tb.Data = new byte[(int)SAHPI_MAX_TEXT_BUFFER_LENGTH];

        try {
            byte[] bytes = s.getBytes( "US-ASCII" );
            tb.DataLength = Math.min( (long)bytes.length, SAHPI_MAX_TEXT_BUFFER_LENGTH );
            System.arraycopy( bytes, 0, tb.Data, 0, (int)(tb.DataLength) );
        } catch ( UnsupportedEncodingException e ) {
        }

        return tb;
    }


    /**********************************************************
     * Entity Path Helpers
     *********************************************************/
    public static SaHpiEntityPathT makeUnspecifiedSaHpiEntityPathT()
    {
        SaHpiEntityPathT ep = new SaHpiEntityPathT();
        ep.Entry = new SaHpiEntityT[(int)SAHPI_MAX_ENTITY_PATH];
        for ( int i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
            ep.Entry[i] = new SaHpiEntityT();
            ep.Entry[i].EntityType = SAHPI_ENT_UNSPECIFIED;
            ep.Entry[i].EntityLocation = 0;
        }
        return ep;
    }

    public static SaHpiEntityPathT makeRootSaHpiEntityPathT()
    {
        SaHpiEntityPathT ep = makeUnspecifiedSaHpiEntityPathT();
        ep.Entry[0].EntityType = SAHPI_ENT_ROOT;
        ep.Entry[0].EntityLocation = 0;
        return ep;
    }

    public static SaHpiEntityPathT cloneSaHpiEntityPathT( SaHpiEntityPathT ep )
    {
        if ( ep == null ) {
            return null;
        }

        SaHpiEntityPathT ep2 = new SaHpiEntityPathT();
        ep2.Entry = new SaHpiEntityT[(int)SAHPI_MAX_ENTITY_PATH];
        for ( int i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
            ep2.Entry[i] = new SaHpiEntityT();
            ep2.Entry[i].EntityType = ep.Entry[i].EntityType;
            ep2.Entry[i].EntityLocation = ep.Entry[i].EntityLocation;
        }
        return ep2;
    }

    private static int getSaHpiEntityPathLength( SaHpiEntityPathT ep )
    {
        int i;
        for ( i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
            if ( ( ep.Entry[i].EntityType == SAHPI_ENT_ROOT ) ) {
                if ( ep.Entry[i].EntityLocation == 0 ) {
                    break;
                }
            }
        }
        return i;
    }

    public static String fromSaHpiEntityPathT( SaHpiEntityPathT ep )
    {
        StringBuilder b = new StringBuilder();
        int l = getSaHpiEntityPathLength( ep );
        for ( int i = ( l - 1 ); i >= 0; --i ) {
            String s = String.format("{%s,%d}",
                                     fromSaHpiEntityTypeT( ep.Entry[i].EntityType ),
                                     ep.Entry[i].EntityLocation );
            b.append( s );
        }
        return b.toString();
    }

    public static SaHpiEntityPathT toSaHpiEntityPathT( String s )
    {
        SaHpiEntityPathT ep = makeRootSaHpiEntityPathT();
        if ( s.length() == 0 ) {
            return ep;
        }

        if ( s.indexOf( '{' ) != 0 ) {
            throw new IllegalArgumentException();
        }

        int i = 0;

        List<String> parts = Arrays.asList( s.substring( 1 ).split( "\\{" ) );
        Collections.reverse( parts );
        for ( String part : parts ) {
            if ( !part.endsWith( "}" ) ) {
                throw new IllegalArgumentException();
            }
            String[] et_el = part.substring( 0, part.length() - 1 ).split( "," );
            if ( et_el.length != 2 ) {
                throw new IllegalArgumentException();
            }
            ep.Entry[i].EntityType = toSaHpiEntityTypeT( et_el[0] );
            try {
                ep.Entry[i].EntityLocation = Integer.parseInt( et_el[1] );
            } catch ( NumberFormatException e ) {
                throw new IllegalArgumentException();
            }
            ++i;
            if ( i == SAHPI_MAX_ENTITY_PATH ) {
                break;
            }
        }
        if ( i < SAHPI_MAX_ENTITY_PATH ) {
            ep.Entry[i].EntityType = SAHPI_ENT_ROOT;
            ep.Entry[i].EntityLocation = 0;
        }

        return ep;
    }

};

