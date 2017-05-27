/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTANNOUNCEMENTLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef ANNOUNCEMENT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define ANNOUNCEMENT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "object.h"


namespace TA {


/**************************************************************
 * class cAnnouncement
 *************************************************************/
class cAnnouncement : public cObject
{
public:

    static const std::string classname;

    explicit cAnnouncement( SaHpiEntryIdT id );
    explicit cAnnouncement( SaHpiEntryIdT id,
                            const SaHpiAnnouncementT& user_data );
    virtual ~cAnnouncement();

public:

    SaHpiEntryIdT GetId() const
    {
        return m_data.EntryId;
    }

    const SaHpiTimeT& GetTimestamp() const
    {
        return m_data.Timestamp;
    }

    SaHpiSeverityT GetSeverity() const
    {
        return m_data.Severity;
    }

    bool IsAcknowledged() const
    {
        return ( m_data.Acknowledged != SAHPI_FALSE );
    }

    void Acknowledge()
    {
        m_data.Acknowledged = SAHPI_TRUE;
    }

    const SaHpiAnnouncementT& GetData() const
    {
        return m_data;
    }

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );

private:

    cAnnouncement( const cAnnouncement& );
    cAnnouncement& operator =( const cAnnouncement& );

private: // data

    SaHpiAnnouncementT m_data;
};


}; // namespace TA


#endif // ANNOUNCEMENT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

