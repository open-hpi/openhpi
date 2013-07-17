/*      -*- c# -*-
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

using System;
using System.Collections.Generic;


namespace org {
namespace openhpi {


internal static class HpiCore
{
    private static Dictionary<long, HpiDomain> domains;
    private static Dictionary<long, HpiSession> sessions;
    private static SaHpiEntityPathT my_ep;

    static HpiCore()
    {
        domains = new Dictionary<long, HpiDomain>();
        sessions = new Dictionary<long, HpiSession>();
        my_ep = null;

        CreateDefaultDomain();
    }

    private static void CreateDefaultDomain()
    {
        if ( domains.ContainsKey( HpiConst.SAHPI_UNSPECIFIED_DOMAIN_ID ) ) {
            return;
        }
        string host = Environment.GetEnvironmentVariable( "OPENHPI_DAEMON_HOST" );
        if ( host == null ) {
            host = "localhost";
        }
        int port;
        string portstr = Environment.GetEnvironmentVariable( "OPENHPI_DAEMON_PORT" );
        if ( portstr != null ) {
            try {
                port = Convert.ToInt32( portstr );
            } catch ( FormatException ) {
                port = OhpiConst.DEFAULT_PORT;
            }
        } else {
            port = OhpiConst.DEFAULT_PORT;
        }
        HpiDomain d = new HpiDomain( HpiConst.SAHPI_UNSPECIFIED_DOMAIN_ID,
                                     host,
                                     port,
                                     HpiUtil.MakeRootSaHpiEntityPathT() );
        domains[d.GetLocalDid()] = d;
    }

    public static HpiDomain CreateDomain( string host,
                                          int port,
                                          SaHpiEntityPathT entity_root )
    {
        HpiDomain d = new HpiDomain( HpiConst.SAHPI_UNSPECIFIED_DOMAIN_ID,
                                     host,
                                     port,
                                     entity_root );
        bool ok = false;

        lock ( domains )
        {
            for ( long did = 0; did < long.MaxValue; ++did ) {
                if ( !domains.ContainsKey( did ) ) {
                    d.SetLocalDid( did );
                    domains[did] = d;
                    ok = true;
                    break;
                }
            }
        }

        if ( !ok ) {
            d = null;
        }

        return d;
    }

    /**********************************************************
     * Creates and returns new domain with specified Domain Id
     * Returns null if overwrite == false and
     * the specified Domain Id is already in use
     *********************************************************/
    public static HpiDomain CreateDomainById( long did,
                                              string host,
                                              int port,
                                              SaHpiEntityPathT entity_root,
                                              bool overwrite )
    {
        HpiDomain d = null; 

        lock ( domains )
        {
            if ( ( !domains.ContainsKey( did ) ) || overwrite ) {
                d = new HpiDomain( did, host, port, entity_root );
                domains[did] = d;
            }
        }

        return d;
    }

    public static HpiSession CreateSession( long local_did )
    {
        bool rc;
        HpiDomain d = null;
        lock ( domains )
        {
            rc = domains.TryGetValue( local_did, out d );
        }
        if ( !rc ) {
            return null;
        }
        HpiSession s = new HpiSession( d );
        lock ( sessions )
        {
            sessions.Add( s.GetLocalSid(), s );
        }

        return s;
    }

    public static void RemoveSession( HpiSession s )
    {
        lock ( sessions )
        {
            sessions.Remove( s.GetLocalSid() );
        }
        s.Close();
    }

    public static HpiSession GetSession( long local_sid )
    {
        bool rc;
        HpiSession s = null;
        lock ( sessions )
        {
            rc = sessions.TryGetValue( local_sid, out s );
        }
        if ( !rc ) {
            s = null;
        }
        return s;
    }

    public static SaHpiEntityPathT GetMyEntity()
    {
        return HpiUtil.CloneSaHpiEntityPathT( my_ep );
    }

    public static void SetMyEntity( SaHpiEntityPathT ep )
    {
        my_ep = HpiUtil.CloneSaHpiEntityPathT( ep );
    }

    private static void Dump()
    {
        if ( my_ep != null ) {
            Console.WriteLine( "My Entity: {0}", HpiUtil.FromSaHpiEntityPathT( my_ep ) );
        }
        Console.WriteLine( "Defined Domains:" );
        foreach( KeyValuePair<long, HpiDomain> kv in domains ) {
            Console.WriteLine( "  id {0} => id {1}, host {2}, port {3}, root {4}",
                               kv.Value.GetLocalDid(),
                               kv.Value.GetRemoteDid(),
                               kv.Value.GetRemoteHost(),
                               kv.Value.GetRemotePort(),
                               HpiUtil.FromSaHpiEntityPathT( kv.Value.GetEntityRoot() ) );
        }
    }
};


}; // namespace openhpi
}; // namespace org

