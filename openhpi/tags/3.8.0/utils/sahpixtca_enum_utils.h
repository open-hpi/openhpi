/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com> 
 *      Anton Pak <anton.pak@pigeonpoint.com> 
 */

#ifndef __SAHPIXTCA_ENUM_UTILS_H
#define __SAHPIXTCA_ENUM_UTILS_H

#ifndef __OH_UTILS_H
#warning *** Include oh_utils.h instead of individual utility header files ***
#endif

#ifdef __cplusplus
extern "C" {
#endif 

#define OH_MAX_XTCAHPILEDCOLOR 9 
extern struct oh_xtcahpiledcolor_map {
  XtcaHpiLedColorT  entity_type;
  char *str;
} xtcahpiledcolor_strings[OH_MAX_XTCAHPILEDCOLOR];

char * oh_lookup_xtcahpiledcolor(XtcaHpiLedColorT value);
SaErrorT oh_encode_xtcahpiledcolor(SaHpiTextBufferT *buffer, XtcaHpiLedColorT *type);

#define OH_MAX_XTCAHPIRESOURCELEDMODE 3 
extern struct oh_xtcahpiresourceledmode_map {
  XtcaHpiResourceLedModeT  entity_type;
  char *str;
} xtcahpiresourceledmode_strings[OH_MAX_XTCAHPIRESOURCELEDMODE];

char * oh_lookup_xtcahpiresourceledmode(XtcaHpiResourceLedModeT value);
SaErrorT oh_encode_xtcahpiresourceledmode(SaHpiTextBufferT *buffer, XtcaHpiResourceLedModeT *type);

#define OH_MAX_XTCAHPILEDBRSUPPORT 3 
extern struct oh_xtcahpiledbrsupport_map {
  XtcaHpiLedBrSupportT  entity_type;
  char *str;
} xtcahpiledbrsupport_strings[OH_MAX_XTCAHPILEDBRSUPPORT];

char * oh_lookup_xtcahpiledbrsupport(XtcaHpiLedBrSupportT value);
SaErrorT oh_encode_xtcahpiledbrsupport(SaHpiTextBufferT *buffer, XtcaHpiLedBrSupportT *type);

#define OH_MAX_XTCAHPIENTITYTYPE 12 
extern struct oh_xtcahpientitytype_map {
  SaHpiEntityTypeT  entity_type;
  char *str;
} xtcahpientitytype_strings[OH_MAX_XTCAHPIENTITYTYPE];

char * oh_lookup_xtcahpientitytype(SaHpiEntityTypeT value);
SaErrorT oh_encode_xtcahpientitytype(SaHpiTextBufferT *buffer, SaHpiEntityTypeT *type);


#ifdef __cplusplus
}
#endif

#endif
