/** 
 * @file    new_sim_rdr.h
 *
 * The file includes an abstract class for rdr data:\n
 * NewSimulatorRdr
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
 
#ifndef __NEW_SIM_RDR_H__
#define __NEW_SIM_RDR_H__


#ifndef __NEW_SIM_TEXT_BUFFER_H__
#include "new_sim_text_buffer.h"
#endif

#include <glib.h>

extern "C" {
#include "SaHpi.h"
}


class NewSimulatorResource;


#ifndef __NEW_SIM_ENTITY_H__
#include "new_sim_entity.h"
#endif

class NewSimulatorDomain;


/**
 * @class NewSimulatorRdr
 * 
 * Abstract class for rdr data
 * 
 **/
class NewSimulatorRdr {

protected:
   /// Holds a reference on the resource for which the rdr is defined
   NewSimulatorResource   *m_resource;
   /// id of the rdr entry
   SaHpiEntryIdT          m_record_id;
   /// type of the rdr entry
   SaHpiRdrTypeT          m_type;
   /// entity path where it can be found
   NewSimulatorEntityPath m_entity_path;
   /// isFru flag - please see also specification about the usage of this flag
   SaHpiBoolT             m_is_fru;
   /// id string with the name of the entry
   NewSimulatorTextBuffer m_id_string;


public:
  NewSimulatorRdr( NewSimulatorResource *res, SaHpiRdrTypeT type );
  NewSimulatorRdr( NewSimulatorResource *res, SaHpiRdrTypeT type, 
                   SaHpiEntityPathT entity, SaHpiBoolT isFru, SaHpiTextBufferT idString);
  virtual ~NewSimulatorRdr();

  /// returns a reference of the resource reference from the class
  NewSimulatorResource    *&Resource()  { return m_resource; }
  /// returns a reference of the record id  
  SaHpiEntryIdT           &RecordId()   { return m_record_id; }
  /// returns a reference of the rdr type
  SaHpiRdrTypeT           &Type()       { return m_type; }
  /// returns a reference of the isFru variable
  SaHpiBoolT              &IsFru()      { return m_is_fru; }
  /// returns a reference of the idString
  NewSimulatorTextBuffer  &IdString()   { return m_id_string; }
  /// returns a reference of the idString
  const NewSimulatorTextBuffer &IdString() const { return m_id_string; }
  /// returns a reference of theEntity Path
  NewSimulatorEntityPath  &EntityPath() { return m_entity_path; }
  
  NewSimulatorDomain      *Domain();

  /// create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  /// sensor num, control num, fru device id
  virtual unsigned int Num() const = 0;
  /// Dump the internal data
  virtual void Dump( NewSimulatorLog &dump ) const = 0;

  
private:
  /// Is the rdr entry populated
  bool m_populate;

public:
  virtual bool Populate(GSList **);
};


#endif
