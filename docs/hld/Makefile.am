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
DIAGRAMS = $(shell ls images/*png images/*gif) 

SERVER = shell1.sourceforge.net
HLDDIR = /home/groups/o/op/openhpi/htdocs/hld2

MAINTAINERCLEANFILES 	= Makefile.in  

openhpi_docsdir		= $(datadir)/openhpi/docs/hld

openhpi_docs_DATA	= \
				OpenHPI_HLD/book1.html 



SGML_FILES              = legal.dtd \
				authors.dtd \
				revisions.dtd \
				alternatives.sgml \
				internel_components.sgml \
				appendix.sgml \
				internel_data_structure.sgml \
				assumptions_and_deps.sgml \
				internel_methods.sgml \
				bookinfo.sgml \
				introduction.sgml \
				design_decomposition.sgml \
				methods_and_notations.sgml \
				design_strategies.sgml \
				OpenHPI_HLD.sgml \
				externel_apis.sgml \
				system_deps.sgml \
				externel_data_structure.sgml \
				sahpi_breakdown.sgml


EXTRA_DIST		= $(DIAGRAMS) OpenHPI_HLD.sgml

all:

pdf-am:
	@echo we have to do this twice to get the latex refs right
	db2pdf OpenHPI_HLD.sgml
	db2pdf OpenHPI_HLD.sgml

ps-am:
	@echo we have to do this twice to get the latex refs right
	db2ps OpenHPI_HLD.sgml
	db2ps OpenHPI_HLD.sgml


OpenHPI_HLD/book1.html: $(SGML_FILES)
	-db2html OpenHPI_HLD.sgml
	-mkdir -p OpenHPI_HLD/images
	-cp $(DIAGRAMS) OpenHPI_HLD/images

publish: OpenHPI_HLD/book1.html
	-rsync -rP OpenHPI_HLD/ $(SERVER):$(HLDDIR)
	-ssh $(SERVER) "find $(HLDDIR) -user `whoami` -exec chmod g+w {} \;"
	-ssh $(SERVER) "find $(HLDDIR) -user `whoami` -type d -exec chmod g+s {} \;"

clean:
	rm -fR OpenHPI_HLD
	rm -f *~ *.tex *.aux *.log *.out
