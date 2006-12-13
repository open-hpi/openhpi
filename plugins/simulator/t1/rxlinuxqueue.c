/*      -*- linux-c -*-
*
*(C) Copyright IBM Corp. 2005
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
* file and program are licensed under a BSD style license.  See
* the Copying file included with the OpenHPI distribution for
* full licensing terms.
*
* Authors:
*     W. david Ashley <dashley@us.ibm.com>
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <SaHpi.h>
#include <../sim_injector_ext.h>
#include <rexx.h>


#define RXFUNC_BADCALL 40

ULONG APIENTRY LnxOpenQueue(PSZ Name, LONG Argc, RXSTRING Argv[],
                            PSZ Queuename, PRXSTRING Retstr);
ULONG APIENTRY LnxWriteQueue(PSZ Name, LONG Argc, RXSTRING Argv[],
                             PSZ Queuename, PRXSTRING Retstr);
ULONG APIENTRY LnxGetHpiConst(PSZ Name, LONG Argc, RXSTRING Argv[],
                              PSZ Queuename, PRXSTRING Retstr);
static long long constval(char * const_str);


ULONG APIENTRY LnxOpenQueue(PSZ Name, LONG Argc, RXSTRING Argv[],
                            PSZ Queuename, PRXSTRING Retstr)
{
    key_t ipckey;
    int msgqueid;

    /* Check for valid arguments */
    if (Argc != 1 || !RXVALIDSTRING(Argv[0])) {
       return RXFUNC_BADCALL;
    }

    /* get the  queue */
    ipckey = ftok(Argv[0].strptr, SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, IPC_CREAT | 0660);
    if (msgqueid == -1) {
        sprintf(Retstr->strptr, "%d", -1);
        Retstr->strlength = strlen(Retstr->strptr);
        return 0;
    }

    /* Set the return code */
    sprintf(Retstr->strptr, "%d", msgqueid);
    Retstr->strlength = strlen(Retstr->strptr);

    return 0;
}


ULONG APIENTRY LnxWriteQueue(PSZ Name, LONG Argc, RXSTRING Argv[],
                             PSZ Queuename, PRXSTRING Retstr)
{
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    int msgsize;
    int rc;


    /* Check for valid arguments */
    if (Argc != 3 || !RXVALIDSTRING(Argv[0]) || !RXVALIDSTRING(Argv[1]) ||
     !RXVALIDSTRING(Argv[2])) {
       return RXFUNC_BADCALL;
    }

    /* get the queue id */
    msgqueid = atoi(Argv[0].strptr);
    if (msgqueid == -1) {
        return RXFUNC_BADCALL;
    }

    /* get the queue message type */
    buf.mtype = atoi(Argv[1].strptr);

    /* get the queue message size */
    msgsize = (int)Argv[2].strlength;
    if (msgsize <= 0 || msgsize > SIM_MSG_QUEUE_BUFSIZE) {
        sprintf(Retstr->strptr, "%d", -1);
        Retstr->strlength = strlen(Retstr->strptr);
        return 0;
    }

    /* get the message */
    memcpy(&buf.mtext, Argv[2].strptr, msgsize);

    /* send the message */
    rc = msgsnd(msgqueid, &buf, msgsize, 0);
    if (rc) {
        sprintf(Retstr->strptr, "%d", -1);
        Retstr->strlength = strlen(Retstr->strptr);
        return 0;
    }

    /* Set the return code */
    *Retstr->strptr = '0';
    Retstr->strlength = 1;

    return 0;
}


ULONG APIENTRY LnxGetHpiConst(PSZ Name, LONG Argc, RXSTRING Argv[],
                              PSZ Queuename, PRXSTRING Retstr)
{
    long long val;

    /* Check for valid arguments */
    if (Argc != 1 || !RXVALIDSTRING(Argv[0])) {
       return RXFUNC_BADCALL;
    }

    val = constval(Argv[0].strptr);

    /* Set the return code */
    sprintf(Retstr->strptr, "%lli", val);
    Retstr->strlength = strlen(Retstr->strptr);;

    return 0;
}


static long long constval(char * const_str)
{
    if (strcmp(const_str, "SAHPI_TRUE") == 0) return (long long) SAHPI_TRUE;
    if (strcmp(const_str, "SAHPI_FALSE") == 0) return (long long) SAHPI_FALSE;
    if (strcmp(const_str, "SAHPI_INTERFACE_VERSION") == 0) return (long long) SAHPI_INTERFACE_VERSION;
    if (strcmp(const_str, "SA_OK") == 0) return (long long) SA_OK;
    if (strcmp(const_str, "SA_HPI_ERR_BASE") == 0) return (long long) SA_HPI_ERR_BASE;
    if (strcmp(const_str, "SA_ERR_HPI_ERROR") == 0) return (long long) SA_ERR_HPI_ERROR;
    if (strcmp(const_str, "SA_ERR_HPI_UNSUPPORTED_API") == 0) return (long long) SA_ERR_HPI_UNSUPPORTED_API;
    if (strcmp(const_str, "SA_ERR_HPI_BUSY") == 0) return (long long) SA_ERR_HPI_BUSY;
    if (strcmp(const_str, "SA_ERR_HPI_INTERNAL_ERROR") == 0) return (long long) SA_ERR_HPI_INTERNAL_ERROR;
    if (strcmp(const_str, "SA_ERR_HPI_INVALID_CMD") == 0) return (long long) SA_ERR_HPI_INVALID_CMD;
    if (strcmp(const_str, "SA_ERR_HPI_TIMEOUT") == 0) return (long long) SA_ERR_HPI_TIMEOUT;
    if (strcmp(const_str, "SA_ERR_HPI_OUT_OF_SPACE") == 0) return (long long) SA_ERR_HPI_OUT_OF_SPACE;
    if (strcmp(const_str, "SA_ERR_HPI_OUT_OF_MEMORY") == 0) return (long long) SA_ERR_HPI_OUT_OF_MEMORY;
    if (strcmp(const_str, "SA_ERR_HPI_INVALID_PARAMS") == 0) return (long long) SA_ERR_HPI_INVALID_PARAMS;
    if (strcmp(const_str, "SA_ERR_HPI_INVALID_DATA") == 0) return (long long) SA_ERR_HPI_INVALID_DATA;
    if (strcmp(const_str, "SA_ERR_HPI_NOT_PRESENT") == 0) return (long long) SA_ERR_HPI_NOT_PRESENT;
    if (strcmp(const_str, "SA_ERR_HPI_NO_RESPONSE") == 0) return (long long) SA_ERR_HPI_NO_RESPONSE;
    if (strcmp(const_str, "SA_ERR_HPI_DUPLICATE") == 0) return (long long) SA_ERR_HPI_DUPLICATE;
    if (strcmp(const_str, "SA_ERR_HPI_INVALID_SESSION") == 0) return (long long) SA_ERR_HPI_INVALID_SESSION;
    if (strcmp(const_str, "SA_ERR_HPI_INVALID_DOMAIN") == 0) return (long long) SA_ERR_HPI_INVALID_DOMAIN;
    if (strcmp(const_str, "SA_ERR_HPI_INVALID_RESOURCE") == 0) return (long long) SA_ERR_HPI_INVALID_RESOURCE;
    if (strcmp(const_str, "SA_ERR_HPI_INVALID_REQUEST") == 0) return (long long) SA_ERR_HPI_INVALID_REQUEST;
    if (strcmp(const_str, "SA_ERR_HPI_ENTITY_NOT_PRESENT") == 0) return (long long) SA_ERR_HPI_ENTITY_NOT_PRESENT;
    if (strcmp(const_str, "SA_ERR_HPI_READ_ONLY") == 0) return (long long) SA_ERR_HPI_READ_ONLY;
    if (strcmp(const_str, "SA_ERR_HPI_CAPABILITY") == 0) return (long long) SA_ERR_HPI_CAPABILITY;
    if (strcmp(const_str, "SA_ERR_HPI_UNKNOWN") == 0) return (long long) SA_ERR_HPI_UNKNOWN;
    if (strcmp(const_str, "SAHPI_UNSPECIFIED_DOMAIN_ID") == 0) return (long long) SAHPI_UNSPECIFIED_DOMAIN_ID;
    if (strcmp(const_str, "SAHPI_UNSPECIFIED_RESOURCE_ID") == 0) return (long long) SAHPI_UNSPECIFIED_RESOURCE_ID;
    if (strcmp(const_str, "SAHPI_FIRST_ENTRY") == 0) return (long long) SAHPI_FIRST_ENTRY;
    if (strcmp(const_str, "SAHPI_LAST_ENTRY") == 0) return (long long) SAHPI_LAST_ENTRY;
    if (strcmp(const_str, "SAHPI_ENTRY_UNSPECIFIED") == 0) return (long long) SAHPI_ENTRY_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_TIME_UNSPECIFIED") == 0) return (long long) SAHPI_TIME_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_TIME_MAX_RELATIVE") == 0) return (long long) SAHPI_TIME_MAX_RELATIVE;
    if (strcmp(const_str, "SAHPI_TIMEOUT_IMMEDIATE") == 0) return (long long) SAHPI_TIMEOUT_IMMEDIATE;
    if (strcmp(const_str, "SAHPI_TIMEOUT_BLOCK") == 0) return (long long) SAHPI_TIMEOUT_BLOCK;
    if (strcmp(const_str, "SAHPI_LANG_UNDEF") == 0) return (long long) SAHPI_LANG_UNDEF;
    if (strcmp(const_str, "SAHPI_LANG_AFAR") == 0) return (long long) SAHPI_LANG_AFAR;
    if (strcmp(const_str, "SAHPI_LANG_ABKHAZIAN") == 0) return (long long) SAHPI_LANG_ABKHAZIAN;
    if (strcmp(const_str, "SAHPI_LANG_AFRIKAANS") == 0) return (long long) SAHPI_LANG_AFRIKAANS;
    if (strcmp(const_str, "SAHPI_LANG_AMHARIC") == 0) return (long long) SAHPI_LANG_AMHARIC;
    if (strcmp(const_str, "SAHPI_LANG_ARABIC") == 0) return (long long) SAHPI_LANG_ARABIC;
    if (strcmp(const_str, "SAHPI_LANG_ASSAMESE") == 0) return (long long) SAHPI_LANG_ASSAMESE;
    if (strcmp(const_str, "SAHPI_LANG_AYMARA") == 0) return (long long) SAHPI_LANG_AYMARA;
    if (strcmp(const_str, "SAHPI_LANG_AZERBAIJANI") == 0) return (long long) SAHPI_LANG_AZERBAIJANI;
    if (strcmp(const_str, "SAHPI_LANG_BASHKIR") == 0) return (long long) SAHPI_LANG_BASHKIR;
    if (strcmp(const_str, "SAHPI_LANG_BYELORUSSIAN") == 0) return (long long) SAHPI_LANG_BYELORUSSIAN;
    if (strcmp(const_str, "SAHPI_LANG_BULGARIAN") == 0) return (long long) SAHPI_LANG_BULGARIAN;
    if (strcmp(const_str, "SAHPI_LANG_BIHARI") == 0) return (long long) SAHPI_LANG_BIHARI;
    if (strcmp(const_str, "SAHPI_LANG_BISLAMA") == 0) return (long long) SAHPI_LANG_BISLAMA;
    if (strcmp(const_str, "SAHPI_LANG_BENGALI") == 0) return (long long) SAHPI_LANG_BENGALI;
    if (strcmp(const_str, "SAHPI_LANG_TIBETAN") == 0) return (long long) SAHPI_LANG_TIBETAN;
    if (strcmp(const_str, "SAHPI_LANG_BRETON") == 0) return (long long) SAHPI_LANG_BRETON;
    if (strcmp(const_str, "SAHPI_LANG_CATALAN") == 0) return (long long) SAHPI_LANG_CATALAN;
    if (strcmp(const_str, "SAHPI_LANG_CORSICAN") == 0) return (long long) SAHPI_LANG_CORSICAN;
    if (strcmp(const_str, "SAHPI_LANG_CZECH") == 0) return (long long) SAHPI_LANG_CZECH;
    if (strcmp(const_str, "SAHPI_LANG_WELSH") == 0) return (long long) SAHPI_LANG_WELSH;
    if (strcmp(const_str, "SAHPI_LANG_DANISH") == 0) return (long long) SAHPI_LANG_DANISH;
    if (strcmp(const_str, "SAHPI_LANG_GERMAN") == 0) return (long long) SAHPI_LANG_GERMAN;
    if (strcmp(const_str, "SAHPI_LANG_BHUTANI") == 0) return (long long) SAHPI_LANG_BHUTANI;
    if (strcmp(const_str, "SAHPI_LANG_GREEK") == 0) return (long long) SAHPI_LANG_GREEK;
    if (strcmp(const_str, "SAHPI_LANG_ENGLISH") == 0) return (long long) SAHPI_LANG_ENGLISH;
    if (strcmp(const_str, "SAHPI_LANG_ESPERANTO") == 0) return (long long) SAHPI_LANG_ESPERANTO;
    if (strcmp(const_str, "SAHPI_LANG_SPANISH") == 0) return (long long) SAHPI_LANG_SPANISH;
    if (strcmp(const_str, "SAHPI_LANG_ESTONIAN") == 0) return (long long) SAHPI_LANG_ESTONIAN;
    if (strcmp(const_str, "SAHPI_LANG_BASQUE") == 0) return (long long) SAHPI_LANG_BASQUE;
    if (strcmp(const_str, "SAHPI_LANG_PERSIAN") == 0) return (long long) SAHPI_LANG_PERSIAN;
    if (strcmp(const_str, "SAHPI_LANG_FINNISH") == 0) return (long long) SAHPI_LANG_FINNISH;
    if (strcmp(const_str, "SAHPI_LANG_FIJI") == 0) return (long long) SAHPI_LANG_FIJI;
    if (strcmp(const_str, "SAHPI_LANG_FAEROESE") == 0) return (long long) SAHPI_LANG_FAEROESE;
    if (strcmp(const_str, "SAHPI_LANG_FRENCH") == 0) return (long long) SAHPI_LANG_FRENCH;
    if (strcmp(const_str, "SAHPI_LANG_FRISIAN") == 0) return (long long) SAHPI_LANG_FRISIAN;
    if (strcmp(const_str, "SAHPI_LANG_IRISH") == 0) return (long long) SAHPI_LANG_IRISH;
    if (strcmp(const_str, "SAHPI_LANG_SCOTSGAELIC") == 0) return (long long) SAHPI_LANG_SCOTSGAELIC;
    if (strcmp(const_str, "SAHPI_LANG_GALICIAN") == 0) return (long long) SAHPI_LANG_GALICIAN;
    if (strcmp(const_str, "SAHPI_LANG_GUARANI") == 0) return (long long) SAHPI_LANG_GUARANI;
    if (strcmp(const_str, "SAHPI_LANG_GUJARATI") == 0) return (long long) SAHPI_LANG_GUJARATI;
    if (strcmp(const_str, "SAHPI_LANG_HAUSA") == 0) return (long long) SAHPI_LANG_HAUSA;
    if (strcmp(const_str, "SAHPI_LANG_HINDI") == 0) return (long long) SAHPI_LANG_HINDI;
    if (strcmp(const_str, "SAHPI_LANG_CROATIAN") == 0) return (long long) SAHPI_LANG_CROATIAN;
    if (strcmp(const_str, "SAHPI_LANG_HUNGARIAN") == 0) return (long long) SAHPI_LANG_HUNGARIAN;
    if (strcmp(const_str, "SAHPI_LANG_ARMENIAN") == 0) return (long long) SAHPI_LANG_ARMENIAN;
    if (strcmp(const_str, "SAHPI_LANG_INTERLINGUA") == 0) return (long long) SAHPI_LANG_INTERLINGUA;
    if (strcmp(const_str, "SAHPI_LANG_INTERLINGUE") == 0) return (long long) SAHPI_LANG_INTERLINGUE;
    if (strcmp(const_str, "SAHPI_LANG_INUPIAK") == 0) return (long long) SAHPI_LANG_INUPIAK;
    if (strcmp(const_str, "SAHPI_LANG_INDONESIAN") == 0) return (long long) SAHPI_LANG_INDONESIAN;
    if (strcmp(const_str, "SAHPI_LANG_ICELANDIC") == 0) return (long long) SAHPI_LANG_ICELANDIC;
    if (strcmp(const_str, "SAHPI_LANG_ITALIAN") == 0) return (long long) SAHPI_LANG_ITALIAN;
    if (strcmp(const_str, "SAHPI_LANG_HEBREW") == 0) return (long long) SAHPI_LANG_HEBREW;
    if (strcmp(const_str, "SAHPI_LANG_JAPANESE") == 0) return (long long) SAHPI_LANG_JAPANESE;
    if (strcmp(const_str, "SAHPI_LANG_YIDDISH") == 0) return (long long) SAHPI_LANG_YIDDISH;
    if (strcmp(const_str, "SAHPI_LANG_JAVANESE") == 0) return (long long) SAHPI_LANG_JAVANESE;
    if (strcmp(const_str, "SAHPI_LANG_GEORGIAN") == 0) return (long long) SAHPI_LANG_GEORGIAN;
    if (strcmp(const_str, "SAHPI_LANG_KAZAKH") == 0) return (long long) SAHPI_LANG_KAZAKH;
    if (strcmp(const_str, "SAHPI_LANG_GREENLANDIC") == 0) return (long long) SAHPI_LANG_GREENLANDIC;
    if (strcmp(const_str, "SAHPI_LANG_CAMBODIAN") == 0) return (long long) SAHPI_LANG_CAMBODIAN;
    if (strcmp(const_str, "SAHPI_LANG_KANNADA") == 0) return (long long) SAHPI_LANG_KANNADA;
    if (strcmp(const_str, "SAHPI_LANG_KOREAN") == 0) return (long long) SAHPI_LANG_KOREAN;
    if (strcmp(const_str, "SAHPI_LANG_KASHMIRI") == 0) return (long long) SAHPI_LANG_KASHMIRI;
    if (strcmp(const_str, "SAHPI_LANG_KURDISH") == 0) return (long long) SAHPI_LANG_KURDISH;
    if (strcmp(const_str, "SAHPI_LANG_KIRGHIZ") == 0) return (long long) SAHPI_LANG_KIRGHIZ;
    if (strcmp(const_str, "SAHPI_LANG_LATIN") == 0) return (long long) SAHPI_LANG_LATIN;
    if (strcmp(const_str, "SAHPI_LANG_LINGALA") == 0) return (long long) SAHPI_LANG_LINGALA;
    if (strcmp(const_str, "SAHPI_LANG_LAOTHIAN") == 0) return (long long) SAHPI_LANG_LAOTHIAN;
    if (strcmp(const_str, "SAHPI_LANG_LITHUANIAN") == 0) return (long long) SAHPI_LANG_LITHUANIAN;
    if (strcmp(const_str, "SAHPI_LANG_LATVIANLETTISH") == 0) return (long long) SAHPI_LANG_LATVIANLETTISH;
    if (strcmp(const_str, "SAHPI_LANG_MALAGASY") == 0) return (long long) SAHPI_LANG_MALAGASY;
    if (strcmp(const_str, "SAHPI_LANG_MAORI") == 0) return (long long) SAHPI_LANG_MAORI;
    if (strcmp(const_str, "SAHPI_LANG_MACEDONIAN") == 0) return (long long) SAHPI_LANG_MACEDONIAN;
    if (strcmp(const_str, "SAHPI_LANG_MALAYALAM") == 0) return (long long) SAHPI_LANG_MALAYALAM;
    if (strcmp(const_str, "SAHPI_LANG_MONGOLIAN") == 0) return (long long) SAHPI_LANG_MONGOLIAN;
    if (strcmp(const_str, "SAHPI_LANG_MOLDAVIAN") == 0) return (long long) SAHPI_LANG_MOLDAVIAN;
    if (strcmp(const_str, "SAHPI_LANG_MARATHI") == 0) return (long long) SAHPI_LANG_MARATHI;
    if (strcmp(const_str, "SAHPI_LANG_MALAY") == 0) return (long long) SAHPI_LANG_MALAY;
    if (strcmp(const_str, "SAHPI_LANG_MALTESE") == 0) return (long long) SAHPI_LANG_MALTESE;
    if (strcmp(const_str, "SAHPI_LANG_BURMESE") == 0) return (long long) SAHPI_LANG_BURMESE;
    if (strcmp(const_str, "SAHPI_LANG_NAURU") == 0) return (long long) SAHPI_LANG_NAURU;
    if (strcmp(const_str, "SAHPI_LANG_NEPALI") == 0) return (long long) SAHPI_LANG_NEPALI;
    if (strcmp(const_str, "SAHPI_LANG_DUTCH") == 0) return (long long) SAHPI_LANG_DUTCH;
    if (strcmp(const_str, "SAHPI_LANG_NORWEGIAN") == 0) return (long long) SAHPI_LANG_NORWEGIAN;
    if (strcmp(const_str, "SAHPI_LANG_OCCITAN") == 0) return (long long) SAHPI_LANG_OCCITAN;
    if (strcmp(const_str, "SAHPI_LANG_AFANOROMO") == 0) return (long long) SAHPI_LANG_AFANOROMO;
    if (strcmp(const_str, "SAHPI_LANG_ORIYA") == 0) return (long long) SAHPI_LANG_ORIYA;
    if (strcmp(const_str, "SAHPI_LANG_PUNJABI") == 0) return (long long) SAHPI_LANG_PUNJABI;
    if (strcmp(const_str, "SAHPI_LANG_POLISH") == 0) return (long long) SAHPI_LANG_POLISH;
    if (strcmp(const_str, "SAHPI_LANG_PASHTOPUSHTO") == 0) return (long long) SAHPI_LANG_PASHTOPUSHTO;
    if (strcmp(const_str, "SAHPI_LANG_PORTUGUESE") == 0) return (long long) SAHPI_LANG_PORTUGUESE;
    if (strcmp(const_str, "SAHPI_LANG_QUECHUA") == 0) return (long long) SAHPI_LANG_QUECHUA;
    if (strcmp(const_str, "SAHPI_LANG_RHAETOROMANCE") == 0) return (long long) SAHPI_LANG_RHAETOROMANCE;
    if (strcmp(const_str, "SAHPI_LANG_KIRUNDI") == 0) return (long long) SAHPI_LANG_KIRUNDI;
    if (strcmp(const_str, "SAHPI_LANG_ROMANIAN") == 0) return (long long) SAHPI_LANG_ROMANIAN;
    if (strcmp(const_str, "SAHPI_LANG_RUSSIAN") == 0) return (long long) SAHPI_LANG_RUSSIAN;
    if (strcmp(const_str, "SAHPI_LANG_KINYARWANDA") == 0) return (long long) SAHPI_LANG_KINYARWANDA;
    if (strcmp(const_str, "SAHPI_LANG_SANSKRIT") == 0) return (long long) SAHPI_LANG_SANSKRIT;
    if (strcmp(const_str, "SAHPI_LANG_SINDHI") == 0) return (long long) SAHPI_LANG_SINDHI;
    if (strcmp(const_str, "SAHPI_LANG_SANGRO") == 0) return (long long) SAHPI_LANG_SANGRO;
    if (strcmp(const_str, "SAHPI_LANG_SERBOCROATIAN") == 0) return (long long) SAHPI_LANG_SERBOCROATIAN;
    if (strcmp(const_str, "SAHPI_LANG_SINGHALESE") == 0) return (long long) SAHPI_LANG_SINGHALESE;
    if (strcmp(const_str, "SAHPI_LANG_SLOVAK") == 0) return (long long) SAHPI_LANG_SLOVAK;
    if (strcmp(const_str, "SAHPI_LANG_SLOVENIAN") == 0) return (long long) SAHPI_LANG_SLOVENIAN;
    if (strcmp(const_str, "SAHPI_LANG_SAMOAN") == 0) return (long long) SAHPI_LANG_SAMOAN;
    if (strcmp(const_str, "SAHPI_LANG_SHONA") == 0) return (long long) SAHPI_LANG_SHONA;
    if (strcmp(const_str, "SAHPI_LANG_SOMALI") == 0) return (long long) SAHPI_LANG_SOMALI;
    if (strcmp(const_str, "SAHPI_LANG_ALBANIAN") == 0) return (long long) SAHPI_LANG_ALBANIAN;
    if (strcmp(const_str, "SAHPI_LANG_SERBIAN") == 0) return (long long) SAHPI_LANG_SERBIAN;
    if (strcmp(const_str, "SAHPI_LANG_SISWATI") == 0) return (long long) SAHPI_LANG_SISWATI;
    if (strcmp(const_str, "SAHPI_LANG_SESOTHO") == 0) return (long long) SAHPI_LANG_SESOTHO;
    if (strcmp(const_str, "SAHPI_LANG_SUDANESE") == 0) return (long long) SAHPI_LANG_SUDANESE;
    if (strcmp(const_str, "SAHPI_LANG_SWEDISH") == 0) return (long long) SAHPI_LANG_SWEDISH;
    if (strcmp(const_str, "SAHPI_LANG_SWAHILI") == 0) return (long long) SAHPI_LANG_SWAHILI;
    if (strcmp(const_str, "SAHPI_LANG_TAMIL") == 0) return (long long) SAHPI_LANG_TAMIL;
    if (strcmp(const_str, "SAHPI_LANG_TELUGU") == 0) return (long long) SAHPI_LANG_TELUGU;
    if (strcmp(const_str, "SAHPI_LANG_TAJIK") == 0) return (long long) SAHPI_LANG_TAJIK;
    if (strcmp(const_str, "SAHPI_LANG_THAI") == 0) return (long long) SAHPI_LANG_THAI;
    if (strcmp(const_str, "SAHPI_LANG_TIGRINYA") == 0) return (long long) SAHPI_LANG_TIGRINYA;
    if (strcmp(const_str, "SAHPI_LANG_TURKMEN") == 0) return (long long) SAHPI_LANG_TURKMEN;
    if (strcmp(const_str, "SAHPI_LANG_TAGALOG") == 0) return (long long) SAHPI_LANG_TAGALOG;
    if (strcmp(const_str, "SAHPI_LANG_SETSWANA") == 0) return (long long) SAHPI_LANG_SETSWANA;
    if (strcmp(const_str, "SAHPI_LANG_TONGA") == 0) return (long long) SAHPI_LANG_TONGA;
    if (strcmp(const_str, "SAHPI_LANG_TURKISH") == 0) return (long long) SAHPI_LANG_TURKISH;
    if (strcmp(const_str, "SAHPI_LANG_TSONGA") == 0) return (long long) SAHPI_LANG_TSONGA;
    if (strcmp(const_str, "SAHPI_LANG_TATAR") == 0) return (long long) SAHPI_LANG_TATAR;
    if (strcmp(const_str, "SAHPI_LANG_TWI") == 0) return (long long) SAHPI_LANG_TWI;
    if (strcmp(const_str, "SAHPI_LANG_UKRAINIAN") == 0) return (long long) SAHPI_LANG_UKRAINIAN;
    if (strcmp(const_str, "SAHPI_LANG_URDU") == 0) return (long long) SAHPI_LANG_URDU;
    if (strcmp(const_str, "SAHPI_LANG_UZBEK") == 0) return (long long) SAHPI_LANG_UZBEK;
    if (strcmp(const_str, "SAHPI_LANG_VIETNAMESE") == 0) return (long long) SAHPI_LANG_VIETNAMESE;
    if (strcmp(const_str, "SAHPI_LANG_VOLAPUK") == 0) return (long long) SAHPI_LANG_VOLAPUK;
    if (strcmp(const_str, "SAHPI_LANG_WOLOF") == 0) return (long long) SAHPI_LANG_WOLOF;
    if (strcmp(const_str, "SAHPI_LANG_XHOSA") == 0) return (long long) SAHPI_LANG_XHOSA;
    if (strcmp(const_str, "SAHPI_LANG_YORUBA") == 0) return (long long) SAHPI_LANG_YORUBA;
    if (strcmp(const_str, "SAHPI_LANG_CHINESE") == 0) return (long long) SAHPI_LANG_CHINESE;
    if (strcmp(const_str, "SAHPI_LANG_ZULU") == 0) return (long long) SAHPI_LANG_ZULU;
    if (strcmp(const_str, "SAHPI_MAX_TEXT_BUFFER_LENGTH") == 0) return (long long) SAHPI_MAX_TEXT_BUFFER_LENGTH;
    if (strcmp(const_str, "SAHPI_TL_TYPE_UNICODE") == 0) return (long long) SAHPI_TL_TYPE_UNICODE;
    if (strcmp(const_str, "SAHPI_TL_TYPE_BCDPLUS") == 0) return (long long) SAHPI_TL_TYPE_BCDPLUS;
    if (strcmp(const_str, "SAHPI_TL_TYPE_ASCII6") == 0) return (long long) SAHPI_TL_TYPE_ASCII6;
    if (strcmp(const_str, "SAHPI_TL_TYPE_TEXT") == 0) return (long long) SAHPI_TL_TYPE_TEXT;
    if (strcmp(const_str, "SAHPI_TL_TYPE_BINARY") == 0) return (long long) SAHPI_TL_TYPE_BINARY;
    if (strcmp(const_str, "SAHPI_ENT_IPMI_GROUP") == 0) return (long long) SAHPI_ENT_IPMI_GROUP;
    if (strcmp(const_str, "SAHPI_ENT_SAFHPI_GROUP") == 0) return (long long) SAHPI_ENT_SAFHPI_GROUP;
    if (strcmp(const_str, "SAHPI_ENT_ROOT_VALUE") == 0) return (long long) SAHPI_ENT_ROOT_VALUE;
    if (strcmp(const_str, "SAHPI_ENT_UNSPECIFIED") == 0) return (long long) SAHPI_ENT_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_ENT_OTHER") == 0) return (long long) SAHPI_ENT_OTHER;
    if (strcmp(const_str, "SAHPI_ENT_UNKNOWN") == 0) return (long long) SAHPI_ENT_UNKNOWN;
    if (strcmp(const_str, "SAHPI_ENT_PROCESSOR") == 0) return (long long) SAHPI_ENT_PROCESSOR;
    if (strcmp(const_str, "SAHPI_ENT_DISK_BAY") == 0) return (long long) SAHPI_ENT_DISK_BAY;
    if (strcmp(const_str, "SAHPI_ENT_PERIPHERAL_BAY") == 0) return (long long) SAHPI_ENT_PERIPHERAL_BAY;
    if (strcmp(const_str, "SAHPI_ENT_SYS_MGMNT_MODULE") == 0) return (long long) SAHPI_ENT_SYS_MGMNT_MODULE;
    if (strcmp(const_str, "SAHPI_ENT_SYSTEM_BOARD") == 0) return (long long) SAHPI_ENT_SYSTEM_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_MEMORY_MODULE") == 0) return (long long) SAHPI_ENT_MEMORY_MODULE;
    if (strcmp(const_str, "SAHPI_ENT_PROCESSOR_MODULE") == 0) return (long long) SAHPI_ENT_PROCESSOR_MODULE;
    if (strcmp(const_str, "SAHPI_ENT_POWER_SUPPLY") == 0) return (long long) SAHPI_ENT_POWER_SUPPLY;
    if (strcmp(const_str, "SAHPI_ENT_ADD_IN_CARD") == 0) return (long long) SAHPI_ENT_ADD_IN_CARD;
    if (strcmp(const_str, "SAHPI_ENT_FRONT_PANEL_BOARD") == 0) return (long long) SAHPI_ENT_FRONT_PANEL_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_BACK_PANEL_BOARD") == 0) return (long long) SAHPI_ENT_BACK_PANEL_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_POWER_SYSTEM_BOARD") == 0) return (long long) SAHPI_ENT_POWER_SYSTEM_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_DRIVE_BACKPLANE") == 0) return (long long) SAHPI_ENT_DRIVE_BACKPLANE;
    if (strcmp(const_str, "SAHPI_ENT_SYS_EXPANSION_BOARD") == 0) return (long long) SAHPI_ENT_SYS_EXPANSION_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_OTHER_SYSTEM_BOARD") == 0) return (long long) SAHPI_ENT_OTHER_SYSTEM_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_PROCESSOR_BOARD") == 0) return (long long) SAHPI_ENT_PROCESSOR_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_POWER_UNIT") == 0) return (long long) SAHPI_ENT_POWER_UNIT;
    if (strcmp(const_str, "SAHPI_ENT_POWER_MODULE") == 0) return (long long) SAHPI_ENT_POWER_MODULE;
    if (strcmp(const_str, "SAHPI_ENT_POWER_MGMNT") == 0) return (long long) SAHPI_ENT_POWER_MGMNT;
    if (strcmp(const_str, "SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD") == 0) return (long long) SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_SYSTEM_CHASSIS") == 0) return (long long) SAHPI_ENT_SYSTEM_CHASSIS;
    if (strcmp(const_str, "SAHPI_ENT_SUB_CHASSIS") == 0) return (long long) SAHPI_ENT_SUB_CHASSIS;
    if (strcmp(const_str, "SAHPI_ENT_OTHER_CHASSIS_BOARD") == 0) return (long long) SAHPI_ENT_OTHER_CHASSIS_BOARD;
    if (strcmp(const_str, "SAHPI_ENT_DISK_DRIVE_BAY") == 0) return (long long) SAHPI_ENT_DISK_DRIVE_BAY;
    if (strcmp(const_str, "SAHPI_ENT_PERIPHERAL_BAY_2") == 0) return (long long) SAHPI_ENT_PERIPHERAL_BAY_2;
    if (strcmp(const_str, "SAHPI_ENT_DEVICE_BAY") == 0) return (long long) SAHPI_ENT_DEVICE_BAY;
    if (strcmp(const_str, "SAHPI_ENT_COOLING_DEVICE") == 0) return (long long) SAHPI_ENT_COOLING_DEVICE;
    if (strcmp(const_str, "SAHPI_ENT_COOLING_UNIT") == 0) return (long long) SAHPI_ENT_COOLING_UNIT;
    if (strcmp(const_str, "SAHPI_ENT_INTERCONNECT") == 0) return (long long) SAHPI_ENT_INTERCONNECT;
    if (strcmp(const_str, "SAHPI_ENT_MEMORY_DEVICE") == 0) return (long long) SAHPI_ENT_MEMORY_DEVICE;
    if (strcmp(const_str, "SAHPI_ENT_SYS_MGMNT_SOFTWARE") == 0) return (long long) SAHPI_ENT_SYS_MGMNT_SOFTWARE;
    if (strcmp(const_str, "SAHPI_ENT_BIOS") == 0) return (long long) SAHPI_ENT_BIOS;
    if (strcmp(const_str, "SAHPI_ENT_OPERATING_SYSTEM") == 0) return (long long) SAHPI_ENT_OPERATING_SYSTEM;
    if (strcmp(const_str, "SAHPI_ENT_SYSTEM_BUS") == 0) return (long long) SAHPI_ENT_SYSTEM_BUS;
    if (strcmp(const_str, "SAHPI_ENT_GROUP") == 0) return (long long) SAHPI_ENT_GROUP;
    if (strcmp(const_str, "SAHPI_ENT_REMOTE") == 0) return (long long) SAHPI_ENT_REMOTE;
    if (strcmp(const_str, "SAHPI_ENT_EXTERNAL_ENVIRONMENT") == 0) return (long long) SAHPI_ENT_EXTERNAL_ENVIRONMENT;
    if (strcmp(const_str, "SAHPI_ENT_BATTERY") == 0) return (long long) SAHPI_ENT_BATTERY;
    if (strcmp(const_str, "SAHPI_ENT_CHASSIS_SPECIFIC") == 0) return (long long) SAHPI_ENT_CHASSIS_SPECIFIC;
    if (strcmp(const_str, "SAHPI_ENT_BOARD_SET_SPECIFIC") == 0) return (long long) SAHPI_ENT_BOARD_SET_SPECIFIC;
    if (strcmp(const_str, "SAHPI_ENT_OEM_SYSINT_SPECIFIC") == 0) return (long long) SAHPI_ENT_OEM_SYSINT_SPECIFIC;
    if (strcmp(const_str, "SAHPI_ENT_ROOT") == 0) return (long long) SAHPI_ENT_ROOT;
    if (strcmp(const_str, "SAHPI_ENT_RACK") == 0) return (long long) SAHPI_ENT_RACK;
    if (strcmp(const_str, "SAHPI_ENT_SUBRACK") == 0) return (long long) SAHPI_ENT_SUBRACK;
    if (strcmp(const_str, "SAHPI_ENT_COMPACTPCI_CHASSIS") == 0) return (long long) SAHPI_ENT_COMPACTPCI_CHASSIS;
    if (strcmp(const_str, "SAHPI_ENT_ADVANCEDTCA_CHASSIS") == 0) return (long long) SAHPI_ENT_ADVANCEDTCA_CHASSIS;
    if (strcmp(const_str, "SAHPI_ENT_RACK_MOUNTED_SERVER") == 0) return (long long) SAHPI_ENT_RACK_MOUNTED_SERVER;
    if (strcmp(const_str, "SAHPI_ENT_SYSTEM_BLADE") == 0) return (long long) SAHPI_ENT_SYSTEM_BLADE;
    if (strcmp(const_str, "SAHPI_ENT_SWITCH") == 0) return (long long) SAHPI_ENT_SWITCH;
    if (strcmp(const_str, "SAHPI_ENT_SWITCH_BLADE") == 0) return (long long) SAHPI_ENT_SWITCH_BLADE;
    if (strcmp(const_str, "SAHPI_ENT_SBC_BLADE") == 0) return (long long) SAHPI_ENT_SBC_BLADE;
    if (strcmp(const_str, "SAHPI_ENT_IO_BLADE") == 0) return (long long) SAHPI_ENT_IO_BLADE;
    if (strcmp(const_str, "SAHPI_ENT_DISK_BLADE") == 0) return (long long) SAHPI_ENT_DISK_BLADE;
    if (strcmp(const_str, "SAHPI_ENT_DISK_DRIVE") == 0) return (long long) SAHPI_ENT_DISK_DRIVE;
    if (strcmp(const_str, "SAHPI_ENT_FAN") == 0) return (long long) SAHPI_ENT_FAN;
    if (strcmp(const_str, "SAHPI_ENT_POWER_DISTRIBUTION_UNIT") == 0) return (long long) SAHPI_ENT_POWER_DISTRIBUTION_UNIT;
    if (strcmp(const_str, "SAHPI_ENT_SPEC_PROC_BLADE") == 0) return (long long) SAHPI_ENT_SPEC_PROC_BLADE;
    if (strcmp(const_str, "SAHPI_ENT_IO_SUBBOARD") == 0) return (long long) SAHPI_ENT_IO_SUBBOARD;
    if (strcmp(const_str, "SAHPI_ENT_SBC_SUBBOARD") == 0) return (long long) SAHPI_ENT_SBC_SUBBOARD;
    if (strcmp(const_str, "SAHPI_ENT_ALARM_MANAGER") == 0) return (long long) SAHPI_ENT_ALARM_MANAGER;
    if (strcmp(const_str, "SAHPI_ENT_SHELF_MANAGER") == 0) return (long long) SAHPI_ENT_SHELF_MANAGER;
    if (strcmp(const_str, "SAHPI_ENT_DISPLAY_PANEL") == 0) return (long long) SAHPI_ENT_DISPLAY_PANEL;
    if (strcmp(const_str, "SAHPI_ENT_SUBBOARD_CARRIER_BLADE") == 0) return (long long) SAHPI_ENT_SUBBOARD_CARRIER_BLADE;
    if (strcmp(const_str, "SAHPI_ENT_PHYSICAL_SLOT") == 0) return (long long) SAHPI_ENT_PHYSICAL_SLOT;
    if (strcmp(const_str, "SAHPI_MAX_ENTITY_PATH") == 0) return (long long) SAHPI_MAX_ENTITY_PATH;
    if (strcmp(const_str, "SAHPI_EC_UNSPECIFIED") == 0) return (long long) SAHPI_EC_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_EC_THRESHOLD") == 0) return (long long) SAHPI_EC_THRESHOLD;
    if (strcmp(const_str, "SAHPI_EC_USAGE") == 0) return (long long) SAHPI_EC_USAGE;
    if (strcmp(const_str, "SAHPI_EC_STATE") == 0) return (long long) SAHPI_EC_STATE;
    if (strcmp(const_str, "SAHPI_EC_PRED_FAIL") == 0) return (long long) SAHPI_EC_PRED_FAIL;
    if (strcmp(const_str, "SAHPI_EC_LIMIT") == 0) return (long long) SAHPI_EC_LIMIT;
    if (strcmp(const_str, "SAHPI_EC_PERFORMANCE") == 0) return (long long) SAHPI_EC_PERFORMANCE;
    if (strcmp(const_str, "SAHPI_EC_SEVERITY") == 0) return (long long) SAHPI_EC_SEVERITY;
    if (strcmp(const_str, "SAHPI_EC_PRESENCE") == 0) return (long long) SAHPI_EC_PRESENCE;
    if (strcmp(const_str, "SAHPI_EC_ENABLE") == 0) return (long long) SAHPI_EC_ENABLE;
    if (strcmp(const_str, "SAHPI_EC_AVAILABILITY") == 0) return (long long) SAHPI_EC_AVAILABILITY;
    if (strcmp(const_str, "SAHPI_EC_REDUNDANCY") == 0) return (long long) SAHPI_EC_REDUNDANCY;
    if (strcmp(const_str, "SAHPI_EC_SENSOR_SPECIFIC") == 0) return (long long) SAHPI_EC_SENSOR_SPECIFIC;
    if (strcmp(const_str, "SAHPI_EC_GENERIC") == 0) return (long long) SAHPI_EC_GENERIC;
    if (strcmp(const_str, "SAHPI_ES_UNSPECIFIED") == 0) return (long long) SAHPI_ES_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_ES_LOWER_MINOR") == 0) return (long long) SAHPI_ES_LOWER_MINOR;
    if (strcmp(const_str, "SAHPI_ES_LOWER_MAJOR") == 0) return (long long) SAHPI_ES_LOWER_MAJOR;
    if (strcmp(const_str, "SAHPI_ES_LOWER_CRIT") == 0) return (long long) SAHPI_ES_LOWER_CRIT;
    if (strcmp(const_str, "SAHPI_ES_UPPER_MINOR") == 0) return (long long) SAHPI_ES_UPPER_MINOR;
    if (strcmp(const_str, "SAHPI_ES_UPPER_MAJOR") == 0) return (long long) SAHPI_ES_UPPER_MAJOR;
    if (strcmp(const_str, "SAHPI_ES_UPPER_CRIT") == 0) return (long long) SAHPI_ES_UPPER_CRIT;
    if (strcmp(const_str, "SAHPI_ES_IDLE") == 0) return (long long) SAHPI_ES_IDLE;
    if (strcmp(const_str, "SAHPI_ES_ACTIVE") == 0) return (long long) SAHPI_ES_ACTIVE;
    if (strcmp(const_str, "SAHPI_ES_BUSY") == 0) return (long long) SAHPI_ES_BUSY;
    if (strcmp(const_str, "SAHPI_ES_STATE_DEASSERTED") == 0) return (long long) SAHPI_ES_STATE_DEASSERTED;
    if (strcmp(const_str, "SAHPI_ES_STATE_ASSERTED") == 0) return (long long) SAHPI_ES_STATE_ASSERTED;
    if (strcmp(const_str, "SAHPI_ES_PRED_FAILURE_DEASSERT") == 0) return (long long) SAHPI_ES_PRED_FAILURE_DEASSERT;
    if (strcmp(const_str, "SAHPI_ES_PRED_FAILURE_ASSERT") == 0) return (long long) SAHPI_ES_PRED_FAILURE_ASSERT;
    if (strcmp(const_str, "SAHPI_ES_LIMIT_NOT_EXCEEDED") == 0) return (long long) SAHPI_ES_LIMIT_NOT_EXCEEDED;
    if (strcmp(const_str, "SAHPI_ES_LIMIT_EXCEEDED") == 0) return (long long) SAHPI_ES_LIMIT_EXCEEDED;
    if (strcmp(const_str, "SAHPI_ES_PERFORMANCE_MET") == 0) return (long long) SAHPI_ES_PERFORMANCE_MET;
    if (strcmp(const_str, "SAHPI_ES_PERFORMANCE_LAGS") == 0) return (long long) SAHPI_ES_PERFORMANCE_LAGS;
    if (strcmp(const_str, "SAHPI_ES_OK") == 0) return (long long) SAHPI_ES_OK;
    if (strcmp(const_str, "SAHPI_ES_MINOR_FROM_OK") == 0) return (long long) SAHPI_ES_MINOR_FROM_OK;
    if (strcmp(const_str, "SAHPI_ES_MAJOR_FROM_LESS") == 0) return (long long) SAHPI_ES_MAJOR_FROM_LESS;
    if (strcmp(const_str, "SAHPI_ES_CRITICAL_FROM_LESS") == 0) return (long long) SAHPI_ES_CRITICAL_FROM_LESS;
    if (strcmp(const_str, "SAHPI_ES_MINOR_FROM_MORE") == 0) return (long long) SAHPI_ES_MINOR_FROM_MORE;
    if (strcmp(const_str, "SAHPI_ES_MAJOR_FROM_CRITICAL") == 0) return (long long) SAHPI_ES_MAJOR_FROM_CRITICAL;
    if (strcmp(const_str, "SAHPI_ES_CRITICAL") == 0) return (long long) SAHPI_ES_CRITICAL;
    if (strcmp(const_str, "SAHPI_ES_MONITOR") == 0) return (long long) SAHPI_ES_MONITOR;
    if (strcmp(const_str, "SAHPI_ES_INFORMATIONAL") == 0) return (long long) SAHPI_ES_INFORMATIONAL;
    if (strcmp(const_str, "SAHPI_ES_ABSENT") == 0) return (long long) SAHPI_ES_ABSENT;
    if (strcmp(const_str, "SAHPI_ES_PRESENT") == 0) return (long long) SAHPI_ES_PRESENT;
    if (strcmp(const_str, "SAHPI_ES_DISABLED") == 0) return (long long) SAHPI_ES_DISABLED;
    if (strcmp(const_str, "SAHPI_ES_ENABLED") == 0) return (long long) SAHPI_ES_ENABLED;
    if (strcmp(const_str, "SAHPI_ES_RUNNING") == 0) return (long long) SAHPI_ES_RUNNING;
    if (strcmp(const_str, "SAHPI_ES_TEST") == 0) return (long long) SAHPI_ES_TEST;
    if (strcmp(const_str, "SAHPI_ES_POWER_OFF") == 0) return (long long) SAHPI_ES_POWER_OFF;
    if (strcmp(const_str, "SAHPI_ES_ON_LINE") == 0) return (long long) SAHPI_ES_ON_LINE;
    if (strcmp(const_str, "SAHPI_ES_OFF_LINE") == 0) return (long long) SAHPI_ES_OFF_LINE;
    if (strcmp(const_str, "SAHPI_ES_OFF_DUTY") == 0) return (long long) SAHPI_ES_OFF_DUTY;
    if (strcmp(const_str, "SAHPI_ES_DEGRADED") == 0) return (long long) SAHPI_ES_DEGRADED;
    if (strcmp(const_str, "SAHPI_ES_POWER_SAVE") == 0) return (long long) SAHPI_ES_POWER_SAVE;
    if (strcmp(const_str, "SAHPI_ES_INSTALL_ERROR") == 0) return (long long) SAHPI_ES_INSTALL_ERROR;
    if (strcmp(const_str, "SAHPI_ES_FULLY_REDUNDANT") == 0) return (long long) SAHPI_ES_FULLY_REDUNDANT;
    if (strcmp(const_str, "SAHPI_ES_REDUNDANCY_LOST") == 0) return (long long) SAHPI_ES_REDUNDANCY_LOST;
    if (strcmp(const_str, "SAHPI_ES_REDUNDANCY_DEGRADED") == 0) return (long long) SAHPI_ES_REDUNDANCY_DEGRADED;
    if (strcmp(const_str, "SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES") == 0) return (long long) SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES;
    if (strcmp(const_str, "SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES") == 0) return (long long) SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES;
    if (strcmp(const_str, "SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES") == 0) return (long long) SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES;
    if (strcmp(const_str, "SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL") == 0) return (long long) SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL;
    if (strcmp(const_str, "SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON") == 0) return (long long) SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON;
    if (strcmp(const_str, "SAHPI_ES_STATE_00") == 0) return (long long) SAHPI_ES_STATE_00;
    if (strcmp(const_str, "SAHPI_ES_STATE_01") == 0) return (long long) SAHPI_ES_STATE_01;
    if (strcmp(const_str, "SAHPI_ES_STATE_02") == 0) return (long long) SAHPI_ES_STATE_02;
    if (strcmp(const_str, "SAHPI_ES_STATE_03") == 0) return (long long) SAHPI_ES_STATE_03;
    if (strcmp(const_str, "SAHPI_ES_STATE_04") == 0) return (long long) SAHPI_ES_STATE_04;
    if (strcmp(const_str, "SAHPI_ES_STATE_05") == 0) return (long long) SAHPI_ES_STATE_05;
    if (strcmp(const_str, "SAHPI_ES_STATE_06") == 0) return (long long) SAHPI_ES_STATE_06;
    if (strcmp(const_str, "SAHPI_ES_STATE_07") == 0) return (long long) SAHPI_ES_STATE_07;
    if (strcmp(const_str, "SAHPI_ES_STATE_08") == 0) return (long long) SAHPI_ES_STATE_08;
    if (strcmp(const_str, "SAHPI_ES_STATE_09") == 0) return (long long) SAHPI_ES_STATE_09;
    if (strcmp(const_str, "SAHPI_ES_STATE_10") == 0) return (long long) SAHPI_ES_STATE_10;
    if (strcmp(const_str, "SAHPI_ES_STATE_11") == 0) return (long long) SAHPI_ES_STATE_11;
    if (strcmp(const_str, "SAHPI_ES_STATE_12") == 0) return (long long) SAHPI_ES_STATE_12;
    if (strcmp(const_str, "SAHPI_ES_STATE_13") == 0) return (long long) SAHPI_ES_STATE_13;
    if (strcmp(const_str, "SAHPI_ES_STATE_14") == 0) return (long long) SAHPI_ES_STATE_14;
    if (strcmp(const_str, "SAHPI_STANDARD_SENSOR_MIN") == 0) return (long long) SAHPI_STANDARD_SENSOR_MIN;
    if (strcmp(const_str, "SAHPI_STANDARD_SENSOR_MAX") == 0) return (long long) SAHPI_STANDARD_SENSOR_MAX;
    if (strcmp(const_str, "SAHPI_TEMPERATURE") == 0) return (long long) SAHPI_TEMPERATURE;
    if (strcmp(const_str, "SAHPI_VOLTAGE") == 0) return (long long) SAHPI_VOLTAGE;
    if (strcmp(const_str, "SAHPI_CURRENT") == 0) return (long long) SAHPI_CURRENT;
    if (strcmp(const_str, "SAHPI_FAN") == 0) return (long long) SAHPI_FAN;
    if (strcmp(const_str, "SAHPI_PHYSICAL_SECURITY") == 0) return (long long) SAHPI_PHYSICAL_SECURITY;
    if (strcmp(const_str, "SAHPI_PLATFORM_VIOLATION") == 0) return (long long) SAHPI_PLATFORM_VIOLATION;
    if (strcmp(const_str, "SAHPI_PROCESSOR") == 0) return (long long) SAHPI_PROCESSOR;
    if (strcmp(const_str, "SAHPI_POWER_SUPPLY") == 0) return (long long) SAHPI_POWER_SUPPLY;
    if (strcmp(const_str, "SAHPI_POWER_UNIT") == 0) return (long long) SAHPI_POWER_UNIT;
    if (strcmp(const_str, "SAHPI_COOLING_DEVICE") == 0) return (long long) SAHPI_COOLING_DEVICE;
    if (strcmp(const_str, "SAHPI_OTHER_UNITS_BASED_SENSOR") == 0) return (long long) SAHPI_OTHER_UNITS_BASED_SENSOR;
    if (strcmp(const_str, "SAHPI_MEMORY") == 0) return (long long) SAHPI_MEMORY;
    if (strcmp(const_str, "SAHPI_DRIVE_SLOT") == 0) return (long long) SAHPI_DRIVE_SLOT;
    if (strcmp(const_str, "SAHPI_POST_MEMORY_RESIZE") == 0) return (long long) SAHPI_POST_MEMORY_RESIZE;
    if (strcmp(const_str, "SAHPI_SYSTEM_FW_PROGRESS") == 0) return (long long) SAHPI_SYSTEM_FW_PROGRESS;
    if (strcmp(const_str, "SAHPI_EVENT_LOGGING_DISABLED") == 0) return (long long) SAHPI_EVENT_LOGGING_DISABLED;
    if (strcmp(const_str, "SAHPI_RESERVED1") == 0) return (long long) SAHPI_RESERVED1;
    if (strcmp(const_str, "SAHPI_SYSTEM_EVENT") == 0) return (long long) SAHPI_SYSTEM_EVENT;
    if (strcmp(const_str, "SAHPI_CRITICAL_INTERRUPT") == 0) return (long long) SAHPI_CRITICAL_INTERRUPT;
    if (strcmp(const_str, "SAHPI_BUTTON") == 0) return (long long) SAHPI_BUTTON;
    if (strcmp(const_str, "SAHPI_MODULE_BOARD") == 0) return (long long) SAHPI_MODULE_BOARD;
    if (strcmp(const_str, "SAHPI_MICROCONTROLLER_COPROCESSOR") == 0) return (long long) SAHPI_MICROCONTROLLER_COPROCESSOR;
    if (strcmp(const_str, "SAHPI_ADDIN_CARD") == 0) return (long long) SAHPI_ADDIN_CARD;
    if (strcmp(const_str, "SAHPI_CHASSIS") == 0) return (long long) SAHPI_CHASSIS;
    if (strcmp(const_str, "SAHPI_CHIP_SET") == 0) return (long long) SAHPI_CHIP_SET;
    if (strcmp(const_str, "SAHPI_OTHER_FRU") == 0) return (long long) SAHPI_OTHER_FRU;
    if (strcmp(const_str, "SAHPI_CABLE_INTERCONNECT") == 0) return (long long) SAHPI_CABLE_INTERCONNECT;
    if (strcmp(const_str, "SAHPI_TERMINATOR") == 0) return (long long) SAHPI_TERMINATOR;
    if (strcmp(const_str, "SAHPI_SYSTEM_BOOT_INITIATED") == 0) return (long long) SAHPI_SYSTEM_BOOT_INITIATED;
    if (strcmp(const_str, "SAHPI_BOOT_ERROR") == 0) return (long long) SAHPI_BOOT_ERROR;
    if (strcmp(const_str, "SAHPI_OS_BOOT") == 0) return (long long) SAHPI_OS_BOOT;
    if (strcmp(const_str, "SAHPI_OS_CRITICAL_STOP") == 0) return (long long) SAHPI_OS_CRITICAL_STOP;
    if (strcmp(const_str, "SAHPI_SLOT_CONNECTOR") == 0) return (long long) SAHPI_SLOT_CONNECTOR;
    if (strcmp(const_str, "SAHPI_SYSTEM_ACPI_POWER_STATE") == 0) return (long long) SAHPI_SYSTEM_ACPI_POWER_STATE;
    if (strcmp(const_str, "SAHPI_RESERVED2") == 0) return (long long) SAHPI_RESERVED2;
    if (strcmp(const_str, "SAHPI_PLATFORM_ALERT") == 0) return (long long) SAHPI_PLATFORM_ALERT;
    if (strcmp(const_str, "SAHPI_ENTITY_PRESENCE") == 0) return (long long) SAHPI_ENTITY_PRESENCE;
    if (strcmp(const_str, "SAHPI_MONITOR_ASIC_IC") == 0) return (long long) SAHPI_MONITOR_ASIC_IC;
    if (strcmp(const_str, "SAHPI_LAN") == 0) return (long long) SAHPI_LAN;
    if (strcmp(const_str, "SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH") == 0) return (long long) SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH;
    if (strcmp(const_str, "SAHPI_BATTERY") == 0) return (long long) SAHPI_BATTERY;
    if (strcmp(const_str, "SAHPI_OPERATIONAL") == 0) return (long long) SAHPI_OPERATIONAL;
    if (strcmp(const_str, "SAHPI_OEM_SENSOR") == 0) return (long long) SAHPI_OEM_SENSOR;
    if (strcmp(const_str, "SAHPI_SENSOR_BUFFER_LENGTH") == 0) return (long long) SAHPI_SENSOR_BUFFER_LENGTH;
    if (strcmp(const_str, "SAHPI_SENSOR_READING_TYPE_INT64") == 0) return (long long) SAHPI_SENSOR_READING_TYPE_INT64;
    if (strcmp(const_str, "SAHPI_SENSOR_READING_TYPE_UINT64") == 0) return (long long) SAHPI_SENSOR_READING_TYPE_UINT64;
    if (strcmp(const_str, "SAHPI_SENSOR_READING_TYPE_FLOAT64") == 0) return (long long) SAHPI_SENSOR_READING_TYPE_FLOAT64;
    if (strcmp(const_str, "SAHPI_SENSOR_READING_TYPE_BUFFER") == 0) return (long long) SAHPI_SENSOR_READING_TYPE_BUFFER;
    if (strcmp(const_str, "SAHPI_SENS_ADD_EVENTS_TO_MASKS") == 0) return (long long) SAHPI_SENS_ADD_EVENTS_TO_MASKS;
    if (strcmp(const_str, "SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS") == 0) return (long long) SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS;
    if (strcmp(const_str, "SAHPI_ALL_EVENT_STATES") == 0) return (long long) SAHPI_ALL_EVENT_STATES;
    if (strcmp(const_str, "SAHPI_SRF_MIN") == 0) return (long long) SAHPI_SRF_MIN;
    if (strcmp(const_str, "SAHPI_SRF_MAX") == 0) return (long long) SAHPI_SRF_MAX;
    if (strcmp(const_str, "SAHPI_SRF_NORMAL_MIN") == 0) return (long long) SAHPI_SRF_NORMAL_MIN;
    if (strcmp(const_str, "SAHPI_SRF_NORMAL_MAX") == 0) return (long long) SAHPI_SRF_NORMAL_MAX;
    if (strcmp(const_str, "SAHPI_SRF_NOMINAL") == 0) return (long long) SAHPI_SRF_NOMINAL;
    if (strcmp(const_str, "SAHPI_SU_UNSPECIFIED") == 0) return (long long) SAHPI_SU_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_SU_DEGREES_C") == 0) return (long long) SAHPI_SU_DEGREES_C;
    if (strcmp(const_str, "SAHPI_SU_DEGREES_F") == 0) return (long long) SAHPI_SU_DEGREES_F;
    if (strcmp(const_str, "SAHPI_SU_DEGREES_K") == 0) return (long long) SAHPI_SU_DEGREES_K;
    if (strcmp(const_str, "SAHPI_SU_VOLTS") == 0) return (long long) SAHPI_SU_VOLTS;
    if (strcmp(const_str, "SAHPI_SU_AMPS") == 0) return (long long) SAHPI_SU_AMPS;
    if (strcmp(const_str, "SAHPI_SU_WATTS") == 0) return (long long) SAHPI_SU_WATTS;
    if (strcmp(const_str, "SAHPI_SU_JOULES") == 0) return (long long) SAHPI_SU_JOULES;
    if (strcmp(const_str, "SAHPI_SU_COULOMBS") == 0) return (long long) SAHPI_SU_COULOMBS;
    if (strcmp(const_str, "SAHPI_SU_VA") == 0) return (long long) SAHPI_SU_VA;
    if (strcmp(const_str, "SAHPI_SU_NITS") == 0) return (long long) SAHPI_SU_NITS;
    if (strcmp(const_str, "SAHPI_SU_LUMEN") == 0) return (long long) SAHPI_SU_LUMEN;
    if (strcmp(const_str, "SAHPI_SU_LUX") == 0) return (long long) SAHPI_SU_LUX;
    if (strcmp(const_str, "SAHPI_SU_CANDELA") == 0) return (long long) SAHPI_SU_CANDELA;
    if (strcmp(const_str, "SAHPI_SU_KPA") == 0) return (long long) SAHPI_SU_KPA;
    if (strcmp(const_str, "SAHPI_SU_PSI") == 0) return (long long) SAHPI_SU_PSI;
    if (strcmp(const_str, "SAHPI_SU_NEWTON") == 0) return (long long) SAHPI_SU_NEWTON;
    if (strcmp(const_str, "SAHPI_SU_CFM") == 0) return (long long) SAHPI_SU_CFM;
    if (strcmp(const_str, "SAHPI_SU_RPM") == 0) return (long long) SAHPI_SU_RPM;
    if (strcmp(const_str, "SAHPI_SU_HZ") == 0) return (long long) SAHPI_SU_HZ;
    if (strcmp(const_str, "SAHPI_SU_MICROSECOND") == 0) return (long long) SAHPI_SU_MICROSECOND;
    if (strcmp(const_str, "SAHPI_SU_MILLISECOND") == 0) return (long long) SAHPI_SU_MILLISECOND;
    if (strcmp(const_str, "SAHPI_SU_SECOND") == 0) return (long long) SAHPI_SU_SECOND;
    if (strcmp(const_str, "SAHPI_SU_MINUTE") == 0) return (long long) SAHPI_SU_MINUTE;
    if (strcmp(const_str, "SAHPI_SU_HOUR") == 0) return (long long) SAHPI_SU_HOUR;
    if (strcmp(const_str, "SAHPI_SU_DAY") == 0) return (long long) SAHPI_SU_DAY;
    if (strcmp(const_str, "SAHPI_SU_WEEK") == 0) return (long long) SAHPI_SU_WEEK;
    if (strcmp(const_str, "SAHPI_SU_MIL") == 0) return (long long) SAHPI_SU_MIL;
    if (strcmp(const_str, "SAHPI_SU_INCHES") == 0) return (long long) SAHPI_SU_INCHES;
    if (strcmp(const_str, "SAHPI_SU_FEET") == 0) return (long long) SAHPI_SU_FEET;
    if (strcmp(const_str, "SAHPI_SU_CU_IN") == 0) return (long long) SAHPI_SU_CU_IN;
    if (strcmp(const_str, "SAHPI_SU_CU_FEET") == 0) return (long long) SAHPI_SU_CU_FEET;
    if (strcmp(const_str, "SAHPI_SU_MM") == 0) return (long long) SAHPI_SU_MM;
    if (strcmp(const_str, "SAHPI_SU_CM") == 0) return (long long) SAHPI_SU_CM;
    if (strcmp(const_str, "SAHPI_SU_M") == 0) return (long long) SAHPI_SU_M;
    if (strcmp(const_str, "SAHPI_SU_CU_CM") == 0) return (long long) SAHPI_SU_CU_CM;
    if (strcmp(const_str, "SAHPI_SU_CU_M") == 0) return (long long) SAHPI_SU_CU_M;
    if (strcmp(const_str, "SAHPI_SU_LITERS") == 0) return (long long) SAHPI_SU_LITERS;
    if (strcmp(const_str, "SAHPI_SU_FLUID_OUNCE") == 0) return (long long) SAHPI_SU_FLUID_OUNCE;
    if (strcmp(const_str, "SAHPI_SU_RADIANS") == 0) return (long long) SAHPI_SU_RADIANS;
    if (strcmp(const_str, "SAHPI_SU_STERADIANS") == 0) return (long long) SAHPI_SU_STERADIANS;
    if (strcmp(const_str, "SAHPI_SU_REVOLUTIONS") == 0) return (long long) SAHPI_SU_REVOLUTIONS;
    if (strcmp(const_str, "SAHPI_SU_CYCLES") == 0) return (long long) SAHPI_SU_CYCLES;
    if (strcmp(const_str, "SAHPI_SU_GRAVITIES") == 0) return (long long) SAHPI_SU_GRAVITIES;
    if (strcmp(const_str, "SAHPI_SU_OUNCE") == 0) return (long long) SAHPI_SU_OUNCE;
    if (strcmp(const_str, "SAHPI_SU_POUND") == 0) return (long long) SAHPI_SU_POUND;
    if (strcmp(const_str, "SAHPI_SU_FT_LB") == 0) return (long long) SAHPI_SU_FT_LB;
    if (strcmp(const_str, "SAHPI_SU_OZ_IN") == 0) return (long long) SAHPI_SU_OZ_IN;
    if (strcmp(const_str, "SAHPI_SU_GAUSS") == 0) return (long long) SAHPI_SU_GAUSS;
    if (strcmp(const_str, "SAHPI_SU_GILBERTS") == 0) return (long long) SAHPI_SU_GILBERTS;
    if (strcmp(const_str, "SAHPI_SU_HENRY") == 0) return (long long) SAHPI_SU_HENRY;
    if (strcmp(const_str, "SAHPI_SU_MILLIHENRY") == 0) return (long long) SAHPI_SU_MILLIHENRY;
    if (strcmp(const_str, "SAHPI_SU_FARAD") == 0) return (long long) SAHPI_SU_FARAD;
    if (strcmp(const_str, "SAHPI_SU_MICROFARAD") == 0) return (long long) SAHPI_SU_MICROFARAD;
    if (strcmp(const_str, "SAHPI_SU_OHMS") == 0) return (long long) SAHPI_SU_OHMS;
    if (strcmp(const_str, "SAHPI_SU_SIEMENS") == 0) return (long long) SAHPI_SU_SIEMENS;
    if (strcmp(const_str, "SAHPI_SU_MOLE") == 0) return (long long) SAHPI_SU_MOLE;
    if (strcmp(const_str, "SAHPI_SU_BECQUEREL") == 0) return (long long) SAHPI_SU_BECQUEREL;
    if (strcmp(const_str, "SAHPI_SU_PPM") == 0) return (long long) SAHPI_SU_PPM;
    if (strcmp(const_str, "SAHPI_SU_RESERVED") == 0) return (long long) SAHPI_SU_RESERVED;
    if (strcmp(const_str, "SAHPI_SU_DECIBELS") == 0) return (long long) SAHPI_SU_DECIBELS;
    if (strcmp(const_str, "SAHPI_SU_DBA") == 0) return (long long) SAHPI_SU_DBA;
    if (strcmp(const_str, "SAHPI_SU_DBC") == 0) return (long long) SAHPI_SU_DBC;
    if (strcmp(const_str, "SAHPI_SU_GRAY") == 0) return (long long) SAHPI_SU_GRAY;
    if (strcmp(const_str, "SAHPI_SU_SIEVERT") == 0) return (long long) SAHPI_SU_SIEVERT;
    if (strcmp(const_str, "SAHPI_SU_COLOR_TEMP_DEG_K") == 0) return (long long) SAHPI_SU_COLOR_TEMP_DEG_K;
    if (strcmp(const_str, "SAHPI_SU_BIT") == 0) return (long long) SAHPI_SU_BIT;
    if (strcmp(const_str, "SAHPI_SU_KILOBIT") == 0) return (long long) SAHPI_SU_KILOBIT;
    if (strcmp(const_str, "SAHPI_SU_MEGABIT") == 0) return (long long) SAHPI_SU_MEGABIT;
    if (strcmp(const_str, "SAHPI_SU_GIGABIT") == 0) return (long long) SAHPI_SU_GIGABIT;
    if (strcmp(const_str, "SAHPI_SU_BYTE") == 0) return (long long) SAHPI_SU_BYTE;
    if (strcmp(const_str, "SAHPI_SU_KILOBYTE") == 0) return (long long) SAHPI_SU_KILOBYTE;
    if (strcmp(const_str, "SAHPI_SU_MEGABYTE") == 0) return (long long) SAHPI_SU_MEGABYTE;
    if (strcmp(const_str, "SAHPI_SU_GIGABYTE") == 0) return (long long) SAHPI_SU_GIGABYTE;
    if (strcmp(const_str, "SAHPI_SU_WORD") == 0) return (long long) SAHPI_SU_WORD;
    if (strcmp(const_str, "SAHPI_SU_DWORD") == 0) return (long long) SAHPI_SU_DWORD;
    if (strcmp(const_str, "SAHPI_SU_QWORD") == 0) return (long long) SAHPI_SU_QWORD;
    if (strcmp(const_str, "SAHPI_SU_LINE") == 0) return (long long) SAHPI_SU_LINE;
    if (strcmp(const_str, "SAHPI_SU_HIT") == 0) return (long long) SAHPI_SU_HIT;
    if (strcmp(const_str, "SAHPI_SU_MISS") == 0) return (long long) SAHPI_SU_MISS;
    if (strcmp(const_str, "SAHPI_SU_RETRY") == 0) return (long long) SAHPI_SU_RETRY;
    if (strcmp(const_str, "SAHPI_SU_RESET") == 0) return (long long) SAHPI_SU_RESET;
    if (strcmp(const_str, "SAHPI_SU_OVERRUN") == 0) return (long long) SAHPI_SU_OVERRUN;
    if (strcmp(const_str, "SAHPI_SU_UNDERRUN") == 0) return (long long) SAHPI_SU_UNDERRUN;
    if (strcmp(const_str, "SAHPI_SU_COLLISION") == 0) return (long long) SAHPI_SU_COLLISION;
    if (strcmp(const_str, "SAHPI_SU_PACKETS") == 0) return (long long) SAHPI_SU_PACKETS;
    if (strcmp(const_str, "SAHPI_SU_MESSAGES") == 0) return (long long) SAHPI_SU_MESSAGES;
    if (strcmp(const_str, "SAHPI_SU_CHARACTERS") == 0) return (long long) SAHPI_SU_CHARACTERS;
    if (strcmp(const_str, "SAHPI_SU_ERRORS") == 0) return (long long) SAHPI_SU_ERRORS;
    if (strcmp(const_str, "SAHPI_SU_CORRECTABLE_ERRORS") == 0) return (long long) SAHPI_SU_CORRECTABLE_ERRORS;
    if (strcmp(const_str, "SAHPI_SU_UNCORRECTABLE_ERRORS") == 0) return (long long) SAHPI_SU_UNCORRECTABLE_ERRORS;
    if (strcmp(const_str, "SAHPI_SMUU_NONE") == 0) return (long long) SAHPI_SMUU_NONE;
    if (strcmp(const_str, "SAHPI_SMUU_BASIC_OVER_MODIFIER") == 0) return (long long) SAHPI_SMUU_BASIC_OVER_MODIFIER;
    if (strcmp(const_str, "SAHPI_SMUU_BASIC_TIMES_MODIFIER") == 0) return (long long) SAHPI_SMUU_BASIC_TIMES_MODIFIER;
    if (strcmp(const_str, "SAHPI_STM_LOW_MINOR") == 0) return (long long) SAHPI_STM_LOW_MINOR;
    if (strcmp(const_str, "SAHPI_STM_LOW_MAJOR") == 0) return (long long) SAHPI_STM_LOW_MAJOR;
    if (strcmp(const_str, "SAHPI_STM_LOW_CRIT") == 0) return (long long) SAHPI_STM_LOW_CRIT;
    if (strcmp(const_str, "SAHPI_STM_UP_MINOR") == 0) return (long long) SAHPI_STM_UP_MINOR;
    if (strcmp(const_str, "SAHPI_STM_UP_MAJOR") == 0) return (long long) SAHPI_STM_UP_MAJOR;
    if (strcmp(const_str, "SAHPI_STM_UP_CRIT") == 0) return (long long) SAHPI_STM_UP_CRIT;
    if (strcmp(const_str, "SAHPI_STM_UP_HYSTERESIS") == 0) return (long long) SAHPI_STM_UP_HYSTERESIS;
    if (strcmp(const_str, "SAHPI_STM_LOW_HYSTERESIS") == 0) return (long long) SAHPI_STM_LOW_HYSTERESIS;
    if (strcmp(const_str, "SAHPI_SEC_PER_EVENT") == 0) return (long long) SAHPI_SEC_PER_EVENT;
    if (strcmp(const_str, "SAHPI_SEC_READ_ONLY_MASKS") == 0) return (long long) SAHPI_SEC_READ_ONLY_MASKS;
    if (strcmp(const_str, "SAHPI_SEC_READ_ONLY") == 0) return (long long) SAHPI_SEC_READ_ONLY;
    if (strcmp(const_str, "SAHPI_DEFAGSENS_OPER") == 0) return (long long) SAHPI_DEFAGSENS_OPER;
    if (strcmp(const_str, "SAHPI_DEFAGSENS_PWR") == 0) return (long long) SAHPI_DEFAGSENS_PWR;
    if (strcmp(const_str, "SAHPI_DEFAGSENS_TEMP") == 0) return (long long) SAHPI_DEFAGSENS_TEMP;
    if (strcmp(const_str, "SAHPI_DEFAGSENS_MIN") == 0) return (long long) SAHPI_DEFAGSENS_MIN;
    if (strcmp(const_str, "SAHPI_DEFAGSENS_MAX") == 0) return (long long) SAHPI_DEFAGSENS_MAX;
    if (strcmp(const_str, "SAHPI_CTRL_TYPE_DIGITAL") == 0) return (long long) SAHPI_CTRL_TYPE_DIGITAL;
    if (strcmp(const_str, "SAHPI_CTRL_TYPE_DISCRETE") == 0) return (long long) SAHPI_CTRL_TYPE_DISCRETE;
    if (strcmp(const_str, "SAHPI_CTRL_TYPE_ANALOG") == 0) return (long long) SAHPI_CTRL_TYPE_ANALOG;
    if (strcmp(const_str, "SAHPI_CTRL_TYPE_STREAM") == 0) return (long long) SAHPI_CTRL_TYPE_STREAM;
    if (strcmp(const_str, "SAHPI_CTRL_TYPE_TEXT") == 0) return (long long) SAHPI_CTRL_TYPE_TEXT;
    if (strcmp(const_str, "SAHPI_CTRL_TYPE_OEM") == 0) return (long long) SAHPI_CTRL_TYPE_OEM;
    if (strcmp(const_str, "SAHPI_CTRL_STATE_OFF") == 0) return (long long) SAHPI_CTRL_STATE_OFF;
    if (strcmp(const_str, "SAHPI_CTRL_STATE_ON") == 0) return (long long) SAHPI_CTRL_STATE_ON;
    if (strcmp(const_str, "SAHPI_CTRL_STATE_PULSE_OFF") == 0) return (long long) SAHPI_CTRL_STATE_PULSE_OFF;
    if (strcmp(const_str, "SAHPI_CTRL_STATE_PULSE_ON") == 0) return (long long) SAHPI_CTRL_STATE_PULSE_ON;
    if (strcmp(const_str, "SAHPI_CTRL_MAX_STREAM_LENGTH") == 0) return (long long) SAHPI_CTRL_MAX_STREAM_LENGTH;
    if (strcmp(const_str, "SAHPI_TLN_ALL_LINES") == 0) return (long long) SAHPI_TLN_ALL_LINES;
    if (strcmp(const_str, "SAHPI_CTRL_MAX_OEM_BODY_LENGTH") == 0) return (long long) SAHPI_CTRL_MAX_OEM_BODY_LENGTH;
    if (strcmp(const_str, "SAHPI_CTRL_MODE_AUTO") == 0) return (long long) SAHPI_CTRL_MODE_AUTO;
    if (strcmp(const_str, "SAHPI_CTRL_MODE_MANUAL") == 0) return (long long) SAHPI_CTRL_MODE_MANUAL;
    if (strcmp(const_str, "SAHPI_CTRL_GENERIC") == 0) return (long long) SAHPI_CTRL_GENERIC;
    if (strcmp(const_str, "SAHPI_CTRL_LED") == 0) return (long long) SAHPI_CTRL_LED;
    if (strcmp(const_str, "SAHPI_CTRL_FAN_SPEED") == 0) return (long long) SAHPI_CTRL_FAN_SPEED;
    if (strcmp(const_str, "SAHPI_CTRL_DRY_CONTACT_CLOSURE") == 0) return (long long) SAHPI_CTRL_DRY_CONTACT_CLOSURE;
    if (strcmp(const_str, "SAHPI_CTRL_POWER_SUPPLY_INHIBIT") == 0) return (long long) SAHPI_CTRL_POWER_SUPPLY_INHIBIT;
    if (strcmp(const_str, "SAHPI_CTRL_AUDIBLE") == 0) return (long long) SAHPI_CTRL_AUDIBLE;
    if (strcmp(const_str, "SAHPI_CTRL_FRONT_PANEL_LOCKOUT") == 0) return (long long) SAHPI_CTRL_FRONT_PANEL_LOCKOUT;
    if (strcmp(const_str, "SAHPI_CTRL_POWER_INTERLOCK") == 0) return (long long) SAHPI_CTRL_POWER_INTERLOCK;
    if (strcmp(const_str, "SAHPI_CTRL_POWER_STATE") == 0) return (long long) SAHPI_CTRL_POWER_STATE;
    if (strcmp(const_str, "SAHPI_CTRL_LCD_DISPLAY") == 0) return (long long) SAHPI_CTRL_LCD_DISPLAY;
    if (strcmp(const_str, "SAHPI_CTRL_OEM") == 0) return (long long) SAHPI_CTRL_OEM;
    if (strcmp(const_str, "SAHPI_CTRL_OEM_CONFIG_LENGTH") == 0) return (long long) SAHPI_CTRL_OEM_CONFIG_LENGTH;
    if (strcmp(const_str, "SAHPI_DEFAULT_INVENTORY_ID") == 0) return (long long) SAHPI_DEFAULT_INVENTORY_ID;
    if (strcmp(const_str, "SAHPI_IDR_AREATYPE_INTERNAL_USE") == 0) return (long long) SAHPI_IDR_AREATYPE_INTERNAL_USE;
    if (strcmp(const_str, "SAHPI_IDR_AREATYPE_CHASSIS_INFO") == 0) return (long long) SAHPI_IDR_AREATYPE_CHASSIS_INFO;
    if (strcmp(const_str, "SAHPI_IDR_AREATYPE_BOARD_INFO") == 0) return (long long) SAHPI_IDR_AREATYPE_BOARD_INFO;
    if (strcmp(const_str, "SAHPI_IDR_AREATYPE_PRODUCT_INFO") == 0) return (long long) SAHPI_IDR_AREATYPE_PRODUCT_INFO;
    if (strcmp(const_str, "SAHPI_IDR_AREATYPE_OEM") == 0) return (long long) SAHPI_IDR_AREATYPE_OEM;
    if (strcmp(const_str, "SAHPI_IDR_AREATYPE_UNSPECIFIED") == 0) return (long long) SAHPI_IDR_AREATYPE_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE") == 0) return (long long) SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_MFG_DATETIME") == 0) return (long long) SAHPI_IDR_FIELDTYPE_MFG_DATETIME;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_MANUFACTURER") == 0) return (long long) SAHPI_IDR_FIELDTYPE_MANUFACTURER;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_PRODUCT_NAME") == 0) return (long long) SAHPI_IDR_FIELDTYPE_PRODUCT_NAME;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION") == 0) return (long long) SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER") == 0) return (long long) SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_PART_NUMBER") == 0) return (long long) SAHPI_IDR_FIELDTYPE_PART_NUMBER;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_FILE_ID") == 0) return (long long) SAHPI_IDR_FIELDTYPE_FILE_ID;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_ASSET_TAG") == 0) return (long long) SAHPI_IDR_FIELDTYPE_ASSET_TAG;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_CUSTOM") == 0) return (long long) SAHPI_IDR_FIELDTYPE_CUSTOM;
    if (strcmp(const_str, "SAHPI_IDR_FIELDTYPE_UNSPECIFIED") == 0) return (long long) SAHPI_IDR_FIELDTYPE_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_DEFAULT_WATCHDOG_NUM") == 0) return (long long) SAHPI_DEFAULT_WATCHDOG_NUM;
    if (strcmp(const_str, "SAHPI_WA_NO_ACTION") == 0) return (long long) SAHPI_WA_NO_ACTION;
    if (strcmp(const_str, "SAHPI_WA_RESET") == 0) return (long long) SAHPI_WA_RESET;
    if (strcmp(const_str, "SAHPI_WA_POWER_DOWN") == 0) return (long long) SAHPI_WA_POWER_DOWN;
    if (strcmp(const_str, "SAHPI_WA_POWER_CYCLE") == 0) return (long long) SAHPI_WA_POWER_CYCLE;
    if (strcmp(const_str, "SAHPI_WAE_NO_ACTION") == 0) return (long long) SAHPI_WAE_NO_ACTION;
    if (strcmp(const_str, "SAHPI_WAE_RESET") == 0) return (long long) SAHPI_WAE_RESET;
    if (strcmp(const_str, "SAHPI_WAE_POWER_DOWN") == 0) return (long long) SAHPI_WAE_POWER_DOWN;
    if (strcmp(const_str, "SAHPI_WAE_POWER_CYCLE") == 0) return (long long) SAHPI_WAE_POWER_CYCLE;
    if (strcmp(const_str, "SAHPI_WAE_TIMER_INT") == 0) return (long long) SAHPI_WAE_TIMER_INT;
    if (strcmp(const_str, "SAHPI_WPI_NONE") == 0) return (long long) SAHPI_WPI_NONE;
    if (strcmp(const_str, "SAHPI_WPI_SMI") == 0) return (long long) SAHPI_WPI_SMI;
    if (strcmp(const_str, "SAHPI_WPI_NMI") == 0) return (long long) SAHPI_WPI_NMI;
    if (strcmp(const_str, "SAHPI_WPI_MESSAGE_INTERRUPT") == 0) return (long long) SAHPI_WPI_MESSAGE_INTERRUPT;
    if (strcmp(const_str, "SAHPI_WPI_OEM") == 0) return (long long) SAHPI_WPI_OEM;
    if (strcmp(const_str, "SAHPI_WTU_NONE") == 0) return (long long) SAHPI_WTU_NONE;
    if (strcmp(const_str, "SAHPI_WTU_BIOS_FRB2") == 0) return (long long) SAHPI_WTU_BIOS_FRB2;
    if (strcmp(const_str, "SAHPI_WTU_BIOS_POST") == 0) return (long long) SAHPI_WTU_BIOS_POST;
    if (strcmp(const_str, "SAHPI_WTU_OS_LOAD") == 0) return (long long) SAHPI_WTU_OS_LOAD;
    if (strcmp(const_str, "SAHPI_WTU_SMS_OS") == 0) return (long long) SAHPI_WTU_SMS_OS;
    if (strcmp(const_str, "SAHPI_WTU_OEM") == 0) return (long long) SAHPI_WTU_OEM;
    if (strcmp(const_str, "SAHPI_WTU_UNSPECIFIED") == 0) return (long long) SAHPI_WTU_UNSPECIFIED;
    if (strcmp(const_str, "SAHPI_WATCHDOG_EXP_BIOS_FRB2") == 0) return (long long) SAHPI_WATCHDOG_EXP_BIOS_FRB2;
    if (strcmp(const_str, "SAHPI_WATCHDOG_EXP_BIOS_POST") == 0) return (long long) SAHPI_WATCHDOG_EXP_BIOS_POST;
    if (strcmp(const_str, "SAHPI_WATCHDOG_EXP_OS_LOAD") == 0) return (long long) SAHPI_WATCHDOG_EXP_OS_LOAD;
    if (strcmp(const_str, "SAHPI_WATCHDOG_EXP_SMS_OS") == 0) return (long long) SAHPI_WATCHDOG_EXP_SMS_OS;
    if (strcmp(const_str, "SAHPI_WATCHDOG_EXP_OEM") == 0) return (long long) SAHPI_WATCHDOG_EXP_OEM;
    if (strcmp(const_str, "SAHPI_HS_INDICATOR_OFF") == 0) return (long long) SAHPI_HS_INDICATOR_OFF;
    if (strcmp(const_str, "SAHPI_HS_INDICATOR_ON") == 0) return (long long) SAHPI_HS_INDICATOR_ON;
    if (strcmp(const_str, "SAHPI_HS_ACTION_INSERTION") == 0) return (long long) SAHPI_HS_ACTION_INSERTION;
    if (strcmp(const_str, "SAHPI_HS_ACTION_EXTRACTION") == 0) return (long long) SAHPI_HS_ACTION_EXTRACTION;
    if (strcmp(const_str, "SAHPI_HS_STATE_INACTIVE") == 0) return (long long) SAHPI_HS_STATE_INACTIVE;
    if (strcmp(const_str, "SAHPI_HS_STATE_INSERTION_PENDING") == 0) return (long long) SAHPI_HS_STATE_INSERTION_PENDING;
    if (strcmp(const_str, "SAHPI_HS_STATE_ACTIVE") == 0) return (long long) SAHPI_HS_STATE_ACTIVE;
    if (strcmp(const_str, "SAHPI_HS_STATE_EXTRACTION_PENDING") == 0) return (long long) SAHPI_HS_STATE_EXTRACTION_PENDING;
    if (strcmp(const_str, "SAHPI_HS_STATE_NOT_PRESENT") == 0) return (long long) SAHPI_HS_STATE_NOT_PRESENT;
    if (strcmp(const_str, "SAHPI_CRITICAL") == 0) return (long long) SAHPI_CRITICAL;
    if (strcmp(const_str, "SAHPI_MAJOR") == 0) return (long long) SAHPI_MAJOR;
    if (strcmp(const_str, "SAHPI_MINOR") == 0) return (long long) SAHPI_MINOR;
    if (strcmp(const_str, "SAHPI_INFORMATIONAL") == 0) return (long long) SAHPI_INFORMATIONAL;
    if (strcmp(const_str, "SAHPI_OK") == 0) return (long long) SAHPI_OK;
    if (strcmp(const_str, "SAHPI_DEBUG") == 0) return (long long) SAHPI_DEBUG;
    if (strcmp(const_str, "SAHPI_ALL_SEVERITIES") == 0) return (long long) SAHPI_ALL_SEVERITIES;
    if (strcmp(const_str, "SAHPI_RESE_RESOURCE_FAILURE") == 0) return (long long) SAHPI_RESE_RESOURCE_FAILURE;
    if (strcmp(const_str, "SAHPI_RESE_RESOURCE_RESTORED") == 0) return (long long) SAHPI_RESE_RESOURCE_RESTORED;
    if (strcmp(const_str, "SAHPI_RESE_RESOURCE_ADDED") == 0) return (long long) SAHPI_RESE_RESOURCE_ADDED;
    if (strcmp(const_str, "SAHPI_DOMAIN_REF_ADDED") == 0) return (long long) SAHPI_DOMAIN_REF_ADDED;
    if (strcmp(const_str, "SAHPI_DOMAIN_REF_REMOVED") == 0) return (long long) SAHPI_DOMAIN_REF_REMOVED;
    if (strcmp(const_str, "SAHPI_SOD_TRIGGER_READING") == 0) return (long long) SAHPI_SOD_TRIGGER_READING;
    if (strcmp(const_str, "SAHPI_SOD_TRIGGER_THRESHOLD") == 0) return (long long) SAHPI_SOD_TRIGGER_THRESHOLD;
    if (strcmp(const_str, "SAHPI_SOD_OEM") == 0) return (long long) SAHPI_SOD_OEM;
    if (strcmp(const_str, "SAHPI_SOD_PREVIOUS_STATE") == 0) return (long long) SAHPI_SOD_PREVIOUS_STATE;
    if (strcmp(const_str, "SAHPI_SOD_CURRENT_STATE") == 0) return (long long) SAHPI_SOD_CURRENT_STATE;
    if (strcmp(const_str, "SAHPI_SOD_SENSOR_SPECIFIC") == 0) return (long long) SAHPI_SOD_SENSOR_SPECIFIC;
    if (strcmp(const_str, "SAHPI_SEOD_CURRENT_STATE") == 0) return (long long) SAHPI_SEOD_CURRENT_STATE;
    if (strcmp(const_str, "SAHPI_HPIE_AUDIT") == 0) return (long long) SAHPI_HPIE_AUDIT;
    if (strcmp(const_str, "SAHPI_HPIE_STARTUP") == 0) return (long long) SAHPI_HPIE_STARTUP;
    if (strcmp(const_str, "SAHPI_HPIE_OTHER") == 0) return (long long) SAHPI_HPIE_OTHER;
    if (strcmp(const_str, "SAHPI_ET_RESOURCE") == 0) return (long long) SAHPI_ET_RESOURCE;
    if (strcmp(const_str, "SAHPI_ET_DOMAIN") == 0) return (long long) SAHPI_ET_DOMAIN;
    if (strcmp(const_str, "SAHPI_ET_SENSOR") == 0) return (long long) SAHPI_ET_SENSOR;
    if (strcmp(const_str, "SAHPI_ET_SENSOR_ENABLE_CHANGE") == 0) return (long long) SAHPI_ET_SENSOR_ENABLE_CHANGE;
    if (strcmp(const_str, "SAHPI_ET_HOTSWAP") == 0) return (long long) SAHPI_ET_HOTSWAP;
    if (strcmp(const_str, "SAHPI_ET_WATCHDOG") == 0) return (long long) SAHPI_ET_WATCHDOG;
    if (strcmp(const_str, "SAHPI_ET_HPI_SW") == 0) return (long long) SAHPI_ET_HPI_SW;
    if (strcmp(const_str, "SAHPI_ET_OEM") == 0) return (long long) SAHPI_ET_OEM;
    if (strcmp(const_str, "SAHPI_ET_USER") == 0) return (long long) SAHPI_ET_USER;
    if (strcmp(const_str, "SAHPI_EVT_QUEUE_OVERFLOW") == 0) return (long long) SAHPI_EVT_QUEUE_OVERFLOW;
    if (strcmp(const_str, "SA_HPI_MAX_NAME_LENGTH") == 0) return (long long) SA_HPI_MAX_NAME_LENGTH;
    if (strcmp(const_str, "SAHPI_STATUS_COND_TYPE_SENSOR") == 0) return (long long) SAHPI_STATUS_COND_TYPE_SENSOR;
    if (strcmp(const_str, "SAHPI_STATUS_COND_TYPE_RESOURCE") == 0) return (long long) SAHPI_STATUS_COND_TYPE_RESOURCE;
    if (strcmp(const_str, "SAHPI_STATUS_COND_TYPE_OEM") == 0) return (long long) SAHPI_STATUS_COND_TYPE_OEM;
    if (strcmp(const_str, "SAHPI_STATUS_COND_TYPE_USER") == 0) return (long long) SAHPI_STATUS_COND_TYPE_USER;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_MODE_AUTO") == 0) return (long long) SAHPI_ANNUNCIATOR_MODE_AUTO;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_MODE_USER") == 0) return (long long) SAHPI_ANNUNCIATOR_MODE_USER;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_MODE_SHARED") == 0) return (long long) SAHPI_ANNUNCIATOR_MODE_SHARED;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_TYPE_LED") == 0) return (long long) SAHPI_ANNUNCIATOR_TYPE_LED;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE") == 0) return (long long) SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_TYPE_AUDIBLE") == 0) return (long long) SAHPI_ANNUNCIATOR_TYPE_AUDIBLE;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY") == 0) return (long long) SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_TYPE_MESSAGE") == 0) return (long long) SAHPI_ANNUNCIATOR_TYPE_MESSAGE;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_TYPE_COMPOSITE") == 0) return (long long) SAHPI_ANNUNCIATOR_TYPE_COMPOSITE;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_TYPE_OEM") == 0) return (long long) SAHPI_ANNUNCIATOR_TYPE_OEM;
    if (strcmp(const_str, "SAHPI_NO_RECORD") == 0) return (long long) SAHPI_NO_RECORD;
    if (strcmp(const_str, "SAHPI_CTRL_RDR") == 0) return (long long) SAHPI_CTRL_RDR;
    if (strcmp(const_str, "SAHPI_SENSOR_RDR") == 0) return (long long) SAHPI_SENSOR_RDR;
    if (strcmp(const_str, "SAHPI_INVENTORY_RDR") == 0) return (long long) SAHPI_INVENTORY_RDR;
    if (strcmp(const_str, "SAHPI_WATCHDOG_RDR") == 0) return (long long) SAHPI_WATCHDOG_RDR;
    if (strcmp(const_str, "SAHPI_ANNUNCIATOR_RDR") == 0) return (long long) SAHPI_ANNUNCIATOR_RDR;
    if (strcmp(const_str, "SAHPI_DEFAULT_PARM") == 0) return (long long) SAHPI_DEFAULT_PARM;
    if (strcmp(const_str, "SAHPI_SAVE_PARM") == 0) return (long long) SAHPI_SAVE_PARM;
    if (strcmp(const_str, "SAHPI_RESTORE_PARM") == 0) return (long long) SAHPI_RESTORE_PARM;
    if (strcmp(const_str, "SAHPI_COLD_RESET") == 0) return (long long) SAHPI_COLD_RESET;
    if (strcmp(const_str, "SAHPI_WARM_RESET") == 0) return (long long) SAHPI_WARM_RESET;
    if (strcmp(const_str, "SAHPI_RESET_ASSERT") == 0) return (long long) SAHPI_RESET_ASSERT;
    if (strcmp(const_str, "SAHPI_RESET_DEASSERT") == 0) return (long long) SAHPI_RESET_DEASSERT;
    if (strcmp(const_str, "SAHPI_POWER_OFF") == 0) return (long long) SAHPI_POWER_OFF;
    if (strcmp(const_str, "SAHPI_POWER_ON") == 0) return (long long) SAHPI_POWER_ON;
    if (strcmp(const_str, "SAHPI_POWER_CYCLE") == 0) return (long long) SAHPI_POWER_CYCLE;
    if (strcmp(const_str, "SAHPI_CAPABILITY_RESOURCE") == 0) return (long long) SAHPI_CAPABILITY_RESOURCE;
    if (strcmp(const_str, "SAHPI_CAPABILITY_EVT_DEASSERTS") == 0) return (long long) SAHPI_CAPABILITY_EVT_DEASSERTS;
    if (strcmp(const_str, "SAHPI_CAPABILITY_AGGREGATE_STATUS") == 0) return (long long) SAHPI_CAPABILITY_AGGREGATE_STATUS;
    if (strcmp(const_str, "SAHPI_CAPABILITY_CONFIGURATION") == 0) return (long long) SAHPI_CAPABILITY_CONFIGURATION;
    if (strcmp(const_str, "SAHPI_CAPABILITY_MANAGED_HOTSWAP") == 0) return (long long) SAHPI_CAPABILITY_MANAGED_HOTSWAP;
    if (strcmp(const_str, "SAHPI_CAPABILITY_WATCHDOG") == 0) return (long long) SAHPI_CAPABILITY_WATCHDOG;
    if (strcmp(const_str, "SAHPI_CAPABILITY_CONTROL") == 0) return (long long) SAHPI_CAPABILITY_CONTROL;
    if (strcmp(const_str, "SAHPI_CAPABILITY_FRU") == 0) return (long long) SAHPI_CAPABILITY_FRU;
    if (strcmp(const_str, "SAHPI_CAPABILITY_ANNUNCIATOR") == 0) return (long long) SAHPI_CAPABILITY_ANNUNCIATOR;
    if (strcmp(const_str, "SAHPI_CAPABILITY_POWER") == 0) return (long long) SAHPI_CAPABILITY_POWER;
    if (strcmp(const_str, "SAHPI_CAPABILITY_RESET") == 0) return (long long) SAHPI_CAPABILITY_RESET;
    if (strcmp(const_str, "SAHPI_CAPABILITY_INVENTORY_DATA") == 0) return (long long) SAHPI_CAPABILITY_INVENTORY_DATA;
    if (strcmp(const_str, "SAHPI_CAPABILITY_EVENT_LOG") == 0) return (long long) SAHPI_CAPABILITY_EVENT_LOG;
    if (strcmp(const_str, "SAHPI_CAPABILITY_RDR") == 0) return (long long) SAHPI_CAPABILITY_RDR;
    if (strcmp(const_str, "SAHPI_CAPABILITY_SENSOR") == 0) return (long long) SAHPI_CAPABILITY_SENSOR;
    if (strcmp(const_str, "SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY") == 0) return (long long) SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
    if (strcmp(const_str, "SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED") == 0) return (long long) SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED;
    if (strcmp(const_str, "SAHPI_DOMAIN_CAP_AUTOINSERT_READ_ONLY") == 0) return (long long) SAHPI_DOMAIN_CAP_AUTOINSERT_READ_ONLY;
    if (strcmp(const_str, "SAHPI_EL_OVERFLOW_DROP") == 0) return (long long) SAHPI_EL_OVERFLOW_DROP;
    if (strcmp(const_str, "SAHPI_EL_OVERFLOW_OVERWRITE") == 0) return (long long) SAHPI_EL_OVERFLOW_OVERWRITE;
    if (strcmp(const_str, "SAHPI_OLDEST_ENTRY") == 0) return (long long) SAHPI_OLDEST_ENTRY;
    if (strcmp(const_str, "SAHPI_NEWEST_ENTRY") == 0) return (long long) SAHPI_NEWEST_ENTRY;
    if (strcmp(const_str, "SAHPI_NO_MORE_ENTRIES") == 0) return (long long) SAHPI_NO_MORE_ENTRIES;
    return 0;
}

