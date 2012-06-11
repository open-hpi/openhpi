
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

from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib.HpiMarshalGen import HpiMarshalGen
from openhpi_baselib.OhpiDataTypes import *


#**********************************************************
# OHPI Marshal
#**********************************************************
class OhpiMarshal( HpiMarshalGen ):

    #**********************************************************
    # Marshal: For OHPI Simple Data Types
    #**********************************************************
    def marshaloHpiHandlerIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshaloHpiGlobalParamTypeT( self, x ):
        self.marshalEnum( x )


    #**********************************************************
    # Marshal: For OHPI Structs and Unions
    #**********************************************************
    def marshaloHpiHandlerConfigT( self, x ):
        self.marshalSaHpiUint8T( len( x.items ) )
        for ( name, value ) in x.items:
            self.marshalByteArray( name, SAHPI_MAX_TEXT_BUFFER_LENGTH )
            self.marshalByteArray( value, SAHPI_MAX_TEXT_BUFFER_LENGTH )

    def marshaloHpiGlobalParamUnionT( self, x, mod ):
        # TODO
        raise NotImplementedError()

    def marshaloHpiGlobalParamT( self, x ):
        # TODO
        raise NotImplementedError()


    #**********************************************************
    # Demarshal: For OHPI Simple Data Types
    #**********************************************************
    def demarshaloHpiHandlerIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshaloHpiGlobalParamTypeT( self ):
        return self.demarshalEnum()


    #**********************************************************
    # Demarshal: For OHPI Structs and Unions
    #**********************************************************
    def demarshaloHpiHandlerInfoT( self ):
        x = oHpiHandlerInfoT()
        x.id = self.demarshaloHpiHandlerIdT()
        x.plugin_name = self.demarshalByteArray( MAX_PLUGIN_NAME_LENGTH )
        x.entity_root = self.demarshalSaHpiEntityPathT()
        x.load_failed = self.demarshalSaHpiInt32T()
        return x

    def demarshaloHpiHandlerConfigT( self ):
        x = oHpiHandlerConfigT()
        x.items = []
        n = self.demarshalSaHpiUint8T()
        for i in range( 0, n ):
            name = self.demarshalByteArray( SAHPI_MAX_TEXT_BUFFER_LENGTH )
            value = self.demarshalByteArray( SAHPI_MAX_TEXT_BUFFER_LENGTH )
            x.items.append( ( name, value ) )
        return x

    def demarshaloHpiGlobalParamUnionT( self, mod ):
        # TODO
        raise NotImplementedError()

    def demarshaloHpiGlobalParamT( self ):
        # TODO
        raise NotImplementedError()


