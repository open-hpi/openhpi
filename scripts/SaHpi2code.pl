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
# print the string definitions of the SaHpi.h's enum typedefs.
# The idea is to automatically generate and centraalize these
# utility functions for use in OpenHPI client and testing programs. 
#
# These utility functions can also be used to determine if an arbitrary
# value is valid for a given enum typedef (a NULL return code indicates
# an invalid value).
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
# --oname     (optional)   File name for generated c and h files. Scripts
#                          tacks on ".c" and ".h" extensions to oname.
#                          Default is ifile's "root_filename"_enum_utils. 
#                          (e.g. SaHpi_enum_utils.c and SaHpi_enum_utils.h)
# --tdir      (optional)   Directory for generated testcase file.
#                          If not defined; no testcase file is generated;
#                          If defined, name of testcase file is 
#                          t"oname".c (e.g. tSaHpi_enum_utils.c)
#
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

if ($oname eq "") {
    ($dir, $base, $ext) = ($ifile =~ m:(.*)/([^/]+)\.([^\./]+)$:);
    ($base, $ext) = ($ifile =~ m:^([^/]+)\.([^\./]+)$:) unless $base;
    die "cannot find base for file $ifile" unless $base;
    $ocfile = $base . "_enum_utils.c";
    $ohfile = $base . "_enum_utils.h";
}
else {
    $ocfile = $oname . ".c";
    $ohfile = $oname . ".h";
}

if ($tdir ne "") {
    $tbase_name = "t" . "$ocfile";
    $tfile = "$tdir/$tbase_name";
}

$file_c   = $odir . "/$ocfile";
$file_h   = $odir . "/$ohfile";

unlink $file_c;
unlink $file_h;
if ($tfile) { unlink $tfile; }

############
# Open files
############

open(INPUT_HEADER, $ifile) or die "$0 Error! Cannot open $ifile. $! Stopped";
open(FILE_C, ">>$file_c") or die "$0 Error! Cannot open file $file_c. $! Stopped";
open(FILE_H, ">>$file_h") or die "$0 Error! Cannot open file $file_h. $! Stopped";
if ($tfile) { open(FILE_TEST, ">>$tfile") or die "$0 Error! Cannot open file $tfile. $! Stopped"; }

#########################
# Parse input header file
#########################

$line_count = 0;
$rtn_code = 0;
$in_enum = 0;

$err_type = "SaErrorT";
@err_array = ();
$cat_type = "SaHpiEventCategoryT";
@cat_array = ();
@enum_array = ();
@normalized_array = ();

if (&normalize_file(INPUT_HEADER)) { $rtn_code = 1; goto CLEANUP; }

&print_copywrite(FILE_C);
&print_copywrite(FILE_H);
if ($tfile) { &print_copywrite(FILE_TEST); }

&print_cfile_header();
&print_hfile_header();

if ($tfile) { &print_testfile_header(); }

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
		&print_cfile_case($case);
		if ($tfile) { &print_testfile_case($enum_type, $case); }
	    }
	    &print_cfile_endfunc();
	    if ($tfile) { &print_testfile_endfunc($enum_type); }
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
	&print_cfile_case($case);
	if ($tfile) { &print_testfile_case($err_type, $case); }
    }
    &print_cfile_endfunc();
    if ($tfile) { &print_testfile_endfunc($err_type); }
    &print_hfile_func($err_type);
}

if ($#cat_array > 0) {
    $line_count++;
    &print_cfile_func($cat_type);
    foreach $case (@cat_array) {
	&print_cfile_case($case);
	if ($tfile) { &print_testfile_case($cat_type, $case); }
    }
    &print_cfile_endfunc();
    if ($tfile) { &print_testfile_endfunc($cat_type); }
    &print_hfile_func($cat_type);
}

&print_hfile_ending();
&print_cfile_ending();
if ($tfile) { &print_testfile_ending(); }

CLEANUP:
close INPUT_HEADER;
close FILE_C;
close FILE_H;
if ($tfile) { close FILE_TEST; }

if ($line_count == 0) {
    print "$0 Warning! No code can be generated from header file - $ifile\n";
    unlink $file_c;
    unlink $file_h;
    if ($tfile) { unlink $tfile; }
}

exit ($rtn_code);

#############
# Subroutines
#############

sub normalize_file {
    my( $input_handle ) = @_;
    $in_comments = 0;

    while ( <$input_handle> ) {
	chomp;
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
 *      Steve Sherman <stevees@us.ibm.com> 
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

###############################
# Print h file's func prototype 
###############################

sub print_hfile_func {
    my( $type ) = @_;
    $func_name = $type . "2str";

    print FILE_H <<EOF;
const char * $func_name($type value);
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

#####################################
# Print c file's static trailing text 
#####################################

sub print_cfile_ending {

    return 0;
}

###########################
# Print c file's func start 
###########################

sub print_cfile_func {
    my( $type ) = @_;
    $func_name = $type . "2str";

    print FILE_C <<EOF;
/**
 * $func_name:
 * \@value: enum value of type $type.
 *
 * Converts \"value\" into its HPI enum string definition.
 * 
 * Returns: 
 * NULL - if \"value\" not a valid $type.
 **/
const char * $func_name($type value)
{
        switch (value) {	
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

###############################
# Print c file's case statement 
###############################

sub print_cfile_case {
    my( $case ) = @_;

    # Same stripping code must be in print_testfile_case 
    $casestr = $case;
    $casestr =~ s/^SAHPI_//; # Strip "SAHPI_"
    $casestr =~ s/^SU_//;    # Strip "SU_" from units 
                             # SaHpi_struct_utils.c depends on this
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
    $func_name = $type . "2str";

    $casestr = $case;
    $casestr =~ s/^SAHPI_//; # Strip "SAHPI_"
    $casestr =~ s/^SU_//;    # Strip "SU_" from units 
                             # SaHpi_struct_utils.c depends on this
    print FILE_TEST <<EOF;
        /* $type - $case testcase */
        {
	        $type value = $case;
                expected_str = "$casestr";

                str = $func_name(value);
                if (strcmp(expected_str, str)) {
                        printf("$tbase_name Error! Testcase $type - $case failed\\n");
                        return -1;             
                }
	}

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

####################################
# Print testcase file's default test
####################################

sub print_testfile_endfunc {
    my( $type ) = @_;  
    $func_name = $type . "2str";
    
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
