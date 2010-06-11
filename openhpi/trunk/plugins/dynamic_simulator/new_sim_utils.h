/** 
 * @file    new_sim_utils.h
 *
 * The file includes an some utilities and a class for time handling\n
 * cClass
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
 * Thanks to 
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *    
 */
 
 
#ifndef __NEW_SIM_UTILS_H__
#define __NEW_SIM_UTILS_H__

#include <sys/time.h>

/**
 * @enum tNewSimulatorFruState
 * 
 * Mapping Fru states to the ipmi state numbering.
 **/
enum tNewSimulatorFruState
{
  eFruStateNotInstalled           = 0,
  eFruStateInactive               = 1,
  eFruStateActivationRequest      = 2,
  eFruStateActivationInProgress   = 3,
  eFruStateActive                 = 4,
  eFruStateDeactivationRequest    = 5,
  eFruStateDeactivationInProgress = 6,
  eFruStateCommunicationLost      = 7
};

/**
 * Returns a string for a Fru state
 * 
 * @param state IPMI state
 * @return string with the name of the state
 **/
const char *NewSimulatorFruStateToString( tNewSimulatorFruState state );

/// Size of a date string
#define dDateStringSize 11
void NewSimulatorDateToString( unsigned int time, char *str );

/// Size of a time string
#define dTimeStringSize 9
void NewSimulatorTimeToString( unsigned int time, char *str );

/// Size of a date and time string
#define dDateTimeStringSize 20
void NewSimulatorDateTimeToString( unsigned int time, char *str );


/**
 * @class cTime
 * 
 * The time class can be used to work with time values
 **/
class cTime
{
public:
  /// time value
  timeval m_time;

  /**
   * Constructor
   **/
  cTime()
  {
    m_time.tv_sec  = 0;
    m_time.tv_usec = 0;    
  }
  
  /**
   * qualified Constructor with timeval
   **/
  cTime( const struct timeval &tv )
  {
    m_time.tv_sec  = tv.tv_sec;
    m_time.tv_usec = tv.tv_usec;
  }
  
  /**
   * copy Constructor
   **/
  cTime( const cTime &t )
  {
    m_time.tv_sec  = t.m_time.tv_sec;
    m_time.tv_usec = t.m_time.tv_usec;
  }
  
  /**
   * qualified Constructor with seconds and usec
   **/
  cTime( unsigned int s, unsigned int u )
  {
    m_time.tv_sec = s;
    m_time.tv_usec = u;
  }

  /**
   * noramlize the internal data
   */
  void Normalize()
  {
    while( m_time.tv_usec > 1000000 )
       {
         m_time.tv_usec -= 1000000;
         m_time.tv_sec++;
       }

    while( m_time.tv_usec < 0 )
       {
         m_time.tv_usec += 1000000;
         m_time.tv_sec--;
       }
  }

  /**
   * compare two time classes
   */
  int Cmp( const cTime &t )
  {
    if ( m_time.tv_sec < t.m_time.tv_sec )
         return -1;

    if ( m_time.tv_sec > t.m_time.tv_sec )
         return 1;

    if ( m_time.tv_usec < t.m_time.tv_usec )
         return -1;

    if ( m_time.tv_usec > t.m_time.tv_usec )
         return 1;

    return 0;
  }

  /**
   * lower operator
   **/
  bool operator<( const cTime &t )
  {
    return Cmp( t ) < 0;
  }

  /**
   * lower equal operator
   **/
  bool operator<=( const cTime &t )
  {
    return Cmp( t ) < 0;
  }

  /**
   * higher operator
   **/
  bool operator>( const cTime &t )
  {
    return Cmp( t ) > 0;
  }

  /**
   * higher equal operator
   **/
  bool operator>=( const cTime &t )
  {
    return Cmp( t ) >= 0;
  }

  /**
   * equal operator
   **/
  bool operator==( const cTime &t )
  {
    return Cmp( t ) == 0;
  }

  /**
   * not equal operator
   **/
  bool operator!=( const cTime &t )
  {
    return Cmp( t ) == 0;
  }

  /**
   * add cTime class operator 
   **/
  cTime &operator+=( const cTime &t )
  {
    m_time.tv_sec += t.m_time.tv_sec;
    m_time.tv_usec += t.m_time.tv_usec;

    Normalize();

    return *this;
  }

  /**
   * add msec value operator 
   **/
  cTime &operator+=( int ms )
  {
    m_time.tv_sec  += ms / 1000;
    m_time.tv_usec += (ms % 1000) * 1000;

    Normalize();

    return *this;
  }

  /**
   * subtract time value operator 
   **/
  cTime &operator-=( int ms )
  {
    m_time.tv_sec  -= ms / 1000;
    m_time.tv_usec -= (ms % 1000) * 1000;

    Normalize();

    return *this;
  }
  
  /**
   * subtract time value operator 
   **/
  cTime &operator-=( cTime t )
  {
    m_time.tv_sec  -= t.m_time.tv_sec;
    m_time.tv_usec -= t.m_time.tv_usec;

    Normalize();

    return *this;
  }

  /**
   * fill the class with actual time 
   **/
  static cTime Now()
  {
    cTime t;
    gettimeofday( &t.m_time, 0 );

    return t;
  }
  
  /**
   * get the time value in msec
   **/
   unsigned int GetMsec()
   {
   	  return (unsigned int) (m_time.tv_sec * 1000 + m_time.tv_usec / 1000);
   }
   
   /**
   * Clear the value
   **/
   void Clear()
   {
      m_time.tv_sec  = 0;
      m_time.tv_usec = 0;
   }
   
   /**
   * Check whether the time was set
   **/
   bool IsSet()
   {
      return ( !(( m_time.tv_sec == 0 ) && ( m_time.tv_usec == 0 )) );
   }
};


#endif
