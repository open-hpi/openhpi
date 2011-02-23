/** 
 * @file    new_sim_utils.cpp
 *
 * The file includes some function for the work with time values and
 * FRU states.
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
 *      
 **/
 
#include <time.h>

#include "new_sim_utils.h"

/// Array including the strings of the FRU states
static const char *fru_state[] =
{
  "not installed",
  "inactive",
  "activation request",
  "activation in progress",
  "active",
  "deactivation request",
  "deactivation in progress",
  "communication lost"
};


/**
 * Return a string a with the state fo a FRU
 * 
 * @param val state of the FRU
 * @return string with the FRU state text
 **/
const char * NewSimulatorFruStateToString( tNewSimulatorFruState val ) {

   if ( val > eFruStateCommunicationLost )
      return "invalid";

   return fru_state[val];
}


/**
 * Fill a string a with date information
 * 
 * @param t time value
 * @param str pointer on the string to be filled
 **/
void NewSimulatorDateToString( unsigned int t, char *str ) {
  struct tm tmt;
  time_t dummy = t;

  localtime_r( &dummy, &tmt );

  // 2003.10.30
  strftime( str, dDateStringSize, "%Y.%m.%d", &tmt );
}


/**
 * Fill a string a with time information
 * 
 * @param t time value
 * @param str pointer on the string to be filled
 **/
void NewSimulatorTimeToString( unsigned int t, char *str ) {
  struct tm tmt;
  time_t dummy = t;

  localtime_r( &dummy, &tmt );

  // 11:11:11
  strftime( str, dTimeStringSize, "%H:%M:%S", &tmt );
}


/**
 * Fill a string a with date and time information
 * 
 * @param t time value
 * @param str pointer on the string to be filled
 **/
void NewSimulatorDateTimeToString( unsigned int t, char *str ) {
  struct tm tmt;
  time_t dummy = t;
  localtime_r( &dummy, &tmt );

  // 2003.10.30 11:11:11
  strftime( str, dDateTimeStringSize, "%Y.%m.%d %H:%M:%S", &tmt );
}
