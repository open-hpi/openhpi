#!/usr/bin/perl
#
# $Id$
#
# (C) Copyright IBM Corp. 2003, 2004
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#       Sean Dague <sdague@users.sf.net>
#

#
#  This program generates the printing utilities for the various enum types
#  from the SaHpi.h header file.
#

use strict;

use constant SA_HPI_ERR_BASE => 1000;

my $inheader = shift;
my $header = shift;
my $cfile = shift;

unless($inheader and $header and $cfile) {
    die("Need 3 parameters to run");
}

my %hash = ();
my %category = ();

open(IN,"$inheader");
while(<IN>) {
    # 
    # First we process all the typedef enum clauses in the header file
    #
    if(/^\s*typedef\s+enum(.*)/) {
        my $contents = $1;
        
        # slurp all contents until we get to the end of the definition
        while($contents !~ /\}/is) {
            $contents .= <IN>;
            # Entity definitions use symbolic jumps all over the table.  This
            # substitutes it back to numbers that we can deal with.  If those
            # jumps change, this needs to as well.  It won't actually hurt anything
            # if the numbers are wrong, but it does make the .c file more readable.
            $contents =~ s/SAHPI_ENT_IPMI_GROUP/0/g;
            $contents =~ s/\b0\s*\+//g;
            $contents =~ s/SAHPI_ENT_SAFHPI_GROUP/0x10000/g;
            $contents =~ s/SAHPI_ENT_ROOT_VALUE/0xFFFF/g;
        }
        
        # DEBUG: the following can be uncommented to see what is happening
        # in the entity case
        #if($contents =~ /_ENT_/is) {
        #   print STDERR $contents;
        #}
        my $type = "";
        my $count = 0;
        
        if($contents =~ /\}\s*(\S+);/is) {
            $type = $1;
        }

        # big scary regex... don't worry it is correct
        while($contents =~ /\s*(SA[^\s=,]+)(,|\s*=\s*0\b\s*,*|\s*=\s*0x\w+,*)*/gm) {
            my $define = $1; # the SA type
            my $other = $2; # the rest of the line

            # if the rest of the line includes an assignment to a number, then 
            # $count becomes that number instead of just the increment from
            # the last time around the look
            if($other =~ /=\s*(0x\w+)/) {
                #print STDERR "Caught hex $1\n";
                $count = hex($1);
            }
            $hash{$type}->{$define}->{value} = $count;
            $hash{$type}->{$define}->{string} = $define;
            
            $count++;
        }
    } elsif (/^\s*\/\*\s*SaHpiEventCategoryT\s*==\s*(\S+)\s*\*\//) {
        # ok, this is more tricky.  If we find SaHpiEventCategoryT in the
        # comments, use that for parsing
        my $cat = $1;
        while((my $line = <IN>) !~ /^\s*$/) {
            if($line =~ /#define\s+(\S+)\s+.*?(0x\w+)/) {
                $category{$cat}->{$1}->{value} = hex($2);
                $category{$cat}->{$1}->{string} = $1;
            }
        }
    } elsif(/^\s*#define\s+(SA\S+)\s+(.*)/) {
        # and last, process all defines
        my $define = $1;
        my $data = $2;
        # errors are a slight specialy case because their
        # prefixes aren't all the same
        if($define =~ /(SA_ERR|SA_OK)/) {
            my $type = "SaErrorT";
            my $value = 0;
            if($data =~ /HPI_ERR_BASE\s*-\s*(\d+)/) {
                $value = 0 - SA_HPI_ERR_BASE - $1;
            }
            my $string = $define;
            $string =~ s/^SA_(ERR_HPI_)*//g;
                        
            $hash{$type}->{$define}->{value} = $value;
            $hash{$type}->{$define}->{string} = $string;
            
        } elsif ($define =~ /SAHPI_EC_/) {
            my $type = "SaHpiEventCategoryT";
            my $value = 0;
            if($data =~ /(0x\w+)/) {
                $value = hex($1);
            }
            
            $hash{$type}->{$define}->{value} = $value;
            $hash{$type}->{$define}->{string} = $define;
        } elsif (1) {
            
        }
    }
}
close(IN);

open(CFILE, ">$cfile");
open(HEADER, ">$header");

print_h_header();
print_c_header();

foreach my $key (sort keys %hash) {
    print_func($hash{$key}, $key);
}

print_category(%category);

close(CFILE);
close(HEADER);

sub print_category {
    my %cats = (@_);
    print HEADER "const char * SaHpiEventStateT2str(SaHpiEventCategoryT c, SaHpiEventStateT v);\n";

    my $str = "const char * SaHpiEventStateT2str(SaHpiEventCategoryT c, SaHpiEventStateT v)\n";
    $str .= "{\n";
    $str .= "        switch(c) {\n";
    foreach my $key (sort {$hash{SaHpiEventCategoryT}->{$a}->{value} <=> 
                             $hash{SaHpiEventCategoryT}->{$b}->{value}} keys %cats) {
        $str .= "        case $key: /* $hash{SaHpiEventCategoryT}->{$key}->{value} */\n";
        $str .= "                switch(v) {\n";
        foreach my $key2 (sort {$cats{$key}->{$a}->{value} <=> 
                                  $cats{$key}->{$b}->{value}} keys %{$cats{$key}}) {
            $str .= "                case $key2: /* $cats{$key}->{$key2}->{value} */\n";
            $str .= "                        return \"$cats{$key}->{$key2}->{string}\";\n";
        }
        $str .= "                default:\n";
        $str .= "                        return NULL;\n";
        $str .= "                }\n";
    }
    $str .= "        default:\n";
    $str .= "                return NULL;\n";
    $str .= "        }\n";
    $str .= "}\n";
    print CFILE $str;
}
    

sub print_func {
    my ($hash, $name) = @_;

    print HEADER "const char * $name" . "2str($name v);\n";
    
    my $str = "const char * $name" . "2str($name v)\n{\n";
    
    $str .= "        switch(v) {\n";
      
    foreach my $key (sort {abs($hash->{$a}->{value}) <=> abs($hash->{$b}->{value})} keys %$hash) {
        my $string = $hash->{$key}->{string};
        $string = beautify($string);
        $str .= "        case $key: /* $hash->{$key}->{value} */\n";
        $str .= "                return \"$string\";\n";
    }
    
    $str .= "        default:\n";
    $str .= "                return NULL;\n";
    $str .= "        }\n";
    $str .= "}\n\n\n";
    
    print CFILE $str;
}

sub beautify {
    my $string = shift;
    
    # trim that off the begining
    $string =~ s/^SAHPI_//;

    # make units nicer
    if($string =~ s/^SU_//) {
        $string = lc($string);
        $string =~ s/_/ /g;
        $string =~ s/\b(.)/\U$1\E/g;
    } 

    return $string;
}

sub print_h_header {
    my $str = <<END;
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
 *      Sean Dague <sdague\@users.sf.net>
 *
 * WARNING: This file is autogenerated, do not modify it directly 
 */

END
    print HEADER $str;
}

sub print_c_header {
    my $str = <<END;
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
 *      Sean Dague <sdague\@users.sf.net>
 *
 * WARNING: This file is autogenerated, do not modify it directly 
 */

#include <SaHpi.h>
#include <stdlib.h>
#include <$header>

END
    print CFILE $str;
}
