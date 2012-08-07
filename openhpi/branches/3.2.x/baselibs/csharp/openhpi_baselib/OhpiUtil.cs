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
using System.Collections.Generic;
using System.Text;


namespace org {
namespace openhpi {


/**********************************************************
 * OHPI Utility Functions
 *********************************************************/
public static partial class OhpiUtil
{
    /**********************************************************
     * NB: ToXXX throws FormatException if lookup fails
     *********************************************************/


    // For encoding
    private static ASCIIEncoding ascii = new ASCIIEncoding();


    /**********************************************************
     * Check Functions for OHPI Complex Data Types
     *********************************************************/

    /**
     * Check function for OHPI struct oHpiHandlerConfigT
     */
    public static bool Check( oHpiHandlerConfigT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.items == null ) {
            return false;
        }
        foreach ( var kv in x.items ) {
            if ( kv.Key == null ) {
                return false;
            }
            if ( kv.Key.Length != HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH ) {
                return false;
            }
            if ( kv.Value == null ) {
                return false;
            }
            if ( kv.Value.Length != HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH ) {
                return false;
            }
        }
        return true;
    }


    /**********************************************************
     * oHpiHandlerConfigT Helpers
     *********************************************************/
    public static Dictionary<string, string> FromoHpiHandlerConfigT( oHpiHandlerConfigT config )
    {
        var d = new Dictionary<string, string>();
        foreach ( var kv in config.items ) {
            d.Add( ascii.GetString( kv.Key ), ascii.GetString( kv.Value ) );
        }

        return d;
    }

    public static oHpiHandlerConfigT TooHpiHandlerConfigT( Dictionary<string, string> d )
    {
        var config = new oHpiHandlerConfigT();
        config.items = new List< KeyValuePair<byte[], byte[]> >();
        if ( d.Count > 255 ) {
            throw new FormatException();
        }
        foreach ( var kv in d ) {
            int len;
            byte[] encoded;

            if ( kv.Key == null ) {
                throw new FormatException();
            }
            var name = new byte[HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH];
            encoded = ascii.GetBytes( kv.Key );
            len = Math.Min( encoded.Length, HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
            Array.Copy( encoded, name, len );

            if ( kv.Value == null ) {
                throw new FormatException();
            }
            var val = new byte[HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH];
            encoded = ascii.GetBytes( kv.Value );
            len = Math.Min( encoded.Length, HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
            Array.Copy( encoded, val, len );

            config.items.Add( new KeyValuePair<byte[], byte[]>( name, val ) );
        }

        return config;
    }


};


}; // namespace openhpi
}; // namespace org

