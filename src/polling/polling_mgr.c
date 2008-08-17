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


/** 
 * @brief Polling Manager struct impl
 *	Sandesha2 Polling Manager
 */
typedef struct sandesha2_polling_mgr_args sandesha2_polling_mgr_args_t;

struct sandesha2_polling_mgr_args
{
    axutil_env_t *env;
	axis2_conf_ctx_t *conf_ctx;
    sandesha2_msg_ctx_t *rm_msg_ctx;
    axis2_char_t *internal_sequence_id;
    axis2_char_t *sequence_id;
};
            
/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_polling_mgr_worker_func(
    axutil_thread_t *thd, 
    void *data);

axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_start (
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_char_t *internal_sequence_id,
    axis2_char_t *sequence_id,
    const axis2_char_t *reply_to)
{
    axutil_thread_t *worker_thread = NULL;
    sandesha2_polling_mgr_args_t *args = NULL;
    axis2_char_t *wsrm_anon_reply_to_uri = NULL;
    sandesha2_msg_ctx_t *make_conn_rm_msg_ctx = NULL;
    
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_polling_mgr_args_t)); 
    args->env = axutil_init_thread_env(env);
    args->conf_ctx = conf_ctx;
    args->internal_sequence_id = (axis2_char_t *) internal_sequence_id;
    args->sequence_id = (axis2_char_t *) sequence_id;

    if(sandesha2_utils_is_wsrm_anon_reply_to(env, reply_to))
    {
        wsrm_anon_reply_to_uri = (axis2_char_t *) reply_to;
    }

    make_conn_rm_msg_ctx = sandesha2_msg_creator_create_make_connection_msg(env, rm_msg_ctx, 
        sequence_id, wsrm_anon_reply_to_uri, NULL);
    
    args->rm_msg_ctx = make_conn_rm_msg_ctx;

    worker_thread = axutil_thread_pool_get_thread(env->thread_pool, 
            sandesha2_polling_mgr_worker_func, (void*)args);

    if(!worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "Thread creation failed sandesha2_polling_mgr_run");

        return AXIS2_FAILURE;
    }

    axutil_thread_pool_thread_detach(env->thread_pool, worker_thread); 
        
    return AXIS2_SUCCESS;
}

/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_polling_mgr_worker_func(
    axutil_thread_t *thd, 
    void *data)
{
    axis2_char_t *dbname = NULL;
    axis2_char_t *internal_sequence_id = NULL;
    axis2_char_t *sequence_id = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_sender_mgr_t *sender_mgr = NULL;
    sandesha2_next_msg_mgr_t *next_msg_mgr = NULL;
    axis2_engine_t *engine = NULL;
    axis2_msg_ctx_t *make_conn_msg_ctx = NULL;
    sandesha2_property_bean_t *property_bean = NULL;
    axis2_conf_t *conf = NULL;
    int wait_time = 0;
    sandesha2_msg_ctx_t *make_conn_rm_msg_ctx = NULL;
    
    sandesha2_polling_mgr_args_t *args = (sandesha2_polling_mgr_args_t*)data;
    axutil_env_t *env = args->env;
    conf_ctx = args->conf_ctx;
    make_conn_rm_msg_ctx = args->rm_msg_ctx;
    internal_sequence_id = axutil_strdup(env, args->internal_sequence_id);
    sequence_id = axutil_strdup(env, args->sequence_id);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[sandesha2] Entry:sandesha2_polling_mgr_worker_func");

    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);
    next_msg_mgr = sandesha2_permanent_next_msg_mgr_create(env, dbname);
    engine = axis2_engine_create(env, conf_ctx);
 
    make_conn_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(make_conn_rm_msg_ctx, env);

    conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    property_bean = sandesha2_utils_get_property_bean(env, conf);
    wait_time = sandesha2_property_bean_get_polling_delay(property_bean, env);

    while(AXIS2_TRUE)
    {
        axis2_char_t *make_conn_msg_store_key = NULL;
        axutil_property_t *property = NULL;
        axis2_status_t status = AXIS2_FAILURE;

        AXIS2_SLEEP(wait_time);
        
        sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
        make_conn_msg_store_key = axutil_uuid_gen(env);
        property = axutil_property_create_with_args(env, 0, 0, 0, sequence_id);
        sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, 
            SANDESHA2_MSG_CTX_PROP_SEQUENCE_PROPERTY_KEY, property);

        /*sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, make_conn_msg_store_key, 
                make_conn_msg_ctx, AXIS2_TRUE);*/

        /* Adde an entry for the MakeConnection message to the sender(with, 
         * send=true, resend=false)
         */
        /*make_conn_sender_bean = sandesha2_sender_bean_create(env);
        if(make_conn_sender_bean)
        {
            long millisecs = 0;

            millisecs = sandesha2_utils_get_current_time_in_millis(env);
            sandesha2_sender_bean_set_time_to_send(make_conn_sender_bean, env, millisecs);
            sandesha2_sender_bean_set_msg_ctx_ref_key(make_conn_sender_bean, env, 
                    make_conn_msg_store_key);

            msg_id = sandesha2_msg_ctx_get_msg_id(make_conn_rm_msg_ctx, env);
            sandesha2_sender_bean_set_msg_id(make_conn_sender_bean, env, msg_id);
            sandesha2_sender_bean_set_msg_type(make_conn_sender_bean, env, 
                SANDESHA2_MSG_TYPE_MAKE_CONNECTION_MSG);

            sandesha2_sender_bean_set_resend(make_conn_sender_bean, env, AXIS2_FALSE);
            sandesha2_sender_bean_set_send(make_conn_sender_bean, env, AXIS2_TRUE);
            //sandesha2_sender_bean_set_seq_id(make_conn_sender_bean, env, seq_id);
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]seq_id:%s", seq_id);
            sandesha2_sender_bean_set_internal_seq_id(make_conn_sender_bean, env, seq_id);
            to = sandesha2_msg_ctx_get_to(make_conn_rm_msg_ctx, env);
            if(to)
            {
                axis2_char_t *address = (axis2_char_t *) axis2_endpoint_ref_get_address(
                    (const axis2_endpoint_ref_t *) to, env);
                sandesha2_sender_bean_set_to_address(make_conn_sender_bean, env, address);
            }
        }*/

        /* This message should not be sent untils it is qualified. i.e. Till
         * it is sent through the sandesha2_transport_sender
         */
        /*property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_FALSE);
        sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, SANDESHA2_QUALIFIED_FOR_SENDING, 
                property);

        if(sender_mgr)
        {
            sandesha2_sender_mgr_insert(sender_mgr, env, make_conn_sender_bean);
        }*/

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Sending the make connection message for the sequence with internal "\
                "sequence id %s", internal_sequence_id);

        /*status = sandesha2_utils_execute_and_store(env, make_conn_rm_msg_ctx, 
            make_conn_msg_store_key);*/

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "came10");
        status = axis2_engine_send(engine, env, make_conn_msg_ctx);

        if(AXIS2_SUCCESS != status)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]make_connection sending failed");

            if(seq_prop_mgr)
            {
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            }

            if(sender_mgr)
            {
                sandesha2_sender_mgr_free(sender_mgr, env);
            }

            if(next_msg_mgr)
            {
                sandesha2_next_msg_mgr_free(next_msg_mgr, env);
            }

            return NULL;
        }
    }

    if(seq_prop_mgr)
    {
        sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
    }

    if(sender_mgr)
    {
        sandesha2_sender_mgr_free(sender_mgr, env);
    }

    if(next_msg_mgr)
    {
        sandesha2_next_msg_mgr_free(next_msg_mgr, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[sandesha2] Exit:sandesha2_polling_mgr_worker_func");

    return NULL;
}

