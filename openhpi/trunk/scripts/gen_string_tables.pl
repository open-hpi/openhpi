#!/usr/bin/perl

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
    if(/^\s*typedef\s+enum(.*)/) {
        my $contents = $1;
        while($contents !~ /\}/is) {
            $contents .= <IN>;
            # special case for ENTITY
            $contents =~ s/SAHPI_ENT_IPMI_GROUP/0/g;
            $contents =~ s/\b0\s*\+//g;
            $contents =~ s/SAHPI_ENT_SAFHPI_GROUP/0x10000/g;
            $contents =~ s/SAHPI_ENT_ROOT_VALUE/0xFFFF/g;
        }
        
        if($contents =~ /_ENT_/is) {
            print STDERR $contents;
        }
        my $type = "";
        my $count = 0;
        
        if($contents =~ /\}\s*(\S+);/is) {
            $type = $1;
        }
        while($contents =~ /\s*(SA[^\s=,]+)(,|\s*=\s*0\b\s*,*|\s*=\s*0x\w+,*)*/gm) {
            my $define = $1;
            my $other = $2;
            #print STDERR "Define: $define, Other: $other\n";
            if($other =~ /=\s*(0x\w+)/) {
                #print STDERR "Caught hex $1\n";
                $count = hex($1);
            }
            $hash{$type}->{$define}->{value} = $count;
            $hash{$type}->{$define}->{string} = $define;
            
            $count++;
        }
    } elsif (/^\s*\/\*\s*SaHpiEventCategoryT\s*==\s*(\S+)\s*\*\//) {
        my $cat = $1;
        while((my $line = <IN>) !~ /^\s*$/) {
            if($line =~ /#define\s+(\S+)\s+.*?(0x\w+)/) {
                $category{$cat}->{$1}->{value} = hex($2);
                $category{$cat}->{$1}->{string} = $1;
            }
        }
    } elsif(/^\s*#define\s+(SA\S+)\s+(.*)/) {
        my $define = $1;
        my $data = $2;
        # errors
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
        $string =~ s/^SAHPI_//;
        $str .= "        case $key: /* $hash->{$key}->{value} */\n";
        $str .= "                return \"$string\";\n";
    }
    
    $str .= "        default:\n";
    $str .= "                return NULL;\n";
    $str .= "        }\n";
    $str .= "}\n\n\n";
    
    print CFILE $str;
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
