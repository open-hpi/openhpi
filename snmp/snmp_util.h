/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#ifndef SNMP_BC_UTIL_H
#define SNMP_BC_UTIL_H

/*****************************************/
/* #include <net-snmp/net-snmp-config.h> */
#define NET_SNMP_CONFIG_H

#define DEFAULT_SNMP_VERSION 3

#define SNMPV1      0xAAAA
#define SNMPV2ANY   0xA000
#define SNMPV2AUTH  0x8000

#define DEFAULT_MIBS "IP-MIB:IF-MIB:TCP-MIB:UDP-MIB:HOST-RESOURCES-MIB:SNMPv2-MIB:RFC1213-MIB:NOTIFICATION-LOG-MIB:UCD-SNMP-MIB:UCD-DEMO-MIB:SNMP-TARGET-MIB:NET-SNMP-AGENT-MIB:SNMP-VIEW-BASED-ACM-MIB:SNMP-COMMUNITY-MIB:UCD-DLMOD-MIB:SNMP-FRAMEWORK-MIB:SNMP-MPD-MIB:SNMP-USER-BASED-SM-MIB:SNMP-NOTIFICATION-MIB:SNMPv2-TM"

#define DEFAULT_MIBDIRS "$HOME/.snmp/mibs:/usr/local/share/snmp/mibs"

#define OPAQUE_SPECIAL_TYPES 1

#define UCD_SNMP_LIBRARY 1

#define SCAPI_AUTHPRIV 1

#define SNMP_ALWAYS_DEBUG 0


#define USE_REVERSE_ASNENCODING       1
#define DEFAULT_ASNENCODING_DIRECTION 1

#define PERSISTENT_DIRECTORY "/var/net-snmp"

#define PERSISTENT_MASK 077

#define AGENT_DIRECTORY_MODE 0700

#define MAX_PERSISTENT_BACKUPS 10

#define SYSTEM_INCLUDE_FILE "net-snmp/system/linux.h"

#define MACHINE_INCLUDE_FILE "net-snmp/machine/generic.h"

#define SNMPLIBPATH "/usr/local/lib/snmp"
#define SNMPSHAREPATH "/usr/local/share/snmp"
#define SNMPCONFPATH "/usr/local/etc/snmp"
#define SNMPDLMODPATH "/usr/local/lib/snmp/dlmod"

#define LOGFILE "/var/log/snmpd.log"

#define SYS_LOC "Unknown"

#define KERNEL_LOC "unknown"

#define ETC_MNTTAB "/etc/mtab"

#define PSCMD "/bin/ps -e"

#define UNAMEPROG "/bin/uname"

#define NETSNMP_TEMP_FILE_PATTERN "/tmp/snmpdXXXXXX"

#define NO_DUMMY_VALUES 1

#define HAVE_AES_CFB128_ENCRYPT 1
#define HAVE_ALLOCA 1
#define HAVE_ALLOCA_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_ASM_PAGE_H 1
#define HAVE_BCOPY 1
#define HAVE_DIRENT_H 1
#define HAVE_DLOPEN 1
#define HAVE_ERR_H 1
#define HAVE_EXECV 1
#define HAVE_FORK 1
#define HAVE_FSTAB_H 1
#define HAVE_GETADDRINFO 1
#define HAVE_GETDTABLESIZE 1
#define HAVE_GETGRNAM 1
#define HAVE_GETHOSTNAME 1
#define HAVE_GETLOADAVG 1
#define HAVE_GETMNTENT 1
#define HAVE_GETOPT_H 1
#define HAVE_GETPAGESIZE 1
#define HAVE_GETPID 1
#define HAVE_GETPWNAM 1
#define HAVE_GRP_H 1
#define HAVE_IF_FREENAMEINDEX 1
#define HAVE_IF_NAMEINDEX 1
#define HAVE_INDEX 1
#define HAVE_LIBCRYPTO 1
#define HAVE_LIBDL 1
#define HAVE_LIBELF 1
#define HAVE_LIBM 1
#define HAVE_LIMITS_H 1
#define HAVE_LINUX_HDREG_H 1
#define HAVE_LOCALE_H 1
#define HAVE_LRAND48 1
#define HAVE_MALLOC_H 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MKSTEMP 1
#define HAVE_MNTENT_H 1
#define HAVE_NETDB_H 1
#define HAVE_NETINET_IF_ETHER_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_NETINET_IN_SYSTM_H 1
#define HAVE_NETINET_IP6_H 1
#define HAVE_NETINET_IP_H 1
#define HAVE_NETINET_IP_ICMP_H 1
#define HAVE_NETINET_TCP_H 1
#define HAVE_NETINET_UDP_H 1
#define HAVE_NETIPX_IPX_H 1
#define HAVE_NET_IF_H 1
#define HAVE_NET_ROUTE_H 1
#define HAVE_NLIST 1
#define HAVE_NLIST_H 1
#define HAVE_OPENSSL_AES_H 1
#define HAVE_OPENSSL_DES_H 1
#define HAVE_OPENSSL_EVP_H 1
#define HAVE_OPENSSL_HMAC_H 1
#define HAVE_PTHREAD_H 1
#define HAVE_PWD_H 1
#define HAVE_RAND 1
#define HAVE_RANDOM 1
#define HAVE_REGCOMP 1
#define HAVE_REGEX_H 1
#define HAVE_SEARCH_H 1
#define HAVE_SELECT 1
#define HAVE_SETENV 1
#define HAVE_SETGID 1
#define HAVE_SETGROUPS 1
#define HAVE_SETITIMER 1
#define HAVE_SETLOCALE 1
#define HAVE_SETMNTENT 1
#define HAVE_SETSID 1
#define HAVE_SETUID 1
#define HAVE_SGTTY_H 1
#define HAVE_SIGACTION 1
#define HAVE_SIGBLOCK 1
#define HAVE_SIGHOLD 1
#define HAVE_SIGNAL 1
#define HAVE_SIGSET 1
#define HAVE_SNPRINTF 1
#define HAVE_SOCKET 1
#define HAVE_STATFS 1
#define HAVE_STATVFS 1
#define HAVE_STDARG_H 1
#define HAVE_STRCASESTR 1
#define HAVE_STRCHR 1
#define HAVE_STRDUP 1
#define HAVE_STRERROR 1
#define HAVE_STRNCASECMP 1
#define HAVE_STRTOL 1
#define HAVE_STRTOUL 1
#define HAVE_SYSLOG_H 1
#define HAVE_SYSTEM 1
#define HAVE_SYS_CDEFS_H 1
#define HAVE_SYS_FILE_H 1
#define HAVE_SYS_IOCTL_H 1
#define HAVE_SYS_MOUNT_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_QUEUE_H 1
#define HAVE_SYS_SELECT_H 1
#define HAVE_SYS_SOCKETVAR_H 1
#define HAVE_SYS_SOCKET_H 1
#define HAVE_SYS_STATFS_H 1
#define HAVE_SYS_STATVFS_H 1
#define HAVE_SYS_SWAP_H 1
#define HAVE_SYS_SYSCTL_H 1
#define HAVE_SYS_UN_H 1
#define HAVE_SYS_USER_H 1
#define HAVE_SYS_UTSNAME_H 1
#define HAVE_SYS_VFS_H 1
#define HAVE_SYS_WAIT_H 1
#define HAVE_TCGETATTR 1
#define HAVE_UNAME 1
#define HAVE_USLEEP 1
#define HAVE_UTMPX_H 1
#define HAVE_VSNPRINTF 1


#define RETSIGTYPE void

#define TIME_WITH_SYS_TIME 1

#define RTENTRY struct rtentry

#define STRUCT_SIGACTION_HAS_SA_SIGACTION 1

#define STRUCT_RTENTRY_HAS_RT_DST 1

#define STRUCT_IFNET_HAS_IF_SPEED 1
#define STRUCT_IFNET_HAS_IF_TYPE 1

#define STRUCT_IFNET_HAS_IF_IQDROPS 1

#define STRUCT_IFNET_HAS_IF_OBYTES 1
#define STRUCT_IFNET_HAS_IF_IBYTES 1

#define STRUCT_IFADDR_HAS_IFA_NEXT 1

#define STRUCT_NLIST_HAS_N_VALUE 1

#define STRUCT_STATVFS_HAS_F_FRSIZE 1

#define STRUCT_STATVFS_HAS_F_FILES 1

#define STRUCT_STATFS_HAS_F_FILES 1
#define STRUCT_STATFS_HAS_F_FFREE 1

#define SYS_IOCTL_H_HAS_SIOCGIFADDR 1

#define HAVE_CPP_UNDERBAR_FUNCTION_DEFINED 1

#define VERS_DESC   "unknown"
#define SYS_NAME    "unknown"

#define PROCMIBNUM 2

#define SHELLMIBNUM 8

#define MEMMIBNUM 4

#define DISKMIBNUM 9

#define LOADAVEMIBNUM 10

#define VERSIONMIBNUM 100

#define ERRORMIBNUM 101

#define AGENTID 250

#define HPUX9ID 1
#define SUNOS4ID 2
#define SOLARISID 3
#define OSFID 4
#define ULTRIXID 5
#define HPUX10ID 6
#define NETBSD1ID 7
#define FREEBSDID 8
#define IRIXID 9
#define LINUXID 10
#define BSDIID 11
#define OPENBSDID 12
#define WIN32ID 13
#define HPUX11ID 14
#define UNKNOWNID 255

#ifdef hpux9
#define OSTYPE HPUX9ID
#endif
#ifdef hpux10
#define OSTYPE HPUX10ID
#endif
#ifdef hpux11
#define OSTYPE HPUX11ID
#endif
#ifdef sunos4
#define OSTYPE SUNOS4ID
#endif
#ifdef solaris2
#define OSTYPE SOLARISID
#endif
#if defined(osf3) || defined(osf4) || defined(osf5)
#define OSTYPE OSFID
#endif
#ifdef ultrix4
#define OSTYPE ULTRIXID
#endif
#ifdef netbsd1
#define OSTYPE NETBSD1ID
#endif
#ifdef freebsd2
#define OSTYPE FREEBSDID
#endif
#if defined(irix6) || defined(irix5)
#define OSTYPE IRIXID
#endif
#ifdef linux
#define OSTYPE LINUXID
#endif
#if defined(bsdi2) || defined(bsdi3) || defined(bsdi4)
#define OSTYPE BSDIID
#endif
#ifdef openbsd2
#define OSTYPE OPENBSDID
#endif

#ifndef OSTYPE
#define OSTYPE UNKNOWNID
#endif

#define ENTERPRISE_OID			8072
#define ENTERPRISE_MIB			1,3,6,1,4,1,8072
#define ENTERPRISE_DOT_MIB		1.3.6.1.4.1.8072
#define ENTERPRISE_DOT_MIB_LENGTH	7

#define SYSTEM_MIB		1,3,6,1,4,1,8072,3,2,OSTYPE
#define SYSTEM_DOT_MIB		1.3.6.1.4.1.8072.3.2.OSTYPE
#define SYSTEM_DOT_MIB_LENGTH	10

#define NOTIFICATION_MIB		1,3,6,1,4,1,8072,4
#define NOTIFICATION_DOT_MIB		1.3.6.1.4.1.8072.4
#define NOTIFICATION_DOT_MIB_LENGTH	8

#define UCDAVIS_OID		2021
#define UCDAVIS_MIB		1,3,6,1,4,1,2021
#define UCDAVIS_DOT_MIB		1.3.6.1.4.1.2021
#define UCDAVIS_DOT_MIB_LENGTH	7

#define ERRORTIMELENGTH 600

#define EXCACHETIME 30
#define CACHEFILE ".snmp-exec-cache"
#define MAXCACHESIZE (200*80)

#define MAXDISKS 50

#define DEFDISKMINIMUMSPACE 100000

#define DEFMAXLOADAVE 12.0

#define MAXREADCOUNT 100

#define SNMPBLOCK 1

#define RESTARTSLEEP 5

#define NUM_COMMUNITIES	5

#define LASTFIELD -1

#define CONFIGURE_OPTIONS "\"\""

#define HAVE_SOCKLEN_T 1
#define HAVE_IN_ADDR_T 1

#ifndef HAVE_STRCHR
#ifdef HAVE_INDEX
# define strchr index
# define strrchr rindex
#endif
#endif

#ifndef HAVE_INDEX
#ifdef HAVE_STRCHR
# define index strchr
# define rindex strrchr
#endif
#endif

#ifndef HAVE_MEMCPY
#ifdef HAVE_BCOPY
# define memcpy(d, s, n) bcopy ((s), (d), (n))
# define memmove(d, s, n) bcopy ((s), (d), (n))
# define memcmp bcmp
#endif
#endif

#ifndef HAVE_MEMMOVE
#ifdef HAVE_MEMCPY
# define memmove memcpy
#endif
#endif

#ifndef HAVE_BCOPY
#ifdef HAVE_MEMCPY
# define bcopy(s, d, n) memcpy ((d), (s), (n))
# define bzero(p,n) memset((p),(0),(n))
# define bcmp memcmp
#endif
#endif

#define USE_OPENSSL 1
#if defined(USE_OPENSSL) && defined(HAVE_OPENSSL_AES_H) && defined(HAVE_AES_CFB128_ENCRYPT)
#define HAVE_AES 1
#endif

#ifndef HAVE_RANDOM
#ifdef HAVE_LRAND48
#define random lrand48
#define srandom(s) srand48(s)
#else
#ifdef HAVE_RAND
#define random rand
#define srandom(s) srand(s)
#endif
#endif
#endif

#ifndef HAVE_SIGNAL
#ifdef HAVE_SIGSET
#define signal(a,b) sigset(a,b)
#endif
#endif

#define HAVE_GETHOSTBYNAME 1
#define HAVE_GETSERVBYNAME 1
#define HAVE_LPSTAT 1
#define LPSTAT_PATH "/usr//bin/lpstat"
#define HAVE_PRINTCAP 1

#define SNMP_TRANSPORT_UDP_DOMAIN 1

#define SNMP_TRANSPORT_CALLBACK_DOMAIN 1

#define SNMP_TRANSPORT_TCP_DOMAIN 1

#define SNMP_TRANSPORT_UNIX_DOMAIN 1

#define SNMP_SECMOD_USM 1

#define config_require(x)
#define config_arch_require(x,y)
#define config_parse_dot_conf(w,x,y,z)
#define config_add_mib(x)

#ifdef WIN32
#define ENV_SEPARATOR ";"
#define ENV_SEPARATOR_CHAR ';'
#else
#define ENV_SEPARATOR ":"
#define ENV_SEPARATOR_CHAR ':'
#endif

#define NETSNMP_INLINE inline
#define NETSNMP_STATIC_INLINE static inline
#define NETSNMP_ENABLE_INLINE 1

#include SYSTEM_INCLUDE_FILE
#include MACHINE_INCLUDE_FILE

#if NETSNMP_ENABLE_INLINE && !defined(NETSNMP_NO_INLINE)
#   define NETSNMP_USE_INLINE
#else

#   define NETSNMP_INLINE

#   define NETSNMP_STATIC_INLINE static
#endif

#if defined(HAVE_NLIST) && defined(STRUCT_NLIST_HAS_N_VALUE) && !defined(DONT_USE_NLIST) && !defined(NO_KMEM_USAGE)
#define CAN_USE_NLIST
#endif

#if HAVE_DMALLOC_H
#define DMALLOC_FUNC_CHECK
#endif

/* end of #include <net-snmp/net-snmp-config.h> substitution */
/*************************************************************/

#include <SaHpi.h>

#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/transform_oids.h>

#define MAX_ASN_STR_LEN 300

#define CHECK_END(a) ((a != SNMP_ENDOFMIBVIEW) &&  \
                  (a != SNMP_NOSUCHOBJECT) &&  \
                  (a != SNMP_NOSUCHINSTANCE))? 1:0 

/* Place-holder for values set and returned by snmp
 */
struct snmp_value {
        u_char type;
        char string[MAX_ASN_STR_LEN];
        unsigned int str_len;
        long integer;
};

int snmp_get(
        struct snmp_session *ss,
        const char *objid,
        struct snmp_value *value);

int snmp_set(
        struct snmp_session *ss,
        char *objid,
        struct snmp_value value);

SaErrorT snmp_get2(struct snmp_session *ss, 
		   oid *objid, 
		   size_t objid_len,
		   struct snmp_value *value);

SaErrorT snmp_set2(struct snmp_session *ss, 
	           oid *objid,
	           size_t objid_len,
                   struct snmp_value *value);

int snmp_getn_bulk( struct snmp_session *ss, 
		    oid *bulk_objid, 
		    size_t bulk_objid_len,
		    struct snmp_pdu *bulk_pdu, 
		    struct snmp_pdu **bulk_response,
		    int num_repetitions );

void sc_free_pdu(struct snmp_pdu **p);

#endif
