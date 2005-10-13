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


#ifndef Included_oSaHpiTextBuffer
#define Included_oSaHpiTextBuffer

#include <stdio.h>
extern "C"
{
#include <SaHpi.h>
}


#define SAHPILANGUAGET_DEFAULT SAHPI_LANG_ENGLISH
#define SAHPITEXTTYPET_DEFAULT SAHPI_TL_TYPE_TEXT


class oSaHpiTextBuffer : public SaHpiTextBufferT {
    public:
        // constructors
        oSaHpiTextBuffer();
        oSaHpiTextBuffer(const SaHpiTextTypeT type,
                         const SaHpiLanguageT lang);
        oSaHpiTextBuffer(const SaHpiTextTypeT type,
                         const SaHpiLanguageT lang,
                         const char *str);
        oSaHpiTextBuffer(const SaHpiTextTypeT type,
                         const SaHpiLanguageT lang,
                         const void *str,
                         const SaHpiUint8T len);
        // copy constructor
        oSaHpiTextBuffer(const oSaHpiTextBuffer& buf);
        // destructor
        ~oSaHpiTextBuffer();
        // other methods
        bool append(const char *str);
        bool append(SaHpiTextBufferT *ptr,
                    const char *str);
        bool append(const void *str,
                    const SaHpiUint8T len);
        bool append(SaHpiTextBufferT *ptr,
                    const void *str,
                    const SaHpiUint8T len);
        bool assignField(const char *field,
                         const char *value);
        bool assignField(SaHpiTextBufferT * ptr,
                         const char *field,
                         const char *value);
        inline SaHpiTextBufferT *getStruct(void) {
            return this;
        }
        bool fprint(FILE *stream,
                    const int indent,
                    const SaHpiTextBufferT *buffer);
        inline bool fprint(FILE *stream,
                           const int indent) {
            return fprint(stream, indent, this);
        }

    protected:
        const char * language2str(SaHpiLanguageT value);
        SaHpiLanguageT str2language(const char *strtype);
        const char * texttype2str(SaHpiTextTypeT value);
        SaHpiTextTypeT str2texttype(const char *type);
};

#endif

