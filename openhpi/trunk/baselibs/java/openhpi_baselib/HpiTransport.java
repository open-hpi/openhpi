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

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.Socket;

import static org.openhpi.OhpiDataTypes.RPC_SAHPI_NULL;


class HpiTransport
{
    /**********************************************************
     * NB: Assumption
     * We assume that marshal layer puts bytes in little-endian
     * order for RPC request.
     * And remote party defines endian for RPC response.
     *********************************************************/

    // PDU
    private final int  MAX_SIZE         = 0xFFFF;
    private final int  HDR_SIZE         = 12;
    private final int  TYPE_OFF         = 0x00;
    private final int  FLAGS_OFF        = 0x01;
    private final int  RPC_ID_OFF       = 0x04;
    private final int  PAYLOAD_SIZE_OFF = 0x08;
    private final byte TYPE             = 0x01; // MSG
    private final byte FLAGS            = 0x11; // RPC version 1, little-endian
    private final byte ENDIAN_FLAG_MASK = 0x01; // If endian flag is set: little-endian


    private Socket       s;
    private InputStream  in;
    private OutputStream out;
    private byte[]       pdu;
    private int          hpos;
    private int          lpos;


    public boolean open( String host, int port )
    {
        boolean rc;
        try {
            rc = true;
            s   = new Socket( host, port );
            in  = s.getInputStream();
            out = s.getOutputStream();
            pdu = new byte[MAX_SIZE];
            reset();
        } catch ( Exception e ) {
            rc = false;
            in  = null;
            out = null;
            s   = null;
            pdu = null;
        }
        return rc;
    }

    public void close()
    {
        if ( ( s != null ) && ( s.isConnected() ) ) {
            try {
                s.close();
            } catch ( IOException e ) {
                // Catch all IO exceptions
            }
        }
        in  = null;
        out = null;
        s   = null;
        pdu = null;
    }

    public void reset()
    {
        setInt8( TYPE_OFF, TYPE );
        setInt8( FLAGS_OFF, FLAGS );
        setInt32( RPC_ID_OFF, RPC_SAHPI_NULL );
        setInt32( PAYLOAD_SIZE_OFF, 0 );
        lpos = HDR_SIZE;
        hpos = HDR_SIZE;
    }

    public boolean isLittleEndian()
    {
        return ( pdu[FLAGS_OFF] & ENDIAN_FLAG_MASK ) != 0;
    }

    public void putByte( byte x )
    {
        pdu[hpos] = x;
        ++hpos;
    }

    public void putBytes( byte[] bytes, int count )
    {
        System.arraycopy( bytes, 0, pdu, hpos, count );
        hpos += count;
    }

    public byte getByte() throws HpiException
    {
        if ( lpos >= hpos ) {
            throw new HpiException( "Not enough data for demarshal" );
        }
        byte x = pdu[lpos];
        ++lpos;
        return x;
    }

    public void getBytes( int count, byte[] bytes ) throws HpiException
    {
        if ( ( lpos + count ) > hpos ) {
            throw new HpiException( "Not enough data for demarshal" );
        }
        System.arraycopy( pdu, lpos, bytes, 0, count );
        lpos += count;
    }

    public boolean interchange( int rpc_id ) throws HpiException
    {
        setInt32( RPC_ID_OFF, rpc_id );
        setInt32( PAYLOAD_SIZE_OFF, hpos - HDR_SIZE );

        boolean rc = false;
        try {
            out.write( pdu, 0, hpos );

            lpos = HDR_SIZE;
            hpos = 0;
            int need = HDR_SIZE;
            while ( hpos < need ) {
                int got = in.read( pdu, hpos, need - hpos );
                if ( got == -1 ) {
                    break;
                }
                hpos += got;
                if ( hpos == HDR_SIZE ) {
                    int payload_size = getInt32( PAYLOAD_SIZE_OFF );
                    need = HDR_SIZE + payload_size;
                    if ( need > MAX_SIZE ) {
                        throw new HpiException( "Incoming data is too large" );
                    }
                }
            }
            rc = ( hpos == need );
        } catch ( IOException e ) {
        }
        // TODO check id, len, type

        return rc;
    }

    private void setInt8( int off, byte x )
    {
        pdu[off] = x;
    }

    private void setInt32( int off, int x )
    {
        // NB: Request data is always little-endian
        pdu[off]     = (byte)( x );
        pdu[off + 1] = (byte)( x >>> 8 );
        pdu[off + 2] = (byte)( x >>> 16 );
        pdu[off + 3] = (byte)( x >>> 24 );
    }

    private int getInt32( int off )
    {
        int b0, b1, b2, b3;
        if ( isLittleEndian() ) {
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

    private void dumpPdu( String name )
    {
        System.out.printf( "PDU %s\n", name );
        System.out.printf( "  LPOS = %d\n", lpos );
        System.out.printf( "  HPOS = %d\n", hpos );
        for ( int i = 0; i < hpos; ++i ) {
            System.out.printf( "  DATA[%d] = %x\n", i, pdu[i] );
        }
    }
};

