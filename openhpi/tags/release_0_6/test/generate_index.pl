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

my $dir = shift;
opendir(IN,"$dir");
my @files = ();
while(my $file = readdir(IN)) {
    push @files, $file;
}
closedir(IN);

my $html = make_html_head();
$html .= make_html_body($dir, @files);
$html .= make_html_tail();

print $html;

sub make_html_head {
    return <<END;
<html>
<head><title>Test Coverage Analysis</title>
<!--#include virtual="/openhpi.css" -->
</head>
<body>
<table>
<tr>
<!--#include virtual="/sidebar.html" -->
<td valign="top">
<h1>Test Coverage Analysis</h1>
<table>
END
}

sub make_html_tail {
    return <<END;
</table>
</td></tr></table>
</body>
</html>
END
}

# ok, here is a set of assumptions, which should hold true
#
# 1. .c.summary.html files will exist for everything we care about
# 2. a .c.gcov.html file will exist for each
# 3. my regex skills are honed enough to pull up the table from .c.summary.html file


sub make_html_body {
    my $dir = shift;
    my @files = @_;
    my $html;
    foreach my $file (sort @files) {
        if($file =~ /\.c\.summary\.html$/) {
            next if(-z "$dir/$file");
            my $name = $file;
            $name =~ s/.summary.html//;
            $html .= slurp_summary_table($name, "$dir/$file");
            $html .= "<a href='$file'>Full Coverage Report</a><br>\n";
            $html .= "<a href='$name.gcov.html'>Detailed Execution Report</a><br>\n";
        }
    }
    return $html;
}

sub slurp_summary_table {
    my ($name, $file) = @_;
    open(IN,"$file");
    local $/ = undef;
    my $content = <IN>;
    close(IN);

    my $snip = "";
    if($content =~ m{(<h2 class='file'>.*?</h2>\s*<table.*?</table>)}igs) {
        $snip = $1;
    }
    return $snip;
}
    
