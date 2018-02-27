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
import java.util.AbstractMap;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Map.Entry;

import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.OhpiDataTypes.*;


public class OhpiUtil
{
    /**********************************************************
     * NB: ToXXX throws IllegalArgumentException if lookup fails
     *********************************************************/

    // Just to ensure nobody creates it
    private OhpiUtil()
    {
        // empty
    }


    /**********************************************************
     * Check Functions for OHPI Complex Data Types
     *********************************************************/

    /**
     * Check function for OHPI struct oHpiHandlerConfigT
     */
    public static boolean check( oHpiHandlerConfigT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.items == null ) {
            return false;
        }
        for ( Entry<byte[], byte[]> kv : x.items ) {
            byte[] k = kv.getKey();
            if ( k == null ) {
                return false;
            }
            if ( k.length != SAHPI_MAX_TEXT_BUFFER_LENGTH ) {
                return false;
            }
            byte[] v = kv.getValue();
            if ( v == null ) {
                return false;
            }
            if ( v.length != SAHPI_MAX_TEXT_BUFFER_LENGTH ) {
                return false;
            }
        }
        return true;
    }


    /**********************************************************
     * oHpiHandlerConfigT Helpers
     *********************************************************/
    public static Map<String, String> fromoHpiHandlerConfigT( oHpiHandlerConfigT config )
    {
        HashMap<String, String> hm = new HashMap<String, String>();
        for ( Entry<byte[], byte[]> kv : config.items ) {
            try {
                hm.put( new String( kv.getKey(), "US-ASCII" ),
                        new String( kv.getValue(), "US-ASCII" ) );
            } catch ( UnsupportedEncodingException e ) {
                throw new IllegalArgumentException();
            }
        }

        return hm;
    }

    public static oHpiHandlerConfigT tooHpiHandlerConfigT( Map<String, String> hm )
    {
        oHpiHandlerConfigT config = new oHpiHandlerConfigT();
        config.items = new LinkedList< Entry<byte[], byte[]> >();
        if ( hm.size() > 255 ) {
            throw new IllegalArgumentException();
        }
        for ( Entry<String, String> kv : hm.entrySet() ) {
            int len;
            byte[] encoded;

            String k = kv.getKey();
            if ( k == null ) {
                throw new IllegalArgumentException();
            }
            byte[] name = new byte[SAHPI_MAX_TEXT_BUFFER_LENGTH];
            try {
                encoded = k.getBytes( "US-ASCII" );
            } catch ( UnsupportedEncodingException e ) {
                throw new IllegalArgumentException();
            }
            len = Math.min( encoded.length, SAHPI_MAX_TEXT_BUFFER_LENGTH );
            System.arraycopy( encoded, 0, name, 0, len );

            String v = kv.getValue();
            if ( v == null ) {
                throw new IllegalArgumentException();
            }
            byte[] val = new byte[SAHPI_MAX_TEXT_BUFFER_LENGTH];
            try {
                encoded = v.getBytes( "US-ASCII" );
            } catch ( UnsupportedEncodingException e ) {
                throw new IllegalArgumentException();
            }
            len = Math.min( encoded.length, SAHPI_MAX_TEXT_BUFFER_LENGTH );
            System.arraycopy( encoded, 0, val, 0, len );

            config.items.add( new AbstractMap.SimpleEntry<byte[], byte[]>( name, val ) );
        }

        return config;
    }


};

