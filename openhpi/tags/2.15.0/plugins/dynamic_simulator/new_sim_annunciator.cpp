/** 
 * @file    new_sim_annunciator.cpp
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "new_sim_domain.h"
#include "new_sim_annunciator.h"
#include "new_sim_announcement.h"
#include "new_sim_entity.h"


/**
 * Constructor
 **/
NewSimulatorAnnunciator::NewSimulatorAnnunciator( NewSimulatorResource *res )
  : NewSimulatorRdr( res, SAHPI_ANNUNCIATOR_RDR ), 
    m_mode( SAHPI_ANNUNCIATOR_MODE_SHARED ),
    m_ann_id( 0 ) {
    	
   memset( &m_ann_rec, 0, sizeof( SaHpiAnnunciatorRecT ));
}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorAnnunciator::NewSimulatorAnnunciator( NewSimulatorResource *res,
                                        SaHpiRdrT rdr )
  : NewSimulatorRdr( res, SAHPI_ANNUNCIATOR_RDR, rdr.Entity, rdr.IsFru, rdr.IdString ),
    m_mode( SAHPI_ANNUNCIATOR_MODE_SHARED ),
    m_ann_id( 0 ) {

   memcpy(&m_ann_rec, &rdr.RdrTypeUnion.AnnunciatorRec, sizeof( SaHpiAnnunciatorRecT ));
}
  
                      
/**
 * Destructor
 **/
NewSimulatorAnnunciator::~NewSimulatorAnnunciator()
{
}


/** 
 * Dump the Annunciator information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorAnnunciator::Dump( NewSimulatorLog &dump ) const {
  char str[256];
  IdString().GetAscii( str, 256 );

  dump << "Annunciator: " << m_ann_rec.AnnunciatorNum << " " << str << "\n";
  dump << "Announcements: " << "\n";
  for (int i= 0; i < m_anns.Num(); i++) {
      m_anns[i]->Dump( dump );
  }
}


/**
 * A rdr structure is filled with the data
 * 
 * This method is called by method NewSimulatorRdr::Populate().
 * 
 * @param resource Address of resource structure
 * @param rdr Address of rdr structure
 * 
 * @return true
 **/
bool NewSimulatorAnnunciator::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
  
  if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
       return false;

  // Annunciator record
  memcpy(&rdr.RdrTypeUnion.AnnunciatorRec, &m_ann_rec, sizeof(SaHpiAnnunciatorRecT));

  return true;
}


/**
 * Find announcement by announcement pointer
 * 
 * @param ann pointer on the announcement to be found 
 * @return return the same pointer if it could be found successfully 
 **/
NewSimulatorAnnouncement *NewSimulatorAnnunciator::FindAnnouncement( NewSimulatorAnnouncement *ann ) {
	
   for( int i = 0; i < m_anns.Num(); i++ ) {
      NewSimulatorAnnouncement *a = m_anns[i];
      if ( a == ann ) return ann;
   }

   return 0;
}


/**
 * Add an announcement to the array if it isn't already included in the array
 * 
 * @param ann pointer to announcement to be added
 * @return bool if successful 
 **/ 
bool NewSimulatorAnnunciator::AddAnnouncement( NewSimulatorAnnouncement *ann ) {
	
   if ( FindAnnouncement( ann ) ) {
      return false;
   }
   
   if (ann->EntryId() > m_ann_id)
      m_ann_id = ann->EntryId();
      
   m_anns.Add( ann );

   return true;
}


/**
 * The annunciator data is copied into the internal record structer
 * 
 * @param ann_data Record to be copied into the internal structure
 **/ 
void NewSimulatorAnnunciator::SetData( SaHpiAnnunciatorRecT ann_data ) {
	
   memcpy( &m_ann_rec, &ann_data, sizeof( SaHpiAnnunciatorRecT ));   
}


// Official HPI functions
 
 /**
 * HPI function saHpiAnnunciatorGetNext()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param severity of announcement to get
 * @param uackOnly flag if only unacknowledge announcement should be returned
 * @param ann address to announcement structure where the announcement should be copied to
 * 
 * @return HPI return code
 **/    
SaErrorT NewSimulatorAnnunciator::GetNextAnnouncement( SaHpiSeverityT severity, 
                                                       SaHpiBoolT uackOnly, 
                                                       SaHpiAnnouncementT &ann ) {
   SaHpiEntryIdT num;
   SaHpiTimeT    time;
   NewSimulatorAnnouncement *a;
   bool found = false;
   
   if (&ann == NULL)
      return SA_ERR_HPI_INVALID_PARAMS;

   if (m_anns.Num() == 0)
      return SA_ERR_HPI_NOT_PRESENT;

   if (ann.EntryId == SAHPI_FIRST_ENTRY) {
      a = m_anns[0];
      num = a->EntryId();
      time = a->TimeStamp();
      found = true;
   } else {
      num = ann.EntryId;
      time = ann.Timestamp;
   }
   
   for (int i = 0; i < m_anns.Num(); i++) {
      a = m_anns[i];
      
      // Be aware that the previos Announcement can already be deleted
      if (( a->EntryId() > num ) && ( a->TimeStamp() >= time )) {
         found = true;
      }
      
      if (found) {
        // If we have found the previous announcement, we have to check the criteria	
      	if ( (severity == SAHPI_ALL_SEVERITIES) ||
      	      (severity == a->Severity()) ) {
      	
      	   if ( (uackOnly == SAHPI_TRUE) &&
      	         !a->IsAcknowledge() ) {
      	         	
      	      memcpy( &ann, &a->AnnRec(), sizeof( SaHpiAnnouncementT ));
      	      return SA_OK;
      	      
      	   } else if (uackOnly == SAHPI_FALSE) {

      	      memcpy( &ann, &a->AnnRec(), sizeof( SaHpiAnnouncementT ));
      	      return SA_OK;
      	   }
      	}

      } else {
         // Find the previous announciator
         if ( a->EntryId() == num ) {
            if ( a->TimeStamp() == time ) {
            	   found = true;
            } else {
            	   return SA_ERR_HPI_INVALID_DATA;
            }
         }
      }
   }
         
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * HPI function saHpiAnnunciatorGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param num announcement id 
 * @param ann address to Announcement structure where the announcement should be copied to
 * 
 * @return HPI return code
 **/   
SaErrorT NewSimulatorAnnunciator::GetAnnouncement( SaHpiEntryIdT num, SaHpiAnnouncementT &ann ) {
   
   if ((&ann == NULL) ||
        (num == SAHPI_FIRST_ENTRY) ||
        (num == SAHPI_LAST_ENTRY))
      return SA_ERR_HPI_INVALID_PARAMS;
   
   for (int i = 0; i < m_anns.Num(); i++) {
   	  NewSimulatorAnnouncement *a = m_anns[i];
      
      if ( a->EntryId() == num ) {
      	memcpy( &ann, &a->AnnRec(), sizeof( SaHpiAnnouncementT ));
      	return SA_OK;
      } 
   }
   
   return SA_ERR_HPI_NOT_PRESENT;
}
/**
 * HPI function saHpiAnnunciatorAcknowledge()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param num announcement id to be acknowledged
 * @param severity everity of announcements to be acknowledged
 * 
 * @return HPI return code
 **/ 
SaErrorT NewSimulatorAnnunciator::SetAcknowledge( SaHpiEntryIdT num, SaHpiSeverityT severity) {
   
   for (int i = 0; i < m_anns.Num(); i++) {
      NewSimulatorAnnouncement *ann = m_anns[i];
      
      if (num != SAHPI_ENTRY_UNSPECIFIED) {
         if ( ann->EntryId() == num ) {
         	ann->SetAcknowledge( true );
            return SA_OK;
         }
      } else {
      	if ( (severity == SAHPI_ALL_SEVERITIES) ||
      	     (severity == ann->Severity()) ) 
      	   ann->SetAcknowledge( true );
      }
   }
   
   if (num != SAHPI_ENTRY_UNSPECIFIED)
      return SA_ERR_HPI_NOT_PRESENT;
      
   return SA_OK;
   
}
/**
 * HPI function saHpiAnnunciatorAdd()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param ann announcement to be added
 * 
 * @return HPI return code
 **/ 
SaErrorT NewSimulatorAnnunciator::AddAnnouncement( SaHpiAnnouncementT &ann ) {
      
   if (&ann == NULL)
      return SA_ERR_HPI_INVALID_PARAMS;
      
   if (m_mode == SAHPI_ANNUNCIATOR_MODE_AUTO)
      return SA_ERR_HPI_READ_ONLY;

   ann.AddedByUser = SAHPI_TRUE;
   oh_gettimeofday(&ann.Timestamp);
   ann.EntryId = ValidEntryId();
   
   NewSimulatorAnnouncement *ann_obj = new NewSimulatorAnnouncement( ann );
   if ( ann_obj == NULL)
      return SA_ERR_HPI_OUT_OF_SPACE;
   
   m_anns.Add( ann_obj );
   
   return SA_OK;
}


/**
 * HPI function saHpiAnnunciatorDelete()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param num entryId of announcement to be deleted
 * @param severity severity of announcements to be deleted
 * 
 * @return HPI return code
 **/ 
SaErrorT NewSimulatorAnnunciator::DeleteAnnouncement( SaHpiEntryIdT &num, 
                                                      SaHpiSeverityT &severity) {
   
   if (m_mode == SAHPI_ANNUNCIATOR_MODE_AUTO)
      return SA_ERR_HPI_READ_ONLY;
   
   for (int i = m_anns.Num() - 1; i >= 0; i--) {
      NewSimulatorAnnouncement *ann = m_anns[i];
      
      if (num != SAHPI_ENTRY_UNSPECIFIED) {
         if ( ann->EntryId() == num ) {
            m_anns.Rem( i );
            return SA_OK;
         }
      } else {
      	if ( (severity == SAHPI_ALL_SEVERITIES) ||
      	     (severity == ann->Severity()) ) 
      	   m_anns.Rem( i );
      }
   }
   
   if (num != SAHPI_ENTRY_UNSPECIFIED)
      return SA_ERR_HPI_NOT_PRESENT;
      
   return SA_OK;
}


/**
 * HPI function saHpiAnnunciatorModeGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param mode address of return value
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorAnnunciator::GetMode( SaHpiAnnunciatorModeT &mode ) {
    
    if (&mode == NULL)
       return SA_ERR_HPI_INVALID_PARAMS;
    
    mode = m_mode;

    return SA_OK;
}


/**
 * HPI function saHpiAnnunciatorModeSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param mode to be set
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorAnnunciator::SetMode( SaHpiAnnunciatorModeT mode ) {
    
   if ( m_ann_rec.ModeReadOnly == SAHPI_TRUE)
       return SA_ERR_HPI_READ_ONLY;

   if ((mode != SAHPI_ANNUNCIATOR_MODE_AUTO) &&
        (mode !=  SAHPI_ANNUNCIATOR_MODE_USER) &&
        (mode != SAHPI_ANNUNCIATOR_MODE_SHARED))
      return SA_ERR_HPI_INVALID_PARAMS;
      
   m_mode = mode;

   return SA_OK;
}
