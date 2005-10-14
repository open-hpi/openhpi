/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *    W. David Ashley <dashley@us.ibm.com>
 */


#ifndef Included_oSaHpiTypesEnums
#define Included_oSaHpiTypesEnums

extern "C"
{
#include <SaHpi.h>
}


class oSaHpiTypesEnums {
    public:
        // all the real methods in this class are static
        // so they can be used from any other class
        static SaHpiLanguageT str2language(const char *strtype);
        static const char * language2str(SaHpiLanguageT value);
        static SaHpiTextTypeT str2texttype(const char *type);
        static const char * texttype2str(SaHpiTextTypeT value);
        static SaHpiEntityTypeT str2entitytype(const char *strtype);
        static const char * entitytype2str(SaHpiEntityTypeT value);
        static SaHpiSensorReadingTypeT str2sensorreadingtype(const char *strtype);
        static const char * sensorreadingtype2str(SaHpiSensorReadingTypeT value);
        static SaHpiSensorUnitsT str2sensorunits(const char *strtype);
        static const char * sensorunits2str(SaHpiSensorUnitsT value);
        static SaHpiSensorModUnitUseT str2sensoruse(const char *strtype);
        static const char * sensoruse2str(SaHpiSensorModUnitUseT value);
};

#endif

