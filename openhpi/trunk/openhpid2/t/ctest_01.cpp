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

int main (int argc, char *argv []) {
	SaErrorT 		err;
	SaHpiSessionIdT 	session_id;

	err =  saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &session_id, NULL);
	if (err != SA_OK) {
		printf("%s: saHpiSessionOpen failed\n", argv[0]);
		printf("%s: the daemon may not be running\n", argv[0]);
		return (-1);
	}

	err =  saHpiDiscover(session_id);
	if (err != SA_OK) {
		printf("%s: saHpiDiscover failed\n", argv[0]);
		return (-1);
	}

	err =  saHpiSessionClose(session_id);
	if (err != SA_OK) {
		printf("%s: saHpiSessionClose failed\n", argv[0]);
		return (-1);
	}

	return 0;
}
