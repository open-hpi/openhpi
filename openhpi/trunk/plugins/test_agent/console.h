/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTConsoleLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef CONSOLE_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define CONSOLE_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <stddef.h>

#include <list>
#include <string>
#include <vector>

#include "object.h"
#include "server.h"


namespace TA {


/**************************************************************
 * class cConsoleCmd
 *************************************************************/
class cConsole;

struct cConsoleCmd
{
    typedef std::vector<std::string> Args;
    typedef void ( cConsole::*CmdHandler)( const Args& args );

    explicit cConsoleCmd( const std::string& _name,
                     const std::string& _usage,
                     const std::string& _info,
                     CmdHandler _cmd_handler,
                     size_t _nargs )
        : name( _name ),
          usage( _usage ),
          info( _info ),
          cmd_handler( _cmd_handler ),
          nargs( _nargs )
    {
        // empty
    }

    std::string name;
    std::string usage;
    std::string info;
    CmdHandler  cmd_handler;
    size_t      nargs;
};


/**************************************************************
 * class cConsole
 *************************************************************/
class cHandler;
typedef std::list<std::string> ObjectPath;

class cConsole : private cServer
{
public:

    explicit cConsole( cHandler& handler, uint16_t port, cObject& root );
    virtual ~cConsole();

    bool Init();
    void Send( const char * data, size_t len ) const;
    void Send( const char * str ) const;
    void Send( const std::string& str ) const;

private:

    cConsole( const cConsole& );
    cConsole& operator =( const cConsole& );

private: // cServer virtual functions

    virtual void WelcomeUser() const;
    virtual void ProcessUserLine( const std::vector<char>& line,
                                  bool& quit );

private: // Console commands

    void CmdHelp( const cConsoleCmd::Args& args );
    void CmdQuit( const cConsoleCmd::Args& args );
    void CmdLs( const cConsoleCmd::Args& args );
    void CmdCd( const cConsoleCmd::Args& args );
    void CmdNew( const cConsoleCmd::Args& args );
    void CmdRm( const cConsoleCmd::Args& args );
    void CmdSet( const cConsoleCmd::Args& args );

private:

    void SendOK( const std::string& msg );
    void SendERR( const std::string& msg );
    void SendCurrentPath() const;
    cObject * GetObject( const ObjectPath& path ) const;
    cObject * TestAndGetCurrentObject();
    void MakeNewPath( ObjectPath& path, const std::string& path_str ) const;

private: // data

    cHandler&                 m_handler;
    std::vector<cConsoleCmd > m_cmds;
    bool                      m_quit;
    ObjectPath                m_path;
    cObject&                  m_root;
};


}; // namespace TA


#endif // CONSOLE_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

