/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004, 2006
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      David Judkovics <djudkovi@us.ibm.com>
 *      Renier Morales <renier@openhpi.org>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <glib.h>
#include <config.h>
#include <oh_utils.h>
#include <oh_error.h>
#include <sahpi_wrappers.h>

#ifdef OH_DBG_MSGS
#define dbg_uid_lock(format, ...) \
        do { \
                if (getenv("OPENHPI_DBG_UID_LOCK") && !strcmp("YES",getenv("OPENHPI_DBG_UID_LOCK"))){ \
                        fprintf(stderr, "        UID_LOCK: %s:%d:%s: ", __FILE__, __LINE__, __func__); \
                        fprintf(stderr, format "\n", ## __VA_ARGS__); \
                } \
        } while(0)
#else
#define dbg_uid_lock(format, ...)
#endif

#define uid_lock(uidmutex) \
        do { \
                dbg_uid_lock("Locking UID mutex..."); \
                wrap_g_static_mutex_lock(uidmutex); \
                dbg_uid_lock("OK. UID mutex locked."); \
        } while (0)

#define uid_unlock(uidmutex) \
        do { \
                dbg_uid_lock("Unlocking UID mutex..."); \
                wrap_g_static_mutex_unlock(uidmutex); \
                dbg_uid_lock("OK. UID mutex unlocked."); \
        } while (0)


/* uid to entity path cross reference (xref) data structure */
typedef struct {
        SaHpiResourceIdT resource_id;
        SaHpiEntityPathT entity_path;
} EP_XREF;

#if GLIB_CHECK_VERSION (2, 32, 0)
	static GMutex oh_uid_lock;
#else
	static GStaticMutex oh_uid_lock = G_STATIC_MUTEX_INIT;
#endif

static GHashTable *oh_ep_table;
static GHashTable *oh_resource_id_table;
static guint       resource_id;
static char * oh_uid_map_file = 0;
static int initialized = FALSE;


/* use to build memory resident map table from file */
static int uid_map_from_file(void);
static int build_uid_map_data(FILE *fp);

/* used by oh_uid_remove() */
static void write_ep_xref(gpointer key, gpointer value, gpointer file);

/* for hash table usage */
guint oh_entity_path_hash(gconstpointer key);
gboolean oh_entity_path_equal(gconstpointer a, gconstpointer b);

/*
 * oh_entity_path_hash: used by g_hash_table_new()
 * in oh_uid_initialize(). See glib library for
 * further details.
 */
guint oh_entity_path_hash(gconstpointer key)
{
        const char *p = key;
        guint h = *p;

        int i;

        int entity_path_len;

        entity_path_len = sizeof(SaHpiEntityPathT);

        p += 1;

        for( i=0; i<entity_path_len - 1; i++ ){
/*              h = (h << 5) - h + *p; */
                h = (h * 131) + *p;
                p++;
        }

        /* don't change the 1009, its magic */
        return( h % 1009 );

}

/*
 * oh_entity_path_equal: used by g_hash_table_new()
 * in oh_uid_initialize(). See glib library for
 * further details.
 */
gboolean oh_entity_path_equal(gconstpointer a, gconstpointer b)
{
        if (oh_cmp_ep(a,b)) {
                return 1;
        }
        else {
                return 0;
        }
}

/**
 * oh_uid_initialize
 *
 * UID utils initialization routine
 * This functions must be called before any other uid_utils
 * are made.
 *
 * Returns: success 0, failure -1.
 **/
SaErrorT oh_uid_initialize(void)
{
        uid_lock(&oh_uid_lock);
        if (!initialized) {
                int cc;
                const char * uid_map_file = 0;

                /* initialize hash tables */
                oh_ep_table = g_hash_table_new(oh_entity_path_hash, oh_entity_path_equal);
                oh_resource_id_table = g_hash_table_new(g_int_hash, g_int_equal);
                initialized = TRUE;
                resource_id = 1;

                /* determine if we can use map file */
                uid_map_file = (char *)getenv("OPENHPI_UID_MAP");
                if (uid_map_file == 0) {
                        uid_map_file = OH_DEFAULT_UID_MAP;
                }
                if ((uid_map_file == 0) || (strlen(uid_map_file) == 0)) {
                        WARN( "UID Map file will not be used." );
                        WARN( "Resource Id will not be persistent." );
                } else {
                        oh_uid_map_file = g_strdup(uid_map_file);
                        INFO( "Using UID Map file %s.", oh_uid_map_file );
                }

                /* initialize uid map */
                cc = uid_map_from_file();
                if (cc != 0) {
                        g_free(oh_uid_map_file);
                        oh_uid_map_file = 0;
                        WARN( "Disabling using UID Map file." );
                        WARN( "Resource Id will not be persistent." );
                }
        }
        uid_unlock(&oh_uid_lock);

        return 0;
}

SaHpiBoolT oh_uid_is_initialized()
{
        if (initialized)
                return SAHPI_TRUE;
        else
                return SAHPI_FALSE;
}

/**
 * oh_uid_from_entity_path
 * @ep: value to be removed from used
 *
 * This function returns an unique value to be used as
 * an uid/resourceID base upon a unique entity path specified
 * by @ep.  If the entity path already exists, the already assigned
 * resource id is returned.  Before returning, this call updates the
 * uid map file saved on disk.
 *
 * Returns: positive unsigned int, failure is 0.
 **/
SaHpiUint32T oh_uid_from_entity_path(SaHpiEntityPathT *ep)
{
        gpointer key;
        gpointer value;
        SaHpiResourceIdT ruid;

        EP_XREF *ep_xref;

        SaHpiEntityPathT entitypath;

        if (!ep) return 0;
        if (!oh_uid_is_initialized()) return 0;

        oh_init_ep(&entitypath);
        oh_concat_ep(&entitypath,ep);
        key = &entitypath;

        uid_lock(&oh_uid_lock);
        /* check for presence of EP and */
        /* previously assigned uid      */
        ep_xref = (EP_XREF *)g_hash_table_lookup (oh_ep_table, key);
        if (ep_xref) {
                /*DBG("Entity Path already assigned uid. Use oh_uid_lookup().");*/
                uid_unlock(&oh_uid_lock);
                return ep_xref->resource_id;
        }

        /* allocate storage for EP cross reference data structure*/
        ep_xref = g_new0(EP_XREF, 1);
        if(!ep_xref) {
                CRIT("malloc failed");
                uid_unlock(&oh_uid_lock);
                return 0;
        }

        memset(ep_xref, 0, sizeof(EP_XREF));
        memcpy(&ep_xref->entity_path, &entitypath, sizeof(SaHpiEntityPathT));

        ep_xref->resource_id = resource_id;
        resource_id++;
        ruid = ep_xref->resource_id;

        value = (gpointer)ep_xref;

        /* entity path based key */
        key = (gpointer)&ep_xref->entity_path;
        g_hash_table_insert(oh_ep_table, key, value);

        /* resource id based key */
        key = (gpointer)&ep_xref->resource_id;
        g_hash_table_insert(oh_resource_id_table, key, value);

        /* save newly created ep xref (iud/resource_id) to map file */
        if (oh_uid_map_file) {
                FILE * fp;
                fp = fopen(oh_uid_map_file, "r+b");
                if (fp) {
                        fseek(fp, 0, SEEK_END);
                        if (fwrite(ep_xref, sizeof(EP_XREF), 1, fp) == 1) {
                                fseek(fp, 0, SEEK_SET);
                                if (fwrite(&resource_id, sizeof(resource_id), 1, fp) != 1) {
                			CRIT("write resource_id failed");
                                        ruid = 0;
                		}
        		} else {
        			CRIT("write ep_xref failed");
                                ruid = 0;
                        }
                        fclose(fp);
                }
        }

        uid_unlock(&oh_uid_lock);

        return ruid;
}

/**
 * oh_uid_remove
 * @uid: value to be removed
 *
 * This functions removes the uid/entity path
 * pair from use and removes the use of the uid forever.
 * A new uid may be requested for this entity path
 * in the future. oh_uid_from_entity_path() writes
 * the entire uid/entity path pairings to file before
 * returning. oh_uid_remove() deletes the pairing from file.
 *
 * Returns: success 0, failure -1.
 **/
SaErrorT oh_uid_remove(SaHpiUint32T uid)
{
        EP_XREF *ep_xref;
        gpointer key;

        if (!oh_uid_is_initialized()) return SA_ERR_HPI_ERROR;

        /* check entry exist in oh_resource_id_table */
        key = (gpointer)&uid;
        uid_lock(&oh_uid_lock);
        ep_xref = (EP_XREF *)g_hash_table_lookup (oh_resource_id_table, key);
        if(!ep_xref) {
                uid_unlock(&oh_uid_lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* check netry exist in oh_resource_id_table */
        key = (gpointer)&ep_xref->entity_path;
        ep_xref = (EP_XREF *)g_hash_table_lookup (oh_ep_table, key);
        if(!ep_xref) {
                uid_unlock(&oh_uid_lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        g_hash_table_remove(oh_resource_id_table, &ep_xref->resource_id);
        g_hash_table_remove(oh_ep_table, &ep_xref->entity_path);

        g_free(ep_xref);

        uid_unlock(&oh_uid_lock);

        return oh_uid_map_to_file();
}

/**
 * oh_uid_lookup
 * @ep: pointer to entity path used to identify resourceID/uid
 *
 * Fetches resourceID/uid based on entity path in @ep.
 *
 * Returns: success returns resourceID/uid, failure is 0.
 **/
SaHpiUint32T oh_uid_lookup(SaHpiEntityPathT *ep)
{
        EP_XREF *ep_xref;
        SaHpiEntityPathT entitypath;
        SaHpiResourceIdT ruid;
        gpointer key;

        if (!ep) return 0;
        if (!oh_uid_is_initialized()) return 0;

        oh_init_ep(&entitypath);
        oh_concat_ep(&entitypath, ep);
        key = &entitypath;

        /* check hash table for entry in oh_ep_table */
        uid_lock(&oh_uid_lock);
        ep_xref = (EP_XREF *)g_hash_table_lookup (oh_ep_table, key);
        if(!ep_xref) {
                uid_unlock(&oh_uid_lock);
                return 0;
        }

        ruid = ep_xref->resource_id;
        uid_unlock(&oh_uid_lock);

        return ruid;
}

/**
 * oh_entity_path_lookup
 * @id: resource_id/uid identifying entity path
 * @ep: pointer to memory to fill in with entity path
 *
 * Fetches entity path based upon resource id, @id.
 *
 * Returns: success 0, failed -1.
 **/
SaErrorT oh_entity_path_lookup(SaHpiUint32T id, SaHpiEntityPathT *ep)
{
        EP_XREF *ep_xref;
        gpointer key = &id;

        if (!id || !ep) return SA_ERR_HPI_ERROR;
        if (!oh_uid_is_initialized()) return SA_ERR_HPI_ERROR;

        /* check hash table for entry in oh_ep_table */
        uid_lock(&oh_uid_lock);
        ep_xref = (EP_XREF *)g_hash_table_lookup (oh_resource_id_table, key);
        if(!ep_xref) {
                uid_unlock(&oh_uid_lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(ep, &ep_xref->entity_path, sizeof(SaHpiEntityPathT));

        uid_unlock(&oh_uid_lock);

        return SA_OK;
}

/**
 * oh_uid_map_to_file: saves current uid and entity path mappings
 * to file, first element in file is 4 bytes for resource id,
 * then repeat EP_XREF structures holding uid and entity path pairings
 *
 * Return value: success 0, failed -1.
 **/
SaErrorT oh_uid_map_to_file(void)
{
        FILE *fp;

        if (!oh_uid_map_file) {
                return SA_OK;
        }

        uid_lock(&oh_uid_lock);

        fp = fopen(oh_uid_map_file, "wb");
        if(!fp) {
                CRIT("Configuration file '%s' could not be opened", oh_uid_map_file);
                uid_unlock(&oh_uid_lock);
                return SA_ERR_HPI_ERROR;
        }

        /* write resource id */
        if (fwrite((void *)&resource_id, sizeof(resource_id), 1, fp) != 1) {
		CRIT("write resource_id failed");
		fclose(fp);
                uid_unlock(&oh_uid_lock);
		return SA_ERR_HPI_ERROR;
	}

        /* write all EP_XREF data records */
        g_hash_table_foreach(oh_resource_id_table, write_ep_xref, fp);

        fclose(fp);

        uid_unlock(&oh_uid_lock);

        return SA_OK;
}


/*
 * write_ep_xref: called by g_hash_table_foreach(), for each
 * hash table entry see glib manual for further details
 *
 * Return value: None (void).
 */
static void write_ep_xref(gpointer key, gpointer value, gpointer fp)
{
        if (fwrite(value, sizeof(EP_XREF), 1, (FILE *)fp) != 1) {
		CRIT("write EP_XREF failed");
	}
}


/*
 * uid_map_from_file: called from oh_uid_initialize() during intialization
 * This function, if a uid map file exists, reads the current value for
 * uid and intializes the memory resident uid map file from file.
 *
 * Return value: success 0, error -1.
 */
static gint uid_map_from_file()
{
        FILE *fp;
        int rval;
#ifndef _WIN32
	mode_t prev_umask;
#endif

        if (!oh_uid_map_file) {
                return 0;
        }
        fp = fopen(oh_uid_map_file, "rb");
        if(!fp) {
                 /* create map file with resource id initial value */
                 WARN("uid_map file '%s' could not be opened, initializing", oh_uid_map_file);
#ifndef _WIN32
		 prev_umask = umask(022);
#endif
                 fp = fopen(oh_uid_map_file, "wb");
                 if(!fp) {
                         CRIT("Could not initialize uid map file, %s", oh_uid_map_file );
#ifndef _WIN32
                         if (geteuid() != 0) 
                              INFO("Use OPENHPI_UID_MAP env var to set uid_map file path");
			 umask (prev_umask);
#endif
			 return -1;
                 }
#ifndef _WIN32
		 umask (prev_umask);
#endif
                 /* write initial uid value */
                 if(fwrite(&resource_id, sizeof(resource_id), 1, fp) != 1 ) {
                         CRIT("failed to write uid, on uid map file initialization");
                         fclose(fp);
                         return -1;
                 }
                 if(fclose(fp) != 0) {
                         CRIT("Couldn't close file '%s'.during uid map file initialization", oh_uid_map_file);
                         return -1;
                 }
                 /* return from successful initialization, from newly created uid map file */
                 return 0;
         }

         /* read uid/resouce_id highest count from uid map file */
         if (fread(&resource_id, sizeof(resource_id), 1, fp) != 1) {
                 CRIT("error setting uid from existing uid map file");
                fclose(fp);
                 return -1;
         }

         rval = build_uid_map_data(fp);
         fclose(fp);

         if (rval < 0)
                return -1;

         /* return from successful initialization from existing uid map file */
         return 0;
}

/*
 * build_uid_map_data: used by uid_map_from_file(),  recursively
 * reads map file and builds two hash tables and EP_XREF data
 * structures
 *
 * @file: key into a GHashTable
 *
 * Return value: success 0, error -1.
 */
static gint build_uid_map_data(FILE *fp)
{
        EP_XREF *ep_xref;
        EP_XREF ep_xref1;
        gpointer value;
        gpointer key;

        while (fread(&ep_xref1, sizeof(EP_XREF), 1, fp) == 1) {

                /* copy read record from ep_xref1 to malloc'd ep_xref */
                ep_xref = g_new0(EP_XREF, 1);
                if (!ep_xref)
                        return -1;
                memcpy(ep_xref, &ep_xref1, sizeof(EP_XREF));

                value = (gpointer)ep_xref;

                /* entity path based key */
                key = (gpointer)&ep_xref->entity_path;
                g_hash_table_insert(oh_ep_table, key, value);

                /* resource id based key */
                key = (gpointer)&ep_xref->resource_id;
                g_hash_table_insert(oh_resource_id_table, key, value);
        }

        if ((feof(fp) == 0) || (ferror(fp) != 0)) {
                CRIT("error building ep xref from map file");
                return -1;
        }
        return 0;
}
