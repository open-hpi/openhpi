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

#ifndef SCHEMA_H_EC5AF80F_A79B_49D7_8371_F71504C426A6
#define SCHEMA_H_EC5AF80F_A79B_49D7_8371_F71504C426A6


extern "C"
{

#ifdef WIN32

extern char binary_schema_xsd_start;
extern char binary_schema_xsd_end;
#define SCHEMA_START (&binary_schema_xsd_start)
#define SCHEMA_END   (&binary_schema_xsd_end)

#else

extern char _binary_schema_xsd_start;
extern char _binary_schema_xsd_end;
#define SCHEMA_START (&_binary_schema_xsd_start)
#define SCHEMA_END   (&_binary_schema_xsd_end)

#endif

};


#endif /* SCHEMA_H_EC5AF80F_A79B_49D7_8371_F71504C426A6 */

