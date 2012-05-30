#!/usr/bin/python

from openhpi_baselib import *

( rv, sid ) = saHpiSessionOpen( SAHPI_UNSPECIFIED_DOMAIN_ID, None )
if rv != SA_OK:
    print "ERROR: saHpiSessionOpen: %s " % HpiUtil.fromSaErrorT( rv )
    exit()

# List all handlers
last_hid = None
for hid in OhpiIterators.Handlers( sid ):
    last_hid = hid
    print "Handler %d" % hid
    ( rv, hinfo, hconf ) = oHpiHandlerInfo( sid, hid )
    if rv != SA_OK:
        print "ERROR: oHpiHandlerInfo: %s " % HpiUtil.fromSaErrorT( rv )
    print " Info"
    print "  id %d" % hinfo.id
    print "  name %s" % hinfo.plugin_name
    print "  entity_root %s" % HpiUtil.fromSaHpiEntityPathT( hinfo.entity_root )
    print "  load_failed %d" % hinfo.load_failed
    d = OhpiUtil.fromoHpiHandlerConfigT( hconf )
    print " Config"
    print "\n".join( [ "  %s = %s" % ( name, value ) for ( name, value ) in d.iteritems() ] )

# Retry last handler
if last_hid != SAHPI_FIRST_ENTRY:
    print "Re-trying last handler: %d" % last_hid
    rv = oHpiHandlerRetry( sid, last_hid )
    if rv != SA_OK:
        print "ERROR: oHpiHandlerRetry: %s " % HpiUtil.fromSaErrorT( rv )

# Destroy last handler
if last_hid != SAHPI_FIRST_ENTRY:
    print "Destroying last handler: %d" % last_hid
    rv = oHpiHandlerDestroy( sid, last_hid )
    if rv != SA_OK:
        print "ERROR: oHpiHandlerDestroy: %s " % HpiUtil.fromSaErrorT( rv )

# Look for handler providing specified resource
rid = 120
( rv, hid ) = oHpiHandlerFind( sid, rid )
if rv != SA_OK:
    print "ERROR: oHpiHandlerFind: %s " % HpiUtil.fromSaErrorT( rv )
if hid is not None:
    print "Resource %s is provided by handler %d" % ( rid, hid )

# Create new instance of test_agent plugin
print "Creating new handler"
d = { "plugin" : "libtest_agent", "port" : "9999" }
hconf = OhpiUtil.tooHpiHandlerConfigT( d )
( rv, hid ) = oHpiHandlerCreate( sid, hconf )
if rv == SA_OK:
    print "Created handler %d" % hid
else:
    print "ERROR: oHpiHandlerCreate: %s " % HpiUtil.fromSaErrorT( rv )

rv = saHpiSessionClose( sid )
if rv != SA_OK:
    print "ERROR: saHpiSessionClose: %s " % HpiUtil.fromSaErrorT( rv )

