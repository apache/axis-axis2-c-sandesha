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
 
#include <sandesha2/sandesha2_app_msg_processor.h>
#include <sandesha2/sandesa2_seq_ack.h>
/** 
 * @brief Application Message Processor struct impl
 *	Sandesha2 App Msg Processor
 */
typedef struct sandesha2_app_msg_processor_impl sandesha2_app_msg_processor_impl_t;  
  
struct sandesha2_app_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(identifier) \
						((sandesha2_app_msg_processor_impl_t *)(identifier))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx);
    
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *msg_ctx);
    
axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_msg_num_is_in_list(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_char_t *list,
                        long num);
                    	
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_send_ack_if_reqd(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *msg_str,
                        sandesha2_storage_manager_t *manager);
                    	
axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *internal_seq_id,
                        axis2_char_t *acks_to,
                        sandesha2_storage_manager_t *manager);

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        axis2_char_t *storage_key,
                        sandesha2_storage_manager_t *manager);

long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        sandesha2_storage_manager_t *manager);

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        sandesha2_storage_manager_t *manager);
                        
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_DECLARE(sandesha2_msg_processor_t*)
sandesha2_app_msg_processor_create(const axis2_env_t *env,  
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, NULL);
          
    msg_proc_impl =  (sandesha2_app_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_app_msg_processor_impl_t));
	
    if(NULL == msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(NULL == msg_proc_impl->msg_processor.ops)
	{
		sandesha2_app_msg_processor_free((sandesha2_msg_processor_t*)
                         msg_proc_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops->process_in_msg = 
                        sandesha2_app_msg_processor_process_in_msg;
    msg_proc_impl->msg_processor.ops->process_out_msg = 
    					sandesha2_app_msg_processor_fprocess_out_msg;
    msg_proc_impl->msg_processor.ops->free = sandesha2_app_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (sandesha2_msg_processor_t *msg_processor, 
						const axis2_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(element);
    
    if(NULL != msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx)
{
    sandesha2_seq_ack_t *seq_ack = NULL;
    sandesha2_ack_requested_t *ack_requested = NULL;
    axis2_msg_ctx_t *msg_ctx1 = NULL;
    axis2_char_t *processed = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_ctx_t *ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_property_t *property = NULL;
    sandesha2_storage_manager_t *storage_man = NULL;
    sandesha2_msg_ctx_t *fault_ctx = NULL;
    sandesha2_fault_manager_t *fault_man = NULL;
    sandesha2_sequence_t *sequence = NULL;
    axis2_char_t *str_seq_id = NULL;
    sandesha2_seq_property_bean_t *msgs_bean = NULL;
    long msg_no = 0;
    long highest_in_msg_no = 0;
    axis2_char_t *str_key = NULL;
    axis2_char_t *msgs_str = "";
    axis2_char_t msg_num_str[32];
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, msg_ctx, AXIS2_FAILURE);
    
    seq_ack = SANDESHA2_MSG_CTX_GET_MSG_PART(msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    if(NULL != seq_ack)
    {
        sandesha2_ack_processor_t ack_proc = NULL;
        ack_proc = sandesha2_ack_processor_create(env);
        SANDESHA2_MSG_PROCESSOR_PROCESS_IN_MSG(ack_proc, env, msg_ctx);
    }
    
    ack_requested = SANDESHA2_MSG_CTX_GET_MSG_PART(msg_ctx, env,
                        SANDESHA2_MSG_PART_ACK_REQUEST);
    if(NULL != ack_requested)
    {
        SANDESHA2_ACK_REQUESTED_SET_MUST_UNDERSTAND(ack_requested, env, 
                        AXIS2_FALSE);
        SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(msg_ctx, env);
    }
    
    msg_ctx1 = SANDESHA2_MSG_CTX_GET_MSG_CTX(msg_ctx, env);
    if(NULL == msg_ctx1)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX, 
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    property = SANDESHA2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE);
    
    if(NULL != property)
    {
        processed = AXIS2_PROPERTY_GET_VALUE(property, env);
    }
    if(NULL != processed && 0 == AXIS2_STRCMP(processed, "true"))
    {
        return AXIS2_SUCCESS;
    }
    
    op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env);
    if (NULL != op_ctx)
    {
        ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
        if (NULL != ctx)
        {
            axis2_char_t *value = NULL;

            value = AXIS2_STRDUP("FALSE", env);
            property = axis2_property_create(env);
            AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
            AXIS2_PROPERTY_SET_VALUE(property, env, value);
            AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN,
                                        property, AXIS2_FALSE);
        }
    }
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx1, env);
    storage_man = sandesha2_utils_get_storage_manager(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    fault_man = sandesh2_fault_manager_create(env);
    fault_ctx = SANDESHA2_FAULT_MANAGER_CHECK_FOR_LAST_MSG_NUM_EXCEEDED(
                        fault_man, env, msg_ctx, storage_man);
    if(NULL != fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
        if(AXIS2_FAILURE == AXIS2_ENGINE_SEND_FAULT(engine, env, 
                        SANDESHA2_MSG_CTX_GET_MSG_CTX(fault_ctx, env)))
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        AXIS2_MSG_CTX_PAUSE(msg_ctx1, env);
        return AXIS2_SUCCESS;
    }
    seq_prop_mgr = SANDESHA2_STORAGE_MANAGER_GET_SEQ_PROPERTY_MANAGER(
                        storage_man, env);
    sequence = SANDESHA2_MSG_CTX_GET_MSG_PART(msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQUENCE);
    SANDESHA2_SEQUNCE_SET_MUST_UNDERSTAND(sequence, env, AXIS2_FALSE);
    str_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQUENCE_GET_IDENTIFIER(sequence, env));
    fault_ctx = SANDESHA2_FAULT_MANAGER_CHECK_FOR_UNKNOWN_SEQ(fault_man, env,
                        msg_ctx, str_seq_id, storage_man);
    if(NULL != fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
        if(AXIS2_FAILURE == AXIS2_ENGINE_SEND_FAULT(engine, env, 
                        SANDESHA2_MSG_CTX_GET_MSG_CTX(fault_ctx, env)))
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        AXIS2_MSG_CTX_PAUSE(msg_ctx1, env);
        return AXIS2_SUCCESS;
    }
    SANDESHA2_SEQUENCE_SET_MUST_UNDERSTAND(sequence, env, AXIS2_FALSE);
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(msg_ctx, env);
    fault_ctx = SANDESHA2_FAULT_MANAGER_CHECK_FOR_SEQ_CLOSED(fault_man, env, 
                        str_seq_id, storage_man);
    if(NULL != fault_code)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
        if(AXIS2_FAILURE == AXIS2_ENGINE_SEND_FAULT(engine, env, 
                        SANDESHA2_MSG_CTX_GET_MSG_CTX(fault_ctx, env)))
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        AXIS2_MSG_CTX_PAUSE(msg_ctx1, env);
        return AXIS2_SUCCESS;
    }
    sandesha2_seq_manager_update_last_activated_time(env, str_seq_id, 
                        storage_man);
    msgs_bean = SANDESHA2_SEQUENCE_PROPERTY_MANAGER_RETRIEVE(seq_prop_mgr, env,
                        str_seq_id, 
                        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
    msg_no = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(SANDESHA2_SEQUENCE_GET_MSG_NUM(
                        sequence, env), env);
    if(0 == msg_no)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    str_key = axis2_uuid_gen(env);
    highest_in_msg_no_str = sandesha2_utils_get_seq_property(env, str_seq_id, 
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, storage_man);
    highest_in_msg_key_str = sandesha2_utils_get_seq_property(env, str_seq_id, 
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, storage_man);
    if(NULL == highest_in_msg_key_str)
    {
        highest_in_msg_key_str = axis2_uuid_gen(env);
    }
    if(NULL != highest_in_msg_no_str)
    {
        highest_in_msg_no = atol(highest_in_msg_no_str);
    }
    
    sprintf(msg_num_str, "%ld", msg_no);
    if(msg_no > highest_in_msg_no)
    {
        sandesha2_seq_property_bean_t *highest_msg_no_bean = NULL;
        sandesha2_seq_property_bean_t *highest_msg_key_bean = NULL;
        
        highest_in_msg_no = msg_no;
        highest_msg_no_bean = sandesha2_seq_property_bean_create(env, str_seq_id,
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, msg_num_str);
        highest_msg_key_bean = sandesha2_seq_property_bean_create(env, 
                        str_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, 
                        highest_in_msg_key_str);
        SANDESHA2_STORAGE_MANAGER_REMOVE_MSG_CTX(storage_man, env, 
                        highest_in_msg_key_str);
        SANDESHA2_STORAGE_MANAGER_store_MSG_CTX(storage_man, env, 
                        highest_in_msg_key_str, msg_ctx1);
        if(NULL != highest_in_msg_no_str)
        {
            SANDESHA2_SEQ_PROPERTY_MANAGER_UPDATE(seq_prop_mgr, env, 
                        highest_msg_no_bean);
            SANDESHA2_SEQ_PROPERTY_MANAGER_UPDATE(seq_prop_mgr, env, 
                        highest_msg_key_bean);
        }
        else
        {
            SANDESHA2_SEQ_PROPERTY_MANAGER_INSERT(seq_prop_mgr, env, 
                        highest_msg_no_bean);
            SANDESHA2_SEQ_PROPERTY_MANAGER_INSERT(seq_prop_mgr, env, 
                        highest_msg_key_bean);
        }
    }
    
    if(NULL != msgs_bean)
    {
        msgs_str = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(msgs_bean, env);
    }
    else
    {
        msgs_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(msgs_bean, env, str_seq_id);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(msgs_bean, env, 
                        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(msgs_bean, env, msgs_str);
    }
    if(AXIS2_TRUE == sandesha2_app_msg_processor_msg_num_is_in_list(
                        msg_processor, env, msgs_str, msg_no) &&
                        0 == AXIS2_STRCMP(SANDESHA2_QOS_DEFAULT_INVOCATION_TYPE, 
                        SANDESHA2_QOS_EXACTLY_ONCE))
    {
        SANDESHA2_MSG_CTX_PAUSE(msg_ctx, env);
    }
    if(NULL != msgs_str && 0 < AXIS2_STRLEN(msgs_str))
    {
        msgs_str = axis2_strcat(env, msgs_str, ",", msg_num_str);
    }
    else
    {
        msgs_str = AXIS2_STRDUP(msg_num_str, env);
    }
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(msgs_bean, env, msgs_str);
    SANDESHA2_SEQ_PROPERY_MANAGER_UPDATE(seq_prop_mgr, env, msgs_bean);
    
    next_bean_mgr = SANDESHA2_STORAGE_MANAGER_GET_NEXT_BEAN_MANAGER(storage_man,
                        env);
    next_msg_bean = SANDESHA2_NEXT_MSG_BEAN_MANAGER_RETRIEVE(next_bean_mgr, env,
                        str_seq_id);
    if(NULL != next_msg_bean)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    
    
}
    
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *msg_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, msg_ctx, AXIS2_FAILURE);
    
    
}
    
axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_msg_num_is_in_list(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_char_t *list,
                        long num)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->allocator, list, AXIS2_FALSE);
    
    
}
                    	
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_send_ack_if_reqd(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *msg_str,
                        sandesha2_storage_manager_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, msg_str, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, manager, AXIS2_FAILURE);
}
                    	
axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *internal_seq_id,
                        axis2_char_t *acks_to,
                        sandesha2_storage_manager_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, acks_to, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, manager, AXIS2_FAILURE);
    
}

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        axis2_char_t *storage_key,
                        sandesha2_storage_manager_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, storage_key, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, manager, AXIS2_FAILURE);
    
}

long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        sandesha2_storage_manager_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, manager, AXIS2_FAILURE);
    
}

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        sandesha2_storage_manager_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->allocator, manager, AXIS2_FAILURE);
    
}
