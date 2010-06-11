/** 
 * @file    new_sim_text_buffer.h
 *
 * The file includes a wrapper class for SaHpiTextBufferT:\n
 * NewSimulatorTextBuffer
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.1
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
 * Thanks to \n
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>\n
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *      
 */

#ifndef __NEW_SIM_TEXT_BUFFER_H__
#define __NEW_SIM_TEXT_BUFFER_H__


#ifndef __NEW_SIM_LOG_H__
#include "new_sim_log.h"
#endif

extern "C" {
#include "SaHpi.h"
}


/**
 * @class NewSimulatorTextBuffer
 * 
 * Wrapper class for SaHpiTextBufferT
 * 
 **/
class NewSimulatorTextBuffer {
	
protected:
  int BinaryToAscii( char *buffer, unsigned int len ) const;
  int BcdPlusToAscii( char *buffer, unsigned int len ) const;
  int Ascii6ToAscii( char *buffer, unsigned int len ) const;
  int LanguageToAscii( char *buffer, unsigned int len ) const;

  int AsciiToBcdPlus ( const char *input );
  int AsciiToAscii6  ( const char *input );
  int AsciiToLanguage( const char *input );

  /// internal SaHpiTextBufferT variable
  SaHpiTextBufferT m_buffer;

public:
  NewSimulatorTextBuffer();
  NewSimulatorTextBuffer( const char *string, SaHpiTextTypeT type, 
		                  SaHpiLanguageT l = SAHPI_LANG_ENGLISH );
  NewSimulatorTextBuffer( const SaHpiTextBufferT &buf );

  void Clear();

  /// return the internal buffer
  operator SaHpiTextBufferT () const { return m_buffer; }
  /// return the DataLength
  SaHpiUint8T DataLength() const { return m_buffer.DataLength; }
  
  SaHpiTextTypeT CheckAscii( const char *s );

  // convert ascii string to text buffer
  bool SetAscii( const char *string, SaHpiTextTypeT type,
		         SaHpiLanguageT l = SAHPI_LANG_ENGLISH );
  
  // copy data in the internal buffer
  bool SetData( SaHpiTextBufferT data );

  // returns length of string or -1 on error
  int GetAscii( char *buffer, unsigned int len ) const;

  bool operator==( const NewSimulatorTextBuffer &tb ) const;
  bool operator!=( const NewSimulatorTextBuffer &tb ) const;
};


NewSimulatorLog &operator<<( NewSimulatorLog &dump, const NewSimulatorTextBuffer &tb );


#endif
