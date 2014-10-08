
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

import re, os, threading

from openhpi_baselib.HpiDataTypes import SAHPI_UNSPECIFIED_DOMAIN_ID
from openhpi_baselib.HpiDomain import HpiDomain
from openhpi_baselib.HpiSession import HpiSession
from openhpi_baselib import HpiUtil
from openhpi_baselib.OhpiDataTypes import DEFAULT_PORT


class HpiCore:
    domains  = dict()
    sessions = dict()
    my_ep    = None
    lock     = threading.RLock()

    @staticmethod
    def createDefaultDomain():
        if HpiCore.domains.has_key( SAHPI_UNSPECIFIED_DOMAIN_ID ):
            return
        host = os.environ.get( "OPENHPI_DAEMON_HOST", "localhost" )
        portstr = os.environ.get( "OPENHPI_DAEMON_PORT", str( DEFAULT_PORT ) )
        try:
            port = int( portstr )
        except ValueError:
            port = DEFAULT_PORT
        root = HpiUtil.makeRootSaHpiEntityPathT()
        d = HpiDomain( SAHPI_UNSPECIFIED_DOMAIN_ID, host, port, root )
        HpiCore.domains[d.getLocalDid()] = d

    @staticmethod
    def createDomain( host, port, entity_root ):
        d = HpiDomain( SAHPI_UNSPECIFIED_DOMAIN_ID, host, port, entity_root )
        HpiCore.lock.acquire()
        did = 0
        while HpiCore.domains.has_key( did ):
            did = did + 1
        d.setLocalDid( did )
        HpiCore.domains[did] = d
        HpiCore.lock.release()
        return d

    #**********************************************************
    # Creates and returns new domain with specified Domain Id
    # Returns None if overwrite == false and
    # the specified Domain Id is already in use
    #**********************************************************
    @staticmethod
    def createDomainById( did, host, port, entity_root, overwrite ):
        d = None
        HpiCore.lock.acquire()
        if ( not HpiCore.domains.has_key( did ) ) or overwrite:
            d = HpiDomain( did, host, port, entity_root )
            HpiCore.domains[did] = d
        HpiCore.lock.release()
        return d

    @staticmethod
    def createSession( local_did ):
        s = None
        HpiCore.lock.acquire()
        d = HpiCore.domains.get( local_did, None )
        if d is not None:
            s = HpiSession( d )
            HpiCore.sessions[s.getLocalSid()] = s
        HpiCore.lock.release()
        return s

    @staticmethod
    def removeSession( s ):
        HpiCore.lock.acquire()
        del HpiCore.sessions[s.getLocalSid()]
        HpiCore.lock.release()
        s.close()

    @staticmethod
    def getSession( local_sid ):
        HpiCore.lock.acquire()
        s = HpiCore.sessions.get( local_sid, None )
        HpiCore.lock.release()
        return s

    @staticmethod
    def getMyEntity():
        return HpiUtil.cloneSaHpiEntityPathT( HpiCore.my_ep )

    @staticmethod
    def setMyEntity( ep ):
        HpiCore.my_ep = HpiUtil.cloneSaHpiEntityPathT( ep )

    @staticmethod
    def dump():
        if HpiCore.my_ep is not None:
            print "My Entity: %s" % HpiUtil.fromSaHpiEntityPathT( HpiCore.my_ep )
        print "Defined Domains:"
        for d in HpiCore.domains.itervalues():
            ldid = d.getLocalDid()
            rdid = d.getRemoteDid()
            host = d.getRemoteHost()
            port = d.getRemotePort()
            root = HpiUtil.fromSaHpiEntityPathT( d.getEntityRoot() )
            print "  id %u => id %u, host %s, port %u, root %s" % ( ldid, rdid, host, port, root )


#**********************************************************
# Initialization
#**********************************************************
HpiCore.createDefaultDomain()

