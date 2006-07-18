/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef SANDESHA2_UTILS_H
#define SANDESHA2_UTILS_H

#include <axiom_soap_envelope.h>

/**
  * @file sandesha2_utils.h
  * @brief 
  */

#include <sandesha2_storage_mgr.h>
#include <axis2_conf_ctx.h>
#include <axis2_conf.h>
#include <axiom_soap_envelope.h>
#include <axis2_qname.h>
#include <axis2_array_list.h>
#include <axis2_op.h>
#include <sandesha2/sandesha2_property_bean.h>
#include <sandesha2/sandesha2_msg_ctx.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_utils
 * @ingroup sandesha2_util
 * @{
 */
/* Array list types */
#define SANDESHA2_ARRAY_LIST_STRING 0
#define SANDESHA2_ARRAY_LIST_LONG 1


AXIS2_EXTERN long AXIS2_CALL
sandesha2_utils_get_current_time_in_millis(
        const axis2_env_t *env);

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_utils_remove_soap_body_part(const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope, axis2_qname_t *qname);
                        
AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_rm_version(const axis2_env_t *env,
                        axis2_char_t *key,
                        sandesha2_storage_mgr_t *storage_man);
                        
AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_utils_get_storage_mgr(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_conf_t *conf);
                        
AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_seq_property(const axis2_env_t *env,
                        axis2_char_t *id,
                        axis2_char_t *name,
                        sandesha2_storage_mgr_t *storage_man);

AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_utils_get_property_bean(const axis2_env_t *env,
                        axis2_conf_t *conf);

AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_utils_get_property_bean_from_op(const axis2_env_t *env,
                        axis2_op_t *op);

AXIS2_EXTERN axis2_array_list_t* AXIS2_CALL
sandesha2_utils_get_array_list_from_string(const axis2_env_t *env,
                        axis2_char_t *string);

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_utils_array_list_contains(const axis2_env_t *env,
                        axis2_array_list_t *list,
                        axis2_char_t *string);

AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_array_list_to_string(const axis2_env_t *env,
                        axis2_array_list_t *list,
                        int type);

AXIS2_EXTERN axis2_status_t AXIS2_CALL                        
sandesha2_utils_start_invoker_for_seq(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id);

AXIS2_EXTERN axis2_status_t AXIS2_CALL                        
sandesha2_utils_start_sender_for_seq(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id);
                        
AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_outgoing_internal_seq_id(const axis2_env_t *env,
                        axis2_char_t *seq_id);

AXIS2_EXTERN axis2_transport_out_desc_t* AXIS2_CALL
sandesha2_utils_get_transport_out(const axis2_env_t *env);

AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_utils_get_inmemory_storage_mgr(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx);
                        
AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_utils_get_permanent_storage_mgr(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx);

AXIS2_EXTERN axis2_char_t* AXIS2_CALL                       
sandesha2_utils_get_svr_side_incoming_seq_id(const axis2_env_t *env,
                        axis2_char_t *incoming_seq_id);

/**
 * Caller must free the returned string.
 * @param to
 * @param seq_key
 *
 * @return internal_seq_id
 */
AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_internal_seq_id(
        const axis2_env_t *env,
        axis2_char_t *to,
        axis2_char_t *seq_key);

AXIS2_EXTERN axis2_msg_ctx_t *AXIS2_CALL
sandesha2_utils_create_new_related_msg_ctx(const axis2_env_t *env,
                        sandesha2_msg_ctx_t *ref_rm_msg,
                        axis2_op_t *op);
                        
AXIS2_EXTERN  int AXIS2_CALL
sandesha2_utils_get_soap_version(const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope);
                        

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_UTILS_H */

