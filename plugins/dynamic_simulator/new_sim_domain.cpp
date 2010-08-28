/** 
 * @file    new_sim_domain.cpp
 *
 * The file includes the abstract domain class:\n
 * NewSimulatorDomain
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

 
#include <errno.h>

#include "new_sim.h"
#include "new_sim_utils.h"
#include "new_sim_file.h"

/** 
 * Constructor 
 **/
NewSimulatorDomain::NewSimulatorDomain()
                   : m_file( 0 ), 
                     m_major_version( 0 ), 
                     m_minor_version( 0 ), 
                     m_initial_discover( 0 ) {
    	
   m_did = 0;
   m_own_domain = false;
   m_running_wdt = false;
   m_running_fumi = false;
   m_insert_timeout = SAHPI_TIMEOUT_IMMEDIATE;
   m_extract_timeout = SAHPI_TIMEOUT_IMMEDIATE;
   
   stdlog << "DBG: NewSimulatorDomain.Constructor is called\n";
}


/** 
 * Destructor 
 **/
NewSimulatorDomain::~NewSimulatorDomain() {
}


/** 
 * Initialization and filling the domain with resources and rdr by
 * calling NewSimulatorFile::Discover(). 
 *
 * @todo Here the domain is set hard coded to '0'. How should this be done for more than one
 * domain ...? 
 * 
 * @param file pointer to NewSimulatorfile which includes system information
 * @return true
 **/
bool NewSimulatorDomain::Init( NewSimulatorFile *file ) {
   
   stdlog << "DBG: We are inside NewSimulatorDomain::Init\n";
   
   if ( m_file != 0 ) {
      stdlog << "New Simulator Domain already initialized !\n";
      return false;
   }

   m_file = file;
   file->Discover( this );

   m_did = 0;

   stdlog << "Domain ID " << m_did << "\n";
   Dump( stdlog );
   
   return true;
}


/** 
 * Ceanup all resources in the domain 
 **/
void NewSimulatorDomain::Cleanup() {
  int i;

  // cleanup all Resources
  for( i = m_resources.Num() - 1; i >= 0; i-- ) {
       NewSimulatorResource *res = m_resources[i];
       CleanupResource( res );
  }

  // now all resources are ready to destroy
   while( m_resources.Num() ) {
      NewSimulatorResource *res = m_resources[0];
      CleanupResource( res );
   }

}


/** 
 * Verify Resource 
 *
 * @param res pointer on a NewSimulatorResource to be verified
 * @return  res pointer for successful verification
 **/
NewSimulatorResource *NewSimulatorDomain::VerifyResource( NewSimulatorResource *res ) {

   stdlog << "DBG: VerifyResource \n";
   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *m_res = m_resources[i];
      if ( res == m_res ) return res;
   }

   return 0;
}


/** 
 * Verify Rdr 
 *
 * @param rdr pointer on a NewSimulatorRdr to be verified
 * @return  rdr pointer for successful verification
 **/
NewSimulatorRdr *NewSimulatorDomain::VerifyRdr( NewSimulatorRdr *rdr ) {
   stdlog << "DBG: VerifyRdr \n";
   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *m_res = m_resources[i];
      if ( m_res->FindRdr( rdr ) >= 0 ) return rdr;
   }

   return 0;
}


/** 
 * Verify Sensor 
 *
 * @param s pointer on a NewSimulatorSensor to be verified
 * @return  s pointer for successfull verification
 **/
NewSimulatorSensor *NewSimulatorDomain::VerifySensor( NewSimulatorSensor *s ) {
   stdlog << "DBG: VerifySensor \n";
   for( int i = 0; i < m_resources.Num(); i++ ) {
       NewSimulatorResource *m_res = m_resources[i];
       if ( m_res->FindRdr( s ) >= 0 ) return s;
   }

   return 0;
}

/** 
 * Verify Control 
 *
 * @param c pointer on a NewSimulatorControl to be verified
 * @return  c pointer for successfull verification
 **/
NewSimulatorControl *NewSimulatorDomain::VerifyControl( NewSimulatorControl *c ) {
  
   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *m_res = m_resources[i];
      if ( m_res->FindRdr( c ) >= 0 ) return c;
   }

   return 0;
}


/** 
 * Verify Annunciator 
 *
 * @param a pointer on a NewSimulatorAnnunciator to be verified
 * @return  a pointer for successfull verification
 **/
NewSimulatorAnnunciator *NewSimulatorDomain::VerifyAnnunciator( NewSimulatorAnnunciator *a ) {
   stdlog << "DBG: VerifyAnnunciator \n";
   for( int i = 0; i < m_resources.Num(); i++ ) {
       NewSimulatorResource *m_res = m_resources[i];
       if ( m_res->FindRdr( a ) >= 0 ) return a;
   }

   return 0;
}


/** 
 * Verify Watchdog
 *
 * @param c pointer on a NewSimulatorInventory object to be verified
 * @return c pointer for successfull verification
 **/
NewSimulatorWatchdog *NewSimulatorDomain::VerifyWatchdog ( NewSimulatorWatchdog *c ) {

   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *m_res = m_resources[i];
      if ( m_res->FindRdr( c ) >= 0 ) return c;
   }

   return 0;
}


/** 
 * Verify Fumi
 *
 * @param f pointer on a NewSimulatorFumi object to be verified
 * @return f pointer for successfull verification
 **/
NewSimulatorFumi *NewSimulatorDomain::VerifyFumi ( NewSimulatorFumi *f ) {

   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *m_res = m_resources[i];
      if ( m_res->FindRdr( f ) >= 0 ) return f;
   }

   return 0;
}


/** 
 * Verify Dimi
 *
 * @param d pointer on a NewSimulatorDimi object to be verified
 * @return d pointer for successfull verification
 **/
NewSimulatorDimi *NewSimulatorDomain::VerifyDimi ( NewSimulatorDimi *d ) {

   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *m_res = m_resources[i];
      if ( m_res->FindRdr( d ) >= 0 ) return d;
   }

   return 0;
}


/** 
 * Verify Inventory
 *
 * @param inv pointer on a NewSimulatorInventory object to be verified
 * @return inv pointer for successfull verification
 **/
NewSimulatorInventory *NewSimulatorDomain::VerifyInventory( NewSimulatorInventory *inv ) {

   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *m_res = m_resources[i];
      if ( m_res->FindRdr( inv ) >= 0 ) return inv;
   }

   return 0;
}


/**
 * Dump some information about the resources hold in the domain
 * 
 * For each resource in the array the dump method is called.
 * 
 * @param dump Address of the NewSimulatorlog in which the dump should be written
 **/
void NewSimulatorDomain::Dump( NewSimulatorLog &dump ) const {

   dump << "Dump of NewSimulatorDomain is called\n" ;
   dump << "Count of resources: " << m_resources.Num() << "\n";
  
   for (int i = 0; i < m_resources.Num(); i++) {
     NewSimulatorResource *m_res = m_resources[i];
     m_res->Dump( dump );
   }
}


/**
 * Cleanup one resource
 * 
 * @param res Pointer on NewSimulorResource to be cleaned up
 * @return true if it was found and was deleted
 **/
bool NewSimulatorDomain::CleanupResource( NewSimulatorResource *res ) {
   
   if ( !res->Destroy() ) return false;

   int idx = m_resources.Find( res );

   if ( idx == -1 ) {
       stdlog << "unable to find resource at " << idx << " in resources list !\n";
       return false;
   }

   m_resources.Rem( idx );
   delete res;

   return true;
}


/**
 * Get Resource
 * 
 * @param i with the number of the resource 
 * @return pointer on the resource which was found 
 **/
NewSimulatorResource *NewSimulatorDomain::GetResource( int i ) {

   if ( i >= m_resources.Num() )
      return 0;

   NewSimulatorResource *res = m_resources[i];

   return res;
}


/**
 * Find resource by resource pointer
 * 
 * @param res pointer on the resource to be found 
 * @return return the same pointer if it could be found successfully 
 **/
NewSimulatorResource *NewSimulatorDomain::FindResource( NewSimulatorResource *res ) {
	
   for( int i = 0; i < m_resources.Num(); i++ ) {
      NewSimulatorResource *r = m_resources[i];
      if ( r == res ) return res;
   }

   return 0;
}


/**
 * Find resource by entity path
 * 
 * @param ep address of entity path to be found 
 * @return return pointer on resource if it could be found successfully 
 **/
NewSimulatorResource *NewSimulatorDomain::FindResource( const NewSimulatorEntityPath &ep ) {
   for( int i = 0; i < m_resources.Num(); i++ ) {
   	  NewSimulatorResource *res = m_resources[i];
      if ( res->EntityPath() == ep ) return res;
   }

   return 0;
}


/**
 * Add a resource to the domain
 * 
 * @param res pointer to the resource 
 **/
void NewSimulatorDomain::AddResource( NewSimulatorResource *res ) {
  
   if ( FindResource( res ) ) {
      assert( 0 );
      return;
   }

   m_resources.Add( res );
}


/**
 * Remove a resource from the domain
 * 
 * @param res pointer to the resource 
 **/

void NewSimulatorDomain::RemResource( NewSimulatorResource *res ) {
  
   int idx = m_resources.Find( res );

   if ( idx == -1 ) {
      assert( 0 );
      return;
   }

   m_resources.Rem( idx );
}
