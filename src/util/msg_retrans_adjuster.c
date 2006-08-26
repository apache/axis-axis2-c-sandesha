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
 
#include <sandesha2_msg_retrans_adjuster.h>
#include <sandesha2_utils.h>
#include <sandesha2_constants.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_msg_init.h>
#include <axis2_property.h>
#include <sandesha2_terminate_mgr.h>

/******************************************************************************/
sandesha2_sender_bean_t * AXIS2_CALL
sandesha2_msg_retrans_adjuster_adjust_next_retrans_time(const axis2_env_t *env, 
                        sandesha2_sender_bean_t *retrans_bean, 
                        sandesha2_property_bean_t *property_bean);

long AXIS2_CALL
sandesha2_msg_retrans_adjuster_next_exp_backoff_diff(const axis2_env_t *env,
                        int count,
                        long initial_interval);
                        
axis2_status_t AXIS2_CALL
sandesha2_msg_retrans_adjuster_finalize_timedout_seq(const axis2_env_t *env,
                        axis2_char_t *int_seq_id,
                        axis2_char_t *seq_id,
                        axis2_msg_ctx_t *msg_ctx,
                        sandesha2_storage_mgr_t *storage_mgr);

/******************************************************************************/


AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_msg_retrans_adjuster_adjust_retrans(
        const axis2_env_t *env,
        sandesha2_sender_bean_t *retrans_bean,
        axis2_conf_ctx_t *conf_ctx, 
        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_char_t *stored_key = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    axis2_char_t *int_seq_id = NULL;
    axis2_char_t *seq_id = NULL;
    sandesha2_property_bean_t *property_bean = NULL;
    int max_attempts = -1;
    axis2_bool_t timeout_seq = AXIS2_FALSE;
    axis2_bool_t seq_timed_out = AXIS2_FALSE;
    axis2_bool_t continue_sending = AXIS2_TRUE;
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, retrans_bean, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FALSE);
    
    stored_key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(retrans_bean, env);
    if(NULL == stored_key)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Stored Key not"
                        " present in the retransmittable message");
        return AXIS2_FALSE;
    }
    msg_ctx = SANDESHA2_STORAGE_MGR_RETRIEVE_MSG_CTX(storage_mgr, env, 
                        stored_key, conf_ctx);
    rm_msg_ctx = sandesha2_msg_init_init_msg(env, msg_ctx);
    int_seq_id = SANDESHA2_SENDER_BEAN_GET_INTERNAL_SEQ_ID(retrans_bean, env);
    seq_id = SANDESHA2_SENDER_BEAN_GET_SEQ_ID(retrans_bean, env);
    
    property_bean = sandesha2_utils_get_property_bean_from_op(env, 
                        AXIS2_MSG_CTX_GET_OP(msg_ctx, env));
    SANDESHA2_SENDER_BEAN_SET_SENT_COUNT(retrans_bean, env, 
                        SANDESHA2_SENDER_BEAN_GET_SENT_COUNT(retrans_bean, 
                        env) + 1);
    sandesha2_msg_retrans_adjuster_adjust_next_retrans_time(env, retrans_bean,
                        property_bean);
    max_attempts = SANDESHA2_PROPERTY_BEAN_GET_MAX_RETRANS_COUNT(property_bean, 
                        env);
    
    if(max_attempts > 0 && SANDESHA2_SENDER_BEAN_GET_SENT_COUNT(retrans_bean, 
                        env) > max_attempts)
        timeout_seq = AXIS2_TRUE;
    seq_timed_out = sandesha2_seq_mgr_has_seq_timedout(env, int_seq_id, 
                        rm_msg_ctx, storage_mgr);
    
    if(AXIS2_TRUE == seq_timed_out)
        timeout_seq = AXIS2_TRUE;
        
    if(AXIS2_TRUE == timeout_seq)
    {
        SANDESHA2_SENDER_BEAN_SET_SEND(retrans_bean, env, AXIS2_FALSE);
        sandesha2_msg_retrans_adjuster_finalize_timedout_seq(env, int_seq_id,
                        seq_id, msg_ctx, storage_mgr);
        continue_sending = AXIS2_FALSE;
    }
    return continue_sending;
}

sandesha2_sender_bean_t * AXIS2_CALL
sandesha2_msg_retrans_adjuster_adjust_next_retrans_time(const axis2_env_t *env, 
                        sandesha2_sender_bean_t *retrans_bean, 
                        sandesha2_property_bean_t *property_bean)
{
    int count = -1;
    long base_interval = -1;
    long new_interval = -1;
    long new_time_to_send = 0;
    long time_now = -1;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, retrans_bean, NULL);
    AXIS2_PARAM_CHECK(env->error, property_bean, NULL);
    
    count = SANDESHA2_SENDER_BEAN_GET_SENT_COUNT(retrans_bean, env);
    base_interval = SANDESHA2_PROPERTY_BEAN_GET_RETRANS_INTERVAL(property_bean,
                        env);
    new_interval = base_interval;
    if(AXIS2_TRUE == SANDESHA2_PROPERTY_BEAN_IS_EXP_BACKOFF(property_bean, env))
    {
        new_interval = sandesha2_msg_retrans_adjuster_next_exp_backoff_diff(env,
                        count, base_interval);
    }
    time_now = sandesha2_utils_get_current_time_in_millis(env);
    
    new_time_to_send = time_now + new_interval;
    SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(retrans_bean, env, new_time_to_send);
    return retrans_bean;
}



long AXIS2_CALL
sandesha2_msg_retrans_adjuster_next_exp_backoff_diff(const axis2_env_t *env,
                        int count,
                        long initial_interval)
{
    long interval = initial_interval;
    AXIS2_ENV_CHECK(env, -1);
    
    interval = initial_interval * (2^count);
    return interval;
}

axis2_status_t AXIS2_CALL
sandesha2_msg_retrans_adjuster_finalize_timedout_seq(const axis2_env_t *env,
                        axis2_char_t *int_seq_id,
                        axis2_char_t *seq_id,
                        axis2_msg_ctx_t *msg_ctx,
                        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_ctx_t *ctx = NULL;
    axis2_property_t *property = NULL;
    
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, int_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_WITHIN_TRANSACTION, AXIS2_FALSE);
    AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_WITHIN_TRANSACTION, property,
                        AXIS2_FALSE);
    /* we have to callback listener here */
    sandesha2_terminate_mgr_time_out_sending_side_seq(env, conf_ctx, int_seq_id,
                        AXIS2_FALSE, storage_mgr);
 
    return AXIS2_SUCCESS;
}
