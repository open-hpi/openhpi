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


RPMREL			= @RPMREL@
RPM			= @RPM@
RPMFLAGS		= -ta
TARFILE			= $(distdir).tar.gz

DESTDIR			=
RPMDIR			=$(HOME)/rpms
RPMSRC	=$(DESTDIR)$(RPMDIR)/SRPMS/$(PACKAGE)-$(VERSION)-$(RPMREL).src.rpm
RPM386	=$(DESTDIR)$(RPMDIR)/RPMS/i386/$(PACKAGE)-$(VERSION)-$(RPMREL).i386.rpm

EXTRA_DIST		= openhpi.spec BigBuild bootstrap nukem.sh \
	$(shell find $(srcdir)/include -name '*.h')

AUTOMAKE_OPTIONS	= foreign

MAINTAINERCLEANFILES 	= Makefile.in aclocal.m4 configure config.guess config.sub \
			depcomp install-sh ltmain.sh missing mkinstalldirs config.h.in \
			stamp-h.in cscope.files cscope.out $(distdir).tar.gz 
MOSTLYCLEANFILES	= tags

SUBDIRS			= utils @SNMPDIR@ @MARSHALDIR@ plugins src docs @ENABLED_DIRS@ test
DIST_SUBDIRS 		= utils snmp marshal plugins src docs openhpid examples clients test

DISTCHECK_CONFIGURE_FLAGS = --with-initdir=prefix

ALLSOURCES = \
	find $(top_srcdir) \( -name SCCS -o -name BitKeeper -o -name .svn -o -name CVS \) -prune -o \
	-name '*.[chS]' -print

includedir=$(base_includedir)/openhpi

# these are the only headers that users should have any access to
include_HEADERS	= $(top_srcdir)/include/SaHpi.h \
                  $(top_srcdir)/utils/epath_utils.h \
                  $(top_srcdir)/utils/uid_utils.h \
                  $(top_srcdir)/utils/rpt_utils.h \
                  $(top_srcdir)/utils/sel_utils.h \
                  $(top_srcdir)/utils/ecode_utils.h \
                  $(top_srcdir)/utils/printevent_utils.h \
                  $(top_srcdir)/utils/sensor_utils.h \
                  $(top_srcdir)/utils/oh_utils.h

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
	$(MAKE) -C examples clean
am_config_clean-local:
	rm -fr $(PACKAGE)-*.tar.gz

report:
	$(MAKE) -C test report

tags:   FORCE
	@echo making tags		
	$(ALLSOURCES) |xargs ctags
TAGS:   FORCE
	@echo making TAGS
	$(ALLSOURCES) |xargs etags
cscope: FORCE
	@echo making cscope
	$(ALLSOURCES) > cscope.files
	cscope -b

.PHONY: FORCE
