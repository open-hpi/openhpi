

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES        = @TEST_CLEAN@

INCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

COMMONLIBS		= $(top_builddir)/src/libopenhpi.la \
				$(top_builddir)/hpi_ui_lib/libopenhpiui.la \
				@STATIC_PLUGIN_LIBS@ @STATIC_PLUGIN_EXTRA_LIBS@

AM_CFLAGS 		= @CFLAGS@ -D_REENTRANT

bin_PROGRAMS		= hpi_cmd

EXTRA_DIST		= hpi_cmd.h \
       			  resource.h

HS_SOURCES		= \
			hpi_cmd.c \
			cmdparser.c \
			commands.c \
			session.c \
			resource.c

hpi_cmd_SOURCES      = ${HS_SOURCES}
hpi_cmd_LDADD        = $(COMMONLIBS)
hpi_cmd_LDFLAGS      = 
	
clean-local:
	rm -f *~ *.o
