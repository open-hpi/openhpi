
#
# TODO: license
#
# Copyright (c) 2003 Intel Corporation <andrea.l.brugger@intel.com>
# 
#
RPMREL			= @RPMREL@
RPM			= @RPM@
RPMFLAGS		= -ta
TARFILE			= $(distdir).tar.gz

DESTDIR			=
RPMDIR			=$(HOME)/rpms
RPMSRC	=$(DESTDIR)$(RPMDIR)/SRPMS/$(PACKAGE)-$(VERSION)-$(RPMREL).src.rpm
RPM386	=$(DESTDIR)$(RPMDIR)/RPMS/i386/$(PACKAGE)-$(VERSION)-$(RPMREL).i386.rpm

EXTRA_DIST		= openhpi.spec

AUTOMAKE_OPTIONS	= foreign

MAINTAINERCLEANFILES 	= Makefile.in aclocal.m4 configure 

SUBDIRS			= include lib test

DISTCHECK_CONFIGURE_FLAGS = --with-initdir=prefix

rpm: dist
	$(RPM) $(RPMFLAGS) $(TARFILE) </dev/null;


clean: am_config_clean
am_config_clean:
	rm -fr $(PACKAGE)-*.tar.gz


