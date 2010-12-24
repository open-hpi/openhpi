/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renier@openhpi.org>
 *
 */

#ifndef __OH_ERROR_H
#define __OH_ERROR_H

#include <config.h>

#ifdef OH_DBG_MSGS

#include <glib.h>

#define CRIT( fmt, ... ) \
    g_critical( "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )

#define WARN( fmt, ... ) \
    g_warning( "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )

#define MSG( fmt, ... ) \
    g_message( "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )

#define INFO( fmt, ... ) \
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_INFO, \
           "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )

#define DBG( fmt, ... ) \
    g_debug( "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )


/******************************************************************
 * Use CRIT, WARN, DBG macros intead of legacy err, warn, dbg
 ******************************************************************/
#define err( fmt, ... ) \
    g_critical( "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )

#define warn( fmt, ... ) \
    g_warning( "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )

#define dbg( fmt, ... ) \
    g_debug( "%s:%d: " fmt, __FILE__, __LINE__,## __VA_ARGS__ )

#else /* OH_DBG_MSGS */

#define CRIT( fmt, ... )
#define WARN( fmt, ... )
#define MSG( fmt, ... )
#define INFO( fmt, ... )
#define DBG( fmt, ... )

#define err( fmt, ... )
#define warn( fmt, ... )
#define dbg( fmt, ... )

#endif /* OH_DBG_MSGS */

#endif /* __OH_ERROR_H */

