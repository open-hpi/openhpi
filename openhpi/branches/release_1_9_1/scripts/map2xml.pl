#!/usr/bin/perl

#######################################################################
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
#       W. David Ashley <dashley@us.ibm.com>
########################################################################

########################################################################
# Script Description:
#
# Script Input:
#
# --debug     (optional)   Turn on debug info.
#                          Default is no.
# --idir      (optional)   Root directory for input file(s).
#                          Default is current directory.
# --mapfile   (optional)   Input file name of bc_strmap.pl's output file
#                          Default is BC_STRING.MAP
# --oxfile    (optional)   Output file name for XML source.
#                          Default is BC_STRING.XML.
# --ohfile    (optional)   Output file name for h source.
#                          Default is com_xml2event.h.
# --oerrfile  (optional)   Output file name for error log h source.
#                          Default is bc_errorlog.h.
# --odir      (optional)   Directory for output file.
#                          Default is current directory.
# --xmapfile  (optional)   Additional event map info. This is used to map 
#                          events that bc_strmap.pl cannot generate. Like
#                          events written directly into the BC's error log 
#                          without using the definitions in evt.h.
#                          Default is no extra file.
# Exit codes
# - 1 successful
# - 0 error occurred
#
# Note: This script is used to produce both the BC and RSA XML and header
#       files. For RSA all the defaults are overridden when producing
#       output files and reading input files.
#
#########################################################################

use Getopt::Long;

GetOptions(
  "debug"         => \$debug,
  "idir=s"        => \$idir,
  "mapfile=s"     => \$mapfile,
  "odir=s"        => \$odir,
  "oxfile=s"      => \$oxfile,
  "ohfile=s"      => \$ohfile,	   
  "oerrfile=s"    => \$oerrfile,	   
  "xmapfile=s"    => \$xmapfile,	   
);

##########################
# Set directory/file names
##########################

if ($idir eq "") {
   $idir = `pwd`;
   chomp $idir;
}

if ($odir eq "") {
   $odir = `pwd`;
   chomp $odir;
}

if ($mapfile eq "") {
   $mapfile = "BC_STRING.MAP";
}

if ($oxfile eq "") {
   $oxfile = "BC_STRING.XML";
}

if ($ohfile eq "") {
   $ohfile = "com_xml2event.h";
}

if ($oerrfile eq "") {
   $oerrfile = "bc_errorlog.h";
}

$file_map = $idir . "/$mapfile";
if ($xmapfile) {
    $file_xmap = $idir . "/$xmapfile";
}

$file_x = $odir . "/$oxfile";
$file_h = $odir . "/$ohfile";
$file_err_h = $odir . "/$oerrfile";
unlink $file_x;
unlink $file_h;
unlink $file_err_h;

############
# Open files
############

open (FILE_MAP, $file_map) or die "$0 Error! Cannot open $file_map. $! Stopped";

if ($xmapfile) {
    open (FILE_XMAP, $file_xmap) or die "$0 Error! Cannot open $file_xmap. $! Stopped";
}

open (FILE_X, ">>$file_x") or die "$0 Error! Cannot open file $file_x. $! Stopped";
open (FILE_H, ">>$file_h") or die "$0 Error! Cannot open file $file_h. $! Stopped";
open (FILE_ERR_H, ">>$file_err_h") or die "$0 Error! Cannot open file $file_err_h. $! Stopped";

#######################################
# Parse Map File(s) into internal table
#######################################

$rtn_code = 1;
%streventmap = ();
%defmap = ();

if (&read_file(FILE_MAP, $file_map)) { $rtn_code = 0; goto CLEANUP; }
if ($xmapfile) {
    if (&read_file(FILE_XMAP, $file_xmap)) { $rtn_code = 0; goto CLEANUP; }
}

###########################################
# Create string 2 event mapping header file
###########################################

if (&print_h_file_header) { $rtn_code = 0; goto CLEANUP; }
#&print_h_file_ending;

#############################################
# Create string 2 event mapping c source file
#############################################

if (&print_xml_file_header) { $rtn_code = 0; goto CLEANUP; }
foreach $event_message (keys %streventmap) {
    ($event_count, $event_name, $event_hex, $event_platform, $event_severity, $event_ovr, $event_msg, $rest) = 
	 split/\|/,$streventmap{$event_message};
    chomp($event_msg);
    $event_hex_str = "\"$event_hex\"";
    $event_hex_str =~ s/^\"0x/\"/;
    if (&print_xml_file_hash_member) { $rtn_code = 0; goto CLEANUP; }
}
if (&print_xml_file_ending) { $rtn_code = 0; goto CLEANUP; }

##############################
# Create error log header file
##############################

if (&print_err_hfile_header) { $rtn_code = 0; goto CLEANUP; }
foreach $d (keys %defmap) {
    chomp $defmap{$d};
    print FILE_ERR_H "#define $d $defmap{$d}\n";
}
if (&print_err_hfile_ending) { $rtn_code = 0; goto CLEANUP; }

CLEANUP:
close FILE_MAP;
close FILE_XMAP;
close FILE_X;
close FILE_H;
close FILE_ERR_H;
exit ($rtn_code);

#############
# Subroutines
#############

sub read_file {

    my( $file_handle, $filename ) = @_;
    while ( <$file_handle> ) {

	# Skip comments/blank lines
	next if /^\/\// || /^#.*$/ || /^\s*$/;

	$line = $_;
	($hpidef_event, $hpidef, $def) = split/\|/,$line;
	($event_name, $event_hex, $event_platform, $event_severity, $event_ovr, $event_msg, $rest) =  split/\|/,$line;

	
	chomp($event_msg);
	if ($hpidef_event eq "HPI_DEF") {
	    if ($hpidef eq "" || $def eq "" || $defmap{$hpidef} ne "") {
		print "*****************************************************\n";
		print "$0: Error! Definition $hpidef not found or not unique\n";
		print "in file $file_name\n";
		print "*****************************************************\n\n";
		return 1;
	    }
	    $defmap{$hpidef} = $def;
	}
	else {
	    if ($event_name eq "" || $event_hex eq "" || $event_platform eq "" ||
		$event_severity eq "" || $event_ovr eq "" || $event_msg eq "") {
		print "************************************************************\n";
		print "$0: Error! Format for event incomplete for event=$event_name\n";
		print "in file $file_name\n";
		print "*************************************************************\n\n";
		return 1;
	    }

	    # Check if platform-specific string. If so tack platform on end of string to make
	    # it unique and put into hash. Note all platforms definitions are mutally exclusive.
	    # So you can't have a list of supported platforms.
	    $hash_msg = $event_msg;
	    if ($event_platform ne "ALL") {
		$hash_msg =~ s/\"$//;
		$hash_msg = $hash_msg . "_HPIPLAT_" . $event_platform . "\"";
	    }

	    # See if string is a duplicate within a platform; track number of duplicates
	    if ($streventmap{$hash_msg} ne "") {
		print "$0: Warning! String $event_msg not unique in file $file_name\n";
                # Update number of dups in original entry
		($num, $entry) = split/\|/,$streventmap{$hash_msg},2;
		$dups = $num + 1;
		chomp($entry);
		$streventmap{$hash_msg} = $dups . "|$entry";
#		print "Update Original Event INDEX=$hash_msg; ENTRY=$streventmap{$hash_msg}\n";

		# Create unique hash entry and HPI string identifier for each duplicate
		$hashdup = $hash_msg;
		$msgdup = $event_msg;
		$hashdup =~ s/\"$//; 
		$msgdup  =~ s/\"$//; # HPI code handles _HPIDUP strings additions in the message itself
		$hashdup = $hashdup  . "_HPIDUP" . $dups . "\"";
		$msgdup  = $msgdup   . "_HPIDUP" . $dups . "\"";
		$streventmap{$hashdup} = "0|$event_name|$event_hex|$event_platform|$event_severity|$event_ovr|$msgdup|$rest";
#		print "Duplicate Event INDEX=$hashdup; ENTRY=$streventmap{$hashdup}\n";
	    }
	    else {
		$streventmap{$hash_msg} = "0|$line";
#		print "ORIGINAL Event INDEX=$hash_msg; ENTRY=$streventmap{$hash_msg}\n";
	    }
	}
    }
    return 0;
}

####################################
# Print h file's static leading text 
####################################

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
 * WARNING! This file is auto-magically generated by map2xml.pl 
 *          Do not change this file manually. Update script instead
 *******************************************************************/

#ifndef __COM_XML2EVENT_H
#define __COM_XML2EVENT_H

#define HPIDUP_STRING  "_HPIDUP"

#define NO_OVR  0x0000  /* No overrides */
#define OVR_SEV 0x0001  /* Override Error Log's severity */
#define OVR_RID 0x0010  /* Override Error Log's source */
#define OVR_EXP 0x0100  /* Override Error Log's source for expansion cards */

/* Global String to Event Hash Table */
extern GHashTable *bc_xml2event_hash;
extern GHashTable *rsa_xml2event_hash;
extern unsigned int bc_xml2event_hash_use_count;
extern unsigned int rsa_xml2event_hash_use_count;

/* xml code for rsa and bc events */
extern char *bc_eventxml;
extern char *rsa_eventxml;

int xml2event_hash_init(GHashTable **hashtable, const char *xmlstr);
int xml2event_hash_free(GHashTable ** hashtable);

#endif // __COM_XML2EVENT_H
EOF

    return 0;
}

#####################################
# Print h file's static trailing text 
#####################################

#sub print_h_file_ending {
#
#    print FILE_H <<EOF;
#
##endif
#EOF
#
#    return 0;
#}

######################################
# Print xml file's static leading text 
######################################

sub print_xml_file_header {

    print FILE_X <<EOF;
<?xml version="1.0"?>
<!--
 (C) Copyright IBM Corp. 2004

****************************************************************** 
 WARNING! This file is auto-magically generated by map2xml.pl 
          Do not change this file manually. Update script instead
******************************************************************/
-->
<events>
EOF
    return 0;
}

#######################################
# Print xml file's static trailing text 
#######################################

sub print_xml_file_ending {

   print FILE_X <<EOF;
</events>
EOF

    return 0;
}

###############################
# Print xml file's dynamic text 
###############################

sub print_xml_file_hash_member {

    $tab = "";
#   if ($event_platform ne "ALL") {
#	$tab = "        ";
#	$event_msg =~ s/_HPIPLAT_$event_platform_//;
#       if ($event_platform eq "BCT") {
#	    print FILE_X "\tif (!strcmp(bc_type, SNMP_BC_PLATFORM_BCT)) {\n";
#	}
#	elsif ($event_platform eq "BC") {
#	    print FILE_X "\tif (!strcmp(bc_type, SNMP_BC_PLATFORM_BC)) {\n";
#	}
#	else {
#	    print "*****************************************************\n";
#	    print "$0: Error! Unrecognized Platform Type=$event_platform\n";
#	    print "Script may need to be updated.\n";
#	    print "*****************************************************\n\n";
#	    return 1;
#	}
#    }	

    # Handle overrides
    if ($event_ovr ne "NO_OVR") { 
	$event_ovr =~ s/,/ | /g;
    }
    
    print FILE_X <<EOF;
<event name="$event_name" hex=$event_hex_str type="$event_platform"
       severity="$event_severity" override="$event_ovr" dup="$event_count"
       msg=$event_msg />
EOF

#   if ($event_platform ne "ALL") {
#	print FILE_X "\t}\n";
#   }

    print FILE_X "\n";

    return 0;
}

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
 * WARNING! This file is auto-magically generated by map2xml.pl 
 *          Do not change this file manually. Update script instead
 *******************************************************************/

#ifndef __BC_ERRORLOG_H
#define __BC_ERRORLOG_H

EOF

return 0;

}

sub print_err_hfile_ending {

    print FILE_ERR_H <<EOF;

#endif
EOF

    return 0;
}
