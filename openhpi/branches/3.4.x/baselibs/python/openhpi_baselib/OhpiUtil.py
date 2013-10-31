
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
from openhpi_baselib.OhpiDataTypes import *


#**********************************************************
# OHPI Utility Functions
#**********************************************************

#**********************************************************
# NB: toXXX raises ValueError if lookup fails
#**********************************************************

#**********************************************************
# Check Functions for OHPI Complex Data Types
#**********************************************************

#**
# Check function for OHPI struct oHpiHandlerConfigT
#**
def checkoHpiHandlerConfigT( x ):
    if x is None:
        return False
    if not isinstance( x, oHpiHandlerConfigT ):
        return False
    if x.items is None:
        return False
    if not isinstance( x.items, list ):
        return False
    for nv in x.items:
        if not isinstance( nv, tuple ):
            return False
        if len( nv ) != 2:
            return False
        ( name, value ) = nv
        if not isinstance( name, str ):
            return False
        if len( name ) != SAHPI_MAX_TEXT_BUFFER_LENGTH:
            return False
        if not isinstance( value, str ):
            return False
        if len( value ) != SAHPI_MAX_TEXT_BUFFER_LENGTH:
            return False
    return True


#**********************************************************
# oHpiHandlerConfigT Helpers
#**********************************************************

def fromoHpiHandlerConfigT( config ):
    d = dict()
    for ( n, v ) in config.items:
        d[n.rstrip( chr(0) )] = v.rstrip( chr(0) )
    return d

def tooHpiHandlerConfigT( d ):
    if not isinstance( d, dict ):
        raise ValueError()
    if len( d ) > 255:
        raise ValueError()
    for ( n, v ) in d.iteritems():
        if not isinstance( n, str ):
            raise ValueError()
        if len( n ) > SAHPI_MAX_TEXT_BUFFER_LENGTH:
            raise ValueError()
        if not isinstance( v, str ):
            raise ValueError()
        if len( v ) > SAHPI_MAX_TEXT_BUFFER_LENGTH:
            raise ValueError()
    config = oHpiHandlerConfigT()
    config.items = []
    for ( n, v ) in d.iteritems():
        n = n.ljust( SAHPI_MAX_TEXT_BUFFER_LENGTH, chr(0) )
        v = v.ljust( SAHPI_MAX_TEXT_BUFFER_LENGTH, chr(0) )
        config.items.append( ( n, v ) )
    return config

