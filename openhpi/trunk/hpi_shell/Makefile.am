
.NOTPARALLEL:

MAINTAINERCLEANFILES 	= Makefile.in
MOSTLYCLEANFILES	= @TEST_CLEAN@

EXTRA_DIST		= hpi_ui.h Makefile.mingw32 version.rc

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"hpi_shell\"

AM_CPPFLAGS		+= -I$(top_srcdir)/include -I$(top_srcdir)/utils

AM_CPPFLAGS 		+= -I$(top_srcdir)/openhpid \
			   -I$(top_srcdir)/marshal \
			   -I$(top_srcdir)/clients \
			   -DCLIENT
COMMONLIBS		= $(top_builddir)/transport/libopenhpitransport.la \
			   $(top_builddir)/utils/libopenhpiutils.la \
			   $(top_builddir)/baselib/libopenhpi.la

bin_PROGRAMS		= hpi_shell

hpi_shell_SOURCES =  \
    cmdparser.c \
    commands.c \
    ctrl_inv.c \
    dimi.c \
    fumi.c \
    hpi_cmd.c  \
    hpi_cmd.h \
    inputcmd.c \
    sensor.c \
    service.c \
    session.c \
    show.c \
    $(top_srcdir)/clients/clients.c


hpi_shell_LDADD   = $(COMMONLIBS)

clean-local:
	rm -f *~ *.o
