/** 
 * @file    new_sim_annunciator.h
 *
 * The file includes a class for annunciator handling:\n
 * NewSimulatorAnnunciator
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
 *    
 */

#ifndef __NEW_SIM_ANNUNCIATOR_H__
#define __NEW_SIM_ANNUNCIATOR_H__


extern "C" {
#include "SaHpi.h"
}


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_ANNOUNCEMENT_H__
#include "new_sim_announcement.h"
#endif

class  NewSimulatorDomain;

/**
 * @class NewSimulatorAnnunciator
 * 
 * Class for simulating Annunciators
 * 
 **/
class NewSimulatorAnnunciator : public NewSimulatorRdr {

private:
  /// Record with the annunciator rdr information
  SaHpiAnnunciatorRecT        m_ann_rec;
  /// Mode of the annunciator
  SaHpiAnnunciatorModeT       m_mode;
  /// Array including the announcements
   cArray<NewSimulatorAnnouncement>  m_anns;
  /// last max EntryId 
  SaHpiEntryIdT m_ann_id;
  
public:
  NewSimulatorAnnunciator( NewSimulatorResource *res );
  NewSimulatorAnnunciator( NewSimulatorResource *res,
                           SaHpiRdrT rdr );
                      
  virtual ~NewSimulatorAnnunciator();

  /// Return annunciator Number
  virtual unsigned int Num() const { return m_ann_rec.AnnunciatorNum; }
  /// Return a new entryId
  SaHpiEntryIdT ValidEntryId() { return ++m_ann_id; }

  // Find and add Announcement
  NewSimulatorAnnouncement *FindAnnouncement( NewSimulatorAnnouncement *ann );
  bool AddAnnouncement( NewSimulatorAnnouncement *ann );
  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );
  virtual void SetData( SaHpiAnnunciatorRecT ann_data );
  
  // Official HPI functions
  SaErrorT GetMode( SaHpiAnnunciatorModeT &mode );
  SaErrorT SetMode( SaHpiAnnunciatorModeT mode );
  SaErrorT DeleteAnnouncement( SaHpiEntryIdT &num, SaHpiSeverityT &severity);
  SaErrorT AddAnnouncement( SaHpiAnnouncementT &ann );
  SaErrorT GetAnnouncement( SaHpiEntryIdT num, SaHpiAnnouncementT &ann );
  SaErrorT GetNextAnnouncement( SaHpiSeverityT severity, SaHpiBoolT uackOnly, SaHpiAnnouncementT &ann );
  SaErrorT SetAcknowledge( SaHpiEntryIdT num, SaHpiSeverityT severity);
  
  virtual void Dump( NewSimulatorLog &dump ) const;
};


#endif
