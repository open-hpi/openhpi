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
extern "C" {
#include "SaHpi.h"
}
#include "client.h"

int main (int argc, char *argv [])
{

	SaErrorT 		err;
	SaHpiSessionIdT 	session_id;

	err =  saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &session_id, NULL);
	if (err != SA_OK) {
		printf("test_01: SessionOpen failed\n");
		return (-1);
	} else {
		printf("test_01: SessionOpen \"sid=%d\"\n", session_id);
	}

	char c;
	while (TRUE) {
		printf ("!!!!type any key or 'q' to exit!!!!\n");
		fscanf(stdin,"%c", &c);
		if (c == 'q') {
			err =  saHpiSessionClose(session_id);
			if (err == SA_OK)
				printf("test_01(%d): SessionClose\n", session_id);
			return 0;
		} else {
			err =  saHpiDiscover(session_id);
			if (err != SA_OK) {
				printf("test_01(%d): Discover failed\n", session_id);
				err = saHpiSessionClose(session_id);
				if (err == SA_OK)
					printf("test_01(%d): SessionClose\n", session_id);
				return (-1);
			} else {
				printf("test_01(%d): Discover\n", session_id);
			}
		}
	}

}
