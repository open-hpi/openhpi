/**
 * TODO: License
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Basic Data Types and Values 				      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_TYPES_H
#define HPI_TYPES_H


#include <sys/types.h>

/* General Types - need to be specified correctly for the host architecture */
/* TODO */
typedef u_int8_t  SaHpiUint8T;
typedef u_int16_t SaHpiUint16T;
typedef u_int32_t SaHpiUint32T;
typedef int8_t    SaHpiInt8T;
typedef int16_t   SaHpiInt16T;
typedef int32_t   SaHpiInt32T;
typedef int64_t   SaHpiInt64T;
typedef float SaHpiFloat32T; /* FIX!!!!!! */
typedef float SaHpiFloat64T; /* FIX!!!!!! */

typedef SaHpiUint8T SaHpiBoolT;

#define SAHPI_TRUE 1
#define SAHPI_FALSE 0

/* Platform, O/S, or Vendor dependent */
#define SAHPI_API
#define SAHPI_IN
#define SAHPI_OUT
#define SAHPI_INOUT
/*
** Identifier for the manufacturer
**
** This is the IANA-assigned private enterprise number for the
** manufacturer of the resource or FRU, or of the manufacturer
** defining an OEM control or event type. A list of current
** IANA-assigned private enterprise numbers may be obtained at
**
** http://www.iana.org/assignments/enterprise-numbers
**
** If a manufacturer does not currently have an assigned number, one
** may be obtained by following the instructions located at
**
** http://www.iana.org/cgi-bin/enterprise.pl
*/
typedef SaHpiUint32T SaHpiManufacturerIdT;
#define SAHPI_MANUFACTURER_ID_UNSPECIFIED (SaHpiManufacturerIdT)0

/* Version Types */
typedef SaHpiUint32T SaHpiVersionT;

/*
** Interface Version
**
** The interface version is the version of the actual interface and not the
** version of the implementation. It is a 24 bit value where
** the most significant 8 bits represent the compatibility level
** (with letters represented as the corresponding numbers);
** the next 8 bits represent the major version number; and
** the least significant 8 bits represent the minor version number.
*/
#define SAHPI_INTERFACE_VERSION (SaHpiVersionT)0x010101 /* A.01.01 */

/*
** Return Codes
**
** SaErrorT is defined in the HPI specification. In the future a
** common SAF types definition may be created to contain this type. At
** that time, this typedef should be removed.
*/

typedef SaHpiInt32T SaErrorT; /* Return code */

/*
** SA_OK: This code indicates that a command completed successfully.
*/
#define SA_OK (SaErrorT)0x0000

/* This value is the base for all HPI-specific error codes. */
#define SA_HPI_ERR_BASE -1000

/*
** SA_ERR_HPI_ERROR: An unspecified error occurred. This code should
** be returned only as a last resort; eg. if the cause of an error
** cannot be determined.
*/
#define SA_ERR_HPI_ERROR (SaErrorT)(SA_HPI_ERR_BASE - 1)
/*
** SA_ERR_HPI_UNSUPPORTED_API: The HPI implementation does not support
** this API. This code is appropriate, for example, if invoking the
** Hot Swap functions on an HPI implementation which has no hot swap
** support. Note that such an implementation would not report any hot
** swap capabilities via its RDRs.
*/
#define SA_ERR_HPI_UNSUPPORTED_API (SaErrorT)(SA_HPI_ERR_BASE - 2)
/*
** SA_ERR_HPI_BUSY: The command cannot be performed because the
** targeted device is busy.
*/
#define SA_ERR_HPI_BUSY (SaErrorT)(SA_HPI_ERR_BASE - 3)
/*
** SA_ERR_HPI_INVALID: The request is fundamentally invalid.
*/
#define SA_ERR_HPI_INVALID (SaErrorT)(SA_HPI_ERR_BASE - 4)
/*
** SA_ERR_HPI_INVALID_CMD: The specific object to which a command was
** directed does not support that command (which was otherwise valid).
*/
#define SA_ERR_HPI_INVALID_CMD (SaErrorT)(SA_HPI_ERR_BASE - 5)
/*
** SA_ERR_HPI_TIMEOUT: The requested operation, which had a timeout
** value specified, timed out. For example, when reading input with a
** timeout value, if no input arrives within the timeout interval,
** this code should be returned. This should only be returned in
** cases where a timeout is anticipated as a valid consequence of the
** operation; if the addressed entity is not responding due to a
** fault, use SA_ERR_HPI_NO_RESPONSE (qv).
*/
#define SA_ERR_HPI_TIMEOUT (SaErrorT)(SA_HPI_ERR_BASE - 6)
/*
** SA_ERR_HPI_OUT_OF_SPACE: The requested command failed due to
** resource limits.
*/
#define SA_ERR_HPI_OUT_OF_SPACE (SaErrorT)(SA_HPI_ERR_BASE - 7)
/*
** SA_ERR_HPI_DATA_TRUNCATED: The returned data was truncated. For
** example, when reading data into a fixed-size buffer, if the data is
** larger than the buffer, this code should be returned.
*/
#define SA_ERR_HPI_DATA_TRUNCATED (SaErrorT)(SA_HPI_ERR_BASE - 8)
/*
** SA_ERR_HPI_DATA_LEN_INVALID: The specified data length is invalid.
*/
#define SA_ERR_HPI_DATA_LEN_INVALID (SaErrorT)(SA_HPI_ERR_BASE - 9)
/*
** SA_ERR_HPI_DATA_EX_LIMITS: The supplied data exceeds limits.
*/
#define SA_ERR_HPI_DATA_EX_LIMITS (SaErrorT)(SA_HPI_ERR_BASE - 10)
/*
** SA_ERR_HPI_INVALID_PARAMS: One or more parameters to the command
** were invalid.
*/
#define SA_ERR_HPI_INVALID_PARAMS (SaErrorT)(SA_HPI_ERR_BASE - 11)
/*
** SA_ERR_HPI_INVALID_DATA: Cannot return requested data; eg. the
** specific object to which a command was directed does not support
** the data required by the command.
*/
#define SA_ERR_HPI_INVALID_DATA (SaErrorT)(SA_HPI_ERR_BASE - 12)
/*
** SA_ERR_HPI_NOT_PRESENT: The requested object was not present. For
** example, this code would be returned when attempting to access an
** entry in a RPT or RDR which is not present. As another example, this
** code would also be returned when accessing an invalid management
** instrument on a valid resource.
*/
#define SA_ERR_HPI_NOT_PRESENT (SaErrorT)(SA_HPI_ERR_BASE - 13)
/*
** SA_ERR_HPI_INVALID_DATA_FIELD: Invalid data field.
*/
#define SA_ERR_HPI_INVALID_DATA_FIELD (SaErrorT)(SA_HPI_ERR_BASE - 14)
/*
** SA_ERR_HPI_INVALID_SENSOR_CMD: Invalid sensor command.
*/
#define SA_ERR_HPI_INVALID_SENSOR_CMD (SaErrorT)(SA_HPI_ERR_BASE - 15)
/*
** SA_ERR_HPI_NO_RESPONSE: There was no response from the domain or
** object targeted by the command, due to some fault. This code
** indicates an un-anticipated failure to respond; compare with
** SA_ERR_HPI_TIMEOUT.
*/
#define SA_ERR_HPI_NO_RESPONSE (SaErrorT)(SA_HPI_ERR_BASE - 16)
/*
** SA_ERR_HPI_DUPLICATE: Duplicate request -- such as attempting to
** initialize something which has already been initialized (and which
** cannot be initialized twice).
*/
#define SA_ERR_HPI_DUPLICATE (SaErrorT)(SA_HPI_ERR_BASE - 17)
/*
** SA_ERR_HPI_UPDATING: The command could not be completed because
** the targeted object is in update mode.
*/
#define SA_ERR_HPI_UPDATING (SaErrorT)(SA_HPI_ERR_BASE - 18)
/*
** SA_ERR_HPI_INITIALIZING: The command could not be completed because
** the targeted object is initializing.
*/
#define SA_ERR_HPI_INITIALIZING (SaErrorT)(SA_HPI_ERR_BASE - 19)
/*
** SA_ERR_HPI_UNKNOWN: This code should be returned if, for some
** reason, the HPI implementation cannot determine the proper response
** to a command. For example, there may be a proper value to return
** for a given call, but the implementation may be unable to determine
** which one it is.
*/
#define SA_ERR_HPI_UNKNOWN (SaErrorT)(SA_HPI_ERR_BASE - 20)
/*
** SA_ERR_HPI_INVALID_SESSION: An invalid session ID was specified in
** the command.
*/
#define SA_ERR_HPI_INVALID_SESSION (SaErrorT)(SA_HPI_ERR_BASE - 21)
/*
** SA_ERR_HPI_INVALID_DOMAIN: Invalid domain ID specified -- ie. a
** domain ID which does not correspond to any real domain was
** specified in the command.
*/
#define SA_ERR_HPI_INVALID_DOMAIN (SaErrorT)(SA_HPI_ERR_BASE - 22)
/*
** SA_ERR_HPI_INVALID_RESOURCE: Invalid resource ID specified -- ie. a
** resource ID which does not correspond to a resource in the addressed
** domain was specified in the command.
*/
#define SA_ERR_HPI_INVALID_RESOURCE (SaErrorT)(SA_HPI_ERR_BASE - 23)
/*
** SA_ERR_HPI_INVALID_REQUEST: The request is invalid in the current
** context. An example would be attempting to unsubscribe for events,
** when the caller has not subscribed to events.
*/
#define SA_ERR_HPI_INVALID_REQUEST (SaErrorT)(SA_HPI_ERR_BASE - 24)
/*
** SA_ERR_HPI_ENTITY_NOT_PRESENT: The addressed management instrument is not active
** because the entity with which it is associated is not present. This
** condition could occur, for instance, when an alarm module is managing a
** fan tray FRU. The alarm module would contain management instruments (sensors,
** etc) for the fan tray. The fan tray may be removed, even though the
** management instruments are still represented in the alarm module. In this
** case, SA_ERR_HPI_ENTITY_NOT_PRESENT would be returned if a management instrument
** associated with a removed entity is accessed.
*/
#define SA_ERR_HPI_ENTITY_NOT_PRESENT (SaErrorT)(SA_HPI_ERR_BASE - 25)
/*
** SA_ERR_HPI_UNINITIALIZED: This code is returned when a request is
** made, and the HPI has not, yet, been initialized via saHpiInitialize().
*/
#define SA_ERR_HPI_UNINITIALIZED (SaErrorT)(SA_HPI_ERR_BASE - 26)
/*
** Domain, Session and Resource Type Definitions
*/
/* Domain ID. */
typedef SaHpiUint32T SaHpiDomainIdT;
#define SAHPI_DEFAULT_DOMAIN_ID (SaHpiDomainIdT)0

/* The SAHPI_UNSPECIFIED_DOMAIN_ID value may be used by an implementation
** when populating the ResourceId value for an RPT entry that is a
** domain only.
*/
#define SAHPI_UNSPECIFIED_DOMAIN_ID (SaHpiDomainIdT) 0xFFFFFFFF

/* Session ID. */
typedef SaHpiUint32T SaHpiSessionIdT;

/* Resource identifier. */
typedef SaHpiUint32T SaHpiResourceIdT;

/* The SAHPI_UNSPECIFIED_RESOURCE_ID value may be used by an implementation
** when populating the DomainId value for an RPT entry that is a
** resource only. Note that this reserved value (0xFFFFFFFF) is also used
** to designate the domain controller, for domain-based event log access.
*/
#define SAHPI_UNSPECIFIED_RESOURCE_ID (SaHpiResourceIdT) 0xFFFFFFFF

/* The SAHPI_DOMAIN_CONTROLLER_ID value is a reserved resource ID
** value which is used to select the domain controller?s event log
** (as opposed to a real resource?s event log) when accessing logs.
** This value must not be used as the ID of any real resource.
*/
#define SAHPI_DOMAIN_CONTROLLER_ID (SaHpiResourceIdT) 0xFFFFFFFE
/* Table Related Type Definitions */
typedef SaHpiUint32T SaHpiEntryIdT;
#define SAHPI_FIRST_ENTRY (SaHpiEntryIdT)0x00000000
#define SAHPI_LAST_ENTRY (SaHpiEntryIdT)0xFFFFFFFF
/*
** Time Related Type Definitions
**
** An HPI time value represents the local time as the number of nanoseconds
** from 00:00:00, January 1, 1970, in a 64-bit signed integer. This format
** is sufficient to represent times with nano-second resolution from the
** year 1678 to 2262. Every API which deals with time values must define
** the timezone used.
**
** It should be noted that although nano-second resolution is supported
** in the data type, the actual resolution provided by an implementation
** may be more limited than this.
**
** The value -2**63, which is 0x8000000000000000, is used to indicate
** ?unknown/unspecified time?.
**
** Conversion to/from POSIX and other common time representations is
** relatively straightforward. The following code framgment converts
** between SaHpiTimeT and time_t:
**
** time_t tt1, tt2;
** SaHpiTimeT saHpiTime;
**
** time(&tt1);
** saHpiTime = (SaHpiTimeT) tt1 * 1000000000;
** tt2 = saHpiTime / 1000000000;
**
** The following fragment converts between SaHpiTimeT and a struct timeval:
**
** struct timeval tv1, tv2;
** SaHpiTimeT saHpiTime;
**
** gettimeofday(&tv1, NULL);
** saHpiTime = (SaHpiTimeT) tv1.tv_sec * 1000000000 + tv1.tv_usec * 1000;
** tv2.tv_sec = saHpiTime / 1000000000;
** tv2.tv_usec = saHpiTime % 1000000000 / 1000;
**
** The following fragment converts between SaHpiTimeT and a struct timespec:
**
** struct timespec ts1, ts2;
** SaHpiTimeT saHpiTime;
**
** clock_gettime(CLOCK_REALTIME, &ts1);
** saHpiTime = (SaHpiTimeT) ts1.tv_sec * 1000000000 + ts1.tv_nsec;
** ts2.tv_sec = saHpiTime / 1000000000;
** ts2.tv_nsec = saHpiTime % 1000000000;
**
** Note, however, that since time_t is (effectively) universally 32 bits,
** all of these conversions will cease to work on January 18, 2038.
**
** Some subsystems may need the flexibility to report either absolute or
** relative (eg. to system boot) times. This will typically be in the
** case of a board which may or may not, depending on the system setup,
** have an idea of absolute time. For example, some boards may have
** ?time of day? clocks which start at zero, and never get set to the
** time of day.
**
** In these cases, times which represent ?current? time (in events, for
** example) can be reported based on the clock value, whether it has been
** set to the actual date/time, or whether it represents the elapsed time
** since boot. If it is the time since boot, the value will be (for 27
** years) less than 0x0C00000000000000, which is Mon May 26 16:58:48 1997.
** If the value is greater than this, then it can be assumed to be an
** absolute time.
**
** Every API which can report either absolute or relative times must
** state this rule clearly in its interface specification.
*/
typedef SaHpiInt64T SaHpiTimeT; /* Time in nanoseconds */
/* Unspecified or unknown time */
#define SAHPI_TIME_UNSPECIFIED (SaHpiTimeT) 0x8000000000000000
/* Maximum time that can be specified as relative */
#define SAHPI_TIME_MAX_RELATIVE (SaHpiTimeT) 0x0C00000000000000
typedef SaHpiInt64T SaHpiTimeoutT; /* Timeout in nanoseconds */
/* Non-blocking call */
#define SAHPI_TIMEOUT_IMMEDIATE (SaHpiTimeoutT) 0x0000000000000000
/* Blocking call, wait indefinitely for call to complete */
#define SAHPI_TIMEOUT_BLOCK (SaHpiTimeoutT) -1
/*
** Language
**
** This enumeration lists all of the languages that can be associated with text.
**
** SAHPI_LANG_UNDEF indicates that the language is unspecified or
** unknown.
*/
typedef enum {
	SAHPI_LANG_UNDEF = 0, SAHPI_LANG_AFAR, SAHPI_LANG_ABKHAZIAN,
	SAHPI_LANG_AFRIKAANS, SAHPI_LANG_AMHARIC, SAHPI_LANG_ARABIC,
	SAHPI_LANG_ASSAMESE, SAHPI_LANG_AYMARA, SAHPI_LANG_AZERBAIJANI,
	SAHPI_LANG_BASHKIR, SAHPI_LANG_BYELORUSSIAN, SAHPI_LANG_BULGARIAN,
	SAHPI_LANG_BIHARI, SAHPI_LANG_BISLAMA, SAHPI_LANG_BENGALI,
	SAHPI_LANG_TIBETAN, SAHPI_LANG_BRETON, SAHPI_LANG_CATALAN,
	SAHPI_LANG_CORSICAN, SAHPI_LANG_CZECH, SAHPI_LANG_WELSH,
	SAHPI_LANG_DANISH, SAHPI_LANG_GERMAN, SAHPI_LANG_BHUTANI,
	SAHPI_LANG_GREEK, SAHPI_LANG_ENGLISH, SAHPI_LANG_ESPERANTO,
	SAHPI_LANG_SPANISH, SAHPI_LANG_ESTONIAN, SAHPI_LANG_BASQUE,
	SAHPI_LANG_PERSIAN, SAHPI_LANG_FINNISH, SAHPI_LANG_FIJI,
	SAHPI_LANG_FAEROESE, SAHPI_LANG_FRENCH, SAHPI_LANG_FRISIAN,
	SAHPI_LANG_IRISH, SAHPI_LANG_SCOTSGAELIC, SAHPI_LANG_GALICIAN,
	SAHPI_LANG_GUARANI, SAHPI_LANG_GUJARATI, SAHPI_LANG_HAUSA,
	SAHPI_LANG_HINDI, SAHPI_LANG_CROATIAN, SAHPI_LANG_HUNGARIAN,
	SAHPI_LANG_ARMENIAN, SAHPI_LANG_INTERLINGUA, SAHPI_LANG_INTERLINGUE,
	SAHPI_LANG_INUPIAK, SAHPI_LANG_INDONESIAN, SAHPI_LANG_ICELANDIC,
	SAHPI_LANG_ITALIAN, SAHPI_LANG_HEBREW, SAHPI_LANG_JAPANESE,
	SAHPI_LANG_YIDDISH, SAHPI_LANG_JAVANESE, SAHPI_LANG_GEORGIAN,
	SAHPI_LANG_KAZAKH, SAHPI_LANG_GREENLANDIC, SAHPI_LANG_CAMBODIAN,
	SAHPI_LANG_KANNADA, SAHPI_LANG_KOREAN, SAHPI_LANG_KASHMIRI,
	SAHPI_LANG_KURDISH, SAHPI_LANG_KIRGHIZ, SAHPI_LANG_LATIN,
	SAHPI_LANG_LINGALA, SAHPI_LANG_LAOTHIAN, SAHPI_LANG_LITHUANIAN,
	SAHPI_LANG_LATVIANLETTISH, SAHPI_LANG_MALAGASY, SAHPI_LANG_MAORI,
	SAHPI_LANG_MACEDONIAN, SAHPI_LANG_MALAYALAM, SAHPI_LANG_MONGOLIAN,
	SAHPI_LANG_MOLDAVIAN, SAHPI_LANG_MARATHI, SAHPI_LANG_MALAY,
	SAHPI_LANG_MALTESE, SAHPI_LANG_BURMESE, SAHPI_LANG_NAURU,
	SAHPI_LANG_NEPALI, SAHPI_LANG_DUTCH, SAHPI_LANG_NORWEGIAN,
	SAHPI_LANG_OCCITAN, SAHPI_LANG_AFANOROMO, SAHPI_LANG_ORIYA,
	SAHPI_LANG_PUNJABI, SAHPI_LANG_POLISH, SAHPI_LANG_PASHTOPUSHTO,
	SAHPI_LANG_PORTUGUESE, SAHPI_LANG_QUECHUA, SAHPI_LANG_RHAETOROMANCE,
	SAHPI_LANG_KIRUNDI, SAHPI_LANG_ROMANIAN, SAHPI_LANG_RUSSIAN,
	SAHPI_LANG_KINYARWANDA, SAHPI_LANG_SANSKRIT, SAHPI_LANG_SINDHI,
	SAHPI_LANG_SANGRO, SAHPI_LANG_SERBOCROATIAN, SAHPI_LANG_SINGHALESE,
	SAHPI_LANG_SLOVAK, SAHPI_LANG_SLOVENIAN, SAHPI_LANG_SAMOAN,
	SAHPI_LANG_SHONA, SAHPI_LANG_SOMALI, SAHPI_LANG_ALBANIAN,
	SAHPI_LANG_SERBIAN, SAHPI_LANG_SISWATI, SAHPI_LANG_SESOTHO,
	SAHPI_LANG_SUDANESE, SAHPI_LANG_SWEDISH, SAHPI_LANG_SWAHILI,
	SAHPI_LANG_TAMIL, SAHPI_LANG_TELUGU, SAHPI_LANG_TAJIK,
	SAHPI_LANG_THAI, SAHPI_LANG_TIGRINYA, SAHPI_LANG_TURKMEN,
	SAHPI_LANG_TAGALOG, SAHPI_LANG_SETSWANA, SAHPI_LANG_TONGA,
	SAHPI_LANG_TURKISH, SAHPI_LANG_TSONGA, SAHPI_LANG_TATAR,
	SAHPI_LANG_TWI, SAHPI_LANG_UKRAINIAN, SAHPI_LANG_URDU,
	SAHPI_LANG_UZBEK, SAHPI_LANG_VIETNAMESE, SAHPI_LANG_VOLAPUK,
	SAHPI_LANG_WOLOF, SAHPI_LANG_XHOSA, SAHPI_LANG_YORUBA,
	SAHPI_LANG_CHINESE, SAHPI_LANG_ZULU
} SaHpiLanguageT;

/*
** Text Buffers
**
** These structures are used for defining the type of data in the text buffer
** and the length of the buffer. Text buffers are used in the inventory data,
** RDR, RPT, etc. for variable length strings of data.
*/
#define SAHPI_MAX_TEXT_BUFFER_LENGTH 255

typedef enum {
	SAHPI_TL_TYPE_BINARY = 0, /* String of bytes, any values legal */
	SAHPI_TL_TYPE_BCDPLUS, /* String of 0-9, space, dash, period ONLY */
	SAHPI_TL_TYPE_ASCII6, /* Reduced ASCII character set: 0x20-0x5F
			       * ONLY */
	SAHPI_TL_TYPE_LANGUAGE /* ASCII or UNICODE depending on language */
} SaHpiTextTypeT;

typedef struct {
	SaHpiTextTypeT DataType;
	SaHpiLanguageT Language; /* Language the text is in. */
	SaHpiUint8T DataLength; /* Bytes used in Data buffer */
	SaHpiUint8T Data[SAHPI_MAX_TEXT_BUFFER_LENGTH]; /* Data buffer */
} SaHpiTextBufferT;

#endif /* HPI_TYPES_H */


