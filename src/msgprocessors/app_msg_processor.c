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
#include <sandesha2/sandesha2_seq_ack.h>
#include <sandesha2/sandesha2_seq.h>
#include <sandesha2/sandesha2_ack_requested.h>
#include <sandesha2/sandesha2_last_msg.h>
#include <sandesha2/sandesha2_create_seq.h>
#include <sandesha2/sandesha2_identifier.h>
#include <sandesha2/sandesha2_spec_specific_consts.h>
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2/sandesha2_constants.h>
#include <sandesha2/sandesha2_utils.h>
#include <sandesha2/sandesha2_msg_ctx.h>
#include <axis2_msg_ctx.h>
#include <axis2_string.h>
#include <axis2_engine.h>
#include <axis2_uuid_gen.h>
#include <axiom_soap_const.h>
#include "../client/sandesha2_client_constants.h"
#include <stdio.h>
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
                        sandesha2_storage_mgr_t *mgr);
                    	
axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *internal_seq_id,
                        axis2_char_t *acks_to,
                        sandesha2_storage_mgr_t *mgr);

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        axis2_char_t *storage_key,
                        sandesha2_storage_mgr_t *mgr);

long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        sandesha2_storage_mgr_t *mgr);

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        sandesha2_storage_mgr_t *mgr);
                        
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
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
    					sandesha2_app_msg_processor_process_out_msg;
    msg_proc_impl->msg_processor.ops->free = sandesha2_app_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (sandesha2_msg_processor_t *msg_processor, 
						const axis2_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
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
    sandesha2_storage_mgr_t *storage_man = NULL;
    sandesha2_msg_ctx_t *fault_ctx = NULL;
    sandesha2_fault_mgr_t *fault_man = NULL;
    sandesha2_seq_t *seq = NULL;
    axis2_char_t *str_seq_id = NULL;
    sandesha2_seq_property_bean_t *msgs_bean = NULL;
    long msg_no = 0;
    long highest_in_msg_no = 0;
    axis2_char_t *str_key = NULL;
    axis2_char_t *msgs_str = "";
    axis2_char_t msg_num_str[32];
    sandesha2_invoker_mgr_t *storage_map_mgr = NULL;
    sandesha2_next_msg_mgr_t *next_mgr = NULL;
    sandesha2_next_msg_bean_t *next_msg_bean = NULL;
    axis2_bool_t in_order_invoke = AXIS2_FALSE;
    sandesha2_invoker_bean_t *invoker_bean = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_char_t *highest_in_msg_no_str = NULL;
    axis2_char_t *highest_in_msg_key_str = NULL;
     
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(msg_ctx, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    if(NULL != seq_ack)
    {
        sandesha2_msg_processor_t *ack_proc = NULL;
        ack_proc = sandesha2_ack_processor_create(env);
        SANDESHA2_MSG_PROCESSOR_PROCESS_IN_MSG(ack_proc, env, msg_ctx);
    }
    
    ack_requested = (sandesha2_ack_requested_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        msg_ctx, env, SANDESHA2_MSG_PART_ACK_REQUEST);
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
    
    op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx1, env);
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
    storage_man = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    fault_man = sandesha2_fault_mgr_create(env);
    fault_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_LAST_MSG_NUM_EXCEEDED(
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
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_man, env);
    seq = (sandesha2_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ);
    SANDESHA2_SEQUNCE_SET_MUST_UNDERSTAND(seq, env, AXIS2_FALSE);
    str_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_GET_IDENTIFIER(seq, env), env);
    fault_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_UNKNOWN_SEQ(fault_man, env,
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
    SANDESHA2_SEQ_SET_MUST_UNDERSTAND(seq, env, AXIS2_FALSE);
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(msg_ctx, env);
    fault_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_SEQ_CLOSED(fault_man, env, 
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
    sandesha2_seq_mgr_update_last_activated_time(env, str_seq_id, 
                        storage_man);
    msgs_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env,
                        str_seq_id, 
                        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
    msg_no = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(SANDESHA2_SEQ_GET_MSG_NUM(
                        seq, env), env);
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
        highest_msg_no_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        str_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, 
                        msg_num_str);
        highest_msg_key_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        str_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, 
                        highest_in_msg_key_str);
        SANDESHA2_STORAGE_MGR_REMOVE_MSG_CTX(storage_man, env, 
                        highest_in_msg_key_str);
        SANDESHA2_STORAGE_MGR_store_MSG_CTX(storage_man, env, 
                        highest_in_msg_key_str, msg_ctx1);
        if(NULL != highest_in_msg_no_str)
        {
            SANDESHA2_SEQ_PROPERTY_MGR_UPDATE(seq_prop_mgr, env, 
                        highest_msg_no_bean);
            SANDESHA2_SEQ_PROPERTY_MGR_UPDATE(seq_prop_mgr, env, 
                        highest_msg_key_bean);
        }
        else
        {
            SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, 
                        highest_msg_no_bean);
            SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, 
                        highest_msg_key_bean);
        }
    }
    
    if(NULL != msgs_bean)
        msgs_str = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(msgs_bean, env);
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
        msgs_str = axis2_strcat(env, msgs_str, ",", msg_num_str);
    else
        msgs_str = AXIS2_STRDUP(msg_num_str, env);
        
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(msgs_bean, env, msgs_str);
    SANDESHA2_SEQ_PROPERY_MGR_UPDATE(seq_prop_mgr, env, msgs_bean);
    
    next_mgr = SANDESHA2_STORAGE_MGR_GET_NEXT_MSG_MGR(storage_man,
                        env);
    next_msg_bean = SANDESHA2_NEXT_MSG_MGR_RETRIEVE(next_mgr, env,
                        str_seq_id);
    if(NULL != next_msg_bean)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    storage_map_mgr = SANDESHA2_STORAGE_MGR_GET_STORAGE_MAP_MGR(
                        storage_man, env);
    in_order_invoke = SANDESHA2_PROPERTY_BEAN_IS_IN_ORDER(
                        sandesha2_utils_get_property_bean(env, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env)), env);
    if(AXIS2_TRUE == in_order_invoke)
    {
        sandesha2_seq_property_bean_t *incoming_seq_list_bean = NULL;
        axis2_array_list_t *incoming_seq_list = NULL;
        
        incoming_seq_list_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(
                        seq_prop_mgr, env, SANDESHA2_SEQ_PROP_ALL_SEQS,
                        SANDESHA2_SEQ_PROP_INCOMING_SEQ_LIST);
        if(NULL == incoming_seq_list_bean)
        {
            /**
              * Our array to_string format is [ele1,ele2,ele3]
              * here we don't have a list so [] should be passed
              */
            incoming_seq_list_bean = sandesha2_seq_property_bean_create(env);
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(incoming_seq_list_bean, env,
                        SANDESHA2_SEQ_PROP_ALL_SEQS);
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(incoming_seq_list_bean, env,
                        SANDESHA2_SEQ_PROP_INCOMING_SEQ_LIST);
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(incoming_seq_list_bean, 
                        env, "[]");
            SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env,
                        incoming_seq_list_bean);
        }
        incoming_seq_list = sandesha2_utils_get_array_list_from_string(env,
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                        incoming_seq_list_bean, env));
        /* Adding current seq to the incoming seq List */
        if(AXIS2_FALSE == sandesha2_utils_array_list_contains(env,
                        incoming_seq_list, str_seq_id))
        {
            axis2_char_t *str_seq_list = NULL;
            AXIS2_ARRAY_LIST_ADD(incoming_seq_list, env, str_seq_id);
            str_seq_list = sandesha2_utils_array_list_to_string(env, 
                        incoming_seq_list);
            /* saving the property. */
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(incoming_seq_list_bean, 
                        env, str_seq_list);
            AXIS2_FREE(env->allocator, str_seq_list);
            SANDESHA2_SEQ_PROPERTY_MGR_UPDATE(seq_prop_mgr, env, 
                        incoming_seq_list_bean);
        }
        /* save the message */
        SANDESHA2_STORAGE_MGR_STORE_MSG_CTX(storage_man, env, str_key, 
                        msg_ctx1);
        invoker_bean = sandesha2_invoker_bean_create_with_data(env, str_key,
                        msg_no, str_seq_id, AXIS2_FALSE);
        SANDESHA2_STORAGE_MAP_MGR_INSERT(storage_map_mgr, env, invoker_bean);
        /* To avoid performing application processing more than once. */
        SANDESHA2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, 
                        SANDESHA2_VALUE_TRUE);
        SANDESHA2_MSG_CTX_PAUSE(msg_ctx, env);
        /* Start the invoker if stopped */
        sandesha2_utils_start_invoker_for_seq(env, conf_ctx, str_seq_id);
    }
    sandesha2_app_msg_processor_send_ack_if_reqd(msg_processor, env, msg_ctx,
                        msgs_str, storage_man);
    return AXIS2_SUCCESS;
    
}
    
axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx1 = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_man = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_bool_t is_svr_side = AXIS2_FALSE;
    axis2_char_t *internal_seq_id = NULL;
    axis2_char_t *storage_key = NULL;
    axis2_bool_t last_msg = AXIS2_FALSE;
    axis2_property_t *property = NULL;
    long msg_num_lng = -1;
    long system_msg_num = -1;
    long msg_number = -1;
    axis2_char_t *dummy_msg_str = NULL;
    axis2_bool_t dummy_msg = AXIS2_FALSE;
    sandesha2_seq_property_bean_t *res_highest_msg_bean = NULL;
    axis2_char_t msg_number_str[32];
    axis2_bool_t send_create_seq = AXIS2_FALSE;
    axis2_char_t *spec_ver = NULL;
    axiom_soap_envelope_t *soap_env = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    sandesha2_seq_property_bean_t *out_seq_bean = NULL;
    axis2_char_t *msg_id1 = NULL;
    axis2_char_t *op_name = NULL;
    axis2_char_t *to_addr = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    msg_ctx1 = SANDESHA2_MSG_CTX_GET_MSG_CTX(msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx1, env);
    /* TODO setting up fault callback */
    
    storage_man = sandesha2_utils_get_storage_mgr(env, conf_ctx,
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_man, env);
    is_svr_side = AXIS2_MSG_CTX_IS_SERVER_SIDE(msg_ctx1, env);
    
    to_epr = AXIS2_MSG_CTX_GET_TO(msg_ctx1, env);
    if(NULL == to_epr || NULL == AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env)
                    || 0 == AXIS2_STRLEN(AXIS2_ENDPOINT_REF_GET_ADDRESS(
                    to_epr, env)))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "to epr is not set - a" 
                    "requirement in sandesha client side");
        return AXIS2_FAILURE;
    }
    
    if(NULL == AXIS2_MSG_CTX_GET_MSG_ID(msg_ctx1, env))
        AXIS2_MSG_CTX_SET_MSG_ID(axis2_uuid_gen(env));
    storage_key = axis2_uuid_gen(env);
    
    if(AXIS2_TRUE == is_svr_side)
    {
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
        sandesha2_seq_t *req_seq = NULL;
        long request_msg_no = -1;
        axis2_char_t *req_last_msg_num_str = NULL;
        axis2_char_t *incoming_seq_id = NULL;
        
        req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(AXIS2_MSG_CTX_GET_OP_CTX(
                        msg_ctx1, env), env, AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
        req_rm_msg_ctx = sandesha2_msg_initilizer_init_msg(env, req_msg_ctx);
        req_seq = (sandesha2_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        req_rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ);
        if(NULL == req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Sequence is NULL");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        incoming_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_GET_IDENTIFIER(req_seq, env), env);
        if(NULL == incoming_seq_id)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Sequence ID is NULL");
            return AXIS2_FAILURE;
        }
        request_msg_no = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(
                        SANDESHA2_SEQ_GET_MSG_NUM(req_seq, env), env);
        internal_seq_id = sandesha2_utils_get_outgoing_internal_seq_id(env,
                        incoming_seq_id);
        req_last_msg_num_str = sandesha2_utils_get_seq_property(env, 
                        incoming_seq_id, SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_NO, 
                        storage_man);
        if(NULL != req_last_msg_num_str)
        {
            long req_last_msg_num = atol(req_last_msg_num_str);
            if(req_last_msg_num == request_msg_no)
                last_msg = AXIS2_TRUE;            
        }
    }
    else
    {
        axis2_char_t *to = NULL;
        axis2_char_t *seq_key = NULL;
        axis2_char_t *last_app_msg = NULL;
        
        to = AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env);
        property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx1, env, 
                        SANDESHA2_CLIENT_SEQ_KEY, AXIS2_FALSE);
        if(NULL != property)
            seq_key = AXIS2_PROPERTY_GET_VALUE(property, env);
        internal_seq_id = sandesha2_utils_get_outgoing_internal_seq_id(env, 
                        seq_key);
        property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx1, env, 
                        SANDESHA2_CLIENT_LAST_MESSAGE, AXIS2_FALSE);
        if(NULL != property)
            last_app_msg = AXIS2_PROPERTY_GET_VALUE(property, env);
        if(NULL != last_app_msg && 0 == AXIS2_STRCMP(last_app_msg, 
                        SANDESHA2_VALUE_TRUE))
            last_msg = AXIS2_TRUE;            
    }
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx1, env, 
                        SANDESHA2_CLIENT_MESSAGE_NUMBER, AXIS2_FALSE);
    if(NULL != property)
        msg_num_lng = *(long*)(AXIS2_PROPERTY_GET_VALUE(property, env));
    if(msg_num_lng <= 0)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Invalid message number");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    system_msg_num = sandesha2_app_msg_processor_get_prev_msg_no(msg_processor,
                        env, internal_seq_id, storage_man);
    if(msg_num_lng >= 0 && msg_num_lng <= system_msg_num)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    if(msg_num_lng > 0)
        msg_number = msg_num_lng;
    else if(system_msg_num > 0)
        msg_number = system_msg_num + 1;
    else
        msg_number = 1;
    
    /* A dummy message is a one which will not be processed as a actual 
     * application message. The RM handlers will simply let these go.
     */
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx1, env, 
                        SANDESHA2_CLIENT_DUMMY_MESSAGE, AXIS2_FALSE);
    if(NULL != property)
        dummy_msg_str = AXIS2_PROPERTY_GET_VALUE(property, env);
    if(NULL != dummy_msg_str && 0 == AXIS2_STRCMP(dummy_msg_str, 
                        SANDESHA2_VALUE_TRUE))
        dummy_msg = AXIS2_TRUE;
    if(AXIS2_FALSE == dummy_msg)
        sandesha2_app_msg_processor_set_next_msg_no(msg_processor, env, 
                        internal_seq_id, msg_number, storage_man);
    
    sprintf(msg_number_str, "%ld", msg_number); 
    res_highest_msg_bean = sandesha2_seq_property_bean_create_with_data(env,
                        internal_seq_id, 
                        SANDESHA2_SEQ_PROP_HIGHEST_OUT_MSG_NUMBER,
                        msg_number_str);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, 
                        res_highest_msg_bean);
    if(AXIS2_TRUE == last_msg)
    {
        sandesha2_seq_property_bean_t *res_highest_msg_key_bean = NULL;
        sandesha2_seq_property_bean_t *res_last_msg_key_bean = NULL;
        
        res_highest_msg_key_bean = sandesha2_seq_property_bean_create_with_data(
                        env, internal_seq_id, 
                        SANDESHA2_SEQ_PROP_HIGHEST_OUT_MSG_KEY,
                        storage_key);
        res_last_msg_key_bean = sandesha2_seq_property_bean_create_with_data(
                        env, internal_seq_id, 
                        SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO,
                        msg_number_str);
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env,
                        res_highest_msg_key_bean);
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env,
                        res_last_msg_key_bean);
    }
    out_seq_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        internal_seq_id, SANDESHA2_SEQ_PROP_OUT_SEQ_ID);
    if(AXIS2_TRUE == is_svr_side)
    {
        axis2_char_t *incoming_seq_id = NULL;
        sandesha2_seq_property_bean_t *incoming_to_bean = NULL;
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
        sandesha2_seq_t *seq = NULL;
        axis2_char_t *req_seq_id = NULL;
        sandesha2_seq_property_bean_t *spec_ver_bean = NULL;

        incoming_seq_id = sandesha2_utils_get_incoming_seq_id(env, 
                        internal_seq_id);
        incoming_to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(
                        seq_prop_mgr, env, incoming_seq_id, 
                        SANDESHA2_SEQ_PROP_TO_EPR);
        if(NULL != incoming_to_bean)
        {
            axis2_char_t *incoming_to = NULL;
            incoming_to = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(incoming_to_bean,
                        env);
            property = axis2_property_create(env);
            AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
            AXIS2_PROPERTY_SET_VALUE(property, env, incoming_to);
            AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx1, env, SANDESHA2_SEQ_PROP_TO_EPR, 
                        property, AXIS2_FALSE);
        }
        
        req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx1,
                        env), env, AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
        if(NULL == req_msg_ctx)
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX, 
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        req_rm_msg_ctx = sandesha2_msg_initilizer_init_msg(env, req_msg_ctx);
        seq = SANDESHA2_MSG_CTX_GET_MSG_PART(req_rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ);
        req_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_GET_IDENTIFIER(seq, env), env);
        spec_ver_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr,
                        env, req_seq_id, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION);
        if(NULL == spec_ver_bean)
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_SPEC_VERSION,
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        spec_ver = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(spec_ver_bean, env);
    }
    else
    {
        property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx1, env, 
                        SANDESHA2_CLIENT_RM_SPEC_VERSION, AXIS2_FALSE);
        if(NULL != property)
            spec_ver = AXIS2_PROPERTY_GET_VALUE(property, env);
    }
    if(NULL == spec_ver)
        spec_ver = sandesha2_spec_specific_consts_get_default_spec_version(env);
    if(1 == msg_number)
    {
        if(NULL == out_seq_bean)
            send_create_seq = AXIS2_TRUE;
        sandesha2_seq_mgr_setup_new_client_seq(env, msg_ctx1, 
                        internal_seq_id, spec_ver, storage_man);
    }
    if(AXIS2_TRUE == send_create_seq)
    {
        sandesha2_seq_property_bean_t *res_create_seq_added = NULL;
        axis2_char_t *addr_ns_uri = NULL;
        axis2_char_t *anon_uri = NULL;
        
        res_create_seq_added = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(
                        seq_prop_mgr, env, internal_seq_id,
                        SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT);
        addr_ns_uri = sandesha2_utils_get_seq_property(env, internal_seq_id,
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, 
                        storage_man);
        anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
        if(NULL == res_create_seq_added)
        {
            axis2_char_t *acks_to = NULL;
            
            res_create_seq_added = sandesha2_seq_property_bean_create_with_data(
                        env, internal_seq_id, 
                        SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT, 
                        SANDESHA2_VALUE_TRUE);
            SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, 
                        res_create_seq_added);
            if(NULL != AXIS2_MSG_CTX_GET_SVC_CTX(msg_ctx1, env))
            {
                property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx1, env, 
                        SANDESHA2_CLIENT_ACKS_TO, AXIS2_FALSE);
                if(NULL != property)
                    acks_to = AXIS2_PROPERTY_GET_VALUE(property, env);
            }
            if(AXIS2_TRUE == is_svr_side)
            {
                axis2_msg_ctx_t *req_msg_ctx = NULL;
                req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(AXIS2_MSG_CTX_GET_OP_CTX(
                        msg_ctx1, env), env, AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
                if(NULL == req_msg_ctx)
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Request message is" 
                        "NULL");
                    AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX,
                        AXIS2_FAILURE);
                    return AXIS2_FAILURE;
                }
                acks_to = AXIS2_ENDPOINT_REF_GET_ADDRESS(AXIS2_MSG_CTX_GET_TO(
                        msg_ctx1, env), env);
            }
            else if(NULL == acks_to)
                acks_to = anon_uri;
            
            if(NULL == acks_to && is_svr_side)
            {
                axis2_char_t *incoming_seq_id = NULL;
                sandesha2_seq_property_bean_t *reply_to_epr_bean = NULL;
                
                incoming_seq_id = sandesha2_utils_get_incoming_seq_id(env, 
                        internal_seq_id);
                reply_to_epr_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(
                        seq_prop_mgr, env, incoming_seq_id, 
                        SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
                if(NULL != reply_to_epr_bean)
                {
                    axis2_endpoint_ref_t *acks_epr = NULL;
                    acks_epr = axis2_endpoint_ref_create(env, 
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(reply_to_epr_bean, 
                        env));
                    if(NULL != acks_epr)
                        acks_to = AXIS2_ENDPOINT_REF_GET_ADDRESS(acks_epr, env);
                }
            }
            /**
             * else if()
             * TODO handle acks_to == anon_uri case
             */
            sandesha2_app_msg_processor_add_create_seq_msg(msg_processor, env,
                        msg_ctx, internal_seq_id, acks_to, storage_man);
        }
    }
    soap_env = SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(msg_ctx, env);
    if(NULL == soap_env)
    {
        soap_env = axiom_soap_envelope_create_default_soap_envelope(env, 
                        AXIOM_SOAP12);
        SANDESHA2_MSG_CTX_SET_SOAP_ENVELOPE(msg_ctx, env, soap_env);
    }
    msg_id1 = axis2_uuid_gen(env);
    if(NULL == SANDESHA2_MSG_CTX_GET_MSG_ID(msg_ctx, env))
        SANDESHA2_MSG_CTX_SET_MSG_ID(msg_ctx, env, msg_id1);
        
    if(AXIS2_TRUE == is_svr_side)
    {
        /* let the request end with 202 if a ack has not been
         * written in the incoming thread
         */
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        axis2_char_t *written = NULL;
        
        req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(AXIS2_MSG_CTX_GET_OP_CTX(
                msg_ctx1, env), env, AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
        if(NULL == req_msg_ctx)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Request message is" 
                "NULL");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX,
                AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        property = AXIS2_MSG_CTX_GET_PROPERTY(req_msg_ctx, env, 
                        SANDESHA2_ACK_WRITTEN, AXIS2_FALSE);
        if(NULL != property)
            written = AXIS2_PROPERTY_GET_VALUE(property, env);
        if(NULL == written || 0 != AXIS2_STRCMP(written, SANDESHA2_VALUE_TRUE))
        {
            axis2_op_ctx_t *op_ctx = NULL;
            axis2_ctx_t *ctx = NULL;
            op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx1, env);
            if (NULL != op_ctx)
            {
                ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
                if (NULL != ctx)
                {
                    property = axis2_property_create(env);
                    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
                    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP("TRUE", 
                        env));
                    AXIS2_CTX_SET_PROPERTY(req_msg_ctx, env, 
                        AXIS2_RESPONSE_WRITTEN, property, AXIS2_FALSE);
                }
            }
        }        
    }
    op_name = AXIS2_QNAME_GET_LOCALPART(AXIS2_OP_GET_QNAME(AXIS2_OP_CTX_GET_OP(
                        AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx1, env), env), env), 
                        env);
    to_addr = AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env);
    if(NULL == AXIS2_MSG_CTX_GET_WSA_ACTION(msg_ctx1, env))
        AXIS2_MSG_CTX_SET_WSA_ACTION(msg_ctx1, env, axis2_strcat(env, to_addr, 
                        "/", op_name, NULL));
    if(NULL == AXIS2_MSG_CTX_GET_SOAP_ACTION(msg_ctx1, env))
        AXIS2_MSG_CTX_SET_SOAP_ACTION(msg_ctx1, env, axis2_strcat(env, "\"",
                        to_addr, "/", op_name, "\"", NULL));
    
    if(AXIS2_FALSE == dummy_msg)
        sandesha2_app_msg_processor_process_response_msg(msg_processor, env,
                        msg_ctx, internal_seq_id, msg_number, storage_key, 
                        storage_man);
    AXIS2_MSG_CTX_PAUSE(msg_ctx1, env);    
    return AXIS2_SUCCESS;
}
    
axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_msg_num_is_in_list(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_char_t *list,
                        long num)
{
    axis2_char_t str_long[32];
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, list, AXIS2_FALSE);
    
    sprintf(str_long, "%ld", num);
    if(NULL != strstr(list, str_long))
        return AXIS2_TRUE;
        
    return AXIS2_FALSE;
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_send_ack_if_reqd(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *msg_str,
                        sandesha2_storage_mgr_t *mgr)
{
    sandesha2_seq_t *seq = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_ack_requested_t *ack_requested = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg = NULL;
    axis2_engine_t *engine = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_str, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    
    seq = (sandesha2_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ);
    seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_GET_IDENTIFIER(seq, env), env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(SANDESHA2_MSG_CTX_GET_MSG_CTX(msg_ctx,
                        env), env);
    if(NULL == conf_ctx)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_CONF_CTX, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    ack_requested = (sandesha2_ack_requested_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        msg_ctx, env, SANDESHA2_MSG_PART_ACK_REQUEST);
    if(NULL != ack_requested)
    {
        SANDESHA2_ACK_REQUESTED_SET_MUST_UNDERSTAND(ack_requested, env, 
                        AXIS2_FALSE);
        SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(msg_ctx, env);
    }
    ack_rm_msg = sandesha2_ack_mgr_generate_ack_msg(env, msg_ctx, seq_id, 
                        mgr);
    engine = axis2_engine_create(env, conf_ctx);
    if(AXIS2_SUCCESS != AXIS2_ENGINE_SEND(engine, env, 
                        SANDESHA2_MSG_CTX_GET_MSG_CTX(ack_rm_msg, env)))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_ACK, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    return AXIS2_SUCCESS;
}
                    	
axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *internal_seq_id,
                        axis2_char_t *acks_to,
                        sandesha2_storage_mgr_t *mgr)
{
    axis2_msg_ctx_t *msg_ctx1 = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    sandesha2_msg_ctx_t *create_seq_rm_msg = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_man = NULL;
    sandesha2_sender_mgr_t *retransmitter_man = NULL;
    sandesha2_seq_offer_t *seq_offer = NULL;
    axis2_msg_ctx_t *create_seq_msg = NULL;
    sandesha2_create_seq_bean_t *create_seq_bean = NULL;
    axis2_char_t *addr_ns_uri = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_char_t *str_key = NULL;
    sandesha2_sender_bean_t *create_seq_entry = NULL;
    struct AXIS2_PLATFORM_TIMEB t1;
    long millisecs = 0;
    axis2_transport_out_desc_t *orig_trans_out = NULL;
    axis2_transport_out_desc_t *trans_out = NULL;
    axis2_engine_t *engine = NULL;
    axis2_property_t *property = NULL;
    
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, acks_to, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    
    msg_ctx1 = SANDESHA2_MSG_CTX_GET_MSG_CTX(msg_ctx, env);
    create_seq_rm_msg = sandesha2_msg_creator_create_create_seq_msg(env,
                        msg_ctx, internal_seq_id, acks_to, mgr);
    SANDESHA2_MSG_CTX_SET_FLOW(create_seq_rm_msg, env, 
                        SANDESHA2_MSG_CTX_OUT_FLOW);
    
    create_seq_part = (sandesha2_create_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        create_seq_rm_msg, env, SANDESHA2_MSG_PART_CREATE_SEQ);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        mgr, env);
    create_seq_man = SANDESHA2_STORAGE_MGR_GET_CREATE_SEQ_MGR(
                        mgr, env);
    retransmitter_man = SANDESHA2_STORAGE_MGR_GET_RETRANS_MGR
                        (mgr, env);
    seq_offer = SANDESHA2_CREATE_SEQ_GET_SEQ_OFFER(create_seq_part, env);
    if(NULL != seq_offer)
    {
        axis2_char_t *seq_offer_id = NULL;
        sandesha2_seq_property_bean_t *offer_seq_bean = NULL;
        
        seq_offer_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_OFFER_GET_IDENTIFIER(seq_offer, env), 
                        env);
        offer_seq_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(offer_seq_bean, env, 
                        SANDESHA2_SEQ_PROP_OFFERED_SEQ);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(offer_seq_bean, env,
                        internal_seq_id);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(offer_seq_bean, env,
                        seq_offer_id);
        SANDESHA2_SEQUNCE_PROPERTY_MGR_INSERT(seq_prop_mgr, env, 
                        offer_seq_bean);
    }
    create_seq_msg = SANDESHA2_MSG_CTX_GET_MSG_CTX(create_seq_rm_msg, env);
    AXIS2_MSG_CTX_SET_RELATES_TO(create_seq_msg, env, NULL);
    
    create_seq_bean = sandesha2_create_seq_bean_create_with_data(env, 
                        internal_seq_id, AXIS2_MSG_CTX_GET_WSA_MESSAGE_ID(
                        create_seq_msg, env), NULL);
    SANDESHA2_CREATE_SEQ_MGR_INSERT(create_seq_man, env, 
                        create_seq_bean);
    addr_ns_uri = sandesha2_utils_get_seq_property(env, internal_seq_id,
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, 
                        mgr);
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    if(NULL == AXIS2_MSG_CTX_GET_REPLY_TO(create_seq_msg, env))
    {
        axis2_endpoint_ref_t *cs_epr = NULL;
        cs_epr = axis2_endpoint_ref_create(env, anon_uri);
        AXIS2_MSG_CTX_SET_REPLY_TO(create_seq_msg, env, cs_epr);
    }
    str_key = axis2_uuid_gen(env);
    create_seq_entry = sandesha2_sender_bean_create(env);
    SANDESHA2_SENDER_BEAN_SET_MSG_CONTEXT_REF_KEY(create_seq_entry, env, 
                        str_key);
    AXIS2_PLATFORM_GET_TIME_IN_MILLIS(&t1);
    millisecs = 1000 * (t1.time) + t1.millitm;
    SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(create_seq_entry, env, millisecs);
    SANDESHA2_SENDER_BEAN_SET_MSG_ID(create_seq_entry, env, 
                        SANDESHA2_MSG_CTX_GET_MSG_ID(create_seq_rm_msg, env));
    SANDESHA2_SENDER_BEAN_SET_INTERNAL_SEQ_ID(create_seq_entry, env, 
                        internal_seq_id);
    SANDESHA2_SENDER_BEAN_SET_SEND(create_seq_entry, env, AXIS2_TRUE);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_FALSE,
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(create_seq_msg, env, 
                        SANDESHA2_QUALIFIED_FOR_SENDING, property,
                        AXIS2_FALSE);
    SANDESHA2_SENDER_BEAN_SET_MSG_TYPE(create_seq_entry, env, 
                        SANDESHA2_MSG_TYPE_CREATE_SEQ);
    SANDESHA2_SENDER_MGR_INSERT(retransmitter_man, env, 
                        create_seq_entry);
    SANDESHA2_STORAGE_MGR_STORE_MSG_CTX(mgr, env, str_key, create_seq_msg);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(str_key, env));
    AXIS2_MSG_CTX_SET_PROPERTY(create_seq_msg, env, SANDESHA2_MESSAGE_STORE_KEY,
                        property, AXIS2_FALSE);
    orig_trans_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(create_seq_msg, env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, orig_trans_out);
    AXIS2_MSG_CTX_SET_PROPERTY(create_seq_msg, env, 
                        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property,
                        AXIS2_FALSE);
    trans_out = sandesha2_utils_get_transport_out(env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE,
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(create_seq_msg, env, SANDESHA2_SET_SEND_TO_TRUE,
                        property, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(create_seq_msg, env, trans_out);
    engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(create_seq_msg, 
                        env));
    if(AXIS2_FAILURE == AXIS2_ENGINE_RESUME_SEND(engine, env, create_seq_msg))
        return AXIS2_FAILURE;
        
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        sandesha2_msg_ctx_t *msg_ctx,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        axis2_char_t *storage_key,
                        sandesha2_storage_mgr_t *mgr)
{
    axis2_msg_ctx_t *msg = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_sender_mgr_t *retransmitter_man = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *out_seq_bean = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_endpoint_ref_t *reply_to_epr = NULL;
    axis2_char_t *new_to_str = NULL;
    sandesha2_msg_ctx_t *req_rm_msg = NULL;
    sandesha2_seq_t *seq = NULL;
    sandesha2_seq_t *req_seq = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_char_t *rm_ns_val = NULL;
    sandesha2_msg_number_t *msg_number = NULL;
    axis2_msg_ctx_t *req_msg = NULL;
    axis2_char_t *str_identifier = NULL;
    sandesha2_sender_bean_t *app_msg_entry = NULL;
    struct AXIS2_PLATFORM_TIMEB t1;
    long millisecs = 0;
    axis2_property_t *property = NULL;
    axis2_transport_sender_t *trs_sender = NULL;
    axis2_engine_t *engine = NULL;
    sandesha2_identifier_t *identifier = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_key, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    
    msg = SANDESHA2_MSG_CTX_GET_MSG_CTX(msg_ctx, env);
    
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        mgr, env);
    retransmitter_man = SANDESHA2_STORAGE_MGR_GET_RETRANS_MGR
                        (mgr, env);
    to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        internal_seq_id, SANDESHA2_SEQ_PROP_TO_EPR);
    reply_to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        internal_seq_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
    out_seq_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        internal_seq_id, SANDESHA2_SEQ_PROP_OUT_SEQ_ID);
    if(NULL != to_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] To is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_TO, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    to_epr = axis2_endpoint_ref_create(env, 
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(to_bean, env));
    if(NULL != reply_to_bean)
        reply_to_epr = axis2_endpoint_ref_create(env, 
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(reply_to_bean, 
                        env));

    req_msg = AXIS2_OP_CTX_GET_MSG_CTX(AXIS2_MSG_CTX_GET_OP_CTX(msg, env), 
                        env, AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
    if(AXIS2_TRUE == AXIS2_MSG_CTX_IS_SERVER_SIDE(msg, env))
    {
        axis2_endpoint_ref_t *reply_to = NULL;
        
        reply_to = AXIS2_MSG_CTX_GET_REPLY_TO(req_msg, env);
        new_to_str = AXIS2_ENDPOINT_REF_GET_ADDRESS(reply_to, env);
    }
    if(NULL != new_to_str)
        SANDESHA2_MSG_CTX_SET_TO(msg_ctx, env, axis2_endpoint_ref_create(env, 
                        new_to_str));
    else
        SANDESHA2_MSG_CTX_SET_TO(msg_ctx, env, to_epr);

    if(NULL != reply_to_epr)
        SANDESHA2_MSG_CTX_SET_REPLY_TO(msg_ctx, env, reply_to_epr);
        
    rm_version = sandesha2_utils_get_rm_version(env, internal_seq_id, mgr);
    if(NULL == rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Unable to fine RM spec version");
        return AXIS2_FAILURE;
    }
    rm_ns_val = sandesha2_spec_specific_consts_get_rm_ns_val(env, rm_version);
    
    seq = sandesha2_seq_create(env, rm_ns_val);
    msg_number = sandesha2_msg_number_create(env, rm_ns_val);
    SANDESHA2_MSG_NUMBER_SET_MSG_NUM(msg_number, env, msg_num);
    SANDESHA2_SEQ_SET_MSG_NUM(seq, env, msg_number);
    
    if(AXIS2_TRUE == AXIS2_MSG_CTX_IS_SERVER_SIDE(msg, env))
    {
        req_rm_msg = sandesha2_msg_initilizer_init_msg(env, req_msg);
        req_seq = (sandesha2_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(req_rm_msg, 
                        env, SANDESHA2_MSG_PART_SEQ);
        if(NULL == seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Sequence not found");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_SEQ, 
                            AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        if(NULL != SANDESHA2_SEQ_GET_LAST_MSG(seq, env))
            SANDESHA2_SEQ_SET_LAST_MSG(seq, env, 
                            sandesha2_last_msg_create(env, rm_ns_val));
    }
    else
    {
        axis2_op_ctx_t *op_ctx = NULL;
        axis2_property_t *property = NULL;
        axis2_ctx_t *ctx = NULL;
        
        op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg, env);
        if(NULL != op_ctx)
        {
            ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
            property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
                        SANDESHA2_CLIENT_LAST_MESSAGE, AXIS2_FALSE);
            if(NULL != property)
            {
                axis2_char_t *value = AXIS2_PROPERTY_GET_VALUE(property, env);
                if(NULL != value && 0 == AXIS2_STRCMP(value, 
                        SANDESHA2_VALUE_TRUE))
                {
                    axis2_char_t *spec_ver = NULL;
                    spec_ver = sandesha2_utils_get_rm_version(env,
                        internal_seq_id, mgr);
                    if(AXIS2_TRUE == 
                        sandesha2_spec_specific_consts_is_last_msg_indicator_reqd
                        (env, spec_ver))
                    {
                        SANDESHA2_SEQ_SET_LAST_MSG(seq, env, 
                            sandesha2_last_msg_create(env, rm_ns_val));
                    }
                }
            }
        }
    }
    if(NULL == out_seq_bean || NULL == SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                        out_seq_bean, env))
        str_identifier = SANDESHA2_TEMP_SEQ_ID;
    else
        str_identifier = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(out_seq_bean, env);
        
    identifier = sandesha2_identifier_create(env, rm_ns_val);
    SANDESHA2_IDENTIFIER_SET_IDENTIFIER(identifier, env, str_identifier);
    SANDESHA2_SEQ_SET_IDENTIFIER(seq, env, identifier);
    SANDESHA2_MSG_CTX_SET_MSG_PART(msg_ctx, env, SANDESHA2_MSG_PART_SEQ, 
                        (sandesha2_iom_rm_part_t*)seq);
    /* TODO add_ack_requested */
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(msg_ctx, env);
    app_msg_entry = sandesha2_sender_bean_create(env);
    SANDESHA2_SENDER_BEAN_SET_MSG_CONTEXT_REF_KEY(app_msg_entry, env, 
                        storage_key);
    AXIS2_PLATFORM_GET_TIME_IN_MILLIS(&t1);
    millisecs = 1000 * (t1.time) + t1.millitm;
    SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(app_msg_entry, env, millisecs);
    SANDESHA2_SENDER_BEAN_SET_MSG_ID(app_msg_entry, env, 
                        SANDESHA2_MSG_CTX_GET_MSG_ID(msg_ctx, env));
    SANDESHA2_MSG_CTX_SET_MSG_NUMBER(app_msg_entry, env, msg_num);
    SANDESHA2_MSG_CTX_SET_MSG_TYPE(app_msg_entry, env, 
                        SANDESHA2_MSG_TYPE_APPLICATION);
    if(NULL == out_seq_bean || NULL == SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                        out_seq_bean, env))
        SANDESHA2_MSG_CTX_SET_SEND(app_msg_entry, env, AXIS2_FALSE);
    else
    {
        SANDESHA2_MSG_CTX_SET_SEND(app_msg_entry, env, AXIS2_TRUE);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                            SANDESHA2_VALUE_TRUE, env));
        AXIS2_MSG_CTX_SET_PROPERTY(req_msg, env, 
                            SANDESHA2_SET_SEND_TO_TRUE, property, AXIS2_FALSE);
    }
    SANDESHA2_MSG_CTX_SET_INTERNAL_SEQ_ID(app_msg_entry, env, internal_seq_id);
    SANDESHA2_STORAGE_MGR_STORE_MSG_CTX(mgr, env, storage_key, msg);
    SANDESHA2_SENDER_MGR_INSERT(retransmitter_man, env, app_msg_entry);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                            SANDESHA2_VALUE_FALSE, env));
    AXIS2_MSG_CTX_SET_PROPERTY(req_msg, env, 
                            SANDESHA2_QUALIFIED_FOR_SENDING, property, AXIS2_FALSE);
    trs_sender = AXIS2_TRANSPORT_OUT_DESC_GET_SENDER(
                        AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg, env), env);
    if(NULL != trs_sender)
    {
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(storage_key, env));
        AXIS2_MSG_CTX_SET_PROPERTY(req_msg, env, 
                        SANDESHA2_MESSAGE_STORE_KEY, property, AXIS2_FALSE);
                        
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, trs_sender);
        AXIS2_MSG_CTX_SET_PROPERTY(req_msg, env, 
                        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property, 
                        AXIS2_FALSE);
        AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(msg, env, 
                        sandesha2_utils_get_transport_out(env));
    }
    AXIS2_MSG_CTX_SET_CURRENT_HANDLER_INDEX(msg, env, 
                        AXIS2_MSG_CTX_GET_CURRENT_HANDLER_INDEX(msg, env) + 1);
    engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg, env));
    return AXIS2_ENGINE_RESUME_SEND(engine, env, msg);
}


long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        sandesha2_storage_mgr_t *mgr)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *next_msg_no_bean = NULL;
    long next_msg_no = -1;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        mgr, env);
    next_msg_no_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr,
                        env, internal_seq_id, 
                        SANDESHA2_SEQ_PROP_NEXT_MESSAGE_NUMBER);

    if(NULL != next_msg_no_bean)
    {
        axis2_char_t *str_value = NULL;
        
        str_value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(next_msg_no_bean, env);
        if(NULL != str_value)
        {
            next_msg_no = atol(str_value);
        }
    }
    return next_msg_no;
}

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env,
                        axis2_char_t *internal_seq_id,
                        long msg_num,
                        sandesha2_storage_mgr_t *mgr)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *next_msg_no_bean = NULL;
    axis2_bool_t update = AXIS2_TRUE;
    axis2_char_t str_long[32];
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    
    if(msg_num <= 0)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        mgr, env);
    next_msg_no_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr,
                        env, internal_seq_id, 
                        SANDESHA2_SEQ_PROP_NEXT_MESSAGE_NUMBER);
    if(NULL == next_msg_no_bean)
    {
        update = AXIS2_FALSE;
        next_msg_no_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(next_msg_no_bean, env, 
                        internal_seq_id);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(next_msg_no_bean, env,
                        SANDESHA2_SEQ_PROP_NEXT_MESSAGE_NUMBER);        
    }
    sprintf(str_long, "%ld", msg_num);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(next_msg_no_bean, env, str_long);
    if(AXIS2_TRUE == update)
    {
        SANDESHA2_SEQ_PROPERTY_MGR_UPDATE(seq_prop_mgr, env, 
                        next_msg_no_bean);
    }
    else
    {
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, 
                        next_msg_no_bean);
    }
    return AXIS2_SUCCESS;
}
