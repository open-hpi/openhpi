MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES     = @TEST_CLEAN@

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"hpixml\"
AM_CCASFLAGS = -DSCHEMA=\"$(srcdir)/schema.xsd\"

INCLUDES       = @OPENHPI_INCLUDES@

EXTRA_DIST     = Makefile.mingw32 schema.xsd

bin_PROGRAMS   = hpixml

hpixml_SOURCES = \
    hpi.cpp \
    hpi.h \
    hpi_xml_writer.cpp \
    hpi_xml_writer.h \
    main.cpp \
    schema.h \
    schema.S \
    xml_writer.cpp \
    xml_writer.h

hpixml_LDADD   = $(top_builddir)/baselib/libopenhpi.la

clean-local:
	rm -f *~ *.o

