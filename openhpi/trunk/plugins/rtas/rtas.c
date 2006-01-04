/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Renier Morales <renierm@users.sf.net>
 */

#include <rtas.h>

void *rtas_open(GHashTable *handler_config)
{
        return NULL;
}

void rtas_close(void *hnd)
{
        return;
}

				  			
/* Function ABI aliases */
void * oh_open (GHashTable *) __attribute__ ((weak, alias("rtas_open")));
void * oh_close (void *) __attribute__ ((weak, alias("rtas_close")));
