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
#include <sandesha2_transaction.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_seq.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_msg_creator.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_sender_bean.h>
#include <sandesha2_sender_mgr.h>
#include <axis2_addr.h>
#include <axis2_engine.h>
#include <axis2_uuid_gen.h>
#include <axis2_rand.h>
#include <stdio.h>
#include <platforms/axis2_platform_auto_sense.h>


/** 
 * @brief Polling Manager struct impl
 *	Sandesha2 Polling Manager
 */
typedef struct sandesha2_polling_mgr_args sandesha2_polling_mgr_args_t;

struct sandesha2_polling_mgr_t
{
	axis2_conf_ctx_t *conf_ctx;
    /**
     * By adding an entry to this, the polling_mgr will be asked to do a polling 
     * request on this sequence.
     */
    axis2_array_list_t *scheduled_polling_reqs;
    axis2_bool_t poll;
    axis2_thread_mutex_t *mutex;
};

struct sandesha2_polling_mgr_args
{
    sandesha2_polling_mgr_t *impl;
    axis2_env_t *env;
    sandesha2_storage_mgr_t *storage_mgr;
};
            
static axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_run (
    sandesha2_polling_mgr_t *polling_mgr,
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr);

/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_polling_mgr_worker_func(
    axis2_thread_t *thd, 
    void *data);

AXIS2_EXTERN sandesha2_polling_mgr_t* AXIS2_CALL
sandesha2_polling_mgr_create(
    const axis2_env_t *env)
{
    sandesha2_polling_mgr_t *polling_mgr = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    polling_mgr =  (sandesha2_polling_mgr_t *)AXIS2_MALLOC 
        (env->allocator, 
        sizeof(sandesha2_polling_mgr_t));
	
    if(!polling_mgr)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    polling_mgr->scheduled_polling_reqs = NULL;
    polling_mgr->poll = AXIS2_FALSE; 
    polling_mgr->mutex = axis2_thread_mutex_create(env->allocator,
        AXIS2_THREAD_MUTEX_DEFAULT);
                        
	return polling_mgr;
}

axis2_status_t AXIS2_CALL
sandesha2_polling_mgr_free_void_arg(
    void *polling_mgr,
    const axis2_env_t *env)
{
    sandesha2_polling_mgr_t *polling_mgr_l = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    polling_mgr_l = (sandesha2_polling_mgr_t *) polling_mgr;
    return sandesha2_polling_mgr_free(polling_mgr_l, env);
}

axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_free(
    sandesha2_polling_mgr_t *polling_mgr, 
    const axis2_env_t *env)
{
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    /* Do not free this */
    polling_mgr->conf_ctx = NULL;
    
    if(polling_mgr->mutex)
    {
        axis2_thread_mutex_destroy(polling_mgr->mutex);
        polling_mgr->mutex = NULL;
    }
    if(polling_mgr->scheduled_polling_reqs)
    {
        AXIS2_ARRAY_LIST_FREE(polling_mgr->scheduled_polling_reqs, env);
        polling_mgr->scheduled_polling_reqs = NULL;
    }
	AXIS2_FREE(env->allocator, polling_mgr);
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_stop_polling (
    sandesha2_polling_mgr_t *polling_mgr,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    sandesha2_polling_mgr_set_poll(polling_mgr, env, AXIS2_FALSE);
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_start (
    sandesha2_polling_mgr_t *polling_mgr, 
    const axis2_env_t *env, 
    axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    
    axis2_thread_mutex_lock(polling_mgr->mutex);
    polling_mgr->conf_ctx = conf_ctx;
    polling_mgr->scheduled_polling_reqs = axis2_array_list_create(env, 
        AXIS2_ARRAY_LIST_DEFAULT_CAPACITY);

    if(!polling_mgr->conf_ctx)
    {
        axis2_thread_mutex_unlock(polling_mgr->mutex);
        return AXIS2_FAILURE;
    }
    storage_mgr = sandesha2_utils_get_storage_mgr(env, 
        polling_mgr->conf_ctx, 
        AXIS2_CONF_CTX_GET_CONF(polling_mgr->conf_ctx, env));
    sandesha2_polling_mgr_set_poll(polling_mgr, env, AXIS2_TRUE);
    sandesha2_polling_mgr_run(polling_mgr, env, storage_mgr);
    axis2_thread_mutex_unlock(polling_mgr->mutex);
    return AXIS2_SUCCESS;
}
            
static axis2_status_t AXIS2_CALL 
sandesha2_polling_mgr_run (
    sandesha2_polling_mgr_t *polling_mgr,
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_thread_t *worker_thread = NULL;
    sandesha2_polling_mgr_args_t *args = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    args = AXIS2_MALLOC(env->allocator, sizeof(
                        sandesha2_polling_mgr_args_t)); 
    args->impl = polling_mgr;
    args->env = (axis2_env_t*)env;
    args->storage_mgr = storage_mgr;
    worker_thread = AXIS2_THREAD_POOL_GET_THREAD(env->thread_pool,
        sandesha2_polling_mgr_worker_func, (void*)args);
    if(!worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Thread creation failed" \
            " sandesha2_polling_mgr_run");
        return AXIS2_FAILURE;
    }
    AXIS2_THREAD_POOL_THREAD_DETACH(env->thread_pool, worker_thread); 
        
    return AXIS2_SUCCESS;
}

/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_polling_mgr_worker_func(
    axis2_thread_t *thd, 
    void *data)
{
    sandesha2_polling_mgr_t *polling_mgr = NULL;
    sandesha2_polling_mgr_args_t *args;
    sandesha2_storage_mgr_t *storage_mgr;
    axis2_env_t *env = NULL;
    
    args = (sandesha2_polling_mgr_args_t*)data;
    env = args->env;
    polling_mgr = args->impl;
    storage_mgr = args->storage_mgr;

    while(polling_mgr->poll)
    {
        sandesha2_next_msg_mgr_t *next_msg_mgr = NULL;
        sandesha2_next_msg_bean_t *next_msg_bean = NULL;
        sandesha2_msg_ctx_t *ref_rm_msg_ctx = NULL;
        sandesha2_msg_ctx_t *make_conn_rm_msg_ctx = NULL;
        sandesha2_sender_bean_t *make_conn_sender_bean = NULL;
        sandesha2_sender_mgr_t *sender_bean_mgr = NULL;
        int size = 0;
        axis2_char_t *seq_id = NULL;
        axis2_char_t *ref_msg_key = NULL;
        axis2_char_t *seq_prop_key = NULL;
        axis2_char_t *reply_to = NULL;
        axis2_char_t *wsrm_anon_reply_to_uri = NULL;
        axis2_char_t *make_conn_msg_store_key = NULL;
        axis2_char_t *msg_id = NULL;
        axis2_msg_ctx_t *ref_msg_ctx = NULL;
        axis2_msg_ctx_t *make_conn_msg_ctx = NULL;
        axis2_endpoint_ref_t *to = NULL;
        axis2_property_t *property = NULL;

        printf("came11\n");
        AXIS2_SLEEP(SANDESHA2_POLLING_MANAGER_WAIT_TIME);
        printf("came12\n");
        next_msg_mgr = sandesha2_storage_mgr_get_next_msg_mgr(
                        storage_mgr, env);
         /* Geting the sequences to be polled. if schedule contains any requests, 
          * do the earliest one. else pick one randomly.
          */
        if(polling_mgr->scheduled_polling_reqs)
            size = AXIS2_ARRAY_LIST_SIZE(polling_mgr->scheduled_polling_reqs, 
                env);
        if(size > 0)
        {
            seq_id = AXIS2_ARRAY_LIST_GET(polling_mgr->scheduled_polling_reqs, 
                env, 0);
            AXIS2_ARRAY_LIST_REMOVE(polling_mgr->scheduled_polling_reqs, env, 0);
        }
        if(!seq_id)
        {
            printf("came13\n");
            sandesha2_next_msg_bean_t *find_bean = 
                sandesha2_next_msg_bean_create(env);
            int size = 0;
            if(find_bean)
            {
                axis2_array_list_t *results = NULL;
                sandesha2_next_msg_bean_set_polling_mode(find_bean, env, 
                    AXIS2_TRUE);
                results = sandesha2_next_msg_mgr_find(next_msg_mgr, env, 
                    find_bean);
                if(results)
                    size = AXIS2_ARRAY_LIST_SIZE(results, env);
                printf("size:%d\n", size);
                if(size > 0)
                {
                    unsigned int rand_var = 
                        axis2_rand_get_seed_value_based_on_time(env);
                    int item = axis2_rand_with_range(&rand_var, 0, size);
                    item--;
                    next_msg_bean = (sandesha2_next_msg_bean_t *) 
                        AXIS2_ARRAY_LIST_GET(results, env, item);
                }

            }
        }
        else
        {
            printf("came14\n");
            sandesha2_next_msg_bean_t *find_bean = 
                sandesha2_next_msg_bean_create(env);
            if(find_bean)
            {
                sandesha2_next_msg_bean_set_polling_mode(find_bean, env, 
                    AXIS2_TRUE);
                sandesha2_next_msg_bean_set_seq_id(find_bean, env, seq_id);
                next_msg_bean = sandesha2_next_msg_mgr_find_unique(next_msg_mgr,
                    env, find_bean);
            }
        }
        /* If no valid entry is found, try again later */
        printf("came15\n");
        if(!next_msg_bean)
            continue;
        printf("came16\n");
        seq_id = sandesha2_next_msg_bean_get_seq_id((sandesha2_rm_bean_t *) 
            next_msg_bean, env);
        /* Create a MakeConnection message */
        ref_msg_key = sandesha2_next_msg_bean_get_ref_msg_key(next_msg_bean, env);
        seq_prop_key = seq_id;
        reply_to = sandesha2_utils_get_seq_property(env, seq_prop_key, 
            SANDESHA2_SEQ_PROP_REPLY_TO_EPR, storage_mgr);
        if(sandesha2_utils_is_wsrm_anon_reply_to(env, reply_to))
            wsrm_anon_reply_to_uri = reply_to;
        ref_msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, 
            ref_msg_key, polling_mgr->conf_ctx);
        if(ref_msg_ctx)
            ref_rm_msg_ctx = sandesha2_msg_init_init_msg(env, ref_msg_ctx);
        make_conn_rm_msg_ctx = 
            sandesha2_msg_creator_create_make_connection_msg(env, 
            ref_rm_msg_ctx, seq_id, wsrm_anon_reply_to_uri, storage_mgr);
        if(!make_conn_rm_msg_ctx)
        {
            printf("came12\n");
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, 
            AXIS2_TRANSPORT_IN, NULL);
        /* Storing the MakeConnection message */
        make_conn_msg_store_key = axis2_uuid_gen(env);
        property = axis2_property_create_with_args(env, 0, 0, 0, seq_prop_key);
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
            sandesha2_sender_bean_set_resend(make_conn_sender_bean, env, AXIS2_FALSE);
            sandesha2_sender_bean_set_send(make_conn_sender_bean, env, AXIS2_TRUE);
            sandesha2_sender_bean_set_seq_id(make_conn_sender_bean, env, seq_id);
            to = sandesha2_msg_ctx_get_to(make_conn_rm_msg_ctx, env);
            if(to)
            {
                axis2_char_t *address = (axis2_char_t *) 
                    AXIS2_ENDPOINT_REF_GET_ADDRESS(
                    (const axis2_endpoint_ref_t *) to, env);
                sandesha2_sender_bean_set_to_address(make_conn_sender_bean, env, 
                    address);
            }
        }
        sender_bean_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, 
            env);
        /* This message should not be sent untils it is qualified. i.e. Till
         * it is sent through the sandesha2_transport_sender
         */
        property = axis2_property_create_with_args(env, 0, 0, 0, 
            SANDESHA2_VALUE_FALSE);
        sandesha2_msg_ctx_set_property(make_conn_rm_msg_ctx, env, 
            SANDESHA2_QUALIFIED_FOR_SENDING, property);
        if(sender_bean_mgr)
        {
            sandesha2_sender_mgr_insert(sender_bean_mgr, env, 
                make_conn_sender_bean);
        }
        sandesha2_utils_execute_and_store(env, make_conn_rm_msg_ctx, 
            make_conn_msg_store_key);
    }
    return NULL;
}

void AXIS2_CALL
sandesha2_polling_mgr_set_poll(
    sandesha2_polling_mgr_t *polling_mgr,
    const axis2_env_t *env,
    axis2_bool_t poll)
{
    polling_mgr->poll = poll;
}

axis2_bool_t AXIS2_CALL
sandesha2_polling_mgr_is_poll(
    sandesha2_polling_mgr_t *polling_mgr,
    const axis2_env_t *env)
{
    return polling_mgr->poll;
}

void AXIS2_CALL
sandesha2_polling_mgr_schedule_polling_request(
    sandesha2_polling_mgr_t *polling_mgr,
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id)
{
    if(!AXIS2_ARRAY_LIST_CONTAINS(polling_mgr->scheduled_polling_reqs, env, 
        internal_seq_id))
    {
        AXIS2_ARRAY_LIST_ADD(polling_mgr->scheduled_polling_reqs, env, 
            internal_seq_id);
    }
}

