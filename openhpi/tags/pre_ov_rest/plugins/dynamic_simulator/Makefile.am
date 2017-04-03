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

MAINTAINERCLEANFILES    = Makefile.in *~ core core.*

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"dynsim\"

AM_CPPFLAGS                += @OPENHPI_INCLUDES@ 

pkglib_LTLIBRARIES      = libdyn_simulator.la

EXTRA_DIST = \
             doc.html.tgz \
             Doxyfile \
             README
             
libdyn_simulator_la_SOURCES= \
		array.h \
		new_sim.h \
		new_sim.cpp \
		new_sim_domain.h \
		new_sim_domain.cpp \
		new_sim_resource.h \
		new_sim_resource.cpp \
		new_sim_event_log.h \
		new_sim_event_log.cpp \
        new_sim_file.h \
        new_sim_file.cpp \
        new_sim_file_util.h \
        new_sim_file_util.cpp \
        new_sim_file_rdr.h \
        new_sim_file_rdr.cpp \
        new_sim_file_sensor.h \
        new_sim_file_sensor.cpp \
        new_sim_file_control.h \
        new_sim_file_control.cpp \
        new_sim_file_annunciator.cpp \
        new_sim_file_annunciator.h  \
        new_sim_file_inventory.cpp \
        new_sim_file_inventory.h \
        new_sim_file_watchdog.cpp \
        new_sim_file_watchdog.h \
        new_sim_file_fumi.cpp \
        new_sim_file_fumi.h \
        new_sim_file_dimi.cpp \
        new_sim_file_dimi.h \
		new_sim_entity.h \
		new_sim_entity.cpp \
		new_sim_log.h \
		new_sim_log.cpp \
		new_sim_rdr.h \
		new_sim_rdr.cpp \
		new_sim_announcement.h \
		new_sim_announcement.cpp \
		new_sim_annunciator.h \
		new_sim_annunciator.cpp \
		new_sim_sensor.h \
		new_sim_sensor.cpp \
		new_sim_sensor_common.h \
		new_sim_sensor_common.cpp \
		new_sim_sensor_threshold.h \
		new_sim_sensor_threshold.cpp \
		new_sim_control.h \
        new_sim_control.cpp \
		new_sim_control_digital.h \
        new_sim_control_digital.cpp \
       	new_sim_control_discrete.h \
        new_sim_control_discrete.cpp \
        	new_sim_control_analog.h \
        new_sim_control_analog.cpp \
        new_sim_control_stream.h \
        new_sim_control_stream.cpp \
        	new_sim_control_text.h \
        new_sim_control_text.cpp \
        	new_sim_control_oem.h \
        new_sim_control_oem.cpp \
        new_sim_inventory.cpp \
        new_sim_inventory.h \
        new_sim_inventory_data.cpp \
        new_sim_inventory_data.h \
        new_sim_watchdog.cpp \
        new_sim_watchdog.h \
        new_sim_fumi.cpp \
        new_sim_fumi.h \
        new_sim_fumi_data.cpp \
        new_sim_fumi_data.h \
        new_sim_dimi.cpp \
        new_sim_dimi.h \
        new_sim_dimi_data.cpp \
        new_sim_dimi_data.h \
		new_sim_text_buffer.h \
		new_sim_text_buffer.cpp \
		new_sim_utils.h \
		new_sim_utils.cpp \
		new_sim_hotswap.h \
		new_sim_hotswap.cpp \
		new_sim_timer_thread.h \
		new_sim_timer_thread.cpp \
		thread.h \
		thread.cpp

libdyn_simulator_la_LIBADD	= -lm -lstdc++ $(top_builddir)/utils/libopenhpiutils.la
libdyn_simulator_la_LDFLAGS= -module -version-info @HPI_LIB_VERSION@

clean-local:
	rm -f *~
