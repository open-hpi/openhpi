/*
 * The code is used to prompt user if threshold event is generated
 *  
 * Author: Intel Corporation
 *         Racing Guo <racing.guo@intel.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <time.h>

#include <OpenIPMI/ipmiif.h>
#include <OpenIPMI/ipmi_smi.h>
#include <OpenIPMI/ipmi_err.h>
#include <OpenIPMI/ipmi_auth.h>
#include <OpenIPMI/ipmi_lan.h>
#include <OpenIPMI/ipmi_posix.h>
#include <OpenIPMI/ipmi_int.h>

/* This file should not normally be included by the user, but is here
   to demonstrate an internal domain function. */
#include <OpenIPMI/ipmi_domain.h>

/* This sample application demostrates a very simple method to use
   OpenIPMI. It just search all sensors in the system.  From this
   application, you can find that there is only 4 lines code in main()
   function if you use the SMI-only interface, and several simple
   callback functions in all cases. */

static const char *progname;

static void
usage(void)
{
    printf("Usage:\n"
	   "  %s [options] smi <smi #>\n"
	   "     Make a connection to a local system management interface.\n"
	   "     smi # is generally 0.\n"
	   "  %s [options] lan <host> <port> "
	   "     <authtype> <privilege> <username> <password>\n"
	   "     Make a connection to a IPMI 1.5 LAN interface.\n"
	   "     Host and port specify where to connect to (port is\n"
	   "     generally 623).  authtype is none, md2, md5, or straight.\n"
	   "     privilege is callback, user, operator, or admin.  The\n"
	   "     username and password must be provided if the authtype is\n"
	   "     not none.\n", progname, progname);
}

static void sensor_threshold_event_handler
                                   (ipmi_sensor_t                *sensor,
                                   enum ipmi_event_dir_e        dir,
                                   enum ipmi_thresh_e           threshold,
                                   enum ipmi_event_value_dir_e  high_low,
                                   enum ipmi_value_present_e    value_present,
                                   unsigned int                 raw_value,
                                   double                       value,
                                   void                         *cb_data,
                                   ipmi_event_t                 *event)
{
    char name[33];
    struct timeval tv;

    ipmi_sensor_get_id(sensor, name, 32);
    gettimeofday(&tv, NULL);
    printf("sensor threshold event: %s at %ld\n",  name, tv.tv_sec);

}
/* Whenever the status of a sensor changes, the function is called
   We display the information of the sensor if we find a new sensor
*/
static void
sensor_change(enum ipmi_update_e op,
	      ipmi_entity_t      *ent,
	      ipmi_sensor_t      *sensor,
	      void               *cb_data)
{
    int id, instance, rv;
    char name[50];

    id = ipmi_entity_get_entity_id(ent);
    instance = ipmi_entity_get_entity_instance(ent);
    ipmi_sensor_get_id(sensor, name, 50);
    if (op == IPMI_ADDED) {

        if (ipmi_sensor_get_event_reading_type(sensor)
                == IPMI_EVENT_READING_TYPE_THRESHOLD) {
                rv = ipmi_sensor_threshold_set_event_handler(
                    sensor,
                    sensor_threshold_event_handler,
                    NULL);
                if (rv) {
                    printf("cannot set event handler %d\n", rv);
                    exit(-1);
                }
        }
    }

}

/* Whenever the status of an entity changes, the function is called
   When a new entity is created, we search all sensors that belong 
   to the entity */
static void
entity_change(enum ipmi_update_e op,
	      ipmi_domain_t      *domain,
	      ipmi_entity_t      *entity,
	      void               *cb_data)
{
    int rv;
    int id, instance;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);
    if (op == IPMI_ADDED) {
	    printf("Entity added: %d.%d\n", id, instance);
	    /* Register callback so that when the status of a
	       sensor changes, sensor_change is called */
	    rv = ipmi_entity_set_sensor_update_handler(entity,
						       sensor_change,
						       entity);
	    if (rv) {
		printf("ipmi_entity_set_sensor_update_handler: 0x%x", rv);
		exit(1);
	    }
    }
}

static void reread_sels_done(ipmi_domain_t *domain, int err, void *cb_data)
{
    int count;
    ipmi_domain_sel_entries_used(domain, &count);
    printf("reread sel done. entries = %d\n", count);
}

static void bus_scan_done(ipmi_domain_t *domain, int err, void *cb_data)
{
    int count;
    ipmi_domain_reread_sels(domain, reread_sels_done, &count);
    printf("bus scan done.\n");
    return;
}


/* After we have established connection to domain, this function get called
   At this time, we can do whatever things we want to do. Herr we want to
   search all entities in the system */ 
static void
setup_done(ipmi_domain_t *domain,
	   int           err,
	   unsigned int  conn_num,
	   unsigned int  port_num,
	   int           still_connected,
	   void          *user_data)
{
    int rv;

    /* Register a callback functin entity_change. When a new entities 
       is created, entity_change is called */
    rv = ipmi_domain_set_entity_update_handler(domain, entity_change, domain);
    if (rv) {      
	printf("ipmi_domain_set_entity_update_handler return error: %d\n", rv);
	return;
    }

    rv = ipmi_domain_set_bus_scan_handler(domain, bus_scan_done, NULL);
    if (rv) {
	printf("ipmi_domain_set_bus_scan_handler return error: %d\n", rv);
	return;
    }
    
}

static os_handler_t *os_hnd;
static selector_t *sel;

int
main(int argc, const char *argv[])
{
    int         rv;
    int         curr_arg = 1;
    ipmi_args_t *args;
    ipmi_con_t  *con;

    progname = argv[0];

    /* OS handler allocated first. */
    os_hnd = ipmi_posix_get_os_handler();
    if (!os_hnd) {
	printf("ipmi_smi_setup_con: Unable to allocate os handler\n");
	exit(1);
    }

    /* Create selector with os handler. */
    sel_alloc_selector(os_hnd, &sel);

    /* The OS handler has to know about the selector. */
    ipmi_posix_os_handler_set_sel(os_hnd, sel);

    /* Initialize the OpenIPMI library. */
    ipmi_init(os_hnd);

#if 0
    if(argc > 1) {
        argc--;
        sname = argv[argc-1];
    }else {
        usage();
        exit(-1);
    }
#endif

    rv = ipmi_parse_args(&curr_arg, argc, argv, &args);
    if (rv) {
	fprintf(stderr, "Error parsing command arguments, argument %d: %s\n",
		curr_arg, strerror(rv));
	usage();
	exit(1);
    }

    rv = ipmi_args_setup_con(args, os_hnd, sel, &con);
    if (rv) {
        fprintf(stderr, "ipmi_ip_setup_con: %s", strerror(rv));
	exit(1);
    }

    rv = ipmi_init_domain(&con, 1, setup_done, NULL, NULL, NULL);
    if (rv) {
	fprintf(stderr, "ipmi_init_domain: %s\n", strerror(rv));
	exit(1);
    }

    /* We run the select loop here, this shows how you can use
       sel_select.  You could add your own processing in this loop. */
    while (1) {
	sel_select(sel, NULL, 0, NULL, NULL);
    }
}

void
posix_vlog(char *format, enum ipmi_log_type_e log_type, va_list ap)
{
    int do_nl = 1;

    switch(log_type)
    {
	case IPMI_LOG_INFO:
	    printf("INFO: ");
	    break;

	case IPMI_LOG_WARNING:
	    printf("WARN: ");
	    break;

	case IPMI_LOG_SEVERE:
	    printf("SEVR: ");
	    break;

	case IPMI_LOG_FATAL:
	    printf("FATL: ");
	    break;

	case IPMI_LOG_ERR_INFO:
	    printf("EINF: ");
	    break;

	case IPMI_LOG_DEBUG_START:
	    do_nl = 0;
	    /* FALLTHROUGH */
	case IPMI_LOG_DEBUG:
	    printf("DEBG: ");
	    break;

	case IPMI_LOG_DEBUG_CONT:
	    do_nl = 0;
	    /* FALLTHROUGH */
	case IPMI_LOG_DEBUG_END:
	    break;
    }

    vprintf(format, ap);

    if (do_nl)
	printf("\n");
}
