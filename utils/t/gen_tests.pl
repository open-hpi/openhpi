#!/usr/bin/perl

my $numtests = 99;
my $el = shift;

if($el eq "--list") {
    for my $n (0..$numtests) {
        my $file = sprintf("epath_conv_%3.3d.t",$n);
        print "$file ";
    }
} elsif($el eq "--make") {
    my $final;
    my $list;
    for my $n (0..$numtests) {
        $list .= sprintf("\\\n\tepath_conv_%3.3d ",$n);
        $final .= sprintf("epath_conv_%3.3d_SOURCES = epath_conv_%3.3d.c \$(REMOTE_SOURCES)\n",$n,$n);
    }
    print "TESTS = $list\n\n";
    print "check_PROGRAMS = $list\n\n";
    print $final;
} else {

    my @ents = ent_list();
    
    for my $n (0..$numtests) {
        my $file = sprintf("epath_conv_%3.3d.c",$n);
        open(OUT,">$file");
        my $str = gen_epath_test($ents[int(rand(scalar(@ents)))],int(rand(100)),
                                 $ents[int(rand(scalar(@ents)))],int(rand(100)));
        print OUT $str;
        close(OUT);
    }
}

sub ent_list {
    my $file = "../../include/SaHpi.h";
    my @ents = ();
    open(IN,"<$file");
    while(<IN>) {
        if(/^\s+SAHPI_ENT_([\_\w]+)/) {
            push @ents, $1;
        }
    }
    return @ents;
}

sub gen_epath_test {
    my ($ent1, $inst1, $ent2, $inst2) = @_;
    return <<END;
/* -*- linux-c -*-
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
 * Authors:
 *     Sean Dague <http://dague.net/sean>
 *
 */

#include <string.h>
#include <SaHpi.h>
#include <epath_utils.h>

/**
 * main: epathstr -> epath test
 * 
 * This test tests whether an entity path string is converted into
 * an entity path properly.  
 *
 * TODO: a more extensive set of tests would be nice, might need to create a
 * perl program to generate that code
 * 
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv) 
{
        char new[255];
        SaHpiEntityPathT tmp_ep;
        char *entity_root = "{$ent1,$inst1}{$ent2,$inst2}";
        
        string2entitypath(entity_root, &tmp_ep);
         
        if(tmp_ep.Entry[0].EntityType != SAHPI_ENT_$ent2)
                return 1;
                
        if(tmp_ep.Entry[0].EntityInstance != $inst2)
                return 1;
        
        if(tmp_ep.Entry[1].EntityType != SAHPI_ENT_$ent1)
                return 1;
        
        if(tmp_ep.Entry[1].EntityInstance != $inst1)
                return 1;

        if(entitypath2string(&tmp_ep, new, 255) < 0) 
                return 1;
        
        if(strcmp(new,entity_root) != 0)
                return 1;

        return 0;
}
END
}
