/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO:License
 *
 */

#ifndef HASH_H
#define HASH_H

#include <hpi_macros.h>

#define HASH_TABLE_MIN_SIZE 11
#define HASH_TABLE_MAX_SIZE 13845163

typedef struct _HashTable HashTable;
typedef struct _HashNode HashNode;

HashTable*		hash_table_new(HashFunc hash_func, 
					EqualFunc key_equal_func,
					DestroyNotify key_destroy_func,
					DestroyNotify value_destroy_func);
void *			hash_table_lookup(HashTable *hash_table, 
		                          const void *key);
unsigned int            direct_hash(const void *v);
void 			hash_table_destroy(HashTable *hash_table);
void 			hash_table_insert(HashTable *hash_table,
		                          void *key, void *value);
int			hash_table_steal(HashTable *hash_table,
					 const void *key);


#endif /* HASH_H */
