
.NOTPARALLEL:

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES        = @TEST_CLEAN@

REMOTE_SOURCES 	= openhpiclient.cpp \
			openhpiclient.h

COMMONLIBS		= $(top_builddir)/hpi_ui_lib/libopenhpiui.la \
				-lreadline -lhistory -lcurses

if SHELL_CLIENT
ADDEDLIBS		= $(top_builddir)/marshal/libohtcpconnx.la \
				-L$(top_builddir)/utils/.libs -lopenhpiutils
ADDEDINCLUDES 		= -I$(top_srcdir)/openhpid2  -I$(top_srcdir)/marshal
else
ADDEDLIBS		= $(top_builddir)/src/libopenhpi.la \
				@STATIC_PLUGIN_LIBS@ @STATIC_PLUGIN_EXTRA_LIBS@
ADDEDINCLUDES 		= 
endif

COMMONINCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils
INCLUDES 		= $(COMMONINCLUDES) $(ADDEDINCLUDES)
AM_CFLAGS 		= @CFLAGS@ -D_REENTRANT

bin_PROGRAMS		= hpi_cmd

include_HEADERS		= hpi_cmd.h

HS_SOURCES		= \
			hpi_cmd.c \
			cmdparser.c \
			commands.c \
			ctrl_inv.c \
			sensor.c \
			session.c

if SHELL_CLIENT
NHS_SOURCES = ${HS_SOURCES} ${REMOTE_SOURCES}
else
NHS_SOURCES = ${HS_SOURCES}
endif

hpi_cmd_SOURCES = ${NHS_SOURCES}
hpi_cmd_LDADD        = $(COMMONLIBS) $(ADDEDLIBS)
hpi_cmd_LDFLAGS      =

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../openhpid2/$@; \
	fi

clean-local:
	rm -f *~ *.o  ${REMOTE_SOURCES}
