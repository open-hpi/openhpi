
#
# TODO: license
#

MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include  	\
			  -I$(top_srcdir)/include/utils

COMMONLIBS		= $(top_srcdir)/lib/libopenhpi.la

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= hpitestapp hashtest

hpitestapp_SOURCES	= hpitestapp.c
hpitestapp_LDADD	= $(COMMONLIBS)
hpitestapp_LDFLAGS	= -export-dynamic -dlpreopen force -dlopen self


hashtest_SOURCES	= hashtest.c
hashtest_LDADD		= $(top_srcdir)/lib/utils/libhpiutils.la
hashtest_LDFLAGS	= -export-dynamic -dlpreopen force -dlopen self
