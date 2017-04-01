/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett Packard Enterprise, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 **/

#include "ov_rest_watchdog.h"

/**
 * ov_rest_get_watchdog_info
 *      @oh_handler:  Handler data pointer
 *      @resource_id: Resource ID
 *      @num:         Watchdog rdr number
 *      @wdt:         Watchdog structure
 *
 * Purpose:
 *      Gets watchdog information of the resource
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current ov_rest implementation does not
 *                                   support this API
 **/
SaErrorT ov_rest_get_watchdog_info(void *oh_handler,
                                    SaHpiResourceIdT resource_id,
                                    SaHpiWatchdogNumT num,
                                    SaHpiWatchdogT *wdt)
{
        err("ov_rest_get_watchdog_info not implemented");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_set_watchdog_info
 *      @oh_handler:  Handler data pointer
 *      @resource_id: Resource ID
 *      @num:         Watchdog rdr number
 *      @wdt:         Watchdog structure
 *
 * Purpose:
 *      Sets watchdog information of the resource
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current ov_rest implementation does not
 *                                   support this API
 **/
SaErrorT ov_rest_set_watchdog_info(void *oh_handler,
                                    SaHpiResourceIdT resource_id,
                                    SaHpiWatchdogNumT num,
                                    SaHpiWatchdogT *wdt)
{
        err("ov_rest_set_watchdog_info not implemented");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_reset_watchdog
 *      @oh_handler:  Handler data pointer
 *      @resource_id: Resource ID
 *      @num:         Watchdog rdr number
 *
 * Purpose:
 *      Starts/Restarts the watchdog timer from initial count down
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current ov_rest implementation does not
 *                                   support this API
 **/
SaErrorT ov_rest_reset_watchdog(void *oh_handler,
                                 SaHpiResourceIdT resource_id,
                                 SaHpiWatchdogNumT num)
{
        err("ov_rest_reset_watchdog not implemented");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

void * oh_get_watchdog_info (void *,
                             SaHpiResourceIdT,
                             SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("ov_rest_get_watchdog_info")));

void * oh_set_watchdog_info (void *,
                             SaHpiResourceIdT,
                             SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("ov_rest_set_watchdog_info")));

void * oh_reset_watchdog (void *,
                          SaHpiResourceIdT,
                          SaHpiWatchdogNumT)
                __attribute__ ((weak, alias("ov_rest_reset_watchdog")));
