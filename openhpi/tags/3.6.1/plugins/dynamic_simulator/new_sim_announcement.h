/** 
 * @file    new_sim_announcement.h
 *
 * The file includes an abstract class for control handling:\n
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
#ifndef __NEW_SIM_ANNOUNCEMENT_H__
#define __NEW_SIM_ANNOUNCEMENT_H__


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

extern "C" {
#include "SaHpi.h"
}


/**
 * @class NewSimulatorAnnouncement
 * 
 * Class for holding Announcement information
 * 
 **/
class NewSimulatorAnnouncement {

protected:
   /// Announcement data
   SaHpiAnnouncementT         m_announcement;
   

public:
  NewSimulatorAnnouncement( SaHpiAnnouncementT &ann );
		               
  virtual ~NewSimulatorAnnouncement();

  /// Return the number (entryId) of an announcement
  virtual unsigned int EntryId() const { return (unsigned int) m_announcement.EntryId; }
  /// Return the severity of an announcement
  virtual SaHpiSeverityT Severity() const { return m_announcement.Severity; }
  /// Return the timestamp of an announcement
  virtual SaHpiTimeT TimeStamp() const { return m_announcement.Timestamp; }
  /// Return the Announcement structure
  SaHpiAnnouncementT &AnnRec() { return m_announcement; }
  /// Return if an announcement is acknowledge
  bool   IsAcknowledge() { return (bool)  m_announcement.Acknowledged; }
  /// Set Acknowledge flag
  void   SetAcknowledge( SaHpiBoolT ack ) { m_announcement.Acknowledged = ack; }
  
  /// abstract method to dump Announcement information
  virtual void Dump( NewSimulatorLog &dump ) const;
};


#endif
