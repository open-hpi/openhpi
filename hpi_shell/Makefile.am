MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES        = @TEST_CLEAN@

INCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

COMMONLIBS		= $(top_builddir)/src/libopenhpi.la @STATIC_PLUGIN_LIBS@ @STATIC_PLUGIN_EXTRA_LIBS@

AM_CFLAGS 		= @CFLAGS@ -D_REENTRANT

bin_PROGRAMS		= hpi_cmd

include_HEADERS		= 

hpi_cmd_SOURCES      = hpi_cmd.c
hpi_cmd_LDADD        = $(COMMONLIBS)
hpi_cmd_LDFLAGS      = 
	
clean-local:
	rm -f *~ *.o

