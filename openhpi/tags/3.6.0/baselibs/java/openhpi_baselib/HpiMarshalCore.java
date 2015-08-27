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


class HpiMarshalCore
{
    private HpiTransport transport;

    private final int BBUF_SIZE = 8;
    private final int LBUF_SIZE = 8;
    private byte[] bbuf;
    private long[] lbuf;
    private boolean little_endian_demarshal;


    public boolean open( String host, int port )
    {
        transport = new HpiTransport();
        boolean rc = transport.open( host, port );
        if ( rc ) {
            bbuf = new byte[BBUF_SIZE];
            lbuf = new long[LBUF_SIZE];
        }
        return rc;
    }

    public void close()
    {
        transport.close();
        bbuf = null;
        lbuf = null;
    }

    public void reset()
    {
        transport.reset();
    }

    public boolean interchange( int rpc_id ) throws HpiException
    {
        boolean rc = transport.interchange( rpc_id );
        if ( rc ) {
            little_endian_demarshal = transport.isLittleEndian();
        }
        return rc;
    }


    /**********************************************************
     * Marshal: For HPI Basic Data Types
     *********************************************************/
    public void marshalSaHpiUint8T( long x )
    {
        transport.putByte( (byte)( x ) );
    }

    public void marshalSaHpiUint16T( long x )
    {
        bbuf[0] = (byte)( x );
        bbuf[1] = (byte)( x >>> 8 );
        transport.putBytes( bbuf, 2 );
    }

    public void marshalSaHpiUint32T( long x )
    {
        bbuf[0] = (byte)( x );
        bbuf[1] = (byte)( x >>> 8 );
        bbuf[2] = (byte)( x >>> 16 );
        bbuf[3] = (byte)( x >>> 24 );
        transport.putBytes( bbuf, 4 );
    }

    public void marshalSaHpiUint64T( long x )
    {
        bbuf[0] = (byte)( x );
        bbuf[1] = (byte)( x >>> 8 );
        bbuf[2] = (byte)( x >>> 16 );
        bbuf[3] = (byte)( x >>> 24 );
        bbuf[4] = (byte)( x >>> 32 );
        bbuf[5] = (byte)( x >>> 40 );
        bbuf[6] = (byte)( x >>> 48 );
        bbuf[7] = (byte)( x >>> 56 );
        transport.putBytes( bbuf, 8 );
    }

    public void marshalSaHpiInt8T( long x )
    {
        transport.putByte( (byte)( x ) );
    }

    public void marshalSaHpiInt16T( long x )
    {
        bbuf[0] = (byte)( x );
        bbuf[1] = (byte)( x >>> 8 );
        transport.putBytes( bbuf, 2 );
    }

    public void marshalSaHpiInt32T( long x )
    {
        bbuf[0] = (byte)( x );
        bbuf[1] = (byte)( x >>> 8 );
        bbuf[2] = (byte)( x >>> 16 );
        bbuf[3] = (byte)( x >>> 24 );
        transport.putBytes( bbuf, 4 );
    }

    public void marshalSaHpiInt64T( long x )
    {
        bbuf[0] = (byte)( x );
        bbuf[1] = (byte)( x >>> 8 );
        bbuf[2] = (byte)( x >>> 16 );
        bbuf[3] = (byte)( x >>> 24 );
        bbuf[4] = (byte)( x >>> 32 );
        bbuf[5] = (byte)( x >>> 40 );
        bbuf[6] = (byte)( x >>> 48 );
        bbuf[7] = (byte)( x >>> 56 );
        transport.putBytes( bbuf, 8 );
    }

    public void marshalEnum( long x )
    {
        marshalSaHpiInt32T( x );
    }

    public void marshalSaHpiFloat64T( double x )
    {
        marshalSaHpiInt64T( Double.doubleToLongBits( x ) );
    }

    public void marshalByteArray( byte[] x, int count )
    {
        transport.putBytes( x, count );
    }


    /**********************************************************
     * Demarshal: For HPI Basic Data Types
     *********************************************************/
    public long demarshalSaHpiUint8T()
        throws HpiException
    {
        return transport.getByte() & 0xFFL;
    }

    public long demarshalSaHpiUint16T()
        throws HpiException
    {
        transport.getBytes( 2, bbuf );
        if ( little_endian_demarshal ) {
            lbuf[0] = bbuf[0] & 0xFFL;
            lbuf[1] = bbuf[1] & 0xFFL;
        } else {
            lbuf[0] = bbuf[1] & 0xFFL;
            lbuf[1] = bbuf[0] & 0xFFL;
        }
        return ( lbuf[1] << 8 ) | lbuf[0];
    }

    public long demarshalSaHpiUint32T()
        throws HpiException
    {
        transport.getBytes( 4, bbuf );
        if ( little_endian_demarshal ) {
            lbuf[0] = bbuf[0] & 0xFFL;
            lbuf[1] = bbuf[1] & 0xFFL;
            lbuf[2] = bbuf[2] & 0xFFL;
            lbuf[3] = bbuf[3] & 0xFFL;
        } else {
            lbuf[0] = bbuf[3] & 0xFFL;
            lbuf[1] = bbuf[2] & 0xFFL;
            lbuf[2] = bbuf[1] & 0xFFL;
            lbuf[3] = bbuf[0] & 0xFFL;
        }
        return ( lbuf[3] << 24 ) | ( lbuf[2] << 16 ) | ( lbuf[1] << 8 ) | lbuf[0];
    }

    public long demarshalSaHpiUint64T()
        throws HpiException
    {
        // NB: highest byte & 0xFFL for getting unsigned value
        // makes no sense for 64bit integer.
        // And Java has no 128bit integer.
        // So for SaHpiUint64T we have no reliable demarshal.
        transport.getBytes( 8, bbuf );
        if ( little_endian_demarshal ) {
            lbuf[0] = bbuf[0] & 0xFFL;
            lbuf[1] = bbuf[1] & 0xFFL;
            lbuf[2] = bbuf[2] & 0xFFL;
            lbuf[3] = bbuf[3] & 0xFFL;
            lbuf[4] = bbuf[4] & 0xFFL;
            lbuf[5] = bbuf[5] & 0xFFL;
            lbuf[6] = bbuf[6] & 0xFFL;
            lbuf[7] = bbuf[7] & 0xFFL;
        } else {
            lbuf[0] = bbuf[7] & 0xFFL;
            lbuf[1] = bbuf[6] & 0xFFL;
            lbuf[2] = bbuf[5] & 0xFFL;
            lbuf[3] = bbuf[4] & 0xFFL;
            lbuf[4] = bbuf[3] & 0xFFL;
            lbuf[5] = bbuf[2] & 0xFFL;
            lbuf[6] = bbuf[1] & 0xFFL;
            lbuf[7] = bbuf[0] & 0xFFL;
        }
        return ( lbuf[7] << 56 ) |
               ( lbuf[6] << 48 ) |
               ( lbuf[5] << 40 ) |
               ( lbuf[4] << 32 ) |
               ( lbuf[3] << 24 ) |
               ( lbuf[2] << 16 ) |
               ( lbuf[1] << 8 ) |
               lbuf[0];
    }

    public long demarshalSaHpiInt8T()
        throws HpiException
    {
        return transport.getByte();
    }

    public long demarshalSaHpiInt16T()
        throws HpiException
    {
        transport.getBytes( 2, bbuf );
        if ( little_endian_demarshal ) {
            lbuf[0] = bbuf[0] & 0xFFL;
            lbuf[1] = bbuf[1];
        } else {
            lbuf[0] = bbuf[1] & 0xFFL;
            lbuf[1] = bbuf[0];
        }
        return ( lbuf[1] << 8 ) | lbuf[0];
    }

    public long demarshalSaHpiInt32T()
        throws HpiException
    {
        transport.getBytes( 4, bbuf );
        if ( little_endian_demarshal ) {
            lbuf[0] = bbuf[0] & 0xFFL;
            lbuf[1] = bbuf[1] & 0xFFL;
            lbuf[2] = bbuf[2] & 0xFFL;
            lbuf[3] = bbuf[3];
        } else {
            lbuf[0] = bbuf[3] & 0xFFL;
            lbuf[1] = bbuf[2] & 0xFFL;
            lbuf[2] = bbuf[1] & 0xFFL;
            lbuf[3] = bbuf[0];
        }
        return ( lbuf[3] << 24 ) | ( lbuf[2] << 16 ) | ( lbuf[1] << 8 ) | lbuf[0];
    }

    public long demarshalSaHpiInt64T()
        throws HpiException
    {
        transport.getBytes( 8, bbuf );
        if ( little_endian_demarshal ) {
            lbuf[0] = bbuf[0] & 0xFFL;
            lbuf[1] = bbuf[1] & 0xFFL;
            lbuf[2] = bbuf[2] & 0xFFL;
            lbuf[3] = bbuf[3] & 0xFFL;
            lbuf[4] = bbuf[4] & 0xFFL;
            lbuf[5] = bbuf[5] & 0xFFL;
            lbuf[6] = bbuf[6] & 0xFFL;
            lbuf[7] = bbuf[7];
        } else {
            lbuf[0] = bbuf[7] & 0xFFL;
            lbuf[1] = bbuf[6] & 0xFFL;
            lbuf[2] = bbuf[5] & 0xFFL;
            lbuf[3] = bbuf[4] & 0xFFL;
            lbuf[4] = bbuf[3] & 0xFFL;
            lbuf[5] = bbuf[2] & 0xFFL;
            lbuf[6] = bbuf[1] & 0xFFL;
            lbuf[7] = bbuf[0];
        }
        return ( lbuf[7] << 56 ) |
               ( lbuf[6] << 48 ) |
               ( lbuf[5] << 40 ) |
               ( lbuf[4] << 32 ) |
               ( lbuf[3] << 24 ) |
               ( lbuf[2] << 16 ) |
               ( lbuf[1] << 8 ) |
               lbuf[0];
    }

    public long demarshalEnum()
        throws HpiException
    {
        return demarshalSaHpiInt32T();
    }

    public double demarshalSaHpiFloat64T()
        throws HpiException
    {
        return Double.longBitsToDouble( demarshalSaHpiInt64T() );
    }

    public byte[] demarshalByteArray( int count )
        throws HpiException
    {
        byte[] x = new byte[count];
        transport.getBytes( count, x );
        return x;
    }

};

