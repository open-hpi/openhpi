
/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: License
 */

#include <stdio.h>
#include <hpi_hash.h>
#include <safhpi.h>

typedef struct value {
   unsigned int data;
   char str[255];
} Value;

int key_compare_func(const void *a, const void *b)
{
   return (a==b) ? 1 : 0;
}

void destroy_key_func(void *data)
{
   printf("in destroy_key_func\n");
   

}
void destroy_value_func (void *data)
{
   Value *val;

   printf("in destroy_value_func\n");
   if (NULL != data)
   {
      val = (Value*) data;
      printf("value->data = %d\n", val->data);
      printf("value->str = %s\n", val->str);
 
      free(data);
   }
}

int main(int argc, char *argv[])
{
   HashTable 	*htable;
   Value 	*value;
   unsigned int  key;
   int 		 i;

   printf("creating hash table\n");
   htable = hash_table_new(NULL,
		           key_compare_func,
			   destroy_key_func, 
			   destroy_value_func);

   if ( NULL == htable)
   {
      fprintf(stderr, "hash_table_new failed \n");
      return -1;
   }
  
   for (i = 0; i<20; i++)
   {
      key = i;
      value = (Value *)malloc(sizeof(Value)); 
 
      if (NULL == value)
      {
         printf("Failed to allocate data\n");
	 return -1;
      }

      value->data = i;
      sprintf(value->str, "entry %d was added", i);

      printf("adding entry %d\n",i);
      hash_table_insert(htable, key, value);
   } 

   for (i=0 ; i<20; i++)
   {
      value = hash_table_lookup(htable, i);
      if (NULL != value)
      {
         printf("value->data = %d\n", value->data);
         printf("value->str = %s\n", value->str);
      }
   }

   printf("destroying hash table\n");
   hash_table_destroy(htable);
}


