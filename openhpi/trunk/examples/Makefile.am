MAINTAINERCLEANFILES = Makefile.in

MOSTLYCLEANFILES        = @TEST_CLEAN@

INCLUDES 		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

COMMONLIBS		= $(top_builddir)/src/libopenhpi.la @STATIC_PLUGIN_LIBS@ @STATIC_PLUGIN_EXTRA_LIBS@
AM_CFLAGS 		= @CFLAGS@

noinst_PROGRAMS		= list_resources @LIST_RESOURCES_THREADED@
EXTRA_PROGRAMS          = list_resources_threaded

include_HEADERS		= 

list_resources_SOURCES	= list_resources.c
list_resources_LDADD	= $(COMMONLIBS)
list_resources_LDFLAGS	=

list_resources_threaded_SOURCES = list_resources_threaded.c
list_resources_threaded_LDADD   = $(COMMONLIBS) 
list_resources_threaded_LDFLAGS = 

install-data-local: $(top_srcdir)/examples/openhpi.conf.example
	$(mkinstalldirs) $(DESTDIR)$(CONFPATH)
	if test ! -e  $(DESTDIR)$(CONFPATH)/openhpi.conf; then \
		$(INSTALL_DATA) $(top_srcdir)/examples/openhpi.conf.example $(DESTDIR)$(CONFPATH)/openhpi.conf; \
	fi

uninstall-local:
	if cmp $(top_srcdir)/examples/openhpi.conf.example $(DESTDIR)$(CONFPATH)/openhpi.conf; then \
		: \
	else \
		rm -f $(DESTDIR)$(CONFPATH)/openhpi.conf; \
	fi

# comment out for now, we do install in rpm
#pkgsysconfdir = $(sysconfdir)/$(PACKAGE)
#pkgsysconf_DATA = openhpi.conf.example

EXTRA_DIST = openhpi.conf.example

clean-local:
	rm -f *~

