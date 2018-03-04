/** 
 * @file    new_sim_log.cpp
 *
 * The file includes a class for handling the log files:\n
 * NewSimulatorLog
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
 **/
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <assert.h>

#include "new_sim_log.h"
#include "new_sim_utils.h"

/// Define object stdlog of class NewSimulatorLog
NewSimulatorLog stdlog;

/**
 * Constructor
 **/
NewSimulatorLog::NewSimulatorLog()
               : m_lock_count( 0 ),
                 m_open_count( 0 ),
                 m_hex( false ),
                 m_time( false ),
                 m_recursive( false ),
                 m_std_out( false ),
                 m_std_err( false ) {}

/**
 * Destructor
 **/
NewSimulatorLog::~NewSimulatorLog() {}

/**
 * Open logfile(s)
 * 
 * @param properties properties to be set
 * @param filename pointer on string with filename
 * @param max_log_files max logfiles
 * 
 * return success
 **/
bool NewSimulatorLog::Open( int properties, const char *filename, int max_log_files ) {
   m_open_count++;

   if ( m_open_count > 1 )
      // already open
      return true;

   assert( m_lock_count == 0 );

   if ( properties & dIpmiLogStdOut )
      m_std_out = true;

   if ( properties & dIpmiLogStdErr )
      m_std_err = true;

   char file[1024] = "";

   if ( properties & dIpmiLogLogFile ) {
      char tf[1024];
      int i;
      struct stat st1, st2;

      if ( filename == 0 || *filename == 0 ) {
         fprintf( stderr, "not filename for logfile !\n" );
         return false;
	  }

      // max numbers of logfiles
      if ( max_log_files < 1 ) max_log_files = 1;

      // find a new one or the oldes logfile
      for( i = 0; i < max_log_files; i++ ) {
      	
         snprintf( tf, sizeof(tf), "%s%02d.log", filename, i );

         if ( file[0] == 0 ) strcpy( file, tf );
         if ( !stat( tf, &st1 ) && S_ISREG( st1. st_mode ) ) {
            if ( !stat( file, &st2 ) && S_ISREG( st1. st_mode ) && st2.st_mtime > st1.st_mtime )
               strcpy( file, tf );
         } else {
            strcpy( file, tf );
            break;
         }
      }
   }

// It looks to me like a small BUG - it is already handled above 
/*
   if ( properties & dIpmiLogFile ) {
      if ( filename == 0 || *filename == 0 ) {
	     fprintf( stderr, "not filename for logfile !\n" );
         return false;
	  }
  
      strcpy( file, filename );
   }
*/  
   if ( file[0] ) {
      m_fd = fopen( file, "w" );

      if ( m_fd == 0 ) {
         fprintf( stderr, "can not open logfile %s\n", file );
	     return false;
	  }
   }

   m_nl = true;

   return true;
}


/**
 * Close logfile(s)
 **/
void NewSimulatorLog::Close() {
   m_open_count--;
   assert( m_open_count >= 0 );

   if ( m_open_count > 0 )
      return;

   assert( m_lock_count == 0 );
   assert( m_nl );

   if ( m_fd ) {
      fclose( m_fd );
      m_fd = 0;
   }

   m_std_out = false;
   m_std_err = false;
}


/**
 * Write the output dependent on which flags are set.
 * 
 * @param str pointer on string to be printed
 **/

void NewSimulatorLog::Output( const char *str ) {
   int l = strlen( str );

   if ( m_fd )      fwrite( str, l, 1, m_fd );

   if ( m_std_out ) fwrite( str, l, 1, stdout );

   if ( m_std_err ) fwrite( str, l, 1, stderr );
}


/**
 * Start the logfile writing.
 **/
void NewSimulatorLog::Start() {
   //Lock();

   if ( m_nl ) {
      if ( m_time ) {
         struct timeval tv;
         gettimeofday( &tv, 0 );

	     char b[dDateTimeStringSize+5];
         NewSimulatorDateTimeToString( tv.tv_sec, b );
#if defined(__sparc) || defined(__sparc__)
         snprintf( b + dDateTimeStringSize - 1, 6, ".%03ld ", (long)tv.tv_usec / 1000 );
#else
         snprintf( b + dDateTimeStringSize - 1, 6, ".%03ld ", tv.tv_usec / 1000 );
#endif
         Output( b );
      }
   }
}


/**
 * Output operator definition for a bool value
 **/
NewSimulatorLog & NewSimulatorLog::operator<<( bool b ) {
   Start();

   Output( b ? "true" : "false" );

   return *this;
}


/**
 * Output operator definition for a unsigned char value
 **/
NewSimulatorLog & NewSimulatorLog::operator<<( unsigned char c ) {
   Start();

   char b[5];
   snprintf( b, sizeof(b), "0x%02x", c );

   Output( b );

   return *this;
}


/**
 * Output operator definition for a int value
 **/
NewSimulatorLog & NewSimulatorLog::operator<<( int i ) {
   Start();

   char b[20];
   snprintf( b, sizeof(b), "%d", i );
   Output( b );

   return *this;
}

/**
 * Output operator definition for a long value
 **/
NewSimulatorLog & NewSimulatorLog::operator<<( long l ) {
   Start();

   char b[20];
   snprintf( b, sizeof(b), "%ld", l );
   Output( b );

   return *this;
}

/**
 * Output operator definition for a unsigned int value
 **/
NewSimulatorLog & NewSimulatorLog::operator<<( unsigned int i ) {
   Start();

   char b[20];

   if ( m_hex ) snprintf( b, sizeof(b), "0x%08x", i );
   else snprintf( b, sizeof(b), "%u", i );
   Output( b );
 
   return *this;
}


/**
 * Output operator definition for a double value
 **/
NewSimulatorLog & NewSimulatorLog::operator<<( double d ) {
   Start();
  
   char b[20];
   snprintf( b, sizeof(b), "%f", d );
   Output( b );

   return *this;  
}


/**
 * Output operator definition for a string
 **/
NewSimulatorLog & NewSimulatorLog::operator<<( const char *str ) {
   Log( "%s", str );

   return *this;
}

/**
 * Writes a complete log entry 
 **/
void NewSimulatorLog::Log( const char *fmt, ... ) {
   Start();

   va_list ap;
   va_start( ap, fmt );

   char b[10240];
   vsnprintf( b, 10240, fmt, ap );  

   va_end( ap );
   
   // It looks to me like a small BUG - can lead to an overflow in extreme rare cases 
   // char buf[10230] = "";
   char buf[10240] = "";

   char *p = b;
   char *q = buf;

   m_nl = false;

   while( *p ) {
      if ( *p == '\n' ) {
         m_nl = true;
         *q++ = *p++;
         *q = 0;
         
         Output( buf );
         q = buf;

	     continue;
	  }

      m_nl = false;
      *q++ = *p++;
   }

   if ( q != b ) {
       *q = 0;
       Output( buf );
   }

   if ( m_nl ) {
      if ( m_fd ) fflush( m_fd );

      if ( m_std_out ) fflush( stdout );

      if ( m_std_err ) fflush( stderr );

       //while( m_lock_count > 0 )
       //    Unlock();
   }
}


/**
 * Writes hex entries
 * 
 * @param data pointer on hex values
 * @param size size of field 
 **/
void NewSimulatorLog::Hex( const unsigned char *data, int size ) {
   char str[256];
   char *s = str;
   int i, remaining;

   for( i = 0; i < size; i++ ) {
      if ( i != 0 && (i % 16) == 0 ) {
         Log( "%s\n", str );
         s = str;
      }
      remaining = sizeof(str) - (s - str);
      if (remaining > 0)
         s += snprintf( s, remaining, " %02x", *data++ );
   }

   if ( s != str ) Log( "%s\n", str );
}


/**
 * Mark a section inside the logfile
 * 
 * @param section kind of section
 * @param name name of section 
 **/
void NewSimulatorLog::Begin( const char *section, const char *name ) {
   
   if ( IsRecursive() ) *this << section << " \"" << name << "\"\n{\n";
}

/**
 * End a section inside the logfile
 **/
void NewSimulatorLog::End() {
   if ( IsRecursive() ) *this << "}\n\n\n";
}


/**
 * Writes an entry and returns address of object
 * 
 * @param entry string
 **/
NewSimulatorLog & NewSimulatorLog::Entry( const char *entry ) {
   char str[256];
   strcpy( str, entry );
   int l = 30 - strlen( entry );

   if ( l > 0 ) {
      char *p = str + strlen( entry );
      while( l-- > 0 ) *p++ = ' ';
      *p = 0;
   }

   *this << "        " << str << " = ";

   return *this;
}
