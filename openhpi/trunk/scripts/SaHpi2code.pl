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
########################################################################

########################################################################
# Script Description:
#
# This script reads SaHpi.h and generates the C code necessary to 
# print the string definitions of the SaHpi.h's enum typedefs (and some
# #defines). The idea is to automatically generate and centralize these
# utility functions for use in OpenHPI client and testing programs. 
#
# These utility functions can also be used to determine if an arbitrary
# value is valid for a given enum typedef (a NULL return code indicates
# an invalid value).
# 
# Script also generates code to support SaHpi_event_utils.c routines.
#
# This script is run as part of the OpenHPI build process.
#
# Script Input:
#
# --debug     (optional)   Turn on debug info.
#                          Default is no.
# --ifile     (optional)   Full path name to header file, including the
#                          header's filename.
#                          Default is "current_directory/SaHpi.h".
# --odir      (optional)   Directory for generated output files.
#                          Default is current directory.
# --tdir      (optional)   Directory for generated testcase files.
#                          If not defined; no testcase files are generated;
# Exit codes
# - 0 successful
# - 1 error occurred
#########################################################################

use Getopt::Long;

GetOptions(
  "debug"   => \$debug,
  "ifile=s" => \$ifile,
  "odir=s"  => \$odir,
  "oname=s" => \$oname,
  "tdir=s"  => \$tdir,
);

##########################
# Set directory/file names
##########################

$cur_dir = `pwd`;
chomp $cur_dir;

if ($ifile eq "") {
   $ifile = $cur_dir . "/SaHpi.h";
}

if ($odir eq "") {
   $odir = $cur_dir;
}

#if ($oname eq "") {
($dir, $base, $ext) = ($ifile =~ m:(.*)/([^/]+)\.([^\./]+)$:);
($base, $ext) = ($ifile =~ m:^([^/]+)\.([^\./]+)$:) unless $base;
die "cannot find base for file $ifile" unless $base;
$ocfile = $base . "_enum_utils.c";
$ohfile = $base . "_enum_utils.h";
$oxcfile = $base . "_event_utils.c.embed";
#}
#else {
#    $ocfile = $oname . ".c";
#    $ohfile = $oname . ".h";
#}

if ($tdir) {
    $tbase_name = "t" . "$ocfile";
    $txbase_name = "t" . "$oxcfile";
    $tfile = "$tdir/$tbase_name";
    $txfile = "$tdir/$txbase_name";
}

$file_c   = $odir . "/$ocfile";
$file_h   = $odir . "/$ohfile";
$file_x   = $odir . "/$oxcfile";

unlink $file_c;
unlink $file_h;
unlink $file_x;
if ($tdir) { 
    unlink $txfile;
    unlink $tfile;
}

############
# Open files
############

open(INPUT_HEADER, $ifile) or die "$0 Error! Cannot open $ifile. $! Stopped";
open(FILE_C, ">>$file_c") or die "$0 Error! Cannot open file $file_c. $! Stopped";
open(FILE_H, ">>$file_h") or die "$0 Error! Cannot open file $file_h. $! Stopped";
open(FILE_X, ">>$file_x") or die "$0 Error! Cannot open file $file_x. $! Stopped";
if ($tdir) { 
    open(FILE_TEST, ">>$tfile") or die "$0 Error! Cannot open file $tfile. $! Stopped";
    open(FILE_XTEST, ">>$txfile") or die "$0 Error! Cannot open file $txfile. $! Stopped";
}

#########################
# Parse input header file
#########################

$in_enum = 0;
$line_count = 0;
$rtn_code = 0;
$max_events = 0;
$max_global_events = 0;

$cat_type = "SaHpiEventCategoryT";
$err_type = "SaErrorT";

@cat_array = ();
@err_array = ();
@enum_array = ();
@normalized_array = ();

%category = ();
%global_category = ();

if (&normalize_file(INPUT_HEADER)) { $rtn_code = 1; goto CLEANUP; }

&print_copywrite(FILE_C);
&print_copywrite(FILE_H);
&print_copywrite(FILE_X);
if ($tdir) { 
    &print_copywrite(FILE_TEST);
    &print_copywrite(FILE_XTEST);
}

&print_cfile_header();
&print_hfile_header();
&print_xfile_header();
if ($tdir) { 
    &print_testfile_header();
    &print_xtestfile_header();
}

foreach $line (@normalized_array) {
    $_ = $line;

    # Handle SaErrorT definitions
    ($err_code) = /^\s*\#define\s+(\w+)\s*\($err_type\).*$/;
    if ($err_code) {
        push(@err_array, $err_code);
	next;
    }

    # Handle SaHpiEventCategoryT definitions
    ($cat_code) = /^\s*\#define\s+(\w*)\s*\($cat_type\).*$/;
    if ($cat_code) {
        push(@cat_array, $cat_code);
	next;
    }

    if ( /^\s*typedef\s+enum.*$/ ) {
	$in_enum = 1;
	next;
    }
	 
    if ($in_enum) {
	# Check for end of enum definition - Assumming all on one line
	($enum_end, $enum_type) = /^\s*(\}+)\s*(\w*)\s*\;\s*$/;
	if ( $enum_end ne "" ) {
	    $in_enum = 0;
	    $line_count++;
	    &print_cfile_func($enum_type);
	    foreach $case (@enum_array) {
		&print_cfile_case($enum_type, $case);
		if ($tdir) { &print_testfile_case($enum_type, $case); }
	    }
	    &print_cfile_endfunc();
	    if ($tdir) { &print_testfile_endfunc($enum_type); }
	    &print_hfile_func($enum_type);
	    @enum_array = ();
	    next;
	}

	# Find enum definition - sometimes "{" is on the next line
	($enum_def) = /^\s*\{*\s*(\w+).*$/;
	if ($enum_def) {
	    push(@enum_array, $enum_def);
	}		     
    }
}

if ($in_enum) { die "$0 Error! Open enum definition. $! Stopped"; }
if ($#err_array > 0) {
    $line_count++;
    &print_cfile_func($err_type);
    foreach $case (@err_array) {
	&print_cfile_case($err_type, $case);
	if ($tdir) { &print_testfile_case($err_type, $case); }
    }
    &print_cfile_endfunc();
    if ($tdir) { &print_testfile_endfunc($err_type); }
    &print_hfile_func($err_type);
}

if ($#cat_array > 0) {
    $line_count++;
    &print_cfile_func($cat_type);
    foreach $case (@cat_array) {
	&print_cfile_case($cat_type, $case);
	if ($tdir) { 
	    &print_testfile_case($cat_type, $case); 
	}
    }
    &print_cfile_endfunc();
    if ($tdir) { &print_testfile_endfunc($cat_type); }
    &print_hfile_func($cat_type);
}

####################################
# Handle event states and categories 
####################################

print FILE_X "static oh_categorystate_map state_global_strings[] = {\n";
foreach $gc (keys %global_category) {
    foreach $gevt (sort {$global_category{$gc}->{$a}->{value} <=>
			     $global_category{$gc}->{$b}->{value}} keys %{$global_category{$gc}}) {
	$max_global_events++;
	if ($debug) { print("CAT=$gc; EVENT=$gevt; STR=$global_category{$gc}->{$gevt}->{string}\n"); }
	print FILE_X "{SAHPI_EC_UNSPECIFIED, $gevt, \"$global_category{$gc}->{$gevt}->{string}\"},\n";
	if ($tdir) { 
	    &print_xtestfile_case($gevt, "SAHPI_EC_UNSPECIFIED", $global_category{$gc}->{$gevt}->{string});
	}
    }
    &print_xtestfile_badevent("SAHPI_EC_UNSPECIFIED");
}
print FILE_X "};\n\n";
print FILE_X "\#define OH_MAX_STATE_GLOBAL_STRINGS $max_global_events\n\n";

print FILE_X "static oh_categorystate_map state_strings[] = {\n";
foreach $c (keys %category) {
    foreach $evt (sort {$category{$c}->{$a}->{value} <=>
			    $category{$c}->{$b}->{value}} keys %{$category{$c}}) {
        $max_events++;
	if ($debug) { print("CAT=$c; EVENT=$evt; STR=$category{$c}->{$evt}->{string}\n"); }
	print FILE_X "{$c, $evt, \"$category{$c}->{$evt}->{string}\"},\n";
	if ($tdir) { 
	    &print_xtestfile_case($evt, $c, $category{$c}->{$evt}->{string});
	}
    }
    &print_xtestfile_badevent($c);
}
print FILE_X "};\n\n";
print FILE_X "\#define OH_MAX_STATE_STRINGS $max_events\n\n";

&print_hfile_ending();
if ($tdir) { 
    &print_testfile_ending();
    &print_xtestfile_ending();
}

CLEANUP:
close INPUT_HEADER;
close FILE_C;
close FILE_H;
close FILE_X;
if ($tdir) { 
    close FILE_TEST;
    close FILE_XTEST;
}

if ($line_count == 0) {
    print "$0 Warning! No code can be generated from header file - $ifile\n";
    unlink $file_c;
    unlink $file_h;
    if ($tdir) { 
	unlink $tfile;
    }
}

if ($max_events == 0) {
    print "$0 Warning! No Events found in header file - $ifile\n";
    unlink $file_x;
    if ($tdir) { 
	unlink $txfile;
    }
}

exit ($rtn_code);

#############
# Subroutines
#############
sub normalize_file {
    my( $input_handle ) = @_;
    $in_comments = 0;
    $in_cat = 0;

    while ( <$input_handle> ) {
	chomp;
	
	# Handle special case for Event Categories and their states
	if (/^.*\s+SaHpiEventCategoryT\s*==.*$/ ) {
	    &parse_category_events($input_handle, $_);
	}

	next if /^\s*$/;               # Skip blank lines
	next if /^\s*\/\/.*$/;         # Skip // lines
	next if /^\s*\/\*.*\*\/\s*$/;  # Skip /* ... */ lines
	
	$line = $_;
	($line) =~ s/^(.*?)\s*$/$1/;    # Strip blanks from end of line
	($line) =~ s/^(.*?)\/\/.*$/$1/; # Strip trailing C++ comments

        # Multi-line C comments
	if ( ( /^.*\/\*.*$/ ) && !( /^.*\*\/.*$/ ) ) {  
	    $in_comments = 1;
	    ($line) =~ s/^(.*?)\/\*.*$/$1/; # Get part of line before comment
	    chomp $line;
	    if ($line ne "") {
		push(@normalized_array, $line);
	    }
	    if ($debug) {
		print "In multi-line comment section\n";
		print "Characters before first comment = $line\n";
	    }
	    next;
	}

	# End C comment
	if ( !( /^.*\/\*.*$/ ) && ( /^.*\*\/.*$/ ) ) {
	    $in_comments = 0;
	    ($line) =~ s/^.*\*\/(.*?)$/$1/; # Get part of line after comment
	    if ($debug) { 
		print "Out of multi-line comment section\n";
		print "Characters after last comment = $line\n";
	    }
	}

	# Embedded single line comment after C code
	if ( ( /^.*\/\*.*$/ ) && ( /^.*\*\/.*$/ ) ) {
	    ($token1, $comment, $token2) = /^(.*)(\/\*.*\*\/)+(.*)$/;
	    $line = $token1 . "\n$token2";
	    if ($debug) { 
		print "Embedded single line comment\n";
		print "Line without comment = $line\n";
	    }
	}

	next if ($in_comments);
	chomp $line;
	next if ($line eq "");

        # Change commas to NLs
	$line =~ s/,/\n/g;
	@fields = split/\n/,$line;
	foreach $field (@fields) {
	    chomp $field;
	    push(@normalized_array, $field);
	}
    }

    return 0;
}

sub parse_category_events {
    my ($file_handle, $line) = @_;

    my $in_global_cat = 0;
    my @global_events = ();
    my @cat_list = ();

    if (/\<any\>/) {
	$in_global_cat = 1;
    }
    else {
	my($cat, $rest_of_line) = /^.*SaHpiEventCategoryT\s*==\s*(\w+)\s+(.*)$/;
	if ($debug) { print("CAT=$cat\n"); }
	push(@cat_list, $cat);
	# Handle multiple categories || together
	while ($rest_of_line =~ /\|\|/) {
	    $rest_of_line =~ s/^\|\|//; # Strip off beginning || 
	    ($cat, $rol) = ($rest_of_line =~ /\s*(\w+)\s+(.*)$/);
	    $rest_of_line = $rol;
	    if ($debug) { print("CAT=$cat\n"); }
	    push(@cat_list, $cat);
	}
    }
    
    # Find events - assume blank lines end #define section; but support 
    # line continuation characters
    while (($line = <$file_handle>) !~ /^\s*$/) {
	($event_state, $event_hex) = ($line =~ /^\s*\#define\s+(\w+)\s+.*?(0x\w+)\s*$/);
	if ($event_state eq "") {
	    ($event_state) = ($line =~ /^\s*\#define\s+(\w+)\s+\\\s*$/);
	    if ($event_state) {
		my $line = <$file_handle>;
		($event_hex) = ($line =~ /^\s*.*?(0x\w+)\s*$/);
		die "Cannot parse continuation event line" unless ($event_hex);
	    }
        }

	# Push event state definition to global hashes   
	if ($event_state && $event_hex) {
	    my $str = $event_state;
	    $str =~ s/SAHPI_ES_//;
	    if ($in_global_cat) {
		$global_category{"ANY"}->{$event_state}->{value} = hex($event_hex);
		$global_category{"ANY"}->{$event_state}->{string} = $str;
	    }
	    else {
		foreach $cat (@cat_list) {
		    if ($debug) {
			print("CAT=$cat; EVENT STATE=$event_state; HEX=$event_hex; STR=$str x\n");
		    }
		    $category{$cat}->{$event_state}->{value} = hex($event_hex);
		    $category{$cat}->{$event_state}->{string} = $str;
		}
	    }
	}
    }

    return 0;
}

####################################
# Print h file's static leading text 
####################################
sub print_copywrite {
    my ( $file_handle ) = @_;

    print $file_handle <<EOF;
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
 *      Steve Sherman <stevees\@us.ibm.com> 
 */

/******************************************************************* 
 * WARNING! This file is auto-magically generated by:
 *          $0. 
 *          Do not change this file manually. Update script instead
 *******************************************************************/

EOF

    return 0;
}

####################################
# Print h file's static leading text 
####################################
sub print_hfile_header {

    $hdef_name = $ohfile;
    $hdef_name =~ tr/a-z/A-Z/;
    $hdef_name =~ s/\./_/g;	

    print FILE_H <<EOF;
#ifndef $hdef_name
#define $hdef_name

#ifdef __cplusplus
extern "C" {
#endif 

EOF

    return 0;
}

####################################
# Print c file's static leading text 
####################################
sub print_cfile_header {

    print FILE_C <<EOF;
#include <stdlib.h>

#include <SaHpi.h>
#include <$ohfile>

EOF
    return 0;
}

####################################
# Print xfile's static leading text 
####################################
sub print_xfile_header {

    print FILE_X <<EOF;
typedef struct {
    SaHpiEventCategoryT category;
    SaHpiEventStateT state;
    unsigned char *str;
} oh_categorystate_map;

EOF
    return 0;
}

###########################################
# Print testcase file's static leading text 
###########################################
sub print_testfile_header {

    print FILE_TEST <<EOF;
#include <stdio.h>
#include <string.h>

#include <SaHpi.h>
#include <$ohfile>

#define BAD_ENUM_VALUE -1

int main(int argc, char **argv) 
{
        const char *expected_str;
        const char *str;

EOF
    return 0;
}

############################################
# Print xtestcase file's static leading text 
############################################
sub print_xtestfile_header {

    print FILE_XTEST <<EOF;
#include <stdio.h>
#include <string.h>

#include <SaHpi.h>
#include <SaHpi_event_utils.h>

int main(int argc, char **argv) 
{
        const char *expected_str;
        SaErrorT err;
        SaHpiEventStateT event_state, expected_state;
        SaHpiEventCategoryT event_cat, expected_cat;
        SaHpiTextBufferT buffer;

        #define BAD_EVENT 0xFFFF

EOF
    return 0;
}

########################################
# Beautify function name from SaHpi type
########################################
sub beautify_func_name($type) {
    my ($name) = @_;
    $name =~ s/Sa//;    # Strip off beginning Sa - for SaErrorT
    $name =~ s/Hpi//;   # Strip off beginning Hpi - for rest of SaHpi types
    $name =~ s/T$//;    # Strip off trailing T
    $name = lc($name);  # Lower case name
    $name = "oh_lookup_" . $name;
    
    return $name;
}

###############################
# Print h file's func prototype 
###############################
sub print_hfile_func {
    my( $type ) = @_;
    my $func_name = &beautify_func_name($type);
    
    print FILE_H <<EOF;
const char * $func_name($type value);
EOF

    return 0;
}

###########################
# Print c file's func start 
###########################
sub print_cfile_func {
    my( $type ) = @_; 
    my $func_name = &beautify_func_name($type);
    if ($debug) { print("CFILE func_name=$func_name\n"); }
 
    print FILE_C <<EOF;
/**
 * $func_name:
 * \@value: enum value of type $type.
 *
 * Converts \@value into a string based on \@value\'s HPI enum definition.
 * 
 * Returns:
 * string - normal operation.
 * NULL - if \@value not a valid $type.
 **/
const char * $func_name($type value)
{
        switch (value) {	
EOF
    
    return 0;
}

############################################
# Beautify enum string from SaHpi definition
# This is highly SaHpi.h dependent.
############################################
sub beautify_enum_name($type, $enum) {
    my ($enum_type, $enum_name) = @_;
    if ($debug) { print("TYPE=$enum_type: ENUM=$enum_name: "); }

    if ($enum_type eq "SaHpiStatusCondTypeT" ||
	$enum_type eq "SaHpiSensorReadingTypeT") {
	$enum_name =~ s/(\s*[A-Za-z]+_{1}){4}//;
    }
    else {
	if ($enum_type eq "SaErrorT" ||
	    $enum_type eq "SaHpiAnnunciatorModeT" ||
	    $enum_type eq "SaHpiAnnunciatorTypeT" ||
	    $enum_type eq "SaHpiDomainEventTypeT" ||
	    $enum_type eq "SaHpiCtrlStateDigitalT" ||
	    $enum_type eq "SaHpiCtrlModeT" ||
	    $enum_type eq "SaHpiCtrlTypeT" ||
	    $enum_type eq "SaHpiIdrAreaTypeT" ||
	    $enum_type eq "SaHpiIdrFieldTypeT" ||
	    $enum_type eq "SaHpiHsActionT" ||
	    $enum_type eq "SaHpiHsIndicatorStateT" ||
	    $enum_type eq "SaHpiHsStateT" ||
	    $enum_type eq "SaHpiResourceEventTypeT" ||
	    $enum_type eq "SaHpiTextTypeT") {
	    $enum_name =~ s/(\s*[A-Za-z]+_{1}){3}//;
	}
	else {
	    if ($enum_type eq "SaHpiParmActionT" ||
		$enum_type eq "SaHpiRdrTypeT" ||
		$enum_type eq "SaHpiResetActionT" ||
		$enum_type eq "SaHpiSensorTypeT" ||
		$enum_type eq "SaHpiSeverityT") {
		
		$enum_name =~ s/\s*SAHPI_//;
	    }
	    else {
		$enum_name =~ s/(\s*[A-Za-z]+_{1}){2}//;	
	    }
	}
    }

    if ($debug) { print("STR=$enum_name\n"); }

    return $enum_name;
}

###############################
# Print c file's case statement 
###############################
sub print_cfile_case {
    my( $type, $case ) = @_;
    my $casestr = &beautify_enum_name($type, $case);
#    print("CASE=$case; STR=$casestr\n");

    print FILE_C <<EOF;
        case $case:
                return \"$casestr\";
EOF

    return 0;
}

################################
# Print testcase file's testcase
################################
sub print_testfile_case {
    my( $type, $case ) = @_;
    my $func_name = &beautify_func_name($type);
    my $casestr = &beautify_enum_name($type, $case);

    print FILE_TEST <<EOF;
        /* $type - $case testcase */
        {
	        $type value = $case;
                expected_str = "$casestr";

                str = $func_name(value);
                if (strcmp(expected_str, str)) {
                        printf("Error! Testcase $type - $case failed\\n");
			printf("Received string=%s\\n", str);
                        return -1;             
                }
	}

EOF

    return 0;
}

#################################
# Print xtestcase file's testcase
#################################
sub print_xtestfile_case {
    my( $state, $cat, $str ) = @_;

    # Special case categories with same event definitions
    if ($cat eq "SAHPI_EC_GENERIC" || $cat eq "SAHPI_EC_SENSOR_SPECIFIC") {
	$valid_cat_test = "(SAHPI_EC_GENERIC == event_cat) || (SAHPI_EC_SENSOR_SPECIFIC == event_cat)";
    }
    else {
	$valid_cat_test = "expected_cat == event_cat";
    }

    # Special case Thresholds
    if ($state eq "SAHPI_ES_LOWER_CRIT") {
	$state = "SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT";
	$str = "LOWER_MINOR | LOWER_MAJOR | LOWER_CRIT";
    }
    if ($state eq "SAHPI_ES_LOWER_MAJOR") {
	$state = "SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR";
	$str = "LOWER_MINOR | LOWER_MAJOR";
    }
    if ($state eq "SAHPI_ES_UPPER_CRIT") {
	$state = "SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT";
	$str = "UPPER_MINOR | UPPER_MAJOR | UPPER_CRIT";
    }
    if ($state eq "SAHPI_ES_UPPER_MAJOR") {
	$state = "SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR";
	$str = "UPPER_MINOR | UPPER_MAJOR";
    }

    print FILE_XTEST <<EOF;
        /* $cat - $state testcase */
        {
                expected_cat = $cat;
                expected_state = $state;
                expected_str = "$str";
  
                err = oh_decode_eventstate($state, $cat, &buffer);
                if (err != SA_OK) {
                        printf("Error! Testcase $cat - $state decode failed. Error=%d\\n", err);
                        return -1; 
                }
    
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase $cat - $state decode failed\\n");
                        printf("Received string=%s\\n", buffer.Data);
                        return -1;             
                }
    
                err = oh_encode_eventstate(&buffer, &event_state, &event_cat);
                if (err != SA_OK) {
                        printf("Error! Testcase $cat - $state encode failed. Error=%d\\n", err);
                        return -1;
                }
    
                if ((expected_state != event_state) || !($valid_cat_test)) {
                        printf("Error! Testcase $cat - $state encode failed\\n");
                        printf("Received state=%x; Received cat=%x\\n", event_state, event_cat);
                        return -1;
                }
        }
	    
EOF

    return 0;
}

###########################################
# Print xtestcase file's bad event testcase
###########################################
sub print_xtestfile_badevent {
    my( $cat ) = @_;

    print FILE_XTEST <<EOF;
        /* $cat - Bad event testcase */
        {
		if (oh_valid_eventstate(BAD_EVENT, $cat)) {
                        printf("Error! oh_valid_eventstate: Bad event for $cat testcase failed\\n");
                        return -1;
                }
	}

EOF

    return 0;
}

#########################
# Print c file's func end
#########################
sub print_cfile_endfunc {

   print FILE_C <<EOF;
        default:
                return NULL;
        }
}
EOF

    print FILE_C "\n";
    return 0;
}

####################################
# Print testcase file's default test
####################################
sub print_testfile_endfunc {
    my( $type ) = @_;
    my $func_name = &beautify_func_name($type);
    
    print FILE_TEST <<EOF;
        /* $type - Default testcase */
        {
	        $type value = BAD_ENUM_VALUE;
                expected_str = NULL;

                str = $func_name(value);
                if (str != expected_str) {
                        printf("$tbase_name Error! Testcase $type - Default failed\\n");
                        return -1;             
                }
	}

EOF

    return 0;
}

#####################################
# Print h file's static trailing text 
#####################################
sub print_hfile_ending {

    print FILE_H <<EOF;

#ifdef __cplusplus
}
#endif

#endif
EOF

    return 0;
}

############################################
# Print testcase file's static trailing text 
############################################
sub print_testfile_ending {

    print FILE_TEST <<EOF;
        return 0;
}
EOF

    return 0;
}

#############################################
# Print xtestcase file's static trailing text 
#############################################
sub print_xtestfile_ending {

    print FILE_XTEST <<EOF;
        return 0;
}
EOF

    return 0;
}
