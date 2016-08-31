/*      
 *
 * Copyright (C) 2014, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Mohan Devarajulu <mohan@fc.hp.com>
 */

#include <glib.h>
#include <oh_error.h>
#include "sahpi_wrappers.h"



GThread *wrap_g_thread_create_new(const gchar *name, GThreadFunc func, 
                         gpointer data, gboolean joinable, GError **error)
{
        GThread *thrd = NULL;
        #if GLIB_CHECK_VERSION (2, 32, 0)
               thrd = g_thread_new(name, func, data); 
        #else
               thrd = g_thread_create(func, data, joinable, error);
        #endif
        return(thrd);
}

/**
 * wrap_g_mutex_new_init()
 *
 * Purpose:    
 *     Wrapper function to get a mutex structure allocated and initialized
 *
 * Detailed Description: NA
 *     Call g_mutex_init(GMutex *) if the glib version is > 2.31.0 
 *     OR
 *     Call g_mutex_new() if the glib version is > 2.31.0 
 *
 * Return Values:
 *    Gmutex pointer is returned. It could be NULL if memory is not allocated
 *
 **/
GMutex* wrap_g_mutex_new_init()
{
        GMutex *mutx = NULL;
        #if GLIB_CHECK_VERSION (2, 32, 0)
               mutx = (GMutex*)g_malloc0(sizeof(GMutex));
               if (mutx == NULL) {
                      err("GMutex allocation failed. Continuing");
                      return NULL;
               }
               g_mutex_init(mutx);
        #else
               mutx = g_mutex_new();
        #endif
        return mutx;
}	
	
void wrap_g_mutex_free_clear(GMutex *mutex)
{

        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_mutex_clear(mutex);
        #else
               g_mutex_free(mutex);
        #endif
}


void wrap_g_mutex_lock(GMutex *mutex)
{
        g_mutex_lock(mutex);
}

gboolean wrap_g_mutex_trylock(GMutex *mutex)
{
        return(g_mutex_trylock(mutex));
}


void wrap_g_mutex_unlock(GMutex *mutex)
{
        g_mutex_unlock(mutex);        
}

void wrap_g_thread_init(gpointer nul) 
{

        #if GLIB_CHECK_VERSION (2, 32, 0)
               dbg("g_thread_init not needed in glib>2.31)");
        #else
               g_thread_init(nul);
        #endif
}

void wrap_g_static_rec_mutex_lock(void *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_rec_mutex_lock ((GRecMutex *)mutex);
        #else
               g_static_rec_mutex_lock((GStaticRecMutex *)mutex);
        #endif
}

gboolean wrap_g_static_rec_mutex_trylock(void  *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               return(g_rec_mutex_trylock ((GRecMutex *)mutex));
        #else
               return(g_static_rec_mutex_trylock ((GStaticRecMutex *)mutex));
        #endif
}

void wrap_g_static_rec_mutex_unlock ( void  *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_rec_mutex_unlock ((GRecMutex *)mutex);
        #else
               g_static_rec_mutex_unlock((GStaticRecMutex *)mutex);
        #endif
}

void wrap_g_static_private_init(void *key)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               err("Not expected to be here");
        #else
               g_static_private_init((GStaticPrivate *)key);
        #endif
}

void wrap_g_static_private_free(void * key)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               ;
        #else
               g_static_private_free((GStaticPrivate*) key);
        #endif
}

#if GLIB_CHECK_VERSION (2, 32, 0)
void wrap_g_static_private_set(void * key, gpointer value)
{
        g_private_set((GPrivate*) key, value);
}
#else
void wrap_g_static_private_set(void * key, gpointer value, GDestroyNotify notify)
{
        g_static_private_set((GStaticPrivate*) key, value, notify);
}
#endif

gpointer wrap_g_static_private_get(void *key)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
              return g_private_get((GPrivate*) key);
        #else
              return g_static_private_get((GStaticPrivate *)key);
        #endif
}

void wrap_g_static_rec_mutex_init(void  *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_rec_mutex_init((GRecMutex *)mutex);
        #else
               g_static_rec_mutex_init((GStaticRecMutex *)mutex);
        #endif
}

void wrap_g_static_rec_mutex_free_clear(void *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_rec_mutex_clear((GRecMutex *)mutex);
        #else
               g_static_rec_mutex_free((GStaticRecMutex *)mutex);
        #endif
}

#if GLIB_CHECK_VERSION (2, 32, 0)
gpointer wrap_g_async_queue_timed_pop(GAsyncQueue *queue, guint64 end_time)
{
        return(g_async_queue_timeout_pop(queue, end_time));
}
#else
gpointer wrap_g_async_queue_timed_pop(GAsyncQueue *queue, GTimeVal *end_time)
{
        return(g_async_queue_timed_pop(queue, end_time));
}
#endif

#if GLIB_CHECK_VERSION (2, 32, 0)
gboolean wrap_g_cond_timed_wait (GCond *cond, GMutex *mutex, gint64 abs_time)
{
        return(g_cond_wait_until (cond, mutex, abs_time));
}
#else
gboolean wrap_g_cond_timed_wait (GCond *cond, GMutex *mutex, GTimeVal *abs_time)
{
        return(g_cond_timed_wait (cond, mutex, abs_time));
}
#endif

GCond* wrap_g_cond_new_init()
{
        GCond *cond;
        #if GLIB_CHECK_VERSION (2, 32, 0)
               cond = g_malloc(sizeof(GCond));
               g_cond_init(cond);
        #else
               cond=g_cond_new();
        #endif
        return cond;
}

void wrap_g_cond_free (GCond *cond)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_free(cond);
               cond=NULL;
        #else
               g_cond_free(cond);
        #endif
}


void wrap_g_static_mutex_init (void *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_mutex_init((GMutex *)mutex);
        #else
               g_static_mutex_init((GStaticMutex *)mutex);
        #endif
}

void wrap_g_static_mutex_free_clear(void *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_mutex_clear((GMutex*)mutex);
        #else
               g_static_mutex_free((GStaticMutex*)mutex);
        #endif
}
void wrap_g_static_mutex_unlock(void *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_mutex_unlock((GMutex*)mutex);
        #else
               g_static_mutex_unlock((GStaticMutex*)mutex);
        #endif
}
void wrap_g_static_mutex_lock(void *mutex)
{
        #if GLIB_CHECK_VERSION (2, 32, 0)
               g_mutex_lock((GMutex*)mutex);
        #else
               g_static_mutex_lock((GStaticMutex*)mutex);
        #endif
}
