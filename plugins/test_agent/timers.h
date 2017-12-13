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

#ifndef TIMERS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define TIMERS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>

#include <glib.h>

#include "SaHpi.h"


namespace TA {


/**************************************************************
 * class cTimerCallback
 *************************************************************/
class cTimerCallback
{
public:

    explicit cTimerCallback()
    {
        // empty
    }

    virtual void TimerEvent() = 0;

protected:

    virtual ~cTimerCallback()
    {
        // empty
    }

private:

    cTimerCallback( const cTimerCallback& );
    cTimerCallback& operator =( const cTimerCallback& );
};


/**************************************************************
 * struct Timer
 *************************************************************/
#if GLIB_CHECK_VERSION (2, 32, 0)
struct Timer
{
    cTimerCallback * callback;
    gint64 expire;
};
#else
struct Timer
{
    cTimerCallback * callback;
    GTimeVal         expire;
};
#endif


/**************************************************************
 * class cTimers
 *************************************************************/
class cTimers
{
public:

    explicit cTimers();
    ~cTimers();

    bool Start();
    void SetTimer( cTimerCallback * callback, SaHpiTimeoutT timeout );
    void CancelTimer( const cTimerCallback * callback );
    bool HasTimerSet( const cTimerCallback * callback );

private:

    cTimers( const cTimers& );
    cTimers& operator =( const cTimers& );

    static gpointer ThreadFuncAdapter( gpointer data );
    void ThreadFunc();

private: //data

    typedef std::list<Timer> Timers;

    GThread *     m_thread;
    GCond *       m_cond;
    GMutex *      m_mutex;
    volatile bool m_stop;
    Timers        m_timers;
};

}; // namespace TA


#endif // TIMERS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

