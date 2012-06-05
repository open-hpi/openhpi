/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTHANDLERLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef HANDLER_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define HANDLER_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <stdint.h>

#include <map>

#include <glib.h>

#include <SaHpi.h>

#include <oh_utils.h>

#include "console.h"
#include "instrument.h"
#include "object.h"
#include "timers.h"


namespace TA {


/**************************************************************
 * class cHandler
 *************************************************************/
class cResource;

class cHandler : public cTimers, private cObject, private cConsole
{
public:

    explicit cHandler( unsigned int id,
                       unsigned short port,
                       oh_evt_queue& eventq );

    ~cHandler();

    bool Init();
    void Lock();
    void Unlock();

public:

    cResource * GetResource( SaHpiResourceIdT rid ) const;

public:  // HPI interface

    SaErrorT RemoveFailedResource( SaHpiResourceIdT rid );
    SaHpiTimeoutT GetAutoInsertTimeout() const;
    SaErrorT SetAutoInsertTimeout( SaHpiTimeoutT t );

public: // Event generation

    void PostEvent( SaHpiEventTypeT type,
                    const SaHpiEventUnionT& data,
                    SaHpiSeverityT severity,
                    const cResource * r,
                    const InstrumentList& updates,
                    const InstrumentList& removals ) const;

private:

    cHandler( const cHandler& );
    cHandler& operator =( const cHandler& );

private: // cObject virtual functions

    virtual void GetNewNames( cObject::NewNames& names ) const;
    virtual bool CreateChild( const std::string& name );
    virtual bool RemoveChild( const std::string& name );
    virtual void GetChildren( cObject::Children& children ) const;
    virtual void GetVars( cVars& vars );

private: // data

    typedef std::map<SaHpiResourceIdT, cResource *> Resources;

    unsigned int  m_id;
    oh_evt_queue& m_eventq;
    GStaticMutex  m_lock;
    Resources     m_resources;
    SaHpiTimeoutT m_ai_timeout;
};


}; // namespace TA


#endif // HANDLER_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

