/** 
 * @file    new_sim_rdr.cpp
 *
 * The file includes an abstract class for rdr types:\n
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
 * Thanks to \n
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>\n
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *      
 */

#include "new_sim_rdr.h"
#include "new_sim_entity.h"
#include "new_sim_resource.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorRdr::NewSimulatorRdr( NewSimulatorResource *res, SaHpiRdrTypeT type )
                : m_resource( 0 ), m_type( type ), m_populate( false ) {
}


/**
 * Full qualified constructor
 **/
NewSimulatorRdr::NewSimulatorRdr( NewSimulatorResource *res, 
                                  SaHpiRdrTypeT type, 
                                  SaHpiEntityPathT entity, 
                                  SaHpiBoolT isFru, 
                                  SaHpiTextBufferT idString)
 : m_resource( 0 ), 
   m_type( type ),
   m_entity_path( entity ),
   m_is_fru( isFru ),
   m_id_string( idString ),
   m_populate( false ) {
}


/**
 * Destructor
 **/
NewSimulatorRdr::~NewSimulatorRdr() {}


/** 
 * Return a pointer on the own domain
 * 
 * @return pointer on NewSimulatorDomain
 **/
NewSimulatorDomain *NewSimulatorRdr::Domain() { 
	return m_resource->Domain();
}


/**
 * A rdr structure is filled with the internally data
 * 
 * This method is called by method NewSimulatorRdr::Populate().
 * 
 * @param resource Address of resource structure
 * @param rdr Address of rdr structure
 * 
 * @return true
 **/
bool NewSimulatorRdr::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   rdr.RecordId = m_record_id;
   rdr.RdrType  = m_type;
   rdr.Entity   = m_entity_path;
   rdr.IdString = m_id_string;

   return true;
}

/**
 * Add the Rdr to the rptcache 
 * 
 * In this function the object is added to the rptcache by calling oh_add_rdr and
 * afterwards append on the given list.
 * 
 * @param list Pointer on GSList
 * @return success
 **/
bool NewSimulatorRdr::Populate(GSList **list) {

   if ( m_populate )
      return true;

   // find resource
   SaHpiRptEntryT *resource = Domain()->FindResource( Resource()->ResourceId() );

   if ( !resource ) {
      stdlog << "Resource not found: Can't populate RDR !\n";
      return false;
   }

   // create rdr
   SaHpiRdrT *rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
   CreateRdr( *resource, *rdr );

   int rv = oh_add_rdr( Domain()->GetHandler()->rptcache,
                        resource->ResourceId,
                        rdr, this, 1 );

   if ( rv != 0 ) {
       stdlog << "Can't add RDR to plugin cache !\n";
       g_free( rdr );
       return false;
   }

   // assign the hpi record id to sensor, so we can find
   // the rdr for a given sensor.
   // the id comes from oh_add_rdr.
   RecordId() = rdr->RecordId;

   stdlog << "NewSimulatorRdr::Populate RDR for resource " << resource->ResourceId << " RDR " << RecordId() << "\n";
   *list = g_slist_append(*list, rdr);

   m_populate = true;

   return true;
}

