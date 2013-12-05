# Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
#                     All rights reserved.
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
# Neither the name of the Hewlett-Packard Corporation, nor the names
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
# Author(s)
#      Raghavendra P.G. <raghavendra.pg@hp.com>
#      Raghavendra M.S. <raghavendra.ms@hp.com>
#      Sudhakar Rao <sudhakara-d-v.rao@hp.com>
#      Bryan Sutula <Bryan.Sutula@hp.com>
#      Bhaskara Bhatt <bhaskara.hg@hp.com>

CC=gcc
LIBS	= -lopenhpi -lopenhpiutils
GLIB_INCLUDE = -I/usr/include/glib-2.0
GLIB_CONF_INCLUDE = -I/usr/lib64/glib-2.0/include
GLIB_CONF_INCLUDE32 = -I/usr/lib/glib-2.0/include
OPENHPI_INCLUDE = -I../../../include -I../../../utils \
		  -I/usr/local/include -I../../../
OPENHPI_LIBS = -L ../../../openhpid/.libs

#CFLAGS	= $(OPENHPI_LIBS) -I. $(GLIB_INCLUDE) $(GLIB_CONF_INCLUDE) \
#	  $(GLIB_CONF_INCLUDE32) $(OPENHPI_INCLUDE) -g  
CFLAGS	= -I. $(GLIB_INCLUDE) $(GLIB_CONF_INCLUDE) $(GLIB_CONF_INCLUDE32) \
	  $(OPENHPI_INCLUDE) -g  

PROGS	= hpi_power_status_get \
	  hpi_power_status_get_all \
	  hpi_power_status_set \
	  hpi_reset_resource_get \
	  hpi_reset_resource_get_all \
	  hpi_reset_resource_set \
	  hpi_hs_state_get \
	  hpi_hs_state_get_all \
	  hpi_hs_action_request \
	  hpi_idr_info_get \
	  hpi_idr_info_get_all \
	  hpi_idr_area_header_get \
	  hpi_idr_area_add \
          hpi_idr_area_add_id \
	  hpi_idr_area_delete \
	  hpi_idr_area_add_delete \
	  hpi_idr_field_add \
          hpi_idr_field_add_by_id \
	  hpi_idr_field_get \
	  hpi_idr_field_set \
	  hpi_idr_field_delete \
	  hpi_eventlog_info_get \
	  hpi_eventlog_entry_get \
	  hpi_eventlog_clear \
	  hpi_eventlog_time_get \
	  hpi_eventlog_time_set \
	  hpi_eventlog_state_get \
	  hpi_eventlog_state_set \
	  hpi_eventlog_overflowflag_reset \
	  hpi_event_get \
	  hpi_control_type_get \
	  hpi_control_get \
	  hpi_control_set \
	  hpi_sensor_reading_get \
	  hpi_sensor_type_get \
	  hpi_sensor_enable_get \
	  hpi_sensor_enable_set \
	  hpi_sensor_event_enable_get \
	  hpi_sensor_event_enable_set \
	  hpi_sensor_event_mask_get \
	  hpi_sensor_event_mask_set \
          hpi_sensor_thresholds_get \
	  hpi_rpt_get \
	  hpi_set_tag

EXTRAS	= soaptest

all:	$(PROGS) $(EXTRAS)

hpi_power_status_get:	hpi_power_status_get.c
	$(CC) $(CFLAGS) -o $@ hpi_power_status_get.c hpi_common.c $(LIBS)

hpi_power_status_get_all:	hpi_power_status_get_all.c 
	$(CC) $(CFLAGS) -o $@ hpi_power_status_get_all.c hpi_common.c $(LIBS)

hpi_power_status_set:	hpi_power_status_set.c 
	$(CC) $(CFLAGS) -o $@ hpi_power_status_set.c hpi_common.c $(LIBS)

hpi_reset_resource_get:	hpi_reset_resource_get.c 
	$(CC) $(CFLAGS) -o $@ hpi_reset_resource_get.c hpi_common.c $(LIBS)

hpi_reset_resource_get_all:	hpi_reset_resource_get_all.c 
	$(CC) $(CFLAGS) -o $@ hpi_reset_resource_get_all.c hpi_common.c $(LIBS)

hpi_reset_resource_set:	hpi_reset_resource_set.c 
	$(CC) $(CFLAGS) -o $@ hpi_reset_resource_set.c hpi_common.c $(LIBS)

hpi_hs_state_get:	hpi_hs_state_get.c
	$(CC) $(CFLAGS) -o $@ hpi_hs_state_get.c hpi_common.c $(LIBS)

hpi_hs_state_get_all:	hpi_hs_state_get_all.c
	$(CC) $(CFLAGS) -o $@ hpi_hs_state_get_all.c hpi_common.c $(LIBS)

hpi_hs_action_request:	hpi_hs_action_request.c
	$(CC) $(CFLAGS) -o $@ hpi_hs_action_request.c hpi_common.c $(LIBS)

hpi_idr_info_get:	hpi_idr_info_get.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_info_get.c hpi_common.c $(LIBS)

hpi_idr_info_get_all:	hpi_idr_info_get_all.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_info_get_all.c hpi_common.c $(LIBS)
		
hpi_idr_area_header_get:	hpi_idr_area_header_get.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_area_header_get.c hpi_common.c $(LIBS)

hpi_idr_area_add:	hpi_idr_area_add.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_area_add.c hpi_common.c $(LIBS)

hpi_idr_area_add_id:       hpi_idr_area_add_id.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_area_add_id.c hpi_common.c $(LIBS)

hpi_idr_area_delete:	hpi_idr_area_delete.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_area_delete.c hpi_common.c $(LIBS)

hpi_idr_area_add_delete:	hpi_idr_area_add_delete.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_area_add_delete.c hpi_common.c $(LIBS)

hpi_idr_field_get:	hpi_idr_field_get.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_field_get.c hpi_common.c $(LIBS)

hpi_idr_field_add:	hpi_idr_field_add.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_field_add.c hpi_common.c $(LIBS)

hpi_idr_field_add_by_id:      hpi_idr_field_add_by_id.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_field_add_by_id.c hpi_common.c $(LIBS)

hpi_idr_field_set:	hpi_idr_field_set.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_field_set.c hpi_common.c $(LIBS)

hpi_idr_field_delete:	hpi_idr_field_delete.c
	$(CC) $(CFLAGS) -o $@ hpi_idr_field_delete.c hpi_common.c $(LIBS)

hpi_eventlog_info_get:	hpi_eventlog_info_get.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_info_get.c hpi_common.c $(LIBS)

hpi_eventlog_entry_get:	hpi_eventlog_entry_get.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_entry_get.c hpi_common.c $(LIBS)

hpi_eventlog_time_get:	hpi_eventlog_time_get.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_time_get.c hpi_common.c $(LIBS)

hpi_eventlog_time_set:	hpi_eventlog_time_set.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_time_set.c hpi_common.c $(LIBS)

hpi_eventlog_state_get:	hpi_eventlog_state_get.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_state_get.c hpi_common.c $(LIBS)

hpi_eventlog_state_set:	hpi_eventlog_state_set.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_state_set.c hpi_common.c $(LIBS)

hpi_eventlog_overflowflag_reset:	hpi_eventlog_overflowflag_reset.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_overflowflag_reset.c hpi_common.c \
	$(LIBS)

hpi_eventlog_clear:	hpi_eventlog_clear.c
	$(CC) $(CFLAGS) -o $@ hpi_eventlog_clear.c hpi_common.c $(LIBS)

hpi_event_get:	hpi_event_get.c
	$(CC) $(CFLAGS) -o $@ hpi_event_get.c hpi_common.c $(LIBS)

hpi_control_type_get:	hpi_control_type_get.c
	$(CC) $(CFLAGS) -o $@ hpi_control_type_get.c hpi_common.c $(LIBS)

hpi_control_get:	hpi_control_get.c
	$(CC) $(CFLAGS) -o $@ hpi_control_get.c hpi_common.c $(LIBS)

hpi_control_set:	hpi_control_set.c
	$(CC) $(CFLAGS) -o $@ hpi_control_set.c hpi_common.c $(LIBS)

hpi_sensor_reading_get:	hpi_sensor_reading_get.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_reading_get.c hpi_common.c $(LIBS)

hpi_sensor_type_get:	hpi_sensor_type_get.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_type_get.c hpi_common.c $(LIBS)

hpi_sensor_enable_get:	hpi_sensor_enable_get.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_enable_get.c hpi_common.c $(LIBS)

hpi_sensor_enable_set:	hpi_sensor_enable_set.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_enable_set.c hpi_common.c $(LIBS)

hpi_sensor_event_enable_get:	hpi_sensor_event_enable_get.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_event_enable_get.c hpi_common.c $(LIBS)

hpi_sensor_event_enable_set:	hpi_sensor_event_enable_set.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_event_enable_set.c hpi_common.c $(LIBS)

hpi_sensor_event_mask_get:	hpi_sensor_event_mask_get.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_event_mask_get.c hpi_common.c $(LIBS)

hpi_sensor_event_mask_set:	hpi_sensor_event_mask_set.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_event_mask_set.c hpi_common.c $(LIBS)

hpi_sensor_thresholds_get:	hpi_sensor_thresholds_get.c
	$(CC) $(CFLAGS) -o $@ hpi_sensor_thresholds_get.c hpi_common.c $(LIBS)

hpi_rpt_get:	hpi_rpt_get.c
	$(CC) $(CFLAGS) -o $@ hpi_rpt_get.c hpi_common.c $(LIBS)

hpi_set_tag:	hpi_set_tag.c
	$(CC) $(CFLAGS) -o $@ hpi_set_tag.c hpi_common.c $(LIBS)

soaptest:	soaptest.c ../oa_soap_callsupport.h ../oa_soap_calls.h \
		../../../ssl/oh_ssl.h
	$(CC) -Wall -g -O -I.. $(OPENHPI_INCLUDE) -I/usr/include/libxml2 \
	-I../../../ssl `pkg-config --cflags glib-2.0` -DHAVE_OPENSSL \
	-o $@ soaptest.c \
	../../../ssl/oh_ssl.o ../oa_soap_calls.o ../oa_soap_callsupport.o \
	-lglib-2.0 -lxml2 -lssl -lcrypto

clean: 
	-rm -rf *.core $(PROGS) $(EXTRAS)
