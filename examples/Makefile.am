MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include 

COMMONLIBS		= $(top_builddir)/src/libopenhpi.la

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= list_resources @LIST_RESOURCES_THREADED@
EXTRA_PROGRAMS          = list_resources_threaded

include_HEADERS		= 

list_resources_SOURCES	= list_resources.c
list_resources_LDADD	= $(COMMONLIBS)
list_resources_LDFLAGS	= 

list_resources_threaded_SOURCES = list_resources_threaded.c
list_resources_threaded_LDADD   = $(COMMONLIBS) 
list_resources_threaded_LDFLAGS = 

# comment out for now, we do install in rpm
#pkgsysconfdir = $(sysconfdir)/$(PACKAGE)
#pkgsysconf_DATA = openhpi.conf.example

EXTRA_DIST = openhpi.conf.example

clean:
	rm -f *~

