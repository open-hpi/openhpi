#include <glib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <oh_utils.h>
#include <el_utils.h>


#include "el_test.h"

/**
 * main: EL test 
 *
 * This negative test sets el equal to null and 
 * then verifies oh_el_close fails.
 *
 * Return value: 0 on success, 1 on failure
 **/


int main(int argc, char **argv)
{
        oh_el *el;
        SaErrorT retc;

	/* create a null EL*/
	el = NULL;

        /* close el without saving to file*/
        retc = oh_el_close(el);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_close on el failed.");
                return 1;
        }

        return 0;
}
