from HpiGen import *


#**********************************************************
# HPI API
# Specific functions that were hard to auto-generate.
# So they were written manually.
#**********************************************************

#**********************************************************
def saHpiSessionOpen(
    DomainId
):
    s = HpiCore.createSession( DomainId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_DOMAIN, None )
    m = s.getMarshal()
    if m is None:
        HpiCore.removeSession( s );
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiDomainIdT( s.getRemoteDid() );
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SESSION_OPEN )
    if not rc:
        m.close()
        HpiCore.removeSession( s );
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        SessionId = m.demarshalSaHpiSessionIdT()
        s.setRemoteSid( SessionId );
        SessionId = s.getLocalSid();

    s.putMarshal( m )

    if rv != SA_OK:
        HpiCore.removeSession( s );
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
    # TODO
    return SA_ERR_HPI_UNSUPPORTED_API

