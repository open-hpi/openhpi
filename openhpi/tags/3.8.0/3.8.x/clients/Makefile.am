MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES     = @TEST_CLEAN@

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"client\" @CRYPT_FLAG@
AM_CFLAGS = @CRYPT_FLAG@

AM_CPPFLAGS             += @OPENHPI_INCLUDES@

SUBDIRS = hpixml
DIST_SUBDIRS = hpixml

EXTRA_DIST       = Makefile.mingw32 version.rc

COMMONLIBS	     = $(top_builddir)/baselib/libopenhpi.la $(top_builddir)/utils/libopenhpiutils.la

CLIENTS_SRC 	     = clients.c oh_clients.h

bin_PROGRAMS = \
    hpidomain \
    hpievents \
    hpifan \
    hpiinv \
    hpiel \
    hpigensimdata \
    hpisensor \
    hpipower \
    hpitree \
    hpialarms \
    hpireset \
    hpiwdt \
    $(HPICRYPT) \
    hpithres \
    hpitop   \
    hpisettime \
    hpionIBMblade \
    ohdomainlist \
    ohhandler \
    ohparam

EXTRA_PROGRAMS = hpicrypt

include_HEADERS         =

hpialarms_SOURCES   = hpialarms.c $(CLIENTS_SRC)
hpialarms_LDADD     = $(COMMONLIBS) 

hpidomain_SOURCES   	= hpidomain.c $(CLIENTS_SRC)
hpidomain_LDADD     	= $(COMMONLIBS) 

hpithres_SOURCES        = hpithres.c $(CLIENTS_SRC)
hpithres_LDADD          = $(COMMONLIBS)

hpifan_SOURCES          = hpifan.c $(CLIENTS_SRC)
hpifan_LDADD            = $(COMMONLIBS)

hpitop_SOURCES          = hpitop.c $(CLIENTS_SRC)
hpitop_LDADD            = $(COMMONLIBS)

hpireset_SOURCES        = hpireset.c $(CLIENTS_SRC)
hpireset_LDADD          = $(COMMONLIBS)

hpiel_SOURCES           = hpiel.c $(CLIENTS_SRC)
hpiel_LDADD             = $(COMMONLIBS)

hpigensimdata_SOURCES   = hpigensimdata.c $(CLIENTS_SRC)
hpigensimdata_LDADD     = $(COMMONLIBS)

hpisensor_SOURCES       = hpisensor.c $(CLIENTS_SRC)
hpisensor_LDADD         = $(COMMONLIBS)

hpiwdt_SOURCES          = hpiwdt.c $(CLIENTS_SRC)
hpiwdt_LDADD            = $(COMMONLIBS)

hpicrypt_SOURCES          = hpicrypt.c $(CLIENTS_SRC)
hpicrypt_LDADD            = $(COMMONLIBS)

hpiinv_SOURCES          = hpiinv.c $(CLIENTS_SRC)
hpiinv_LDADD            = $(COMMONLIBS)

hpisettime_SOURCES      = hpisettime.c $(CLIENTS_SRC)
hpisettime_LDADD        = $(COMMONLIBS)

hpievents_SOURCES      = hpievents.c $(CLIENTS_SRC)
hpievents_LDADD        = $(COMMONLIBS)

hpipower_SOURCES      = hpipower.c $(CLIENTS_SRC)
hpipower_LDADD        = $(COMMONLIBS)

hpitree_SOURCES      = hpitree.c $(CLIENTS_SRC)
hpitree_LDADD        = $(COMMONLIBS)

hpionIBMblade_SOURCES      = hpionIBMblade.c $(CLIENTS_SRC)
hpionIBMblade_LDADD        = $(COMMONLIBS)

ohdomainlist_SOURCES      = ohdomainlist.c $(CLIENTS_SRC)
ohdomainlist_LDADD        = $(COMMONLIBS)

ohhandler_SOURCES      = ohhandler.c $(CLIENTS_SRC)
ohhandler_LDADD        = $(COMMONLIBS)

ohparam_SOURCES      = ohparam.c $(CLIENTS_SRC)
ohparam_LDADD        = $(COMMONLIBS)

clean-local:
	rm -f *~ *.o

