MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include -I/.

COMMONLIBS		= $(top_builddir)/src/libopenhpi.la

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= openhpid

include_HEADERS         = openhpid.h

openhpid_SOURCES	= server.c answer.c openhpid.c
openhpid_LDADD	        = $(COMMONLIBS)
openhpid_LDFLAGS	= 

clean:
	rm -f *~

