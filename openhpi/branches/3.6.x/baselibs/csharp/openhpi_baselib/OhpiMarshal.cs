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


namespace org {
namespace openhpi {


/**********************************************************
 * OHPI Marshal
 *********************************************************/
internal class OhpiMarshal: HpiMarshalGen
{
    /**********************************************************
     * Marshal: For OHPI Simple Data Types
     *********************************************************/
    public void MarshaloHpiHandlerIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshaloHpiGlobalParamTypeT( long x )
    {
        MarshalEnum( x );
    }

    /**********************************************************
     * Marshal: For OHPI Structs and Unions
     *********************************************************/
    public void MarshaloHpiHandlerConfigT( oHpiHandlerConfigT x )
    {
        MarshalSaHpiUint8T( x.items.Count );
        foreach ( var kv in x.items ) {
            MarshalByteArray( kv.Key, HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
            MarshalByteArray( kv.Value, HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
        }
    }


    /**********************************************************
     * Demarshal: For OHPI Simple Data Types
     *********************************************************/
    public long DemarshaloHpiHandlerIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshaloHpiGlobalParamTypeT()
    {
        return DemarshalEnum();
    }


    /**********************************************************
     * Demarshal: For OHPI Structs and Unions
     *********************************************************/
    public oHpiHandlerInfoT DemarshaloHpiHandlerInfoT()
    {
        oHpiHandlerInfoT x = new oHpiHandlerInfoT();

        x.id = DemarshaloHpiHandlerIdT();
        x.plugin_name = DemarshalByteArray( OhpiConst.MAX_PLUGIN_NAME_LENGTH );
        x.entity_root = DemarshalSaHpiEntityPathT();
        x.load_failed = DemarshalSaHpiInt32T();

        return x;
    }

    public oHpiHandlerConfigT DemarshaloHpiHandlerConfigT()
    {
        oHpiHandlerConfigT x = new oHpiHandlerConfigT();

        x.items = new List< KeyValuePair<byte[], byte[]> >();
        long n = DemarshalSaHpiUint8T();
        for ( int i = 0; i < n; ++i ) {
            byte[] name = DemarshalByteArray( HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
            byte[] val = DemarshalByteArray( HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
            x.items.Add( new KeyValuePair<byte[], byte[]>( name, val ) );
        }

        return x;
    }


};


}; // namespace openhpi
}; // namespace org

