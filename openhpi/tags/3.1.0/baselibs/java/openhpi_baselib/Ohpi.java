package org.openhpi;

import java.io.InputStream;
import java.io.IOException;
import java.util.jar.Attributes;
import java.util.jar.Manifest;

import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.OhpiDataTypes.*;


/**********************************************************
 * OHPI API (NB: Partly implemented)
 *********************************************************/
public class Ohpi
{
    // Just to ensure nobody creates it
    protected Ohpi()
    {
        // empty
    }


    public static long oHpiVersionGet()
    {
        long vmajor = 0L;
        long vminor = 0L;
        long vaux   = 0L;

        String sversion = null;
        try {
            // Trying to get version from jar manifest
            InputStream s = Hpi.class.getResourceAsStream( "/META-INF/MANIFEST.MF" );
            if ( s != null ) {
                Manifest m = new Manifest( s );
                Attributes a = m.getAttributes( "org.openhpi" );
                if ( a != null ) {
                    sversion = a.getValue( Attributes.Name.IMPLEMENTATION_VERSION );
                }
            }
        } catch ( IOException e ) {
            // do nothing
        }

        if ( sversion != null ) {
            String[] parts = sversion.split( "\\." );
            if ( parts.length >= 0 ) {
                vmajor = Long.parseLong( parts[0] );
            }
            if ( parts.length >= 1 ) {
                vminor = Long.parseLong( parts[1] );
            }
            if ( parts.length >= 2 ) {
                vaux = Long.parseLong( parts[2] );
            }
        }

        return ( vmajor << 48 ) | ( vminor << 32 ) | ( vaux << 16 );
    }

    public static long oHpiDomainAdd(
        SaHpiTextBufferT Host,
        int Port,
        SaHpiEntityPathT EntityRoot,
        oHpiDomainAddOutputParamsT out
    )
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        String s = HpiUtil.fromSaHpiTextBufferT( Host );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiDomain d = HpiCore.createDomain( s, Port, EntityRoot );
        if ( d == null ) {
            return SA_ERR_HPI_INTERNAL_ERROR;
        }

        out.DomainId = d.getLocalDid();

        return SA_OK;
    }

    // TODO other OHPI API
};

