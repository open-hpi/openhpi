/* This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTUTILSLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <algorithm>

#include "timers.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
struct CallbackPred
{
    bool operator ()( const Timer& item ) const
    {
        return item.callback == callback;
    }

    const cTimerCallback * callback;
};

/**********************************************************
 * NB: comparison relies on the fact
 * that x1.tv_usec and x2.tv_usec both < 1000000.
 * I.e. GTimeVal data is valid.
 *********************************************************/
bool operator <( const GTimeVal& x1, const GTimeVal& x2 )
{
    if ( x1.tv_sec != x2.tv_sec ) {
        return x1.tv_sec < x2.tv_sec;
    } else {
        return x1.tv_usec < x2.tv_usec;
    }
}


/**************************************************************
 * class cTimers
 *************************************************************/
cTimers::cTimers()
    : m_thread( 0 ),
      m_cond( g_cond_new() ),
      m_mutex( g_mutex_new() ),
      m_stop( false )
{
}

cTimers::~cTimers()
{
    if ( m_thread ) {
        m_stop = true;
        g_mutex_lock( m_mutex );
        g_cond_signal( m_cond );
        g_mutex_unlock( m_mutex );
        g_thread_join( m_thread );
    }
    g_mutex_free( m_mutex );
    g_cond_free( m_cond );
}

bool cTimers::Start()
{
    m_thread = g_thread_create( ThreadFuncAdapter, this, TRUE, 0 );
//g_usleep( 1000 );

    return ( m_thread != 0 );
}

void cTimers::SetTimer( cTimerCallback * callback, SaHpiTimeoutT timeout )
{
    if ( timeout == SAHPI_TIMEOUT_IMMEDIATE ) {
        callback->TimerEvent();
        return;
    } else if ( timeout == SAHPI_TIMEOUT_BLOCK ) {
        return;
    }

    glong sec  = timeout / 1000000000LL; // TODO possible overflow
    glong usec = ( timeout % 1000000000LL ) / 1000LL;

    Timer timer;
    timer.callback = callback;
    g_get_current_time( &timer.expire );
    timer.expire.tv_sec  += sec;
    timer.expire.tv_usec += usec;
    if ( timer.expire.tv_usec > 1000000L ) {
        ++timer.expire.tv_sec;
        timer.expire.tv_usec -= 1000000L;
    }

    g_mutex_lock( m_mutex );
    m_timers.push_back( timer );
    g_cond_signal( m_cond );
    g_mutex_unlock( m_mutex );
}

void cTimers::CancelTimer( const cTimerCallback * callback )
{
    g_mutex_lock( m_mutex );

    CallbackPred pred;
    pred.callback = callback;
    m_timers.remove_if( pred );

    g_cond_signal( m_cond );
    g_mutex_unlock( m_mutex );
}

bool cTimers::HasTimerSet( const cTimerCallback * callback )
{
    g_mutex_lock( m_mutex );

    CallbackPred pred;
    pred.callback = callback;
    Timers::const_iterator iter = std::find_if( m_timers.begin(), m_timers.end(), pred );
    bool has = iter != m_timers.end();

    g_mutex_unlock( m_mutex );

    return has;
}


gpointer cTimers::ThreadFuncAdapter( gpointer data )
{
    cTimers * me = reinterpret_cast<cTimers *>(data);
    me->ThreadFunc();

    return 0;
}

void cTimers::ThreadFunc()
{
    if ( m_stop ) {
        return;
    }

    g_mutex_lock( m_mutex );

    GTimeVal now, next;

    while ( !m_stop ) {
        g_get_current_time( &next );
        g_time_val_add( &next, 1800000000L /* + 30 min from now */ );
        Timers rest;
        while ( ( !m_stop ) && ( !m_timers.empty() ) ) {
            Timer t = m_timers.front();
            m_timers.pop_front();
            g_get_current_time( &now );
            if ( now < t.expire ) {
                rest.push_back( t );
                if ( t.expire < next ) {
                    next = t.expire;
                }
            } else {
                g_mutex_unlock( m_mutex );
                // TODO Callback can be deleted there
                t.callback->TimerEvent();
                g_mutex_lock( m_mutex );
            }
        }
        if ( m_stop ) {
            break;
        }
        m_timers.swap( rest );
        g_cond_timed_wait( m_cond, m_mutex, &next );
    }

    g_mutex_unlock( m_mutex );
}


}; // namespace TA

