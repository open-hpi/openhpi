/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <glib.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_error.h>
#include <snmp_util.h>

/***********************************************************
 * NOTE!
 *
 * This file needs to be a separate source/object file.
 * The SNMP simulator needs the functions in this file but
 * cannot include this directory's SNMP library
 **********************************************************/

/**********************************************************************
 * snmp_derive_objid:
 * @ep - Enitity's HPI SaHpiEntityPathT.
 * @oid - Un-normalized SNMP OID command character string.
 *
 * This function "normalizes" OID command strings based on entity path.
 * Starting from the end of @oid, this routine replaces the letter 'x',
 * with the last instance number of entity path, the process is repeated
 * until all 'x' are replaced by an instance number. For example,
 * 
 * @oid = ".1.3.6.1.4.1.2.3.x.2.22.1.5.1.1.5.x"
 * @ep = {SAHPI_ENT_CHASSIS, 51}{SAHPI_ENT_SBC_BLADE, 3}
 *
 * returns a normalized string of ".1.3.6.1.4.1.2.3.51.2.22.1.5.1.1.5.3".
 *
 * If @oid does not contain any 'x' characters, this routine still 
 * allocates memory and returns a "normalized" string. In this case,
 * the normalized string is identical to @oid.
 *
 * Note!
 * Caller of this routine MUST g_free() the returned normalized string
 * when finished with it.
 *
 * Returns:
 * Pointer to normalized OID string - normal case.
 * NULL - on error.
 **********************************************************************/

/* FIXME:: this should take a pointer to an ep and not the structure itself */

gchar * snmp_derive_objid(SaHpiEntityPathT ep, const gchar *oid)
{
        gchar *new_oid = NULL, *oid_walker = NULL;
        gchar **fragments = NULL, **oid_nodes = NULL;
        guint num_epe, num_blanks, oid_strlen = 0;
        guint total_num_digits, i, work_instance_num, num_digits;

        for (num_epe = 0;
             ep.Entry[num_epe].EntityType != SAHPI_ENT_ROOT && num_epe < SAHPI_MAX_ENTITY_PATH;
             num_epe++);
        /* trace("Number of elements in entity path: %d", num_epe); */

        if (num_epe == 0) {
                error("Entity Path is null.");
                return NULL;
        }
        if ((oid_strlen = strlen(oid)) == 0) return NULL; /* Oid is zero length. */
        if (!strrchr(oid, OID_BLANK_CHAR)) return g_strdup(oid); /* Nothing to replace. */

        for (num_blanks = 0, i = 0; i < oid_strlen; i++) {
                if (oid[i] == OID_BLANK_CHAR) num_blanks++;
        }
        /* trace("Number of blanks in oid: %d, %s", num_blanks, oid); */
        if (num_blanks > num_epe) {
                error("Number of replacments=%d > entity path elements=%d\n", num_blanks, num_epe);
                return NULL;
        }

        fragments = g_strsplit(oid, OID_BLANK_STR, -1);
        if (!fragments) {error("Could not split OID"); goto CLEANUP;}
        oid_nodes = g_malloc0((num_blanks+1)*sizeof(gchar **));
        if (!oid_nodes) {error("Out of memory."); goto CLEANUP;}
        total_num_digits = 0;
        for (i = 0; i < num_blanks; i++) {
                work_instance_num = ep.Entry[num_blanks-1-i].EntityLocation;
                for (num_digits = 1;
                     (work_instance_num = work_instance_num/10) > 0; num_digits++);
                oid_nodes[i] = g_malloc0((num_digits+1)*sizeof(gchar));
                if (!oid_nodes[i]) {error("Out of memory."); goto CLEANUP;}
                snprintf(oid_nodes[i], (num_digits+1)*sizeof(gchar), "%d", 
			 ep.Entry[num_blanks-1-i].EntityLocation);
                /* trace("Instance number: %s", oid_nodes[i]); */
                total_num_digits = total_num_digits + num_digits;
        }

        new_oid = g_malloc0((oid_strlen-num_blanks+total_num_digits+1)*sizeof(gchar));
        if (!new_oid) {error("Out of memory."); goto CLEANUP;}
        oid_walker = new_oid;
        for (i = 0; fragments[i]; i++) {
                oid_walker = strcpy(oid_walker, fragments[i]);
                oid_walker = oid_walker + strlen(fragments[i]);
                if (oid_nodes[i]) {
                        oid_walker = strcpy(oid_walker, oid_nodes[i]);
                        /* trace("Instance number: %s", oid_nodes[i]); */
                        oid_walker = oid_walker + strlen(oid_nodes[i]);
                }
                /* trace("New oid: %s", new_oid); */
        }

CLEANUP:
        g_strfreev(fragments);
        g_strfreev(oid_nodes);

        return new_oid;
}
