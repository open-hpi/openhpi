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
use HTML::Entities;

foreach my $file (@ARGV) {
#    my $outfile = $file . ".html";
    open(IN,"$file");
    my @lines = <IN>;
    close(IN);
    
    my $html = make_html_head($file);
    $html .= make_html_body(parse_gcov(@lines));
    $html .= make_html_tail();

    print $html;
#    open(OUT,">$outfile");
#    print OUT $html;
#    close(OUT);
}

sub make_html_head {
    my $title = shift;
    return <<END;
<html>
<head><title>GCOV Report for $title</title>
<style>
TR.na {color: grey}
TR.notexec {background-color: #e84545}
TR.low {background-color: #dde244}
TR.good {background-color: #4ae544}
TD.num {align: right}
PRE {font-family: san-serif}
</style>
</head>
<body>
<h1>GCOV Report for $title</h1>
<table>
<tr><th>Exec</th><th>&nbsp;</th><th>Code</th></tr>
END
}

sub make_html_tail {
    return <<END;
</table>
</body>
</html>
END
}

sub set_status {
    my $exec = shift;
    if($exec eq "-") {
        return "na";
    } elsif($exec eq "#####") {
        return "notexec";
    } elsif($exec < 10) {
        return "low";
    } else {
        return "good";
    }
}

sub make_html_body {
    my $lines = shift;
    my $html;
    foreach my $line (@$lines) {
        my $status = set_status($line->{exec});
        my $exec = $line->{exec} + 0;
        my $data = encode_entities($line->{data});
        $html .= "<tr class='$status'><td align='right'>$exec</td><td>&nbsp;</td><td><pre>$data</pre></td></tr>\n";
    }
    return $html;
}

sub parse_gcov {
    my @in = @_;
    my $lines = [];
    foreach my $line (@in) {
        if($line =~ /^\s+(.*?):\s+(.*?):(.*)/) {
            my $hash = {
                        exec => $1,
                        line => $2,
                        data => $3,
                       };
            push @$lines, $hash;
        }
    }
    return $lines;
}
