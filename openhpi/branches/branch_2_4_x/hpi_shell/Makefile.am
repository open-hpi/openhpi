
.NOTPARALLEL:

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES        = @TEST_CLEAN@

REMOTE_SOURCES 	= openhpiclient.cpp \
			openhpiclient.h

EXTRA_DIST = \
                hpi_cmd.h

if SHELL_CLIENT
COMMONLIBS		= $(top_builddir)/hpi_ui_lib/libopenhpiui.la \
				$(top_builddir)/marshal/libohtcpconnx.la \
				-L$(top_builddir)/utils/.libs -lopenhpiutils
INCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils \
				-I$(top_srcdir)/openhpid2  -I$(top_srcdir)/marshal \
				-DCLIENT
else
INCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils
COMMONLIBS		= $(top_builddir)/src/libopenhpi.la \
				$(top_builddir)/hpi_ui_lib/libopenhpiui.la \
				@STATIC_PLUGIN_LIBS@ @STATIC_PLUGIN_EXTRA_LIBS@
endif

AM_CFLAGS 		= @CFLAGS@ -D_REENTRANT

bin_PROGRAMS		= hpi_cmd

HS_SOURCES		= \
			hpi_cmd.c \
			cmdparser.c \
			commands.c \
			ctrl_inv.c \
			inputcmd.c \
			sensor.c \
			session.c

if SHELL_CLIENT
NHS_SOURCES = ${HS_SOURCES} ${REMOTE_SOURCES}
else
NHS_SOURCES = ${HS_SOURCES}
endif
hpi_cmd_SOURCES = ${NHS_SOURCES}
hpi_cmd_LDADD        = $(COMMONLIBS)
hpi_cmd_LDFLAGS      =
	
$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../openhpid2/$@; \
	fi

clean-local:
	rm -f *~ *.o  ${REMOTE_SOURCES}
