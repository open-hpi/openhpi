import collections, threading
from openhpi_baselib.HpiDomain import HpiDomain
from openhpi_baselib.HpiMarshalGen import HpiMarshal


class HpiSession:

    next_local_sid = 1
    next_local_sid_lock = threading.RLock()

    def __init__( self, domain ):
        HpiSession.next_local_sid_lock.acquire()
        self.local_sid  = HpiSession.next_local_sid
        HpiSession.next_local_sid += 1
        HpiSession.next_local_sid_lock.release()
        self.remote_sid = None
        self.domain = HpiDomain.clone( domain )
        self.marshals = collections.deque()
        self.marshals_lock = threading.RLock()

    def close( self ):
        self.marshals_lock.acquire()
        marshals = self.marshals
        self.marshal = collections.deque()
        self.marshals_lock.release()
        for m in marshals:
            m.close()
        marshals.clear()

    def getLocalSid( self ):
        return self.local_sid

    def getRemoteSid( self ):
        return self.remote_sid

    def setRemoteSid( self, remote_sid ):
        self.remote_sid = remote_sid

    def getRemoteDid( self ):
        return self.domain.getRemoteDid()

    def getMarshal( self ):
        m = None
        self.marshals_lock.acquire()
        if len( self.marshals ) > 0:
            m = self.marshals.pop()
        self.marshals_lock.release()
        if m is None:
            m = HpiMarshal()
            rc = m.open( self.domain.getRemoteHost(), self.domain.getRemotePort() )
            if not rc:
                m = None
        if m:
            m.reset()
        return m

    def putMarshal( self, m ):
        m.reset()
        self.marshals_lock.acquire()
        self.marshals.append( m )
        self.marshals_lock.release()

