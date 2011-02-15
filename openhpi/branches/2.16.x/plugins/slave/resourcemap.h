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

#ifndef __RESOURCE_MAP_H__
#define __RESOURCE_MAP_H__

#include <map>
#include <vector>

#include <SaHpi.h>

#include "lock.h"


namespace Slave {


/**************************************************************
 * struct ResourceMapEntry
 *************************************************************/
struct ResourceMapEntry
{
    SaHpiResourceIdT slave_rid;
    SaHpiResourceIdT master_rid;
};


/**************************************************************
 * class cResourceMap
 *************************************************************/

class cResourceMap
{
public:

    SaHpiResourceIdT GetMaster( SaHpiResourceIdT slave_rid ) const;
    SaHpiResourceIdT GetSlave( SaHpiResourceIdT master_rid ) const;

protected:

    explicit cResourceMap();
    ~cResourceMap();

    bool IsSlaveKnown( SaHpiResourceIdT slave_rid ) const;

    void AddEntry( SaHpiResourceIdT master_rid, SaHpiResourceIdT slave_rid );
    void RemoveEntry( SaHpiResourceIdT slave_rid );
    void TakeEntriesAway( std::vector<ResourceMapEntry>& entries );

private:

    cResourceMap( const cResourceMap& );
    cResourceMap& operator =( const cResourceMap& );

private:

    // data
    mutable cLock m_lock;
    typedef std::map<SaHpiResourceIdT, SaHpiResourceIdT> OneWayMap;
    OneWayMap m_master_rids;
    OneWayMap m_slave_rids;
};


}; // namespace Slave


#endif // __RESOURCE_MAP_H__

