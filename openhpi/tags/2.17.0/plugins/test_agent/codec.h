/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTCODECLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef CODEC_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define CODEC_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>


namespace TA {


/**************************************************************
 * Codec Interface
 *************************************************************/
struct Var;

// Data Codecs
void ToTxt( const Var& var, std::string& txt );
bool FromTxt( const std::string& txt, Var& var );


// Object Name Codecs
std::string AssembleNumberedObjectName( const std::string& classname,
                                        SaHpiUint32T num );
bool DisassembleNumberedObjectName( const std::string& name,
                                    std::string& classname,
                                    SaHpiUint32T& num );

std::string AssembleResourceObjectName( const SaHpiEntityPathT& ep );
bool DisassembleResourceObjectName( const std::string& name,
                                    SaHpiEntityPathT& ep );


}; // namespace TA


#endif // CODEC_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

