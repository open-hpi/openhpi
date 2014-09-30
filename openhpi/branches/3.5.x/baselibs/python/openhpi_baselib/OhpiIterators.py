
#      -*- python -*-
#
# Copyright (C) 2012, Pigeon Point Systems
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#      Anton Pak <anton.pak@pigeonpoint.com>
#

from openhpi_baselib.Ohpi import *


#**********************************************************
# OHPI Utility Functions: Iterators
#**********************************************************

#**********************************************************
# Iterate over Handler ids
#**********************************************************
def HandlerIds( sid ):
    hid = SAHPI_FIRST_ENTRY
    while hid != SAHPI_LAST_ENTRY:
        ( rv, hid ) = oHpiHandlerGetNext( sid, hid )
        if rv != SA_OK:
            break
        yield hid

