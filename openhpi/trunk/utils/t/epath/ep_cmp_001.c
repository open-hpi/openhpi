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
 *     Chris Chia <cchia@users.sf.net>
 *
 */

#include <string.h>
#include <stdio.h>
#include <SaHpi.h>
#include <epath_utils.h>

/**
 * ep_cmp test1
 *   zero element entity path comarison, expect success
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep1 = {{{0}}};
        SaHpiEntityPathT ep2 = {{{0}}};
        int mydebug = 0;

        if (ep_cmp(&ep1, &ep2)) {
                if (mydebug) printf("ep_cmp test1 failed\n");
                return 1;
        }

        if (mydebug) printf("ep_cmp test1 OK\n");
        return 0;
}
