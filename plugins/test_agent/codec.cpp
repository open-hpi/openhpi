/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTCODECLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include <SaHpi.h>

#include <oh_utils.h>

#include "codec.h"
#include "utils.h"
#include "vars.h"


namespace TA {


/**************************************************************
 * Helper Functions
 *************************************************************/
static bool ToHex( const char src, unsigned char& dst )
{
    switch ( toupper( src ) ) {
        case '0':
            dst = 0;
            break;
        case '1':
            dst = 0x1;
            break;
        case '2':
            dst = 0x2;
            break;
        case '3':
            dst = 0x3;
            break;
        case '4':
            dst = 0x4;
            break;
        case '5':
            dst = 0x5;
            break;
        case '6':
            dst = 0x6;
            break;
        case '7':
            dst = 0x7;
            break;
        case '8':
            dst = 0x8;
            break;
        case '9':
            dst = 0x9;
            break;
        case 'A':
            dst = 0xA;
            break;
        case 'B':
            dst = 0xB;
            break;
        case 'C':
            dst = 0xC;
            break;
        case 'D':
            dst = 0xD;
            break;
        case 'E':
            dst = 0xE;
            break;
        case 'F':
            dst = 0xF;
            break;
        default:
            return false;
    }
    return true;
}


/**************************************************************
 * Basic Types Codecs
 *************************************************************/
static void ToTxt_Uint( const uint64_t& src, std::string& txt )
{
    char buf[32];
    snprintf( buf, sizeof(buf), "%" PRIu64, src );
    txt.append( buf );
}

static bool FromTxt_Uint( const std::string& txt, uint64_t& dst )
{
    char * endptr = 0;
    dst = strtoull( txt.c_str(), &endptr, 0 );

    return *endptr == '\0';
}

static void ToTxt_Int( const int64_t& src, std::string& txt )
{
    char buf[32];
    snprintf( buf, sizeof(buf), "%" PRId64, src );
    txt.append( buf );
}

static bool FromTxt_Int( const std::string& txt, int64_t& dst )
{
    char * endptr = 0;
    dst = strtoll( txt.c_str(), &endptr, 0 );

    return *endptr == '\0';
}

static void ToTxt_Double( double& src, std::string& txt )
{
    char buf[32];
    snprintf( buf, sizeof(buf), "%f", src );
    txt.append( buf );
}

static bool FromTxt_Double( const std::string& txt, double& dst )
{
    char * endptr = 0;
    dst = strtod( txt.c_str(), &endptr );

    return *endptr == '\0';
}

static void ToTxt_Ascii( const void * src, size_t len, std::string& txt )
{
    const char * x = ConstPtr<char>( src );
    txt.append( x, len );
}

static bool FromTxt_Ascii( const std::string& txt,
                           size_t capacity,
                           void * dst,
                           size_t& len )
{
    memset( dst, 0, capacity );
    len = std::min<size_t>( txt.length(), capacity );
    memcpy( dst, txt.data(), len );

    return true;
}

static void ToTxt_Binary( const void * src, size_t len, std::string& txt )
{
    char buf[8];
    const unsigned char * x = ConstPtr<unsigned char>( src );
    for ( size_t i = 0; i < len; ++i ) {
        snprintf( buf, sizeof(buf), "%02X", x[i] );
        txt.append( buf );
    }
}

static bool FromTxt_Binary( const std::string& txt,
                            size_t capacity,
                            void * dst,
                            size_t& len )
{
    memset( dst, 0, capacity );

    size_t n = txt.length();
    if ( n & 1 ) {
        // accept only odd number of characters
        return false;
    }
    len = std::min<size_t>( n / 2, capacity );

    for ( size_t i = 0; i < len; ++i ) {
        unsigned char x = 0;
        for ( size_t j = 0; j < 2; ++j ) {
            x <<= 4;
            unsigned char xx = 0;
            bool rc = ToHex( txt[ 2 * i + j], xx );
            if ( !rc ) {
                return false;
            }
            x |= xx;
        }
        *( Ptr<unsigned char>( dst ) + i ) = x;
    }

    return true;
}


/**************************************************************
 * Enum Codecs
 *************************************************************/
struct EElem
{
    int          val;
    const char * name;
};

static void ToTxt_Enum( const EElem * elems, const void * src, std::string& txt )
{
    int x = ConstRef<int>( src );
    for ( size_t i = 0; elems[i].name != 0; ++i ) {
        if ( x == elems[i].val ) {
            txt.append( elems[i].name );
            return;
        }
    }

    // No name was found. Use numeric value.
    ToTxt_Int( x, txt );
}

static void ToTxt_EnumU8( const EElem * elems, const void * src, std::string& txt )
{
    int x = ConstRef<SaHpiUint8T>( src );
    ToTxt_Enum( elems, &x, txt );
}

static bool FromTxt_Enum( const EElem * elems, const std::string& txt, void * dst )
{
    for ( size_t i = 0; elems[i].name != 0; ++i ) {
        if ( txt == elems[i].name ) {
            Ref<int>( dst ) = elems[i].val;
            return true;
        }
    }

    // No name was found. Use numeric value.
    int64_t x = 0;
    bool rc = FromTxt_Int( txt, x );
    if ( rc ) {
        Ref<int>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_EnumU8( const EElem * elems, const std::string& txt, void * dst )
{
    int x = 0;
    bool rc = FromTxt_Enum( elems, txt, &x );
    if ( rc ) {
        Ref<SaHpiUint8T>( dst ) = x;
    }

    return rc;
}


/**************************************************************
 * Enum Data
 *************************************************************/
static EElem SaHpiLanguageTElems[] =
{
    { SAHPI_LANG_UNDEF          , "UNDEF"          },
    { SAHPI_LANG_AFAR           , "AFAR"           },
    { SAHPI_LANG_ABKHAZIAN      , "ABKHAZIAN"      },
    { SAHPI_LANG_AFRIKAANS      , "AFRIKAANS"      },
    { SAHPI_LANG_AMHARIC        , "AMHARIC"        },
    { SAHPI_LANG_ARABIC         , "ARABIC"         },
    { SAHPI_LANG_ASSAMESE       , "ASSAMESE"       },
    { SAHPI_LANG_AYMARA         , "AYMARA"         },
    { SAHPI_LANG_AZERBAIJANI    , "AZERBAIJANI"    },
    { SAHPI_LANG_BASHKIR        , "BASHKIR"        },
    { SAHPI_LANG_BYELORUSSIAN   , "BYELORUSSIAN"   },
    { SAHPI_LANG_BULGARIAN      , "BULGARIAN"      },
    { SAHPI_LANG_BIHARI         , "BIHARI"         },
    { SAHPI_LANG_BISLAMA        , "BISLAMA"        },
    { SAHPI_LANG_BENGALI        , "BENGALI"        },
    { SAHPI_LANG_TIBETAN        , "TIBETAN"        },
    { SAHPI_LANG_BRETON         , "BRETON"         },
    { SAHPI_LANG_CATALAN        , "CATALAN"        },
    { SAHPI_LANG_CORSICAN       , "CORSICAN"       },
    { SAHPI_LANG_CZECH          , "CZECH"          },
    { SAHPI_LANG_WELSH          , "WELSH"          },
    { SAHPI_LANG_DANISH         , "DANISH"         },
    { SAHPI_LANG_GERMAN         , "GERMAN"         },
    { SAHPI_LANG_BHUTANI        , "BHUTANI"        },
    { SAHPI_LANG_GREEK          , "GREEK"          },
    { SAHPI_LANG_ENGLISH        , "ENGLISH"        },
    { SAHPI_LANG_ESPERANTO      , "ESPERANTO"      },
    { SAHPI_LANG_SPANISH        , "SPANISH"        },
    { SAHPI_LANG_ESTONIAN       , "ESTONIAN"       },
    { SAHPI_LANG_BASQUE         , "BASQUE"         },
    { SAHPI_LANG_PERSIAN        , "PERSIAN"        },
    { SAHPI_LANG_FINNISH        , "FINNISH"        },
    { SAHPI_LANG_FIJI           , "FIJI"           },
    { SAHPI_LANG_FAEROESE       , "FAEROESE"       },
    { SAHPI_LANG_FRENCH         , "FRENCH"         },
    { SAHPI_LANG_FRISIAN        , "FRISIAN"        },
    { SAHPI_LANG_IRISH          , "IRISH"          },
    { SAHPI_LANG_SCOTSGAELIC    , "SCOTSGAELIC"    },
    { SAHPI_LANG_GALICIAN       , "GALICIAN"       },
    { SAHPI_LANG_GUARANI        , "GUARANI"        },
    { SAHPI_LANG_GUJARATI       , "GUJARATI"       },
    { SAHPI_LANG_HAUSA          , "HAUSA"          },
    { SAHPI_LANG_HINDI          , "HINDI"          },
    { SAHPI_LANG_CROATIAN       , "CROATIAN"       },
    { SAHPI_LANG_HUNGARIAN      , "HUNGARIAN"      },
    { SAHPI_LANG_ARMENIAN       , "ARMENIAN"       },
    { SAHPI_LANG_INTERLINGUA    , "INTERLINGUA"    },
    { SAHPI_LANG_INTERLINGUE    , "INTERLINGUE"    },
    { SAHPI_LANG_INUPIAK        , "INUPIAK"        },
    { SAHPI_LANG_INDONESIAN     , "INDONESIAN"     },
    { SAHPI_LANG_ICELANDIC      , "ICELANDIC"      },
    { SAHPI_LANG_ITALIAN        , "ITALIAN"        },
    { SAHPI_LANG_HEBREW         , "HEBREW"         },
    { SAHPI_LANG_JAPANESE       , "JAPANESE"       },
    { SAHPI_LANG_YIDDISH        , "YIDDISH"        },
    { SAHPI_LANG_JAVANESE       , "JAVANESE"       },
    { SAHPI_LANG_GEORGIAN       , "GEORGIAN"       },
    { SAHPI_LANG_KAZAKH         , "KAZAKH"         },
    { SAHPI_LANG_GREENLANDIC    , "GREENLANDIC"    },
    { SAHPI_LANG_CAMBODIAN      , "CAMBODIAN"      },
    { SAHPI_LANG_KANNADA        , "KANNADA"        },
    { SAHPI_LANG_KOREAN         , "KOREAN"         },
    { SAHPI_LANG_KASHMIRI       , "KASHMIRI"       },
    { SAHPI_LANG_KURDISH        , "KURDISH"        },
    { SAHPI_LANG_KIRGHIZ        , "KIRGHIZ"        },
    { SAHPI_LANG_LATIN          , "LATIN"          },
    { SAHPI_LANG_LINGALA        , "LINGALA"        },
    { SAHPI_LANG_LAOTHIAN       , "LAOTHIAN"       },
    { SAHPI_LANG_LITHUANIAN     , "LITHUANIAN"     },
    { SAHPI_LANG_LATVIANLETTISH , "LATVIANLETTISH" },
    { SAHPI_LANG_MALAGASY       , "MALAGASY"       },
    { SAHPI_LANG_MAORI          , "MAORI"          },
    { SAHPI_LANG_MACEDONIAN     , "MACEDONIAN"     },
    { SAHPI_LANG_MALAYALAM      , "MALAYALAM"      },
    { SAHPI_LANG_MONGOLIAN      , "MONGOLIAN"      },
    { SAHPI_LANG_MOLDAVIAN      , "MOLDAVIAN"      },
    { SAHPI_LANG_MARATHI        , "MARATHI"        },
    { SAHPI_LANG_MALAY          , "MALAY"          },
    { SAHPI_LANG_MALTESE        , "MALTESE"        },
    { SAHPI_LANG_BURMESE        , "BURMESE"        },
    { SAHPI_LANG_NAURU          , "NAURU"          },
    { SAHPI_LANG_NEPALI         , "NEPALI"         },
    { SAHPI_LANG_DUTCH          , "DUTCH"          },
    { SAHPI_LANG_NORWEGIAN      , "NORWEGIAN"      },
    { SAHPI_LANG_OCCITAN        , "OCCITAN"        },
    { SAHPI_LANG_AFANOROMO      , "AFANOROMO"      },
    { SAHPI_LANG_ORIYA          , "ORIYA"          },
    { SAHPI_LANG_PUNJABI        , "PUNJABI"        },
    { SAHPI_LANG_POLISH         , "POLISH"         },
    { SAHPI_LANG_PASHTOPUSHTO   , "PASHTOPUSHTO"   },
    { SAHPI_LANG_PORTUGUESE     , "PORTUGUESE"     },
    { SAHPI_LANG_QUECHUA        , "QUECHUA"        },
    { SAHPI_LANG_RHAETOROMANCE  , "RHAETOROMANCE"  },
    { SAHPI_LANG_KIRUNDI        , "KIRUNDI"        },
    { SAHPI_LANG_ROMANIAN       , "ROMANIAN"       },
    { SAHPI_LANG_RUSSIAN        , "RUSSIAN"        },
    { SAHPI_LANG_KINYARWANDA    , "KINYARWANDA"    },
    { SAHPI_LANG_SANSKRIT       , "SANSKRIT"       },
    { SAHPI_LANG_SINDHI         , "SINDHI"         },
    { SAHPI_LANG_SANGRO         , "SANGRO"         },
    { SAHPI_LANG_SERBOCROATIAN  , "SERBOCROATIAN"  },
    { SAHPI_LANG_SINGHALESE     , "SINGHALESE"     },
    { SAHPI_LANG_SLOVAK         , "SLOVAK"         },
    { SAHPI_LANG_SLOVENIAN      , "SLOVENIAN"      },
    { SAHPI_LANG_SAMOAN         , "SAMOAN"         },
    { SAHPI_LANG_SHONA          , "SHONA"          },
    { SAHPI_LANG_SOMALI         , "SOMALI"         },
    { SAHPI_LANG_ALBANIAN       , "ALBANIAN"       },
    { SAHPI_LANG_SERBIAN        , "SERBIAN"        },
    { SAHPI_LANG_SISWATI        , "SISWATI"        },
    { SAHPI_LANG_SESOTHO        , "SESOTHO"        },
    { SAHPI_LANG_SUDANESE       , "SUDANESE"       },
    { SAHPI_LANG_SWEDISH        , "SWEDISH"        },
    { SAHPI_LANG_SWAHILI        , "SWAHILI"        },
    { SAHPI_LANG_TAMIL          , "TAMIL"          },
    { SAHPI_LANG_TELUGU         , "TELUGU"         },
    { SAHPI_LANG_TAJIK          , "TAJIK"          },
    { SAHPI_LANG_THAI           , "THAI"           },
    { SAHPI_LANG_TIGRINYA       , "TIGRINYA"       },
    { SAHPI_LANG_TURKMEN        , "TURKMEN"        },
    { SAHPI_LANG_TAGALOG        , "TAGALOG"        },
    { SAHPI_LANG_SETSWANA       , "SETSWANA"       },
    { SAHPI_LANG_TONGA          , "TONGA"          },
    { SAHPI_LANG_TURKISH        , "TURKISH"        },
    { SAHPI_LANG_TSONGA         , "TSONGA"         },
    { SAHPI_LANG_TATAR          , "TATAR"          },
    { SAHPI_LANG_TWI            , "TWI"            },
    { SAHPI_LANG_UKRAINIAN      , "UKRAINIAN"      },
    { SAHPI_LANG_URDU           , "URDU"           },
    { SAHPI_LANG_UZBEK          , "UZBEK"          },
    { SAHPI_LANG_VIETNAMESE     , "VIETNAMESE"     },
    { SAHPI_LANG_VOLAPUK        , "VOLAPUK"        },
    { SAHPI_LANG_WOLOF          , "WOLOF"          },
    { SAHPI_LANG_XHOSA          , "XHOSA"          },
    { SAHPI_LANG_YORUBA         , "YORUBA"         },
    { SAHPI_LANG_CHINESE        , "CHINESE"        },
    { SAHPI_LANG_ZULU           , "ZULU"           },
    { 0                         , 0                },
};

static EElem SaHpiTextTypeTElems[] =
{
    { SAHPI_TL_TYPE_UNICODE , "UNICODE" },
    { SAHPI_TL_TYPE_BCDPLUS , "BCDPLUS" },
    { SAHPI_TL_TYPE_ASCII6  , "ASCII6"  },
    { SAHPI_TL_TYPE_TEXT    , "TEXT"    },
    { SAHPI_TL_TYPE_BINARY  , "BINARY"  },
    { 0                     , 0         },
};

static EElem SaHpiEventCategoryTElems[] =
{
    { SAHPI_EC_UNSPECIFIED     , "UNSPECIFIED"     },
    { SAHPI_EC_THRESHOLD       , "THRESHOLD"       },
    { SAHPI_EC_USAGE           , "USAGE"           },
    { SAHPI_EC_STATE           , "STATE"           },
    { SAHPI_EC_PRED_FAIL       , "PRED_FAIL"       },
    { SAHPI_EC_LIMIT           , "LIMIT"           },
    { SAHPI_EC_PERFORMANCE     , "PERFORMANCE"     },
    { SAHPI_EC_SEVERITY        , "SEVERITY"        },
    { SAHPI_EC_PRESENCE        , "PRESENCE"        },
    { SAHPI_EC_ENABLE          , "ENABLE"          },
    { SAHPI_EC_AVAILABILITY    , "AVAILABILITY"    },
    { SAHPI_EC_REDUNDANCY      , "REDUNDANCY"      },
    { SAHPI_EC_SENSOR_SPECIFIC , "SENSOR_SPECIFIC" },
    { SAHPI_EC_GENERIC         , "GENERIC"         },
    { 0                        , 0                 },
};

static EElem SaHpiSensorTypeTElems[] =
{
    { SAHPI_TEMPERATURE                 , "TEMPERATURE"                 },
    { SAHPI_VOLTAGE                     , "VOLTAGE"                     },
    { SAHPI_CURRENT                     , "CURRENT"                     },
    { SAHPI_FAN                         , "FAN"                         },
    { SAHPI_PHYSICAL_SECURITY           , "PHYSICAL_SECURITY"           },
    { SAHPI_PLATFORM_VIOLATION          , "PLATFORM_VIOLATION"          },
    { SAHPI_PROCESSOR                   , "PROCESSOR"                   },
    { SAHPI_POWER_SUPPLY                , "POWER_SUPPLY"                },
    { SAHPI_POWER_UNIT                  , "POWER_UNIT"                  },
    { SAHPI_COOLING_DEVICE              , "COOLING_DEVICE"              },
    { SAHPI_OTHER_UNITS_BASED_SENSOR    , "OTHER_UNITS_BASED_SENSOR"    },
    { SAHPI_MEMORY                      , "MEMORY"                      },
    { SAHPI_DRIVE_SLOT                  , "DRIVE_SLOT"                  },
    { SAHPI_POST_MEMORY_RESIZE          , "POST_MEMORY_RESIZE"          },
    { SAHPI_SYSTEM_FW_PROGRESS          , "SYSTEM_FW_PROGRESS"          },
    { SAHPI_EVENT_LOGGING_DISABLED      , "EVENT_LOGGING_DISABLED"      },
    { SAHPI_RESERVED1                   , "RESERVED1"                   },
    { SAHPI_SYSTEM_EVENT                , "SYSTEM_EVENT"                },
    { SAHPI_CRITICAL_INTERRUPT          , "CRITICAL_INTERRUPT"          },
    { SAHPI_BUTTON                      , "BUTTON"                      },
    { SAHPI_MODULE_BOARD                , "MODULE_BOARD"                },
    { SAHPI_MICROCONTROLLER_COPROCESSOR , "MICROCONTROLLER_COPROCESSOR" },
    { SAHPI_ADDIN_CARD                  , "ADDIN_CARD"                  },
    { SAHPI_CHASSIS                     , "CHASSIS"                     },
    { SAHPI_CHIP_SET                    , "CHIP_SET"                    },
    { SAHPI_OTHER_FRU                   , "OTHER_FRU"                   },
    { SAHPI_CABLE_INTERCONNECT          , "CABLE_INTERCONNECT"          },
    { SAHPI_TERMINATOR                  , "TERMINATOR"                  },
    { SAHPI_SYSTEM_BOOT_INITIATED       , "SYSTEM_BOOT_INITIATED"       },
    { SAHPI_BOOT_ERROR                  , "BOOT_ERROR"                  },
    { SAHPI_OS_BOOT                     , "OS_BOOT"                     },
    { SAHPI_OS_CRITICAL_STOP            , "OS_CRITICAL_STOP"            },
    { SAHPI_SLOT_CONNECTOR              , "SLOT_CONNECTOR"              },
    { SAHPI_SYSTEM_ACPI_POWER_STATE     , "SYSTEM_ACPI_POWER_STATE"     },
    { SAHPI_RESERVED2                   , "RESERVED2"                   },
    { SAHPI_PLATFORM_ALERT              , "PLATFORM_ALERT"              },
    { SAHPI_ENTITY_PRESENCE             , "ENTITY_PRESENCE"             },
    { SAHPI_MONITOR_ASIC_IC             , "MONITOR_ASIC_IC"             },
    { SAHPI_LAN                         , "LAN"                         },
    { SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH , "MANAGEMENT_SUBSYSTEM_HEALTH" },
    { SAHPI_BATTERY                     , "BATTERY"                     },
    { SAHPI_SESSION_AUDIT               , "SESSION_AUDIT"               },
    { SAHPI_VERSION_CHANGE              , "VERSION_CHANGE"              },
    { SAHPI_OPERATIONAL                 , "OPERATIONAL"                 },
    { SAHPI_OEM_SENSOR                  , "OEM_SENSOR"                  },
    { SAHPI_COMM_CHANNEL_LINK_STATE     , "COMM_CHANNEL_LINK_STATE"     },
    { SAHPI_MANAGEMENT_BUS_STATE        , "MANAGEMENT_BUS_STATE"        },
    { SAHPI_COMM_CHANNEL_BUS_STATE      , "COMM_CHANNEL_BUS_STATE"      },
    { SAHPI_CONFIG_DATA                 , "CONFIG_DATA"                 },
    { SAHPI_POWER_BUDGET                , "POWER_BUDGET"                },
    { 0                                 , 0                             },
};

static EElem SaHpiSensorReadingTypeTElems[] =
{
    { SAHPI_SENSOR_READING_TYPE_INT64   , "INT64"   },
    { SAHPI_SENSOR_READING_TYPE_UINT64  , "UINT64"  },
    { SAHPI_SENSOR_READING_TYPE_FLOAT64 , "FLOAT64" },
    { SAHPI_SENSOR_READING_TYPE_BUFFER  , "BUFFER"  },
    { 0                                 , 0         },
};

static EElem SaHpiSensorUnitsTElems[] =
{
    { SAHPI_SU_UNSPECIFIED          , "UNSPECIFIED"          },
    { SAHPI_SU_DEGREES_C            , "DEGREES_C"            },
    { SAHPI_SU_DEGREES_F            , "DEGREES_F"            },
    { SAHPI_SU_DEGREES_K            , "DEGREES_K"            },
    { SAHPI_SU_VOLTS                , "VOLTS"                },
    { SAHPI_SU_AMPS                 , "AMPS"                 },
    { SAHPI_SU_WATTS                , "WATTS"                },
    { SAHPI_SU_JOULES               , "JOULES"               },
    { SAHPI_SU_COULOMBS             , "COULOMBS"             },
    { SAHPI_SU_VA                   , "VA"                   },
    { SAHPI_SU_NITS                 , "NITS"                 },
    { SAHPI_SU_LUMEN                , "LUMEN"                },
    { SAHPI_SU_LUX                  , "LUX"                  },
    { SAHPI_SU_CANDELA              , "CANDELA"              },
    { SAHPI_SU_KPA                  , "KPA"                  },
    { SAHPI_SU_PSI                  , "PSI"                  },
    { SAHPI_SU_NEWTON               , "NEWTON"               },
    { SAHPI_SU_CFM                  , "CFM"                  },
    { SAHPI_SU_RPM                  , "RPM"                  },
    { SAHPI_SU_HZ                   , "HZ"                   },
    { SAHPI_SU_MICROSECOND          , "MICROSECOND"          },
    { SAHPI_SU_MILLISECOND          , "MILLISECOND"          },
    { SAHPI_SU_SECOND               , "SECOND"               },
    { SAHPI_SU_MINUTE               , "MINUTE"               },
    { SAHPI_SU_HOUR                 , "HOUR"                 },
    { SAHPI_SU_DAY                  , "DAY"                  },
    { SAHPI_SU_WEEK                 , "WEEK"                 },
    { SAHPI_SU_MIL                  , "MIL"                  },
    { SAHPI_SU_INCHES               , "INCHES"               },
    { SAHPI_SU_FEET                 , "FEET"                 },
    { SAHPI_SU_CU_IN                , "CU_IN"                },
    { SAHPI_SU_CU_FEET              , "CU_FEET"              },
    { SAHPI_SU_MM                   , "MM"                   },
    { SAHPI_SU_CM                   , "CM"                   },
    { SAHPI_SU_M                    , "M"                    },
    { SAHPI_SU_CU_CM                , "CU_CM"                },
    { SAHPI_SU_CU_M                 , "CU_M"                 },
    { SAHPI_SU_LITERS               , "LITERS"               },
    { SAHPI_SU_FLUID_OUNCE          , "FLUID_OUNCE"          },
    { SAHPI_SU_RADIANS              , "RADIANS"              },
    { SAHPI_SU_STERADIANS           , "STERADIANS"           },
    { SAHPI_SU_REVOLUTIONS          , "REVOLUTIONS"          },
    { SAHPI_SU_CYCLES               , "CYCLES"               },
    { SAHPI_SU_GRAVITIES            , "GRAVITIES"            },
    { SAHPI_SU_OUNCE                , "OUNCE"                },
    { SAHPI_SU_POUND                , "POUND"                },
    { SAHPI_SU_FT_LB                , "FT_LB"                },
    { SAHPI_SU_OZ_IN                , "OZ_IN"                },
    { SAHPI_SU_GAUSS                , "GAUSS"                },
    { SAHPI_SU_GILBERTS             , "GILBERTS"             },
    { SAHPI_SU_HENRY                , "HENRY"                },
    { SAHPI_SU_MILLIHENRY           , "MILLIHENRY"           },
    { SAHPI_SU_FARAD                , "FARAD"                },
    { SAHPI_SU_MICROFARAD           , "MICROFARAD"           },
    { SAHPI_SU_OHMS                 , "OHMS"                 },
    { SAHPI_SU_SIEMENS              , "SIEMENS"              },
    { SAHPI_SU_MOLE                 , "MOLE"                 },
    { SAHPI_SU_BECQUEREL            , "BECQUEREL"            },
    { SAHPI_SU_PPM                  , "PPM"                  },
    { SAHPI_SU_RESERVED             , "RESERVED"             },
    { SAHPI_SU_DECIBELS             , "DECIBELS"             },
    { SAHPI_SU_DBA                  , "DBA"                  },
    { SAHPI_SU_DBC                  , "DBC"                  },
    { SAHPI_SU_GRAY                 , "GRAY"                 },
    { SAHPI_SU_SIEVERT              , "SIEVERT"              },
    { SAHPI_SU_COLOR_TEMP_DEG_K     , "COLOR_TEMP_DEG_K"     },
    { SAHPI_SU_BIT                  , "BIT"                  },
    { SAHPI_SU_KILOBIT              , "KILOBIT"              },
    { SAHPI_SU_MEGABIT              , "MEGABIT"              },
    { SAHPI_SU_GIGABIT              , "GIGABIT"              },
    { SAHPI_SU_BYTE                 , "BYTE"                 },
    { SAHPI_SU_KILOBYTE             , "KILOBYTE"             },
    { SAHPI_SU_MEGABYTE             , "MEGABYTE"             },
    { SAHPI_SU_GIGABYTE             , "GIGABYTE"             },
    { SAHPI_SU_WORD                 , "WORD"                 },
    { SAHPI_SU_DWORD                , "DWORD"                },
    { SAHPI_SU_QWORD                , "QWORD"                },
    { SAHPI_SU_LINE                 , "LINE"                 },
    { SAHPI_SU_HIT                  , "HIT"                  },
    { SAHPI_SU_MISS                 , "MISS"                 },
    { SAHPI_SU_RETRY                , "RETRY"                },
    { SAHPI_SU_RESET                , "RESET"                },
    { SAHPI_SU_OVERRUN              , "OVERRUN"              },
    { SAHPI_SU_UNDERRUN             , "UNDERRUN"             },
    { SAHPI_SU_COLLISION            , "COLLISION"            },
    { SAHPI_SU_PACKETS              , "PACKETS"              },
    { SAHPI_SU_MESSAGES             , "MESSAGES"             },
    { SAHPI_SU_CHARACTERS           , "CHARACTERS"           },
    { SAHPI_SU_ERRORS               , "ERRORS"               },
    { SAHPI_SU_CORRECTABLE_ERRORS   , "CORRECTABLE_ERRORS"   },
    { SAHPI_SU_UNCORRECTABLE_ERRORS , "UNCORRECTABLE_ERRORS" },
    { 0                             , 0                      },
};

static EElem SaHpiSensorModUnitUseTElems[] =
{
    { SAHPI_SMUU_NONE                 , "NONE"                 },
    { SAHPI_SMUU_BASIC_OVER_MODIFIER  , "BASIC_OVER_MODIFIER"  },
    { SAHPI_SMUU_BASIC_TIMES_MODIFIER , "BASIC_TIMES_MODIFIER" },
    { 0                               , 0                      },
};

static EElem SaHpiSensorEventCtrlTElems[] =
{
    { SAHPI_SEC_PER_EVENT       , "PER_EVENT"       },
    { SAHPI_SEC_READ_ONLY_MASKS , "READ_ONLY_MASKS" },
    { SAHPI_SEC_READ_ONLY       , "READ_ONLY"       },
    { 0                         , 0                 },
};

static EElem SaHpiCtrlTypeTElems[] =
{
    { SAHPI_CTRL_TYPE_DIGITAL  , "DIGITAL"  },
    { SAHPI_CTRL_TYPE_DISCRETE , "DISCRETE" },
    { SAHPI_CTRL_TYPE_ANALOG   , "ANALOG"   },
    { SAHPI_CTRL_TYPE_STREAM   , "STREAM"   },
    { SAHPI_CTRL_TYPE_TEXT     , "TEXT"     },
    { SAHPI_CTRL_TYPE_OEM      , "OEM"      },
    { 0                        , 0          },
};

static EElem SaHpiCtrlStateDigitalTElems[] =
{
    { SAHPI_CTRL_STATE_OFF       , "OFF"       },
    { SAHPI_CTRL_STATE_ON        , "ON"        },
    { SAHPI_CTRL_STATE_PULSE_OFF , "PULSE_OFF" },
    { SAHPI_CTRL_STATE_PULSE_ON  , "PULSE_ON"  },
    { 0                          , 0           },
};

static EElem SaHpiCtrlModeTElems[] =
{
    { SAHPI_CTRL_MODE_AUTO   , "AUTO"   },
    { SAHPI_CTRL_MODE_MANUAL , "MANUAL" },
    { 0                      , 0        },
};

static EElem SaHpiCtrlOutputTypeTElems[] =
{
    { SAHPI_CTRL_GENERIC              , "GENERIC"              },
    { SAHPI_CTRL_LED                  , "LED"                  },
    { SAHPI_CTRL_FAN_SPEED            , "FAN_SPEED"            },
    { SAHPI_CTRL_DRY_CONTACT_CLOSURE  , "DRY_CONTACT_CLOSURE"  },
    { SAHPI_CTRL_POWER_SUPPLY_INHIBIT , "POWER_SUPPLY_INHIBIT" },
    { SAHPI_CTRL_AUDIBLE              , "AUDIBLE"              },
    { SAHPI_CTRL_FRONT_PANEL_LOCKOUT  , "FRONT_PANEL_LOCKOUT"  },
    { SAHPI_CTRL_POWER_INTERLOCK      , "POWER_INTERLOCK"      },
    { SAHPI_CTRL_POWER_STATE          , "POWER_STATE"          },
    { SAHPI_CTRL_LCD_DISPLAY          , "LCD_DISPLAY"          },
    { SAHPI_CTRL_OEM                  , "OEM"                  },
    { SAHPI_CTRL_GENERIC_ADDRESS      , "GENERIC_ADDRESS"      },
    { SAHPI_CTRL_IP_ADDRESS           , "IP_ADDRESS"           },
    { SAHPI_CTRL_RESOURCE_ID          , "RESOURCE_ID"          },
    { SAHPI_CTRL_POWER_BUDGET         , "POWER_BUDGET"         },
    { SAHPI_CTRL_ACTIVATE             , "ACTIVATE"             },
    { SAHPI_CTRL_RESET                , "RESET"                },
    { 0                               , 0                      },
};

static EElem SaHpiIdrAreaTypeTElems[] =
{
    { SAHPI_IDR_AREATYPE_INTERNAL_USE , "INTERNAL_USE" },
    { SAHPI_IDR_AREATYPE_CHASSIS_INFO , "CHASSIS_INFO" },
    { SAHPI_IDR_AREATYPE_BOARD_INFO   , "BOARD_INFO"   },
    { SAHPI_IDR_AREATYPE_PRODUCT_INFO , "PRODUCT_INFO" },
    { SAHPI_IDR_AREATYPE_OEM          , "OEM"          },
    { SAHPI_IDR_AREATYPE_UNSPECIFIED  , "UNSPECIFIED"  },
    { 0                               , 0              },
};

static EElem SaHpiIdrFieldTypeTElems[] =
{
    { SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE    , "CHASSIS_TYPE"    },
    { SAHPI_IDR_FIELDTYPE_MFG_DATETIME    , "MFG_DATETIME"    },
    { SAHPI_IDR_FIELDTYPE_MANUFACTURER    , "MANUFACTURER"    },
    { SAHPI_IDR_FIELDTYPE_PRODUCT_NAME    , "PRODUCT_NAME"    },
    { SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION , "PRODUCT_VERSION" },
    { SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER   , "SERIAL_NUMBER"   },
    { SAHPI_IDR_FIELDTYPE_PART_NUMBER     , "PART_NUMBER"     },
    { SAHPI_IDR_FIELDTYPE_FILE_ID         , "FILE_ID"         },
    { SAHPI_IDR_FIELDTYPE_ASSET_TAG       , "ASSET_TAG"       },
    { SAHPI_IDR_FIELDTYPE_CUSTOM          , "CUSTOM"          },
    { SAHPI_IDR_FIELDTYPE_UNSPECIFIED     , "UNSPECIFIED"     },
    { 0                                   , 0                 },
};

static EElem SaHpiWatchdogActionTElems[] =
{
    { SAHPI_WA_NO_ACTION   , "NO_ACTION"   },
    { SAHPI_WA_RESET       , "RESET"       },
    { SAHPI_WA_POWER_DOWN  , "POWER_DOWN"  },
    { SAHPI_WA_POWER_CYCLE , "POWER_CYCLE" },
    { 0                    , 0             },
};

static EElem SaHpiWatchdogActionEventTElems[] =
{
    { SAHPI_WAE_NO_ACTION   , "NO_ACTION"   },
    { SAHPI_WAE_RESET       , "RESET"       },
    { SAHPI_WAE_POWER_DOWN  , "POWER_DOWN"  },
    { SAHPI_WAE_POWER_CYCLE , "POWER_CYCLE" },
    { SAHPI_WAE_TIMER_INT   , "TIMER_INT"   },
    { 0                     , 0             },
};

static EElem SaHpiWatchdogPretimerInterruptTElems[] =
{
    { SAHPI_WPI_NONE              , "NONE"              },
    { SAHPI_WPI_SMI               , "SMI"               },
    { SAHPI_WPI_NMI               , "NMI"               },
    { SAHPI_WPI_MESSAGE_INTERRUPT , "MESSAGE_INTERRUPT" },
    { SAHPI_WPI_OEM               , "OEM"               },
    { 0                           , 0                   },
};

static EElem SaHpiWatchdogTimerUseTElems[] =
{
    { SAHPI_WTU_NONE        , "NONE"        },
    { SAHPI_WTU_BIOS_FRB2   , "BIOS_FRB2"   },
    { SAHPI_WTU_BIOS_POST   , "BIOS_POST"   },
    { SAHPI_WTU_OS_LOAD     , "OS_LOAD"     },
    { SAHPI_WTU_SMS_OS      , "SMS_OS"      },
    { SAHPI_WTU_OEM         , "OEM"         },
    { SAHPI_WTU_UNSPECIFIED , "UNSPECIFIED" },
    { 0                     , 0             },
};

static EElem SaHpiDimiTestServiceImpactTElems[] =
{
    { SAHPI_DIMITEST_NONDEGRADING         , "NONDEGRADING"         },
    { SAHPI_DIMITEST_DEGRADING            , "DEGRADING"            },
    { SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL , "VENDOR_DEFINED_LEVEL" },
    { 0                                   , 0                      },
};

static EElem SaHpiDimiTestRunStatusTElems[] =
{
    { SAHPI_DIMITEST_STATUS_NOT_RUN            , "NOT_RUN"            },
    { SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS , "FINISHED_NO_ERRORS" },
    { SAHPI_DIMITEST_STATUS_FINISHED_ERRORS    , "FINISHED_ERRORS"    },
    { SAHPI_DIMITEST_STATUS_CANCELED           , "CANCELED"           },
    { SAHPI_DIMITEST_STATUS_RUNNING            , "RUNNING"            },
    { 0                                        , 0                    },
};

static EElem SaHpiDimiTestErrCodeTElems[] =
{
    { SAHPI_DIMITEST_STATUSERR_NOERR  , "NOERR"  },
    { SAHPI_DIMITEST_STATUSERR_RUNERR , "RUNERR" },
    { SAHPI_DIMITEST_STATUSERR_UNDEF  , "UNDEF"  },
    { 0                               , 0        },
};

static EElem SaHpiDimiTestParamTypeTElems[] =
{
    { SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN , "BOOLEAN" },
    { SAHPI_DIMITEST_PARAM_TYPE_INT32   , "INT32"   },
    { SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 , "FLOAT64" },
    { SAHPI_DIMITEST_PARAM_TYPE_TEXT    , "TEXT"    },
    { 0                                 , 0         },
};

static EElem SaHpiDimiReadyTElems[] =
{
    { SAHPI_DIMI_READY       , "READY"       },
    { SAHPI_DIMI_WRONG_STATE , "WRONG_STATE" },
    { SAHPI_DIMI_BUSY        , "BUSY"        },
    { 0                      , 0             },
};

static EElem SaHpiFumiSpecInfoTypeTElems[] =
{
    { SAHPI_FUMI_SPEC_INFO_NONE        , "NONE"        },
    { SAHPI_FUMI_SPEC_INFO_SAF_DEFINED , "SAF_DEFINED" },
    { SAHPI_FUMI_SPEC_INFO_OEM_DEFINED , "OEM_DEFINED" },
    { 0                                , 0             },
};

static EElem SaHpiFumiSafDefinedSpecIdTElems[] =
{
    { SAHPI_FUMI_SPEC_HPM1 , "HPM1" },
    { 0                    , 0                      },
};

static EElem SaHpiFumiServiceImpactTElems[] =
{
    { SAHPI_FUMI_PROCESS_NONDEGRADING                , "NONDEGRADING"                },
    { SAHPI_FUMI_PROCESS_DEGRADING                   , "DEGRADING"                   },
    { SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL , "VENDOR_DEFINED_IMPACT_LEVEL" },
    { 0                                              , 0                             },
};

static EElem SaHpiFumiSourceStatusTElems[] =
{
    { SAHPI_FUMI_SRC_VALID                  , "VALID"                  },
    { SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED , "PROTOCOL_NOT_SUPPORTED" },
    { SAHPI_FUMI_SRC_UNREACHABLE            , "UNREACHABLE"            },
    { SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED , "VALIDATION_NOT_STARTED" },
    { SAHPI_FUMI_SRC_VALIDATION_INITIATED   , "VALIDATION_INITIATED"   },
    { SAHPI_FUMI_SRC_VALIDATION_FAIL        , "VALIDATION_FAIL"        },
    { SAHPI_FUMI_SRC_TYPE_MISMATCH          , "TYPE_MISMATCH"          },
    { SAHPI_FUMI_SRC_INVALID                , "INVALID"                },
    { SAHPI_FUMI_SRC_VALIDITY_UNKNOWN       , "VALIDITY_UNKNOWN"       },
    { 0                                     , 0                        },
};

static EElem SaHpiFumiBankStateTElems[] =
{
    { SAHPI_FUMI_BANK_VALID               , "VALID"               },
    { SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS , "UPGRADE_IN_PROGRESS" },
    { SAHPI_FUMI_BANK_CORRUPTED           , "CORRUPTED"           },
    { SAHPI_FUMI_BANK_ACTIVE              , "ACTIVE"              },
    { SAHPI_FUMI_BANK_BUSY                , "BUSY"                },
    { SAHPI_FUMI_BANK_UNKNOWN             , "UNKNOWN"             },
    { 0                                   , 0                     },
};

static EElem SaHpiFumiUpgradeStatusTElems[] =
{
    { SAHPI_FUMI_OPERATION_NOTSTARTED                  , "OPERATION_NOTSTARTED"                  },
    { SAHPI_FUMI_SOURCE_VALIDATION_INITIATED           , "SOURCE_VALIDATION_INITIATED"           },
    { SAHPI_FUMI_SOURCE_VALIDATION_FAILED              , "SOURCE_VALIDATION_FAILED"              },
    { SAHPI_FUMI_SOURCE_VALIDATION_DONE                , "SOURCE_VALIDATION_DONE"                },
    { SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED           , "SOURCE_VALIDATION_CANCELLED"           },
    { SAHPI_FUMI_INSTALL_INITIATED                     , "INSTALL_INITIATED"                     },
    { SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED        , "INSTALL_FAILED_ROLLBACK_NEEDED"        },
    { SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED     , "INSTALL_FAILED_ROLLBACK_INITIATED"     },
    { SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE  , "INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE"  },
    { SAHPI_FUMI_INSTALL_DONE                          , "INSTALL_DONE"                          },
    { SAHPI_FUMI_INSTALL_CANCELLED                     , "INSTALL_CANCELLED"                     },
    { SAHPI_FUMI_ROLLBACK_INITIATED                    , "ROLLBACK_INITIATED"                    },
    { SAHPI_FUMI_ROLLBACK_FAILED                       , "ROLLBACK_FAILED"                       },
    { SAHPI_FUMI_ROLLBACK_DONE                         , "ROLLBACK_DONE"                         },
    { SAHPI_FUMI_ROLLBACK_CANCELLED                    , "ROLLBACK_CANCELLED"                    },
    { SAHPI_FUMI_BACKUP_INITIATED                      , "BACKUP_INITIATED"                      },
    { SAHPI_FUMI_BACKUP_FAILED                         , "BACKUP_FAILED"                         },
    { SAHPI_FUMI_BACKUP_DONE                           , "BACKUP_DONE"                           },
    { SAHPI_FUMI_BACKUP_CANCELLED                      , "BACKUP_CANCELLED"                      },
    { SAHPI_FUMI_BANK_COPY_INITIATED                   , "BANK_COPY_INITIATED"                   },
    { SAHPI_FUMI_BANK_COPY_FAILED                      , "BANK_COPY_FAILED"                      },
    { SAHPI_FUMI_BANK_COPY_DONE                        , "BANK_COPY_DONE"                        },
    { SAHPI_FUMI_BANK_COPY_CANCELLED                   , "BANK_COPY_CANCELLED"                   },
    { SAHPI_FUMI_TARGET_VERIFY_INITIATED               , "TARGET_VERIFY_INITIATED"               },
    { SAHPI_FUMI_TARGET_VERIFY_FAILED                  , "TARGET_VERIFY_FAILED"                  },
    { SAHPI_FUMI_TARGET_VERIFY_DONE                    , "TARGET_VERIFY_DONE"                    },
    { SAHPI_FUMI_TARGET_VERIFY_CANCELLED               , "TARGET_VERIFY_CANCELLED"               },
    { SAHPI_FUMI_ACTIVATE_INITIATED                    , "ACTIVATE_INITIATED"                    },
    { SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED       , "ACTIVATE_FAILED_ROLLBACK_NEEDED"       },
    { SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED    , "ACTIVATE_FAILED_ROLLBACK_INITIATED"    },
    { SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE , "ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE" },
    { SAHPI_FUMI_ACTIVATE_DONE                         , "ACTIVATE_DONE"                         },
    { SAHPI_FUMI_ACTIVATE_CANCELLED                    , "ACTIVATE_CANCELLED"                    },
    { 0                                                , 0                                       },
};

static EElem SaHpiHsIndicatorStateTElems[] =
{
    { SAHPI_HS_INDICATOR_OFF , "OFF" },
    { SAHPI_HS_INDICATOR_ON  , "ON"  },
    { 0                      , 0     },
};

static EElem SaHpiHsStateTElems[] =
{
    { SAHPI_HS_STATE_INACTIVE           , "INACTIVE"           },
    { SAHPI_HS_STATE_INSERTION_PENDING  , "INSERTION_PENDING"  },
    { SAHPI_HS_STATE_ACTIVE             , "ACTIVE"             },
    { SAHPI_HS_STATE_EXTRACTION_PENDING , "EXTRACTION_PENDING" },
    { SAHPI_HS_STATE_NOT_PRESENT        , "NOT_PRESENT"        },
    { 0                                 , 0                    },
};

static EElem SaHpiHsCauseOfStateChangeTElems[] =
{
    { SAHPI_HS_CAUSE_AUTO_POLICY             , "AUTO_POLICY"             },
    { SAHPI_HS_CAUSE_EXT_SOFTWARE            , "EXT_SOFTWARE"            },
    { SAHPI_HS_CAUSE_OPERATOR_INIT           , "OPERATOR_INIT"           },
    { SAHPI_HS_CAUSE_USER_UPDATE             , "USER_UPDATE"             },
    { SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION , "UNEXPECTED_DEACTIVATION" },
    { SAHPI_HS_CAUSE_SURPRISE_EXTRACTION     , "SURPRISE_EXTRACTION"     },
    { SAHPI_HS_CAUSE_EXTRACTION_UPDATE       , "EXTRACTION_UPDATE"       },
    { SAHPI_HS_CAUSE_HARDWARE_FAULT          , "HARDWARE_FAULT"          },
    { SAHPI_HS_CAUSE_CONTAINING_FRU          , "CONTAINING_FRU"          },
    { SAHPI_HS_CAUSE_UNKNOWN                 , "UNKNOWN"                 },
    { 0                                      , 0                         },
};

static EElem SaHpiSeverityTElems[] =
{
    { SAHPI_CRITICAL       , "CRITICAL"       },
    { SAHPI_MAJOR          , "MAJOR"          },
    { SAHPI_MINOR          , "MINOR"          },
    { SAHPI_INFORMATIONAL  , "INFORMATIONAL"  },
    { SAHPI_OK             , "OK"             },
    { SAHPI_DEBUG          , "DEBUG"          },
    { SAHPI_ALL_SEVERITIES , "ALL_SEVERITIES" },
    { 0                    , 0                },
};

static EElem SaHpiResourceEventTypeTElems[] =
{
    { SAHPI_RESE_RESOURCE_FAILURE      , "FAILURE"      },
    { SAHPI_RESE_RESOURCE_RESTORED     , "RESTORED"     },
    { SAHPI_RESE_RESOURCE_ADDED        , "ADDED"        },
    { SAHPI_RESE_RESOURCE_REMOVED      , "REMOVED"      },
    { SAHPI_RESE_RESOURCE_INACCESSIBLE , "INACCESSIBLE" },
    { SAHPI_RESE_RESOURCE_UPDATED      , "UPDATED"      },
    { 0                                , 0              },
};

static EElem SaHpiDomainEventTypeTElems[] =
{
    { SAHPI_DOMAIN_REF_ADDED   , "REF_ADDED"   },
    { SAHPI_DOMAIN_REF_REMOVED , "REF_REMOVED" },
    { 0                        , 0             },
};

static EElem SaHpiSwEventTypeTElems[] =
{
    { SAHPI_HPIE_AUDIT   , "AUDIT"   },
    { SAHPI_HPIE_STARTUP , "STARTUP" },
    { SAHPI_HPIE_OTHER   , "OTHER"   },
    { 0                  , 0         },
};

static EElem SaHpiEventTypeTElems[] =
{
    { SAHPI_ET_RESOURCE             , "RESOURCE"             },
    { SAHPI_ET_DOMAIN               , "DOMAIN"               },
    { SAHPI_ET_SENSOR               , "SENSOR"               },
    { SAHPI_ET_SENSOR_ENABLE_CHANGE , "SENSOR_ENABLE_CHANGE" },
    { SAHPI_ET_HOTSWAP              , "HOTSWAP"              },
    { SAHPI_ET_WATCHDOG             , "WATCHDOG"             },
    { SAHPI_ET_HPI_SW               , "HPI_SW"               },
    { SAHPI_ET_OEM                  , "OEM"                  },
    { SAHPI_ET_USER                 , "USER"                 },
    { SAHPI_ET_DIMI                 , "DIMI"                 },
    { SAHPI_ET_DIMI_UPDATE          , "DIMI_UPDATE"          },
    { SAHPI_ET_FUMI                 , "FUMI"                 },
    { 0                             , 0                      },
};

static EElem SaHpiStatusCondTypeTElems[] =
{
    { SAHPI_STATUS_COND_TYPE_SENSOR   , "SENSOR"   },
    { SAHPI_STATUS_COND_TYPE_RESOURCE , "RESOURCE" },
    { SAHPI_STATUS_COND_TYPE_OEM      , "OEM"      },
    { SAHPI_STATUS_COND_TYPE_USER     , "USER"     },
    { 0                               , 0          },
};

static EElem SaHpiAnnunciatorModeTElems[] =
{
    { SAHPI_ANNUNCIATOR_MODE_AUTO   , "AUTO"   },
    { SAHPI_ANNUNCIATOR_MODE_USER   , "USER"   },
    { SAHPI_ANNUNCIATOR_MODE_SHARED , "SHARED" },
    { 0                             , 0        },
};

static EElem SaHpiAnnunciatorTypeTElems[] =
{
    { SAHPI_ANNUNCIATOR_TYPE_LED                 , "LED"                 },
    { SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE , "DRY_CONTACT_CLOSURE" },
    { SAHPI_ANNUNCIATOR_TYPE_AUDIBLE             , "AUDIBLE"             },
    { SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY         , "LCD_DISPLAY"         },
    { SAHPI_ANNUNCIATOR_TYPE_MESSAGE             , "MESSAGE"             },
    { SAHPI_ANNUNCIATOR_TYPE_COMPOSITE           , "COMPOSITE"           },
    { SAHPI_ANNUNCIATOR_TYPE_OEM                 , "OEM"                 },
    { 0                                          , 0                     },
};

static EElem SaHpiRdrTypeTElems[] =
{
    { SAHPI_NO_RECORD       , "NO_RECORD"       },
    { SAHPI_CTRL_RDR        , "CTRL_RDR"        },
    { SAHPI_SENSOR_RDR      , "SENSOR_RDR"      },
    { SAHPI_INVENTORY_RDR   , "INVENTORY_RDR"   },
    { SAHPI_WATCHDOG_RDR    , "WATCHDOG_RDR"    },
    { SAHPI_ANNUNCIATOR_RDR , "ANNUNCIATOR_RDR" },
    { SAHPI_DIMI_RDR        , "DIMI_RDR"        },
    { SAHPI_FUMI_RDR        , "FUMI_RDR"        },
    { 0                     , 0                 },
};

static EElem SaHpiParmActionTElems[] =
{
    { SAHPI_DEFAULT_PARM , "DEFAULT_PARM" },
    { SAHPI_SAVE_PARM    , "SAVE_PARM"    },
    { SAHPI_RESTORE_PARM , "RESTORE_PARM" },
    { 0                  , 0              },
};

static EElem SaHpiResetActionTElems[] =
{
    { SAHPI_COLD_RESET     , "COLD_RESET"     },
    { SAHPI_WARM_RESET     , "WARM_RESET"     },
    { SAHPI_RESET_ASSERT   , "RESET_ASSERT"   },
    { SAHPI_RESET_DEASSERT , "RESET_DEASSERT" },
    { 0                    , 0                },
};

static EElem SaHpiPowerStateTElems[] =
{
    { SAHPI_POWER_OFF   , "OFF"   },
    { SAHPI_POWER_ON    , "ON"    },
    { SAHPI_POWER_CYCLE , "CYCLE" },
    { 0                 , 0       },
};

static EElem SaHpiEventLogOverflowActionTElems[] =
{
    { SAHPI_EL_OVERFLOW_DROP      , "DROP"      },
    { SAHPI_EL_OVERFLOW_OVERWRITE , "OVERWRITE" },
    { 0                           , 0           },
};


/**************************************************************
 * Flags Codecs
 *************************************************************/
struct FElem
{
    uint64_t     val;
    const char * name;
};

static void ToTxt_Flags( const FElem * elems, const uint64_t& src, std::string& txt )
{
    uint64_t done = 0;
    if ( src != 0 ) {
        bool first = true;
        for ( size_t i = 0; elems[i].name != 0; ++i ) {
            if ( ( src & elems[i].val ) == elems[i].val ) {
                if ( first ) {
                    first = false;
                } else {
                    txt.append( " | " );
                }
                txt.append( elems[i].name );
                done |= elems[i].val;
            }
        }
        if ( done != src ) {
            // done != src - some bits were not recognized
            if ( !first ) {
                txt.append( " | " );
            }
            ToTxt_Uint( src & ( ~done ), txt );
        }
    } else {
        txt.append( "0" );
    }
}

static void ToTxt_Flags8( const FElem * elems, const void * src, std::string& txt )
{
    const uint8_t x = ConstRef<uint8_t>( src );
    ToTxt_Flags( elems, x, txt );
}

static void ToTxt_Flags16( const FElem * elems, const void * src, std::string& txt )
{
    const uint16_t x = ConstRef<uint16_t>( src );
    ToTxt_Flags( elems, x, txt );
}

static void ToTxt_Flags32( const FElem * elems, const void * src, std::string& txt )
{
    const uint32_t x = ConstRef<uint32_t>( src );
    ToTxt_Flags( elems, x, txt );
}

static bool FromTxt_Flags( const FElem * elems, const std::string& txt, uint64_t& dst )
{
    dst = 0;

    static const char delimeters[] = " \t|";
    std::vector<char> buf( txt.begin(), txt.end() );
    buf.push_back( '\0' );
    char * token = strtok( &buf[0], delimeters );
    if ( !token ) {
        return false;
    }
    while ( token ) {
        std::string stoken( token );
        bool found = false;
        for ( size_t i = 0; elems[i].name != 0; ++i ) {
            if ( stoken == elems[i].name ) {
                dst |= elems[i].val;
                found = true;
                break;
            }
        }
        if ( !found ) {
            uint64_t x = 0;
            bool rc = FromTxt_Uint( stoken, x );
            if ( !rc ) {
                return false;
            }
            dst |= x;
        }
        token = strtok( 0, delimeters );
    }

    return true;
}

static bool FromTxt_Flags8( const FElem * elems, const std::string& txt, void * dst )
{
    uint64_t x = 0;
    bool rc = FromTxt_Flags( elems, txt, x );
    if ( rc ) {
        Ref<SaHpiUint8T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_Flags16( const FElem * elems, const std::string& txt, void * dst )
{
    uint64_t x = 0;
    bool rc = FromTxt_Flags( elems, txt, x );
    if ( rc ) {
        Ref<SaHpiUint16T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_Flags32( const FElem * elems, const std::string& txt, void * dst )
{
    uint64_t x = 0;
    bool rc = FromTxt_Flags( elems, txt, x );
    if ( rc ) {
        Ref<SaHpiUint32T>( dst ) = x;
    }

    return rc;
}


/**************************************************************
 * Flags Data
 *************************************************************/
static FElem SaHpiEventStateTElems[] =
{
    { SAHPI_ES_STATE_00 , "STATE_00" },
    { SAHPI_ES_STATE_01 , "STATE_01" },
    { SAHPI_ES_STATE_02 , "STATE_02" },
    { SAHPI_ES_STATE_03 , "STATE_03" },
    { SAHPI_ES_STATE_04 , "STATE_04" },
    { SAHPI_ES_STATE_05 , "STATE_05" },
    { SAHPI_ES_STATE_06 , "STATE_06" },
    { SAHPI_ES_STATE_07 , "STATE_07" },
    { SAHPI_ES_STATE_08 , "STATE_08" },
    { SAHPI_ES_STATE_09 , "STATE_09" },
    { SAHPI_ES_STATE_10 , "STATE_10" },
    { SAHPI_ES_STATE_11 , "STATE_11" },
    { SAHPI_ES_STATE_12 , "STATE_12" },
    { SAHPI_ES_STATE_13 , "STATE_13" },
    { SAHPI_ES_STATE_14 , "STATE_14" },
    { 0                 , 0          },
};

static FElem SaHpiEventStateTThresholdElems[] =
{
    { SAHPI_ES_LOWER_MINOR , "LOWER_MINOR" },
    { SAHPI_ES_LOWER_MAJOR , "LOWER_MAJOR" },
    { SAHPI_ES_LOWER_CRIT  , "LOWER_CRIT"  },
    { SAHPI_ES_UPPER_MINOR , "UPPER_MINOR" },
    { SAHPI_ES_UPPER_MAJOR , "UPPER_MAJOR" },
    { SAHPI_ES_UPPER_CRIT  , "UPPER_CRIT"  },
    { 0                    , 0             },
};

static FElem SaHpiSensorRangeFlagsTElems[] =
{
    { SAHPI_SRF_MIN        , "MIN"        },
    { SAHPI_SRF_MAX        , "MAX"        },
    { SAHPI_SRF_NORMAL_MIN , "NORMAL_MIN" },
    { SAHPI_SRF_NORMAL_MAX , "NORMAL_MAX" },
    { SAHPI_SRF_NOMINAL    , "NOMINAL"    },
    { 0                    , 0            },
};

static FElem SaHpiSensorThdMaskTElems[] =
{
    { SAHPI_STM_LOW_MINOR      , "LOW_MINOR"      },
    { SAHPI_STM_LOW_MAJOR      , "LOW_MAJOR"      },
    { SAHPI_STM_LOW_CRIT       , "LOW_CRIT"       },
    { SAHPI_STM_UP_MINOR       , "UP_MINOR"       },
    { SAHPI_STM_UP_MAJOR       , "UP_MAJOR"       },
    { SAHPI_STM_UP_CRIT        , "UP_CRIT"        },
    { SAHPI_STM_UP_HYSTERESIS  , "UP_HYSTERESIS"  },
    { SAHPI_STM_LOW_HYSTERESIS , "LOW_HYSTERESIS" },
    { 0                        , 0                },
};

static FElem SaHpiWatchdogExpFlagsTElems[] =
{
    { SAHPI_WATCHDOG_EXP_BIOS_FRB2 , "BIOS_FRB2" },
    { SAHPI_WATCHDOG_EXP_BIOS_POST , "BIOS_POST" },
    { SAHPI_WATCHDOG_EXP_OS_LOAD   , "OS_LOAD"   },
    { SAHPI_WATCHDOG_EXP_SMS_OS    , "SMS_OS"    },
    { SAHPI_WATCHDOG_EXP_OEM       , "OEM"       },
    { 0                            , 0           },
};

static FElem SaHpiDimiTestCapabilityTElems[] =
{
    { SAHPI_DIMITEST_CAPABILITY_RESULTSOUTPUT , "RESULTSOUTPUT" },
    { SAHPI_DIMITEST_CAPABILITY_SERVICEMODE   , "SERVICEMODE"   },
    { SAHPI_DIMITEST_CAPABILITY_LOOPCOUNT     , "LOOPCOUNT"     },
    { SAHPI_DIMITEST_CAPABILITY_LOOPTIME      , "LOOPTIME"      },
    { SAHPI_DIMITEST_CAPABILITY_LOGGING       , "LOGGING"       },
    { SAHPI_DIMITEST_CAPABILITY_TESTCANCEL    , "TESTCANCEL"    },
    { 0                                       , 0               },
};

static FElem SaHpiFumiLogicalBankStateFlagsTElems[] =
{
    { SAHPI_FUMI_NO_MAIN_PERSISTENT_COPY , "NO_MAIN_PERSISTENT_COPY" },
    { 0                                  , 0                         },
};

static FElem SaHpiFumiProtocolTElems[] =
{
    { SAHPI_FUMI_PROT_TFTP     , "TFTP"     },
    { SAHPI_FUMI_PROT_FTP      , "FTP"      },
    { SAHPI_FUMI_PROT_HTTP     , "HTTP"     },
    { SAHPI_FUMI_PROT_LDAP     , "LDAP"     },
    { SAHPI_FUMI_PROT_LOCAL    , "LOCAL"    },
    { SAHPI_FUMI_PROT_NFS      , "NFS"      },
    { SAHPI_FUMI_PROT_DBACCESS , "DBACCESS" },
    { 0                        , 0          },
};

static FElem SaHpiFumiCapabilityTElems[] =
{
    { SAHPI_FUMI_CAP_ROLLBACK                     , "ROLLBACK"                     },
    { SAHPI_FUMI_CAP_BANKCOPY                     , "BANKCOPY"                     },
    { SAHPI_FUMI_CAP_BANKREORDER                  , "BANKREORDER"                  },
    { SAHPI_FUMI_CAP_BACKUP                       , "BACKUP"                       },
    { SAHPI_FUMI_CAP_TARGET_VERIFY                , "TARGET_VERIFY"                },
    { SAHPI_FUMI_CAP_TARGET_VERIFY_MAIN           , "TARGET_VERIFY_MAIN"           },
    { SAHPI_FUMI_CAP_COMPONENTS                   , "COMPONENTS"                   },
    { SAHPI_FUMI_CAP_AUTOROLLBACK                 , "AUTOROLLBACK"                 },
    { SAHPI_FUMI_CAP_AUTOROLLBACK_CAN_BE_DISABLED , "AUTOROLLBACK_CAN_BE_DISABLED" },
    { SAHPI_FUMI_CAP_MAIN_NOT_PERSISTENT          , "MAIN_NOT_PERSISTENT"          },
    { 0                                           , 0                              },
};

static FElem SaHpiSensorOptionalDataTElems[] =
{
    { SAHPI_SOD_TRIGGER_READING   , "TRIGGER_READING"   },
    { SAHPI_SOD_TRIGGER_THRESHOLD , "TRIGGER_THRESHOLD" },
    { SAHPI_SOD_OEM               , "OEM"               },
    { SAHPI_SOD_PREVIOUS_STATE    , "PREVIOUS_STATE"    },
    { SAHPI_SOD_CURRENT_STATE     , "CURRENT_STATE"     },
    { SAHPI_SOD_SENSOR_SPECIFIC   , "SENSOR_SPECIFIC"   },
    { 0                           , 0                   },
};

static FElem SaHpiSensorEnableOptDataTElems[] =
{
    { SAHPI_SEOD_CURRENT_STATE , "CURRENT_STATE" },
    { SAHPI_SEOD_ALARM_STATES  , "ALARM_STATES"  },
    { 0                        , 0               },
};

static FElem SaHpiCapabilitiesTElems[] =
{
    { SAHPI_CAPABILITY_RESOURCE         , "RESOURCE"         },
    { SAHPI_CAPABILITY_FUMI             , "FUMI"             },
    { SAHPI_CAPABILITY_EVT_DEASSERTS    , "EVT_DEASSERTS"    },
    { SAHPI_CAPABILITY_DIMI             , "DIMI"             },
    { SAHPI_CAPABILITY_AGGREGATE_STATUS , "AGGREGATE_STATUS" },
    { SAHPI_CAPABILITY_CONFIGURATION    , "CONFIGURATION"    },
    { SAHPI_CAPABILITY_MANAGED_HOTSWAP  , "MANAGED_HOTSWAP"  },
    { SAHPI_CAPABILITY_WATCHDOG         , "WATCHDOG"         },
    { SAHPI_CAPABILITY_CONTROL          , "CONTROL"          },
    { SAHPI_CAPABILITY_FRU              , "FRU"              },
    { SAHPI_CAPABILITY_LOAD_ID          , "LOAD_ID"          },
    { SAHPI_CAPABILITY_ANNUNCIATOR      , "ANNUNCIATOR"      },
    { SAHPI_CAPABILITY_POWER            , "POWER"            },
    { SAHPI_CAPABILITY_RESET            , "RESET"            },
    { SAHPI_CAPABILITY_INVENTORY_DATA   , "INVENTORY_DATA"   },
    { SAHPI_CAPABILITY_EVENT_LOG        , "EVENT_LOG"        },
    { SAHPI_CAPABILITY_RDR              , "RDR"              },
    { SAHPI_CAPABILITY_SENSOR           , "SENSOR"           },
    { 0                                 , 0                  },
};

static FElem SaHpiHsCapabilitiesTElems[] =
{
    { SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY , "AUTOEXTRACT_READ_ONLY" },
    { SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED   , "INDICATOR_SUPPORTED"   },
    { SAHPI_HS_CAPABILITY_AUTOINSERT_IMMEDIATE  , "AUTOINSERT_IMMEDIATE"  },
    { 0                                         , 0                       },
};

static FElem SaHpiEventLogCapabilitiesTElems[] =
{
    { SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD      , "ENTRY_ADD"      },
    { SAHPI_EVTLOG_CAPABILITY_CLEAR          , "CLEAR"          },
    { SAHPI_EVTLOG_CAPABILITY_TIME_SET       , "TIME_SET"       },
    { SAHPI_EVTLOG_CAPABILITY_STATE_SET      , "STATE_SET"      },
    { SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET , "OVERFLOW_RESET" },
    { 0                                      , 0                },
};


/**************************************************************
 * Buffer Codecs
 *************************************************************/
static void ToTxt_Buffer( const SaHpiTextTypeT type,
                          const void * src,
                          size_t len,
                          std::string& txt )
{
    ToTxt_Enum( SaHpiTextTypeTElems, &type, txt );
    txt.append( ":" );

    switch ( type ) {
        case SAHPI_TL_TYPE_UNICODE:
            // TODO unicode
            break;
        case SAHPI_TL_TYPE_BCDPLUS:
        case SAHPI_TL_TYPE_ASCII6:
        case SAHPI_TL_TYPE_TEXT:
            ToTxt_Ascii( src, len, txt );
            break;
        case SAHPI_TL_TYPE_BINARY:
            ToTxt_Binary( src, len, txt );
            break;
        default:
            break;
    }
}

static bool FromTxt_Buffer( const std::string& txt,
                            size_t capacity,
                            SaHpiTextTypeT& type,
                            void * dst,
                            size_t& len )
{
    std::string::const_iterator iter;

    size_t n = txt.find_first_of( ':' );
    if ( n == std::string::npos ) {
        type = SAHPI_TL_TYPE_TEXT;
        iter = txt.begin();
    } else {
        iter = txt.begin() + n;
        std::string stype( txt.begin(), iter );
        bool rc = FromTxt_Enum( SaHpiTextTypeTElems, stype, &type );
        if ( !rc ) {
            return false;
        }
        ++iter;
    }

    std::string data( iter, txt.end() );

    switch ( type ) {
        case SAHPI_TL_TYPE_UNICODE:
            // TODO unicode
            return false;
        case SAHPI_TL_TYPE_BCDPLUS:
        case SAHPI_TL_TYPE_ASCII6:
        case SAHPI_TL_TYPE_TEXT:
            return FromTxt_Ascii( data, capacity, dst, len );
        case SAHPI_TL_TYPE_BINARY:
            return FromTxt_Binary( data, capacity, dst, len );
        default:
            return false;
    }
}


/**************************************************************
 * ToTxt_XXX
 *************************************************************/
static void ToTxt_SaHpiUint8T( const void * src, std::string& txt )
{
    const uint64_t x = ConstRef<SaHpiUint8T>( src );
    ToTxt_Uint( x, txt );
}

static void ToTxt_SaHpiUint16T( const void * src, std::string& txt )
{
    const uint64_t x = ConstRef<SaHpiUint16T>( src );
    ToTxt_Uint( x, txt );
}

static void ToTxt_SaHpiUint32T( const void * src, std::string& txt )
{
    const uint64_t x = ConstRef<SaHpiUint32T>( src );
    ToTxt_Uint( x, txt );
}

static void ToTxt_SaHpiUint64T( const void * src, std::string& txt )
{
    const uint64_t x = ConstRef<SaHpiUint64T>( src );
    ToTxt_Uint( x, txt );
}

static void ToTxt_SaHpiInt8T( const void * src, std::string& txt )
{
    int64_t x = ConstRef<SaHpiInt8T>( src );
    ToTxt_Int( x, txt );
}

static void ToTxt_SaHpiInt16T( const void * src, std::string& txt )
{
    int64_t x = ConstRef<SaHpiInt16T>( src );
    ToTxt_Int( x, txt );
}

static void ToTxt_SaHpiInt32T( const void * src, std::string& txt )
{
    int64_t x = ConstRef<SaHpiInt32T>( src );
    ToTxt_Int( x, txt );
}

static void ToTxt_SaHpiInt64T( const void * src, std::string& txt )
{
    int64_t x = ConstRef<SaHpiInt64T>( src );
    ToTxt_Int( x, txt );
}

static void ToTxt_SaHpiFloat64T( const void * src, std::string& txt )
{
    double x = ConstRef<SaHpiFloat64T>( src );
    ToTxt_Double( x, txt );
}

static void ToTxt_SaHpiBoolT( const void * src, std::string& txt )
{
    SaHpiBoolT x = ConstRef<SaHpiBoolT>( src );
    txt.append( ( x == SAHPI_FALSE ) ? "FALSE" : "TRUE" );
}

static void ToTxt_SaHpiDomainIdT( const void * src, std::string& txt )
{
    SaHpiDomainIdT x = ConstRef<SaHpiDomainIdT>( src );
    if ( x == SAHPI_UNSPECIFIED_DOMAIN_ID ) {
        txt.append( "UNSPECIFIED" );
        return;
    }

    ToTxt_SaHpiUint32T( src, txt );
}

static void ToTxt_SaHpiResourceIdT( const void * src, std::string& txt )
{
    SaHpiResourceIdT x = ConstRef<SaHpiResourceIdT>( src );
    if ( x == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        txt.append( "UNSPECIFIED" );
        return;
    }

    ToTxt_SaHpiUint32T( src, txt );
}

static void ToTxt_SaHpiTimeT( const void * src, std::string& txt )
{
    SaHpiTimeT x = ConstRef<SaHpiTimeT>( src );
    if ( x == SAHPI_TIME_UNSPECIFIED ) {
        txt.append( "UNSPECIFIED" );
        return;
    }

    ToTxt_SaHpiInt64T( src, txt );
}

static void ToTxt_SaHpiTimeoutT( const void * src, std::string& txt )
{
    SaHpiTimeoutT x = ConstRef<SaHpiTimeoutT>( src );
    if ( x == SAHPI_TIMEOUT_IMMEDIATE ) {
        txt.append( "IMMEDIATE" );
        return;
    } else if ( x == SAHPI_TIMEOUT_BLOCK ) {
        txt.append( "BLOCK" );
        return;
    }

    ToTxt_SaHpiInt64T( src, txt );
}

static void ToTxt_SaHpiTextBufferT( const void * src, std::string& txt )
{
    const SaHpiTextBufferT& tb = ConstRef<SaHpiTextBufferT>( src );
    ToTxt_Buffer( tb.DataType, &tb.Data, tb.DataLength, txt );
}

static void ToTxt_SaHpiEntityPathT( const void * src, std::string& txt )
{
    const SaHpiEntityPathT& ep = ConstRef<SaHpiEntityPathT>( src );
    oh_big_textbuffer buf;
    oh_decode_entitypath( &ep, &buf );
    txt.append( ConstPtr<char>( &buf.Data[0] ), buf.DataLength );
}

static void ToTxt_SaHpiNameT( const void * src, std::string& txt )
{
    const SaHpiNameT& name = ConstRef<SaHpiNameT>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_TEXT, &name.Value, name.Length, txt );
}

static void ToTxt_SaHpiLoadNumberT( const void * src, std::string& txt )
{
    SaHpiLoadNumberT ln = ConstRef<SaHpiLoadNumberT>( src );
    if ( ln == SAHPI_LOAD_ID_DEFAULT ) {
        txt.append( "DEFAULT" );
        return;
    } else if ( ln == SAHPI_LOAD_ID_BYNAME ) {
        txt.append( "BYNAME" );
        return;
    }

    ToTxt_SaHpiUint32T( src, txt );
}

static void ToTxt_SaHpiGuidT( const void * src, std::string& txt )
{
    const SaHpiGuidT& guid = ConstRef<SaHpiGuidT>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_BINARY, &guid, sizeof(guid), txt );
}

static void ToTxt_SaHpiCtrlStateStreamTWithoutRepeat( const void * src, std::string& txt )
{
    const SaHpiCtrlStateStreamT& state = ConstRef<SaHpiCtrlStateStreamT>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_BINARY, &state.Stream, state.StreamLength, txt );
}

static void ToTxt_SaHpiCtrlStateOemTWithoutMId( const void * src, std::string& txt )
{
    const SaHpiCtrlStateOemT& state = ConstRef<SaHpiCtrlStateOemT>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_BINARY, &state.Body, state.BodyLength, txt );
}

static void ToTxt_ControlOemConfigData( const void * src, std::string& txt )
{
    const SaHpiUint8T * cd = ConstPtr<SaHpiUint8T>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_BINARY, cd, SAHPI_CTRL_OEM_CONFIG_LENGTH, txt );
}

static void ToTxt_SensorReadingBuffer( const void * src, std::string& txt )
{
    const SaHpiUint8T * cd = ConstPtr<SaHpiUint8T>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_BINARY, cd, SAHPI_SENSOR_BUFFER_LENGTH, txt );
}

static void ToTxt_DimiTestParamName( const void * src, std::string& txt )
{
    const SaHpiUint8T * name = ConstPtr<SaHpiUint8T>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_TEXT, name, SAHPI_DIMITEST_PARAM_NAME_LEN, txt );
}

static void ToTxt_SaHpiFumiOemDefinedSpecInfoTWithoutMid( const void * src, std::string& txt )
{
    const SaHpiFumiOemDefinedSpecInfoT& info = ConstRef<SaHpiFumiOemDefinedSpecInfoT>( src );
    ToTxt_Buffer( SAHPI_TL_TYPE_BINARY, &info.Body, info.BodyLength, txt );
}


/**************************************************************
 * FromTxt_XXX
 *************************************************************/
static bool FromTxt_SaHpiUint8T( const std::string& txt, void * dst )
{
    uint64_t x = 0;
    bool rc = FromTxt_Uint( txt, x );
    if ( rc ) {
        Ref<SaHpiUint8T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiUint16T( const std::string& txt, void * dst )
{
    uint64_t x = 0;
    bool rc = FromTxt_Uint( txt, x );
    if ( rc ) {
        Ref<SaHpiUint16T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiUint32T( const std::string& txt, void * dst )
{
    uint64_t x = 0;
    bool rc = FromTxt_Uint( txt, x );
    if ( rc ) {
        Ref<SaHpiUint32T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiUint64T( const std::string& txt, void * dst )
{
    uint64_t x = 0;
    bool rc = FromTxt_Uint( txt, x );
    if ( rc ) {
        Ref<SaHpiUint64T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiInt8T( const std::string& txt, void * dst )
{
    int64_t x = 0;
    bool rc = FromTxt_Int( txt, x );
    if ( rc ) {
        Ref<SaHpiInt8T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiInt16T( const std::string& txt, void * dst )
{
    int64_t x = 0;
    bool rc = FromTxt_Int( txt, x );
    if ( rc ) {
        Ref<SaHpiInt16T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiInt32T( const std::string& txt, void * dst )
{
    int64_t x = 0;
    bool rc = FromTxt_Int( txt, x );
    if ( rc ) {
        Ref<SaHpiInt32T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiInt64T( const std::string& txt, void * dst )
{
    int64_t x = 0;
    bool rc = FromTxt_Int( txt, x );
    if ( rc ) {
        Ref<SaHpiInt64T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiFloat64T( const std::string& txt, void * dst )
{
    double x = 0;
    bool rc = FromTxt_Double( txt, x );
    if ( rc ) {
        Ref<SaHpiFloat64T>( dst ) = x;
    }

    return rc;
}

static bool FromTxt_SaHpiBoolT( const std::string& txt, void * dst )
{
    SaHpiBoolT& x = Ref<SaHpiBoolT>( dst );
    if ( txt == "FALSE" ) {
        x = SAHPI_FALSE;
        return true;
    } else if ( txt == "TRUE" ) {
        x = SAHPI_TRUE;
        return true;
    } else {
        return false;
    }
}

static bool FromTxt_SaHpiDomainIdT( const std::string& txt, void * dst )
{
    SaHpiDomainIdT& x = Ref<SaHpiDomainIdT>( dst );
    if ( txt == "UNSPECIFIED" ) {
        x = SAHPI_UNSPECIFIED_DOMAIN_ID;
        return true;
    }

    return FromTxt_SaHpiUint32T( txt, dst );
}

static bool FromTxt_SaHpiResourceIdT( const std::string& txt, void * dst )
{
    SaHpiResourceIdT& x = Ref<SaHpiResourceIdT>( dst );
    if ( txt == "UNSPECIFIED" ) {
        x = SAHPI_UNSPECIFIED_RESOURCE_ID;
        return true;
    }

    return FromTxt_SaHpiUint32T( txt, dst );
}

static bool FromTxt_SaHpiTimeT( const std::string& txt, void * dst )
{
    SaHpiTimeT& x = Ref<SaHpiTimeT>( dst );
    if ( txt == "UNSPECIFIED" ) {
        x = SAHPI_TIME_UNSPECIFIED;
        return true;
    }

    return FromTxt_SaHpiInt64T( txt, dst );
}

static bool FromTxt_SaHpiTimeoutT( const std::string& txt, void * dst )
{
    SaHpiTimeoutT& x = Ref<SaHpiTimeoutT>( dst );
    if ( txt == "IMMEDIATE" ) {
        x = SAHPI_TIMEOUT_IMMEDIATE;
        return true;
    } else if ( txt == "BLOCK" ) {
        x = SAHPI_TIMEOUT_BLOCK;
        return true;
    }

    return FromTxt_SaHpiInt64T( txt, dst );
}

static bool FromTxt_SaHpiTextBufferT( const std::string& txt, void * dst )
{
    SaHpiTextBufferT& tb = Ref<SaHpiTextBufferT>( dst );
    size_t len = 0;
    bool rc = FromTxt_Buffer( txt,
                              SAHPI_MAX_TEXT_BUFFER_LENGTH,
                              tb.DataType,
                              &tb.Data,
                              len );
    if ( rc ) {
        tb.DataLength = len;
    }

    return rc;
}

static bool FromTxt_SaHpiInstrumentIdT( const std::string& txt, void * dst )
{
    return FromTxt_SaHpiUint32T( txt, dst );
}

static bool FromTxt_SaHpiEntityPathT( const std::string& txt, void * dst )
{
    SaHpiEntityPathT& ep = Ref<SaHpiEntityPathT>( dst );
    SaErrorT rv = oh_encode_entitypath( txt.c_str(), &ep );

    return ( rv == SA_OK );
}

static bool FromTxt_SaHpiNameT( const std::string& txt, void * dst )
{
    SaHpiNameT& name = Ref<SaHpiNameT>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    bool rc = FromTxt_Buffer( txt,
                              SA_HPI_MAX_NAME_LENGTH,
                              type,
                              &name.Value,
                              len );
    if ( rc ) {
        name.Length = len;
    }

    return rc;
}

static bool FromTxt_SaHpiLoadNumberT( const std::string& txt, void * dst )
{
    SaHpiLoadNumberT& ln = Ref<SaHpiLoadNumberT>( dst );
    if ( txt == "DEFAULT" ) {
        ln = SAHPI_LOAD_ID_DEFAULT;
        return true;
    } else if ( txt == "BYNAME" ) {
        ln = SAHPI_LOAD_ID_BYNAME;
        return true;
    }

    return FromTxt_SaHpiUint32T( txt, dst );
}

static bool FromTxt_SaHpiGuidT( const std::string& txt, void * dst )
{
    SaHpiGuidT& guid = Ref<SaHpiGuidT>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    return FromTxt_Buffer( txt, sizeof(guid), type, &guid, len );
}

static bool FromTxt_SaHpiCtrlStateStreamTWithoutRepeat( const std::string& txt, void * dst )
{
    SaHpiCtrlStateStreamT& state = Ref<SaHpiCtrlStateStreamT>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    bool rc = FromTxt_Buffer( txt,
                              SAHPI_CTRL_MAX_STREAM_LENGTH,
                              type,
                              &state.Stream,
                              len );
    if ( rc ) {
        state.StreamLength = len;
    }

    return rc;
}

static bool FromTxt_SaHpiCtrlStateOemTWithoutMId( const std::string& txt, void * dst )
{
    SaHpiCtrlStateOemT& state = Ref<SaHpiCtrlStateOemT>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    bool rc = FromTxt_Buffer( txt,
                              SAHPI_CTRL_MAX_OEM_BODY_LENGTH,
                              type,
                              &state.Body,
                              len );
    if ( rc ) {
        state.BodyLength = len;
    }

    return rc;
}

static bool FromTxt_ControlOemConfigData( const std::string& txt, void * dst )
{
    SaHpiUint8T * cd = Ptr<SaHpiUint8T>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    return FromTxt_Buffer( txt, SAHPI_CTRL_OEM_CONFIG_LENGTH, type, cd, len );
}

static bool FromTxt_SensorReadingBuffer( const std::string& txt, void * dst )
{
    SaHpiUint8T * cd = Ptr<SaHpiUint8T>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    return FromTxt_Buffer( txt, SAHPI_SENSOR_BUFFER_LENGTH, type, cd, len );
}

static bool FromTxt_DimiTestParamName( const std::string& txt, void * dst )
{
    SaHpiUint8T * name = Ptr<SaHpiUint8T>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    return FromTxt_Buffer( txt, SAHPI_DIMITEST_PARAM_NAME_LEN, type, name, len );
}

static bool FromTxt_SaHpiFumiOemDefinedSpecInfoTWithoutMid( const std::string& txt, void * dst )
{
    SaHpiFumiOemDefinedSpecInfoT& info = Ref<SaHpiFumiOemDefinedSpecInfoT>( dst );
    SaHpiTextTypeT type;
    size_t len = 0;
    bool rc = FromTxt_Buffer( txt,
                              SAHPI_FUMI_MAX_OEM_BODY_LENGTH,
                              type,
                              &info.Body,
                              len );
    if ( rc ) {
        info.BodyLength = len;
    }

    return rc;
}


/**************************************************************
 * Codec Interface
 *************************************************************/

// Data Codecs
void ToTxt( const Var& var, std::string& txt )
{
    const void * src = var.rdata;

    switch ( var.type ) {
        case dtSaHpiUint8T:
            ToTxt_SaHpiUint8T( src, txt );
            return;
        case dtSaHpiUint16T:
            ToTxt_SaHpiUint16T( src, txt );
            return;
        case dtSaHpiUint32T:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiUint64T:
            ToTxt_SaHpiUint64T( src, txt );
            return;
        case dtSaHpiInt8T:
            ToTxt_SaHpiInt8T( src, txt );
            return;
        case dtSaHpiInt16T:
            ToTxt_SaHpiInt16T( src, txt );
            return;
        case dtSaHpiInt32T:
            ToTxt_SaHpiInt32T( src, txt );
            return;
        case dtSaHpiInt64T:
            ToTxt_SaHpiInt64T( src, txt );
            return;
        case dtSaHpiFloat64T:
            ToTxt_SaHpiFloat64T( src, txt );
            return;
        case dtSaHpiBoolT:
            ToTxt_SaHpiBoolT( src, txt );
            return;
        case dtSaHpiManufacturerIdT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiDomainIdT:
            ToTxt_SaHpiDomainIdT( src, txt );
            return;
        case dtSaHpiResourceIdT:
            ToTxt_SaHpiResourceIdT( src, txt );
            return;
        case dtSaHpiEntryIdT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiTimeT:
            ToTxt_SaHpiTimeT( src, txt );
            return;
        case dtSaHpiTimeoutT:
            ToTxt_SaHpiTimeoutT( src, txt );
            return;
        case dtSaHpiLanguageT:
            ToTxt_Enum( SaHpiLanguageTElems, src, txt );
            return;
        case dtSaHpiTextTypeT:
            ToTxt_Enum( SaHpiTextTypeTElems, src, txt );
            return;
        case dtSaHpiTextBufferT:
            ToTxt_SaHpiTextBufferT( src, txt );
            return;
        case dtSaHpiInstrumentIdT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiEntityPathT:
            ToTxt_SaHpiEntityPathT( src, txt );
            return;
        case dtSaHpiEventCategoryT:
            ToTxt_EnumU8( SaHpiEventCategoryTElems, src, txt );
            return;
        case dtSaHpiEventStateT:
            ToTxt_Flags16( SaHpiEventStateTElems, src, txt );
            return;
        case dtSaHpiEventStateTThreshold:
            ToTxt_Flags16( SaHpiEventStateTThresholdElems, src, txt );
            return;
        case dtSaHpiSensorNumT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiSensorTypeT:
            ToTxt_Enum( SaHpiSensorTypeTElems, src, txt );
            return;
        case dtSaHpiSensorReadingTypeT:
            ToTxt_Enum( SaHpiSensorReadingTypeTElems, src, txt );
            return;
        case dtSaHpiSensorRangeFlagsT:
            ToTxt_Flags8( SaHpiSensorRangeFlagsTElems, src, txt );
            return;
        case dtSaHpiSensorUnitsT:
            ToTxt_Enum( SaHpiSensorUnitsTElems, src, txt );
            return;
        case dtSaHpiSensorModUnitUseT:
            ToTxt_Enum( SaHpiSensorModUnitUseTElems, src, txt );
            return;
        case dtSaHpiSensorThdMaskT:
            ToTxt_Flags8( SaHpiSensorThdMaskTElems, src, txt );
            return;
        case dtSaHpiSensorEventCtrlT:
            ToTxt_Enum( SaHpiSensorEventCtrlTElems, src, txt );
            return;
        case dtSaHpiCtrlNumT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiCtrlTypeT:
            ToTxt_Enum( SaHpiCtrlTypeTElems, src, txt );
            return;
        case dtSaHpiCtrlStateDigitalT:
            ToTxt_Enum( SaHpiCtrlStateDigitalTElems, src, txt );
            return;
        case dtSaHpiCtrlStateDiscreteT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiCtrlStateAnalogT:
            ToTxt_SaHpiInt32T( src, txt );
            return;
        case dtSaHpiTxtLineNumT:
            ToTxt_SaHpiUint8T( src, txt );
            return;
        case dtSaHpiCtrlModeT:
            ToTxt_Enum( SaHpiCtrlModeTElems, src, txt );
            return;
        case dtSaHpiCtrlOutputTypeT:
            ToTxt_Enum( SaHpiCtrlOutputTypeTElems, src, txt );
            return;
        case dtSaHpiIdrIdT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiIdrAreaTypeT:
            ToTxt_Enum( SaHpiIdrAreaTypeTElems, src, txt );
            return;
        case dtSaHpiIdrFieldTypeT:
            ToTxt_Enum( SaHpiIdrFieldTypeTElems, src, txt );
            return;
        case dtSaHpiWatchdogNumT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiWatchdogActionT:
            ToTxt_Enum( SaHpiWatchdogActionTElems, src, txt );
            return;
        case dtSaHpiWatchdogActionEventT:
            ToTxt_Enum( SaHpiWatchdogActionEventTElems, src, txt );
            return;
        case dtSaHpiWatchdogPretimerInterruptT:
            ToTxt_Enum( SaHpiWatchdogPretimerInterruptTElems, src, txt );
            return;
        case dtSaHpiWatchdogTimerUseT:
            ToTxt_Enum( SaHpiWatchdogTimerUseTElems, src, txt );
            return;
        case dtSaHpiWatchdogExpFlagsT:
            ToTxt_Flags8( SaHpiWatchdogExpFlagsTElems, src, txt );
            return;
        case dtSaHpiDimiNumT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiDimiTestServiceImpactT:
            ToTxt_Enum( SaHpiDimiTestServiceImpactTElems, src, txt );
            return;
        case dtSaHpiDimiTestRunStatusT:
            ToTxt_Enum( SaHpiDimiTestRunStatusTElems, src, txt );
            return;
        case dtSaHpiDimiTestErrCodeT:
            ToTxt_Enum( SaHpiDimiTestErrCodeTElems, src, txt );
            return;
        case dtSaHpiDimiTestParamTypeT:
            ToTxt_Enum( SaHpiDimiTestParamTypeTElems, src, txt );
            return;
        case dtSaHpiDimiTestCapabilityT:
            ToTxt_Flags32( SaHpiDimiTestCapabilityTElems, src, txt );
            return;
        case dtSaHpiDimiTestNumT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiDimiTestPercentCompletedT:
            ToTxt_SaHpiUint8T( src, txt );
            return;
        case dtSaHpiDimiReadyT:
            ToTxt_Enum( SaHpiDimiReadyTElems, src, txt );
            return;
        case dtSaHpiFumiNumT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiBankNumT:
            ToTxt_SaHpiUint8T( src, txt );
            return;
        case dtSaHpiFumiSpecInfoTypeT:
            ToTxt_Enum( SaHpiFumiSpecInfoTypeTElems, src, txt );
            return;
        case dtSaHpiFumiSafDefinedSpecIdT:
            ToTxt_Enum( SaHpiFumiSafDefinedSpecIdTElems, src, txt );
            return;
        case dtSaHpiFumiServiceImpactT:
            ToTxt_Enum( SaHpiFumiServiceImpactTElems, src, txt );
            return;
        case dtSaHpiFumiSourceStatusT:
            ToTxt_Enum( SaHpiFumiSourceStatusTElems, src, txt );
            return;
        case dtSaHpiFumiBankStateT:
            ToTxt_Enum( SaHpiFumiBankStateTElems, src, txt );
            return;
        case dtSaHpiFumiUpgradeStatusT:
            ToTxt_Enum( SaHpiFumiUpgradeStatusTElems, src, txt );
            return;
        case dtSaHpiFumiLogicalBankStateFlagsT:
            ToTxt_Flags32( SaHpiFumiLogicalBankStateFlagsTElems, src, txt );
            return;
        case dtSaHpiFumiProtocolT:
            ToTxt_Flags32( SaHpiFumiProtocolTElems, src, txt );
            return;
        case dtSaHpiFumiCapabilityT:
            ToTxt_Flags32( SaHpiFumiCapabilityTElems, src, txt );
            return;
        case dtSaHpiHsIndicatorStateT:
            ToTxt_Enum( SaHpiHsIndicatorStateTElems, src, txt );
            return;
        case dtSaHpiHsStateT:
            ToTxt_Enum( SaHpiHsStateTElems, src, txt );
            return;
        case dtSaHpiHsCauseOfStateChangeT:
            ToTxt_Enum( SaHpiHsCauseOfStateChangeTElems, src, txt );
            return;
        case dtSaHpiSeverityT:
            ToTxt_Enum( SaHpiSeverityTElems, src, txt );
            return;
        case dtSaHpiResourceEventTypeT:
            ToTxt_Enum( SaHpiResourceEventTypeTElems, src, txt );
            return;
        case dtSaHpiDomainEventTypeT:
            ToTxt_Enum( SaHpiDomainEventTypeTElems, src, txt );
            return;
        case dtSaHpiSensorOptionalDataT:
            ToTxt_Flags8( SaHpiSensorOptionalDataTElems, src, txt );
            return;
        case dtSaHpiSensorEnableOptDataT:
            ToTxt_Flags8( SaHpiSensorEnableOptDataTElems, src, txt );
            return;
        case dtSaHpiSwEventTypeT:
            ToTxt_Enum( SaHpiSwEventTypeTElems, src, txt );
            return;
        case dtSaHpiEventTypeT:
            ToTxt_Enum( SaHpiEventTypeTElems, src, txt );
            return;
        case dtSaHpiAnnunciatorNumT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiNameT:
            ToTxt_SaHpiNameT( src, txt );
            return;
        case dtSaHpiStatusCondTypeT:
            ToTxt_Enum( SaHpiStatusCondTypeTElems, src, txt );
            return;
        case dtSaHpiAnnunciatorModeT:
            ToTxt_Enum( SaHpiAnnunciatorModeTElems, src, txt );
            return;
        case dtSaHpiAnnunciatorTypeT:
            ToTxt_Enum( SaHpiAnnunciatorTypeTElems, src, txt );
            return;
        case dtSaHpiRdrTypeT:
            ToTxt_Enum( SaHpiRdrTypeTElems, src, txt );
            return;
        case dtSaHpiParmActionT:
            ToTxt_Enum( SaHpiParmActionTElems, src, txt );
            return;
        case dtSaHpiResetActionT:
            ToTxt_Enum( SaHpiResetActionTElems, src, txt );
            return;
        case dtSaHpiPowerStateT:
            ToTxt_Enum( SaHpiPowerStateTElems, src, txt );
            return;
        case dtSaHpiLoadNumberT:
            ToTxt_SaHpiLoadNumberT( src, txt );
            return;
        case dtSaHpiGuidT:
            ToTxt_SaHpiGuidT( src, txt );
            return;
        case dtSaHpiCapabilitiesT:
            ToTxt_Flags32( SaHpiCapabilitiesTElems, src, txt );
            return;
        case dtSaHpiHsCapabilitiesT:
            ToTxt_Flags32( SaHpiHsCapabilitiesTElems, src, txt );
            return;
        case dtSaHpiEventLogOverflowActionT:
            ToTxt_Enum( SaHpiEventLogOverflowActionTElems, src, txt );
            return;
        case dtSaHpiEventLogCapabilitiesT:
            ToTxt_Flags32( SaHpiEventLogCapabilitiesTElems, src, txt );
            return;
        case dtSaHpiEventLogEntryIdT:
            ToTxt_SaHpiUint32T( src, txt );
            return;
        case dtSaHpiCtrlStateStreamTWithoutRepeat:
            ToTxt_SaHpiCtrlStateStreamTWithoutRepeat( src, txt );
            return;
        case dtSaHpiCtrlStateOemTWithoutMId:
            ToTxt_SaHpiCtrlStateOemTWithoutMId( src, txt );
            return;
        case dtControlOemConfigData:
            ToTxt_ControlOemConfigData( src, txt );
            return;
        case dtSensorReadingBuffer:
            ToTxt_SensorReadingBuffer( src, txt );
            return;
        case dtDimiTestParamName:
            ToTxt_DimiTestParamName( src, txt );
            return;
        case dtSaHpiFumiOemDefinedSpecInfoTWithoutMid:
            ToTxt_SaHpiFumiOemDefinedSpecInfoTWithoutMid( src, txt );
            return;
        default:
            return;
    }
}

bool FromTxt( const std::string& txt, Var& var )
{
    void * dst = var.wdata;
    if ( !dst ) {
        return false;
    }

    switch ( var.type ) {
        case dtSaHpiUint8T:
            return FromTxt_SaHpiUint8T( txt, dst );
        case dtSaHpiUint16T:
            return FromTxt_SaHpiUint16T( txt, dst );
        case dtSaHpiUint32T:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiUint64T:
            return FromTxt_SaHpiUint64T( txt, dst );
        case dtSaHpiInt8T:
            return FromTxt_SaHpiInt8T( txt, dst );
        case dtSaHpiInt16T:
            return FromTxt_SaHpiInt16T( txt, dst );
        case dtSaHpiInt32T:
            return FromTxt_SaHpiInt32T( txt, dst );
        case dtSaHpiInt64T:
            return FromTxt_SaHpiInt64T( txt, dst );
        case dtSaHpiFloat64T:
            return FromTxt_SaHpiFloat64T( txt, dst );
        case dtSaHpiBoolT:
            return FromTxt_SaHpiBoolT( txt, dst );
        case dtSaHpiManufacturerIdT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiDomainIdT:
            return FromTxt_SaHpiDomainIdT( txt, dst );
        case dtSaHpiResourceIdT:
            return FromTxt_SaHpiResourceIdT( txt, dst );
        case dtSaHpiEntryIdT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiTimeT:
            return FromTxt_SaHpiTimeT( txt, dst );
        case dtSaHpiTimeoutT:
            return FromTxt_SaHpiTimeoutT( txt, dst );
        case dtSaHpiLanguageT:
            return FromTxt_Enum( SaHpiLanguageTElems, txt, dst );
        case dtSaHpiTextTypeT:
            return FromTxt_Enum( SaHpiTextTypeTElems, txt, dst );
        case dtSaHpiTextBufferT:
            return FromTxt_SaHpiTextBufferT( txt, dst );
        case dtSaHpiInstrumentIdT:
            return FromTxt_SaHpiInstrumentIdT( txt, dst );
        case dtSaHpiEntityPathT:
            return FromTxt_SaHpiEntityPathT( txt, dst );
        case dtSaHpiEventCategoryT:
            return FromTxt_EnumU8( SaHpiEventCategoryTElems, txt, dst );
        case dtSaHpiEventStateT:
            return FromTxt_Flags16( SaHpiEventStateTElems, txt, dst );
        case dtSaHpiEventStateTThreshold:
            return FromTxt_Flags16( SaHpiEventStateTThresholdElems, txt, dst );
        case dtSaHpiSensorNumT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiSensorTypeT:
            return FromTxt_Enum( SaHpiSensorTypeTElems, txt, dst );
        case dtSaHpiSensorReadingTypeT:
            return FromTxt_Enum( SaHpiSensorReadingTypeTElems, txt, dst );
        case dtSaHpiSensorRangeFlagsT:
            return FromTxt_Flags8( SaHpiSensorRangeFlagsTElems, txt, dst );
        case dtSaHpiSensorUnitsT:
            return FromTxt_Enum( SaHpiSensorUnitsTElems, txt, dst );
        case dtSaHpiSensorModUnitUseT:
            return FromTxt_Enum( SaHpiSensorModUnitUseTElems, txt, dst );
        case dtSaHpiSensorThdMaskT:
            return FromTxt_Flags8( SaHpiSensorThdMaskTElems, txt, dst );
        case dtSaHpiSensorEventCtrlT:
            return FromTxt_Enum( SaHpiSensorEventCtrlTElems, txt, dst );
        case dtSaHpiCtrlNumT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiCtrlTypeT:
            return FromTxt_Enum( SaHpiCtrlTypeTElems, txt, dst );
        case dtSaHpiCtrlStateDigitalT:
            return FromTxt_Enum( SaHpiCtrlStateDigitalTElems, txt, dst );
        case dtSaHpiCtrlStateDiscreteT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiCtrlStateAnalogT:
            return FromTxt_SaHpiInt32T( txt, dst );
        case dtSaHpiTxtLineNumT:
            return FromTxt_SaHpiUint8T( txt, dst );
        case dtSaHpiCtrlModeT:
            return FromTxt_Enum( SaHpiCtrlModeTElems, txt, dst );
        case dtSaHpiCtrlOutputTypeT:
            return FromTxt_Enum( SaHpiCtrlOutputTypeTElems, txt, dst );
        case dtSaHpiIdrIdT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiIdrAreaTypeT:
            return FromTxt_Enum( SaHpiIdrAreaTypeTElems, txt, dst );
        case dtSaHpiIdrFieldTypeT:
            return FromTxt_Enum( SaHpiIdrFieldTypeTElems, txt, dst );
        case dtSaHpiWatchdogNumT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiWatchdogActionT:
            return FromTxt_Enum( SaHpiWatchdogActionTElems, txt, dst );
        case dtSaHpiWatchdogActionEventT:
            return FromTxt_Enum( SaHpiWatchdogActionEventTElems, txt, dst );
        case dtSaHpiWatchdogPretimerInterruptT:
            return FromTxt_Enum( SaHpiWatchdogPretimerInterruptTElems, txt, dst );
        case dtSaHpiWatchdogTimerUseT:
            return FromTxt_Enum( SaHpiWatchdogTimerUseTElems, txt, dst );
        case dtSaHpiWatchdogExpFlagsT:
            return FromTxt_Flags8( SaHpiWatchdogExpFlagsTElems, txt, dst );
        case dtSaHpiDimiNumT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiDimiTestServiceImpactT:
            return FromTxt_Enum( SaHpiDimiTestServiceImpactTElems, txt, dst );
        case dtSaHpiDimiTestRunStatusT:
            return FromTxt_Enum( SaHpiDimiTestRunStatusTElems, txt, dst );
        case dtSaHpiDimiTestErrCodeT:
            return FromTxt_Enum( SaHpiDimiTestErrCodeTElems, txt, dst );
        case dtSaHpiDimiTestParamTypeT:
            return FromTxt_Enum( SaHpiDimiTestParamTypeTElems, txt, dst );
        case dtSaHpiDimiTestCapabilityT:
            return FromTxt_Flags32( SaHpiDimiTestCapabilityTElems, txt, dst );
        case dtSaHpiDimiTestNumT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiDimiTestPercentCompletedT:
            return FromTxt_SaHpiUint8T( txt, dst );
        case dtSaHpiDimiReadyT:
            return FromTxt_Enum( SaHpiDimiReadyTElems, txt, dst );
        case dtSaHpiFumiNumT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiBankNumT:
            return FromTxt_SaHpiUint8T( txt, dst );
        case dtSaHpiFumiSpecInfoTypeT:
            return FromTxt_Enum( SaHpiFumiSpecInfoTypeTElems, txt, dst );
        case dtSaHpiFumiSafDefinedSpecIdT:
            return FromTxt_Enum( SaHpiFumiSafDefinedSpecIdTElems, txt, dst );
        case dtSaHpiFumiServiceImpactT:
            return FromTxt_Enum( SaHpiFumiServiceImpactTElems, txt, dst );
        case dtSaHpiFumiSourceStatusT:
            return FromTxt_Enum( SaHpiFumiSourceStatusTElems, txt, dst );
        case dtSaHpiFumiBankStateT:
            return FromTxt_Enum( SaHpiFumiBankStateTElems, txt, dst );
        case dtSaHpiFumiUpgradeStatusT:
            return FromTxt_Enum( SaHpiFumiUpgradeStatusTElems, txt, dst );
        case dtSaHpiFumiLogicalBankStateFlagsT:
            return FromTxt_Flags32( SaHpiFumiLogicalBankStateFlagsTElems, txt, dst );
        case dtSaHpiFumiProtocolT:
            return FromTxt_Flags32( SaHpiFumiProtocolTElems, txt, dst );
        case dtSaHpiFumiCapabilityT:
            return FromTxt_Flags32( SaHpiFumiCapabilityTElems, txt, dst );
        case dtSaHpiHsIndicatorStateT:
            return FromTxt_Enum( SaHpiHsIndicatorStateTElems, txt, dst );
        case dtSaHpiHsStateT:
            return FromTxt_Enum( SaHpiHsStateTElems, txt, dst );
        case dtSaHpiHsCauseOfStateChangeT:
            return FromTxt_Enum( SaHpiHsCauseOfStateChangeTElems, txt, dst );
        case dtSaHpiSeverityT:
            return FromTxt_Enum( SaHpiSeverityTElems, txt, dst );
        case dtSaHpiResourceEventTypeT:
            return FromTxt_Enum( SaHpiResourceEventTypeTElems, txt, dst );
        case dtSaHpiDomainEventTypeT:
            return FromTxt_Enum( SaHpiDomainEventTypeTElems, txt, dst );
        case dtSaHpiSensorOptionalDataT:
            return FromTxt_Flags8( SaHpiSensorOptionalDataTElems, txt, dst );
        case dtSaHpiSensorEnableOptDataT:
            return FromTxt_Flags8( SaHpiSensorEnableOptDataTElems, txt, dst );
        case dtSaHpiSwEventTypeT:
            return FromTxt_Enum( SaHpiSwEventTypeTElems, txt, dst );
        case dtSaHpiEventTypeT:
            return FromTxt_Enum( SaHpiEventTypeTElems, txt, dst );
        case dtSaHpiAnnunciatorNumT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiNameT:
            return FromTxt_SaHpiNameT( txt, dst );
        case dtSaHpiStatusCondTypeT:
            return FromTxt_Enum( SaHpiStatusCondTypeTElems, txt, dst );
        case dtSaHpiAnnunciatorModeT:
            return FromTxt_Enum( SaHpiAnnunciatorModeTElems, txt, dst );
        case dtSaHpiAnnunciatorTypeT:
            return FromTxt_Enum( SaHpiAnnunciatorTypeTElems, txt, dst );
        case dtSaHpiRdrTypeT:
            return FromTxt_Enum( SaHpiRdrTypeTElems, txt, dst );
        case dtSaHpiParmActionT:
            return FromTxt_Enum( SaHpiParmActionTElems, txt, dst );
        case dtSaHpiResetActionT:
            return FromTxt_Enum( SaHpiResetActionTElems, txt, dst );
        case dtSaHpiPowerStateT:
            return FromTxt_Enum( SaHpiPowerStateTElems, txt, dst );
        case dtSaHpiLoadNumberT:
            return FromTxt_SaHpiLoadNumberT( txt, dst );
        case dtSaHpiGuidT:
            return FromTxt_SaHpiGuidT( txt, dst );
        case dtSaHpiCapabilitiesT:
            return FromTxt_Flags32( SaHpiCapabilitiesTElems, txt, dst );
        case dtSaHpiHsCapabilitiesT:
            return FromTxt_Flags32( SaHpiHsCapabilitiesTElems, txt, dst );
        case dtSaHpiEventLogOverflowActionT:
            return FromTxt_Enum( SaHpiEventLogOverflowActionTElems, txt, dst );
        case dtSaHpiEventLogCapabilitiesT:
            return FromTxt_Flags32( SaHpiEventLogCapabilitiesTElems, txt, dst );
        case dtSaHpiEventLogEntryIdT:
            return FromTxt_SaHpiUint32T( txt, dst );
        case dtSaHpiCtrlStateStreamTWithoutRepeat:
            return FromTxt_SaHpiCtrlStateStreamTWithoutRepeat( txt, dst );
        case dtSaHpiCtrlStateOemTWithoutMId:
            return FromTxt_SaHpiCtrlStateOemTWithoutMId( txt, dst );
        case dtControlOemConfigData:
            return FromTxt_ControlOemConfigData( txt, dst );
        case dtSensorReadingBuffer:
            return FromTxt_SensorReadingBuffer( txt, dst );
        case dtDimiTestParamName:
            return FromTxt_DimiTestParamName( txt, dst );
        case dtSaHpiFumiOemDefinedSpecInfoTWithoutMid:
            return FromTxt_SaHpiFumiOemDefinedSpecInfoTWithoutMid( txt, dst );
        default:
            return false;
    }
}


// Object Name Codecs
std::string AssembleNumberedObjectName( const std::string& classname,
                                        SaHpiUint32T num )
{
    std::string name( classname );
    name.push_back( '-' );
    ToTxt_SaHpiUint32T( &num, name );

    return name;
}

bool DisassembleNumberedObjectName( const std::string& name,
                                    std::string& classname,
                                    SaHpiUint32T& num )
{
    size_t n = name.find_first_of( '-' );
    if ( n == std::string::npos ) {
        return false;
    }
    std::string::const_iterator iter = name.begin() + n;

    classname.assign( name.begin(), iter );
    ++iter;
    std::string snum( iter, name.end() );

    return FromTxt_SaHpiUint32T( snum, &num );
}

std::string AssembleResourceObjectName( const SaHpiEntityPathT& ep )
{
    std::string name;
    ToTxt_SaHpiEntityPathT( &ep, name );

    return name;
}

bool DisassembleResourceObjectName( const std::string& name,
                                    SaHpiEntityPathT& ep )
{
    return FromTxt_SaHpiEntityPathT( name, &ep );
}


}; // namespace TA

