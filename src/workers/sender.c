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
#include <sandesha2_msg_ctx.h>
#include <sandesha2_seq.h>
#include <axis2_addr.h>
#include <axis2_engine.h>
#include <stdlib.h>
#include <axis2_http_transport.h>
#include <axis2_http_transport_utils.h>
#include <axiom_soap_const.h>
#include <axiom_soap_fault.h>
#include <axiom_soap_body.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_msg_retrans_adjuster.h>
#include <platforms/axis2_platform_auto_sense.h>

/** 
 * @brief Sender struct impl
 *	Sandesha2 Sender Invoker
 */
typedef struct sandesha2_sender_impl sandesha2_sender_impl_t;  
typedef struct sandesha2_sender_args sandesha2_sender_args_t;

struct sandesha2_sender_impl
{
    sandesha2_sender_t sender;
	axis2_conf_ctx_t *conf_ctx;
    axis2_bool_t run_sender;
    axis2_array_list_t *working_seqs;
    axis2_thread_mutex_t *mutex;
    int counter;
};

struct sandesha2_sender_args
{
    sandesha2_sender_impl_t *impl;
    axis2_env_t *env;
};

#define SANDESHA2_INTF_TO_IMPL(sender) \
                        ((sandesha2_sender_impl_t *)(sender))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sender_for_seq
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, axis2_char_t *seq_id);
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sending (sandesha2_sender_t *sender,
                        const axis2_env_t *env);
            
axis2_bool_t AXIS2_CALL 
sandesha2_sender_is_sender_started 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run_for_seq 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axis2_conf_ctx_t *conf_ctx, 
                        axis2_char_t *seq_id);
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run (sandesha2_sender_t *sender,
                        const axis2_env_t *env);
                        
axis2_status_t AXIS2_CALL
sandesha2_sender_check_for_sync_res(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axis2_msg_ctx_t *msg_ctx);
                        
axis2_bool_t AXIS2_CALL
sandesha2_sender_is_ack_already_piggybacked(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx);
axis2_bool_t AXIS2_CALL
sandesha2_sender_is_fault_envelope(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axiom_soap_envelope_t *soap_envelope);
                        

static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(axis2_thread_t *thd, void *data);

axis2_status_t AXIS2_CALL 
sandesha2_sender_free(sandesha2_sender_t *sender, 
                        const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_sender_t* AXIS2_CALL
sandesha2_sender_create(const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    sender_impl =  (sandesha2_sender_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_sender_impl_t));
	
    if(NULL == sender_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sender_impl->conf_ctx = NULL;
    sender_impl->run_sender = AXIS2_FALSE;
    sender_impl->working_seqs = NULL;
    sender_impl->mutex = NULL;
    sender_impl->counter = 0;
    sender_impl->sender.ops = NULL;
    
    sender_impl->sender.ops = AXIS2_MALLOC(env->allocator,
                        sizeof(sandesha2_sender_ops_t));
    if(NULL == sender_impl->sender.ops)
	{
		sandesha2_sender_free(
                        (sandesha2_sender_t*)sender_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    sender_impl->working_seqs = axis2_array_list_create(env, 
                        AXIS2_ARRAY_LIST_DEFAULT_CAPACITY);
    sender_impl->mutex = axis2_thread_mutex_create(env->allocator,
                        AXIS2_THREAD_MUTEX_DEFAULT);
                        
    sender_impl->sender.ops->stop_for_seq = 
                        sandesha2_sender_stop_sender_for_seq;
    sender_impl->sender.ops->stop_sending = 
                        sandesha2_sender_stop_sending;
    sender_impl->sender.ops->is_sender_started = 
                        sandesha2_sender_is_sender_started;
    sender_impl->sender.ops->run_for_seq = 
                        sandesha2_sender_run_for_seq;
    sender_impl->sender.ops->run = sandesha2_sender_run;
    sender_impl->sender.ops->free = sandesha2_sender_free;
                        
	return &(sender_impl->sender);
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
sandesha2_sender_free(sandesha2_sender_t *sender, 
                        const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    /* Do not free this */
    sender_impl->conf_ctx = NULL;
    
    if(NULL != sender_impl->mutex)
    {
        axis2_thread_mutex_destroy(sender_impl->mutex);
        sender_impl->mutex = NULL;
    }
    if(NULL != sender_impl->working_seqs)
    {
        AXIS2_ARRAY_LIST_FREE(sender_impl->working_seqs, env);
        sender_impl->working_seqs = NULL;
    }
    if(NULL != sender->ops)
    {
        AXIS2_FREE(env->allocator, sender->ops);
        sender->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(sender));
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sender_for_seq
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, axis2_char_t *seq_id)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    int i = 0;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(sender_impl->working_seqs, env); i++)
    {
        axis2_char_t *tmp_id = NULL;
        tmp_id = AXIS2_ARRAY_LIST_GET(sender_impl->working_seqs, env, i);
        if(0 == AXIS2_STRCMP(seq_id, tmp_id))
        {
            AXIS2_ARRAY_LIST_REMOVE(sender_impl->working_seqs, env, i);
            break;
        }
    }
    if(0 == AXIS2_ARRAY_LIST_SIZE(sender_impl->working_seqs, env))
        sender_impl->run_sender = AXIS2_FALSE;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sending (sandesha2_sender_t *sender,
                        const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    SANDESHA2_INTF_TO_IMPL(sender)->run_sender = AXIS2_FALSE;
    return AXIS2_SUCCESS;
}
            
axis2_bool_t AXIS2_CALL 
sandesha2_sender_is_sender_started 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
     
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    return sender_impl->run_sender;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run_for_seq 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, axis2_conf_ctx_t *conf_ctx, 
                        axis2_char_t *seq_id)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    
    axis2_thread_mutex_lock(sender_impl->mutex);
    if(seq_id && AXIS2_FALSE == sandesha2_utils_array_list_contains(env, 
                        sender_impl->working_seqs, seq_id))
        AXIS2_ARRAY_LIST_ADD(sender_impl->working_seqs, env, seq_id);
    if(AXIS2_FALSE == sender_impl->run_sender)
    {
        sender_impl->conf_ctx = conf_ctx;
        sender_impl->run_sender = AXIS2_TRUE;
        sandesha2_sender_run(sender, env);
    }
    axis2_thread_mutex_unlock(sender_impl->mutex);
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run (
        sandesha2_sender_t *sender,
        const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    axis2_thread_t *worker_thread = NULL;
    sandesha2_sender_args_t *args = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
   
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_sender_args_t)); 
    args->impl = sender_impl;
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

axis2_status_t AXIS2_CALL
sandesha2_sender_check_for_sync_res(
    sandesha2_sender_t *sender, 
    const axis2_env_t *env, 
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_property_t *property = NULL;
    axis2_msg_ctx_t *res_msg_ctx = NULL;
    axis2_op_ctx_t *req_op_ctx = NULL;
    axiom_soap_envelope_t *res_envelope = NULL;
    axis2_char_t *soap_ns_uri = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_FALSE);
    if(NULL == property)
        return AXIS2_SUCCESS;
        
    res_msg_ctx = axis2_msg_ctx_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx,
                    env), AXIS2_MSG_CTX_GET_TRANSPORT_IN_DESC(
                    msg_ctx, env), AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx,
                    env));
    AXIS2_MSG_CTX_SET_SERVER_SIDE(res_msg_ctx, env, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_FALSE), AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_SVC_CTX(res_msg_ctx, env, AXIS2_MSG_CTX_GET_SVC_CTX(
                    msg_ctx, env));
    AXIS2_MSG_CTX_SET_SVC_GRP_CTX(res_msg_ctx, env, 
                    AXIS2_MSG_CTX_GET_SVC_GRP_CTX(msg_ctx, env));
    req_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env);
    if(NULL != req_op_ctx)
    {
        axis2_ctx_t *ctx = NULL;
        
        ctx = AXIS2_OP_CTX_GET_BASE(req_op_ctx, env);
        if(NULL != AXIS2_CTX_GET_PROPERTY(ctx, env, MTOM_RECIVED_CONTENT_TYPE, 
                    AXIS2_FALSE))
        {
            AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, 
                    MTOM_RECIVED_CONTENT_TYPE, AXIS2_CTX_GET_PROPERTY(ctx, env, 
                    MTOM_RECIVED_CONTENT_TYPE, AXIS2_FALSE), AXIS2_FALSE);
        }
        if(NULL != AXIS2_CTX_GET_PROPERTY(ctx, env, AXIS2_HTTP_CHAR_SET_ENCODING, 
                    AXIS2_FALSE))
        {
            AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, 
                    AXIS2_HTTP_CHAR_SET_ENCODING, AXIS2_CTX_GET_PROPERTY(ctx, env, 
                    AXIS2_HTTP_CHAR_SET_ENCODING, AXIS2_FALSE), AXIS2_FALSE);
        }
    }
    AXIS2_MSG_CTX_SET_DOING_REST(res_msg_ctx, env, AXIS2_MSG_CTX_GET_DOING_REST(
                    msg_ctx, env));
    soap_ns_uri = AXIS2_MSG_CTX_GET_IS_SOAP_11(msg_ctx, env) ?
                    AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
                    AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;

    res_envelope = axis2_http_transport_utils_create_soap_msg(env, msg_ctx,
                    soap_ns_uri);
   
    AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, SANDESHA2_WITHIN_TRANSACTION,
                    AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env,
                    SANDESHA2_WITHIN_TRANSACTION, AXIS2_FALSE), AXIS2_FALSE);
    if(NULL != res_envelope)
    {
        axis2_engine_t *engine = NULL;
        AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(res_msg_ctx, env, res_envelope);
        
        engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, 
                    env));
        if(AXIS2_TRUE == sandesha2_sender_is_fault_envelope(sender, env, 
                    res_envelope))
            AXIS2_ENGINE_RECEIVE_FAULT(engine, env, res_msg_ctx);
        else
            AXIS2_ENGINE_RECEIVE(engine, env, res_msg_ctx);        
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_is_piggybackable_msg_type(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        int msg_type)
{
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(SANDESHA2_MSG_TYPE_ACK == msg_type)
        return AXIS2_FALSE;
    
    return AXIS2_TRUE;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_is_ack_already_piggybacked(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    if(NULL != SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT))
        return AXIS2_TRUE;
    
    return AXIS2_FALSE;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_is_fault_envelope(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axiom_soap_envelope_t *soap_envelope)
{
    axiom_soap_fault_t *fault = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    
    fault = AXIOM_SOAP_BODY_GET_FAULT(AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope,
                        env), env);
    if(NULL != fault)
        return AXIS2_TRUE;
        
    return AXIS2_FALSE;
}

/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(
        axis2_thread_t *thd, 
        void *data)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    sandesha2_sender_t *sender = NULL;
    sandesha2_sender_args_t *args;
    axis2_env_t *env = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    
    args = (sandesha2_sender_args_t*)data;
    env = axis2_init_thread_env(args->env);
    sender_impl = args->impl;
    sender = (sandesha2_sender_t*)sender_impl;
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, 
                        sender_impl->conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(sender_impl->conf_ctx, env));
                        
    while(AXIS2_TRUE == sender_impl->run_sender)
    {
        sandesha2_transaction_t *transaction = NULL;
        /* Use when transaction handling is done 
        axis2_bool_t rollbacked = AXIS2_FALSE;*/
        sandesha2_sender_mgr_t *mgr = NULL;
        sandesha2_sender_bean_t *sender_bean = NULL;
        sandesha2_sender_bean_t *bean1 = NULL;
        sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
        axis2_char_t *key = NULL;
        axis2_msg_ctx_t *msg_ctx = NULL;
        axis2_property_t *property = NULL;
        axis2_bool_t continue_sending = AXIS2_TRUE;
        axis2_char_t *qualified_for_sending = NULL;
        sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
        sandesha2_property_bean_t *prop_bean = NULL;
        axis2_array_list_t *msgs_not_to_send = NULL;
        int msg_type = -1;
        axis2_transport_out_desc_t *transport_out = NULL;
        axis2_transport_sender_t *transport_sender = NULL;
        axis2_bool_t successfully_sent = AXIS2_FALSE;
        axis2_char_t *msg_id = NULL;
   
        AXIS2_SLEEP(1); 
        transaction = sandesha2_storage_mgr_get_transaction(storage_mgr,
                        env);
        mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
        seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(
                        storage_mgr, env);
        sender_bean = SANDESHA2_SENDER_MGR_GET_NEXT_MSG_TO_SEND(mgr, env);
        if(!sender_bean)
        {
            continue;
        }
            
        key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(sender_bean, env);
        msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, key, 
                        sender_impl->conf_ctx);
        if(!msg_ctx)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] msg_ctx is "
                        "not present in the store");
            break;
        }
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_TRUE, env));
        AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, SANDESHA2_WITHIN_TRANSACTION,
                        property, AXIS2_FALSE);
        continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env,
                        sender_bean, sender_impl->conf_ctx, storage_mgr);
        if(!continue_sending)
        {
            continue;
        }
        
        property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_QUALIFIED_FOR_SENDING, AXIS2_FALSE);
        if(property)
            qualified_for_sending = AXIS2_PROPERTY_GET_VALUE(property, env);
            
        if(qualified_for_sending && 0 != AXIS2_STRCMP(
                        qualified_for_sending, SANDESHA2_VALUE_TRUE))
            continue;
        rm_msg_ctx = sandesha2_msg_init_init_msg(env, msg_ctx);
        
        prop_bean = sandesha2_utils_get_property_bean_from_op(env, 
                        AXIS2_MSG_CTX_GET_OP(msg_ctx, env));
        if(prop_bean)
            msgs_not_to_send = sandesha2_property_bean_get_msg_types_to_drop(
                        prop_bean, env);
        if(msgs_not_to_send)
        {
            int j = 0;
            axis2_bool_t continue_sending = AXIS2_FALSE;

            for(j = 0; j < AXIS2_ARRAY_LIST_SIZE(msgs_not_to_send, env); j++)
            {
                axis2_char_t *value = NULL;
                int int_val = -1;
                int msg_type = -1;
                
                value = AXIS2_ARRAY_LIST_GET(msgs_not_to_send, env, j);
                int_val = atoi(value);
                msg_type = SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, env);
                if(msg_type == int_val)
                    continue_sending = AXIS2_TRUE;
            }
            if(continue_sending)
                continue;
        }
        /* 
         *   This method is not implemented yet
         *  update_msg(sender, env, msg_xtx);
         */
        msg_type = SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, env);
        if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
        {
            sandesha2_seq_t *seq = NULL;
            axis2_char_t *seq_id = NULL;
            sandesha2_identifier_t *identifier = NULL;
            
            seq = (sandesha2_seq_t*)
                        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, 
                        env, SANDESHA2_MSG_PART_SEQ);
            identifier = sandesha2_seq_get_identifier(seq, env);
            seq_id = sandesha2_identifier_get_identifier(identifier, env);
        }
        
        if(sandesha2_sender_is_piggybackable_msg_type(sender, env,
                        msg_type) && AXIS2_FALSE  == 
                        sandesha2_sender_is_ack_already_piggybacked(sender, env,
                        rm_msg_ctx))
        {
            sandesha2_ack_mgr_piggyback_acks_if_present(env, rm_msg_ctx, 
                        storage_mgr);
        }
        
        
        transport_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx, env);
        transport_sender = AXIS2_TRANSPORT_OUT_DESC_GET_SENDER(transport_out, 
                        env);
        if(transport_sender)
        {
            SANDESHA2_TRANSACTION_COMMIT(transaction, env);
            property = axis2_property_create(env);
            AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
            AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_FALSE, env));
            AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_WITHIN_TRANSACTION, property, AXIS2_FALSE);
            /* Consider building soap envelope */
            AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, msg_ctx);
            successfully_sent = AXIS2_TRUE;
            sender_impl->counter++;
            /*printf("**********************counter******************:%d\n", sender_impl->counter);
            if(2 == sender_impl->counter)
            sleep(300000);*/
                        
        }
        transaction = sandesha2_storage_mgr_get_transaction(storage_mgr,
                        env);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_TRUE, env));
        AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_WITHIN_TRANSACTION, property, AXIS2_FALSE);
        msg_id = SANDESHA2_SENDER_BEAN_GET_MSG_ID(sender_bean, env);
        bean1 = SANDESHA2_SENDER_MGR_RETRIEVE(mgr, env, msg_id);
        if(bean1)
        {
            axis2_bool_t resend = AXIS2_FALSE;
            
            resend = SANDESHA2_SENDER_BEAN_IS_RESEND(sender_bean, env);
            if(AXIS2_TRUE == resend)
            {
                SANDESHA2_SENDER_BEAN_SET_SENT_COUNT(bean1, env, 
                        SANDESHA2_SENDER_BEAN_GET_SENT_COUNT(sender_bean, env));
                SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(bean1, env, 
                        SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(sender_bean, env));
                SANDESHA2_SENDER_MGR_UPDATE(mgr, env, bean1);
            }
            else
            {
                axis2_char_t *msg_stored_key = NULL;
                
                msg_id = SANDESHA2_SENDER_BEAN_GET_MSG_ID(bean1, env); 
                SANDESHA2_SENDER_MGR_REMOVE(mgr, env, msg_id);
                /* Removing the message from the storage */
                msg_stored_key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(
                    bean1, env);
                sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
                    msg_stored_key);
            }
        }
        if(successfully_sent)
        {
            if(AXIS2_FALSE == AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env))
                sandesha2_sender_check_for_sync_res(sender, env, msg_ctx);
        }
        if(SANDESHA2_MSG_TYPE_TERMINATE_SEQ == SANDESHA2_MSG_CTX_GET_MSG_TYPE(
                        rm_msg_ctx, env))
        {
            sandesha2_terminate_seq_t *terminate_seq = NULL;
            axis2_char_t *seq_id = NULL;
            axis2_conf_ctx_t *conf_ctx = NULL;
            axis2_char_t *int_seq_id = NULL;
            
            terminate_seq = (sandesha2_terminate_seq_t*)
                        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_TERMINATE_SEQ);
            seq_id = sandesha2_identifier_get_identifier(
                        sandesha2_terminate_seq_get_identifier(terminate_seq, 
                        env), env);
            conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
            int_seq_id = sandesha2_utils_get_seq_property(env, seq_id, 
                        SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID, storage_mgr);
            sandesha2_terminate_mgr_terminate_sending_side(env, conf_ctx,
                        int_seq_id, AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env), 
                        storage_mgr);
        }
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_FALSE, env));
        AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_WITHIN_TRANSACTION, property, AXIS2_FALSE);
        /* TODO make transaction handling effective */
        if(transaction)
        {
            SANDESHA2_TRANSACTION_COMMIT(transaction, env);
        }
    }
    axis2_env_free(env); 
    
    return NULL;
}

