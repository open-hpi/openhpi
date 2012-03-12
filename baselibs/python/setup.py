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

