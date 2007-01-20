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
 
#include <sandesha2_app_msg_processor.h>
#include <sandesha2_ack_msg_processor.h>
#include <sandesha2_seq_ack.h>
#include <sandesha2_seq_mgr.h>
#include <sandesha2_seq.h>
#include <sandesha2_ack_requested.h>
#include <sandesha2_last_msg.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_identifier.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_sender_bean.h>

#include <axis2_msg_ctx.h>
#include <axis2_string.h>
#include <axis2_engine.h>
#include <axis2_uuid_gen.h>
#include <axiom_soap_const.h>
#include <sandesha2_client_constants.h>
#include <stdio.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_msg_creator.h>

/** 
 * @brief Application Message Processor struct impl
 *	Sandesha2 App Msg Processor
 */
typedef struct sandesha2_app_msg_processor_impl sandesha2_app_msg_processor_impl_t;  
  
struct sandesha2_app_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(msg_proc) \
						((sandesha2_app_msg_processor_impl_t *)(msg_proc))

/***************************** Function headers *******************************/
static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx);
    
static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *msg_ctx);


static axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_msg_num_is_in_list(
    const axis2_env_t *env, 
    axis2_char_t *list,
    long num);
                  	
static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx,
    axis2_char_t *internal_seq_id,
    axis2_char_t *acks_to,
    sandesha2_storage_mgr_t *mgr);

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx,
    axis2_char_t *internal_seq_id,
    long msg_num,
    axis2_char_t *storage_key,
    sandesha2_storage_mgr_t *mgr);

static long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id,
    sandesha2_storage_mgr_t *mgr);

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id,
    long msg_num,
    sandesha2_storage_mgr_t *mgr);
                        
static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (
    sandesha2_msg_processor_t *element, 
    const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_app_msg_processor_create(
    const axis2_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
          
    msg_proc_impl =  (sandesha2_app_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_app_msg_processor_impl_t));
	
    if(!msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(!msg_proc_impl->msg_processor.ops)
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


static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
    const axis2_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_char_t *processed = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_ctx_t *ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_property_t *property = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_msg_ctx_t *fault_ctx = NULL;
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
    axis2_bool_t msg_no_present_in_list = AXIS2_FALSE; 
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2] Start:sandesha2_app_msg_processor_process_in_msg");
   
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    if(!msg_ctx)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX, 
            AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    property = sandesha2_msg_ctx_get_property(rm_msg_ctx, env, 
        SANDESHA2_APPLICATION_PROCESSING_DONE);
    
    if(property)
    {
        processed = axis2_property_get_value(property, env);
    }
    if(processed && 0 == axis2_strcmp(processed, "true"))
    {
        return AXIS2_SUCCESS;
    }
    
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    if (op_ctx)
    {
        ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
        if (ctx)
        {
            property = axis2_property_create_with_args(env, 0, 0, 0, "FALSE");
            AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN,
                property, AXIS2_FALSE);
        }
    }
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    axis2_allocator_switch_to_global_pool(env->allocator);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    axis2_allocator_switch_to_local_pool(env->allocator);
    fault_ctx = sandesha2_fault_mgr_check_for_last_msg_num_exceeded(
        env, rm_msg_ctx, storage_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
        if(!AXIS2_ENGINE_SEND_FAULT(engine, env, 
            sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
        return AXIS2_SUCCESS;
    }
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(
        storage_mgr, env);
    seq = (sandesha2_seq_t*)sandesha2_msg_ctx_get_msg_part(rm_msg_ctx, env, 
        SANDESHA2_MSG_PART_SEQ);
    sandesha2_seq_set_must_understand(seq, env, AXIS2_FALSE);
    str_seq_id = sandesha2_identifier_get_identifier(
        sandesha2_seq_get_identifier(seq, env), env);
    fault_ctx = sandesha2_fault_mgr_check_for_unknown_seq(env,rm_msg_ctx, 
        str_seq_id, storage_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
        if(!AXIS2_ENGINE_SEND_FAULT(engine, env, 
            sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
        return AXIS2_SUCCESS;
    }
    sandesha2_seq_set_must_understand(seq, env, AXIS2_FALSE);
    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    fault_ctx = sandesha2_fault_mgr_check_for_seq_closed(env, rm_msg_ctx, 
            str_seq_id, storage_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
        if(!AXIS2_ENGINE_SEND_FAULT(engine, env, 
            sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
        return AXIS2_SUCCESS;
    }
    sandesha2_seq_mgr_update_last_activated_time(env, str_seq_id, 
        storage_mgr);
    msgs_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env,
        str_seq_id, 
        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
    msg_no = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(sandesha2_seq_get_msg_num(
        seq, env), env);
    if(0 == msg_no)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
            AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    str_key = axis2_uuid_gen(env);
    highest_in_msg_no_str = sandesha2_utils_get_seq_property(env, str_seq_id, 
        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, storage_mgr);
    highest_in_msg_key_str = sandesha2_utils_get_seq_property(env, str_seq_id, 
        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, storage_mgr);
    if(!highest_in_msg_key_str)
    {
        highest_in_msg_key_str = axis2_uuid_gen(env);
    }
    if(highest_in_msg_no_str)
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
        sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
            highest_in_msg_key_str);
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, 
            highest_in_msg_key_str, msg_ctx);
        if(highest_in_msg_no_str)
        {
            sandesha2_seq_property_mgr_update(seq_prop_mgr, env, 
                highest_msg_no_bean);
            sandesha2_seq_property_mgr_update(seq_prop_mgr, env, 
                highest_msg_key_bean);
        }
        else
        {
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                highest_msg_no_bean);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                highest_msg_key_bean);
        }
    }
    
    if(msgs_bean)
    {
        msgs_str = sandesha2_seq_property_bean_get_value(msgs_bean, env);
    }
    else
    {
        msgs_bean = sandesha2_seq_property_bean_create(env);
        printf("str_seq_id:%s\n", str_seq_id);
        sandesha2_seq_property_bean_set_seq_id(msgs_bean, env, str_seq_id);
        sandesha2_seq_property_bean_set_name(msgs_bean, env, 
            SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
        sandesha2_seq_property_bean_set_value(msgs_bean, env, msgs_str);
    }
    msg_no_present_in_list = sandesha2_app_msg_processor_msg_num_is_in_list(env, 
        msgs_str, msg_no);
    if(msg_no_present_in_list &&
        0 == axis2_strcmp(SANDESHA2_QOS_DEFAULT_INVOCATION_TYPE, 
        SANDESHA2_QOS_EXACTLY_ONCE))
    {
        sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
    }
    if(!msg_no_present_in_list)
    {
        if(msgs_str && 0 < AXIS2_STRLEN(msgs_str))
            msgs_str = axis2_strcat(env, msgs_str, ",", msg_num_str, NULL);
        else
            msgs_str = AXIS2_STRDUP(msg_num_str, env);
        sandesha2_seq_property_bean_set_value(msgs_bean, env, msgs_str);
        sandesha2_seq_property_mgr_update(seq_prop_mgr, env, msgs_bean);
    }
    
    next_mgr = sandesha2_storage_mgr_get_next_msg_mgr(storage_mgr,
       env);
    next_msg_bean = sandesha2_next_msg_mgr_retrieve(next_mgr, env,
       str_seq_id);
    if(!next_msg_bean)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
            AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    storage_map_mgr = sandesha2_storage_mgr_get_storage_map_mgr(
        storage_mgr, env);
    in_order_invoke = sandesha2_property_bean_is_in_order(
        sandesha2_utils_get_property_bean(env, 
            AXIS2_CONF_CTX_GET_CONF(conf_ctx, env)), env);
    if(AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env) && in_order_invoke)
    {
        sandesha2_seq_property_bean_t *incoming_seq_list_bean = NULL;
        axis2_array_list_t *incoming_seq_list = NULL;
        axis2_char_t *str_value = NULL;
        axis2_property_t *property = NULL;
        
        incoming_seq_list_bean = sandesha2_seq_property_mgr_retrieve(
                        seq_prop_mgr, env, SANDESHA2_SEQ_PROP_ALL_SEQS,
                        SANDESHA2_SEQ_PROP_INCOMING_SEQ_LIST);
        if(!incoming_seq_list_bean)
        {
            /**
              * Our array to_string format is [ele1,ele2,ele3]
              * here we don't have a list so [] should be passed
              */
            incoming_seq_list_bean = sandesha2_seq_property_bean_create(env);
            sandesha2_seq_property_bean_set_seq_id(incoming_seq_list_bean, env,
                        SANDESHA2_SEQ_PROP_ALL_SEQS);
            sandesha2_seq_property_bean_set_name(incoming_seq_list_bean, env,
                        SANDESHA2_SEQ_PROP_INCOMING_SEQ_LIST);
            sandesha2_seq_property_bean_set_value(incoming_seq_list_bean, 
                        env, "[]");
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env,
                        incoming_seq_list_bean);
        }
        str_value = sandesha2_seq_property_bean_get_value(
            incoming_seq_list_bean, env);
        incoming_seq_list = sandesha2_utils_get_array_list_from_string(env, 
            str_value);
        if(!incoming_seq_list)
        {
            axis2_status_t status = AXIS2_ERROR_GET_STATUS_CODE(env->error);
            if(AXIS2_SUCCESS != status)
                return status;
        }
        /* Adding current seq to the incoming seq List */
        if(!sandesha2_utils_array_list_contains(env,
                        incoming_seq_list, str_seq_id))
        {
            axis2_char_t *str_seq_list = NULL;
            AXIS2_ARRAY_LIST_ADD(incoming_seq_list, env, str_seq_id);
            str_seq_list = sandesha2_utils_array_list_to_string(env, 
                        incoming_seq_list, SANDESHA2_ARRAY_LIST_STRING);
            /* saving the property. */
            sandesha2_seq_property_bean_set_value(incoming_seq_list_bean, 
                        env, str_seq_list);
            AXIS2_FREE(env->allocator, str_seq_list);
            sandesha2_seq_property_mgr_update(seq_prop_mgr, env, 
                        incoming_seq_list_bean);
        }
        /* save the message */
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, str_key, 
                        msg_ctx);
        invoker_bean = sandesha2_invoker_bean_create_with_data(env, str_key,
                        msg_no, str_seq_id, AXIS2_FALSE);
        sandesha2_invoker_mgr_insert(storage_map_mgr, env, invoker_bean);
        property = axis2_property_create_with_args(env, 0, 0, 0, 
            SANDESHA2_VALUE_TRUE);
        /* To avoid performing application processing more than once. */
        sandesha2_msg_ctx_set_property(rm_msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property);
        sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
        /* Start the invoker if stopped */
        sandesha2_utils_start_invoker_for_seq(env, conf_ctx, str_seq_id);
    }
    sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, 
            storage_mgr);
    /*if(AXIS2_MSG_CTX_IS_PAUSED(msg_ctx, env))
    {
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_FALSE);
    }*/
    /*AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);*/
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2] Exit: sandesha2_app_msg_processor_process_in_msg");
    return AXIS2_SUCCESS;
    
}
    
static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_out_msg(
   sandesha2_msg_processor_t *msg_processor,
   const axis2_env_t *env, 
   sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
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
    axis2_char_t *msg_id = NULL;
    axis2_char_t *op_name = NULL;
    axis2_char_t *to_addr = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_msg_ctx_t *req_msg_ctx = NULL;
    
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2] Start:sandesha2_app_msg_processor_process_out_msg");
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
  
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(op_ctx, env, 
        AXIS2_WSDL_MESSAGE_LABEL_IN);
    /* TODO setting up fault callback */

    axis2_allocator_switch_to_global_pool(env->allocator);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx,
        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    axis2_allocator_switch_to_local_pool(env->allocator);
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(
                        storage_mgr, env);
    if(!seq_prop_mgr)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "seq_prop_mgr is NULL");
        return AXIS2_FAILURE;
    }
    is_svr_side = AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env);
    
    to_epr = AXIS2_MSG_CTX_GET_TO(msg_ctx, env);
    if(!to_epr || !AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env)
        || 0 == AXIS2_STRLEN(AXIS2_ENDPOINT_REF_GET_ADDRESS(
            to_epr, env)))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "to epr is not set - a" 
            "requirement in sandesha client side");
        return AXIS2_FAILURE;
    }
    
    if(!AXIS2_MSG_CTX_GET_MSG_ID(msg_ctx, env))
        AXIS2_MSG_CTX_SET_MESSAGE_ID(msg_ctx, env, axis2_uuid_gen(env));
    storage_key = axis2_uuid_gen(env);
    
    if(is_svr_side)
    {
        sandesha2_seq_t *req_seq = NULL;
        long request_msg_no = -1;
        axis2_char_t *req_last_msg_num_str = NULL;
        axis2_char_t *incoming_seq_id = NULL;
        sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
       
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        req_seq = (sandesha2_seq_t *) sandesha2_msg_ctx_get_msg_part(
            req_rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Sequence is NULL");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        incoming_seq_id = sandesha2_identifier_get_identifier(
                        sandesha2_seq_get_identifier(req_seq, env), env);
        if(!incoming_seq_id)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Sequence ID is NULL");
            return AXIS2_FAILURE;
        }
        request_msg_no = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(
                        sandesha2_seq_get_msg_num(req_seq, env), env);
        internal_seq_id = sandesha2_utils_get_outgoing_internal_seq_id(env,
                        incoming_seq_id);
        req_last_msg_num_str = sandesha2_utils_get_seq_property(env, 
                        incoming_seq_id, SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_NO, 
                        storage_mgr);
        if(req_last_msg_num_str)
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
        
        to = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "to:%s", to);
        property = axis2_msg_ctx_get_property(msg_ctx, env, 
            SANDESHA2_CLIENT_SEQ_KEY, AXIS2_FALSE);
        if(property)
            seq_key = axis2_property_get_value(property, env);
        internal_seq_id = sandesha2_utils_get_internal_seq_id(env, 
            to, seq_key);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "internal_seq_id:%s", 
            internal_seq_id);
        property = axis2_msg_ctx_get_property(msg_ctx, env, 
            SANDESHA2_CLIENT_LAST_MESSAGE, AXIS2_FALSE);
        if(property)
            last_app_msg = axis2_property_get_value(property, env);
        if(last_app_msg && 0 == axis2_strcmp(last_app_msg,  
            SANDESHA2_VALUE_TRUE))
                last_msg = AXIS2_TRUE;            
    }
    property = axis2_msg_ctx_get_property(msg_ctx, env, 
        SANDESHA2_CLIENT_MESSAGE_NUMBER, AXIS2_FALSE);
    if(property)
    {
        msg_num_lng = *(long*)(axis2_property_get_value(property, env));
        if(msg_num_lng <= 0)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Invalid message number");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
    }
    system_msg_num = sandesha2_app_msg_processor_get_prev_msg_no(env, 
            internal_seq_id, storage_mgr);
    if(msg_num_lng > 0 && msg_num_lng <= system_msg_num)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Invalid Message Number");
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
    property = axis2_msg_ctx_get_property(msg_ctx, env, 
        SANDESHA2_CLIENT_DUMMY_MESSAGE, AXIS2_FALSE);
    if(property)
        dummy_msg_str = axis2_property_get_value(property, env);
    if(dummy_msg_str && 0 == axis2_strcmp(dummy_msg_str, 
                        SANDESHA2_VALUE_TRUE))
        dummy_msg = AXIS2_TRUE;
    if(!dummy_msg)
        sandesha2_app_msg_processor_set_next_msg_no(env, internal_seq_id, 
            msg_number, storage_mgr);
    
    sprintf(msg_number_str, "%ld", msg_number); 
    res_highest_msg_bean = sandesha2_seq_property_bean_create_with_data(env,
        internal_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_OUT_MSG_NUMBER, 
            msg_number_str);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, res_highest_msg_bean);
    if(last_msg)
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
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env,
                        res_highest_msg_key_bean);
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env,
                        res_last_msg_key_bean);
    }
    out_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_OUT_SEQ_ID);
    if(is_svr_side)
    {
        axis2_char_t *incoming_seq_id = NULL;
        sandesha2_seq_property_bean_t *incoming_to_bean = NULL;
        sandesha2_seq_t *seq = NULL;
        axis2_char_t *req_seq_id = NULL;
        sandesha2_seq_property_bean_t *spec_ver_bean = NULL;
        sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;

        incoming_seq_id = sandesha2_utils_get_svr_side_incoming_seq_id(env, 
                        internal_seq_id);
        incoming_to_bean = sandesha2_seq_property_mgr_retrieve(
                        seq_prop_mgr, env, incoming_seq_id, 
                        SANDESHA2_SEQ_PROP_TO_EPR);
        if(incoming_to_bean)
        {
            axis2_char_t *incoming_to = NULL;
            axis2_char_t *value = NULL;
    
            value = sandesha2_seq_property_bean_get_value(incoming_to_bean, env);
            incoming_to = AXIS2_STRDUP(value, env);
            property = axis2_property_create_with_args(env, 0, 0, 0, incoming_to);
            axis2_msg_ctx_set_property(msg_ctx, env, SANDESHA2_SEQ_PROP_TO_EPR, 
                        property, AXIS2_FALSE);
        }
       
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        seq = (sandesha2_seq_t *) sandesha2_msg_ctx_get_msg_part(
                req_rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ);
        
        req_seq_id = sandesha2_identifier_get_identifier(
                        sandesha2_seq_get_identifier(seq, env), env);
        spec_ver_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr,
                        env, req_seq_id, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION);
        if(!spec_ver_bean)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Invalid spec version");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_SPEC_VERSION,
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        spec_ver = sandesha2_seq_property_bean_get_value(spec_ver_bean, env);
    }
    else
    {
        property = axis2_msg_ctx_get_property(msg_ctx, env, 
            SANDESHA2_CLIENT_RM_SPEC_VERSION, AXIS2_FALSE);
        if(property)
            spec_ver = axis2_property_get_value(property, env);
    }
    if(!spec_ver)
        spec_ver = sandesha2_spec_specific_consts_get_default_spec_version(env);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Spec version:%s", spec_ver);
    if(1 == msg_number)
    {
        if(!out_seq_bean)
            send_create_seq = AXIS2_TRUE;
        sandesha2_seq_mgr_setup_new_client_seq(env, msg_ctx, internal_seq_id, 
            spec_ver, storage_mgr);
    }
    if(send_create_seq)
    {
        sandesha2_seq_property_bean_t *res_create_seq_added = NULL;
        axis2_char_t *addr_ns_uri = NULL;
        axis2_char_t *anon_uri = NULL;
        
        res_create_seq_added = sandesha2_seq_property_mgr_retrieve(
                        seq_prop_mgr, env, internal_seq_id,
                        SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT);
        addr_ns_uri = sandesha2_utils_get_seq_property(env, internal_seq_id,
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, 
                        storage_mgr);
        anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
        if(!res_create_seq_added)
        {
            axis2_char_t *acks_to = NULL;
            
            res_create_seq_added = sandesha2_seq_property_bean_create_with_data(
                        env, internal_seq_id, 
                        SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT, 
                        SANDESHA2_VALUE_TRUE);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                        res_create_seq_added);
            if(AXIS2_MSG_CTX_GET_SVC_CTX(msg_ctx, env))
            {
                property = axis2_msg_ctx_get_property(msg_ctx, env, 
                        SANDESHA2_CLIENT_ACKS_TO, AXIS2_FALSE);
                if(property)
                    acks_to = axis2_property_get_value(property, env);
            }
            if(is_svr_side)
            {
                axis2_endpoint_ref_t *acks_to_epr = NULL;

                acks_to_epr = AXIS2_MSG_CTX_GET_TO(req_msg_ctx, env);
                acks_to = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(
                        acks_to_epr, env);
            }
            else if(!acks_to)
                acks_to = anon_uri;
            
            if(!acks_to && is_svr_side)
            {
                axis2_char_t *incoming_seq_id = NULL;
                sandesha2_seq_property_bean_t *reply_to_epr_bean = NULL;
                
                incoming_seq_id = sandesha2_utils_get_svr_side_incoming_seq_id(
                        env, internal_seq_id);
                reply_to_epr_bean = sandesha2_seq_property_mgr_retrieve(
                        seq_prop_mgr, env, incoming_seq_id, 
                        SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
                if(reply_to_epr_bean)
                {
                    axis2_endpoint_ref_t *acks_epr = NULL;
                    acks_epr = axis2_endpoint_ref_create(env, 
                        sandesha2_seq_property_bean_get_value(reply_to_epr_bean, 
                        env));
                    if(acks_epr)
                        acks_to = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(
                            acks_epr, env);
                }
            }
            /**
             * else if()
             * TODO handle acks_to == anon_uri case
             */
            sandesha2_app_msg_processor_add_create_seq_msg(env, rm_msg_ctx, 
                internal_seq_id, acks_to, storage_mgr);
        }
    }
    soap_env = sandesha2_msg_ctx_get_soap_envelope(rm_msg_ctx, env);
    if(!soap_env)
    {
        soap_env = axiom_soap_envelope_create_default_soap_envelope(env, 
            AXIOM_SOAP12);
        sandesha2_msg_ctx_set_soap_envelope(rm_msg_ctx, env, soap_env);
    }
    msg_id = axis2_uuid_gen(env);
    if(!sandesha2_msg_ctx_get_msg_id(rm_msg_ctx, env))
        sandesha2_msg_ctx_set_msg_id(rm_msg_ctx, env, msg_id);
        
    if(is_svr_side)
    {
        /* let the request end with 202 if a ack has not been
         * written in the incoming thread
         */
        axis2_ctx_t *ctx = NULL;
        axis2_char_t *written = NULL;
        
        ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
        property = AXIS2_CTX_GET_PROPERTY(ctx, env, SANDESHA2_ACK_WRITTEN, 
                AXIS2_FALSE);
        if(property)
            written = axis2_property_get_value(property, env);
        if(!written || 0 != axis2_strcmp(written, SANDESHA2_VALUE_TRUE))
        {
            axis2_ctx_t *ctx = NULL;
            if (op_ctx)
            {
                ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
                if (ctx)
                {
                    property = axis2_property_create_with_args(env, 0, 0, 0, 
                        "TRUE");
                    AXIS2_CTX_SET_PROPERTY(ctx, env, 
                        AXIS2_RESPONSE_WRITTEN, property, AXIS2_FALSE);
                }
            }
        }        
    }
    op_name = AXIS2_QNAME_GET_LOCALPART(AXIS2_OP_GET_QNAME(AXIS2_OP_CTX_GET_OP(
        axis2_msg_ctx_get_op_ctx(msg_ctx, env), env), env), env);
    to_addr = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env);
    /* test code */
    /*if(!AXIS2_MSG_CTX_GET_WSA_ACTION(msg_ctx, env))
        AXIS2_MSG_CTX_SET_WSA_ACTION(msg_ctx, env, axis2_strcat(env, to_addr, 
                        "/", op_name, NULL));*/
    if(!AXIS2_MSG_CTX_GET_WSA_ACTION(msg_ctx, env))
        AXIS2_MSG_CTX_SET_WSA_ACTION(msg_ctx, env, to_addr);
    /*if(!AXIS2_MSG_CTX_GET_SOAP_ACTION(msg_ctx, env))
        AXIS2_MSG_CTX_SET_SOAP_ACTION(msg_ctx, env, axis2_strcat(env, "\"",
                        to_addr, "/", op_name, "\"", NULL));*/
    if(!AXIS2_MSG_CTX_GET_SOAP_ACTION(msg_ctx, env))
        AXIS2_MSG_CTX_SET_SOAP_ACTION(msg_ctx, env, to_addr);
    /* end test code */
    
    if(!dummy_msg)
        sandesha2_app_msg_processor_process_response_msg(env, rm_msg_ctx, 
                internal_seq_id, msg_number, storage_key, storage_mgr);
    AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);    
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2] Exit: sandesha2_app_msg_processor_process_out_msg");
    return AXIS2_SUCCESS;
}
    
static axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_msg_num_is_in_list(
    const axis2_env_t *env, 
    axis2_char_t *list,
    long num)
{
    axis2_char_t str_long[32];
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, list, AXIS2_FALSE);
    
    sprintf(str_long, "%ld", num);
    if(strstr(list, str_long))
        return AXIS2_TRUE;
        
    return AXIS2_FALSE;
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_send_ack_if_reqd(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *msg_str,
    sandesha2_storage_mgr_t *mgr)
{
    sandesha2_seq_t *seq = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_ack_requested_t *ack_requested = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg = NULL;
    axis2_engine_t *engine = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    
    AXIS2_LOG_INFO(env->log,  
        "[Sandesha2] sandesha2_app_msg_processor_send_ack_if_reqd");
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_str, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    seq = (sandesha2_seq_t*)sandesha2_msg_ctx_get_msg_part(rm_msg_ctx, env, 
        SANDESHA2_MSG_PART_SEQ);
    seq_id = sandesha2_identifier_get_identifier(
        sandesha2_seq_get_identifier(seq, env), env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx,
        env), env);
    if(!conf_ctx)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2] cont_ctx is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    ack_requested = (sandesha2_ack_requested_t*)sandesha2_msg_ctx_get_msg_part(
        rm_msg_ctx, env, SANDESHA2_MSG_PART_ACK_REQUEST);
    if(ack_requested)
    {
        sandesha2_ack_requested_set_must_understand(ack_requested, env, 
            AXIS2_FALSE);
        sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    }
    ack_rm_msg = sandesha2_ack_mgr_generate_ack_msg(env, rm_msg_ctx, seq_id, 
        mgr);
    engine = axis2_engine_create(env, conf_ctx);
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(ack_rm_msg, env);
    if(AXIS2_SUCCESS != AXIS2_ENGINE_SEND(engine, env, msg_ctx))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[Sandesha2]Engine Send failed");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_ACK, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    AXIS2_LOG_INFO(env->log,  
        "[Sandesha2] Exit:sandesha2_app_msg_processor_send_ack_if_reqd");
    return AXIS2_SUCCESS;
}
                    	
static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
     const axis2_env_t *env,
     sandesha2_msg_ctx_t *rm_msg_ctx,
     axis2_char_t *internal_seq_id,
     axis2_char_t *acks_to,
     sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    sandesha2_msg_ctx_t *create_seq_rm_msg = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_man = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_seq_offer_t *seq_offer = NULL;
    axis2_msg_ctx_t *create_seq_msg = NULL;
    sandesha2_create_seq_bean_t *create_seq_bean = NULL;
    axis2_char_t *addr_ns_uri = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_char_t *str_key = NULL;
    sandesha2_sender_bean_t *create_seq_entry = NULL;
    long millisecs = 0;
    axis2_transport_out_desc_t *orig_trans_out = NULL;
    axis2_transport_out_desc_t *trans_out = NULL;
    axis2_engine_t *engine = NULL;
    axis2_property_t *property = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_char_t *create_seq_msg_store_key = NULL;
    axis2_char_t *ref_msg_store_key = NULL;
    
    AXIS2_LOG_INFO(env->log,  
        "[Sandesha2] sandesha2_app_msg_processor_add_create_seq_msg");
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, acks_to, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    create_seq_rm_msg = sandesha2_msg_creator_create_create_seq_msg(env,
        rm_msg_ctx, internal_seq_id, acks_to, storage_mgr);
    sandesha2_msg_ctx_set_flow(create_seq_rm_msg, env, 
        SANDESHA2_MSG_CTX_OUT_FLOW);
    
    create_seq_part = (sandesha2_create_seq_t*)sandesha2_msg_ctx_get_msg_part(
        create_seq_rm_msg, env, SANDESHA2_MSG_PART_CREATE_SEQ);
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(
        storage_mgr, env);
    create_seq_man = sandesha2_storage_mgr_get_create_seq_mgr(
        storage_mgr, env);
    retrans_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
    seq_offer = sandesha2_create_seq_get_seq_offer(create_seq_part, env);
    if(seq_offer)
    {
        axis2_char_t *seq_offer_id = NULL;
        sandesha2_seq_property_bean_t *offer_seq_bean = NULL;
        
        seq_offer_id = sandesha2_identifier_get_identifier(
            sandesha2_seq_offer_get_identifier(seq_offer, env), env);
        offer_seq_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_name(offer_seq_bean, env, 
            SANDESHA2_SEQ_PROP_OFFERED_SEQ);
        sandesha2_seq_property_bean_set_seq_id(offer_seq_bean, env,
            internal_seq_id);
        sandesha2_seq_property_bean_set_value(offer_seq_bean, env,
            seq_offer_id);
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
            offer_seq_bean);
    }
    create_seq_msg = sandesha2_msg_ctx_get_msg_ctx(create_seq_rm_msg, env);
    AXIS2_MSG_CTX_SET_RELATES_TO(create_seq_msg, env, NULL);
    /* Set that the create sequence message is part of a transaction. */
    property = axis2_property_create_with_args(env, 0, 0, 0, 
        SANDESHA2_VALUE_TRUE);
    axis2_msg_ctx_set_property(create_seq_msg, env, SANDESHA2_WITHIN_TRANSACTION, 
        property, AXIS2_FALSE);
    create_seq_bean = sandesha2_create_seq_bean_create_with_data(env, 
        internal_seq_id, (axis2_char_t*)AXIS2_MSG_CTX_GET_WSA_MESSAGE_ID(
        create_seq_msg, env), NULL);
    create_seq_msg_store_key = axis2_uuid_gen(env);
    sandesha2_create_seq_bean_set_create_seq_msg_store_key(create_seq_bean, env, 
        create_seq_msg_store_key); 
    /* Storing the create_seq_msg_ctx as a reference */
    ref_msg_store_key = axis2_uuid_gen(env);
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, ref_msg_store_key, 
        create_seq_msg);
    sandesha2_create_seq_bean_set_ref_msg_store_key(create_seq_bean, env, 
        ref_msg_store_key);
    sandesha2_create_seq_mgr_insert(create_seq_man, env, create_seq_bean);
    addr_ns_uri = sandesha2_utils_get_seq_property(env, internal_seq_id,
        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, storage_mgr);
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    if(!AXIS2_MSG_CTX_GET_REPLY_TO(create_seq_msg, env))
    {
        axis2_endpoint_ref_t *cs_epr = NULL;
        cs_epr = axis2_endpoint_ref_create(env, anon_uri);
        AXIS2_MSG_CTX_SET_REPLY_TO(create_seq_msg, env, cs_epr);
    }
    str_key = axis2_uuid_gen(env);
    create_seq_entry = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_msg_ctx_ref_key(create_seq_entry, env, str_key);
    millisecs = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(create_seq_entry, env, millisecs);
    msg_id = sandesha2_msg_ctx_get_msg_id(create_seq_rm_msg, env);
    sandesha2_sender_bean_set_msg_id(create_seq_entry, env, msg_id);
    sandesha2_sender_bean_set_internal_seq_id(create_seq_entry, env, 
        internal_seq_id);
    sandesha2_sender_bean_set_send(create_seq_entry, env, AXIS2_TRUE);
    property = axis2_property_create_with_args(env, 0, 0, 0, SANDESHA2_VALUE_FALSE);
    axis2_msg_ctx_set_property(create_seq_msg, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING, property, AXIS2_FALSE);
    sandesha2_sender_bean_set_msg_type(create_seq_entry, env, 
        SANDESHA2_MSG_TYPE_CREATE_SEQ);
    sandesha2_sender_mgr_insert(retrans_mgr, env, create_seq_entry);
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, str_key, create_seq_msg);
    property = axis2_property_create_with_args(env, 0, 0, 0, str_key);
    axis2_msg_ctx_set_property(create_seq_msg, env, SANDESHA2_MESSAGE_STORE_KEY,
        property, AXIS2_FALSE);
    orig_trans_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(create_seq_msg, env);
    property = axis2_property_create_with_args(env, 0, 0, 
        orig_trans_out->ops->free_void_arg, orig_trans_out);
    axis2_msg_ctx_set_property(create_seq_msg, env, 
        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property, AXIS2_FALSE);
    trans_out = sandesha2_utils_get_transport_out(env);
    property = axis2_property_create_with_args(env, 0, 0, 0, SANDESHA2_VALUE_TRUE);
    axis2_msg_ctx_set_property(create_seq_msg, env, SANDESHA2_SET_SEND_TO_TRUE,
        property, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(create_seq_msg, env, trans_out);
    engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(create_seq_msg, 
        env));
    if(!AXIS2_ENGINE_RESUME_SEND(engine, env, create_seq_msg))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Engine Resume Send failed");
        return AXIS2_FAILURE;
    }
    AXIS2_LOG_INFO(env->log,  
        "[Sandesha2] Exit:sandesha2_app_msg_processor_add_create_seq_msg");
    return AXIS2_SUCCESS;
}

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *internal_seq_id,
    long msg_num,
    axis2_char_t *storage_key,
    sandesha2_storage_mgr_t *mgr)
{
    axis2_msg_ctx_t *app_msg_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *out_seq_bean = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_endpoint_ref_t *reply_to_epr = NULL;
    axis2_char_t *new_to_str = NULL;
    sandesha2_seq_t *seq = NULL;
    sandesha2_seq_t *req_seq = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_char_t *rm_ns_val = NULL;
    sandesha2_msg_number_t *msg_number = NULL;
    axis2_msg_ctx_t *req_msg = NULL;
    axis2_char_t *str_identifier = NULL;
    sandesha2_sender_bean_t *app_msg_entry = NULL;
    long millisecs = 0;
    axis2_property_t *property = NULL;
    axis2_transport_sender_t *trs_sender = NULL;
    axis2_engine_t *engine = NULL;
    sandesha2_identifier_t *identifier = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_bool_t last_msg = AXIS2_FALSE;
    
    AXIS2_LOG_INFO(env->log,  
        "[Sandesha2] sandesha2_app_msg_processor_process_response_msg");
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_key, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    
    app_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(mgr, env);
    retrans_mgr = sandesha2_storage_mgr_get_retrans_mgr(mgr, env);
    to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_TO_EPR);
    reply_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
    out_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_OUT_SEQ_ID);
    if(!to_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] To is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_TO, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    to_epr = axis2_endpoint_ref_create(env, 
        sandesha2_seq_property_bean_get_value(to_bean, env));
    if(reply_to_bean)
        reply_to_epr = axis2_endpoint_ref_create(env, 
            sandesha2_seq_property_bean_get_value(reply_to_bean, env));

    if(AXIS2_MSG_CTX_GET_SERVER_SIDE(app_msg_ctx, env))
    {
        axis2_endpoint_ref_t *reply_to = NULL;
        
        req_msg = AXIS2_OP_CTX_GET_MSG_CTX(axis2_msg_ctx_get_op_ctx(app_msg_ctx, env), 
            env, AXIS2_WSDL_MESSAGE_LABEL_IN);
        if(req_msg)
        {
            reply_to = AXIS2_MSG_CTX_GET_REPLY_TO(req_msg, env);
        }
        if(reply_to)
            new_to_str = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(
                reply_to, env);
    }
    if(new_to_str)
        sandesha2_msg_ctx_set_to(rm_msg_ctx, env, axis2_endpoint_ref_create(env, 
            new_to_str));
    else
        sandesha2_msg_ctx_set_to(rm_msg_ctx, env, to_epr);

    if(reply_to_epr)
        sandesha2_msg_ctx_set_reply_to(rm_msg_ctx, env, reply_to_epr);
        
    rm_version = sandesha2_utils_get_rm_version(env, internal_seq_id, mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Unable to fine RM spec version");
        return AXIS2_FAILURE;
    }
    rm_ns_val = sandesha2_spec_specific_consts_get_rm_ns_val(env, rm_version);
    
    seq = sandesha2_seq_create(env, rm_ns_val);
    msg_number = sandesha2_msg_number_create(env, rm_ns_val);
    SANDESHA2_MSG_NUMBER_SET_MSG_NUM(msg_number, env, msg_num);
    sandesha2_seq_set_msg_num(seq, env, msg_number);
    
    if(AXIS2_MSG_CTX_GET_SERVER_SIDE(app_msg_ctx, env))
    {
        sandesha2_msg_ctx_t *req_rm_msg = NULL;

        req_rm_msg = sandesha2_msg_init_init_msg(env, req_msg);
        req_seq = (sandesha2_seq_t*)sandesha2_msg_ctx_get_msg_part(req_rm_msg, 
            env, SANDESHA2_MSG_PART_SEQ);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Sequence not found");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_SEQ, AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        if(sandesha2_seq_get_last_msg(req_seq, env))
        {
            last_msg = AXIS2_TRUE;
            sandesha2_seq_set_last_msg(seq, env, 
                sandesha2_last_msg_create(env, rm_ns_val));
        }
    }
    else
    {
        axis2_op_ctx_t *op_ctx = NULL;
        axis2_property_t *property = NULL;
        
        op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
        if(op_ctx)
        {
            property = axis2_msg_ctx_get_property(app_msg_ctx, env, 
                SANDESHA2_CLIENT_LAST_MESSAGE, AXIS2_FALSE);
            if(property)
            {
                axis2_char_t *value = axis2_property_get_value(property, env);
                if(value && 0 == axis2_strcmp(value, SANDESHA2_VALUE_TRUE))
                {
                    axis2_char_t *spec_ver = NULL;
                    spec_ver = sandesha2_utils_get_rm_version(env,
                        internal_seq_id, mgr);
                    last_msg = AXIS2_TRUE;
                    if(sandesha2_spec_specific_consts_is_last_msg_indicator_reqd
                        (env, spec_ver))
                    {
                        sandesha2_seq_set_last_msg(seq, env, 
                            sandesha2_last_msg_create(env, rm_ns_val));
                    }
                }
            }
        }
    }
    if(!out_seq_bean || !sandesha2_seq_property_bean_get_value(out_seq_bean, 
        env))
        str_identifier = SANDESHA2_TEMP_SEQ_ID;
    else
        str_identifier = sandesha2_seq_property_bean_get_value(out_seq_bean, env);
        
    identifier = sandesha2_identifier_create(env, rm_ns_val);
    sandesha2_identifier_set_identifier(identifier, env, str_identifier);
    sandesha2_seq_set_identifier(seq, env, identifier);
    sandesha2_msg_ctx_set_msg_part(rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ, 
       (sandesha2_iom_rm_part_t*)seq);
    /* TODO add_ack_requested */
    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    app_msg_entry = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_msg_ctx_ref_key(app_msg_entry, env, 
        storage_key);
    millisecs = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(app_msg_entry, env, millisecs);
    msg_id = sandesha2_msg_ctx_get_msg_id(rm_msg_ctx, env);
    sandesha2_sender_bean_set_msg_id(app_msg_entry, env, msg_id);
    sandesha2_sender_bean_set_msg_no(app_msg_entry, env, msg_num);
    sandesha2_sender_bean_set_msg_type(app_msg_entry, env, 
        SANDESHA2_MSG_TYPE_APPLICATION);
    if(!out_seq_bean || !sandesha2_seq_property_bean_get_value(out_seq_bean, 
        env))
    {
        sandesha2_sender_bean_set_send(app_msg_entry, env, AXIS2_FALSE);
    }
    else
    {
        sandesha2_sender_bean_set_send(app_msg_entry, env, AXIS2_TRUE);
        property = axis2_property_create_with_args(env, 0, 0, 0, 
            SANDESHA2_VALUE_TRUE);
        axis2_msg_ctx_set_property(app_msg_ctx, env, 
           SANDESHA2_SET_SEND_TO_TRUE, property, AXIS2_FALSE);
    }
    sandesha2_sender_bean_set_internal_seq_id(app_msg_entry, env, internal_seq_id);
    sandesha2_storage_mgr_store_msg_ctx(mgr, env, storage_key, app_msg_ctx);
    sandesha2_sender_mgr_insert(retrans_mgr, env, app_msg_entry);
    
    property = axis2_property_create_with_args(env, 0, 0, 0, SANDESHA2_VALUE_FALSE);
    axis2_msg_ctx_set_property(app_msg_ctx, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING, property, AXIS2_FALSE);
    trs_sender = AXIS2_TRANSPORT_OUT_DESC_GET_SENDER(
        AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(app_msg_ctx, env), env);
    if(trs_sender)
    {
        axis2_transport_out_desc_t *trans_out = NULL;

        property = axis2_property_create_with_args(env, 0, 0, 0, storage_key);
        axis2_msg_ctx_set_property(app_msg_ctx, env, 
            SANDESHA2_MESSAGE_STORE_KEY, property, AXIS2_FALSE);
                        
        trans_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(app_msg_ctx, env);
        property = axis2_property_create_with_args(env, 0, 0, 
            trans_out->ops->free_void_arg, trans_out);
        axis2_msg_ctx_set_property(app_msg_ctx, env, 
            SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property, AXIS2_FALSE);
        AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(app_msg_ctx, env, 
            sandesha2_utils_get_transport_out(env));
    }
    AXIS2_MSG_CTX_SET_CURRENT_HANDLER_INDEX(app_msg_ctx, env, 
        AXIS2_MSG_CTX_GET_CURRENT_HANDLER_INDEX(app_msg_ctx, env) + 1);
    engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(app_msg_ctx, 
        env));
    AXIS2_LOG_INFO(env->log,  
        "[Sandesha2] Exit:sandesha2_app_msg_processor_process_response_msg");
    return AXIS2_ENGINE_RESUME_SEND(engine, env, app_msg_ctx);
}


static long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
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
    
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(mgr, env);
    next_msg_no_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr,
        env, internal_seq_id, SANDESHA2_SEQ_PROP_NEXT_MESSAGE_NUMBER);

    if(next_msg_no_bean)
    {
        axis2_char_t *str_value = NULL;
        str_value = sandesha2_seq_property_bean_get_value(next_msg_no_bean, env);
        if(str_value)
        {
            next_msg_no = atol(str_value);
        }
    }
    return next_msg_no;
}

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id,
    long msg_num,
    sandesha2_storage_mgr_t *mgr)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *next_msg_no_bean = NULL;
    axis2_bool_t update = AXIS2_TRUE;
    axis2_char_t str_long[32];
    
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Start:sandesha2_app_msg_processor_set_next_msg_no");
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, mgr, AXIS2_FAILURE);
    
    if(msg_num <= 0)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
        "[sandesha2] Invalid Message Number");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
            AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(mgr, env);
    next_msg_no_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_NEXT_MESSAGE_NUMBER);
    if(!next_msg_no_bean)
    {
        update = AXIS2_FALSE;
        next_msg_no_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_seq_id(next_msg_no_bean, env, 
            internal_seq_id);
        sandesha2_seq_property_bean_set_name(next_msg_no_bean, env,
            SANDESHA2_SEQ_PROP_NEXT_MESSAGE_NUMBER);        
    }
    sprintf(str_long, "%ld", msg_num);
    sandesha2_seq_property_bean_set_value(next_msg_no_bean, env, str_long);
    if(update)
    {
        sandesha2_seq_property_mgr_update(seq_prop_mgr, env, next_msg_no_bean);
    }
    else
    {
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, next_msg_no_bean);
    }
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_app_msg_processor_set_next_msg_no");
    return AXIS2_SUCCESS;
}

