/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: License
 *
 */

#ifndef MACROS_H
#define MACROS_H

#define TRUE 1
#define FALSE 0

typedef int (*EqualFunc) (const void *a, const void *b);
typedef void (*DestroyNotify) (void *data);
typedef unsigned int (*HashFunc) (const void *key);
typedef void (*FreeFunc) (void *data);
typedef int (*CompareDataFunc) (const void *a, 
				const void *b,
				void *user_data);
typedef int (*CompareFunc) (const void *a, const void *b);
typedef void (*Func)	(void *data, void *user_data);

#undef  CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

extern void glib_dummy_decl (void);
#define LOCK_NAME(name)               g__ ## name ## _lock
#ifdef  THREADS_ENABLED
#if 0
#    define LOCK(name) g_static_mutex_lock       (&LOCK_NAME (name))
#    define UNLOCK(name) g_static_mutex_unlock   (&LOCK_NAME (name))
#    define TRYLOCK(name) g_static_mutex_trylock (&_LOCK_NAME (name))
#endif
#else   /* !THREADS_ENABLED */
#  define LOCK_DEFINE_STATIC(name)    extern void glib_dummy_decl (void)
#  define LOCK_DEFINE(name)           extern void glib_dummy_decl (void)
#  define LOCK_EXTERN(name)           extern void glib_dummy_decl (void)
#  define LOCK(name)
#  define UNLOCK(name)
#  define TRYLOCK(name)               (TRUE)
#endif  /* !THREADS_ENABLED */



#endif /* MACROS_H */
