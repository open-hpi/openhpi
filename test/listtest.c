/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: License
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <hpi_list.h>

void func(void *data, void *user_data);

void func(void *data, void *user_data)
{
   int *i = data;

   printf("data = %d\n", *i);
}

int main(int argc, char *argv[])
{
   List *list = NULL;
   int i;
   int *data;

   for (i = 10; i<20; i++)
   {
      data = (int*) malloc(sizeof (int));
      *data = i;
      list = list_append(list, data );
   }	

   for (i = 0; i<10; i++)
   {
      data = (int*) malloc(sizeof (int));
      *data = i;
      list = list_insert(list, data, i);
   }	

   list_foreach(list, func, NULL);

   list_free(list);

   return 0;
}


