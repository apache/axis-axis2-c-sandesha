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
#include <platforms/axutil_platform_auto_sense.h>
#include <axutil_types.h>

/** 
 * @brief Sender struct impl
 *	Sandesha2 Sender Invoker
 */
typedef struct sandesha2_sender_args sandesha2_sender_args_t;

struct sandesha2_sender_t
{
	axis2_conf_ctx_t *conf_ctx;
    axis2_bool_t run_sender;
    axis2_char_t *seq_id;
    axis2_bool_t persistent_msg_ctx;
};

struct sandesha2_sender_args
{
    sandesha2_sender_t *impl;
    axutil_env_t *env;
};

static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(
    axutil_thread_t *thd, 
    void *data);


AXIS2_EXTERN sandesha2_sender_t* AXIS2_CALL
sandesha2_sender_create(
    const axutil_env_t *env)
{
    sandesha2_sender_t *sender = NULL;
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
    sender->seq_id = NULL;
    
	return sender;
}

axis2_status_t AXIS2_CALL
sandesha2_sender_free_void_arg(
    void *sender,
    const axutil_env_t *env)
{
    sandesha2_sender_t *sender_l = NULL;

    sender_l = (sandesha2_sender_t *) sender;
    return sandesha2_sender_free(sender_l, env);
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_free(
    sandesha2_sender_t *sender, 
    const axutil_env_t *env)
{
    sender->conf_ctx = NULL;
	AXIS2_FREE(env->allocator, sender);
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sending(
    sandesha2_sender_t *sender,
    const axutil_env_t *env)
{
    sender->run_sender = AXIS2_FALSE;
    sandesha2_sender_free(sender, env);
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run_for_seq(
    sandesha2_sender_t *sender, 
    const axutil_env_t *env, 
    axis2_conf_ctx_t *conf_ctx, 
    axis2_char_t *seq_id,
    const axis2_bool_t persistent)
{
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Entry:sandesha2_sender_run_for_seq");
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
   
    sender->conf_ctx = conf_ctx;
    sender->run_sender = AXIS2_TRUE;
    sender->persistent_msg_ctx = persistent;
    sender->seq_id = seq_id;
    sandesha2_sender_run(sender, env);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Exit:sandesha2_sender_run_for_seq");
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run (
    sandesha2_sender_t *sender,
    const axutil_env_t *env)
{
    axutil_thread_t *worker_thread = NULL;
    sandesha2_sender_args_t *args = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Entry:sandesha2_sender_run");
    
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_sender_args_t)); 
    args->impl = sender;
    args->env = (axutil_env_t*)env;

    worker_thread = axutil_thread_pool_get_thread(env->thread_pool,
        sandesha2_sender_worker_func, (void*)args);
    if(!worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Thread creation "\
            "failed for sandesha2_sender_run");
        return AXIS2_FAILURE;
    }
    axutil_thread_pool_thread_detach(env->thread_pool, worker_thread);     
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Exit:sandesha2_sender_run");
    return AXIS2_SUCCESS;
}

/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(
    axutil_thread_t *thd, 
    void *data)
{
    sandesha2_sender_t *sender = NULL;
    sandesha2_sender_args_t *args;
    axutil_env_t *env = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_conf_t *conf = NULL;
    axis2_module_desc_t *module_desc = NULL;
    int sleep_time = 0;
    axutil_qname_t *qname = NULL;
    axutil_param_t *sleep_time_param = NULL;
    axis2_char_t *seq_id = NULL;
    
    args = (sandesha2_sender_args_t*)data;
    env = axutil_init_thread_env(args->env);
    sender = args->impl;
    sender = (sandesha2_sender_t*)sender;
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Entry:sandesha2_sender_worker_func");
    conf = axis2_conf_ctx_get_conf(sender->conf_ctx, env);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, sender->conf_ctx, 
        conf);
    qname = axutil_qname_create(env, SANDESHA2_MODULE, NULL, NULL);
    module_desc = axis2_conf_get_module(conf, env, qname);
    sleep_time_param = axis2_module_desc_get_param(module_desc, env, 
        SANDESHA2_SENDER_SLEEP);
    if(sleep_time_param)
    {
        sleep_time = AXIS2_ATOI(axutil_param_get_value(sleep_time_param, 
            env));
    }
    axutil_qname_free(qname, env);
    seq_id = sender->seq_id;
    while(sender->run_sender)
    {
        sandesha2_sender_mgr_t *sender_mgr = NULL;
        sandesha2_sender_bean_t *sender_bean = NULL;
        axis2_char_t *msg_id = NULL;

        seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(
            storage_mgr, env);
        sender_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
        sender_bean = sandesha2_sender_mgr_get_next_msg_to_send(sender_mgr, env, 
            seq_id);
        if(!sender_bean)
        {
            AXIS2_USLEEP(sleep_time);
            continue;
        }
        msg_id = sandesha2_sender_bean_get_msg_id((sandesha2_rm_bean_t *) 
            sender_bean, env);
        if(msg_id)
        {
            axis2_bool_t status = AXIS2_TRUE;
            status = sandesha2_sender_worker_send(env, sender->conf_ctx, msg_id, 
                sender->persistent_msg_ctx);
            if(!status)
            {
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                    "[sandesha2]Stopping the sender for sequence %s", seq_id);
                sandesha2_sender_stop_sending(sender, env);
            }
        }
    }
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Exit:sandesha2_sender_worker_func");
    #ifdef AXIS2_SVR_MULTI_THREADED
        AXIS2_THREAD_POOL_EXIT_THREAD(env->thread_pool, thd);
    #endif
    return NULL;
}

