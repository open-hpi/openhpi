#!/usr/bin/perl

#       $Id$
 
#  (C) Copyright IBM Corp. 2004
 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
#  file and program are licensed under a BSD style license.  See
#  the Copying file included with the OpenHPI distribution for
#  full licensing terms.
 
#  Authors:
#      Sean Dague <http://dague.net/sean>

use strict;
use Cwd;

my $start = cwd();

chdir("../src");

my $report = "";

# this needs to be made more generic over time
my %files = (
             "sel_utils.c" => "t/sel",
             "rpt_utils.c" => "t/rpt",
             "epath_utils.c" => "t/epath",
            );

# we must ensure that we have coverage created
system("make -ks check");

foreach my $file (sort keys %files) {
    my $report = "Coverage Report for $file\n\n";
    my $body = "";
    my $header = "";
    open(GCOV,"gcov -blf -o $files{$file} $file |");
    while(<GCOV>) {
        if(/^File.*$file/) {
            $header .= $_; # File
            $header .= <GCOV>; # Lines
            $header .= <GCOV>; # Branches
            $header .= <GCOV>; # Taken
            $header .= <GCOV>; # Calls
            $header .= "\n";
            last; # and now we are *done*
        } else {
            $body .= $_;
        }
    }
    close(GCOV);
    
    open(OUT,">$file.summary");
    print OUT $report, $header, $body;
    close(OUT);
}

