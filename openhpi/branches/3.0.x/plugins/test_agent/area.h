/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTAREALITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef AREA_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define AREA_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>
#include <string>

#include <SaHpi.h>

#include "object.h"


namespace TA {


/**************************************************************
 * class cArea
 *************************************************************/
class cField;

class cArea : public cObject
{
public:

    static const std::string classname;

    explicit cArea( volatile SaHpiUint32T& update_count,
                    SaHpiEntryIdT id,
                    SaHpiIdrAreaTypeT type = SAHPI_IDR_AREATYPE_OEM );
    virtual ~cArea();

public:

    SaHpiEntryIdT GetId() const
    {
        return m_id;
    }

    SaHpiIdrAreaTypeT GetType() const
    {
        return m_type;
    }

    void GetHeader( SaHpiIdrAreaHeaderT& hdr ) const;

    bool CanBeDeleted() const;

    cField * GetField( SaHpiEntryIdT fid ) const;

public: // HPI interface

    SaErrorT GetField( SaHpiIdrFieldTypeT ftype,
                       SaHpiEntryIdT fid,
                       SaHpiEntryIdT& next_fid,
                       SaHpiIdrFieldT& field ) const;
    SaErrorT AddField( SaHpiIdrFieldTypeT ftype,
                       const SaHpiTextBufferT& fdata,
                       SaHpiEntryIdT& fid );
    SaErrorT AddFieldById( SaHpiEntryIdT fid,
                           SaHpiIdrFieldTypeT ftype,
                           const SaHpiTextBufferT& fdata );
    SaErrorT SetField( SaHpiEntryIdT fid,
                       SaHpiIdrFieldTypeT ftype,
                       const SaHpiTextBufferT& fdata );
    SaErrorT DeleteFieldById( SaHpiEntryIdT fid );

protected: // cObject virtual functions

    virtual void GetNewNames( cObject::NewNames& names ) const;
    virtual bool CreateChild( const std::string& name );
    virtual bool RemoveChild( const std::string& name );
    virtual void GetChildren( cObject::Children& children ) const;
    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cArea( const cArea& );
    cArea& operator =( const cArea& );

private: // data

    SaHpiEntryIdT          m_id;
    SaHpiIdrAreaTypeT      m_type;
    SaHpiBoolT             m_readonly;
    volatile SaHpiUint32T& m_update_count;

    typedef std::list<cField *> Fields;
    Fields m_fields;
};


}; // namespace TA


#endif // AREA_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

