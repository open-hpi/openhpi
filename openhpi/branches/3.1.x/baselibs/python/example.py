
from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib.Hpi import *
from openhpi_baselib.Ohpi import *
from openhpi_baselib.OhpiDataTypes import DEFAULT_PORT
from openhpi_baselib import HpiUtil

version = oHpiVersionGet()
print "OpenHPI baselib package version: %0x" % version

host = HpiUtil.toSaHpiTextBufferT( "localhost" )
root = HpiUtil.makeRootSaHpiEntityPathT()
( rv, did ) = oHpiDomainAdd( host, DEFAULT_PORT, root )
if rv != SA_OK:
    print "ERROR: oHpiDomainAdd: %s " % HpiUtil.fromSaErrorT( rv )
    exit()

print "DID %u" % did

( rv, sid ) = saHpiSessionOpen( did, None )
if rv != SA_OK:
    print "ERROR: saHpiSessionOpen: %s " % HpiUtil.fromSaErrorT( rv )
    exit()

print "SID %u" % sid

( rv, my_ep )  = saHpiMyEntityPathGet( sid )
if rv == SA_OK:
    print "My entity: %s" % HpiUtil.fromSaHpiEntityPathT( my_ep )

print "Resource List:"

eid = SAHPI_FIRST_ENTRY
next_eid = SAHPI_FIRST_ENTRY
while eid != SAHPI_LAST_ENTRY:
    ( rv, next_eid, rpte ) = saHpiRptEntryGet( sid, eid )
    if ( eid == SAHPI_FIRST_ENTRY ) and ( rv == SA_ERR_HPI_NOT_PRESENT ):
        break 
    if rv != SA_OK:
        print "ERROR: saHpiRptEntryGet: %s " % HpiUtil.fromSaErrorT( rv )
        exit()
    tag = HpiUtil.fromSaHpiTextBufferT( rpte.ResourceTag )
    ep = HpiUtil.fromSaHpiEntityPathT( rpte.ResourceEntity )
    print "  HPI Resource %u: %s: %s" % ( rpte.ResourceId, tag, ep )
    eid = next_eid

rv = saHpiSessionClose( sid )
if rv != SA_OK:
    print "ERROR: saHpiSessionClose: %s " % HpiUtil.fromSaErrorT( rv )
    exit()

print "End"

