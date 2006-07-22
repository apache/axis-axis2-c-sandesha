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
#include <sandesha2/sandesha2_utils.h>
#include <sandesha2/sandesha2_constants.h>
#include <sandesha2/sandesha2_spec_specific_consts.h>
#include <sandesha2_msg_init.h>


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

