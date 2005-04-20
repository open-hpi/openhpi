/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 *      Renier Morales <renierm@users.sf.net>
 *      Thomas Kanngieser <thomas.kanngieser@fci.com>
 *      Chris Chia <cchia@users.sf.net.com>
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oh_utils.h>
#include <oh_error.h>

/**
 * oh_encode_entitypath:
 * @epstr: Pointer to canonical entity path string.
 * @ep: Location to store HPI's entity path structure.
 *
 * Converts an entity path canonical string (generally generated
 * by oh_decode_entitypath()) into an SaHpiEntityPathT structure.
 * 
 * Returns: 
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Input pointer(s) NULL.
 * SA_ERR_HPI_INVALID_DATA - Invalid canonical entity path string.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory for internal storage.
 **/
SaErrorT oh_encode_entitypath(const gchar *epstr, SaHpiEntityPathT *ep)
{
 	gchar **epathdefs = NULL, **epathvalues = NULL;
	gchar *gstr = NULL, *endptr = NULL;
        GSList  *epath_list = NULL, *lst = NULL;
        int   i, location, num_entities = 0;
        SaErrorT  err = SA_OK;
	SaHpiEntityT  *entityptr = NULL;
	SaHpiTextBufferT tmpbuffer;
	SaHpiEntityTypeT eptype;

	if (!epstr || epstr[0] == '\0' || !ep) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check for runaway string */
	if (strlen(epstr) >  OH_MAX_TEXT_BUFFER_LENGTH) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_DATA);
	}

        /* Split out {xxx,yyy} definition pairs */
       	gstr = g_strstrip(g_strdup(epstr));
	if (gstr == NULL || gstr[0] == '\0') {
		dbg("Stripped entity path string is NULL"); 
		err = SA_ERR_HPI_INVALID_DATA;
		goto CLEANUP;
	}

	epathdefs = g_strsplit(gstr, EPATHSTRING_END_DELIMITER, -1);
	if (epathdefs == NULL) {
		dbg("Cannot split entity path string.");
		err = SA_ERR_HPI_INTERNAL_ERROR;
		goto CLEANUP;
        }

	/* Split out HPI entity type and location strings */
	for (i=0; epathdefs[i] != NULL && epathdefs[i][0] != '\0'; i++) {

		epathdefs[i] = g_strstrip(epathdefs[i]);
		/* Check format - for starting delimiter and a comma */
		if ((epathdefs[i][0] != EPATHSTRING_START_DELIMITER_CHAR) || 
		    (strpbrk(epathdefs[i], EPATHSTRING_VALUE_DELIMITER) == NULL)) {
			dbg("Invalid entity path format.");
			err = SA_ERR_HPI_INVALID_DATA;
			goto CLEANUP;
		}

		epathvalues = g_strsplit(epathdefs[i],
                                         EPATHSTRING_VALUE_DELIMITER,
                                         ELEMENTS_IN_SaHpiEntityT);
		epathvalues[0] = g_strdelimit(epathvalues[0], EPATHSTRING_START_DELIMITER, ' ');

		/* Find entity type */
		oh_init_textbuffer(&tmpbuffer);
		oh_append_textbuffer(&tmpbuffer, g_strstrip(epathvalues[0]));
		err = oh_encode_entitytype(&tmpbuffer, &eptype);

                /* If not an HPI type - support a numeric type. Needed by IPMI Direct plugin */
		if (err) {
			err = SA_OK;
                        int num = strtol(g_strstrip(epathvalues[0]), &endptr, 0);
                        if (num <= 0 || endptr[0] != '\0') {
                                dbg("Invalid entity type string");
                                err = SA_ERR_HPI_INVALID_DATA;
                                goto CLEANUP;
                        }
			eptype = num;
		}

		/* Find entity location */
		location = strtol(g_strstrip(epathvalues[1]), &endptr, 10);
		if (endptr[0] != '\0') {
			dbg("Invalid location character");
			err = SA_ERR_HPI_INVALID_DATA;
			goto CLEANUP;
                }

		/* Save entity path definitions; reverse order */
		if (num_entities < SAHPI_MAX_ENTITY_PATH) {
			entityptr = (SaHpiEntityT *)g_malloc0(sizeof(*entityptr));
			if (entityptr == NULL) {
				dbg("No memory.");
				err = SA_ERR_HPI_OUT_OF_SPACE;
				goto CLEANUP;
			}

			entityptr->EntityType = eptype;
			entityptr->EntityLocation = location;
			epath_list = g_slist_prepend(epath_list, (gpointer)entityptr);
		}
		num_entities++;
	}
  
	/* Initialize and write HPI entity path structure */
	oh_init_ep(ep);
	for (i = 0; epath_list != NULL; i++) {
                lst = epath_list;
                if (i < SAHPI_MAX_ENTITY_PATH) {
                        ep->Entry[i].EntityType =
                                ((SaHpiEntityT *)(lst->data))->EntityType;
                        ep->Entry[i].EntityLocation =
                                ((SaHpiEntityT *)(lst->data))->EntityLocation;
                }
                epath_list = g_slist_remove_link(epath_list,lst);
                g_free(lst->data);
		g_slist_free(lst);
	}

	if (num_entities > SAHPI_MAX_ENTITY_PATH) {
		dbg("Too many entity defs");
		err = SA_ERR_HPI_INVALID_DATA;
	}

 CLEANUP:
	g_free(gstr);
	g_strfreev(epathdefs);
	g_strfreev(epathvalues);
	g_slist_free(epath_list);

	return(err);
}

/**
 * oh_decode_entitypath:
 * @ep: Pointer to HPI's entity path structure.
 * @bigbuf: Location to store canonical entity path string.
 *
 * Converts an entity path structure into its canonical string version. 
 * The canonical string is formed by removing the "SAHPI_ENT_" prefix
 * from the HPI types, and creating tuples for the entity types.
 * Order of significance is inverted to make entity paths look more
 * like Unix directory structure. It is also assumed that {ROOT,0}
 * exists implicitly before all of these entries. For example:
 *
 * {SYSTEM_CHASSIS,2}{PROCESSOR_BOARD,0}
 *
 * SAHPI_ENT_ROOT is used to identify end element of an entity path.
 * Fully populated entity path may not have an SAHPI_ENT_ROOT.
 * Duplicate names in SaHPIEntityTypeT enumeration aren't handled
 * and won't be preserved across conversion calls.
 *
 * Returns: 
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - Input pointer(s) NULL.
 * SA_ERR_HPI_INVALID_DATA - Location value too big for OpenHpi.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory for internal storage.
 **/
SaErrorT oh_decode_entitypath(const SaHpiEntityPathT *ep,
			      oh_big_textbuffer *bigbuf)
{
        char  *typestr;
 	gchar  *locstr, *catstr;
        gchar  typestr_buffer[20];
	int    i;
	oh_big_textbuffer tmpbuf;
	SaErrorT  err = SA_OK;

	if (!bigbuf || !ep) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = oh_init_bigtext(&tmpbuf);
	if (err) return(err);

#if 0
	/* Turn off strict HPI EP validation, for IPMI Direct
           numeric entity type support */
	if (!oh_valid_ep(ep)) {
		dbg("Invalid entity path");
		return(SA_ERR_HPI_INVALID_DATA);
	}
#endif

	locstr = (gchar *)g_malloc0(OH_MAX_LOCATION_DIGITS + 1);
	if (locstr == NULL) {
		dbg("No memory.");
		err = SA_ERR_HPI_OUT_OF_SPACE;
		goto CLEANUP;
	}
        
        /* Find last element of structure. Disregard ROOT element
         * and count as last in entity path. */
        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                if (ep->Entry[i].EntityType == SAHPI_ENT_ROOT) {
                            break;
                }
        }

        /* Parse entity path into a string */
	for (i--; i >= 0; i--) {
                guint num_digits, work_location_num;
                
		/* Validate and convert data */
                work_location_num = ep->Entry[i].EntityLocation;
                for (num_digits=1;
		     (work_location_num = work_location_num/10) > 0;
		     num_digits++);
		
		if (num_digits > OH_MAX_LOCATION_DIGITS) {
                        dbg("Location value too big");
                        err = SA_ERR_HPI_INVALID_DATA;
			goto CLEANUP;
		}
                memset(locstr, 0, OH_MAX_LOCATION_DIGITS + 1);
                snprintf(locstr, OH_MAX_LOCATION_DIGITS + 1, 
			 "%d", ep->Entry[i].EntityLocation);

                /* Find string for current entity type */
		typestr = oh_lookup_entitytype(ep->Entry[i].EntityType);

		/* Support numeric entity types - need by IPMI Direct plugin */
		if (typestr == NULL) {
			snprintf(typestr_buffer, 20, "%d", ep->Entry[i].EntityType);
			typestr = typestr_buffer;
		}

		catstr = g_strconcat(EPATHSTRING_START_DELIMITER,
				     typestr,
				     EPATHSTRING_VALUE_DELIMITER,
				     locstr,
				     EPATHSTRING_END_DELIMITER,
				     NULL);

		oh_append_bigtext(&tmpbuf, catstr);
		g_free(catstr);
	}

	/* Write string */
	oh_init_bigtext(bigbuf);
	oh_append_bigtext(bigbuf, (char *)tmpbuf.Data);

 CLEANUP:
	g_free(locstr);

	return(err);
}

/**
 * oh_init_ep:
 * @ep: Pointer to SaHpiEntityPathT structure to initialize.
 *
 * Initializes an entity path to all {ROOT,0} elements.
 *
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @ep is NULL.
 **/
SaErrorT oh_init_ep(SaHpiEntityPathT *ep)
{
	 int i;

         if (!ep) {
		 dbg("Invalid parameter.");
		 return(SA_ERR_HPI_INVALID_PARAMS);
	 }
         
         for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                 ep->Entry[i].EntityType = SAHPI_ENT_ROOT;
                 ep->Entry[i].EntityLocation = 0;
         }

	 return(SA_OK);
 }

/**
 * oh_concat_ep:
 * @dest: Pointer to entity path. Gets appended with @append.
 * @append: Pointer to entity path to append.
 *
 * Concatenate two entity path structures (SaHpiEntityPathT).
 * @append is appeded to @dest. If @dest doesn't have enough room, @append
 * will be truncated into @dest.
 *
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @dest is NULL.
 **/
SaErrorT oh_concat_ep(SaHpiEntityPathT *dest, const SaHpiEntityPathT *append)
{
        int i, j;

        if (!dest) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        if (!append) return(SA_OK);

        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                if (dest->Entry[i].EntityType == SAHPI_ENT_ROOT) break;
        }

        for (j=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                dest->Entry[i].EntityLocation = append->Entry[j].EntityLocation;
                dest->Entry[i].EntityType = append->Entry[j].EntityType;
                if (append->Entry[j].EntityType == SAHPI_ENT_ROOT) break;
                j++;
        }
	
        return(SA_OK);
}

/**
 * oh_valid_ep:
 * @ep: Pointer to an SaHpiEntityPathT structure.
 *
 * Check an entity path to make sure it does not contain
 * any invalid entity types. The entity path is checked up to
 * the first root element or to the end of the array, if there
 * are no root elements in the structure.
 *
 * Returns:
 * SAHPI_TRUE - Valid entity path.
 * SAHPI_FALSE - Invalid entity path.
 **/
SaHpiBoolT oh_valid_ep(const SaHpiEntityPathT *ep)
{
	int i;

        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
		if (ep->Entry[i].EntityType == SAHPI_ENT_ROOT) break;
/* FIXME:: Add explicit check for types with HPI resolves its conflicts with IPMI types */
/* Right now we're allowing users to specify any numeric type */
#if 0
		char *typestr;

		typestr = oh_lookup_entitytype(ep->Entry[i].EntityType);
		if (typestr == NULL) return(SAHPI_FALSE);
#endif
        }

        return(SAHPI_TRUE);
}

/**
 * oh_set_ep_location:
 * @ep: Pointer to entity path to work on
 * @et: entity type to look for
 * @ei: entity location to set when entity type is found
 *
 * Set an location number in the entity path given at the first
 * position (from least significant to most) the specified entity type is found.
 *
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @ep is NULL.
 * SA_ERR_HPI_INVALID_DATA - @ep invalid entity path.
 **/
SaErrorT oh_set_ep_location(SaHpiEntityPathT *ep, SaHpiEntityTypeT et, SaHpiEntityLocationT ei)
{
        int i;

	if (!ep) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (!oh_valid_ep(ep)) {
		dbg("Invalid entity path");
		return(SA_ERR_HPI_INVALID_DATA);
	}

        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                if (ep->Entry[i].EntityType == et) {
                        ep->Entry[i].EntityLocation = ei;
                        break;
                } else {
			if (ep->Entry[i].EntityType == SAHPI_ENT_ROOT) break;
                }
        }

        return(SA_OK);
}

/**
 * oh_cmp_ep:
 * @ep1: Pointer to entity path structure.
 * @ep2: Pointer to entity path structure.
 *
 * Compares two entity paths up to their root element.
 * To be equal, they must have the same number of elements and each element
 * (type and location pair) must be equal to the corresponding element
 * in the other entity path.
 *
 * Returns:
 * SAHPI_TRUE - if equal.
 * SAHPI_FALSE - if not equal.
 **/
SaHpiBoolT oh_cmp_ep(const SaHpiEntityPathT *ep1, const SaHpiEntityPathT *ep2)
{
        unsigned int i, j;
        
        if (!ep1 || !ep2) {
                dbg("Invalid parameter.");
                return(SAHPI_FALSE);
        }

        for (i=0; i <SAHPI_MAX_ENTITY_PATH; i++) {
                if (ep1->Entry[i].EntityType == SAHPI_ENT_ROOT) {
                        i++;
                        break;                                
                }
        }

        for (j=0; j<SAHPI_MAX_ENTITY_PATH; j++) {
                if (ep2->Entry[j].EntityType == SAHPI_ENT_ROOT) {
                        j++;
                        break;
                }
        }

        if (i != j) return(SAHPI_FALSE);

        for (i=0; i<j; i++) {
                if (ep1->Entry[i].EntityType != ep2->Entry[i].EntityType ||
                    ep1->Entry[i].EntityLocation != ep2->Entry[i].EntityLocation) {
                        /* dbg("Entity element %d: EP1 {%d,%d} != EP2 {%d,%d}", i, 
                            ep1->Entry[i].EntityType,
                            ep1->Entry[i].EntityLocation,
                            ep2->Entry[i].EntityType,
                            ep2->Entry[i].EntityLocation); */
                        return(SAHPI_FALSE);
                }
        }
        
        return(SAHPI_TRUE);
}

/**
 * oh_fprint_ep:
 * @ep: Pointer to entity path stucture.
 *
 * Prints the string form of an entity path structure.
 * The MACRO oh_print_ep(), uses this function to print to STDOUT.
 *
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @ep is NULL.
 **/
SaErrorT oh_fprint_ep(FILE *stream, const SaHpiEntityPathT *ep, int offsets)
{
	oh_big_textbuffer bigbuf1, bigbuf2;
        SaErrorT err;
	
        if (!ep) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
        }

	err = oh_init_bigtext(&bigbuf1);
	if (err) return(err);
	err = oh_init_bigtext(&bigbuf2);
	if (err) return(err);

	err = oh_append_offset(&bigbuf1, offsets);
	if (err) return(err);
	err = oh_append_bigtext(&bigbuf1, "Entity Path: ");
	if (err) return(err);
	
	err = oh_decode_entitypath(ep, &bigbuf2);
	if (err) return(err);

	err = oh_append_bigtext(&bigbuf1, (char *)bigbuf2.Data);
	if (err) return(err);
	err = oh_append_bigtext(&bigbuf1, "\n");
	if (err) return(err);

	fprintf(stream, "%s", bigbuf1.Data);

        return(SA_OK);
}

/**********************************************************************
 * oh_derive_string:
 * @ep - Pointer to entity's HPI SaHpiEntityPathT.
 * @str - Un-normalized character string.
 *
 * This function "normalizes" a string (such as an SNMP OID) 
 * based on entity path. Starting from the end of @str, this routine 
 * replaces the letter 'x', with the last location number of entity path,
 * the process is repeated until all 'x' are replaced by an location number.
 * For example,
 * 
 * @str = ".1.3.6.1.4.1.2.3.x.2.22.1.5.1.1.5.x"
 * @ep = {SAHPI_ENT_CHASSIS, 51}{SAHPI_ENT_SBC_BLADE, 3}
 *
 * Returns a normalized string of ".1.3.6.1.4.1.2.3.51.2.22.1.5.1.1.5.3".
 *
 * If @str does not contain any 'x' characters, this routine still 
 * allocates memory and returns a "normalized" string. In this case,
 * the normalized string is identical to @str.
 *
 * Note!
 * Caller of this routine MUST g_free() the returned normalized string
 * when finished with it.
 *
 * Returns:
 * Pointer to normalize string - Normal case.
 * NULL - Error.
 **********************************************************************/
gchar * oh_derive_string(SaHpiEntityPathT *ep, const gchar *str)
{
        gchar *new_str = NULL, *str_walker = NULL;
        gchar **fragments = NULL, **str_nodes = NULL;
        guint num_epe, num_blanks, str_strlen = 0;
        guint total_num_digits, i, work_location_num, num_digits;

	if (!ep || !str) {
		dbg("NULL parameter.");
		return(NULL);
	}

        for (num_epe = 0;
             ep->Entry[num_epe].EntityType != SAHPI_ENT_ROOT && num_epe < SAHPI_MAX_ENTITY_PATH;
             num_epe++);
        /* trace("Number of elements in entity path: %d", num_epe); */

        if (num_epe == 0) {
                dbg("Entity Path is null.");
                return(NULL);
        }
        if ((str_strlen = strlen(str)) == 0) return(NULL); /* Str is zero length */
        if (!strrchr(str, OH_DERIVE_BLANK_CHAR)) return(g_strdup(str)); /* Nothing to replace */

        for (num_blanks=0, i=0; i<str_strlen; i++) {
                if (str[i] == OH_DERIVE_BLANK_CHAR) num_blanks++;
        }
        /* trace("Number of blanks in str: %d, %s", num_blanks, str); */
        if (num_blanks > num_epe) {
                dbg("Number of replacments=%d > entity path elements=%d", num_blanks, num_epe);
                return(NULL);
        }

        fragments = g_strsplit(str, OH_DERIVE_BLANK_STR, - 1);
        if (!fragments) { dbg("Cannot split string"); goto CLEANUP; }
        str_nodes = g_malloc0((num_blanks + 1) * sizeof(gchar **));
        if (!str_nodes) { dbg("Out of memory."); goto CLEANUP; }
        total_num_digits = 0;
        for (i=0; i<num_blanks; i++) {
                work_location_num = ep->Entry[num_blanks-1-i].EntityLocation;
                for (num_digits = 1;
                     (work_location_num = work_location_num/10) > 0; num_digits++);
                str_nodes[i] = g_malloc0((num_digits+1) * sizeof(gchar));
                if (!str_nodes[i]) {dbg("Out of memory."); goto CLEANUP;}
                snprintf(str_nodes[i], (num_digits + 1) * sizeof(gchar), "%d", 
			 ep->Entry[num_blanks - 1 - i].EntityLocation);
                /* trace("Location number: %s", str_nodes[i]); */
                total_num_digits = total_num_digits + num_digits;
        }

        new_str = g_malloc0((str_strlen-num_blanks + total_num_digits + 1) * sizeof(gchar));
        if (!new_str) { dbg("Out of memory."); goto CLEANUP; }
        str_walker = new_str;
        for (i=0; fragments[i]; i++) {
                str_walker = strcpy(str_walker, fragments[i]);
                str_walker = str_walker + strlen(fragments[i]);
                if (str_nodes[i]) {
                        str_walker = strcpy(str_walker, str_nodes[i]);
                        /* trace("Location number: %s", str_nodes[i]); */
                        str_walker = str_walker + strlen(str_nodes[i]);
                }
                /* trace("New str: %s", new_str); */
        }

CLEANUP:
        g_strfreev(fragments);
        g_strfreev(str_nodes);

        return(new_str);
}
