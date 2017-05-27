/*      
 *
 * Copyright (C) 2013, Hewlett-Packard Development Company, LLP
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

#ifndef __SAHPI_WRAPPERS_H
#define __SAHPI_WRAPPERS_H

#include <glib.h>
#ifdef __cplusplus
extern "C" {
#endif

GThread *wrap_g_thread_create_new(const gchar *name, GThreadFunc func, 
                         gpointer data, gboolean joinable, GError **error);
GMutex* wrap_g_mutex_new_init(void);
void wrap_g_mutex_free_clear(GMutex *mutex);
void wrap_g_mutex_lock(GMutex *mutex);
gboolean wrap_g_mutex_trylock(GMutex *mutex);
void wrap_g_mutex_unlock(GMutex *mutex);
void wrap_g_thread_init(gpointer nul);
void wrap_g_static_rec_mutex_lock(void *mutex);
gboolean wrap_g_static_rec_mutex_trylock (void  *mutex);
void wrap_g_static_rec_mutex_unlock( void  *mutex);
void wrap_g_static_private_init(void *key);
void wrap_g_static_private_free(void * key);
#if GLIB_CHECK_VERSION (2, 32, 0)
void wrap_g_static_private_set(void* key, gpointer value);
#else
void wrap_g_static_private_set(void* key, gpointer value, GDestroyNotify notify);
#endif
gpointer wrap_g_static_private_get(void *key);
void wrap_g_static_rec_mutex_init(void  *mutex);
void wrap_g_static_rec_mutex_free_clear(void *mutex);
#if GLIB_CHECK_VERSION (2, 32, 0)
gpointer wrap_g_async_queue_timed_pop(GAsyncQueue *queue, guint64 end_time);
#else
gpointer wrap_g_async_queue_timed_pop(GAsyncQueue *queue, GTimeVal *end_time);
#endif
#if GLIB_CHECK_VERSION (2, 32, 0)
gboolean wrap_g_cond_timed_wait (GCond *cond, GMutex *mutex, gint64 abs_time);
#else
gboolean wrap_g_cond_timed_wait (GCond *cond, GMutex *mutex, GTimeVal *abs_time);
#endif
GCond* wrap_g_cond_new_init(void);
void wrap_g_cond_free (GCond *cond);
void wrap_g_static_mutex_init (void *mutex);
void wrap_g_static_mutex_free_clear(void *mutex);
void wrap_g_static_mutex_unlock(void *mutex);
void wrap_g_static_mutex_lock(void *mutex);
#define wrap_g_free(ptr) g_free(ptr); ptr=NULL
#define wrap_free(ptr) free(ptr); ptr=NULL

#ifdef __cplusplus
} /* extern "C" */
#endif



#endif /* __SAHPI_WRAPPERS_H */
