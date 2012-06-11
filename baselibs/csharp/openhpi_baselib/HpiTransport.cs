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
using System.Net;
using System.Net.Sockets;


namespace org {
namespace openhpi {


internal class HpiTransport
{
    /**********************************************************
     * NB: Assumption
     * We assume that marshal layer puts bytes in little-endian
     * order for RPC request.
     * And remote party defines endian for RPC response.
     *********************************************************/

    // PDU
    private const int  MAX_SIZE          = 0xFFFF;
    private const int  HDR_SIZE          = 12;
    private const int  TYPE_OFF          = 0x00;
    private const int  FLAGS_OFF         = 0x01;
    private const int  RPC_ID_OFF        = 0x04;
    private const int  PAYLOAD_SIZE_OFF  = 0x08;
    private const byte TYPE              = 0x01; // MSG
    private const byte FLAGS             = 0x11; // RPC version 1, little-endian
    private const byte ENDIAN_FLAG_MASK  = 0x01; // If endian flag is set: little-endian

    private Socket s;
    private byte[] pdu;
    private int    hpos;
    private int    lpos;


    public bool Open( string host, int port )
    {
        IPAddress[] addresses = null;
        s = null;
        try {
            addresses = Dns.GetHostAddresses( host );
        } catch ( SocketException ) {
            addresses = null;
        }
        if ( addresses == null ) {
            return false;
        }

        foreach ( var addr in addresses ) {
            try {
                s = new Socket( addr.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
                s.Connect( addr, port );
            } catch ( SocketException ) {
                s = null;
            }
            if ( s != null ) {
                break;
            }
        }
        if ( s == null ) {
            return false;
        }

        pdu = new byte[MAX_SIZE];
        Reset();

        return true;
    }

    public void Close()
    {
        if ( ( s != null ) && s.Connected ) {
            s.Shutdown( SocketShutdown.Both );
            s.Close();
        }
        s   = null;
        pdu = null;
    }

    public void Reset()
    {
        SetUint8( TYPE_OFF, TYPE );
        SetUint8( FLAGS_OFF, FLAGS );
        SetInt32( RPC_ID_OFF, OhpiConst.RPC_SAHPI_NULL  );
        SetInt32( PAYLOAD_SIZE_OFF, 0 );
        lpos = HDR_SIZE;
        hpos = HDR_SIZE;
    }

    public bool IsLittleEndian()
    {
        return ( pdu[FLAGS_OFF] & ENDIAN_FLAG_MASK ) != 0;
    }

    public void PutByte( byte x )
    {
        pdu[hpos] = x;
        ++hpos;
    }

    public void PutBytes( byte[] bytes, int count )
    {
        Array.Copy( bytes, 0, pdu, hpos, count );
        hpos += count;
    }

    public byte GetByte()
    {
        if ( lpos >= hpos ) {
            throw new HpiException( "Not enough data for demarshal" );
        }
        byte x = pdu[lpos];
        ++lpos;
        return x;
    }

    public void GetBytes( int count, byte[] bytes )
    {
        if ( ( lpos + count ) > hpos ) {
            throw new HpiException( "Not enough data for demarshal" );
        }
        Array.Copy( pdu, lpos, bytes, 0, count );
        lpos += count;
    }

    public bool Interchange( int rpc_id )
    {
        SetInt32( RPC_ID_OFF, rpc_id );
        SetInt32( PAYLOAD_SIZE_OFF, hpos - HDR_SIZE );

        bool rc = false;
        try {
            // TODO shall we check the byte count sent?
            s.Send( pdu, 0, hpos, SocketFlags.None );

            lpos = HDR_SIZE;
            hpos = 0;
            int need = HDR_SIZE;
            while ( hpos < need ) {
                int got = s.Receive( pdu, hpos, need - hpos, SocketFlags.None );
                if ( got <= 0 ) {
                    break;
                }
                hpos += got;
                if ( hpos == HDR_SIZE ) {
                    int payload_size = GetInt32( PAYLOAD_SIZE_OFF );
                    need = HDR_SIZE + payload_size;
                    if ( need > MAX_SIZE ) {
                        throw new HpiException( "Incoming data is too large" );
                    }
                }
            }
            rc = ( hpos == need );
//DumpPdu( "INTERCHANGE2" );
        } catch ( SocketException ) {
//DumpPdu( "INTERCHANGE3" );
        }

        // TODO check id, len, type

        return rc;
    }

    private void SetUint8( int off, byte x )
    {
        pdu[off] = x;
    }

    private void SetInt32( int off, int x )
    {
        // NB: Request data is always little-endian
        pdu[off]     = (byte)( x );
        pdu[off + 1] = (byte)( x >> 8 );
        pdu[off + 2] = (byte)( x >> 16 );
        pdu[off + 3] = (byte)( x >> 24 );
    }

    private int GetInt32( int off )
    {
        int b0, b1, b2, b3;
        if ( IsLittleEndian() ) {
            b0 = (int)(pdu[off]) & 0xFF;
            b1 = (int)(pdu[off + 1]) & 0xFF;
            b2 = (int)(pdu[off + 2]) & 0xFF;
            b3 = (int)(pdu[off + 3]) & 0xFF;
        } else {
            b3 = (int)(pdu[off]) & 0xFF;
            b2 = (int)(pdu[off + 1]) & 0xFF;
            b1 = (int)(pdu[off + 2]) & 0xFF;
            b0 = (int)(pdu[off + 3]) & 0xFF;
        }
        return ( b3 << 24 ) | ( b2 << 16 ) | ( b1 << 8 ) | b0;
    }

    private void DumpPdu( string name )
    {
        Console.WriteLine( "PDU {0}", name );
        Console.WriteLine( "  LPOS = {0}", lpos );
        Console.WriteLine( "  HPOS = {0}", hpos );
        for ( int i = 0; i < hpos; ++i ) {
            Console.WriteLine( "  DATA[{0}] = {1:x}", i, pdu[i] );
        }
    }
};


}; // namespace openhpi
}; // namespace org

