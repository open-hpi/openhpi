
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

from distutils.core import setup
from openhpi_baselib import OhpiVersion

setup( name         = "openhpi_baselib",
       version      = ".".join( map( str, OhpiVersion.__version_info__ ) ),
       description  = "OpenHPI Base Library For Python",
       author       = "Anton Pak",
       author_email = "anton.pak@pigeonpoint.com",
       url          = "http://openhpi.org",
       license      = "BSD",
       packages     = [ "openhpi_baselib" ] )

