#!/usr/bin/perl

use strict;

my @dirs = qw(src);

my $outdir = "report_html";

mkdir $outdir, 0755;

foreach my $dir (@dirs) {
    opendir(DIR,"../$dir");
    while(my $file = readdir(DIR)) {
        if($file =~ /\.gcov$/) {
            system("./gcov2html.pl ../$dir/$file > $outdir/$file.html");
        } elsif ($file =~ /\.summary$/) {
            system("./gsum2html.pl ../$dir/$file > $outdir/$file.html");
        }
    }
    closedir(DIR);
}
