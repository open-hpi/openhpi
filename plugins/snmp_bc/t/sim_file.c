/*      -*- linux-c -*-
 * Note: This is a prototype program, not all functions are completely
 *       coded or tested. At the end is a sample Makefile and input file.
 *
 * Main function:
 *   Parse a file, either pointed to by an environmental variable or
 * default file name, to build a hash table that will be used by
 * bladecenter simulator to simulate resource access.
 *
 *   Each line in input file represents an entry in the hash table
 * for example:
 * .1.3.6.1.4.1.2.3.51.2.2.3.1.0 = STRING: "    46% of maximum"
 *    ".1.3.6..." is the key into hash table
 *    STRING" and "46% of maximum" are values associated to this key 
 *
 *   Input file can be hand made or the output of snmp walk, such as:
 * snmpwalk -v 3 -t 10 -l authNoPriv -A netsnmpv3 -n "" -u $user $host -Of -On .1
 *
 *   Signature is recorded after an input file is processed, it is to
 * avoid a file getting processed more than once.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
static  unsigned int file_sig = 0;
#define PRTSUBSTR 							\
	ii = 0;								\
	while ( (tmpstr = substring[ii]) ) {				\
	printf("substring %d = %s\n", ii, tmpstr);			\
	ii++; }
	
main()
{
	#define MAX_STR_LEN  1024
	#define STD_STR_CNT  4
	FILE    *file;
        char    *file_in;
        char    file_std[] = "./test_file";
	char	str_in[MAX_STR_LEN];
	gchar	oid_hdr[] = ".1.3.6.1.4.1.2.3.51.2";
	gchar	str_type[] ="STRING:";
	gchar	int_type[] ="INTEGER:";
	gchar	**substring = NULL;
	gchar	*tmpstr = NULL;
        const   gchar  *str_delimiter = " ";
	int	valid=0, invalid=0, total=0, ch=0, sub_str_cnt=0, ii=0;
	unsigned int sum_ck = 0;

        file_in = getenv("TEST_FILE");
        if (!file_in)  file_in = file_std; 
        printf("file to be tested - %s\n", file_in);      
	file = fopen( file_in, "r");
	if ( file == NULL ) {
		printf("file %s open failed\n", file_in);
		goto cleanup;
	}
	clearerr( file );
	sum_ck = 0;
	//while ( !feof(file) )  sum_ck += getc(file);
	while ( (ch = getc(file)) != EOF) sum_ck += ch;
	clearerr( file );
	rewind(file);
	printf("file checksum=%d, cached signature=%d\n", sum_ck, file_sig);
	if ( sum_ck == file_sig ) {
		printf("file already processed\n");
		goto cleanup;
	}
	while ( !feof(file) )  {
		fgets(str_in, MAX_STR_LEN, file);
		printf("%s", str_in);
		g_strstrip(str_in);
		if (str_in[0] != '\0') {
			substring = g_strsplit(str_in, str_delimiter, STD_STR_CNT);
			for (sub_str_cnt=0; substring[sub_str_cnt]; sub_str_cnt++);
			printf("number of sub-strings = %d\n", sub_str_cnt);
			if (sub_str_cnt==STD_STR_CNT) {
				tmpstr = strstr(substring[0], oid_hdr);
				if (tmpstr == substring[0]) {
					if (!g_ascii_strcasecmp(substring[2],int_type)){
						printf("got a int type\n");
						//  todo: process int type
						valid++;
					}
					else if (!g_ascii_strcasecmp(substring[2],str_type)){
						printf("got a string type\n");
						// todo: process string type
						valid++;
					}
					else {
						printf("not a valid type\n");
					}
				}
			}
			else {
				printf("not a valid line\n");
			}
			PRTSUBSTR;
			g_strfreev(substring);
		}
		total++;
	}
	printf("%d out of %d lines in file %s got processed\n", valid, total, file_in);
	fclose( file );
	file_sig = sum_ck; // todo: record signature only for valid case

cleanup:
	exit(0);
}

/* Sample Makefile for stand alone testing
all:
        gcc -I/usr/include/glib-2.0 -I/usr/include/glib-2.0/include -I/usr/include/glib-2.0/glib -I/usr/lib/glib-2.0/include -lglib-2.0 sim_file.c -o test
*/
/* Sample input file
.1.3.6.1.2.1.1.2.0 = OID: .1.3.6.1.4.1.2.6.158.3
.1.3.6.1.4.1.2.3.51.2.2.3.1.0 = STRING: "    46% of maximum"
.1.3.6.1.4.1.2.3.51.2.2.5.2.6.0 = INTEGER: 0
.1.3.6.1.4.1.2.3.51.2.2.21.8.1.1.9.1 = Hex-STRING: D0 07 F0 D2
.1.3.6.1.4.1.2.3.51.2.2.21.21.1.7.14 = ""
.1.3.6.1.4.1.2.3.51.2.22.5.1.1.3.1 = IpAddress: 9.3.202.71
.1.3.6.1.6.3.11.2.1.1.0 = Counter32: 0
.1.3.6.1.6.3.12.1.1.0 = INTEGER: 0
.1.3.6.1.6.3.12.1.2.1.2.72.111.115.116.49 = Wrong Type (should be OBJECT IDENTIFIER): INTEGER: 1
*/
