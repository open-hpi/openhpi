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


namespace org {
namespace openhpi {


internal class HpiDomain
{
    private long             local_did;
    private long             remote_did;
    private string           remote_host;
    private int              remote_port;
    private SaHpiEntityPathT entity_root;

    public HpiDomain( long local_did,
                      string remote_host,
                      int remote_port,
                      SaHpiEntityPathT entity_root )
    {
        this.local_did   = local_did;
        this.remote_did  = HpiConst.SAHPI_UNSPECIFIED_DOMAIN_ID;
        this.remote_host = remote_host;
        this.remote_port = remote_port;
        this.entity_root = entity_root;
    }

    public HpiDomain( HpiDomain other )
    {
        this.local_did   = other.local_did;
        this.remote_did  = other.remote_did;
        this.remote_host = other.remote_host;
        this.remote_port = other.remote_port;
        this.entity_root = other.entity_root;
    }

    public long GetLocalDid()
    {
        return local_did;
    }

    public void SetLocalDid( long local_did )
    {
        this.local_did = local_did;
    }

    public long GetRemoteDid()
    {
        return remote_did;
    }

    public string GetRemoteHost()
    {
        return remote_host;
    }

    public int GetRemotePort()
    {
        return remote_port;
    }

    public SaHpiEntityPathT GetEntityRoot()
    {
        return entity_root;
    }
};


}; // namespace openhpi
}; // namespace org

