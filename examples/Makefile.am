MAINTAINERCLEANFILES = Makefile.in

MOSTLYCLEANFILES        = @TEST_CLEAN@

install-data-local: $(top_srcdir)/examples/openhpi.conf.example
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)/openhpi
	if test ! -e  $(DESTDIR)$(sysconfdir)/openhpi/openhpi.conf; then \
		$(INSTALL_DATA) $(top_srcdir)/examples/openhpi.conf.example $(DESTDIR)$(sysconfdir)/openhpi/openhpi.conf; \
	fi

uninstall-local:
	if cmp $(top_srcdir)/examples/openhpi.conf.example $(DESTDIR)$(sysconfdir)/openhpi/openhpi.conf; then \
		: \
	else \
		rm -f $(DESTDIR)$(sysconfdir)/openhpi/openhpi.conf; \
	fi

# comment out for now, we do install in rpm
#pkgsysconfdir = $(sysconfdir)/$(PACKAGE)
#pkgsysconf_DATA = openhpi.conf.example

EXTRA_DIST = openhpi.conf.example

clean-local:
	rm -f *~

