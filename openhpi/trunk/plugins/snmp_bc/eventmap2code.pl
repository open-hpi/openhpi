#!/usr/bin/perl

##################################################################
# (C) COPYRIGHT IBM Corp 2004
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
#  file and program are licensed under a BSD style license.  See
#  the Copying file included with the OpenHPI distribution for
#  full licensing terms.
#
#  Author(s):
#       Steve Sherman <stevees@us.ibm.com>
###################################################################

###################################################################
# Script Description:
#
# This script takes raw event infomation contained in 
# snmp_bc_event.map and generates code that populates the 
# errlog2event_hash table. This can be done in two ways - 
# with C code or with XML code. The C code way is simpler
# but takes more memory. XML is more complex but takes less space.
# 
# The default way is to generate C code and header files to
# populate the hash table. This generates the following files:
#
# el.h - Error log header file with common #defines
# el2event.h - Header file definitions need by generated C file.
# el2event.c - Generated C code that adds events to directly to
#              the errlog2event_hash table.
#
# The second way is to translate events into XML data which is 
# (in a later step and script) turned into source code that
# populates the hash table. This generates the following files:
#
# el.h - Error log header file with common #defines
# el2event.h - Header file definitions need by generated source.
# event.xml - XML formatted events.
#
# If the XML method is used, another script, scripts/text2cstr.pl, 
# needs to be invoked to turn the XML formatted events into source 
# code that populates the hash table.
#
# Script Input:
#
# --debug     (optional)   Turn on debug info.
#                          Default is no.
# --idir      (optional)   Root directory for input file(s).
#                          Default is current directory.
# --mapfile   (optional)   Input file name of the event map file.
#                          Default is snmp_bc_event.map.
# --odir      (optional)   Directory for output file(s).
#                          Default is current directory.
# --xml       (optional)   Generate XML formatted events.
#                          Default is to generate C formatted
#                          events and code.
#
# Exit codes
# - 1 successful
# - 0 error occurred
####################################################################

use strict;
use Getopt::Long;

sub check4dups($$$);
sub print_h_file_header;
sub print_h_file_ending;
sub print_c_file_header;
sub print_c_file_ending;
sub print_c_file_hash_member($);
sub print_xml_file_header;
sub print_xml_file_ending;
sub print_xml_file_hash_member($);
sub print_err_hfile_header;
sub print_err_hfile_ending;

GetOptions(
  "debug"         => \my $debug,
  "idir=s"        => \my $idir,
  "mapfile=s"     => \my $mapfile,
  "odir=s"        => \my $odir,
  "xml"           => \my $xml,
);

##########################
# Set directory/file names
##########################
if ($idir eq "") {
   $idir = `pwd`;
   chomp $idir;
}
if ($mapfile eq "") {
   $mapfile = "snmp_bc_event.map";
}
my $file_map = $idir . "/$mapfile";

if ($odir eq "") {
   $odir = `pwd`;
   chomp $odir;
}
my $oerror_hfile = "el.h";
my $oevent_hfile = "el2event.h";
my $oevent_cfile = "el2event.c";

if ($xml) {
    $oevent_cfile = "event.xml";
}

my $file_c = $odir . "/$oevent_cfile";
my $file_h = $odir . "/$oevent_hfile";
my $file_err_h = $odir . "/$oerror_hfile";
unlink $file_c;
unlink $file_h;
unlink $file_err_h;

############
# Open files
############
open (FILE_MAP, $file_map) or die "$0 Error! Cannot open $file_map. $! Stopped";
open (FILE_C, ">>$file_c") or die "$0 Error! Cannot open file $file_c. $! Stopped";
open (FILE_H, ">>$file_h") or die "$0 Error! Cannot open file $file_h. $! Stopped";
open (FILE_ERR_H, ">>$file_err_h") or die "$0 Error! Cannot open file $file_err_h. $! Stopped";

#################################################################
# Parse event map file information into internal perl hash tables
#################################################################
my $err = 1;
my %eventmap = ();
my %defmap = ();

while ( <FILE_MAP> ) {

    # Skip comments/blank lines
    next if /^\/\// || /^#.*$/ || /^\s*$/;

    my $line = $_;
    (my $hpidef_event, my $hpidef, my $def) = split/\|/,$line;
    (my $event_name, my $event_hex, my $platforms, my $event_severity,
     my $override_flags, my $event_msg, my $rest) = split/\|/,$line;

    chomp($def);
    chomp($event_msg);

    if ($hpidef_event eq "HPI_DEF") {
	if ($hpidef eq "" || $def eq "" || $defmap{$hpidef} ne "") {
	    print "******************************************************\n";
	    print "$0: Error! Definition $hpidef not found or not unique.\n";
	    print "******************************************************\n\n";
	    $err = 0;
	    goto CLEANUP;
	}
	$defmap{$hpidef} = $def;
    }
    else {
	if ($event_name eq "" || $event_hex eq "" || $platforms eq "" ||
	    $event_severity eq "" || $override_flags eq "" || $event_msg eq "") {
	    print "*************************************************************\n";
	    print "$0: Error! Format for event incomplete for event=$event_name.\n";
	    print "**************************************************************\n\n";
	    $err = 0;
	    goto CLEANUP;
	}
	
	# Check if this string is platform-specific. If it is, tack platform on
	# end of string to make it unique and put into internal hash.
	if ($platforms eq "ALL") {
	    check4dups($line, $event_msg, "ALL");
	}
	else {
	    while ($platforms ne "") {
		my $hash_msg = $event_msg;
		$hash_msg =~ s/\"$//;
		(my $plat, my $platrest) = split/,/,$platforms,2;
		($plat) =~ s/^\s*(.*?)\s*$/$1/; # strip leading/trailing blanks
		$hash_msg = $hash_msg . "_HPIPLAT_" . $plat . "\"";
		check4dups($line, $hash_msg, $plat);
		$platforms = $platrest;
	    }
	}
    }
}

##############################
# Create error log header file
##############################
if (&print_err_hfile_header) { $err = 0; goto CLEANUP; }
foreach my $d (keys %defmap) {
    chomp $defmap{$d};
    print FILE_ERR_H "#define $d $defmap{$d}\n";
}
if (&print_err_hfile_ending) { $err = 0; goto CLEANUP; }

#################################################
# Create "Error Log to event" mapping header file
#################################################
if (&print_h_file_header) { $err = 0; goto CLEANUP; }
if (&print_h_file_ending) { $err = 0; goto CLEANUP; }

################################################
# Create "Error Log 2 event" mapping source file
################################################
if ($xml) {
    if (&print_xml_file_header) { $err = 0; goto CLEANUP; }
    foreach my $event_message (keys %eventmap) {
	if (&print_xml_file_hash_member($event_message)) { $err = 0; goto CLEANUP; }
    }
    if (&print_xml_file_ending) { $err = 0; goto CLEANUP; }
}
else {
    if (&print_c_file_header) { $err = 0; goto CLEANUP; }
    foreach my $event_message (keys %eventmap) {
	if (&print_c_file_hash_member($event_message)) { $err = 0; goto CLEANUP; }
    }
    if (&print_c_file_ending) { $err = 0; goto CLEANUP; }
}

CLEANUP:
close FILE_MAP;
close FILE_C;
close FILE_H;
close FILE_ERR_H;
exit ($err);

##################################################################
# Check for duplicate event messages for a given platform.
# Add _HPIDUP to both the internal hash_msg name and to the
# external event_msg name. HPI code handles stripping the _HPIDUP
# string from the external names.
# The internal tables thus have hash keys that look like:
#   - msg 
#   - msg_HPIDUPx
#   - msg_HPIPLAT_XXX
#   - msg_HPIPLAT_XXX_HPIDUPx
# The platform infomation is used by this script to generate the
# source code.
##################################################################
sub check4dups($$$) {

    my ($line, $hash_msg, $plat) = @_;
 
    my ($event_name, $event_hex, $platforms, $event_severity,
	$override_flags, $event_msg, $rest) = split/\|/,$line;

    chomp($event_msg);

    if ($eventmap{$hash_msg} ne "") {
	if ($debug) {
	    "$0: Warning! $event_msg not unique.\n";
	}
	# Update number of dups in original entry
	my ($num, $entry) = split/\|/,$eventmap{$hash_msg},2;
	my $dups = $num + 1;
	chomp($entry);
	$eventmap{$hash_msg} = $dups . "|$entry";
	
	# Create unique hash entry and HPI string identifier for each duplicate
	my $hashdup = $hash_msg;
	my $msgdup = $event_msg;
	$hashdup =~ s/\"$//; 
	$msgdup  =~ s/\"$//;
	$hashdup = $hashdup  . "_HPIDUP" . $dups . "\"";
	$msgdup  = $msgdup   . "_HPIDUP" . $dups . "\"";
	$eventmap{$hashdup} =
	    "0|$event_name|$event_hex|$plat|$event_severity|$override_flags|$msgdup|$rest";
    }
    else {
	$eventmap{$hash_msg} =
	    "0|$event_name|$event_hex|$plat|$event_severity|$override_flags|$event_msg|$rest";
    }
    
    return 0;
}

#######################################################
# Print "Error Log to Event" header file's leading text 
#######################################################
sub print_h_file_header {

    print FILE_H <<EOF;
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
 */

/******************************************************************* 
 * WARNING! This file is auto-magically generated by:
 *          $0. 
 *          Do not change this file manually. Update script instead.
 *******************************************************************/

#ifndef __EL2EVENT_H
#define __EL2EVENT_H

#define HPIDUP_STRING  "_HPIDUP"

#define NO_OVR  0x0000  /* No overrides */
#define OVR_SEV 0x0001  /* Override Error Log's severity */
#define OVR_RID 0x0010  /* Override Error Log's source */
#define OVR_EXP 0x0100  /* Override Error Log's source for expansion cards */

typedef struct {
        gchar *event;
	SaHpiSeverityT event_sev;
	unsigned short event_ovr;
        short          event_dup;
} ErrLog2EventInfoT;

/* Global \"Error Log to Event\" mapping hash table and usage count */
extern GHashTable *errlog2event_hash;
extern unsigned int errlog2event_hash_use_count;

SaErrorT errlog2event_hash_init(struct snmp_bc_hnd *custom_handle);
SaErrorT errlog2event_hash_free(void);
EOF

    if ($xml) {
	print FILE_H <<EOF;

/* XML event code and mapping structures */
extern char *eventxml;

struct errlog2event_hash_info {
        int platform;
	GHashTable *hashtable;
};
EOF
    }

    return 0;
}

#######################################################
# Print "Error Log to Event" header file's leading text 
#######################################################
sub print_h_file_ending {

    print FILE_H <<EOF;

#endif
EOF

    return 0;
}

####################################
# Print c file's static leading text 
####################################
sub print_c_file_header {

    print FILE_C <<EOF;
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
 */

/******************************************************************* 
 * WARNING! This file is auto-magically generated by:
 *          $0. 
 *          Do not change this file manually. Update script instead.
 *******************************************************************/

#include <glib.h>
#include <string.h>
#include <SaHpi.h>

#include <openhpi.h>
#include <snmp_bc_plugin.h>

GHashTable *errlog2event_hash;

static void free_hash_data(gpointer key, gpointer value, gpointer user_data);

/************************************************************************
 * errlog2event_hash_init:
 * \@custom_handle: Plugin's data pointer.
 * 
 * Initializes the Error Log to event translation hash table.
 *
 * Returns:
 * SA_OK - Normal operation.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory to allocate hash table structures.
 * SA_ERR_HPI_INVALID_PARAMS - \@custom_handle is NULL.
 ************************************************************************/
SaErrorT errlog2event_hash_init(struct snmp_bc_hnd *custom_handle) {

	gchar *key;
/*       gchar *key_exists; */
	ErrLog2EventInfoT *strinfo;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	errlog2event_hash = g_hash_table_new(g_str_hash, g_str_equal);
	if (errlog2event_hash == NULL) {
		dbg("No memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

EOF
    return 0;
}

#####################################
# Print c file's static trailing text 
#####################################
sub print_c_file_ending {

   print FILE_C <<EOF;

	return(SA_OK);
}

/**********************************************************************
 * errlog2event_hash_free:
 *
 * Frees the Error Log to event translation hash table.
 *
 * Returns:
 * SA_OK - Normal operation.
 **********************************************************************/
SaErrorT errlog2event_hash_free()
{
        g_hash_table_foreach(errlog2event_hash, free_hash_data, NULL);
	g_hash_table_destroy(errlog2event_hash);

	return(SA_OK);
}

static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
/*        g_free(key);  keys are string constants - cannot free them */
        g_free(value);
}
EOF

    return 0;
}

#############################
# Print c file's dynamic text 
#############################
sub print_c_file_hash_member($) {
    my ($event_message) = @_;
    
    my ($event_count, $event_name, $event_hex, $event_platform, 
	$event_severity, $override_flags, $event_msg, $rest) = 
	    split/\|/,$eventmap{$event_message};

    chomp($event_msg);

    my $event_hex_str = "\"$event_hex\"";
    $event_hex_str =~ s/^\"0x/\"/;

    my $tab = "";
    if ($event_platform ne "ALL") {
	$tab = "        ";
#	$event_msg =~ s/_HPIPLAT_$event_platform_//;
	if ($event_platform eq "BCT") {
	    print FILE_C "\tif (custom_handle->platform == SNMP_BC_PLATFORM_BCT) {\n";
	}
	elsif ($event_platform eq "BC") {
	    print FILE_C "\tif (custom_handle->platform == SNMP_BC_PLATFORM_BC) {\n";
	}
	elsif ($event_platform eq "RSA") {
	    print FILE_C "\tif (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {\n";
	}
	else {
	    print "******************************************************\n";
	    print "$0: Error! Unrecognized Platform Type=$event_platform.\n";
	    print "Script may need to be updated.\n";
	    print "******************************************************\n\n";
	    return 1;
	}
    }	

    # Format override flags
    if ($override_flags ne "NO_OVR") { 
	$override_flags =~ s/,/ | /g;
    }
    
    print FILE_C <<EOF;
$tab	key = $event_msg;
$tab	strinfo = g_malloc0(sizeof(ErrLog2EventInfoT));
$tab	if (!strinfo) {
$tab		dbg("No memory. Key=%s.", key);
$tab		errlog2event_hash_free();
$tab		return(SA_ERR_HPI_OUT_OF_SPACE);
$tab	}
$tab	strinfo->event = $event_hex_str; /* $event_name */
$tab	strinfo->event_sev = $event_severity;
$tab	strinfo->event_ovr = $override_flags;
$tab	strinfo->event_dup = $event_count;
$tab	g_hash_table_insert(errlog2event_hash, key, strinfo);
EOF
#$tab
#$tab	key_exists = g_hash_table_lookup(errlog2event_hash, key); 
#$tab	if (!key_exists) {
#$tab		strinfo->event = $event_hex_str; /* $event_name */
#$tab		strinfo->event_sev = $event_severity;
#$tab		strinfo->event_ovr = $override_flags;
#$tab		strinfo->event_dup = $event_count;
#$tab		g_hash_table_insert(errlog2event_hash, key, strinfo);
#$tab	}
#$tab	else {
#$tab		dbg("Error!: Key %s defined twice", key);
#$tab		errlog2event_hash_free();
#$tab		return -1;
#$tab	}
#EOF

    if ($event_platform ne "ALL") {
	print FILE_C "\t}\n";
    }

    print FILE_C "\n";

    return 0;
}

############################################
# Print error log header file's leading text
############################################
sub print_err_hfile_header {
    print FILE_ERR_H <<EOF;
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
 */

/******************************************************************* 
 * WARNING! This file is auto-magically generated by:
 *          $0.
 *          Do not change this file manually. Update script instead.
 *******************************************************************/

#ifndef __EL_H
#define __EL_H

EOF

return 0;

}

#############################################
# Print error log header file's trailing text
#############################################
sub print_err_hfile_ending {

    print FILE_ERR_H <<EOF;

#endif
EOF

    return 0;
}

######################################
# Print XML file's static leading text 
######################################
sub print_xml_file_header {

    print FILE_C <<EOF;
<?xml version="1.0"?>
<!--
 (C) Copyright IBM Corp. 2004

/******************************************************************* 
 * WARNING! This file is auto-magically generated by:
 *          $0. 
 *          Do not change this file manually. Update script instead.
 *******************************************************************/
-->
<events>
EOF

    return 0;
}

#######################################
# Print XML file's static trailing text 
#######################################
sub print_xml_file_ending {

   print FILE_C <<EOF;
</events>
EOF

    return 0;
}

###############################
# Print XML file's dynamic text 
###############################
sub print_xml_file_hash_member($) {
    my ($event_message) = @_;
    
    my ($event_count, $event_name, $event_hex, $event_platform,
	$event_severity, $override_flags, $event_msg, $rest) =
	    split/\|/,$eventmap{$event_message};
    chomp($event_msg);
    my $event_hex_str = "\"$event_hex\"";
    $event_hex_str =~ s/^\"0x/\"/;

    # Format override flags
    if ($override_flags ne "NO_OVR") {
	$override_flags =~ s/,/ | /g;
    }
    
    print FILE_C <<EOF;
<event name="$event_name" hex=$event_hex_str type="$event_platform"
       severity="$event_severity" override="$override_flags" dup="$event_count"
       msg=$event_msg />
EOF

    print FILE_C "\n";

    return 0;
}
