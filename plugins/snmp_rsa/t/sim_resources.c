/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      Steve Sherman <stevees@us.ibm.com>
 *      W. David Ashley <dashley@us.ibm.com>
 */

/**************************************************************************
 * This source file defines the resource arrays declared in sim_resources.h
 *************************************************************************/

#include <glib.h>
#include <snmp_util.h>
#include <sim_resources.h>

struct snmp_rsa_data sim_resource_array[] = {
	/* Add more example event log messages */
        {
		/* Event Log Index Number */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.1.1",
		.mib = {
			.type = ASN_INTEGER,
			.value = {
				.integer = 1,
			},
		},
	},
        {
		/* Event Log Index Number */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.1.2",
		.mib = {
			.type = ASN_INTEGER,
			.value = {
				.integer = 2,
			},
		},
	},
        {
		/* 
		 *  Special End of Log Entry - Simulator ONLY
		 * Code always reads one SNMP OID past end of log. When 
		 * snmp_get returns a negative value, the code knows its read 
		 * the entire error log. This entry allows the simulator to 
		 * force the snmp_get to return a negative value
		 */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.1.3",
		.mib = {
			.type = ASN_INTEGER,
			.value = {
				.integer = SNMP_FORCE_ERROR, /* Force negative return */
			},
		},
	},
        {
		/* Event Log Message */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.2.1",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:48  Text:Remote Login Successful. Login ID:'(SNMP Manager at IP@=192.168.64.5 authenticated).'",
			},
		},
	},
        {
		/* Event Log Message */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.2.2",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:48  Text:Remote Login Successful. Login ID:'(SNMP Manager at IP@=192.168.64.5 authenticated).'",
			},
		},
	},
        {
		/* Clear Event Log */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.3.4.3",
		.mib = {
			.type = ASN_INTEGER,
			.value = {
				.integer = 1, /* write-only */
			},
		},
	},

	{
		/* System Chassis Health */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.7.1.0",
		.mib = {
			.type = ASN_INTEGER,
			.value = {
				.integer = 1, /* unknown=0; good=1; warning=2; bad=3 */
			},
		},
	},
	{
		/* Ambient temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.5.1.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  28.00 Centigrade",
			},
		},
	},
	{
		/* DASD area temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.1.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  28.00 Centigrade",
			},
		},
	},
        {
		/* CPU 1 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.1.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 1 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.1.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 1 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.1.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* CPU 2 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.2.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 2 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.2.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 2 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.2.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* CPU 3 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.3.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 3 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.3.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 3 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.3.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* CPU 4 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.4.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 4 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.4.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 4 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.4.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* CPU 5 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.5.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 5 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.5.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 5 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.5.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* CPU 6 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.6.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 6 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.6.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 6 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.6.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* CPU 7 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.7.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 7 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.7.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 7 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.7.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* CPU 8 temperature */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.8.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  30.00 Centigrade",
			},
		},
	},
        {
		/* CPU 8 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.8.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* CPU 8 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.8.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* DASD 1 temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.1",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 1 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.1",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 1 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.1",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* DASD 2 temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.2",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 2 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.2",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 2 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.2",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* DASD 3 temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.3",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 3 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.3",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 3 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.3",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* DASD 4 temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.4",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 4 Up Critical temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.4",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  95.00 Centigrade",
			},
		},
	},
        {
		/* DASD 4 Up Major temperature */
		.oid = ".1.3.6.1.4.1.2.3.51.1.6.1.1.3.4",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "  85.00 Centigrade",
			},
		},
	},
        {
		/* Fan 1 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.1.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},
        {
		/* Fan 2 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.2.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},
        {
		/* Fan 3 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.3.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},
        {
		/* Fan 4 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.4.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},
        {
		/* Fan 5 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.5.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},
        {
		/* Fan 6 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.6.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},
        {
		/* Fan 7 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.7.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},
        {
		/* Fan 8 Speed */
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.8.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = " 67% of maximum",
			},
		},
	},

/* Need to test 0 length, maximum, and max +1 strings here */

        {
		/* Chassis Product Name VPD */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.1.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "8687",
			},
		},
	},
        {
		/* Chassis Model Number VPD */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.2.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "1RX",
			},
		},
	},
        {
		/* Chassis Serial Number VPD */	
		.oid = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.3.0",
		.mib = {
			.type = ASN_OCTET_STR,
			.value = {
				.string = "78R4432",
			},
		},
	},

	{} /* Terminate array with a null element */
};
