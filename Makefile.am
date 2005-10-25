#
# Copyright (c) 2003, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or
# without modification, are permitted provided that the following
# conditions are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in
# the documentation and/or other materials provided with the distribution.
#
# Neither the name of Intel Corporation nor the names
# of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#AUTOMAKE_OPTIONS        = 1.8

RPMREL                  = @RPMREL@
RPM                     = @RPM@
RPMFLAGS                = -ta
TARFILE                 = $(distdir).tar.gz

DESTDIR                 =
RPMDIR                  =$(HOME)/rpms
RPMSRC  =$(DESTDIR)$(RPMDIR)/SRPMS/$(PACKAGE)-$(VERSION)-$(RPMREL).src.rpm
RPM386  =$(DESTDIR)$(RPMDIR)/RPMS/i386/$(PACKAGE)-$(VERSION)-$(RPMREL).i386.rpm

EXTRA_DIST              = openhpi.spec \
        $(shell find $(srcdir)/include -name '*.h')

AUTOMAKE_OPTIONS        = foreign

MAINTAINERCLEANFILES    = Makefile.in aclocal.m4 configure config.guess config.sub \
                        depcomp install-sh ltmain.sh missing mkinstalldirs config.h.in \
                        stamp-h.in cscope.files cscope.out $(distdir).tar.gz
MOSTLYCLEANFILES        = tags


SUBDIRS                 = utils tools src hpi_ui_lib @SNMPDIR@ @MARSHALDIR@ plugins docs hpi_shell @ENABLED_DIRS@ examples test man
DIST_SUBDIRS            = utils tools src hpi_ui_lib snmp marshal plugins docs hpi_shell openhpid2 examples clients test scripts man cpp


DISTCHECK_CONFIGURE_FLAGS = --with-initdir=prefix

ALLSOURCES = \
        $(shell find $(top_srcdir)/include -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/') \
        $(shell find $(top_srcdir)/src -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/') \
        $(shell find $(top_srcdir)/utils -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/') \
        $(shell find $(top_srcdir)/openhpid2 -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/') \
        $(shell find $(top_srcdir)/plugins -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/') \
        $(shell find $(top_srcdir)/snmp -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/') \
        $(shell find $(top_srcdir)/marshal -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/') \
        $(shell find $(top_srcdir)/clients -name .svn -o -name CVS -prune -o \
        -name '*.[chS]' -print | grep -v '/t/')

includedir=$(base_includedir)/openhpi

# These are the only headers that users should have any access to
# The sim_injector_ext.h file is included for the convienence of simulator users
include_HEADERS = $(top_srcdir)/include/SaHpi.h \
                  $(top_srcdir)/include/oHpi.h \
                  $(top_srcdir)/utils/oh_utils.h \
                  $(top_srcdir)/utils/announcement_utils.h \
                  $(top_srcdir)/utils/rpt_utils.h \
                  $(top_srcdir)/utils/sahpi_enum_utils.h \
                  $(top_srcdir)/utils/sahpi_event_encode.h \
                  $(top_srcdir)/utils/sahpi_event_utils.h \
                  $(top_srcdir)/utils/sahpi_struct_utils.h \
                  $(top_srcdir)/utils/sahpi_time_utils.h \
                  $(top_srcdir)/utils/uid_utils.h \
                  $(top_srcdir)/utils/epath_utils.h \
                  $(top_srcdir)/utils/el_utils.h \
                  $(top_srcdir)/plugins/simulator/sim_injector_ext.h

$(top_srcdir)/utils/sahpi_enum_utils.h:
	make -C $(top_builddir)/utils sahpi_enum_utils.h

install-data-local: openhpi.pc
	$(mkinstalldirs) $(DESTDIR)$(libdir)/pkgconfig
	$(INSTALL_DATA) openhpi.pc $(DESTDIR)$(libdir)/pkgconfig
	$(mkinstalldirs) $(DESTDIR)$(VARPATH)
	chmod 777 $(DESTDIR)$(VARPATH)

uninstall-local:
	rm -f $(DESTDIR)$(libdir)/pkgconfig/openhpi.pc


rpm: dist
	$(RPM) $(RPMFLAGS) $(TARFILE) </dev/null;

documentation:
	$(MAKE) -C docs

clean-local: am_config_clean-local

am_config_clean-local:
	rm -fr $(PACKAGE)-*.tar.gz

report:
	$(MAKE) -C test report

tags:   FORCE
	@echo making tags
	ctags $(ALLSOURCES)
TAGS:   FORCE
	@echo making TAGS
	etags $(ALLSOURCES)
cscope: FORCE
	@echo making cscope
	echo "$(ALLSOURCES)" | sed -e 's/[ ]\+/\n/g' > cscope.files
	cscope -b

.PHONY: FORCE
