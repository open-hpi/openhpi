/* -*- linux-c -*-
 * 
 * (C) Copyright IBM Corp. 2004, 2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. This
 * file and program are licensed under a BSD style license. See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     Peter D Phan <pdphan@users.sourceforge.net>
 */

#ifndef TSETUP_H
#define TSETUP_H

SaErrorT tsetup (SaHpiSessionIdT *sessionid_ptr);

SaErrorT tfind_resource(SaHpiSessionIdT *sessionid_ptr,
                        SaHpiCapabilitiesT search_rdr_type,
                        SaHpiEntryIdT i_rptentryid,
                        SaHpiRptEntryT *rptentry, 
			SaHpiBoolT samecap);

SaErrorT tfind_resource_by_ep(SaHpiSessionIdT *sessionid_ptr,
			      SaHpiEntityPathT *ep,
			      SaHpiEntryIdT i_rptentryid,
			      SaHpiRptEntryT *rptentry);

SaErrorT tfind_rdr_by_name(SaHpiSessionIdT *sessionid_ptr,
			   SaHpiResourceIdT rid,
			   char *rdr_name,
			   SaHpiRdrT *rdr);
			
SaErrorT tcleanup(SaHpiSessionIdT *sessionid_ptr);

 
#define checkstatus(err, expected_err, testfail) 				\
{										\
    if (err != expected_err) {							\
	printf("Error! Test fails: File=%s, Line=%d\n", __FILE__, __LINE__);	\
	printf("Returned err=%s, expected=%s\n",				\
		oh_lookup_error(err), oh_lookup_error(expected_err));		\
	testfail = -1;								\
    }										\
}

#endif
