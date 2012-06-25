/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTWATCHDOGLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <algorithm>
#include <string>

#include "codec.h"
#include "dimi.h"
#include "test.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultDimiRec( SaHpiDimiNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiDimiRecT& rec = data.DimiRec;

    rec.DimiNum = num;
    rec.Oem     = 0;

    return data;
}


/**************************************************************
 * Functors for working with tests
 *************************************************************/
struct TestDeleter
{
    void operator ()( cTest * test )
    {
        delete test;
    }
};

struct ObjectCollector
{
    explicit ObjectCollector( cObject::Children& _children )
        : children( _children )
    {
        // empty
    }

    void operator ()( cTest * test )
    {
        if ( test ) {
            children.push_back( test );
        }
    }

    cObject::Children& children;
};


/**************************************************************
 * class cDimi
 *************************************************************/
const std::string cDimi::classname( "dimi" );

cDimi::cDimi( cHandler& handler, cResource& resource, SaHpiDimiNumT num )
    : cInstrument( handler,
                   resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_DIMI_RDR,
                   MakeDefaultDimiRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.DimiRec ),
      m_update_count( 0 )
{
    // empty
}

cDimi::~cDimi()
{
    TestDeleter deleter;
    std::for_each( m_tests.begin(), m_tests.end(), deleter );
    m_tests.clear();
}


cTest * cDimi::GetTest( SaHpiDimiTestNumT tnum ) const
{
    return ( tnum < m_tests.size() ) ? m_tests[tnum] : 0;
}

void cDimi::PostEvent( SaHpiDimiTestNumT tnum,
                       SaHpiDimiTestRunStatusT status,
                       SaHpiDimiTestPercentCompletedT progress )
{
    SaHpiEventUnionT data;
    SaHpiDimiEventT& de = data.DimiEvent;

    de.DimiNum                  = m_rec.DimiNum;
    de.TestNum                  = tnum;
    de.DimiTestRunStatus        = status;
    de.DimiTestPercentCompleted = progress;

    cInstrument::PostEvent( SAHPI_ET_DIMI, data, SAHPI_INFORMATIONAL );
}


// HPI interface
SaErrorT cDimi::GetInfo( SaHpiDimiInfoT& info ) const
{
    info.NumberOfTests        = m_tests.size();
    info.TestNumUpdateCounter = m_update_count;

    return SA_OK;
}


// cObject virtual functions
void cDimi::GetNB( std::string& nb ) const
{
    cObject::GetNB( nb );
    nb += "- Test Agent supports creation of a DIMI test with\n";
    nb += "    id == current number of tests.\n";
    nb += "- Test Agent supports removal of a DIMI test with\n";
    nb += "    id == (current number of tests - 1).\n";
    nb += "- Be careful when removing a test:\n";
    nb += "-- Any DIMI API directed to the removed test will fail.\n";
    nb += "-- Any DIMI asynchronous operation on the test can fail or cause crash.\n";
}

void cDimi::GetNewNames( cObject::NewNames& names ) const
{
    cObject::GetNewNames( names );
    names.push_back( cTest::classname + "-XXX" );
}

bool cDimi::CreateChild( const std::string& name )
{
    bool rc;

    rc = cObject::CreateChild( name );
    if ( rc ) {
        return true;
    }

    std::string cname;
    SaHpiUint32T id;
    rc = DisassembleNumberedObjectName( name, cname, id );
    if ( !rc ) {
        return false;
    }

    if ( cname != cTest::classname ) {
        return false;
    }

    if ( id != m_tests.size() ) {
        return false;
    }

    m_tests.push_back( new cTest( m_handler, *this, id ) );
    Update();

    return true;
}

bool cDimi::RemoveChild( const std::string& name )
{
    bool rc;

    rc = cObject::RemoveChild( name );
    if ( rc ) {
        return true;
    }

    std::string cname;
    SaHpiUint32T id;
    rc = DisassembleNumberedObjectName( name, cname, id );
    if ( !rc ) {
        return false;
    }

    if ( ( id + 1 ) != m_tests.size() ) {
        return false;
    }

    delete m_tests[id];
    m_tests[id] = 0;
    m_tests.resize( id );
    Update();

    return true;
}

void cDimi::GetChildren( Children& children ) const
{
    cObject::GetChildren( children );

    ObjectCollector collector( children );
    std::for_each( m_tests.begin(), m_tests.end(), collector );
}


void cDimi::Update()
{
    ++m_update_count;

    SaHpiEventUnionT data;
    SaHpiDimiUpdateEventT& due = data.DimiUpdateEvent;

    due.DimiNum = m_rec.DimiNum;

    cInstrument::PostEvent( SAHPI_ET_DIMI_UPDATE, data, SAHPI_INFORMATIONAL );
}


}; // namespace TA

