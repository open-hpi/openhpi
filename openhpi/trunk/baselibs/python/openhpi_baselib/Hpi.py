
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

from openhpi_baselib.HpiGen import *


#**********************************************************
# HPI API
# Specific functions that were hard to auto-generate.
# So they were written manually.
#**********************************************************

#**********************************************************
def saHpiSessionOpen(
    DomainId,
    SecurityParams
):
    if SecurityParams is not None:
        return ( SA_ERR_HPI_INVALID_PARAMS, None )
    s = HpiCore.createSession( DomainId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_DOMAIN, None )
    m = s.getMarshal()
    if m is None:
        HpiCore.removeSession( s )
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiDomainIdT( s.getRemoteDid() )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SESSION_OPEN )
    if not rc:
        m.close()
        HpiCore.removeSession( s )
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        SessionId = m.demarshalSaHpiSessionIdT()
        s.setRemoteSid( SessionId )
        SessionId = s.getLocalSid()

    s.putMarshal( m )

    if rv != SA_OK:
        HpiCore.removeSession( s )
        return ( rv, None )
    return ( SA_OK, SessionId )

#**********************************************************
def saHpiSessionClose(
    SessionId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SESSION_CLOSE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv == SA_OK:
        HpiCore.removeSession( s )
    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiMyEntityPathGet(
    SessionId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    my_ep = HpiCore.getMyEntity()
    if my_ep is None:
        return ( SA_ERR_HPI_UNKNOWN, None )
    return ( SA_OK, my_ep )

#**********************************************************
def saHpiDimiTestStart(
    SessionId,
    ResourceId,
    DimiNum,
    TestNum,
    NumberOfParams,
    ParamsList
):
    if NumberOfParams != 0:
        if ParamsList is None:
            return SA_ERR_HPI_INVALID_PARAMS
        if not isinstance( ParamsList, list ):
            return SA_ERR_HPI_INVALID_PARAMS
        if NumberOfParams > len( ParamsList ):
            return SA_ERR_HPI_INVALID_PARAMS
        for p in ParamsList:
            rc = HpiUtil.checkSaHpiDimiTestVariableParamsT( p )
            if not rc:
                return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiDimiNumT( DimiNum )
    m.marshalSaHpiDimiTestNumT( TestNum )
    m.marshalSaHpiUint8T( NumberOfParams )
    if ParamsList is not None:
        for p in ParamsList:
            m.marshalSaHpiDimiTestVariableParamsT( p )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DIMI_TEST_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

