 #include <glib.h>
 #include <string.h>
 
 #include <SaHpi.h>
 #include <oh_utils.h>
 
 #include "el_test.h"
 
 /* function to compare two event logs */
 
 int el_compare(oh_el *el1, oh_el *el2){
 

        oh_el_entry *entry1, *entry2; 
	SaHpiEventLogEntryIdT prev1, prev2, next1, next2;
	SaHpiRptEntryT res1, res2;
	SaHpiRdrT rdr1, rdr2;
 	SaErrorT retc;
 
        if(g_list_length(el1->elentries) != g_list_length(el2->elentries)) {
                 dbg("ERROR: el1->elentries != el2->elentries.");
                 return 1;
         }

	if((g_list_length(el1->elentries) == 0) && (g_list_length(el2->elentries) == 0)){
		return 0;
	}

	entry1 = (oh_el_entry *)(g_list_first(el1->elentries)->data);
	entry2 = (oh_el_entry *)(g_list_first(el2->elentries)->data);

        do{
	    res1 = entry1->res;
	    res2 = entry2->res;
	    rdr1 = entry1->rdr;
	    rdr2 = entry2->rdr;
	
        
	    /* fetch the event for el1*/
            retc = oh_el_get(el1, entry1->event.EntryId, &prev1, &next1, &entry1);
            if (retc != SA_OK) {
                     dbg("ERROR: oh_el_get failed.");
                     return 1;
            }
 
            /* fetch the event for el2*/
            retc = oh_el_get(el2, entry2->event.EntryId, &prev2, &next2, &entry2);
            if (retc != SA_OK) {
                    dbg("ERROR: oh_el_get failed.");
                    return 1;
            }
  

            if (memcmp(&entry1->event.Event, &entry2->event.Event, sizeof(SaHpiEventT))) {
                     dbg("ERROR: Data from el1 and el2 do not match");
                     return 1;
            }
   
       	    /* Compare resource from el1 and el2 */
            if (memcmp(&entry1->res, &entry2->res, sizeof(SaHpiRptEntryT))) {
                     dbg("ERROR: Res from el1 and el2 do not match.");
                     return 1;
            }
 
 	    /* Compare rdr from el1 and el2 */
            if (memcmp(&entry1->rdr, &entry2->rdr, sizeof(SaHpiRdrT))) {
                     dbg("ERROR: Rdr from el1 and el2 do not match.");
                     return 1;
            }
 
	    /* Get ready for next iteration */
	    entry1->event.EntryId = next1;
	    entry2->event.EntryId = next2;

        } while (next1 != SAHPI_NO_MORE_ENTRIES);

        return 0;
}
 
