from openhpi_baselib.HpiCore import HpiCore
from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib import HpiUtil
from openhpi_baselib import OhpiVersion


#**********************************************************
# OHPI API (NB: Partly implemented)
#**********************************************************

#**********************************************************
def oHpiVersionGet():
    ( vmaj, vmin, vaux ) = OhpiVersion.__version_info__
    return ( long( vmaj ) << 48 ) | ( long( vmin ) << 32 ) | ( long( vaux ) << 16 )

#**********************************************************
def oHpiDomainAdd( host, port, entity_root ):
    s = HpiUtil.fromSaHpiTextBufferT( host )
    d = HpiCore.createDomain( s, port, entity_root )
    if d is None:
        return ( SA_ERR_HPI_INTERNAL_ERROR, None )
    return ( SA_OK, d.getLocalDid() )

