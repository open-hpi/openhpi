using System;
using System.Collections.Generic;


namespace org {
namespace openhpi {


/**********************************************************
 * OHPI Utility Functions: Iterators
 *********************************************************/
public static class OhpiIterators
{

    /**********************************************************
     * Iterate over Handler ids
     *********************************************************/
    public static IEnumerable<long> HandlerIds( long sid )
    {
        long hid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_hid;
        do {
            long rv = Api.oHpiHandlerGetNext( sid, hid, out next_hid );
            if ( ( hid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            hid = next_hid;
            yield return hid;
        }  while ( hid != HpiConst.SAHPI_LAST_ENTRY );
    }


}; // class OhpiIterators


}; // namespace openhpi
}; // namespace org

