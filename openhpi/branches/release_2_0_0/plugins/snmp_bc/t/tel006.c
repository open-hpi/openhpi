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
 * Author(s):
 *     Peter D Phan <pdphan@users.sourceforge.net>
 */


#include <snmp_bc_plugin.h>
#include <sahpimacros.h>
#include <tsetup.h>

int main(int argc, char **argv) 
{

	/* ************************
	 * Local variables
	 * ***********************/	 
	int testfail = 0;
	SaErrorT          err;
	SaErrorT expected_err;
					
	SaHpiResourceIdT  id = 0;
        SaHpiSessionIdT sessionid;
	SaHpiEventLogEntryIdT current = 0;
	SaHpiEventLogEntryIdT previd;
	SaHpiEventLogEntryIdT nextid;
	SaHpiEventLogEntryT   entry;
	SaHpiRdrT             rdr;
	SaHpiRptEntryT        rptentry;
	
	/************************** 
	 * Test: NULL EventLog cache 
	 **************************/
	struct oh_handler_state handle;
	handle.elcache = NULL;
	
	expected_err = SA_ERR_HPI_INTERNAL_ERROR;                   
	err = snmp_bc_get_sel_entry(&handle, id, current, &previd, &nextid, &entry, &rdr, &rptentry);
	checkstatus(err, expected_err, testfail);

	/***************************
	 * Cleanup after all tests
	 ***************************/
	err = tcleanup(&sessionid);
	return testfail;

}

#include <tsetup.c>
