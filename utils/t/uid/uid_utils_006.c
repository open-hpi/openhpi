/* -*- linux-c -*-
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
 * Authors:
 *     Renier Morales <renierm@users.sf.net>
 *
 */

#include <SaHpi.h>
#include <glib.h>
#include <uid_utils.h>
#include <epath_utils.h>

/**
 * main: Get 10 new unique ids. Look up ids by ep.
 * Passes if lookups find correct ids,
 * otherwise fails.
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
	SaHpiEntityPathT ep;
	guint id[10], rid, i;

	if (oh_uid_initialize())
		return 1;

	ep_init(&ep);

        for (i = 0; i < 10; i++) {
                ep.Entry[0].EntityInstance = i;
                id[i] = oh_uid_from_entity_path(&ep);
        }

        for (i = 0; i < 10; i++) {
                ep.Entry[0].EntityInstance = i;
                rid = oh_uid_lookup(&ep);
                if (rid != id[i])
                        return 1;
        }

	return 0;
}
