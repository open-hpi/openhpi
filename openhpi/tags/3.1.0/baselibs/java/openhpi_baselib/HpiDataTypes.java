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

