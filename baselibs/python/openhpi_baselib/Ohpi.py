
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

from openhpi_baselib.HpiCore import HpiCore
from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib import HpiUtil
from openhpi_baselib import OhpiDataTypes
from openhpi_baselib import OhpiUtil
from openhpi_baselib import OhpiVersion


#**********************************************************
# OHPI API (NB: Partly implemented)
#**********************************************************

#**********************************************************
def oHpiVersionGet():
    ( vmaj, vmin, vaux ) = OhpiVersion.__version_info__
    return ( long( vmaj ) << 48 ) | ( long( vmin ) << 32 ) | ( long( vaux ) << 16 )

#**********************************************************
def oHpiHandlerCreate( sid, config ):
    rc = OhpiUtil.checkoHpiHandlerConfigT( config )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None )

    s = HpiCore.getSession( sid )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshaloHpiHandlerConfigT( config )
    rc = m.interchange( OhpiDataTypes.RPC_OHPI_HANDLER_CREATE )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        hid = m.demarshaloHpiHandlerIdT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, hid )

#**********************************************************
def oHpiHandlerDestroy( sid, hid ):
    s = HpiCore.getSession( sid )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshaloHpiHandlerIdT( hid )
    rc = m.interchange( OhpiDataTypes.RPC_OHPI_HANDLER_DESTROY )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def oHpiHandlerInfo( sid, hid ):
    s = HpiCore.getSession( sid )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshaloHpiHandlerIdT( hid )
    rc = m.interchange( OhpiDataTypes.RPC_OHPI_HANDLER_INFO )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        info = m.demarshaloHpiHandlerInfoT()
        config = m.demarshaloHpiHandlerConfigT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, info, config )

#**********************************************************
def oHpiHandlerGetNext( sid, hid ):
    s = HpiCore.getSession( sid )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshaloHpiHandlerIdT( hid )
    rc = m.interchange( OhpiDataTypes.RPC_OHPI_HANDLER_GET_NEXT )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        next_hid = m.demarshaloHpiHandlerIdT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, next_hid )

#**********************************************************
def oHpiHandlerFind( sid, rid ):
    s = HpiCore.getSession( sid )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( rid )
    rc = m.interchange( OhpiDataTypes.RPC_OHPI_HANDLER_FIND )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        hid = m.demarshaloHpiHandlerIdT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, hid )

#**********************************************************
def oHpiHandlerRetry( sid, hid ):
    s = HpiCore.getSession( sid )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshaloHpiHandlerIdT( hid )
    rc = m.interchange( OhpiDataTypes.RPC_OHPI_HANDLER_RETRY )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def oHpiGlobalParamGet( sid ):
    return ( SA_ERR_HPI_UNSUPPORTED_API, None )
#RPC_OHPI_GLOBAL_PARAM_GET                        = 113
#SaErrorT SAHPI_API oHpiGlobalParamGet (
#     SAHPI_IN    SaHpiSessionIdT sid,
#     SAHPI_INOUT oHpiGlobalParamT *param );
#

#**********************************************************
def oHpiGlobalParamSet( sid, param ):
    return SA_ERR_HPI_UNSUPPORTED_API
#RPC_OHPI_GLOBAL_PARAM_SET                        = 114
#SaErrorT SAHPI_API oHpiGlobalParamSet (
#     SAHPI_IN    SaHpiSessionIdT sid,
#     SAHPI_IN    oHpiGlobalParamT *param );
#

#**********************************************************
def oHpiInjectEvent( sid, hid, event, rpte, rdr ):
    return SA_ERR_HPI_UNSUPPORTED_API
#RPC_OHPI_INJECT_EVENT                            = 115
#SaErrorT SAHPI_API oHpiInjectEvent (
#     SAHPI_IN    SaHpiSessionIdT sid,
#     SAHPI_IN    oHpiHandlerIdT id,
#     SAHPI_IN    SaHpiEventT    *event,
#     SAHPI_IN    SaHpiRptEntryT *rpte,
#     SAHPI_IN    SaHpiRdrT *rdr);

#**********************************************************
def oHpiDomainAdd( host, port, entity_root ):
    s = HpiUtil.fromSaHpiTextBufferT( host )
    d = HpiCore.createDomain( s, port, entity_root )
    if d is None:
        return ( SA_ERR_HPI_INTERNAL_ERROR, None )
    return ( SA_OK, d.getLocalDid() )

#**********************************************************
def oHpiDomainAddById( did, host, port, entity_root ):
    return SA_ERR_HPI_UNSUPPORTED_API
#SaErrorT SAHPI_API oHpiDomainAddById (
#     SAHPI_IN    SaHpiDomainIdT domain_id,
#     SAHPI_IN    const SaHpiTextBufferT *host,
#     SAHPI_IN    SaHpiUint16T port,
#     SAHPI_IN    const SaHpiEntityPathT *entity_root );
#

#**********************************************************
def oHpiDomainEntryGet( eid ):
    return ( SA_ERR_HPI_UNSUPPORTED_API, None, None )
#SaErrorT SAHPI_API oHpiDomainEntryGet (
#     SAHPI_IN    SaHpiEntryIdT    EntryId,
#     SAHPI_OUT   SaHpiEntryIdT    *NextEntryId,
#     SAHPI_OUT   oHpiDomainEntryT *DomainEntry );
#

#**********************************************************
def oHpiDomainEntryGetByDomainId( did ):
    return ( SA_ERR_HPI_UNSUPPORTED_API, None )
#SaErrorT SAHPI_API oHpiDomainEntryGetByDomainId (
#     SAHPI_IN    SaHpiDomainIdT    DomainId,
#     SAHPI_OUT   oHpiDomainEntryT *DomainEntry );

