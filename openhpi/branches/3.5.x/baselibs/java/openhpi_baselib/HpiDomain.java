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

import static org.openhpi.HpiDataTypes.SAHPI_UNSPECIFIED_DOMAIN_ID;
import static org.openhpi.HpiDataTypes.SaHpiEntityPathT;


class HpiDomain
{
    private long             local_did;
    private long             remote_did;
    private String           remote_host;
    private int              remote_port;
    private SaHpiEntityPathT entity_root;

    HpiDomain( long local_did,
               String remote_host,
               int remote_port,
               SaHpiEntityPathT entity_root )
    {
        this.local_did   = local_did;
        this.remote_did  = SAHPI_UNSPECIFIED_DOMAIN_ID;
        this.remote_host = remote_host;
        this.remote_port = remote_port;
        this.entity_root = entity_root;
    }

    HpiDomain( HpiDomain other )
    {
        this.local_did   = other.local_did;
        this.remote_did  = other.remote_did;
        this.remote_host = other.remote_host;
        this.remote_port = other.remote_port;
        this.entity_root = other.entity_root;
    }

    long getLocalDid()
    {
        return local_did;
    }

    void setLocalDid( long local_did )
    {
        this.local_did = local_did;
    }

    long getRemoteDid()
    {
        return remote_did;
    }

    String getRemoteHost()
    {
        return remote_host;
    }

    int getRemotePort()
    {
        return remote_port;
    }

    SaHpiEntityPathT getEntityRoot()
    {
        return entity_root;
    }
};

