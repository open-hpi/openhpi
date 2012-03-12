using System;
using System.Reflection;


namespace org {
namespace openhpi {


/**********************************************************
 * OHPI API (NB: Partly implemented)
 *********************************************************/
public static partial class Api
{
    public static long oHpiVersionGet()
    {
        Version v = Assembly.GetExecutingAssembly().GetName().Version;
        long vmajor = unchecked( (ushort)( v.Major & 0xFFFF ) );
        long vminor = unchecked( (ushort)( v.Minor & 0xFFFF ) );
        long vaux   = unchecked( (ushort)( v.Build & 0xFFFF ) );

        return ( vmajor << 48 ) | ( vminor << 32 ) | ( vaux << 16 );
    }

    public static long oHpiDomainAdd(
        SaHpiTextBufferT Host,
        int Port,
        SaHpiEntityPathT EntityRoot,
        out long DomainId
    )
    {
        DomainId = 0;

        String s = HpiUtil.FromSaHpiTextBufferT( Host );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiDomain d = HpiCore.CreateDomain( s, Port, EntityRoot );
        if ( d == null ) {
            return HpiConst.SA_ERR_HPI_INTERNAL_ERROR;
        }

        DomainId = d.GetLocalDid();

        return HpiConst.SA_OK;
    }

    // TODO other OHPI API
};


}; // namespace openhpi
}; // namespace org

