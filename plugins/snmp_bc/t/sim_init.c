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
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <glib.h>
#include <stdio.h>
#include <string.h>

#include <openhpi.h>
#include <snmp_util.h>
#include <snmp_bc_plugin.h>

#include <sim_init.h>
#include <sim_resources.h>

GHashTable * sim_hash;

static void free_hash_data(gpointer key, gpointer value, gpointer user_data);

SaHpiBoolT is_simulator(void) 
{
  return(SAHPI_TRUE);
}

SaErrorT sim_banner(struct snmp_bc_hnd *custom_handle)
{

	printf("************************************\n");
	printf("****** BladeCenter Simulator  ******\n");

	if (custom_handle->platform == SNMP_BC_PLATFORM_BC) {
		printf("****** BladeCenter Integrated ******\n");
	}
	if (custom_handle->platform == SNMP_BC_PLATFORM_BCT) {
		printf("********** BladeCenter T ***********\n");
	}
	
	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		printf("*************** RSA ****************\n");
	}

	printf("************************************\n");

  return(SA_OK);
}

SaErrorT sim_init() 
{
	int i;

	sim_hash = g_hash_table_new(g_str_hash, g_str_equal);
	if (sim_hash == NULL) {
		error("Cannot allocate simulation hash table");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	for (i=0; sim_resource_array[i].oid != NULL; i++) {
		
		char *key;
		char *key_exists;
		SnmpMibInfoT *mibinfo;
    
		key = g_strdup(sim_resource_array[i].oid);
		if (!key) {
			error("Cannot allocate memory for key for oid=%s",
			      sim_resource_array[i].oid);
			sim_close();
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		mibinfo = g_malloc0(sizeof(SnmpMibInfoT));
		if (!mibinfo) {
			error("Cannot allocate memory for hash value for oid=%s", 
			      sim_resource_array[i].oid);
			sim_close();
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		key_exists = g_hash_table_lookup(sim_hash, key); 
		if (!key_exists) {
			mibinfo->type = sim_resource_array[i].mib.type;

			switch (mibinfo->type) {
			case ASN_INTEGER:
				mibinfo->value.integer = sim_resource_array[i].mib.value.integer;
				break;
			case ASN_OCTET_STR:
				strcpy(mibinfo->value.string, sim_resource_array[i].mib.value.string);
				break;
			default:
				error("Unknown SNMP type=%d for oid=%s", mibinfo->type, key);
				return(SA_ERR_HPI_INTERNAL_ERROR);
			}
			g_hash_table_insert(sim_hash, key, mibinfo);
		}
		else {
			info("Oid %s is defined twice", sim_resource_array[i].oid);
		}
	}

	return(SA_OK);
}

SaErrorT sim_close()
{
        g_hash_table_foreach(sim_hash, free_hash_data, NULL);
	g_hash_table_destroy(sim_hash);

	return(SA_OK);
}

static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
        g_free(key);
        g_free(value);
}
