/*      -*- c++ -*-
 *
 * Copyright (C) 2011, Pigeon Point Systems
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
#include <oh_clients.h>


class cHpiXmlWriter;

/***************************************************
 * class cHpi
 ***************************************************/
class cHpi
{
public:

    explicit cHpi( oHpiCommonOptionsT copt );
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
    oHpiCommonOptionsT m_copt;
    SaHpiSessionIdT m_sid;
};


#endif /* HPI_H_EC5AF80F_A79B_49D7_8371_F71504C426A6 */

