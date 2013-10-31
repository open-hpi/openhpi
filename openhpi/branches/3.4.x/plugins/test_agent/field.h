/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTFIELDLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef FIELD_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define FIELD_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "object.h"


namespace TA {


/**************************************************************
 * class cField
 *************************************************************/
class cField : public cObject
{
public:

    static const std::string classname;

    explicit cField( volatile SaHpiUint32T& update_count, SaHpiEntryIdT id );
    virtual ~cField();

public:

    SaHpiEntryIdT GetId() const
    {
        return m_id;
    }

    SaHpiIdrFieldTypeT GetType() const
    {
        return m_type;
    }

    bool IsReadOnly() const
    {
        return m_readonly != SAHPI_FALSE;
    }

    void Get( SaHpiEntryIdT& id,
              SaHpiIdrFieldTypeT& type,
              SaHpiBoolT& readonly,
              SaHpiTextBufferT& data ) const;

    void Set( SaHpiIdrFieldTypeT type, const SaHpiTextBufferT& data );

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cField( const cField& );
    cField& operator =( const cField& );

private: // data

    SaHpiEntryIdT          m_id;
    SaHpiIdrFieldTypeT     m_type;
    SaHpiBoolT             m_readonly;
    SaHpiTextBufferT       m_data;
    volatile SaHpiUint32T& m_update_count;
};


}; // namespace TA


#endif // FIELD_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

