MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include  	\
			  -I$(top_srcdir)/include/utils

COMMONLIBS		= $(top_srcdir)/src/libopenhpi.la

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= list_resources

include_HEADERS		= 

list_resources_SOURCES	= list_resources.c
list_resources_LDADD	= $(COMMONLIBS) -lltdl
list_resources_LDFLAGS	= -export-dynamic -dlpreopen force -dlopen self

clean:
	rm -f *~

