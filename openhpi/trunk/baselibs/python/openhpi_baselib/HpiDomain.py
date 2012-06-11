
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

from openhpi_baselib.HpiDataTypes import SAHPI_UNSPECIFIED_DOMAIN_ID


class HpiDomain:
    def __init__( self, local_did, remote_host, remote_port, entity_root ):
        self.local_did   = local_did
        self.remote_did  = SAHPI_UNSPECIFIED_DOMAIN_ID
        self.remote_host = remote_host
        self.remote_port = remote_port
        self.entity_root = entity_root

    @staticmethod
    def clone( other ):
        d = HpiDomain( other.local_did, other.remote_host,
                       other.remote_port, other.entity_root )
        d.remote_did  = other.remote_did
        return d

    def getLocalDid( self ):
        return self.local_did

    def setLocalDid( self, local_did ):
        self.local_did = local_did

    def getRemoteDid( self ):
        return self.remote_did

    def getRemoteHost( self ):
        return self.remote_host

    def getRemotePort( self ):
        return self.remote_port

    def getEntityRoot( self ):
        return self.entity_root

