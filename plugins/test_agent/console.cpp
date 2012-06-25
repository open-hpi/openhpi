/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTOBJECTLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <string.h>

#include <algorithm>
#include <cctype>
#include <list>
#include <string>
#include <vector>

#include <oh_error.h>

#include "codec.h"
#include "console.h"
#include "handler.h"
#include "utils.h"


namespace TA {


/**************************************************************
 * Helper data
 *************************************************************/
static const char sepline[] = "----------------------------------------------------\n";
static const char indent1[] = "  ";
static const char indent2[] = "    ";


/**************************************************************
 * Helper functions
 *************************************************************/
static bool IsSpace( char c )
{
    return isspace( c ) != 0;
}

static bool IsNonSpace( char c )
{
    return isspace( c ) == 0;
}

static bool IsNonCommand( const std::vector<char>& line )
{
    std::vector<char>::const_iterator iter;
    iter = std::find_if( line.begin(), line.end(), IsNonSpace );
    return ( iter == line.end() ) || ( *iter == '#' );
}

/***
 * All commands can be presented in form:
 * name[ arg0[ = arg1]], where [] shows optional part.
 * This function parses input line and extracts name and args.
 **/
static void ParseLine( const std::vector<char>& line,
                       std::string& name,
                       cConsoleCmd::Args& args )
{
    name.clear();
    args.clear();

    std::vector<char>::const_iterator a, b, c;

    // name
    a = std::find_if( line.begin(), line.end(), IsNonSpace );
    b = std::find_if( a, line.end(), IsSpace );
    name.assign( a, b );

    // arg0
    a = std::find_if( b, line.end(), IsNonSpace );
    if ( a == line.end() ) {
        return;
    }
    b = std::find_if( a, line.end(), IsSpace );
    std::string arg0( a, b );
    args.push_back( arg0 );

    // =
    b = std::find( b, line.end(), '=' );
    if ( b == line.end() ) {
        return;
    }
    ++b;

    // arg1
    a = std::find_if( b, line.end(), IsNonSpace );
    if ( a == line.end() ) {
        return;
    }
    // arg1: do not copy trailing spaces
    c = a;
    do {
        b = std::find_if( c, line.end(), IsSpace );
        c = std::find_if( b, line.end(), IsNonSpace );
    } while ( c != line.end() );
    std::string arg1( a, b );
    args.push_back( arg1 );
}


/**************************************************************
 * class cConsole
 *************************************************************/
cConsole::cConsole( cHandler& handler, uint16_t port, cObject& root )
    : cServer( port ),
      m_handler( handler ),
      m_root( root )
{
    // empty
}

cConsole::~cConsole()
{
    // empty
}

bool cConsole::Init()
{
    m_cmds.push_back( cConsoleCmd( "help",
                              "help",
                              "Prints this help message.",
                              &cConsole::CmdHelp,
                              0 ) );
    m_cmds.push_back( cConsoleCmd( "quit",
                              "quit",
                              "Quits from the console.",
                              &cConsole::CmdQuit,
                              0 ) );
    m_cmds.push_back( cConsoleCmd( "ls",
                              "ls",
                              "Shows current object.",
                              &cConsole::CmdLs,
                              0 ) );
    m_cmds.push_back( cConsoleCmd( "cd",
                              "cd <objname|objpath>",
                              "Enters to the specified object.",
                              &cConsole::CmdCd,
                              1 ) );
    m_cmds.push_back( cConsoleCmd( "new",
                              "new <objname>",
                              "Creates new child object.",
                              &cConsole::CmdNew,
                              1 ) );
    m_cmds.push_back( cConsoleCmd( "rm",
                              "rm <objname>",
                              "Deletes the specified child object.",
                              &cConsole::CmdRm,
                              1 ) );
    m_cmds.push_back( cConsoleCmd( "set",
                              "set <var> = <val>",
                              "Sets the specified variable in the current object.",
                              &cConsole::CmdSet,
                              2 ) );

    bool rc = cServer::Init();
    if ( !rc ) {
        CRIT( "cannot initialize Server" );
        return false;
    }

    return true;
}

void cConsole::Send( const char * data, size_t len ) const
{
    cServer::Send( data, len );
}

void cConsole::Send( const char * str ) const
{
    if ( str ) {
        cServer::Send( str, strlen( str ) ) ;
    }
}

void cConsole::Send( const std::string& str ) const
{
    Send( str.data(), str.length() );
}

void cConsole::WelcomeUser() const
{
    Send( sepline, sizeof(sepline) );
    Send( "Welcome to Test Agent Console.\n" );
    Send( "Type \"help\" for command information.\n" );
    Send( sepline, sizeof(sepline) );
}

void cConsole::ProcessUserLine( const std::vector<char>& line, bool& quit )
{
    m_quit = false;

    bool rc = IsNonCommand( line );
    if ( rc ) {
        return;
    }

    std::string name;
    cConsoleCmd::Args args;
    ParseLine( line, name, args );

    if ( name.empty() ) {
        return;
    }

    cLocker<cHandler> al( &m_handler );

    for ( size_t i = 0, n = m_cmds.size(); i < n; ++i ) {
        const cConsoleCmd& cmd = m_cmds[i];
        if ( name == cmd.name ) {
            if ( args.size() == cmd.nargs ) {
                (this->*(cmd.cmd_handler))( args );
                quit = m_quit;
                return;
            } else {
                SendERR( "Wrong number of parameters." );
                return;
            }
        }
    }
    SendERR( "Unknown command." );
}

void cConsole::CmdHelp( const cConsoleCmd::Args& args )
{
    Send( sepline, sizeof(sepline) );
    Send( "Supported commands:\n" );
    for ( size_t i = 0, n = m_cmds.size(); i < n; ++i ) {
        const cConsoleCmd& cmd = m_cmds[i];
        Send( indent1 );
        Send( cmd.usage );
        Send( "\n" );
        Send( indent2 );
        Send( cmd.info );
        Send( "\n" );
    }
    Send( "\n" );
    Send( "If input line begins with #, it will be ignored.\n" );
    Send( "\n" );
    SendOK( "Help displayed." );
}

void cConsole::CmdQuit( const cConsoleCmd::Args& args )
{
    m_quit = true;
    SendOK( "Quit." );
}

void cConsole::CmdLs( const cConsoleCmd::Args& args )
{
    cObject * current = TestAndGetCurrentObject();
    if ( !current ) {
        return;
    }

    Send( sepline, sizeof(sepline) );
    Send( "Current object: " );
    SendCurrentPath();
    Send( "\n" );

    Send( indent1 );
    Send( "Targets for cd/rm:\n" );
    cObject::Children children;
    cObject::Children::const_iterator ci, cend;
    current->GetChildren( children );
    for ( ci = children.begin(), cend  = children.end(); ci != cend; ++ci ) {
        Send( indent2 );
        Send( (*ci)->GetName() );
        Send( "\n" );
    }

    Send( indent1 );
    Send( "Targets for new:\n" );
    cObject::NewNames nnames;
    cObject::NewNames::const_iterator ni, nend;
    current->GetNewNames( nnames );
    for ( ni = nnames.begin(), nend  = nnames.end(); ni != nend; ++ni ) {
        Send( indent2 );
        Send( *ni );
        Send( "\n" );
    }

    Send( indent1 );
    Send( "Vars:\n" );
    cVars vars;
    VarIter vi, vend;
    current->GetVars( vars );
    for ( vi = vars.begin(), vend  = vars.end(); vi != vend; ++vi ) {
        Send( indent2 );
        if ( vi->wdata ) {
            Send( "RW " );
        } else {
            Send( "RO " );
        }
        Send( vi->name );
        std::string data;
        ToTxt( *vi, data );
        Send( " = " );
        Send( data );
        Send( "\n" );
    }

    SendOK( "Object displayed." );
}

void cConsole::CmdCd( const cConsoleCmd::Args& args )
{
    const std::string& name = args[0];

    ObjectPath new_path;
    MakeNewPath( new_path, name );

    cObject * obj = GetObject( new_path );
    if ( !obj ) {
        obj = TestAndGetCurrentObject();
        SendERR( "No object." );
        return;
    }

    m_path = new_path;

    Send( sepline, sizeof(sepline) );
    Send( "Current object: " );
    SendCurrentPath();
    Send( "\n" );

    std::string nb;
    obj->GetNB( nb );
    if ( !nb.empty() ) {
        Send( sepline, sizeof(sepline) );
        Send( "NB!:\n\n" );
        Send( nb );
    }

    SendOK( "Object changed." );
}

void cConsole::CmdNew( const cConsoleCmd::Args& args )
{
    cObject * current = TestAndGetCurrentObject();
    if ( !current ) {
        return;
    }

    const std::string& name = args[0];

    cObject * child = current->GetChild( name );
    if ( child ) {
        SendERR( "Object already exists." );
        return;
    }

    bool rc = current->CreateChild( name );
    if ( !rc ) {
        SendERR( "Failed to create object." );
        return;
    }

    SendOK( "Object created." );
}

void cConsole::CmdRm( const cConsoleCmd::Args& args )
{
    cObject * current = TestAndGetCurrentObject();
    if ( !current ) {
        return;
    }

    const std::string& name = args[0];

    cObject * child = current->GetChild( name );
    if ( !child ) {
        SendERR( "No such child object." );
        return;
    }

    bool rc = current->RemoveChild( name );
    if ( !rc ) {
        SendERR( "Failed to remove object." );
        return;
    }

    SendOK( "Object removed." );
}

void cConsole::CmdSet( const cConsoleCmd::Args& args )
{
    bool rc;

    cObject * current = TestAndGetCurrentObject();
    if ( !current ) {
        return;
    }

    const std::string& var_name = args[0];
    Var var;
    rc = current->GetVar( var_name, var );
    if ( !rc ) {
        SendERR( "No such var." );
        return;
    }
    if ( !var.wdata ) {
        SendERR( "Read-only var." );
        return;
    }

    const std::string& data = args[1];
    current->BeforeVarSet( var_name );
    rc = FromTxt( data, var );
    if ( !rc ) {
        SendERR( "Cannot decode data." );
        return;
    }
    current->AfterVarSet( var_name );

    SendOK( "Var set." );
}

void cConsole::SendOK( const std::string& msg )
{
    Send( sepline, sizeof(sepline) );
    Send( "OK: " );
    Send( msg );
    Send( "\n" );
    Send( sepline, sizeof(sepline) );
}

void cConsole::SendERR( const std::string& msg )
{
    Send( sepline, sizeof(sepline) );
    Send( "ERR: " );
    Send( msg );
    Send( "\n" );
    Send( sepline, sizeof(sepline) );
}

void cConsole::SendCurrentPath() const
{
    if ( m_path.empty() ) {
        Send( "/" );
    } else {
        ObjectPath::const_iterator iter = m_path.begin();
        ObjectPath::const_iterator end  = m_path.end();
        for ( ; iter != end; ++iter ) {
            Send( "/" );
            Send( *iter );
        }
    }
}

cObject * cConsole::GetObject( const ObjectPath& path ) const
{
    cObject * obj = &m_root;

    ObjectPath::const_iterator iter = path.begin();
    ObjectPath::const_iterator end  = path.end();
    for ( ; iter != end; ++iter ) {
        cObject * child = obj->GetChild( *iter );
        if ( !child ) {
            obj = 0;
            break;
        }
        obj = child;
    }

    return obj;
}

cObject * cConsole::TestAndGetCurrentObject()
{
    cObject * current = GetObject( m_path );
    if ( current == 0 ) {
        SendERR( "Current object is no longer exists." );
        while ( ( current == 0 ) && ( !m_path.empty() ) ) {
            m_path.pop_back();
            current = GetObject( m_path );
        }
        Send( "New current object: " );
        SendCurrentPath();
        Send( "\n" );
        current = 0;
    }

    if ( current == 0 ) {
        SendERR( "No object." );
    }

    return current;
}

void cConsole::MakeNewPath( ObjectPath& path,
                            const std::string& path_str ) const
{
    std::vector<char> buf( path_str.begin(), path_str.end() );
    buf.push_back( '\0' );

    ObjectPath tmp;
    if ( buf[0] != '/' ) {
        tmp = m_path;
    }

    char * token = strtok( &buf[0], "/" );
    while( token ) {
        std::string stoken( token );
        if ( ( !stoken.empty() ) && ( stoken != "." ) ) {
            tmp.push_back( std::string( token ) );
        }
        token = strtok( 0, "/" );
    }

    path.clear();
    while( !tmp.empty() ) {
        if ( tmp.front() != ".." ) {
            path.push_back( tmp.front() );
        } else {
            if ( !path.empty() ) {
                path.pop_back();
            }
        }
        tmp.pop_front();
    }
}

}; // namespace TA

