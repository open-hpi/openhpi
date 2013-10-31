/*      -*- linux-c -*-
 *
 * Copyright (c) 2008 Pigeon Point Systems.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *    Anton Pak    <anton.pak@pigeonpoint.com>
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hpi_ui.h"
#include "hpi_cmd.h"

typedef struct {
    SaHpiResourceIdT     rptid;
    SaHpiFumiNumT        fuminum;
    SaHpiFumiCapabilityT fumicaps;
} fumi_block_env_t;

static fumi_block_env_t fumi_block_env;

static ret_code_t get_banknum_prompt( char * prompt, SaHpiBankNumT * banknum )
{
    int i, res;

    i = get_int_param( prompt, &res );
    if ( i != 1 ) {
        printf( "Error!!! Invalid Bank Num\n" );
        return HPI_SHELL_PARM_ERROR ;
    }
    *banknum = (SaHpiBankNumT)res;
    return HPI_SHELL_OK;
}

static ret_code_t get_banknum( SaHpiBankNumT * banknum )
{
    return get_banknum_prompt( "Bank Num(0 == active bank): ", banknum );
}

static ret_code_t get_position( SaHpiUint32T * position )
{
    int i, res;

    i = get_int_param( "Position of the bank in boot order: ", &res );
    if ( i != 1 ) {
        printf( "Error!!! Invalid position\n" );
        return HPI_SHELL_PARM_ERROR ;
    }
    *position = (SaHpiUint32T)res;
    return HPI_SHELL_OK;
}

static ret_code_t get_uri( SaHpiTextBufferT* uri )
{
    int i;

    i = get_string_param("Source URI: ", (char*)(uri->Data), SAHPI_MAX_TEXT_BUFFER_LENGTH);
    if ( i != 0 ) {
        printf( "Error!!! Invalid string: %s\n", uri->Data );
        return HPI_SHELL_PARM_ERROR ;
    }
    uri->DataType   = SAHPI_TL_TYPE_TEXT;
    uri->Language   = SAHPI_LANG_ENGLISH;
    uri->DataLength = strlen( (char*)(uri->Data) );
    return HPI_SHELL_OK;
}

static void show_fw_info( const char * fwname,
                          const char * indent,
                          SaHpiFumiFirmwareInstanceInfoT * fwinfo )
{
    printf( "%s", indent );
    printf( "%s Present: %s\n", fwname, ( fwinfo->InstancePresent == SAHPI_FALSE ) ? "FALSE" : "TRUE" );
    if ( fwinfo->InstancePresent != SAHPI_FALSE ) {
        printf( "%s    ", indent );
        print_text_buffer_text( "Identifier: \"",
                                &fwinfo->Identifier,
                                "\"\n",
                                ui_print );
        printf( "%s    ", indent );
        print_text_buffer_text( "Description: \"",
                                &fwinfo->Description,
                                "\"\n",
                                ui_print );
        printf( "%s    ", indent );
        print_text_buffer_text( "DateTime: \"",
                                &fwinfo->DateTime,
                                "\"\n",
                                ui_print );
        printf( "%s    ", indent );
        printf( "Version: %u.%u.%u\n", fwinfo->MajorVersion, fwinfo->MinorVersion, fwinfo->AuxVersion );
    }
}

static void show_component_info( const char * indent, SaHpiFumiComponentInfoT * cinfo )
{
    char fw_info_indent[256];

    printf( "%s", indent );
    printf( "Component %u\n", cinfo->ComponentId );
    strncpy( fw_info_indent, indent, sizeof(fw_info_indent) );
    strncat( fw_info_indent, "    ", sizeof(fw_info_indent) );
    show_fw_info( "Main Instance", fw_info_indent, &cinfo->MainFwInstance );
    printf( "%s    ", indent );
    printf( "ComponentFlags 0x%x\n", cinfo->ComponentFlags );
}


static ret_code_t show_spec_info( SaHpiSessionIdT sessionId,
                                  SaHpiResourceIdT rptid,
                                  SaHpiFumiNumT fuminum )
{
    SaErrorT rv;
    SaHpiFumiSpecInfoT info;

    rv = saHpiFumiSpecInfoGet( sessionId, rptid, fuminum, &info );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiSpecInfoGet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }

    printf( "SpecInfo Type: %s\n", oh_lookup_fumispecinfotype( info.SpecInfoType ) );
    switch ( info.SpecInfoType ) {
        case SAHPI_FUMI_SPEC_INFO_NONE:
            break;
        case SAHPI_FUMI_SPEC_INFO_SAF_DEFINED:
            printf( "    Spec ID: %s\n",
                    oh_lookup_fumisafdefinedspecid( info.SpecInfoTypeUnion.SafDefined.SpecID ) );
            printf( "    Revision ID: %u\n", info.SpecInfoTypeUnion.SafDefined.RevisionID );
            break;
        case SAHPI_FUMI_SPEC_INFO_OEM_DEFINED:
            {
                unsigned int i;
                SaHpiTextBufferT tb_mid;

                rv = oh_decode_manufacturerid( info.SpecInfoTypeUnion.OemDefined.Mid, &tb_mid );
                if ( rv != SA_OK ) {
                    printf( "ERROR!!! oh_decode_manufacturerid: %s\n", oh_lookup_error( rv ) );
                    return HPI_SHELL_CMD_ERROR;
                }
                print_text_buffer_text( "    Manufacturer ID: ",
                                        &tb_mid,
                                       "\n",
                                       ui_print );
                printf( "    Body:" );
                for ( i = 0; i < info.SpecInfoTypeUnion.OemDefined.BodyLength; ++i ) {
                    printf( " %02x", info.SpecInfoTypeUnion.OemDefined.Body[i] );
                }
                printf( " (len=%u)\n", i );
            }
            break;
        default:
            printf( "Unknown SpecInfo Type\n" );
            return HPI_SHELL_CMD_ERROR;
    }

    return HPI_SHELL_OK;
}

static ret_code_t show_service_impact( SaHpiSessionIdT sessionId,
                                       SaHpiResourceIdT rptid,
                                       SaHpiFumiNumT fuminum )
{
    unsigned int i;
    SaErrorT rv;
    SaHpiFumiServiceImpactDataT si;

    rv = saHpiFumiServiceImpactGet( sessionId, rptid, fuminum, &si );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiServiceImpactGet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }

    if ( si.NumEntities == 0 ) {
        printf( "No Service Impact\n" );
        return HPI_SHELL_OK;
    }

    printf( "Service Impact:\n" );
    for ( i = 0; i < si.NumEntities && i < SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
        oh_big_textbuffer tmpbuf;

        oh_init_bigtext(&tmpbuf);
        rv = oh_decode_entitypath( &si.ImpactedEntities[i].ImpactedEntity, &tmpbuf );
        if ( rv != SA_OK ) {
            printf( "ERROR!!! oh_decode_entitypath: %s\n", oh_lookup_error( rv ) );
            return HPI_SHELL_CMD_ERROR;
        }
        printf( "    " );
        fwrite( tmpbuf.Data, tmpbuf.DataLength, 1, stdout );
        printf( ": %s\n", oh_lookup_fumiserviceimpact( si.ImpactedEntities[i].ServiceImpact ) );
    }
    return HPI_SHELL_OK;
}
static ret_code_t set_source( SaHpiSessionIdT sessionId,
                              SaHpiResourceIdT rptid,
                              SaHpiFumiNumT fuminum,
                              SaHpiBankNumT banknum,
                              SaHpiTextBufferT* uri )
{
    SaErrorT rv;

    rv = saHpiFumiSourceSet( sessionId, rptid, fuminum, banknum, uri );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiSourceSet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t start_source_validation( SaHpiSessionIdT sessionId,
                                           SaHpiResourceIdT rptid,
                                           SaHpiFumiNumT fuminum,
                                           SaHpiBankNumT banknum )
{
    SaErrorT rv;

    rv = saHpiFumiSourceInfoValidateStart( sessionId, rptid, fuminum, banknum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiSourceInfoValidateStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t show_source_info( SaHpiSessionIdT sessionId,
                                    SaHpiResourceIdT rptid,
                                    SaHpiFumiNumT fuminum,
                                    SaHpiBankNumT banknum )
{
    SaErrorT rv;
    SaHpiFumiSourceInfoT info;

    rv = saHpiFumiSourceInfoGet( sessionId, rptid, fuminum, banknum, &info );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiSourceInfoGet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }

    printf( "Bank Num: %d\n", banknum );
    print_text_buffer_text( "    Source URI: \"", &info.SourceUri, "\"\n", ui_print );
    printf( "    Source status: %s\n", oh_lookup_fumisourcestatus(  info.SourceStatus ) );
    print_text_buffer_text( "    Identifier: \"", &info.Identifier, "\"\n", ui_print );
    print_text_buffer_text( "    Description: \"", &info.Description, "\"\n", ui_print );
    print_text_buffer_text( "    DateTime: \"", &info.DateTime, "\"\n", ui_print );
    printf( "    Version: %u.%u.%u\n", info.MajorVersion, info.MinorVersion, info.AuxVersion );

    return HPI_SHELL_OK;
}

static ret_code_t show_source_component_info( SaHpiSessionIdT sessionId,
                                              SaHpiResourceIdT rptid,
                                              SaHpiFumiNumT fuminum,
                                              SaHpiBankNumT banknum )
{
    SaErrorT rv;
    SaHpiEntryIdT id = SAHPI_FIRST_ENTRY, next_id;
    SaHpiFumiComponentInfoT cinfo;

    while ( id != SAHPI_LAST_ENTRY ) {
        rv = saHpiFumiSourceComponentInfoGet( sessionId, rptid, fuminum,
                                              banknum, id, &next_id, &cinfo );
        if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
            break;
        } else if ( rv != SA_OK ) {
            printf( "ERROR!!! saHpiFumiSourceComponentInfoGet: %s\n", oh_lookup_error( rv ) );
            return HPI_SHELL_CMD_ERROR;
        }
        show_component_info( "    ", &cinfo );

        id = next_id;
    }

    return HPI_SHELL_OK;
}

static ret_code_t show_bank_info( SaHpiSessionIdT sessionId,
                                    SaHpiResourceIdT rptid,
                                    SaHpiFumiNumT fuminum,
                                    SaHpiBankNumT banknum )
{
    SaErrorT rv;
    SaHpiFumiBankInfoT info;

    rv = saHpiFumiTargetInfoGet( sessionId, rptid, fuminum, banknum, &info );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiTargetInfoGet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }

    printf( "Bank Num: %d\n", banknum );
    printf( "    Bank id: %u\n", (unsigned int)info.BankId );
    printf( "    Bank size: %u KB\n", info.BankSize );
    printf( "    Position: %u\n", info.Position );
    printf( "    Bank state: %s\n", oh_lookup_fumibankstate( info.BankState ) );
    print_text_buffer_text( "    Identifier: \"", &info.Identifier, "\"\n", ui_print );
    print_text_buffer_text( "    Description: \"", &info.Description, "\"\n", ui_print );
    print_text_buffer_text( "    DateTime: \"", &info.DateTime, "\"\n", ui_print );
    printf( "    Version: %u.%u.%u\n", info.MajorVersion, info.MinorVersion, info.AuxVersion );

    return HPI_SHELL_OK;
}

static ret_code_t show_logical_bank_info( SaHpiSessionIdT sessionId,
                                          SaHpiResourceIdT rptid,
                                          SaHpiFumiNumT fuminum )
{
    SaErrorT rv;
    SaHpiFumiLogicalBankInfoT linfo;

    rv = saHpiFumiLogicalTargetInfoGet( sessionId, rptid, fuminum, &linfo );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiLogicalTargetInfoGet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }

    printf( "Logical Bank Information:\n" );
    printf( "    Firmware Persistent Location Count: %u\n", linfo.FirmwarePersistentLocationCount );
    printf( "    Bank State Flags:" );
    if ( linfo.BankStateFlags & SAHPI_FUMI_NO_MAIN_PERSISTENT_COPY ) {
        printf( "NO_MAIN_PERSISTENT_COPY" );
    }
    printf( "\n" );
    show_fw_info( "Pending Instance", "    ", &linfo.PendingFwInstance );
    show_fw_info( "Rollback Instance", "    ", &linfo.RollbackFwInstance );

    return HPI_SHELL_OK;
}

static ret_code_t show_bank_component_info( SaHpiSessionIdT sessionId,
                                            SaHpiResourceIdT rptid,
                                            SaHpiFumiNumT fuminum,
                                            SaHpiBankNumT banknum )
{
    SaErrorT rv;
    SaHpiEntryIdT id = SAHPI_FIRST_ENTRY, next_id;
    SaHpiFumiComponentInfoT cinfo;

    while ( id != SAHPI_LAST_ENTRY ) {
        rv = saHpiFumiTargetComponentInfoGet( sessionId, rptid, fuminum,
                                              banknum, id, &next_id, &cinfo );
        if ( rv != SA_OK ) {
            printf( "ERROR!!! saHpiFumiTargetComponentInfoGet: %s\n", oh_lookup_error( rv ) );
            return HPI_SHELL_CMD_ERROR;
        }

        show_component_info( "    ", &cinfo );

        id = next_id;
    }

    return HPI_SHELL_OK;
}

static ret_code_t show_logical_bank_component_info( SaHpiSessionIdT sessionId,
                                                    SaHpiResourceIdT rptid,
                                                    SaHpiFumiNumT fuminum )
{
    SaErrorT rv;
    SaHpiEntryIdT id = SAHPI_FIRST_ENTRY, next_id;

    SaHpiFumiLogicalComponentInfoT lcinfo;

    while ( id != SAHPI_LAST_ENTRY ) {
        rv = saHpiFumiLogicalTargetComponentInfoGet( sessionId, rptid, fuminum, id, &next_id, &lcinfo );
        if ( rv != SA_OK ) {
            printf( "ERROR!!! saHpiFumiLogicalTargetComponentInfoGet: %s\n", oh_lookup_error( rv ) );
            return HPI_SHELL_CMD_ERROR;
        }

        printf( "    Component %u\n", lcinfo.ComponentId );
        show_fw_info( "Pending Instance", "        ", &lcinfo.PendingFwInstance );
        show_fw_info( "Rollback Instance", "        ", &lcinfo.RollbackFwInstance );
        printf( "        ComponentFlags 0x%x\n", lcinfo.ComponentFlags );

        id = next_id;
    }

    return HPI_SHELL_OK;
}

static ret_code_t start_backup( SaHpiSessionIdT sessionId,
                                SaHpiResourceIdT rptid,
                                SaHpiFumiNumT fuminum )
{
    SaErrorT rv;

    rv = saHpiFumiBackupStart( sessionId, rptid, fuminum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiBackupStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t set_bank_boot_order( SaHpiSessionIdT sessionId,
                                       SaHpiResourceIdT rptid,
                                       SaHpiFumiNumT fuminum,
                                       SaHpiBankNumT banknum,
                                       SaHpiUint32T position )
{
    SaErrorT rv;

    rv = saHpiFumiBankBootOrderSet( sessionId, rptid, fuminum, banknum, position );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiBankBootOrderSet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t start_bank_copying( SaHpiSessionIdT sessionId,
                                      SaHpiResourceIdT rptid,
                                      SaHpiFumiNumT fuminum,
                                      SaHpiBankNumT srcbanknum,
                                      SaHpiBankNumT dstbanknum )
{
    SaErrorT rv;

    rv = saHpiFumiBankCopyStart( sessionId, rptid, fuminum, srcbanknum, dstbanknum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiBankCopyStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t start_install( SaHpiSessionIdT sessionId,
                                 SaHpiResourceIdT rptid,
                                 SaHpiFumiNumT fuminum,
                                 SaHpiBankNumT banknum )
{
    SaErrorT rv;

    rv = saHpiFumiInstallStart( sessionId, rptid, fuminum, banknum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiInstallStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t show_upgrade_status( SaHpiSessionIdT sessionId,
                                       SaHpiResourceIdT rptid,
                                       SaHpiFumiNumT fuminum,
                                       SaHpiBankNumT banknum )
{
    SaErrorT rv;
    SaHpiFumiUpgradeStatusT status;

    rv = saHpiFumiUpgradeStatusGet( sessionId, rptid, fuminum, banknum, &status );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiUpgradeStatusGet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }

    printf( "Upgrade status on bank %d: %s\n", banknum, oh_lookup_fumiupgradestatus( status ) );

    return HPI_SHELL_OK;
}

static ret_code_t start_target_verification( SaHpiSessionIdT sessionId,
                                             SaHpiResourceIdT rptid,
                                             SaHpiFumiNumT fuminum,
                                             SaHpiBankNumT banknum )
{
    SaErrorT rv;

    rv = saHpiFumiTargetVerifyStart( sessionId, rptid, fuminum, banknum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiTargetVerifyStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t start_main_target_verification( SaHpiSessionIdT sessionId,
                                                  SaHpiResourceIdT rptid,
                                                  SaHpiFumiNumT fuminum )
{
    SaErrorT rv;

    rv = saHpiFumiTargetVerifyMainStart( sessionId, rptid, fuminum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiTargetVerifyMainStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t cancel_upgrade( SaHpiSessionIdT sessionId,
                                  SaHpiResourceIdT rptid,
                                  SaHpiFumiNumT fuminum,
                                  SaHpiBankNumT banknum )
{
    SaErrorT rv;

    rv = saHpiFumiUpgradeCancel( sessionId, rptid, fuminum, banknum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiUpgradeCancel: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t show_autorollback_disable( SaHpiSessionIdT sessionId,
                                             SaHpiResourceIdT rptid,
                                             SaHpiFumiNumT fuminum )
{
    SaErrorT rv;
    SaHpiBoolT disable;

    rv = saHpiFumiAutoRollbackDisableGet( sessionId, rptid, fuminum, &disable );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiAutoRollbackDisableGet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    printf( "Auto Rollback Disabled: %s\n", ( disable == SAHPI_FALSE ) ? "FALSE" : "TRUE" );

    return HPI_SHELL_OK;
}

static ret_code_t set_autorollback_disable( SaHpiSessionIdT sessionId,
                                            SaHpiResourceIdT rptid,
                                            SaHpiFumiNumT fuminum,
                                            SaHpiBoolT disable )
{
    SaErrorT rv;

    rv = saHpiFumiAutoRollbackDisableSet( sessionId, rptid, fuminum, disable );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiAutoRollbackDisableSet: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    printf( "Automatic Rollback %s successfully\n",
            ( disable == SAHPI_FALSE ) ? "enabled" : "disabled" );

    return HPI_SHELL_OK;
}

static ret_code_t start_rollback( SaHpiSessionIdT sessionId,
                                  SaHpiResourceIdT rptid,
                                  SaHpiFumiNumT fuminum )
{
    SaErrorT rv;

    rv = saHpiFumiRollbackStart( sessionId, rptid, fuminum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiRollbackStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t activate( SaHpiSessionIdT sessionId,
                            SaHpiResourceIdT rptid,
                            SaHpiFumiNumT fuminum,
                            SaHpiBoolT logical )
{
    SaErrorT rv;

    rv = saHpiFumiActivateStart( sessionId, rptid, fuminum, logical );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiActivateStart: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

static ret_code_t cleanup( SaHpiSessionIdT sessionId,
                           SaHpiResourceIdT rptid,
                           SaHpiFumiNumT fuminum,
                           SaHpiBankNumT banknum )
{
    SaErrorT rv;

    rv = saHpiFumiCleanup( sessionId, rptid, fuminum, banknum );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiFumiCleanup: %s\n", oh_lookup_error( rv ) );
        return HPI_SHELL_CMD_ERROR;
    }
    return HPI_SHELL_OK;
}

/*************************************
 * commands
 ************************************/

ret_code_t fumi_block( void )
{
    SaErrorT           rv;
    ret_code_t         ret;
    SaHpiResourceIdT   rptid;
    SaHpiInstrumentIdT rdrnum;
    SaHpiRdrT          rdr;

    ret = ask_rpt( &rptid );
    if ( ret != HPI_SHELL_OK ) {
        return ret;
    };
    ret = ask_rdr( rptid, SAHPI_FUMI_RDR, &rdrnum );
    if ( ret != HPI_SHELL_OK ) {
        return ret;
    }
    rv = saHpiRdrGetByInstrumentId( Domain->sessionId, rptid, SAHPI_FUMI_RDR, rdrnum, &rdr );
    if ( rv != SA_OK ) {
        printf( "ERROR!!! saHpiRdrGetByInstrumentId"
                "(Rpt=%d RdrType=%d Rdr=%d): %s\n",
                rptid, SAHPI_FUMI_RDR, rdrnum,
                oh_lookup_error( rv )
              );
        return HPI_SHELL_CMD_ERROR;
    };

    fumi_block_env.rptid    = rptid;
    fumi_block_env.fuminum  = rdr.RdrTypeUnion.FumiRec.Num;
    fumi_block_env.fumicaps = rdr.RdrTypeUnion.FumiRec.Capability;

    block_type = FUMI_COM;
    for ( ;; ) {
        int res;
        term_def_t * term ;
        char buf[256];

        res = get_new_command( "FUMI block ==> " );
        if ( res == 2 ) {
            unget_term();
            break;
        };
        term = get_next_term();
        if ( term == NULL ) continue;
        snprintf( buf, 256, "%s", term->term );
        if ( ( strcmp( buf, "q" ) == 0) || ( strcmp( buf, "quit" ) == 0 ) ) {
            break;
        }
    }
    block_type = MAIN_COM;
    return HPI_SHELL_OK;
}

ret_code_t fumi_block_specinfo(void)
{
    return show_spec_info( Domain->sessionId,
                           fumi_block_env.rptid,
                           fumi_block_env.fuminum );
}

ret_code_t fumi_block_serviceimpact(void)
{
    return show_service_impact( Domain->sessionId,
                                fumi_block_env.rptid,
                                fumi_block_env.fuminum );
}

ret_code_t fumi_block_setsource(void)
{
    SaHpiBankNumT banknum;
    SaHpiTextBufferT uri;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }
    if ( get_uri( &uri) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return set_source( Domain->sessionId,
                       fumi_block_env.rptid,
                       fumi_block_env.fuminum,
                       banknum,
                       &uri );
}

ret_code_t fumi_block_validatesource(void)
{
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return start_source_validation( Domain->sessionId,
                                    fumi_block_env.rptid,
                                    fumi_block_env.fuminum,
                                    banknum );
}

ret_code_t fumi_block_sourceinfo(void)
{
    ret_code_t rc;
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    rc = show_source_info( Domain->sessionId,
                           fumi_block_env.rptid,
                           fumi_block_env.fuminum,
                           banknum );
    if ( rc != HPI_SHELL_OK ) {
        return rc;
    }
    if ( fumi_block_env.fumicaps & SAHPI_FUMI_CAP_COMPONENTS ) {
        rc = show_source_component_info( Domain->sessionId,
                                         fumi_block_env.rptid,
                                         fumi_block_env.fuminum,
                                         banknum );
    }

    return rc;
}

ret_code_t fumi_block_targetinfo(void)
{
    ret_code_t rc;
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    rc = show_bank_info( Domain->sessionId,
                         fumi_block_env.rptid,
                         fumi_block_env.fuminum,
                         banknum
                       );
    if ( rc != HPI_SHELL_OK ) {
        return rc;
    }

    if ( fumi_block_env.fumicaps & SAHPI_FUMI_CAP_COMPONENTS ) {
        rc = show_bank_component_info( Domain->sessionId,
                                       fumi_block_env.rptid,
                                       fumi_block_env.fuminum,
                                       banknum );
        if ( rc != HPI_SHELL_OK ) {
            return rc;
        }
    }

    if ( banknum == 0 ) {
        rc = show_logical_bank_info( Domain->sessionId,
                                     fumi_block_env.rptid,
                                     fumi_block_env.fuminum );
        if ( rc != HPI_SHELL_OK ) {
            return rc;
        }

        if ( fumi_block_env.fumicaps & SAHPI_FUMI_CAP_COMPONENTS ) {
            if ( banknum == 0 ) {
                rc = show_logical_bank_component_info( Domain->sessionId,
                                                       fumi_block_env.rptid,
                                                       fumi_block_env.fuminum );
            }
        }
    }

    return rc;
}

ret_code_t fumi_block_backup(void)
{
    return start_backup( Domain->sessionId,
                         fumi_block_env.rptid,
                         fumi_block_env.fuminum );
}

ret_code_t fumi_block_setbootorder(void)
{
    SaHpiBankNumT banknum;
    SaHpiUint32T  position;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }
    if ( get_position( &position) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return set_bank_boot_order( Domain->sessionId,
                                fumi_block_env.rptid,
                                fumi_block_env.fuminum,
                                banknum,
                                position );
}

ret_code_t fumi_block_bankcopy(void)
{
    SaHpiBankNumT srcbanknum;
    SaHpiBankNumT dstbanknum;

    if ( get_banknum_prompt( "Source Bank Num(0 == active bank): ", &srcbanknum) != HPI_SHELL_OK )
    {
        return HPI_SHELL_PARM_ERROR;
    }
    if ( get_banknum_prompt( "Target Bank Num(0 == active bank): ", &dstbanknum) != HPI_SHELL_OK )
    {
        return HPI_SHELL_PARM_ERROR;
    }

    return start_bank_copying( Domain->sessionId,
                               fumi_block_env.rptid,
                               fumi_block_env.fuminum,
                               srcbanknum,
                               dstbanknum );
}

ret_code_t fumi_block_install(void)
{
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return start_install( Domain->sessionId,
                          fumi_block_env.rptid,
                          fumi_block_env.fuminum,
                          banknum );
}

ret_code_t fumi_block_status(void)
{
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return show_upgrade_status( Domain->sessionId,
                                fumi_block_env.rptid,
                                fumi_block_env.fuminum,
                                banknum );
}

ret_code_t fumi_block_verify(void)
{
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return start_target_verification( Domain->sessionId,
                                      fumi_block_env.rptid,
                                      fumi_block_env.fuminum,
                                      banknum );
}

ret_code_t fumi_block_verifymain(void)
{
    return start_main_target_verification( Domain->sessionId,
                                           fumi_block_env.rptid,
                                           fumi_block_env.fuminum );
}

ret_code_t fumi_block_cancel(void)
{
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return cancel_upgrade( Domain->sessionId,
                           fumi_block_env.rptid,
                           fumi_block_env.fuminum,
                           banknum );
}

ret_code_t fumi_block_disableautorollback(void)
{
    term_def_t * term;

    term = get_next_term();

    if ( term == NULL ) {
        show_autorollback_disable( Domain->sessionId,
                                   fumi_block_env.rptid,
                                   fumi_block_env.fuminum );
        return HPI_SHELL_OK;
    }
    if ( strcmp(term->term, "on" ) == 0 ) {
        set_autorollback_disable( Domain->sessionId,
                                  fumi_block_env.rptid,
                                  fumi_block_env.fuminum,
                                  SAHPI_TRUE );
    } else if ( strcmp(term->term, "off" ) == 0 ) {
        set_autorollback_disable( Domain->sessionId,
                                  fumi_block_env.rptid,
                                  fumi_block_env.fuminum,
                                  SAHPI_FALSE );
    } else {
        return HPI_SHELL_PARM_ERROR;
    }

    return HPI_SHELL_OK;
}

ret_code_t fumi_block_rollback(void)
{
    return start_rollback( Domain->sessionId,
                           fumi_block_env.rptid,
                           fumi_block_env.fuminum );
}

ret_code_t fumi_block_activate(void)
{
    SaHpiBoolT logical;

    term_def_t * term;

    term = get_next_term();

    if ( term == NULL ) {
        logical = SAHPI_FALSE;
    } else if ( strcmp(term->term, "logical" ) == 0 ) {
        logical = SAHPI_TRUE;
    } else {
        return HPI_SHELL_PARM_ERROR;
    }

    return activate( Domain->sessionId,
                     fumi_block_env.rptid,
                     fumi_block_env.fuminum,
                     logical );
}

ret_code_t fumi_block_cleanup(void)
{
    SaHpiBankNumT banknum;

    if ( get_banknum( &banknum) != HPI_SHELL_OK ) {
        return HPI_SHELL_PARM_ERROR;
    }

    return cleanup( Domain->sessionId,
                    fumi_block_env.rptid,
                    fumi_block_env.fuminum,
                    banknum );
}

