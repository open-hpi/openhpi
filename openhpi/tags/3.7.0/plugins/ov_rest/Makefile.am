#
# (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
#                    All rights reserved.
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
# Neither the name of the Hewlett Packard Enterprise, nor the names
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

MAINTAINERCLEANFILES    = Makefile.in

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"ov_rest\"

AM_CPPFLAGS  += @OPENHPI_INCLUDES@ 

pkglib_LTLIBRARIES      = libov_rest.la

EXTRA_DIST = \
             OpenHPI_Synergy_DevGuide.pdf

libov_rest_la_SOURCES   = ov_rest_parser_calls.c  \
                        ov_rest_parser_calls.h  \
                        ov_rest_event.c \
                        ov_rest_event.h \
                        ov_rest_composer_event.c \
                        ov_rest_composer_event.h \
                        ov_rest_server_event.c \
                        ov_rest_server_event.h \
                        ov_rest_interconnect_event.c \
                        ov_rest_interconnect_event.h \
                        ov_rest_ps_event.c \
                        ov_rest_ps_event.h \
                        ov_rest_fan_event.c \
                        ov_rest_fan_event.h \
                        ov_rest.c  \
                        ov_rest.h  \
                        ov_rest_discover.c \
                        ov_rest_discover.h \
                        ov_rest_re_discover.c \
                        ov_rest_re_discover.h \
                        ov_rest_inventory.h \
                        ov_rest_inventory.c \
                        ov_rest_sensor.c \
                        ov_rest_sensor.h \
                        ov_rest_callsupport.c \
                        ov_rest_callsupport.h \
                        ov_rest_utils.c \
                        ov_rest_utils.h \
                        ov_rest_power.c \
                        ov_rest_power.h \
                        ov_rest_reset.c \
                        ov_rest_reset.h  \
                        ov_rest_control.c \
                        ov_rest_control.h \
                        ov_rest_hotswap.c \
                        ov_rest_hotswap.h \
                        ov_rest_resources.c \
                        ov_rest_resources.h \
                        ov_rest_annunciator.c \
                        ov_rest_annunciator.h \
                        ov_rest_watchdog.c \
                        ov_rest_watchdog.h \
                        ov_rest_dimi.c \
                        ov_rest_dimi.h \
                        ov_rest_fumi.c \
                        ov_rest_fumi.h \
                        ov_rest_sel.c \
                        ov_rest_sel.h \
                        ov_rest_load_id.c \
                        ov_rest_load_id.h


# TODO: What about -luuid in the ilo2_ribcl?
libov_rest_la_LIBADD    = @CURL_LIB@ @JSON_C_LIB@ @RABBITMQ_LIB@ \
                          $(top_builddir)/utils/libopenhpiutils.la
libov_rest_la_LDFLAGS   = -module -version-info @HPI_LIB_VERSION@
