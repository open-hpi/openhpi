/*      -*- java -*-
 *
 * Copyright (C) 2012, Pigeon Point Systems
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

package org.openhpi;

import java.util.Collection;
import java.util.LinkedList;

import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.OhpiDataTypes.*;
import static org.openhpi.Ohpi.*;


/**********************************************************
 * OHPI Utility Functions: Collections
 *********************************************************/
public class OhpiCollections
{
    // Just to ensure nobody creates it
    private OhpiCollections()
    {
        // empty
    }


    /***********************************************************
     * Handler Ids
     **********************************************************/
    public static Collection<Long> HandlerIds(
        long sid
    ) throws HpiException
    {
        LinkedList<Long> all = new LinkedList<Long>();

        long hid = SAHPI_FIRST_ENTRY;
        do {
            oHpiHandlerGetNextOutputParamsT out = new oHpiHandlerGetNextOutputParamsT();
            long rv = oHpiHandlerGetNext( sid, hid, out );
            if ( ( hid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            hid = out.NextHandlerId;
            all.add( hid );
        } while ( hid != SAHPI_LAST_ENTRY );

        return all;
    }


}; // class HpiCollections

