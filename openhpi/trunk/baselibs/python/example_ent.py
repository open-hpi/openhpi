#!/usr/bin/python

import sys
from openhpi_baselib import *

if len( sys.argv ) != 2:
    print "Usage: %s <entity_path>" % sys.argv[0]
    exit()

try:
    parent = HpiUtil.toSaHpiEntityPathT( sys.argv[1] )
except ValueError:
    print "Invalid entity path: %s" % sys.argv[1]
    exit()

( rv, sid ) = saHpiSessionOpen( SAHPI_UNSPECIFIED_DOMAIN_ID, None )
if rv != SA_OK:
    print "ERROR: saHpiSessionOpen: %s " % HpiUtil.fromSaErrorT( rv )
    exit()

print "Entity %s" % HpiUtil.fromSaHpiEntityPathT( parent )

for child in HpiIterators.ChildEntities( sid, parent ):
    print " Child Entity %s" % HpiUtil.fromSaHpiEntityPathT( child )

for rid in HpiIterators.EntityResources( sid, parent ):
    print " Resource %d" % rid

for rdrtype in [ SAHPI_CTRL_RDR,
                 SAHPI_SENSOR_RDR,
                 SAHPI_INVENTORY_RDR,
                 SAHPI_WATCHDOG_RDR,
                 SAHPI_ANNUNCIATOR_RDR,
                 SAHPI_DIMI_RDR,
                 SAHPI_FUMI_RDR ]:
    for ( rid, instrid ) in HpiIterators.EntityInstruments( sid, parent, rdrtype ):
        print " Resource %d Instrument %d (%s)" % ( rid, instrid, HpiUtil.fromSaHpiRdrTypeT( rdrtype ) )

rv = saHpiSessionClose( sid )
if rv != SA_OK:
    print "ERROR: saHpiSessionClose: %s " % HpiUtil.fromSaErrorT( rv )

