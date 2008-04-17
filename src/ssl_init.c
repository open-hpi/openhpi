/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *		       All rights reserved.
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
 *     Bryan Sutula <Bryan.Sutula@hp.com>
 *
 *
 * This file implements any necessary OpenSSL initialization functions.
 * This is done here so that individual modules and plugins don't have
 * to worry about any global OpenSSL initialization.
 *
 * The following function is provided:
 *
 * oh_ssl_init()		- Intializes the OpenSSL library
 */


/* OpenSSL and other header files */
#include <unistd.h>
#include <glib.h>
#include <oh_ssl_init.h>
#include <oh_error.h>

#ifdef HAVE_OPENSSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#endif


#if defined(_REENTRANT) && defined(HAVE_OPENSSL)
/* Data types used by this module */
struct CRYPTO_dynlock_value {
	GMutex	*mutex;
};
#endif


/* Global (static) data for this module */
static int	oh_ssl_init_done = 0;	/* Will be set true when done */
#if defined(_REENTRANT) && defined(HAVE_OPENSSL)
static GMutex **mutexes = NULL;		/* Holds array of SSL mutexes */
static GStaticMutex ssl_mutexes = G_STATIC_MUTEX_INIT; /* Lock for above */
#endif


/* Local (static) functions, used by this module.  Note that these aren't
 * necessary if we aren't compiling as a threaded implementation, so we
 * skip them in that case.
 */


#if defined(_REENTRANT) && defined(HAVE_OPENSSL)

/**
 * id_function
 *
 * SSL thread ID function
 *
 * Return value: a unique thread identifier, as an unsigned long
 **/
static unsigned long id_function(void)
{
	return((unsigned long) g_thread_self());
}


/**
 * lock_function
 * @mode:	Includes the CRYPTO_LOCK bit if a lock is intended.  Otherwise,
 *		an unlock is desired.
 * @type:	Ordinal number of the mutex being manipulated
 * @file:	(unused)
 * @line:	(unused)
 *
 * SSL mutex lock and unlock function.  This is complicated, somewhat,
 * because we're trying to defer allocation of memory and mutexes until
 * they're actually needed.
 *
 * Note that OpenSSL defines that this function has no error return.  In the
 * case where we can't allocate memory, we'll just have to return, pretending
 * that we did the lock.  This will be a silent failure.  The alternative
 * would be to allocate the array of mutex pointers during thread_setup().
 *
 * Return value: (none)
 **/
static void lock_function(int mode, int type, const char * file, int line)
{
	/* Do we have an array of mutex pointers yet? */
	if (! mutexes) {
		/* Messing with this requires the static lock */
		g_static_mutex_lock(&ssl_mutexes);
		if (! mutexes) {	/* Need to check again */
			mutexes = (GMutex **)g_malloc0(CRYPTO_num_locks() *
						       sizeof(GMutex *));
			if (! mutexes) {
				err("out of memory");
				g_static_mutex_unlock(&ssl_mutexes);
				return;
			}
		}
		g_static_mutex_unlock(&ssl_mutexes);
	}

	/* Have we initialized this particular mutex? */
	if (! mutexes[type]) {
		/* Same firedrill as above */
		g_static_mutex_lock(&ssl_mutexes);
		if (! mutexes[type]) {
			mutexes[type] = g_mutex_new();
		}
		g_static_mutex_unlock(&ssl_mutexes);
	}

	/* Finally, go ahead and lock or unlock it */
	if (mode & CRYPTO_LOCK) {
		g_mutex_lock(mutexes[type]);
	}
	else {
		g_mutex_unlock(mutexes[type]);
	}
}


/**
 * dyn_create_function
 * @file:	(unused)
 * @line:	(unused)
 *
 * Function to create and initialize dynlock mutexes
 *
 * Return value: pointer to dynlock structure, or NULL on failure (out of mem)
 **/
static struct CRYPTO_dynlock_value *dyn_create_function(const char *file,
							int line)
{
	struct CRYPTO_dynlock_value *value;

	if ((value = (struct CRYPTO_dynlock_value *)
			g_malloc(sizeof(struct CRYPTO_dynlock_value)))) {
		value->mutex = g_mutex_new();
	}
	else {
		err("out of memory");
	}

	return(value);
}


/**
 * dyn_lock_function
 * @mode:	Includes the CRYPTO_LOCK bit if a lock is intended.  Otherwise,
 *		an unlock is desired.
 * @l:		Pointer to dynlock structure returned by dyn_create_function()
 * @file:	(unused)
 * @line:	(unused)
 *
 * Function to lock and unlock dynlock mutexes
 *
 * Return value: (none)
 **/
static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l,
			      const char *file, int line)
{
	if (mode & CRYPTO_LOCK) {
		g_mutex_lock(l->mutex);
	}
	else {
		g_mutex_unlock(l->mutex);
	}
}


/**
 * dyn_destroy_function
 * @l:		Pointer to dynlock structure returned by dyn_create_function()
 * @file:	(unused)
 * @line:	(unused)
 *
 * Function to destroy dynlock mutexes
 *
 * Return value: (none)
 **/
static void	dyn_destroy_function(struct CRYPTO_dynlock_value *l,
				     const char *file, int line)
{
	g_mutex_free(l->mutex);
	g_free(l);
}


/**
 * thread_setup
 *
 * Set up multi-thread protection used by the SSL library
 *
 * Return value: 0 for success, -1 for failure
 **/
static int	thread_setup(void)
{
	/* Register our locking functions with the SSL library */
	CRYPTO_set_id_callback(id_function);
	CRYPTO_set_locking_callback(lock_function);
	CRYPTO_set_dynlock_create_callback(dyn_create_function);
	CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
	CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);

	return(0);			/* No errors */
}


#if 0					/* Not used right now */
/**
 * thread_cleanup
 *
 * Clean up multi-thread protection used by the SSL library.
 *
 * Note that this function is not currently used because there is no shutdown
 * code for plugins.  It is left here in case that happens in the future.
 *
 * Return value: 0 for success, -1 for failure (though it currently can't fail)
 **/
static int	thread_cleanup(void)
{
	int		i;

	/* Nullify the locking functions we registered with the SSL library */
	CRYPTO_set_id_callback(NULL);
	CRYPTO_set_locking_callback(NULL);
	CRYPTO_set_dynlock_create_callback(NULL);
	CRYPTO_set_dynlock_lock_callback(NULL);
	CRYPTO_set_dynlock_destroy_callback(NULL);

	/* Clean up and destroy mutexes, if we have any */
	g_static_mutex_lock(&ssl_mutexes);
	if (mutexes) {
		for (i = 0; i < CRYPTO_num_locks(); i++) {
			if (mutexes[i]) {
				g_mutex_free(mutexes[i]);
			}
		}
		g_free(mutexes);
		mutexes = NULL;
	}
	g_static_mutex_free(&ssl_mutexes);

	return(0);			/* No errors */
}
#endif /* Not used right now */
#endif /* _REENTRANT && HAVE_OPENSSL */


/**
 * oh_ssl_init
 *
 * Intialize the OpenSSL library.  Note that the calls used in this routine
 * set up global data and are only to be called once for an SSL-based program.
 * To enforce this while allowing multiple callers (plugins) to initialize
 * the library, we use a static global variable to mark when we've done the
 * initialization.
 *
 * Note that the thread-safe initialization portion requires that
 * g_thread_init() has already been called, so don't call this routine
 * before then.
 *
 * Return value: 0 for success, -1 for failure
 **/
int		oh_ssl_init(void)
{
	if (! oh_ssl_init_done) {	/* Do this only once */
		oh_ssl_init_done = 1;

#ifdef HAVE_OPENSSL
		/* Load error strings to provide human-readable error
		 * messages
		 */
		SSL_load_error_strings();
		ERR_load_BIO_strings();

		/* Initialize the SSL library */
		if (! SSL_library_init()) {
			err("SSL_library_init() failed");
			return(-1);
		}

#ifndef NO_SSL_RAND_SEED		/* In case this isn't portable */
		/* Actions to seed PRNG */
		RAND_load_file("/dev/urandom", 1024);
#endif

#ifdef _REENTRANT
		/* Set up multi-thread protection functions */
		if (thread_setup() ) {
			err("SSL multi-thread protection setup call failed");
			return(-1);
		}
#endif /* _REENTRANT */
#endif /* HAVE_OPENSSL */

	}

	return(0);			/* Successful return */
}
