MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES     = @TEST_CLEAN@

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"hpixml\"
AM_CCASFLAGS = -DSCHEMA=\"$(srcdir)/schema.xsd\"

AM_CPPFLAGS       += @OPENHPI_INCLUDES@

EXTRA_DIST     = Makefile.mingw32 schema.xsd version.rc

bin_PROGRAMS   = hpixml

hpixml_SOURCES = \
    flags.h \
    hpi.cpp \
    hpi.h \
    hpi_xml_writer.cpp \
    hpi_xml_writer.h \
    main.cpp \
    schema.h \
    schema.S \
    xml_writer.cpp \
    xml_writer.h \
    $(top_srcdir)/clients/clients.c \
    $(top_srcdir)/clients/oh_clients.h

hpixml_LDADD   = $(top_builddir)/baselib/libopenhpi.la $(top_builddir)/utils/libopenhpiutils.la

clean-local:
	rm -f *~ *.o

