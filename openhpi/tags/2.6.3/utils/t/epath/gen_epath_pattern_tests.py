#!/usr/bin/env python
import sys

patterns = ['{SYSTEM_CHASSIS,1}{SBC_BLADE,3}',
	    '{SYSTEM_CHASSIS,1}{SBC_BLADE,.}',
	    '{SYSTEM_CHASSIS,1}{SBC_BLADE,.}*',
	    '{SYSTEM_CHASSIS,1}{.,.}',
	    '{SYSTEM_CHASSIS,1}{.,.}*',
	    '{SYSTEM_CHASSIS,1}*',
	    '{SYSTEM_CHASSIS,1}',
	    '{SBC_BLADE,3}',
	    '*{SBC_BLADE,3}',
	    '*{SBC_BLADE,.}',
	    '*{SBC_BLADE,.}*',
	    '*{SYSTEM_CHASSIS,1}{SBC_BLADE,.}*',
	    '*{SYSTEM_CHASSIS,.}{SBC_BLADE,.}',
	    '*{SYSTEM_CHASSIS,.}*{SBC_BLADE,.}',
	    '*{SYSTEM_CHASSIS,.}*{SBC_BLADE,.}*',
	    '{SYSTEM_CHASSIS,.}',
	    '{.,.}',
	    '*{.,.}',
	    '{.,.}*',
	    '*{.,.}*',
	    '*{.,.}*{.,.}',
	    '{.,.}*{.,.}*',
	    '{.,.}{.,.}*',
	    '*{.,.}{.,.}',
	    '*'
	   ]

entitypaths = ['{SYSTEM_CHASSIS,1}{SBC_BLADE,3}',
	       '{SYSTEM_CHASSIS,1}',
	       '{SYSTEM_CHASSIS,1}{SBC_BLADE,3}{ADD_IN_CARD,8}',
	       '{SYSTEM_CHASSIS,1}{PHYSICAL_SLOT,3}{SBC_BLADE,3}{ADD_IN_CARD,8}',
	       '{GROUP,4}{SYSTEM_CHASSIS,1}{PHYSICAL_SLOT,3}{SBC_BLADE,3}{ADD_IN_CARD,8}',
	       '{GROUP,4}{SYSTEM_CHASSIS,1}',
	       '{GROUP,4}{SYSTEM_CHASSIS,1}{SBC_BLADE,3}',
	       '{GROUP,4}{SYSTEM_CHASSIS,1}{SBC_BLADE,3}{ADD_IN_CARD,8}',
	       '{GROUP,4}{SYSTEM_CHASSIS,1}{PHYSICAL_SLOT,3}{SBC_BLADE,3}',
	       '{SBC_BLADE,3}',
	       '{SBC_BLADE,3}{ADD_IN_CARD,8}',
	       '{PHYSICAL_SLOT,4}{SBC_BLADE,3}',
	       '{PHYSICAL_SLOT,4}{SBC_BLADE,3}{ADD_IN_CARD,8}',
	       '{SYSTEM_CHASSIS,1}{PHYSICAL_SLOT,4}{SBC_BLADE,3}',
	       '{GROUP,2}',
	       '{GROUP,2}{SUBRACK,5}',
	       '{GROUP,7}{REMOTE,2}{SUBRACK,4}',
	       '{GROUP,7}{REMOTE,2}{SUBRACK,4}{BIOS,8}'
	      ]

results = [True, True, True, True, True, True, False, False, True, True, True,
           True, True, True, True, False, False, True, True, True, True, True,
           True, True, True, False, False, False, False, False, True, True,
           False, False, False, False, False, False, False, False, True, True,
           True, True, True, False, False, False, False, True, False, False,
           True, False, True, True, False, False, False, False, True, True,
           False, False, True, False, False, True, True, True, True, True,
           True, True, True, False, False, False, False, True, True, False,
           False, False, False, True, False, False, False, True, False, False,
           True, True, True, True, True, True, True, True, False, False, False,
           False, False, False, False, False, False, False, True, False, False,
           False, True, False, False, True, True, True, True, True, True, True,
           True, False, False, False, False, False, False, False, False, False,
           False, False, False, False, False, False, False, False, True, True,
           True, True, True, True, True, True, False, False, False, False,
           False, False, False, False, True, True, True, True, True, True,
           True, False, False, True, True, True, True, True, True, True, True,
           False, False, False, False, False, False, False, False, False,
           False, True, True, False, False, True, False, False, True, True,
           True, True, True, True, True, True, False, False, False, False,
           False, False, False, False, True, True, True, False, False, True,
           True, False, False, True, True, True, True, True, True, True, True,
           False, False, False, False, False, False, False, True, True, True,
           True, False, False, False, False, False, True, True, True, True,
           False, False, False, False, True, False, False, False, False, False,
           False, False, False, False, False, True, False, False, False, False,
           False, False, True, True, True, True, True, True, True, True, False,
           False, False, False, False, False, False, False, True, True, True,
           False, False, False, False, False, False, True, True, True, True,
           True, True, True, True, False, False, False, False, False, False,
           False, False, False, False, True, False, False, False, False,
           False, False, True, True, True, True, True, True, True, True,
           False, False, False, False, True, True, False, False, True, True,
           True, False, False, True, True, False, False, True, True, True,
           True, True, True, True, True, False, False, False, False, False,
           False, False, False, False, False, False, False, False, False,
           False, False, True, True, True, True, False, False, False, False,
           True, False, False, False, False, False, False, False, False, False,
           False, False, False, False, False, False, False, False, True, True,
           True, True, True, True, True, True, False, False, False, False,
           False, False, False, False, False, False, False, False, False,
           False, False, False, False, True, True, True, True, True, True,
           True, True, False, False, False, False, False, False, False, False,
           False, False, False, False, False, False, False, False, False, True,
           True, True, True, True, True, True, True]

header = """/* -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author: Renier Morales <renier@openhpi.org>
 */

#include <SaHpi.h>
#include <oh_utils.h>

/*******************************************************************
 * WARNING! This file is auto-magically generated by:
 *          %s
 *          Do not change this file manually. Update script instead
 *******************************************************************/

/**
 * This takes an entity path and an entity path's pattern,
 * and knowing the proper result beforehand, checks if the
 * pattern matches the entity path. If the proper result is
 * achieved, the test passes.
 **/
int main(int argc, char **argv)
{
""" % sys.argv[0]

footer = """
        return 0;
}

"""

body = """
        char *ep_str = "%(e)s";
        char *epp_str = "%(p)s";
        oh_entitypath_pattern epp;
        SaHpiEntityPathT ep;
        SaErrorT error = SA_OK;
        SaHpiBoolT match = %(m)s;

        error = oh_encode_entitypath(ep_str, &ep);
        if (error) { printf("Encoding of entitypath failed.\\n"); return -1; }

        error = oh_compile_entitypath_pattern(epp_str, &epp);
        if (error) { printf("Compilation of pattern failed.\\n"); return -1; }

        if (oh_match_entitypath_pattern(&epp, &ep) != match)
                return -1;
        
"""

m = 0
for e in entitypaths:
        for p in patterns:
                testfile = open('epath_pattern_%03i.c' % m,'w')
                if not testfile:
                        print 'Error opening file'
                        sys.exit()
                if results[m]: match = 'SAHPI_TRUE'
                else: match = 'SAHPI_FALSE'
                vals = {'e': e, 'p': p, 'm': match}
                testfile.write(header+(body % vals)+footer)
                testfile.close()
                m += 1
