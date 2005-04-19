MAINTAINERCLEANFILES = Makefile.in

MOSTLYCLEANFILES        = @TEST_CLEAN@

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

