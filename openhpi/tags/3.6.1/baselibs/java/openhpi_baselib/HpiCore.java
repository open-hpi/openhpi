/*      -*- java -*-
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

package org.openhpi;

import java.util.concurrent.ConcurrentHashMap;
import java.util.Map;

import static org.openhpi.HpiDataTypes.SAHPI_UNSPECIFIED_DOMAIN_ID;
import static org.openhpi.HpiDataTypes.SaHpiEntityPathT;
import static org.openhpi.HpiDataTypes.SaHpiTextBufferT;
import static org.openhpi.OhpiDataTypes.OPENHPI_DEFAULT_DAEMON_PORT;


class HpiCore
{
    private static ConcurrentHashMap<Long, HpiDomain> domains;
    private static ConcurrentHashMap<Long, HpiSession> sessions;
    private static SaHpiEntityPathT my_ep;

    static
    {
        domains = new ConcurrentHashMap<Long, HpiDomain>();
        sessions = new ConcurrentHashMap<Long, HpiSession>();
        my_ep = null;

        createDefaultDomain();
    }

    private static void createDefaultDomain()
    {
        if ( domains.containsKey( SAHPI_UNSPECIFIED_DOMAIN_ID ) ) {
            return;
        }
        String host = System.getenv( "OPENHPI_DAEMON_HOST" );
        if ( host == null ) {
            host = "localhost";
        }
        int port;
        String portstr = System.getenv( "OPENHPI_DAEMON_PORT" );
        if ( portstr != null ) {
            try {
                port = Integer.parseInt( portstr );
            } catch ( NumberFormatException e ) {
                port = OPENHPI_DEFAULT_DAEMON_PORT;
            }
        } else {
            port = OPENHPI_DEFAULT_DAEMON_PORT;
        }
        HpiDomain d = new HpiDomain( SAHPI_UNSPECIFIED_DOMAIN_ID,
                                     host,
                                     port,
                                     HpiUtil.makeRootSaHpiEntityPathT() );
        domains.put( d.getLocalDid(), d );
    }

    static HpiDomain createDomain( String host,
                                   int port,
                                   SaHpiEntityPathT entity_root )
    {
        HpiDomain d = new HpiDomain( SAHPI_UNSPECIFIED_DOMAIN_ID,
                                     host,
                                     port,
                                     entity_root );
        for ( long did = 0; did < Long.MAX_VALUE; ++did ) {
            HpiDomain d2 = domains.putIfAbsent( did, d );
            if ( d2 == null ) {
                d.setLocalDid( did );
                return d;
            }
        }
        return null;
    }

    /**********************************************************
     * Creates and returns new domain with specified Domain Id
     * Returns null if overwrite == false and
     * the specified Domain Id is already in use
     *********************************************************/
    static HpiDomain createDomainById( long did,
                                       String host,
                                       int port,
                                       SaHpiEntityPathT entity_root,
                                       boolean overwrite )
    {
        HpiDomain dnew = new HpiDomain( did, host, port, entity_root );

        if ( overwrite ) {
            domains.put( did, dnew );
        } else {
            HpiDomain dprev = domains.putIfAbsent( did, dnew );
            if ( dprev != null ) {
                dnew = null;
            }
        }

        return dnew;
    }

    static HpiSession createSession( long local_did )
    {
        HpiDomain d = domains.get( local_did );
        if ( d == null ) {
            return null;
        }
        HpiSession s = new HpiSession( d );
        sessions.put( s.getLocalSid(), s );

        return s;
    }

    static void removeSession( HpiSession s )
    {
        if ( s == null ) {
            return;
        }
        sessions.remove( s.getLocalSid() );
        s.close();
    }

    static HpiSession getSession( long local_sid )
    {
        return sessions.get( local_sid );
    }

    static SaHpiEntityPathT getMyEntity()
    {
        return HpiUtil.cloneSaHpiEntityPathT( my_ep );
    }

    static void setMyEntity( SaHpiEntityPathT ep )
    {
        my_ep = HpiUtil.cloneSaHpiEntityPathT( ep );
    }

    private static void dump()
    {
        if ( my_ep != null ) {
            System.out.printf( "My Entity: %s\n", HpiUtil.fromSaHpiEntityPathT( my_ep ) );
        }
        System.out.printf( "Defined Domains:\n" );
        for ( Map.Entry<Long, HpiDomain> e: domains.entrySet() ) {
            System.out.printf( "  id %d => id %d, host %s, port %d, root %s\n",
                               e.getValue().getLocalDid(),
                               e.getValue().getRemoteDid(),
                               e.getValue().getRemoteHost(),
                               e.getValue().getRemotePort(),
                               HpiUtil.fromSaHpiEntityPathT( e.getValue().getEntityRoot() ) );
        }
    }
};

