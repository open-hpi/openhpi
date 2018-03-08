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

#ifndef __LOCK_H__
#define __LOCK_H__

#include <glib.h>
#include "sahpi_wrappers.h"

namespace Slave {


/**************************************************************
 * class cLock
 *************************************************************/
class cLock
{
public:

    friend class cLocker;

    explicit cLock()
    {
        m_mutex = wrap_g_mutex_new_init();
    }

    ~cLock()
    {
        wrap_g_mutex_free_clear( m_mutex );
    }

    void Lock()
    {
        g_mutex_lock( m_mutex );
    }

    void Unlock()
    {
        g_mutex_unlock( m_mutex );
    }

private:

    cLock( const cLock& );
    cLock& operator =( const cLock& );

    // data
    GMutex * m_mutex;
};


/**************************************************************
 * class cLocker
 *************************************************************/
class cLocker
{
public:

    explicit cLocker( GMutex * mutex )
        : m_mutex( mutex )
    {
        g_mutex_lock( m_mutex );
    }

    explicit cLocker( cLock& lock )
        : m_mutex( lock.m_mutex )
    {
        g_mutex_lock( m_mutex );
    }

    ~cLocker()
    {
        g_mutex_unlock( m_mutex );
    }

private:

    cLocker( const cLocker& );
    cLocker& operator =( const cLocker& );

    // data
    GMutex * m_mutex;
};


}; // namespace Slave


#endif // __LOCK_H__

