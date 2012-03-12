package org.openhpi;

import java.util.Collection;
import java.util.concurrent.atomic.AtomicLong;
import java.util.LinkedList;


class HpiSession
{
    private static AtomicLong next_local_sid = new AtomicLong( 1L );


    private long                   local_sid;
    private long                   remote_sid;
    private HpiDomain              domain;
    private LinkedList<HpiMarshal> marshals;


    public HpiSession( HpiDomain domain )
    {
        this.local_sid  = next_local_sid.getAndIncrement();
        this.remote_sid = 0;
        this.domain     = new HpiDomain( domain );
        this.marshals   = new LinkedList<HpiMarshal>();
    }

    public synchronized void close()
    {
        while( !marshals.isEmpty() ) {
            HpiMarshal m = marshals.removeFirst();
            m.close();
        }
    }

    public long getLocalSid()
    {
        return local_sid;
    }

    public long getRemoteSid()
    {
        return remote_sid;
    }

    public void setRemoteSid( long remote_sid )
    {
        this.remote_sid = remote_sid;
    }

    public long getRemoteDid()
    {
        return domain.getRemoteDid();
    }

    public synchronized HpiMarshal getMarshal()
    {
        HpiMarshal m;
        if ( !marshals.isEmpty() ) {
            m = marshals.removeFirst();
        } else {
            m = new HpiMarshal();
            boolean rc = m.open( domain.getRemoteHost(), domain.getRemotePort() );
            if ( !rc ) {
                m = null;
            }
        }
        if ( m != null ) {
            m.reset();
        }
        return m;
    }

    public synchronized void putMarshal( HpiMarshal m )
    {
        m.reset();
        marshals.addFirst( m );
    }
};

