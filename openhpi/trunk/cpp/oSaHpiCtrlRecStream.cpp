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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern "C"
{
#include <SaHpi.h>
}
#include "oSaHpiTypesEnums.hpp"
#include "oSaHpiCtrlStateStream.hpp"
#include "oSaHpiCtrlRecStream.hpp"


/**
 * Default constructor.
 */
oSaHpiCtrlRecStream::oSaHpiCtrlRecStream() {
    Default.Repeat = false;
    Default.StreamLength = 0;
    Default.Stream[0] = '\0';
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiCtrlRecStream::oSaHpiCtrlRecStream(const oSaHpiCtrlRecStream& ent) {
    memcpy(this, &ent, sizeof(SaHpiCtrlRecStreamT));
}

