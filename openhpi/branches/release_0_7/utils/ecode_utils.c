/* -*- linux-c -*-
 *
 *
 * Copyright (c) 2003 Intel Corporation.
 * (C) Copyright IBM Corp 2003, 2004
 * Authors:  
 *     Andy Cress  arcress@users.sourceforge.net
 *  
 * 03/22/04 	pdphan@users.sourceforge.net     
 * 		Extract from files in clients to make a general utilities
 *				
 */
/*M*
Copyright (c) 2003, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

  a.. Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer. 
  b.. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution. 
  c.. Neither the name of Intel Corporation nor the names of its contributors 
      may be used to endorse or promote products derived from this software 
      without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *M*/

#include <ecode_utils.h>

struct { int code; char *str;
} ecodes[NECODES] = { 
 {  0,    "Success" },
 { -1001, "HPI unspecified error" },
 { -1002, "HPI unsupported function" },
 { -1003, "HPI busy" },
 { -1004, "HPI request invalid" },
 { -1005, "HPI command invalid" },
 { -1006, "HPI timeout" },
 { -1007, "HPI out of space" },
 { -1008, "HPI data truncated" },
 { -1009, "HPI data length invalid" },
 { -1010, "HPI data exceeds limits" },
 { -1011, "HPI invalid params" },
 { -1012, "HPI invalid data" },
 { -1013, "HPI not present" },
 { -1014, "HPI invalid data field" },
 { -1015, "HPI invalid sensor command" },
 { -1016, "HPI no response" },
 { -1017, "HPI duplicate request" },
 { -1018, "HPI updating" },
 { -1019, "HPI initializing" },
 { -1020, "HPI unknown error" },
 { -1021, "HPI invalid session" },
 { -1022, "HPI invalid domain" },
 { -1023, "HPI invalid resource id" },
 { -1024, "HPI invalid request" },
 { -1025, "HPI entity not present" },
 { -1026, "HPI uninitialized" }
};
char def_estr[15] = "HPI error %d   ";


char *decode_error(SaErrorT code)
{
        int i;
        char *str = NULL;
        for (i = 0; i < NECODES; i++) {
                if (code == ecodes[i].code) { str = ecodes[i].str; break; }
        }
        if (str == NULL) { 
                sprintf(&def_estr[10],"%d",code);
                str = &def_estr[0];
        }
        return(str);
}

 
/* end hpisel.c */
