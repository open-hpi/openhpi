
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
from openhpi_baselib.HpiUtilGen import *


#**********************************************************
# HPI Utility Functions
#**********************************************************

#**********************************************************
# NB: toXXX raises ValueError if lookup fails
#**********************************************************

#**********************************************************
# Text Buffer Helpers
#**********************************************************

def fromSaHpiTextBufferT( tb ):
    if tb.DataType == SAHPI_TL_TYPE_UNICODE:
        # TODO
        raise ValueError()
    else:
        return tb.Data[0:tb.DataLength]

def toSaHpiTextBufferT( s ):
    if not isinstance( s, str ):
        # TODO
        raise ValueError()
    if len( s ) > SAHPI_MAX_TEXT_BUFFER_LENGTH:
        raise ValueError()
    tb = SaHpiTextBufferT()
    tb.DataType   = SAHPI_TL_TYPE_TEXT
    tb.Language   = SAHPI_LANG_ENGLISH
    tb.DataLength = len( s )
    tb.Data       = s.ljust( SAHPI_MAX_TEXT_BUFFER_LENGTH, chr(0) )
    return tb


#**********************************************************
# Entity Path Helpers
#**********************************************************
def makeUnspecifiedSaHpiEntityPathT():
    ep = SaHpiEntityPathT()
    ep.Entry = []
    for i in range( 0, SAHPI_MAX_ENTITY_PATH ):
        e = SaHpiEntityT()
        e.EntityType = SAHPI_ENT_UNSPECIFIED
        e.EntityLocation = 0
        ep.Entry.append( e )
    return ep

def makeRootSaHpiEntityPathT():
    ep = makeUnspecifiedSaHpiEntityPathT()
    ep.Entry[0].EntityType = SAHPI_ENT_ROOT
    ep.Entry[0].EntityLocation = 0
    return ep

def cloneSaHpiEntityPathT( ep ):
    if ep is None:
        return None
    ep2 = makeUnspecifiedSaHpiEntityPathT()
    for i in range( 0, SAHPI_MAX_ENTITY_PATH ):
        ep2.Entry[i].EntityType = ep.Entry[i].EntityType
        ep2.Entry[i].EntityLocation = ep.Entry[i].EntityLocation
    return ep2

def getSaHpiEntityPathTLength( ep ):
    for i in range( 0, SAHPI_MAX_ENTITY_PATH ):
        if ep.Entry[i].EntityType == SAHPI_ENT_ROOT:
            if ep.Entry[i].EntityLocation == 0:
                return i
    return SAHPI_MAX_ENTITY_PATH

def fromSaHpiEntityPathT( ep ):
    l = getSaHpiEntityPathTLength( ep )
    x = [ ( ep.Entry[i].EntityType, ep.Entry[i].EntityLocation ) for i in range( 0, l ) ]
    x.reverse()
    return "".join( [ "{%s,%d}" % ( fromSaHpiEntityTypeT( et ), el ) for ( et, el ) in x ] )

def toSaHpiEntityPathT( s ):
    ep = makeRootSaHpiEntityPathT()
    if len( s ) == 0:
        return ep
    if s.index( "{" ) != 0:
        raise ValueError()
    parts = s.split("{")[1:]
    parts.reverse()
    i = 0
    if len( parts ) > SAHPI_MAX_ENTITY_PATH:
        raise ValueError()
    for part in parts:
        if part[-1] != "}":
            raise ValueError()
        et_el = part[0:-1].split( "," )
        if len( et_el ) != 2:
            raise ValueError()
        ep.Entry[i].EntityType = toSaHpiEntityTypeT( et_el[0] )
        ep.Entry[i].EntityLocation = int( et_el[1] )
        i = i + 1
    if i < SAHPI_MAX_ENTITY_PATH:
        ep.Entry[i].EntityType = SAHPI_ENT_ROOT
        ep.Entry[i].EntityLocation = 0
    return ep

