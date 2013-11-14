/*      -*- linux-c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include "resourcemap.h"


namespace Slave {


/**************************************************************
 * class cResourceMap
 *************************************************************/
cResourceMap::cResourceMap()
{
    // empty
}

cResourceMap::~cResourceMap()
{
    // empty
}

bool
cResourceMap::IsSlaveKnown( SaHpiResourceIdT slave_rid ) const
{
    if ( slave_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return true;
    }

    cLocker locker( m_lock );
    OneWayMap::const_iterator iter = m_master_rids.find( slave_rid );
    return ( iter != m_master_rids.end() );
}

void
cResourceMap::AddEntry( SaHpiResourceIdT master_rid,
                        SaHpiResourceIdT slave_rid )
{
    if ( master_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return;
    }
    if ( slave_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return;
    }

    cLocker locker( m_lock );
    m_master_rids[slave_rid] = master_rid;
    m_slave_rids[master_rid] = slave_rid;
}

void
cResourceMap::RemoveEntry( SaHpiResourceIdT slave_rid )
{
    if ( slave_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return;
    }

    cLocker locker( m_lock );
    OneWayMap::iterator iter = m_master_rids.find( slave_rid );
    if ( iter != m_master_rids.end() ) {
        SaHpiResourceIdT master_rid = iter->second;
        m_slave_rids.erase( master_rid );
        m_master_rids.erase( iter );
    }
}

void
cResourceMap::TakeEntriesAway( std::vector<ResourceMapEntry>& entries )
{
    cLocker locker( m_lock );

    ResourceMapEntry entry;
    OneWayMap::const_iterator iter, end;
    for ( iter = m_master_rids.begin(), end = m_master_rids.end(); iter != end; ++iter ) {
        entry.slave_rid = iter->first;
        entry.master_rid = iter->second;
        entries.push_back( entry );
    }

    m_slave_rids.clear();
    m_master_rids.clear();
}

SaHpiResourceIdT
cResourceMap::GetMaster( SaHpiResourceIdT slave_rid ) const
{
    if ( slave_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return SAHPI_UNSPECIFIED_RESOURCE_ID;
    }

    cLocker locker( m_lock );
    OneWayMap::const_iterator iter = m_master_rids.find( slave_rid );
    if ( iter != m_master_rids.end() ) {
        return iter->second;
    } else {
        return SAHPI_UNSPECIFIED_RESOURCE_ID;
    }
}

SaHpiResourceIdT
cResourceMap::GetSlave( SaHpiResourceIdT master_rid ) const
{
    if ( master_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return SAHPI_UNSPECIFIED_RESOURCE_ID;
    }

    cLocker locker( m_lock );
    OneWayMap::const_iterator iter = m_slave_rids.find( master_rid );
    if ( iter != m_slave_rids.end() ) {
        return iter->second;
    } else {
        return SAHPI_UNSPECIFIED_RESOURCE_ID;
    }
}


}; // namespace Slave

