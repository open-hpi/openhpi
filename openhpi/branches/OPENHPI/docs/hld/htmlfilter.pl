#!/usr/bin/perl
#
# Filter to make documentation look like website
#

my @files = @ARGV;

foreach my $file (@files) {
    system("tidy -c -m $file 2>/dev/null");
}

foreach my $file (@files) {
    local $/ = undef;
    open(IN,"<$file");
    my $contents = <IN>;
    close(IN);

    $contents =~ s{(</style>\s*)(<!--.*?-->)*}{$1<!--#include virtual="/openhpi.css" -->\n}is;
    $contents =~ s{(<body.*?>\s*).*?(<div)}{$1<table>\n<tr>\n<!--#include virtual="/sidebar.html" -->\n<td valign="top">\n$2}is;
    $contents =~ s{(</td></tr></table>\s*)*(</body>)}{</td></tr></table></body>}is;

    open(OUT,">$file");
    print OUT $contents;
    close(OUT);
}
