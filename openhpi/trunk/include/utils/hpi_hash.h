/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO:License
 *
 */

#ifndef HASH_H
#define HASH_H


#define HASH_TABLE_MIN_SIZE 11
#define HASH_TABLE_MAX_SIZE 13845163

#define TRUE 1
#define FALSE 0

typedef int (*EqualFunc) (const void *a, const void *b);
typedef void (*DestroyNotify) (void *data);
typedef unsigned int (*HashFunc) (const void *key);
typedef void (*FreeFunc) (void *data);

typedef struct _HashTable HashTable;
typedef struct _HashNode HashNode;

HashTable*		hash_table_new(HashFunc hash_func, 
					EqualFunc key_equal_func,
					DestroyNotify key_destroy_func,
					DestroyNotify value_destroy_func);
static void 		hash_table_resize(HashTable *hash_table);
static HashNode**	hash_table_lookup_node(HashTable *hash_table,
					       const void * key);
void *			hash_table_lookup(HashTable *hash_table, 
		                          const void *key);
static HashNode*	hash_node_new(void *key, void *value);
static void		hash_node_destroy(HashNode *hash_node,
					DestroyNotify key_destroy_func,
					DestroyNotify value_destroy_func);
static void             hash_nodes_destroy (HashNode *hash_node,
                        		FreeFunc  key_destroy_func,
                     			FreeFunc  value_destroy_func);
unsigned int            direct_hash(const void *v);
void 			hash_table_destroy(HashTable *hash_table);
void 			hash_table_insert(HashTable *hash_table,
		                          void *key, void *value);
int			hash_table_steal(HashTable *hash_table,
					 const void *key);

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

#endif /* HASH_H */
