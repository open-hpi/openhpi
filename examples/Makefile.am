MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include 

COMMONLIBS		= $(top_srcdir)/src/libopenhpi.la

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= list_resources

include_HEADERS		= 

list_resources_SOURCES	= list_resources.c
list_resources_LDADD	= $(COMMONLIBS)
list_resources_LDFLAGS	= 

pkgsysconfdir = $(sysconfdir)/$(PACKAGE)
pkgsysconf_DATA = openhpi.conf.example

clean:
	rm -f *~

