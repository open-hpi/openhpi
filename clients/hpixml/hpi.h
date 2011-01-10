/*      -*- c++ -*-
 *
 * Copyright (C) 2010, Pigeon Point Systems
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef HPI_H_EC5AF80F_A79B_49D7_8371_F71504C426A6
#define HPI_H_EC5AF80F_A79B_49D7_8371_F71504C426A6

#include <SaHpi.h>


class cHpiXmlWriter;

/***************************************************
 * class cHpi
 ***************************************************/
class cHpi
{
public:

    explicit cHpi( SaHpiDomainIdT did );
    ~cHpi();

    bool Open();
    void Close();

    bool Dump( cHpiXmlWriter& writer );

private:

    cHpi( const cHpi& );
    cHpi& operator =( const cHpi& );

private:

    bool            m_initialized;
    bool            m_opened;
    SaHpiDomainIdT  m_did;
    SaHpiSessionIdT m_sid;
};


#endif /* HPI_H_EC5AF80F_A79B_49D7_8371_F71504C426A6 */

