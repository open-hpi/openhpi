/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      W. David Ashley <dashley@us.ibm.com>
 *
 */


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

#include "strmsock.h"


/*--------------------------------------------------------------------*/
/*                                                                    */
/* Function: main                                                     */
/*                                                                    */
/* Purpose:  Entry point from the operating system.                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

int main (
   int argc,
   char *argv [])
   {
   pcstrmsock clientinst;
   char *host;

   clientinst = new cstrmsock;

   if (argc > 1)
      host = argv[1];
   else
      host = "localhost";

   if (clientinst -> Open(host, 55566))
      {
      printf("Could not open client socket.\n");
      return 8;
      }
   else 
      printf("Client instance created.\n");

   clientinst -> WriteStr("Client message.");
   printf("Message sent to server.\n");

   clientinst -> Close();
   printf("Client socket closed.\n");

   delete clientinst;
   printf("Client instance deleted.\n");

   return 0;
   }

