using System;


namespace org {
namespace openhpi {


public class HpiException : Exception
{
    public HpiException( string message )
        : base( message )
    {
        // empty
    }
};


}; // namespace openhpi
}; // namespace org

