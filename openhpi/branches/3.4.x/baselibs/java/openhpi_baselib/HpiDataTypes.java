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


public class HpiDataTypes extends HpiDataTypesGen
{
    // Just to ensure nobody creates it
    protected HpiDataTypes()
    {
        // empty
    }

    /**********************************************************
     * HPI API Returns Types
     *********************************************************/

    /**
     * Represents output parameters
     * for saHpiSessionOpen().
     */
    public static class saHpiSessionOpenOutputParamsT
    {
        public long SessionId;
    };

    /**
     * Represents output parameters
     * for saHpiMyEntityPathGet().
     */
    public static class saHpiMyEntityPathGetOutputParamsT
    {
        public SaHpiEntityPathT EntityPath;
    };

};

