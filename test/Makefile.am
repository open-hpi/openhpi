
#
# TODO: license
#

MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include

COMMONLIBS		= $(top_srcdir)/lib/libopenhpi.la

AM_CFAGS 		= @CFLAGS@

bin_PROGRAMS		= hpitestapp

hpitestapp_SOURCES	= hpitestapp.c

hpitestapp_LDADD	= $(COMMONLIBS)

hpitestapp_LDFLAGS	= -export-dynamic -dlpreopen force -dlopen self


