MAINTAINERCLEANFILES = Makefile.in

MOSTLYCLEANFILES        = @TEST_CLEAN@

#sysconf_DATA = openhpi.conf

EXTRA_DIST = openhpi.conf.example

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

#openhpi.conf: openhpi.conf.example
#	cp -f $(srcdir)/openhpi.conf.example openhpi.conf

clean-local:
	rm -f *~

