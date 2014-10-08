/** 
 * @file    new_sim_text_buffer.cpp
 * 
 *
 * The file includes an abstract class for sensor handling:\n
 * NewSimulatorSensor
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.2
 * @date    2010
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * The new Simulator plugin is adapted from the ipmidirect plugin.
 * Thanks to 
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *    
 */
  
#include <string.h>

#include "new_sim_text_buffer.h"

/**
 * Constructor
 **/
NewSimulatorTextBuffer::NewSimulatorTextBuffer() {
   Clear();
}

/**
 * full qualified constructor
 **/
NewSimulatorTextBuffer::NewSimulatorTextBuffer( const char *string, 
                                                SaHpiTextTypeT type, 
				                                SaHpiLanguageT l ) {
   m_buffer.DataType = type;
   m_buffer.Language = l;

   SetAscii( string, type, l );
}

/**
 * full qualified constructor
 **/
NewSimulatorTextBuffer::NewSimulatorTextBuffer( const SaHpiTextBufferT &buf ) {
   m_buffer = buf;
}

/** 
 * Clear the internal m_buffer and set it to default values
 **/
void NewSimulatorTextBuffer::Clear() {
   m_buffer.DataType   = SAHPI_TL_TYPE_TEXT;
   m_buffer.Language   = SAHPI_LANG_ENGLISH;
   m_buffer.DataLength = 0;
   memset( m_buffer.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH );
}


/// Array to check the validity of a character for BCDPLUS en- and decoding
/// Element will be zero if not present, n-1 if present.
static SaHpiUint8T table_4_bit[256] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0x0c, 0x0d, 0x00,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x09, 0x0a, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/**
 * Convert an ascii string to BCDPLUS
 * 
 * @param s pointer on the string
 * @return DataLength after conversion
 **/
int NewSimulatorTextBuffer::AsciiToBcdPlus( const char *s ) {
   m_buffer.DataType = SAHPI_TL_TYPE_BCDPLUS;
   m_buffer.DataLength = 0;

   SaHpiUint8T *p = m_buffer.Data;
   int   bit = 0;

   while( *s ) {
      if ( m_buffer.DataLength == SAHPI_MAX_TEXT_BUFFER_LENGTH )
	     break;

      switch( bit ) {
	  case 0:
                 m_buffer.DataLength++;
                 *p = table_4_bit[(unsigned int)*s];
                 bit = 4;
                 break;

	  case 4:
                 *p |= table_4_bit[(unsigned int)*s++] << 4;
                 p++;
                 bit = 0;
                 break;
      }
   }

   return m_buffer.DataLength;
}


/// Array to check the validity of a character for ASCII 6 en- and decoding
/// Element will be zero if not present, n-1 if present.
static SaHpiUint8T table_6_bit[256] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x21, 0x08,
  0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
  0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
  0x00, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
  0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
  0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


////////
// 0       1       2       3
// 0123456789012345678901234567890
// 000000111111222222333333444444
/**
 * Convert an ascii string to ascii6
 * 
 * @param s pointer on the string
 * @return DataLength after conversion
 **/
int NewSimulatorTextBuffer::AsciiToAscii6( const char *s ) {
   m_buffer.DataType   = SAHPI_TL_TYPE_ASCII6;
   m_buffer.DataLength = 0;

   SaHpiUint8T *p = m_buffer.Data;
   int   bit = 0;

   while( *s ) {
      if ( m_buffer.DataLength == SAHPI_MAX_TEXT_BUFFER_LENGTH )
	    break;

      switch( bit ) {
	  case 0:
                 *p = table_6_bit[(unsigned int)*s++];
                 m_buffer.DataLength++;
                 bit = 6;
                 break;

	  case 2:
                 *p |= (table_6_bit[(unsigned int)*s] << 2);
                 bit = 0;
                 break;

	  case 4:
                 *p |= table_4_bit[(unsigned int)*s] << 4;
                 p++;
                 *p = (table_4_bit[(unsigned int)*s++] >> 4) & 0x3;
                 m_buffer.DataLength++;
                 bit = 2;
                 break;

	  case 6:
                 *p |= table_4_bit[(unsigned int)*s] << 6;
                 p++;
                 *p = (table_4_bit[(unsigned int)*s++] >> 2) & 0xf;
                 m_buffer.DataLength++;
                 bit = 4;
                 break;
      }
   }

   return m_buffer.DataLength;
}

/**
 * Copy an ascii string into the internal structure
 * 
 * @param s pointer on the string
 * @return DataLength
 **/
int NewSimulatorTextBuffer::AsciiToLanguage( const char *s ) {
   m_buffer.DataType = SAHPI_TL_TYPE_TEXT;

   int l = strlen( s );

   if ( l > SAHPI_MAX_TEXT_BUFFER_LENGTH ) l = SAHPI_MAX_TEXT_BUFFER_LENGTH;

   m_buffer.DataLength = l;

   strncpy( ( char *)m_buffer.Data, s, SAHPI_MAX_TEXT_BUFFER_LENGTH );

   return l;
}


/**
 * Set the value internally on ASCII
 * 
 * @param string pointer on the string
 * @param type SaHpiTextTypeT value
 * @param l language value
 * @return DataLength
 **/
bool NewSimulatorTextBuffer::SetAscii( const char *string, 
                                        SaHpiTextTypeT type,
			                            SaHpiLanguageT l ) {
   m_buffer.Language = l;
  
   switch( type ) {
       
   case SAHPI_TL_TYPE_BCDPLUS:
	    AsciiToBcdPlus( string );
	    return true;

   case SAHPI_TL_TYPE_ASCII6:
	    AsciiToAscii6( string );	    
	    return true;

   case SAHPI_TL_TYPE_TEXT:
	    AsciiToLanguage( string );
	    return true;
	    
   default:
	    break;
   }

   return false;
}


/**
 * Fill m_buffer
 * 
 * @param data SaHpiTextBufferT record to set internally 
 * @return true
 **/
bool NewSimulatorTextBuffer::SetData( SaHpiTextBufferT data ) {
   stdlog << "get DataLength = " << data.DataLength << "\n";
   
   memcpy( &m_buffer, &data, sizeof( SaHpiTextBufferT ));
 
   stdlog << "Databuffer: ";
   for (int i = 0; i < m_buffer.DataLength; i++)
      stdlog << m_buffer.Data[i];
   stdlog << "\n";
   
   return true;
}


/**
 * Return the type of the string
 * 
 * @param s pointer on the string
 * @return type of string 
 **/
SaHpiTextTypeT NewSimulatorTextBuffer::CheckAscii( const char *s ) {
   SaHpiTextTypeT type = SAHPI_TL_TYPE_BCDPLUS;

   while( *s ) {
      if ( type  == SAHPI_TL_TYPE_BCDPLUS && table_4_bit[(int)*s] == 0 )
	     type = SAHPI_TL_TYPE_ASCII6;

      if ( type == SAHPI_TL_TYPE_ASCII6 && table_6_bit[(int) *s] == 0 ) {
	     type = SAHPI_TL_TYPE_TEXT;
	     break;
      }
   }
  
   return type;
}


/**
 * Copy the internal buffer into a Ascii buffer
 * 
 * @param buffer pointer on Ascii buffer to be filled
 * @param len length of buffer
 * @return length 
 **/
int NewSimulatorTextBuffer::BinaryToAscii( char *buffer, unsigned int len ) const {
   unsigned int l = m_buffer.DataLength;

   if ( l >= len ) l = len - 1;

   memcpy( buffer, m_buffer.Data, l );
   buffer[l] = 0;

   return len;
}

/**
 * Copy the internal buffer into a Ascii buffer
 * 
 * @param buffer pointer to Ascii buffer to be filled
 * @param len length of buffer
 * @return length 
 **/
int NewSimulatorTextBuffer::BcdPlusToAscii( char *buffer, unsigned int len ) const {
   static char table[] = "0123456789 -.:,_";

   unsigned int real_length = 2 * m_buffer.DataLength;

   if ( len > real_length ) len = real_length;

   bool first = true;
   const unsigned char *d = m_buffer.Data;

   for( unsigned int i = 0; i < len; i++ ) {
      int val = 0;

      if ( first ) val = *d & 0xf;
      else val = (*d++ >> 4) & 0xf;

      first = !first;
      *buffer++ = table[val];
   }

   *buffer = 0;

   return len;
}


/**
 * Copy the internal buffer into a Ascii buffer
 * 
 * @param buffer pointer to Ascii buffer to be filled
 * @param len length of buffer
 * @return length 
 **/
int NewSimulatorTextBuffer::Ascii6ToAscii( char *buffer, unsigned int len ) const {
   static char table[64] =
   {
    ' ', '!', '"', '#', '$', '%', '&', '\'',
    '(', ')', '*', '+', ',', '-', '.', '/', 
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?',
    '&', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_' 
   };

   unsigned int real_length = (m_buffer.DataLength * 8) / 6;

   if ( len >= real_length ) len = real_length;

   const unsigned char *d = m_buffer.Data;
   int bo = 0;

   for( unsigned int i =0; i < len; i++ ) {
      int val = 0;

      switch( bo ) {
	  case 0:
                 val = *d & 0x3f;
                 bo = 6;
                 break;

	  case 2:
                 val = (*d >> 2) & 0x3f;
                 d++;
                 bo = 0;
                 break;

	  case 4:
                 val = (*d >> 4) & 0xf;
                 d++;
                 val |= (*d & 0x3) << 4;
                 bo = 2;
                 break;
                 
	  case 6:
                 val = (*d >> 6) & 0x3;
                 d++;
                 val |= (*d & 0xf) << 2;
                 bo = 4;
                 break;
      }

      *buffer++ = table[val];
   }

   *buffer = 0;

   return len;
}

/**
 * Copy the internal buffer into a Ascii buffer
 * Only SAHPI_LANG_ENGLISH is supported. 
 * 
 * @param buffer pointer to Ascii buffer to be filled
 * @param len length of buffer
 * @return length 
 **/
int NewSimulatorTextBuffer::LanguageToAscii( char *buffer, unsigned int len ) const {

   if ( m_buffer.Language == SAHPI_LANG_ENGLISH ) return BinaryToAscii( buffer, len );

   // unicode not supported
   return -1;
}


/**
 * Fill the an Ascii buffer with the internal value
 * 
 * @param buffer pointer to Ascii buffer to be filled
 * @param len length of buffer
 * @return length 
 **/
int NewSimulatorTextBuffer::GetAscii( char *buffer, unsigned int len ) const {
   switch( m_buffer.DataType ) {
   case SAHPI_TL_TYPE_BINARY:
            return BinaryToAscii( buffer, len );

   case SAHPI_TL_TYPE_BCDPLUS:
            return BcdPlusToAscii( buffer, len );

   case SAHPI_TL_TYPE_ASCII6:
            return Ascii6ToAscii( buffer, len );

   case SAHPI_TL_TYPE_TEXT:
            return LanguageToAscii( buffer, len );

   default:
            return -1;
   }
}


/**
 * Comparison equal operator
 **/
bool NewSimulatorTextBuffer::operator==( const NewSimulatorTextBuffer &tb ) const {
 
   if ( m_buffer.DataType != tb.m_buffer.DataType ) return false;
  
   if ( m_buffer.Language != tb.m_buffer.Language ) return false;

   if ( m_buffer.DataLength != tb.m_buffer.DataLength ) return false;

   if ( m_buffer.DataLength )
       return memcmp( m_buffer.Data, tb.m_buffer.Data, tb.m_buffer.DataLength ) == 0;

   return true;
}

/**
 * Comparison not equal operator
 **/
bool NewSimulatorTextBuffer::operator!=( const NewSimulatorTextBuffer &tb ) const {

   return !operator==( tb );
}

/**
 * output operator
 **/
NewSimulatorLog & operator<<( NewSimulatorLog &dump, const NewSimulatorTextBuffer &tb ) {
 
   char str[2*SAHPI_MAX_TEXT_BUFFER_LENGTH+1] = "";
   tb.GetAscii( str, 2*SAHPI_MAX_TEXT_BUFFER_LENGTH+1 );

   dump << str;

   return dump;
}
