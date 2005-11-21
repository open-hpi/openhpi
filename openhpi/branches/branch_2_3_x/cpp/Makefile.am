#      -*- linux-c -*-
#
# (C) Copyright IBM Corp. 2005
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#        W. David Ashley <dashley@us.ibm.com>
#

SUBDIRS                 = t
DIST_SUBDIRS            = t

MAINTAINERCLEANFILES 	= Makefile.in

INCLUDES		= @OPENHPI_INCLUDES@

AM_CFLAGS		= -g @CFLAGS@

pkglib_LTLIBRARIES	= libosahpi.la

libosahpi_la_LIBADD	= -luuid
libosahpi_la_SOURCES	= oSaHpiAlarm.cpp oSaHpiAlarm.hpp \
                          oSaHpiAnnouncement.cpp oSaHpiAnnouncement.hpp \
                          oSaHpiAnnunciatorRec.cpp oSaHpiAnnunciatorRec.hpp \
                          oSaHpiCondition.cpp oSaHpiCondition.hpp \
                          oSaHpiCtrlDefaultMode.cpp oSaHpiCtrlDefaultMode.hpp \
                          oSaHpiCtrlRec.cpp oSaHpiCtrlRec.hpp \
                          oSaHpiCtrlRecAnalog.cpp oSaHpiCtrlRecAnalog.hpp \
                          oSaHpiCtrlRecDigital.cpp oSaHpiCtrlRecDigital.hpp \
                          oSaHpiCtrlRecDiscrete.cpp oSaHpiCtrlRecDiscrete.hpp \
                          oSaHpiCtrlRecOem.cpp oSaHpiCtrlRecOem.hpp \
                          oSaHpiCtrlRecStream.cpp oSaHpiCtrlRecStream.hpp \
                          oSaHpiCtrlRecText.cpp oSaHpiCtrlRecText.hpp \
                          oSaHpiCtrlState.cpp oSaHpiCtrlState.hpp \
                          oSaHpiCtrlStateOem.cpp oSaHpiCtrlStateOem.hpp \
                          oSaHpiCtrlStateStream.cpp oSaHpiCtrlStateStream.hpp \
                          oSaHpiCtrlStateText.cpp oSaHpiCtrlStateText.hpp \
                          oSaHpiDomainEvent.cpp oSaHpiDomainEvent.hpp \
                          oSaHpiDomainInfo.cpp oSaHpiDomainInfo.hpp \
                          oSaHpiDrtEntry.cpp oSaHpiDrtEntry.hpp \
                          oSaHpiEntity.cpp oSaHpiEntity.hpp \
                          oSaHpiEntityPath.cpp oSaHpiEntityPath.hpp \
                          oSaHpiEvent.cpp oSaHpiEvent.hpp \
                          oSaHpiEventLogEntry.cpp oSaHpiEventLogEntry.hpp \
                          oSaHpiEventLogInfo.cpp oSaHpiEventLogInfo.hpp \
                          oSaHpiHotSwapEvent.cpp oSaHpiHotSwapEvent.hpp \
                          oSaHpiHpiSwEvent.cpp oSaHpiHpiSwEvent.hpp \
                          oSaHpiIdrAreaHeader.cpp oSaHpiIdrAreaHeader.hpp \
                          oSaHpiIdrField.cpp oSaHpiIdrField.hpp \
                          oSaHpiIdrInfo.cpp oSaHpiIdrInfo.hpp \
                          oSaHpiInventoryRec.cpp oSaHpiInventoryRec.hpp \
                          oSaHpiName.cpp oSaHpiName.hpp \
                          oSaHpiOemEvent.cpp oSaHpiOemEvent.hpp \
                          oSaHpiRdr.cpp oSaHpiRdr.hpp \
                          oSaHpiResourceEvent.cpp oSaHpiResourceEvent.hpp \
                          oSaHpiResourceInfo.cpp oSaHpiResourceInfo.hpp \
                          oSaHpiRptEntry.cpp oSaHpiRptEntry.hpp \
                          oSaHpiSensorDataFormat.cpp oSaHpiSensorDataFormat.hpp \
                          oSaHpiSensorEnableChangeEvent.cpp oSaHpiSensorEnableChangeEvent.hpp \
                          oSaHpiSensorEvent.cpp oSaHpiSensorEvent.hpp \
                          oSaHpiSensorRange.cpp oSaHpiSensorRange.hpp \
                          oSaHpiSensorReading.cpp oSaHpiSensorReading.hpp \
                          oSaHpiSensorRec.cpp oSaHpiSensorRec.hpp \
                          oSaHpiSensorThdDefn.cpp oSaHpiSensorThdDefn.hpp \
                          oSaHpiSensorThresholds.cpp oSaHpiSensorThresholds.hpp \
                          oSaHpiTextBuffer.cpp oSaHpiTextBuffer.hpp \
                          oSaHpiTypesEnums.cpp oSaHpiTypesEnums.hpp \
                          oSaHpiUserEvent.cpp oSaHpiUserEvent.hpp \
                          oSaHpiWatchdog.cpp oSaHpiWatchdog.hpp \
                          oSaHpiWatchdogEvent.cpp oSaHpiWatchdogEvent.hpp \
                          oSaHpiWatchdogRec.cpp oSaHpiWatchdogRec.hpp
libosahpi_la_LDFLAGS	= -version-info @HPI_LIB_VERSION@
