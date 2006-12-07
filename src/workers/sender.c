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
#include <sandesha2_sender.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_transaction.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_seq.h>
#include <sandesha2_sender_worker.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_sender_worker.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_msg_retrans_adjuster.h>
#include <axis2_addr.h>
#include <axis2_engine.h>
#include <stdlib.h>
#include <axis2_http_transport.h>
#include <axis2_http_transport_utils.h>
#include <axiom_soap_const.h>
#include <axiom_soap_fault.h>
#include <axiom_soap_body.h>
#include <platforms/axis2_platform_auto_sense.h>

/** 
 * @brief Sender struct impl
 *	Sandesha2 Sender Invoker
 */
typedef struct sandesha2_sender_args sandesha2_sender_args_t;

struct sandesha2_sender_t
{
	axis2_conf_ctx_t *conf_ctx;
    axis2_bool_t run_sender;
    axis2_array_list_t *working_seqs;
    axis2_thread_mutex_t *mutex;
    int counter;
};

struct sandesha2_sender_args
{
    sandesha2_sender_t *impl;
    axis2_env_t *env;
};

static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(
    axis2_thread_t *thd, 
    void *data);


AXIS2_EXTERN sandesha2_sender_t* AXIS2_CALL
sandesha2_sender_create(
    const axis2_env_t *env)
{
    sandesha2_sender_t *sender = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    sender =  (sandesha2_sender_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_sender_t));
	
    if(NULL == sender)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sender->conf_ctx = NULL;
    sender->run_sender = AXIS2_FALSE;
    sender->working_seqs = NULL;
    sender->mutex = NULL;
    sender->counter = 0;
    
    sender->working_seqs = axis2_array_list_create(env, 
                        AXIS2_ARRAY_LIST_DEFAULT_CAPACITY);
    sender->mutex = axis2_thread_mutex_create(env->allocator,
                        AXIS2_THREAD_MUTEX_DEFAULT);
                        
	return sender;
}

axis2_status_t AXIS2_CALL
sandesha2_sender_free_void_arg(
    void *sender,
    const axis2_env_t *env)
{
    sandesha2_sender_t *sender_l = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    sender_l = (sandesha2_sender_t *) sender;
    return sandesha2_sender_free(sender_l, env);
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_free(
    sandesha2_sender_t *sender, 
    const axis2_env_t *env)
{
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    /* Do not free this */
    sender->conf_ctx = NULL;
    
    if(sender->mutex)
    {
        axis2_thread_mutex_destroy(sender->mutex);
        sender->mutex = NULL;
    }
    if(sender->working_seqs)
    {
        AXIS2_ARRAY_LIST_FREE(sender->working_seqs, env);
        sender->working_seqs = NULL;
    }
	AXIS2_FREE(env->allocator, sender);
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sender_for_seq(
    sandesha2_sender_t *sender, 
    const axis2_env_t *env, axis2_char_t *seq_id)
{
    int i = 0;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(sender->working_seqs, env); i++)
    {
        axis2_char_t *tmp_id = NULL;
        tmp_id = AXIS2_ARRAY_LIST_GET(sender->working_seqs, env, i);
        if(0 == AXIS2_STRCMP(seq_id, tmp_id))
        {
            AXIS2_ARRAY_LIST_REMOVE(sender->working_seqs, env, i);
            break;
        }
    }
    if(0 == AXIS2_ARRAY_LIST_SIZE(sender->working_seqs, env))
        sender->run_sender = AXIS2_FALSE;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sending (
    sandesha2_sender_t *sender,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    sender->run_sender = AXIS2_FALSE;
    return AXIS2_SUCCESS;
}
            
axis2_bool_t AXIS2_CALL 
sandesha2_sender_is_sender_started(
    sandesha2_sender_t *sender, 
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    return sender->run_sender;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run_for_seq(
    sandesha2_sender_t *sender, 
    const axis2_env_t *env, 
    axis2_conf_ctx_t *conf_ctx, 
    axis2_char_t *seq_id)
{
    axis2_thread_mutex_lock(sender->mutex);
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
   
    if(seq_id && !sandesha2_utils_array_list_contains(env, 
                        sender->working_seqs, seq_id))
        AXIS2_ARRAY_LIST_ADD(sender->working_seqs, env, seq_id);
    if(!sender->run_sender)
    {
        sender->conf_ctx = conf_ctx;
        sender->run_sender = AXIS2_TRUE;
        sandesha2_sender_run(sender, env);
    }
    axis2_thread_mutex_unlock(sender->mutex);
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run (
    sandesha2_sender_t *sender,
    const axis2_env_t *env)
{
    axis2_thread_t *worker_thread = NULL;
    sandesha2_sender_args_t *args = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_sender_args_t)); 
    args->impl = sender;
    args->env = (axis2_env_t*)env;

    worker_thread = AXIS2_THREAD_POOL_GET_THREAD(env->thread_pool,
                        sandesha2_sender_worker_func, (void*)args);
    if(NULL == worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Thread creation "
                        "failed sandesha2_sender_run");
        return AXIS2_FAILURE;
    }
    AXIS2_THREAD_POOL_THREAD_DETACH(env->thread_pool, worker_thread); 
        
    return AXIS2_SUCCESS;
}

/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(
    axis2_thread_t *thd, 
    void *data)
{
    sandesha2_sender_t *sender = NULL;
    sandesha2_sender_args_t *args;
    axis2_env_t *env = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    
    args = (sandesha2_sender_args_t*)data;
    env = axis2_init_thread_env(args->env);
    sender = args->impl;
    sender = (sandesha2_sender_t*)sender;
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, sender->conf_ctx, 
        AXIS2_CONF_CTX_GET_CONF(sender->conf_ctx, env));
                        
    while(sender->run_sender)
    {
        sandesha2_transaction_t *transaction = NULL;
        /* Use when transaction handling is done 
        axis2_bool_t rollbacked = AXIS2_FALSE;*/
        sandesha2_sender_mgr_t *mgr = NULL;
        sandesha2_sender_bean_t *sender_bean = NULL;
        sandesha2_sender_worker_t *sender_worker = NULL;
        axis2_char_t *msg_id = NULL;
   
        AXIS2_SLEEP(SANDESHA2_SENDER_SLEEP_TIME); 
        transaction = sandesha2_storage_mgr_get_transaction(storage_mgr,
                        env);
        mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
        seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(
                        storage_mgr, env);
        sender_bean = sandesha2_sender_mgr_get_next_msg_to_send(mgr, env);
        if(!sender_bean)
        {
            continue;
        }
        msg_id = sandesha2_sender_bean_get_msg_id(sender_bean, env);
        if(msg_id)
        {
        /* Start a sender worker which will work on this message */
            sender_worker = sandesha2_sender_worker_create(env, sender->conf_ctx, 
                msg_id);
            sandesha2_sender_worker_run(sender_worker, env);
        }
    }
    #ifdef AXIS2_SVR_MULTI_THREADED
        AXIS2_THREAD_POOL_EXIT_THREAD(env->thread_pool, thd);
    #endif
    return NULL;
}
   
