/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <sldague@us.ibm.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include <SaHpi.h>
#include <oh_utils.h>

#define fill_text_buffer(b, chartype, string)                    \
        do {                                                            \
                b.DataType = chartype;                                  \
                b.Language = SAHPI_LANG_ENGLISH;                        \
                b.DataLength = sizeof(string) - 1;                          \
                strncpy(b.Data,string,sizeof(string) - 1);                  \
        } while(0)

#define TEXTT SAHPI_TL_TYPE_TEXT
#define UNICODET SAHPI_TL_TYPE_UNICODE 
#define BCDPLUST SAHPI_TL_TYPE_BCDPLUS
#define ASCII6T SAHPI_TL_TYPE_ASCII6
#define BINARYT SAHPI_TL_TYPE_BINARY 

#define failed(num) \
        do {                 \
                failcount++; \
                dbg("Failed Test %d", num); \
        } while(0)


int main(int argc, char **argv) 
{
        /* const char *expected_str; */
        // SaErrorT   expected_err, err;
        SaHpiTextBufferT buffer;
        int failcount = 0;
       
        setenv("OPENHPI_DEBUG","YES",1);
        // Test1 - valid text string
        
        fill_text_buffer(buffer, TEXTT, "a brown fox! ");
        if(!oh_valid_textbuffer(&buffer)) {
                failed(1);
        }

        // Test2 - valid ascii6
        fill_text_buffer(buffer, ASCII6T, "XYZ 0123!");
        if(!oh_valid_textbuffer(&buffer)) {
                failed(2);
        }
        
        // Test3 - valid utf16 - this is Om 42, incase anyone cares
        fill_text_buffer(buffer, UNICODET, "à¼à¼¤à¼¢");
        if(!oh_valid_textbuffer(&buffer)) {
                failed(3);
        }
        
        // Test 4 - valid BCDPLUS
        fill_text_buffer(buffer, BCDPLUST, "1234-98.56:11");
        if(!oh_valid_textbuffer(&buffer)) {
                failed(4);
        }

        // Test 5 - invalid utf16 - 
        fill_text_buffer(buffer, UNICODET, "à¼à¼¤à¼");
        if(oh_valid_textbuffer(&buffer)) {
                failed(5);
        }
        
        // Test 6 - invalid BCDPLUS
        fill_text_buffer(buffer, BCDPLUST, "a quick brown fox");
        if(oh_valid_textbuffer(&buffer)) {
                failed(6);
        }
        
        // Test7 - invalid ascii6
        fill_text_buffer(buffer, ASCII6T, "abc");
        if(oh_valid_textbuffer(&buffer)) {
                failed(7);
        }
        
        // Test 8 - invalid ascii6 (has a null)
        memset(buffer.Data, sizeof(*buffer.Data), 0);
        fill_text_buffer(buffer, ASCII6T, "abc");
        buffer.DataLength++;
        if(oh_valid_textbuffer(&buffer)) {
                failed(8);
        }

        // Test 9 - invalid bcdplus (has a null)
        memset(buffer.Data, sizeof(*buffer.Data), 0);
        fill_text_buffer(buffer, BCDPLUST, "1234");
        buffer.DataLength++;
        if(oh_valid_textbuffer(&buffer)) {
                failed(9);
        }
        
        if(failcount) {
                return -1;
        }
	return(0);
}
