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

#ifndef _OV_REST_FUMI_H
#define _OV_REST_FUMI_H

/* Include files */
#include <SaHpi.h>
#include <oh_error.h>

SaErrorT ov_rest_set_fumi_source(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum,
                                SaHpiTextBufferT *sourceuri);

SaErrorT ov_rest_validate_fumi_source(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum);

SaErrorT ov_rest_get_fumi_source(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum,
                                SaHpiFumiSourceInfoT *sourceinfo);

SaErrorT ov_rest_get_fumi_target(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum,
                                SaHpiFumiBankInfoT *bankinfo);

SaErrorT ov_rest_start_fumi_backup(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num);

SaErrorT ov_rest_set_fumi_bank_order(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum,
                                SaHpiUint32T position);

SaErrorT ov_rest_start_fumi_bank_copy(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT sourcebanknum,
                                SaHpiBankNumT targetbanknum);

SaErrorT ov_rest_start_fumi_install(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum);

SaErrorT ov_rest_get_fumi_status(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum,
                                SaHpiFumiUpgradeStatusT *status);

SaErrorT ov_rest_start_fumi_verify(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum);

SaErrorT ov_rest_cancel_fumi_upgrade(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num,
                                SaHpiBankNumT banknum);

SaErrorT ov_rest_start_fumi_rollback(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num);

SaErrorT ov_rest_activate_fumi(void *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiFumiNumT num);

#endif
