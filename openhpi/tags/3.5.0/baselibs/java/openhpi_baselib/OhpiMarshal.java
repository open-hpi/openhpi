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

import java.util.AbstractMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map.Entry;

import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.OhpiDataTypes.*;


/**********************************************************
 * OHPI Marshal
 *********************************************************/
class OhpiMarshal extends HpiMarshalGen
{

    /**********************************************************
     * Marshal: For OHPI Simple Data Types
     *********************************************************/
    public void marshaloHpiHandlerIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshaloHpiGlobalParamTypeT( long x )
    {
        marshalEnum( x );
    }


    /**********************************************************
     * Marshal: For OHPI Structs and Unions
     *********************************************************/
    public void marshaloHpiHandlerConfigT( oHpiHandlerConfigT x )
    {
        marshalSaHpiUint8T( x.items.size() );
        for ( Entry<byte[], byte[]> kv : x.items ) {
            marshalByteArray( kv.getKey(), SAHPI_MAX_TEXT_BUFFER_LENGTH );
            marshalByteArray( kv.getValue(), SAHPI_MAX_TEXT_BUFFER_LENGTH );
        }
    }


    /**********************************************************
     * Demarshal: For OHPI Simple Data Types
     *********************************************************/
    public long demarshaloHpiHandlerIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshaloHpiGlobalParamTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }


    /**********************************************************
     * Demarshal: For OHPI Structs and Unions
     *********************************************************/
    public oHpiHandlerInfoT demarshaloHpiHandlerInfoT()
        throws HpiException
    {
        oHpiHandlerInfoT x = new oHpiHandlerInfoT();

        x.id = demarshaloHpiHandlerIdT();
        x.plugin_name = demarshalByteArray( OPENHPI_MAX_PLUGIN_NAME_LENGTH );
        x.entity_root = demarshalSaHpiEntityPathT();
        x.load_failed = demarshalSaHpiInt32T();

        return x;
    }

    public oHpiHandlerConfigT demarshaloHpiHandlerConfigT()
        throws HpiException
    {
        oHpiHandlerConfigT x = new oHpiHandlerConfigT();

        x.items = new LinkedList< Entry<byte[], byte[]> >();
        long n = demarshalSaHpiUint8T();
        for ( int i = 0; i < n; ++i ) {
            byte[] name = demarshalByteArray( SAHPI_MAX_TEXT_BUFFER_LENGTH );
            byte[] val = demarshalByteArray( SAHPI_MAX_TEXT_BUFFER_LENGTH );
            x.items.add( new AbstractMap.SimpleEntry<byte[], byte[]>( name, val ) );
        }

        return x;
    }


};

