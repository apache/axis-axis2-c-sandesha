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
 
#include <sandesha2/sandesha2_utils.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2/sandesha2_constants.h>
#include <sandesha2/sandesha2_property_bean.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_ack_range.h>
#include <sandesha2/sandesha2_spec_specific_consts.h>
#include <axis2_string.h>
#include <axis2_uuid_gen.h>
#include <axis2_addr.h>
#include <axis2_property.h>
#include <axis2_array_list.h>

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_clean_recv_side_after_terminate_msg(
                        const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_clean_recv_side_after_invocation(
                        const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_complete_term_on_recv_side(
                        const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_remove_recv_side_propertis(
                        const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_terminate_sending_side(
                        const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id,
                        axis2_bool_t svr_side,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_do_updates_if_needed(
                        const axis2_env_t *env,
                        axis2_char_t *seq_id,
                        sandesha2_seq_property_bean_t *prop_bean,
                        sandesha2_seq_property_mgr_t *prop_mgr)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, prop_bean, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, prop_mgr, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_terminate_mgr_is_property_deletable(
                        const axis2_env_t *env,
                        axis2_char_t *name)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
        
    
    return AXIS2_TRUE;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_time_out_sending_side_seq(
                        const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id,
                        axis2_bool_t svr_side,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_clean_sending_side_data(
                        const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id,
                        axis2_bool_t svr_side,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_add_terminate_seq_msg(
                        const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx,
                        axis2_char_t *out_seq_id,
                        axis2_char_t *int_seq_id,
                        sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, out_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, int_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    return AXIS2_SUCCESS;
}
