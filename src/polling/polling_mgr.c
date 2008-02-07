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
#include <sandesha2_polling_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_permanent_seq_property_mgr.h>
#include <sandesha2_permanent_sender_mgr.h>
#include <sandesha2_permanent_next_msg_mgr.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_seq.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_msg_creator.h>
#include <sandesha2_sender_bean.h>
#include <axis2_addr.h>
#include <axis2_engine.h>
#include <axutil_uuid_gen.h>
#include <axutil_rand.h>
#include <stdio.h>
#include <platforms/axutil_platform_auto_sense.h>
#include <axutil_types.h>

axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_start (
    const axutil_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_sender_mgr_t *sender_mgr, 
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *internal_seq_id)
{
    sandesha2_next_msg_bean_t *next_msg_bean = NULL;
    sandesha2_msg_ctx_t *ref_rm_msg_ctx = NULL;
    sandesha2_msg_ctx_t *make_conn_rm_msg_ctx = NULL;
    sandesha2_sender_bean_t *make_conn_sender_bean = NULL;
    axis2_char_t *make_conn_seq_id = NULL;
    axis2_char_t *ref_msg_key = NULL;
    axis2_char_t *seq_prop_key = NULL;
    axis2_char_t *reply_to = NULL;
    axis2_char_t *wsrm_anon_reply_to_uri = NULL;
    axis2_char_t *make_conn_msg_store_key = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_msg_ctx_t *ref_msg_ctx = NULL;
    axis2_msg_ctx_t *make_conn_msg_ctx = NULL;
    axis2_endpoint_ref_t *to = NULL;
    axutil_property_t *property = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    sandesha2_next_msg_bean_t *find_bean = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Entry:sandesha2_polling_mgr_start");

    find_bean = sandesha2_next_msg_bean_create(env);
    if(find_bean)
    {
        sandesha2_next_msg_bean_set_polling_mode(find_bean, env, 
            AXIS2_TRUE);
        sandesha2_next_msg_bean_set_internal_seq_id(find_bean, env, 
            internal_seq_id);
        next_msg_bean = sandesha2_next_msg_mgr_find_unique(next_msg_mgr,
            env, find_bean);
    }
    /* If no valid entry is found, try again later */
    if(!next_msg_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]No valid MakeConnection entry is found.");
        return AXIS2_FAILURE;
    }
    make_conn_seq_id = sandesha2_next_msg_bean_get_seq_id(next_msg_bean, env);
    /* Create a MakeConnection message */
    ref_msg_key = sandesha2_next_msg_bean_get_ref_msg_key(next_msg_bean, env);
    seq_prop_key = make_conn_seq_id;
    reply_to = sandesha2_utils_get_seq_property(env, seq_prop_key, 
        SANDESHA2_SEQ_PROP_REPLY_TO_EPR, seq_prop_mgr);
    if(sandesha2_utils_is_wsrm_anon_reply_to(env, reply_to))
        wsrm_anon_reply_to_uri = reply_to;
    ref_msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, 
        ref_msg_key, conf_ctx, AXIS2_FALSE);
    if(ref_msg_ctx)
        ref_rm_msg_ctx = sandesha2_msg_init_init_msg(env, ref_msg_ctx);
    make_conn_rm_msg_ctx = 
        sandesha2_msg_creator_create_make_connection_msg(env, 
        ref_rm_msg_ctx, make_conn_seq_id, wsrm_anon_reply_to_uri, seq_prop_mgr);
    if(reply_to)
        AXIS2_FREE(env->allocator, reply_to);
    if(!make_conn_rm_msg_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "No memory");
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, 
        AXIS2_TRANSPORT_IN, NULL);
    /* Storing the MakeConnection message */
    make_conn_msg_store_key = axutil_uuid_gen(env);
    property = axutil_property_create_with_args(env, 0, 0, 0, seq_prop_key);
    sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, 
        SANDESHA2_MSG_CTX_PROP_SEQUENCE_PROPERTY_KEY, property); 
    make_conn_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(make_conn_rm_msg_ctx, 
        env);
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, 
        make_conn_msg_store_key, make_conn_msg_ctx);
    /* Adde an entry for the MakeConnection message to the sender(with, 
     * send=true, resend=false)
     */
    make_conn_sender_bean = sandesha2_sender_bean_create(env);
    if(make_conn_sender_bean)
    {
        long millisecs = 0;
        millisecs = sandesha2_utils_get_current_time_in_millis(env);
        sandesha2_sender_bean_set_time_to_send(make_conn_sender_bean, env, 
            millisecs);
        sandesha2_sender_bean_set_msg_ctx_ref_key(make_conn_sender_bean, env, 
            make_conn_msg_store_key);
        msg_id = sandesha2_msg_ctx_get_msg_id(make_conn_rm_msg_ctx, env);
        sandesha2_sender_bean_set_msg_id(make_conn_sender_bean, env, msg_id);
        sandesha2_sender_bean_set_msg_type(make_conn_sender_bean, env, 
            SANDESHA2_MSG_TYPE_MAKE_CONNECTION_MSG);
        sandesha2_sender_bean_set_resend(make_conn_sender_bean, env, AXIS2_TRUE);
        sandesha2_sender_bean_set_send(make_conn_sender_bean, env, AXIS2_TRUE);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]internal_seq_id:%s", internal_seq_id);
        sandesha2_sender_bean_set_internal_seq_id(make_conn_sender_bean, 
            env, internal_seq_id);
        to = sandesha2_msg_ctx_get_to(make_conn_rm_msg_ctx, env);
        if(to)
        {
            axis2_char_t *address = (axis2_char_t *) 
                axis2_endpoint_ref_get_address(
                (const axis2_endpoint_ref_t *) to, env);
            sandesha2_sender_bean_set_to_address(make_conn_sender_bean, env, 
                address);
        }
    }
    /* This message should not be sent untils it is qualified. i.e. Till
     * it is sent through the sandesha2_transport_sender
     */
    property = axutil_property_create_with_args(env, 0, 0, 0, 
        AXIS2_VALUE_FALSE);
    sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING, property);
    if(sender_mgr)
    {
        sandesha2_sender_mgr_insert(sender_mgr, env, 
            make_conn_sender_bean);
    }
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]Sending the make "\
        "connection message for the sequence with internal sequence id %s", 
        internal_seq_id);
    status = sandesha2_utils_execute_and_store(env, make_conn_rm_msg_ctx, 
        make_conn_msg_store_key);
    if(!status)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]make_connection sending failed");
        return AXIS2_FAILURE;
    }
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Exit:sandesha2_polling_mgr_start");
    return AXIS2_SUCCESS;
}

