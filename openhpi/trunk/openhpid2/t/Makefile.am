#
# Copyright (c) 2004 by IBM Corporation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Authors:
#    W. David Ashley <dashley@us.ibm.com>
#

MAINTAINERCLEANFILES    = Makefile.in *~ core core.*
MOSTLYCLEANFILES 	= client.cpp
INCLUDES			= -I$(top_srcdir)/utils -I$(top_srcdir)/include -I$(top_srcdir)/openhpid2 -I$(top_srcdir)/marshal

AM_CFLAGS               = @CFLAGS@

bin_PROGRAMS      = ctest_01

ctest_01_SOURCES= \
	client.cpp \
	ctest_01.cpp

ctest_01_LDADD = $(top_builddir)/marshal/libohtcpconnx.la
ctest_01_LDFLAGS=

client.cpp:
	if test ! -f $@ -a ! -L $@; then \
		ln -s $(top_srcdir)/openhpid2/client.cpp $(top_builddir)/openhpid2/t/client.cpp; \
	fi

clean-local:
	rm -f *~
	if test -L $(top_builddir)/openhpid2/t/client.cpp; then \
		rm -r $(top_builddir)/openhpid2/t/client.cpp; \
	fi
