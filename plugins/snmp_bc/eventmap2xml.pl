#!/usr/bin/perl
#
# (C) Copyright IBM Corp. 2004
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Convert a map file to xml.
#
# Author(s):
#      W. David Ashley <dashley@us.ibm.com>
#

eval "exec /usr/bin/perl -S $0 $*"
	if $Shell_cannot_understand; #!

@fieldlist = qw(name hex type severity override msg);
$0 =~ /[^\/]+$/ ;	# get the name of this program
$program = $&;
$sflag = 0;
while($_ = $ARGV[0], /^-/){ # get options
	shift;
	if(/^-s$/){ $sflag = 1; $strname = $ARGV[0]; shift; }
	elsif(/^-s(.*)/){ $sflag = 1; $strname = $1; }
	else{
		print stderr "$program: convert map file to xml...\n";
                print stderr "  Usage: $program  { textfile }\n";
		exit 0;
	}
}
print "<?xml version=\"1.0\"?>\n";
print "<events>\n";
while(<>){
	chop;
        if (length($_) == 0) {
               next;
        }
        if (substr($_, 0, 1) eq "#") {
               next;
        }
	@data{@fieldlist} = split(/\|/, $_, scalar @fieldlist);
        if (substr($_, 0, 7) eq "HPI_DEF") {
               next;
        }
	print "<event ";
        foreach (@fieldlist) {
               if ($_ eq "msg") {
                      printf("%s=%s ", $_, $data{$_});
               }
               else {
                      printf("%s=\"%s\" ", $_, $data{$_});
               }
        }
	print "/>\n";
}
print "</events>\n";
