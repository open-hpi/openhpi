#
# Copyright IBM 2005
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

MAINTAINERCLEANFILES 	= Makefile.in

EXTRA_DIST		= openhpi.7.pod   openhpid.8.pod      \
                          hpialarms.1.pod hpifan.1.pod        \
                          hpipower.1.pod  hpisettime.1.pod    \
                          hpitree.1.pod   hpiel.1.pod         \
                          hpiinv.1.pod    hpireset.1.pod      \
                          hpithres.1.pod  hpiwdt.1.pod        \
                          hpievents.1.pod hpionIBMblade.1.pod \
                          hpisensor.1.pod hpitop.1.pod \
			  hpidomain.1.pod hpigensimdata.1.pod \
			  hpixml.1.pod hpicrypt.1.pod \
			  ohhandler.1.pod ohparam.1.pod \
                          ohdomainlist.1.pod \
			  hpi_shell.1.pod


SUFFIXES=.pod

.pod:
	pod2man --release=$(VERSION) --name=`echo $@ | sed 's/\..*//'` \
		-s `echo $@ | sed 's/.*\.//'` -c "OpenHPI" $< $@

#mandir = $(datadir)/man
man_MANS = openhpi.7   openhpid.8      \
           hpialarms.1 hpifan.1        \
           hpipower.1  hpisettime.1    \
           hpitree.1   hpiel.1         \
           hpiinv.1    hpireset.1      \
           hpithres.1  hpiwdt.1        \
           hpievents.1 hpionIBMblade.1 \
           hpisensor.1 hpitop.1        \
	   hpidomain.1 hpigensimdata.1 \
	   hpixml.1 $(HPICRYPT_MAN) \
           ohhandler.1 ohparam.1       \
           ohdomainlist.1 \
           hpi_shell.1

clean-local: am_config_clean-local

am_config_clean-local:
	rm -f *~
	find . -name \*\.[13578] | grep -v pod | xargs rm -f


.PHONY: FORCE
