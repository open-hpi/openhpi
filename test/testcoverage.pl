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
use File::Basename;

chdir("..");

my $start = cwd();

my $report = "";

# this needs to be made more generic over time
my %files = (
             "src/sel_utils.c" => "t/sel",
             "src/rpt_utils.c" => "t/rpt",
             "src/epath_utils.c" => "t/epath",
             # now for the blade center stuff
             "src/plugins/snmp_bc/bc_resources.c" => "t",
             "src/plugins/snmp_bc/bc_str2event.c" => "t",
             "src/plugins/snmp_bc/snmp_bc.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_control.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_discover.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_hotswap.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_sel.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_sensor.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_session.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_utils.c" => "t",
             "src/plugins/snmp_bc/snmp_bc_watchdog.c" => "t",
            );

# we must ensure that we have coverage created
system("make -ks check");

foreach my $fullfile (sort keys %files) {
    chdir($start);
    my $file = basename($fullfile);
    my $dir = dirname($fullfile);
    chdir($dir);
    print STDERR "Cwd is now" . cwd() . "\n";
    my $cmd = "gcov -blf -o $files{$fullfile} $file";
    
    my $report = "Coverage Report for $fullfile\n\n";
    my $body = "";
    my $header = "";
    open(GCOV,"$cmd |");
    while(<GCOV>) {
        if(s{^(File.*)($file)}{$1$dir/$file}) {
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

