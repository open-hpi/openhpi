/** 
 * @file    new_sim_announcement.cpp
 *
 * The file includes a class for holding announcement information:\n
 * NewSimulatorAnnouncement
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
 */
 
#include "new_sim_announcement.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorAnnouncement::NewSimulatorAnnouncement( SaHpiAnnouncementT &ann ) {
   	
   memcpy(&m_announcement, &ann, sizeof( SaHpiAnnouncementT ));                                     
}


/**
 * Destructor
 **/
NewSimulatorAnnouncement::~NewSimulatorAnnouncement() {}

/** 
 * Dump the control information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorAnnouncement::Dump( NewSimulatorLog &dump ) const {
   dump << "---------------\n";
   dump << "Announcement Id " << m_announcement.EntryId << "\n";
   dump << "Announcement Name " << m_announcement.StatusCond.Name.Value << "\n";
   dump << "Acknowledge = " << m_announcement.Acknowledged;
   dump << " AddedByUser = " << m_announcement.AddedByUser << "\n";
}
