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
 *      Steve Sherman  <stevees@us.ibm.com>
 *      W. David Ashley  <dashley@us.ibm.com>
 */

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <snmp_rsa_session.h>

#include <rsa_resources.h>
#include <sim_resources.h>

int is_simulator(void);
int sim_banner(void);
int sim_init(void);
int sim_close(void);

static void free_hash_data(gpointer key, gpointer value, gpointer user_data);

int is_simulator(void) 
{
  return 1;
}

int sim_banner(void)
{

	printf("************************************\n");
	printf("********** RSA Simulator  **********\n");
	printf("************************************\n");

  return 0;

}

int sim_init() 
{
	int i;

	sim_hash = g_hash_table_new(g_str_hash, g_str_equal);
	if (sim_hash == NULL) {
		dbg("Cannot allocate simulation hash table\n");
		return -1;
	}

	for (i=0; sim_resource_array[i].oid != NULL; i++) {
		
		char *key;
		char *key_exists;
		SnmpMibInfoT *mibinfo;
    
		key = g_strdup(sim_resource_array[i].oid);
		if (!key) {
			dbg("Cannot allocate memory for key for oid=%s\n",
			    sim_resource_array[i].oid);
			sim_close();
			return -1;
		}
		mibinfo = g_malloc0(sizeof(SnmpMibInfoT));
		if (!mibinfo) {
			dbg("Cannot allocate memory for hash value for oid=%s", 
			    sim_resource_array[i].oid);
			sim_close();
			return -1;
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
				dbg("Unknown SNMP type=%d for oid=%s\n", mibinfo->type, key);
				return -1;
			}
			g_hash_table_insert(sim_hash, key, mibinfo);
		}
		else {
			dbg("WARNING: Oid %s is defined twice\n", sim_resource_array[i].oid);
		}
	}

	return 0;
}

int sim_close()
{
        g_hash_table_foreach(sim_hash, free_hash_data, NULL);
	g_hash_table_destroy(sim_hash);

	return 0;
}

static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
        g_free(key);
        g_free(value);
}

