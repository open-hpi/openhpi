
.NOTPARALLEL:

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES        = @TEST_CLEAN@

REMOTE_LIB_SOURCES 	= openhpiclient.cpp \
		  	  openhpiclient.h

if SHELL_CLIENT
COMMONLIBS		= $(top_builddir)/hpi_ui_lib/libopenhpiui.la libopenhpiclient.la \
				-L$(top_builddir)/utils/.libs -lopenhpiutils
INCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils \
				-I$(top_srcdir)/openhpid2  -I$(top_srcdir)/marshal
else
INCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils
COMMONLIBS		= $(top_builddir)/src/libopenhpi.la \
				$(top_builddir)/hpi_ui_lib/libopenhpiui.la \
				@STATIC_PLUGIN_LIBS@ @STATIC_PLUGIN_EXTRA_LIBS@
endif

AM_CFLAGS 		= @CFLAGS@ -D_REENTRANT

bin_PROGRAMS		= hpi_cmd

include_HEADERS		= hpi_cmd.h

HS_SOURCES		= \
			hpi_cmd.c \
			cmdparser.c \
			commands.c \
			session.c

hpi_cmd_SOURCES      = ${HS_SOURCES}
hpi_cmd_LDADD        = $(COMMONLIBS)
hpi_cmd_LDFLAGS      = 
	
$(REMOTE_LIB_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../openhpid2/$@; \
	fi

if SHELL_CLIENT
noinst_LTLIBRARIES	= libopenhpiclient.la

libopenhpiclient_la_SOURCES = $(REMOTE_LIB_SOURCES)

libopenhpiclient_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libopenhpiclient_la_LIBADD  = $(top_builddir)/marshal/libohtcpconnx.la

endif

clean-local:
	rm -f *~ *.o
