
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

import binascii, ctypes, socket, struct
from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib.HpiException import HpiException
from openhpi_baselib.OhpiDataTypes import *


######################################################################
class PduDef:
    MAX_SIZE         = 0xFFFF
    HDR_SIZE         = 12
    TYPE_OFF         = 0x00
    FLAGS_OFF        = 0x01
    RPC_ID_OFF       = 0x04
    PAYLOAD_SIZE_OFF = 0x08
    TYPE             = 0x01 # MSG
    FLAGS            = 0x11 # RPC version 1, little-endian
    ENDIAN_FLAG_MASK = 0x01 # If endian flag is set: little-endian

######################################################################
class Structs:
    def __init__( self, is_le ):
        self.u8 = struct.Struct( "=B" )
        self.i8 = struct.Struct( "=b" )
        if is_le:
            self.u16 = struct.Struct( "<H" )
            self.i16 = struct.Struct( "<h" )
            self.u32 = struct.Struct( "<I" )
            self.i32 = struct.Struct( "<i" )
            self.u64 = struct.Struct( "<Q" )
            self.i64 = struct.Struct( "<q" )
            self.f64 = struct.Struct( "<d" )
            self.hdr = struct.Struct( "<BBBBii" )
        else:
            self.u16 = struct.Struct( ">H" )
            self.i16 = struct.Struct( ">h" )
            self.u32 = struct.Struct( ">I" )
            self.i32 = struct.Struct( ">i" )
            self.u64 = struct.Struct( ">Q" )
            self.i64 = struct.Struct( ">q" )
            self.f64 = struct.Struct( ">d" )
            self.hdr = struct.Struct( ">BBBBii" )
        self.arrays = dict()
        sizes = [ SAHPI_GUID_LENGTH, SAHPI_MAX_TEXT_BUFFER_LENGTH,
                  SAHPI_SENSOR_BUFFER_LENGTH, SAHPI_CTRL_MAX_STREAM_LENGTH,
                  SAHPI_CTRL_MAX_OEM_BODY_LENGTH, SAHPI_CTRL_OEM_CONFIG_LENGTH,
                  SAHPI_DIMITEST_PARAM_NAME_LEN, SAHPI_DIMITEST_PARAM_NAME_LEN,
                  SAHPI_FUMI_MAX_OEM_BODY_LENGTH, SA_HPI_MAX_NAME_LENGTH,
                  MAX_PLUGIN_NAME_LENGTH ]
        for s in sizes:
            self.arrays[s] = struct.Struct( "=%ss" % s )


######################################################################
class HpiTransport:
    #**********************************************************
    # NB: Assumption
    # We assume that marshal layer puts bytes in little-endian
    # order for RPC request.
    # And remote party defines endian for RPC response.
    #**********************************************************

    def __init__( self ):
        self.sock  = None
        self.pdu   = None
        self.hpos  = 0
        self.lpos  = 0
        self.st_le = Structs( True )
        self.st_be = Structs( False )
        self.st    = self.st_le

    def open( self, host, port ):
        try:
            infos = socket.getaddrinfo( host, port, socket.AF_UNSPEC, socket.SOCK_STREAM )
        except socket.gaierror:
            infos = []
        s = None
        for ( af, stype, proto, cname, sa ) in infos:
            try:
                s = socket.socket( af, stype, proto )
            except socket.error:
                s = None
                continue
            try:
                s.connect( sa )
            except socket.error:
                s.close()
                s = None
                continue
            break
        if s is not None:
            self.sock = s
            self.pdu = ctypes.create_string_buffer( PduDef.MAX_SIZE )
            self.reset()
        return self.sock is not None

    def close( self ):
        self.sock.shutdown( socket.SHUT_RDWR )
        self.sock.close()
        self.sock = None
        self.pdu  = None

    def reset( self ):
        self.lpos = PduDef.HDR_SIZE
        self.hpos = PduDef.HDR_SIZE
        self.st   = self.st_le

    def get_st( self ):
        return self.st

    def put_data( self, st, x ):
        st.pack_into( self.pdu, self.hpos, x )
        self.hpos += st.size

    def get_data( self, st ):
        if self.lpos + st.size > self.hpos:
            raise HpiException( "Not enough data for demarshal" )
        x = st.unpack_from( self.pdu, self.lpos )
        self.lpos += st.size
        return x[0]

    def interchange( self, rpc_id ):
        self.st.hdr.pack_into( self.pdu, 0,
                               PduDef.TYPE, PduDef.FLAGS, 0, 0,
                               rpc_id, self.hpos - PduDef.HDR_SIZE )
        self.sock.sendall( buffer( self.pdu, 0, self.hpos ) )
        self.lpos = PduDef.HDR_SIZE
        self.hpos = 0
        need = PduDef.HDR_SIZE
        while self.hpos < need:
            chunk = self.sock.recv( need - self.hpos )
            got = len( chunk )
            if got == 0:
                break
            struct.pack_into( "=%ds" % got, self.pdu, self.hpos, chunk )
            self.hpos += got
            if self.hpos == PduDef.HDR_SIZE:
                flags = self.st.u8.unpack_from( self.pdu, PduDef.FLAGS_OFF )[0]
                if ( flags & PduDef.ENDIAN_FLAG_MASK ) == 0:
                    self.st = self.st_be
                psize = self.st.i32.unpack_from( self.pdu, PduDef.PAYLOAD_SIZE_OFF )[0]
                need += psize
                if need > PduDef.MAX_SIZE:
                    raise HpiException( "Incoming data is too large" )
        return ( self.hpos == need )

    def dump( self, name ):
        print "PDU %s" % name
        print "  LPOS %d" % self.lpos
        print "  HPOS %d" % self.hpos
        print "  DATA %s" % binascii.hexlify( self.pdu[0:self.hpos] )

