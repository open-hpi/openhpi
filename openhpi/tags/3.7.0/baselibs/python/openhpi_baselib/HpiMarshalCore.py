
#      -*- python -*-
#
# Copyright (C) 2012, Pigeon Point Systems
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#      Anton Pak <anton.pak@pigeonpoint.com>
#

from openhpi_baselib.HpiTransport import HpiTransport


class HpiMarshalCore:
    def __init__( self ):
        self.transport = None
        self.st = None

    def open( self, host, port ):
        self.transport = HpiTransport()
        rc = self.transport.open( host, port )
        return rc

    def close( self ):
        self.transport.close()
        self.transport = None

    def reset( self ):
        self.transport.reset()
        self.st = self.transport.get_st()

    def interchange( self, rpc_id ):
        rc = self.transport.interchange( rpc_id )
        self.st = self.transport.get_st()
        return rc


    #**********************************************************
    # Marshal: For HPI Basic Data Types
    #**********************************************************
    def marshalSaHpiUint8T( self, x ):
        self.transport.put_data( self.st.u8, x )

    def marshalSaHpiUint16T( self, x ):
        self.transport.put_data( self.st.u16, x )

    def marshalSaHpiUint32T( self, x ):
        self.transport.put_data( self.st.u32, x )

    def marshalSaHpiUint64T( self, x ):
        self.transport.put_data( self.st.u64, x )

    def marshalSaHpiInt8T( self, x ):
        self.transport.put_data( self.st.i8, x )

    def marshalSaHpiInt16T( self, x ):
        self.transport.put_data( self.st.i16, x )

    def marshalSaHpiInt32T( self, x ):
        self.transport.put_data( self.st.i32, x )

    def marshalSaHpiInt64T( self, x ):
        self.transport.put_data( self.st.i64, x )

    def marshalEnum( self, x ):
        self.marshalSaHpiInt32T( x )

    def marshalSaHpiFloat64T( self, x ):
        self.transport.put_data( self.st.f64, x )

    def marshalByteArray( self, x, count ):
        self.transport.put_data( self.st.arrays[count], x )

    #**********************************************************
    # Demarshal: For HPI Basic Data Types
    #**********************************************************
    def demarshalSaHpiUint8T( self ):
        return self.transport.get_data( self.st.u8 )

    def demarshalSaHpiUint16T( self ):
        return self.transport.get_data( self.st.u16 )

    def demarshalSaHpiUint32T( self ):
        return self.transport.get_data( self.st.u32 )

    def demarshalSaHpiUint64T( self ):
        return self.transport.get_data( self.st.u64 )

    def demarshalSaHpiInt8T( self ):
        return self.transport.get_data( self.st.i8 )

    def demarshalSaHpiInt16T( self ):
        return self.transport.get_data( self.st.i16 )

    def demarshalSaHpiInt32T( self ):
        return self.transport.get_data( self.st.i32 )

    def demarshalSaHpiInt64T( self ):
        return self.transport.get_data( self.st.i64 )

    def demarshalEnum( self ):
        return self.demarshalSaHpiInt32T()

    def demarshalSaHpiFloat64T( self ):
        return self.transport.get_data( self.st.f64 )

    def demarshalByteArray( self, count ):
        return self.transport.get_data( self.st.arrays[count] )

