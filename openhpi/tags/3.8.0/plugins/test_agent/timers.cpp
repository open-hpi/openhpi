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

// #include <oh_error.h>
#include "timers.h"

#include "sahpi_wrappers.h"

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
      m_cond( wrap_g_cond_new_init() ),
      m_mutex( wrap_g_mutex_new_init() ),
      m_stop( false )
{
}

cTimers::~cTimers()
{
    if ( m_thread ) {
        m_stop = true;
        wrap_g_mutex_lock( m_mutex );
        g_cond_signal( m_cond );
        wrap_g_mutex_unlock( m_mutex );
        g_thread_join( m_thread );
    }
    wrap_g_mutex_free_clear( m_mutex );
    wrap_g_cond_free( m_cond );
}

bool cTimers::Start()
{
    m_thread = wrap_g_thread_create_new( "Start", ThreadFuncAdapter, this, TRUE, 0 );
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

    #if GLIB_CHECK_VERSION (2, 32, 0)
        gint64 usec = timeout / 1000LL; // Just convert to microseconds
	// CRIT("GLIB > 2.32 and timeout in usec is %li\n", usec);
    #else 
        glong sec  = timeout / 1000000000LL; // TODO possible overflow
        glong usec = ( timeout % 1000000000LL ) / 1000LL;
	// CRIT("GLIB < 2.32 and timeout is %ld secs and %ld usecs\n", sec, usec);
    #endif

    Timer timer;
    timer.callback = callback;
    #if GLIB_CHECK_VERSION (2, 32, 0)
    timer.expire = g_get_monotonic_time();
    timer.expire += usec;
    #else
    g_get_current_time( &timer.expire );
    timer.expire.tv_sec  += sec;
    timer.expire.tv_usec += usec;
    if ( timer.expire.tv_usec > 1000000L ) {
        ++timer.expire.tv_sec;
        timer.expire.tv_usec -= 1000000L;
    }
    #endif

    wrap_g_mutex_lock( m_mutex );
    m_timers.push_back( timer );
    g_cond_signal( m_cond );
    wrap_g_mutex_unlock( m_mutex );
}

void cTimers::CancelTimer( const cTimerCallback * callback )
{
    wrap_g_mutex_lock( m_mutex );

    CallbackPred pred;
    pred.callback = callback;
    m_timers.remove_if( pred );

    g_cond_signal( m_cond );
    wrap_g_mutex_unlock( m_mutex );
}

bool cTimers::HasTimerSet( const cTimerCallback * callback )
{
    wrap_g_mutex_lock( m_mutex );

    CallbackPred pred;
    pred.callback = callback;
    Timers::const_iterator iter = std::find_if( m_timers.begin(), m_timers.end(), pred );
    bool has = iter != m_timers.end();

    wrap_g_mutex_unlock( m_mutex );

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

    wrap_g_mutex_lock( m_mutex );

    #if GLIB_CHECK_VERSION (2, 32, 0)
    gint64 now, next;
    #else
    GTimeVal now, next;
    #endif

    while ( !m_stop ) {
        Timers rest;
        #if GLIB_CHECK_VERSION (2, 32, 0)
        next = g_get_monotonic_time();
        next = next + (1800000000L /* + 30 min from now */ );
        while ( ( !m_stop ) && ( !m_timers.empty() ) ) {
            Timer t = m_timers.front();
            m_timers.pop_front();
            now = g_get_monotonic_time();
            // CRIT("GLIB>2.32 now=%li front=%li\n",now, t.expire);
            if ( now < t.expire ) {
                rest.push_back( t );
                if ( t.expire < next ) {
                    next = t.expire;
                }
            } else {
                wrap_g_mutex_unlock( m_mutex );
                // TODO Callback can be deleted there
                t.callback->TimerEvent();
                wrap_g_mutex_lock( m_mutex );
            }
        }        
        #else
        g_get_current_time( &next );
        g_time_val_add( &next, 1800000000L /* + 30 min from now */ );

        while ( ( !m_stop ) && ( !m_timers.empty() ) ) {
            Timer t = m_timers.front();
            m_timers.pop_front();
            g_get_current_time( &now );
            // CRIT("GLIB<2.32 now %ld secs, %ld usecs t %ld secs %ld usecs\n",
	    //	now.tv_sec, no.tv_usec, t.expire.tv_sec, t.expire.tv_usec);
            if ( now < t.expire ) {
                rest.push_back( t );
                if ( t.expire < next ) {
                    next = t.expire;
                }
            } else {
                wrap_g_mutex_unlock( m_mutex );
                // TODO Callback can be deleted there
                t.callback->TimerEvent();
                wrap_g_mutex_lock( m_mutex );
            }
        }
        #endif
        if ( m_stop ) {
            break;
        }
        m_timers.swap( rest );
        #if GLIB_CHECK_VERSION (2, 32, 0)
        wrap_g_cond_timed_wait( m_cond, m_mutex, next );
        #else
        wrap_g_cond_timed_wait( m_cond, m_mutex, &next );
        #endif
    }

    wrap_g_mutex_unlock( m_mutex );
}


}; // namespace TA

