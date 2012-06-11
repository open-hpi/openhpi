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


namespace org {
namespace openhpi {


internal class HpiMarshalCore
{
    private HpiTransport transport;

    private const int BBUF_SIZE = 8;
    private byte[] bbuf;
    private bool little_endian_demarshal;


    public bool Open( string host, int port )
    {
        transport = new HpiTransport();
        bool rc = transport.Open( host, port );
        if ( rc ) {
            bbuf = new byte[BBUF_SIZE];
        }
        return rc;
    }

    public void Close()
    {
        transport.Close();
        bbuf = null;
    }

    public void Reset()
    {
        transport.Reset();
    }

    public bool Interchange( int rpc_id )
    {
        bool rc = transport.Interchange( rpc_id );
        if ( rc ) {
            little_endian_demarshal = transport.IsLittleEndian();
        }
        return rc;
    }


    /**********************************************************
     * Marshal: For HPI Basic Data Types
     *********************************************************/
    public void MarshalSaHpiUint8T( long x )
    {
        transport.PutByte( unchecked( (byte)x ) );
    }

    public void MarshalSaHpiUint16T( long x )
    {
        unchecked
        {
            ushort xx = (ushort)( x );
            bbuf[0] = (byte)( xx );
            bbuf[1] = (byte)( xx >> 8 );
        }
        transport.PutBytes( bbuf, 2 );
    }

    public void MarshalSaHpiUint32T( long x )
    {
        unchecked
        {
            uint xx = (uint)( x );
            bbuf[0] = (byte)( xx );
            bbuf[1] = (byte)( xx >> 8 );
            bbuf[2] = (byte)( xx >> 16 );
            bbuf[3] = (byte)( xx >> 24 );
        }
        transport.PutBytes( bbuf, 4 );
    }

    public void MarshalSaHpiUint64T( long x )
    {
        unchecked
        {
            ulong xx = (ulong)( x );
            bbuf[0] = (byte)( xx );
            bbuf[1] = (byte)( xx >> 8 );
            bbuf[2] = (byte)( xx >> 16 );
            bbuf[3] = (byte)( xx >> 24 );
            bbuf[4] = (byte)( xx >> 32 );
            bbuf[5] = (byte)( xx >> 40 );
            bbuf[6] = (byte)( xx >> 48 );
            bbuf[7] = (byte)( xx >> 56 );
        }
        transport.PutBytes( bbuf, 8 );
    }

    public void MarshalSaHpiInt8T( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiInt16T( long x )
    {
        MarshalSaHpiUint16T( x );
    }

    public void MarshalSaHpiInt32T( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiInt64T( long x )
    {
        MarshalSaHpiUint64T( x );
    }

    public void MarshalEnum( long x )
    {
        MarshalSaHpiInt32T( x );
    }

    public void MarshalSaHpiFloat64T( double x )
    {
        MarshalSaHpiInt64T( BitConverter.DoubleToInt64Bits( x ) );
    }

    public void MarshalByteArray( byte[] x, int count )
    {
        transport.PutBytes( x, count );
    }


    /**********************************************************
     * Demarshal: For HPI Basic Data Types
     *********************************************************/
    public long DemarshalSaHpiUint8T()
    {
        return transport.GetByte();
    }

    public long DemarshalSaHpiUint16T()
    {
        transport.GetBytes( 2, bbuf );
        uint b0, b1;
        if ( little_endian_demarshal ) {
            b0 = bbuf[0];
            b1 = bbuf[1];
        } else {
            b0 = bbuf[1];
            b1 = bbuf[0];
        }
        return (ushort)( ( b1 << 8 ) | b0 );
    }

    public long DemarshalSaHpiUint32T()
    {
        transport.GetBytes( 4, bbuf );
        uint b0, b1, b2, b3;
        if ( little_endian_demarshal ) {
            b0 = bbuf[0];
            b1 = bbuf[1];
            b2 = bbuf[2];
            b3 = bbuf[3];
        } else {
            b0 = bbuf[3];
            b1 = bbuf[2];
            b2 = bbuf[1];
            b3 = bbuf[0];
        }
        return (uint)( ( b3 << 24 ) | ( b2 << 16 ) | ( b1 << 8 ) | b0 );
    }

    // NB: SaHpiUint64T we have no reliable demarshal
    // since it is represented as signed int64
    public long DemarshalSaHpiUint64T()
    {
        transport.GetBytes( 8, bbuf );
        long b0, b1, b2, b3, b4, b5, b6, b7;
        if ( little_endian_demarshal ) {
            b0 = bbuf[0];
            b1 = bbuf[1];
            b2 = bbuf[2];
            b3 = bbuf[3];
            b4 = bbuf[4];
            b5 = bbuf[5];
            b6 = bbuf[6];
            b7 = bbuf[7];
        } else {
            b0 = bbuf[7];
            b1 = bbuf[6];
            b2 = bbuf[5];
            b3 = bbuf[4];
            b4 = bbuf[3];
            b5 = bbuf[2];
            b6 = bbuf[1];
            b7 = bbuf[0];
        }
        return ( b7 << 56 ) |
               ( b6 << 48 ) |
               ( b5 << 40 ) |
               ( b4 << 32 ) |
               ( b3 << 24 ) |
               ( b2 << 16 ) |
               ( b1 << 8 ) |
               b0;
    }

    public long DemarshalSaHpiInt8T()
    {
        return unchecked( (sbyte)DemarshalSaHpiUint8T() );
    }

    public long DemarshalSaHpiInt16T()
    {
        return unchecked( (short)DemarshalSaHpiUint16T() );
    }

    public long DemarshalSaHpiInt32T()
    {
        return unchecked( (int)DemarshalSaHpiUint32T() );
    }

    public long DemarshalSaHpiInt64T()
    {
        return unchecked( (long)DemarshalSaHpiUint64T() );
    }

    public long DemarshalEnum()
    {
        return DemarshalSaHpiInt32T();
    }

    public double DemarshalSaHpiFloat64T()
    {
        return BitConverter.Int64BitsToDouble( DemarshalSaHpiInt64T() );
    }

    public byte[] DemarshalByteArray( int count )
    {
        byte[] x = new byte[count];
        transport.GetBytes( count, x );
        return x;
    }
};


}; // namespace openhpi
}; // namespace org

