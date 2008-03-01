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
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_permanent_seq_property_mgr.h>
#include <sandesha2_permanent_create_seq_mgr.h>
#include <sandesha2_permanent_next_msg_mgr.h>
#include <sandesha2_permanent_sender_mgr.h>
#include <sandesha2_permanent_invoker_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_create_seq_bean.h>
#include <sandesha2_sender_bean.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_msg_creator.h>
#include <sandesha2_client_constants.h>

#include <axis2_const.h>
#include <axis2_msg_ctx.h>
#include <axutil_string.h>
#include <axis2_engine.h>
#include <axutil_uuid_gen.h>
#include <axis2_relates_to.h>
#include <axiom_soap_const.h>
#include <axiom_soap_body.h>

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
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx);
    
static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axutil_env_t *env, 
    sandesha2_msg_ctx_t *msg_ctx);


static axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_msg_num_is_in_list(
    const axutil_env_t *env, 
    axis2_char_t *list,
    long num);
                  	
static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx,
    axis2_char_t *internal_seq_id,
    axis2_char_t *acks_to,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_sender_mgr_t *sender_mgr);

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx,
    axis2_char_t *internal_seq_id,
    axis2_char_t *out_seq_id,
    long msg_num,
    axis2_char_t *storage_key,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_sender_mgr_t *sender_mgr);

static long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id,
    sandesha2_seq_property_mgr_t *seq_prop_mgr);

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id,
    long msg_num,
    sandesha2_seq_property_mgr_t *seq_prop_mgr);
                        
static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (
    sandesha2_msg_processor_t *element, 
    const axutil_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_app_msg_processor_create(
    const axutil_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
          
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
    const axutil_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_char_t *processed = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axutil_property_t *property = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_msg_ctx_t *fault_ctx = NULL;
    sandesha2_seq_t *seq = NULL;
    axis2_char_t *str_seq_id = NULL;
    sandesha2_seq_property_bean_t *msgs_bean = NULL;
    long msg_no = 0;
    long highest_in_msg_no = 0;
    axis2_char_t *msgs_str = "";
    axis2_char_t msg_num_str[32];
    sandesha2_invoker_mgr_t *invoker_mgr = NULL;
    sandesha2_sender_mgr_t *sender_mgr = NULL;
    sandesha2_next_msg_mgr_t *next_msg_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_next_msg_bean_t *next_msg_bean = NULL;
    axis2_bool_t in_order_invoke = AXIS2_FALSE;
    sandesha2_invoker_bean_t *invoker_bean = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_char_t *highest_in_msg_no_str = NULL;
    axis2_char_t *highest_in_msg_key_str = NULL;
    axis2_bool_t msg_no_present_in_list = AXIS2_FALSE;
    axis2_endpoint_ref_t *reply_to_epr = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    const axis2_char_t *reply_to_addr = NULL;
    const axis2_char_t *to_addr = NULL;
    const axutil_string_t *str_soap_action = NULL;
    const axis2_char_t *wsa_action = NULL;
    const axis2_char_t *soap_action = NULL;
    axis2_char_t *rm_version = NULL;
    sandesha2_seq_property_bean_t *acks_to_bean = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    axis2_char_t *acks_to_str = NULL;
    /*axis2_endpoint_ref_t *acks_to = NULL;*/
    axis2_bool_t back_channel_free = AXIS2_FALSE;
    axis2_op_t *op = NULL;
    int mep = -1;
    axis2_char_t *dbname = NULL;
   
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Entry:sandesha2_app_msg_processor_process_in_msg");
 
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    if(!msg_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Message context is not set");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX, 
            AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    property = sandesha2_msg_ctx_get_property(rm_msg_ctx, env, 
        SANDESHA2_APPLICATION_PROCESSING_DONE);
    
    if(property)
    {
        processed = axutil_property_get_value(property, env);
    }
    if(processed && 0 == axutil_strcmp(processed, "true"))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Message already processed. So returning here");
        return AXIS2_SUCCESS;
    }
    
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    axis2_op_ctx_set_in_use(op_ctx, env, AXIS2_TRUE);
    axis2_op_ctx_set_response_written(op_ctx, env, AXIS2_TRUE);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
    fault_ctx = sandesha2_fault_mgr_check_for_last_msg_num_exceeded(
        env, rm_msg_ctx, seq_prop_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);

		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]sandesha2_app_msg_processor_process_in_msg send Fault");

		if(!axis2_engine_send_fault(engine, env, 
            sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
		    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]An error occured while sending the fault");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                AXIS2_FAILURE);
            if(fault_ctx)
                sandesha2_msg_ctx_free(fault_ctx, env);
            if(engine)
                axis2_engine_free(engine, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
        if(fault_ctx)
            sandesha2_msg_ctx_free(fault_ctx, env);
        if(engine)
            axis2_engine_free(engine, env);
        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_SUCCESS;
    }
    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(env, dbname);
    next_msg_mgr = sandesha2_permanent_next_msg_mgr_create(env, dbname);
    invoker_mgr = sandesha2_permanent_invoker_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);
    seq = sandesha2_msg_ctx_get_sequence(rm_msg_ctx, env);
    sandesha2_seq_set_must_understand(seq, env, AXIS2_FALSE);
    str_seq_id = sandesha2_identifier_get_identifier(
        sandesha2_seq_get_identifier(seq, env), env);
    fault_ctx = sandesha2_fault_mgr_check_for_unknown_seq(env,rm_msg_ctx, 
        str_seq_id, seq_prop_mgr, create_seq_mgr, next_msg_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]sandesha2_app_msg_processor_process_in_msg send Fault");
        if(!axis2_engine_send_fault(engine, env, 
            sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
		    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]An error occured while sending the fault");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                AXIS2_FAILURE);
            if(fault_ctx)
                sandesha2_msg_ctx_free(fault_ctx, env);
            if(engine)
                axis2_engine_free(engine, env);
            if(seq_prop_mgr)
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            if(create_seq_mgr)
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            if(sender_mgr)
                sandesha2_sender_mgr_free(sender_mgr, env);
            if(next_msg_mgr)
                sandesha2_next_msg_mgr_free(next_msg_mgr, env);
            if(invoker_mgr)
                sandesha2_invoker_mgr_free(invoker_mgr, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
        if(fault_ctx)
            sandesha2_msg_ctx_free(fault_ctx, env);
        if(engine)
            axis2_engine_free(engine, env);
        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        if(invoker_mgr)
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_SUCCESS;
    }
    sandesha2_seq_set_must_understand(seq, env, AXIS2_FALSE);
    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    fault_ctx = sandesha2_fault_mgr_check_for_seq_closed(env, rm_msg_ctx, 
            str_seq_id, seq_prop_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]sandesha2_app_msg_processor_process_in_msg send Fault");
        if(!axis2_engine_send_fault(engine, env, 
            sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
		    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]An error occured while sending the fault");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT,
                AXIS2_FAILURE);
            if(fault_ctx)
                sandesha2_msg_ctx_free(fault_ctx, env);
            if(engine)
                axis2_engine_free(engine, env);
            if(seq_prop_mgr)
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            if(create_seq_mgr)
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            if(sender_mgr)
                sandesha2_sender_mgr_free(sender_mgr, env);
            if(next_msg_mgr)
                sandesha2_next_msg_mgr_free(next_msg_mgr, env);
            if(invoker_mgr)
                sandesha2_invoker_mgr_free(invoker_mgr, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
        if(fault_ctx)
            sandesha2_msg_ctx_free(fault_ctx, env);
        if(engine)
            axis2_engine_free(engine, env);
        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        if(invoker_mgr)
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_SUCCESS;
    }
    sandesha2_seq_mgr_update_last_activated_time(env, str_seq_id, 
        seq_prop_mgr);
    msgs_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env,
        str_seq_id, 
        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
    msg_no = sandesha2_msg_number_get_msg_num(sandesha2_seq_get_msg_num(
        seq, env), env);
    if(0 == msg_no)
    {
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Invalid message number");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
            AXIS2_FAILURE);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        if(invoker_mgr)
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    highest_in_msg_no_str = sandesha2_utils_get_seq_property(env, str_seq_id, 
        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, seq_prop_mgr);
    highest_in_msg_key_str = sandesha2_utils_get_seq_property(env, str_seq_id, 
        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, seq_prop_mgr);
    if(!highest_in_msg_key_str)
    {
        highest_in_msg_key_str = axutil_uuid_gen(env);
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
        sandesha2_seq_property_bean_t *highest_msg_id_bean = NULL;
        const axis2_char_t *msg_id = NULL;
        /*axiom_soap_envelope_t *response_envelope = NULL;*/
        /*int soap_version = -1;*/
        axutil_property_t *property = NULL;
        axis2_char_t *client_seq_key = NULL;
        
        highest_in_msg_no = msg_no;
        msg_id = axis2_msg_ctx_get_msg_id(msg_ctx, env);
        highest_msg_no_bean = sandesha2_seq_property_bean_create_with_data(env, 
            str_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, 
                msg_num_str);
        highest_msg_key_bean = sandesha2_seq_property_bean_create_with_data(env, 
            str_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, 
                highest_in_msg_key_str);
        highest_msg_id_bean = sandesha2_seq_property_bean_create_with_data(env, 
            str_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_ID, 
                (axis2_char_t *)msg_id);
        sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
            highest_in_msg_key_str, conf_ctx, -1);
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, 
            highest_in_msg_key_str, msg_ctx);
        /*response_envelope = axis2_msg_ctx_get_soap_envelope(msg_ctx, env);*/
        property = axis2_msg_ctx_get_property(msg_ctx, env, 
            SANDESHA2_CLIENT_SEQ_KEY);
        if(property)
        {
            client_seq_key = axutil_property_get_value(property, env);
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Client sequence key:%s found", client_seq_key);
        }
        else
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Client sequence key not found");
        }
        /*if(client_seq_key)
        {
            if(axis2_msg_ctx_get_is_soap_11(msg_ctx, env))
                soap_version = SANDESHA2_SOAP_VERSION_1_1;
            else
                soap_version = SANDESHA2_SOAP_VERSION_1_2;
            if(response_envelope)
            {
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                    "[sandesha2]Client sequence key:%s", client_seq_key);
                sandesha2_storage_mgr_store_response(storage_mgr, env, 
                    client_seq_key, response_envelope, msg_no, soap_version);
            }
        }*/
        if(highest_in_msg_no_str)
        {
            sandesha2_seq_property_mgr_update(seq_prop_mgr, env, 
                highest_msg_no_bean);
            sandesha2_seq_property_mgr_update(seq_prop_mgr, env, 
                highest_msg_key_bean);
            sandesha2_seq_property_mgr_update(seq_prop_mgr, env, 
                highest_msg_id_bean);
        }
        else
        {
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                highest_msg_no_bean);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                highest_msg_key_bean);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                highest_msg_id_bean);
        }
        if(highest_msg_no_bean)
            sandesha2_seq_property_bean_free(highest_msg_no_bean, env);
        if(highest_msg_key_bean)
            sandesha2_seq_property_bean_free(highest_msg_key_bean, env);
        if(highest_msg_id_bean)
            sandesha2_seq_property_bean_free(highest_msg_id_bean, env);
    }
    if(highest_in_msg_no_str)
        AXIS2_FREE(env->allocator, highest_in_msg_no_str);
    if(highest_in_msg_key_str)
        AXIS2_FREE(env->allocator, highest_in_msg_key_str);
    if(msgs_bean)
    {
        msgs_str = sandesha2_seq_property_bean_get_value(msgs_bean, env);
    }
    else
    {
        msgs_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_seq_id(msgs_bean, env, str_seq_id);
        sandesha2_seq_property_bean_set_name(msgs_bean, env, 
            SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
        sandesha2_seq_property_bean_set_value(msgs_bean, env, msgs_str);
    }
    msg_no_present_in_list = sandesha2_app_msg_processor_msg_num_is_in_list(env, 
        msgs_str, msg_no);
    if(msg_no_present_in_list &&
        0 == axutil_strcmp(SANDESHA2_QOS_DEFAULT_INVOCATION_TYPE, 
        SANDESHA2_QOS_EXACTLY_ONCE))
    {
        sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
    }
    if(!msg_no_present_in_list)
    {
        if(msgs_str && 0 < axutil_strlen(msgs_str))
            msgs_str = axutil_strcat(env, msgs_str, ",", msg_num_str, NULL);
        else
            msgs_str = axutil_strdup(env, msg_num_str);
        sandesha2_seq_property_bean_set_value(msgs_bean, env, msgs_str);
        sandesha2_seq_property_mgr_update(seq_prop_mgr, env, msgs_bean);
        if(msgs_str)
            AXIS2_FREE(env->allocator, msgs_str);
    }
    next_msg_bean = sandesha2_next_msg_mgr_retrieve(next_msg_mgr, env,
       str_seq_id);
    if(!next_msg_bean)
    {
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Sequence with seq_id:%s does not exist", str_seq_id);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
            AXIS2_FAILURE);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        if(invoker_mgr)
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    in_order_invoke = sandesha2_property_bean_is_in_order(
        sandesha2_utils_get_property_bean(env, 
            axis2_conf_ctx_get_conf(conf_ctx, env)), env);
    /* test code */
    if(axis2_msg_ctx_get_server_side(msg_ctx, env))
    {
        sandesha2_last_msg_t *last_msg = sandesha2_seq_get_last_msg(seq, env);
        axis2_char_t *msg_id = (axis2_char_t *)axis2_msg_ctx_get_msg_id(msg_ctx, env);
        if(last_msg)
        {
            sandesha2_seq_property_bean_t *seq_prop_bean = NULL;
            seq_prop_bean = sandesha2_seq_property_bean_create_with_data(
                env, str_seq_id, SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_ID, msg_id);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, seq_prop_bean);
        }
    }
    /* end test code */
    /*
     * If this message matches the WSRM 1.0 pattern for an empty last message (e.g.
     * the sender wanted to signal the last message, but didn't have an application
     * message to send) then we do not need to send the message on to the application.
     */
    str_soap_action = axis2_msg_ctx_get_soap_action(msg_ctx, env);
    soap_action = axutil_string_get_buffer(str_soap_action, env);
    wsa_action = axis2_msg_ctx_get_wsa_action(msg_ctx, env);
    if(0 == axutil_strcmp(SANDESHA2_SPEC_2005_02_ACTION_LAST_MESSAGE, 
        wsa_action) || 0 == axutil_strcmp(
        SANDESHA2_SPEC_2005_02_SOAP_ACTION_LAST_MESSAGE, soap_action)) 
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,
            "[sandesha2]Got WSRM 1.0 last message, aborting");
        sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        if(invoker_mgr)
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_SUCCESS;
    }
    if(axis2_msg_ctx_get_server_side(msg_ctx, env) && in_order_invoke)
    {
        sandesha2_seq_property_bean_t *incoming_seq_list_bean = NULL;
        axutil_array_list_t *incoming_seq_list = NULL;
        axis2_char_t *str_value = NULL;
        axutil_property_t *property = NULL;
        axis2_char_t *str_key = NULL;

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
            {
		        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                    "[sandesha2]Incoming sequence list empty");
                if(seq_prop_mgr)
                    sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
                if(create_seq_mgr)
                    sandesha2_create_seq_mgr_free(create_seq_mgr, env);
                if(sender_mgr)
                    sandesha2_sender_mgr_free(sender_mgr, env);
                if(next_msg_mgr)
                    sandesha2_next_msg_mgr_free(next_msg_mgr, env);
                if(invoker_mgr)
                    sandesha2_invoker_mgr_free(invoker_mgr, env);
                if(storage_mgr)
                    sandesha2_storage_mgr_free(storage_mgr, env);
                return status;
            }
        }
        /* Adding current seq to the incoming seq List */
        if(!sandesha2_utils_array_list_contains(env,
            incoming_seq_list, str_seq_id))
        {
            axis2_char_t *str_seq_list = NULL;
            axutil_array_list_add(incoming_seq_list, env, str_seq_id);
            str_seq_list = sandesha2_utils_array_list_to_string(env, 
                incoming_seq_list, SANDESHA2_ARRAY_LIST_STRING);
            /* saving the property. */
            sandesha2_seq_property_bean_set_value(incoming_seq_list_bean, 
                env, str_seq_list);
            if(str_seq_list)
                AXIS2_FREE(env->allocator, str_seq_list);
            sandesha2_seq_property_mgr_update(seq_prop_mgr, env, 
                incoming_seq_list_bean);
        }
        /* save the message */
        str_key = axutil_uuid_gen(env);
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, str_key, 
            msg_ctx);
        invoker_bean = sandesha2_invoker_bean_create_with_data(env, str_key,
            msg_no, str_seq_id, AXIS2_FALSE);
        if(str_key)
            AXIS2_FREE(env->allocator, str_key);
        sandesha2_invoker_mgr_insert(invoker_mgr, env, invoker_bean);
        property = axutil_property_create_with_args(env, 0, 0, 0, 
            AXIS2_VALUE_TRUE);
        /* To avoid performing application processing more than once. */
        sandesha2_msg_ctx_set_property(rm_msg_ctx, env, 
            SANDESHA2_APPLICATION_PROCESSING_DONE, property);
        sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
        /* Start the invoker if stopped */
        sandesha2_utils_start_invoker_for_seq(env, conf_ctx, str_seq_id);
    }
    rm_version = sandesha2_utils_get_rm_version(env, str_seq_id, seq_prop_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Unable to find RM spec version for seq_id %s", str_seq_id);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        if(invoker_mgr)
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    reply_to_epr = axis2_msg_ctx_get_reply_to(msg_ctx, env);
    to_epr = axis2_msg_ctx_get_to(msg_ctx, env);
    if(reply_to_epr)
        reply_to_addr = axis2_endpoint_ref_get_address(reply_to_epr, env);
    if(to_epr)
        to_addr = axis2_endpoint_ref_get_address(to_epr, env);

    op = axis2_op_ctx_get_op(op_ctx, env);
    acks_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, str_seq_id,
        SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
    if(acks_to_bean)
        acks_to_str = sandesha2_seq_property_bean_get_value(acks_to_bean, env); 
    if(!acks_to_str)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]acks_to_str"
            " seqeunce property is not set correctly");
        if(rm_version)
            AXIS2_FREE(env->allocator, rm_version);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        if(invoker_mgr)
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        if(acks_to_bean)
            sandesha2_seq_property_bean_free(acks_to_bean, env);
        return AXIS2_FAILURE;
    }
    /*acks_to = axis2_endpoint_ref_create(env, acks_to_str);*/
    mep = axis2_op_get_axis_specific_mep_const(op, env);
    back_channel_free = (reply_to_addr && !sandesha2_utils_is_anon_uri(env, 
        reply_to_addr)) || AXIS2_MEP_CONSTANT_IN_ONLY == mep;
    if(!reply_to_epr)
    {
        to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, str_seq_id,
            SANDESHA2_SEQ_PROP_TO_EPR);
        if(to_bean)
            reply_to_addr = sandesha2_seq_property_bean_get_value(to_bean, env); 
    }
    if(back_channel_free)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]back_channel_free true");
        sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, 
            storage_mgr, sender_mgr, seq_prop_mgr);
    }
    else if(sandesha2_utils_is_single_channel(env, rm_version, acks_to_str))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]is_single_channel true");
        /* Do nothing */
    } 
    else
    {
        sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, 
            storage_mgr, sender_mgr, seq_prop_mgr);
    }
    if(rm_version)
        AXIS2_FREE(env->allocator, rm_version);

    if(acks_to_bean)
        sandesha2_seq_property_bean_free(acks_to_bean, env);
    if(seq_prop_mgr)
        sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
    if(create_seq_mgr)
        sandesha2_create_seq_mgr_free(create_seq_mgr, env);
    if(sender_mgr)
        sandesha2_sender_mgr_free(sender_mgr, env);
    if(next_msg_mgr)
        sandesha2_next_msg_mgr_free(next_msg_mgr, env);
    if(invoker_mgr)
        sandesha2_invoker_mgr_free(invoker_mgr, env);
    if(storage_mgr)
        sandesha2_storage_mgr_free(storage_mgr, env);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Exit:sandesha2_app_msg_processor_process_in_msg");
    return AXIS2_SUCCESS;
    
}
    
static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_out_msg(
   sandesha2_msg_processor_t *msg_processor,
   const axutil_env_t *env, 
   sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_sender_mgr_t *sender_mgr = NULL;
    axis2_bool_t is_svr_side = AXIS2_FALSE;
    axis2_char_t *internal_seq_id = NULL;
    axis2_char_t *out_seq_id = NULL;
    axis2_char_t *storage_key = NULL;
    axis2_bool_t last_msg = AXIS2_FALSE;
    axutil_property_t *property = NULL;
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
    axis2_char_t *op_name = NULL;
    axis2_char_t *to_addr = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_msg_ctx_t *req_msg_ctx = NULL;
    axis2_relates_to_t *relates_to = NULL;
    axis2_char_t *dbname = NULL;
    
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Entry:sandesha2_app_msg_processor_process_out_msg");
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
  
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    req_msg_ctx =  axis2_op_ctx_get_msg_ctx(op_ctx, env, 
        AXIS2_WSDL_MESSAGE_LABEL_IN);
    /* TODO setting up fault callback */

    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    if(!seq_prop_mgr)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]seq_prop_mgr is NULL");
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);
    is_svr_side = axis2_msg_ctx_get_server_side(msg_ctx, env);
    
    to_epr = axis2_msg_ctx_get_to(msg_ctx, env);
    if((!to_epr || !axis2_endpoint_ref_get_address(to_epr, env)
        || 0 == axutil_strlen(axis2_endpoint_ref_get_address(
            to_epr, env))) && !is_svr_side)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]To epr is not set - a" 
            "requirement in sandesha client side");
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    
    if(!axis2_msg_ctx_get_msg_id(msg_ctx, env))
        axis2_msg_ctx_set_message_id(msg_ctx, env, axutil_uuid_gen(env));
    if(is_svr_side)
    {
        sandesha2_seq_t *req_seq = NULL;
        long request_msg_no = -1;
        axis2_char_t *incoming_seq_id = NULL;
        sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
        const axis2_relates_to_t *relates_to = NULL;
        axis2_char_t *relates_to_value = NULL;
        axis2_char_t *last_req_id = NULL;
       
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        req_seq = sandesha2_msg_ctx_get_sequence(req_rm_msg_ctx, env);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Sequence is NULL");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
                AXIS2_FAILURE);
            if(req_rm_msg_ctx)
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            if(seq_prop_mgr)
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            if(create_seq_mgr)
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            if(sender_mgr)
                sandesha2_sender_mgr_free(sender_mgr, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
        incoming_seq_id = sandesha2_identifier_get_identifier(
            sandesha2_seq_get_identifier(req_seq, env), env);
        if(!incoming_seq_id)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Sequence ID is NULL");
            if(req_rm_msg_ctx)
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            if(seq_prop_mgr)
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            if(create_seq_mgr)
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            if(sender_mgr)
                sandesha2_sender_mgr_free(sender_mgr, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
        request_msg_no = sandesha2_msg_number_get_msg_num(
            sandesha2_seq_get_msg_num(req_seq, env), env);
        internal_seq_id = sandesha2_utils_get_outgoing_internal_seq_id(env,
            incoming_seq_id);
        /* Deciding whether this is the last message. We assume it is if it 
         * relates to a message which arrived with the LastMessage flag on it.
         */
        last_req_id = sandesha2_utils_get_seq_property(env, incoming_seq_id, 
            SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_ID, seq_prop_mgr);
        relates_to = axis2_msg_ctx_get_relates_to(msg_ctx, env);
        relates_to_value = (axis2_char_t *)axis2_relates_to_get_value(relates_to, env);
        if(relates_to && last_req_id && 0 == axutil_strcmp(last_req_id, 
            relates_to_value))
        {
            last_msg = AXIS2_TRUE;
        }
        if(last_req_id)
            AXIS2_FREE(env->allocator, last_req_id);
        if(req_rm_msg_ctx)
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
    }
    else
    {
        axis2_char_t *to = NULL;
        axis2_char_t *seq_key = NULL;
        axis2_char_t *last_app_msg = NULL;
        
        to = (axis2_char_t*)axis2_endpoint_ref_get_address(to_epr, env);
        property = axis2_msg_ctx_get_property(msg_ctx, env, 
            SANDESHA2_CLIENT_SEQ_KEY);
        if(property)
            seq_key = axutil_property_get_value(property, env);
        if(!seq_key)
        {
            seq_key = axutil_uuid_gen(env);
            property = axutil_property_create_with_args(env, 0, 0, 0, seq_key);
            axis2_msg_ctx_set_property(msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY, 
                property);
        }
        internal_seq_id = sandesha2_utils_get_internal_seq_id(env, 
            to, seq_key);
        property = axis2_msg_ctx_get_property(msg_ctx, env, 
            SANDESHA2_CLIENT_LAST_MESSAGE);
        if(property)
            last_app_msg = axutil_property_get_value(property, env);
        if(last_app_msg && 0 == axutil_strcmp(last_app_msg, AXIS2_VALUE_TRUE))
        {
            axis2_char_t *spec_ver = NULL;
            spec_ver = sandesha2_utils_get_rm_version(env, internal_seq_id, 
                seq_prop_mgr);
            if(!spec_ver)
            {
                axutil_property_t *spec_ver_prop = NULL;
                spec_ver_prop = axis2_msg_ctx_get_property(msg_ctx, env, 
                    SANDESHA2_CLIENT_RM_SPEC_VERSION);
                spec_ver = axutil_strdup(env, axutil_property_get_value(
                    spec_ver_prop, env));
            }
            if(sandesha2_spec_specific_consts_is_last_msg_indicator_reqd
                (env, spec_ver))
            {
                last_msg = AXIS2_TRUE;
            }
            if(spec_ver)
                AXIS2_FREE(env->allocator, spec_ver);
        }
    }
    property = axis2_msg_ctx_get_property(msg_ctx, env, 
        SANDESHA2_CLIENT_MESSAGE_NUMBER);
    if(property)
    {
        msg_num_lng = *(long*)(axutil_property_get_value(property, env));
        if(msg_num_lng <= 0)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Invalid message number");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
                AXIS2_FAILURE);
            if(internal_seq_id)
                AXIS2_FREE(env->allocator, internal_seq_id);
            if(seq_prop_mgr)
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            if(create_seq_mgr)
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            if(sender_mgr)
                sandesha2_sender_mgr_free(sender_mgr, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
    }
    system_msg_num = sandesha2_app_msg_processor_get_prev_msg_no(env, 
        internal_seq_id, seq_prop_mgr);
    if(msg_num_lng > 0 && msg_num_lng <= system_msg_num)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Invalid Message Number");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
            AXIS2_FAILURE);
        if(internal_seq_id)
            AXIS2_FREE(env->allocator, internal_seq_id);
        if(seq_prop_mgr)
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        if(create_seq_mgr)
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        if(sender_mgr)
            sandesha2_sender_mgr_free(sender_mgr, env);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
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
        SANDESHA2_CLIENT_DUMMY_MESSAGE);
    if(property)
        dummy_msg_str = axutil_property_get_value(property, env);
    if(dummy_msg_str && 0 == axutil_strcmp(dummy_msg_str, AXIS2_VALUE_TRUE))
        dummy_msg = AXIS2_TRUE;
    if(!dummy_msg)
        sandesha2_app_msg_processor_set_next_msg_no(env, internal_seq_id, 
            msg_number, seq_prop_mgr);
    sprintf(msg_number_str, "%ld", msg_number); 
    res_highest_msg_bean = sandesha2_seq_property_bean_create_with_data(env,
        internal_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_OUT_MSG_NUMBER, 
            msg_number_str);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, res_highest_msg_bean);
    if(res_highest_msg_bean)
        sandesha2_seq_property_bean_free(res_highest_msg_bean, env);
    relates_to = axis2_msg_ctx_get_relates_to(msg_ctx, env);
    if(relates_to)
    {
        const axis2_char_t *relates_to_value = axis2_relates_to_get_value(
            relates_to, env);
        sandesha2_seq_property_bean_t *response_relates_to_bean = 
            sandesha2_seq_property_bean_create_with_data(env, internal_seq_id, 
            SANDESHA2_SEQ_PROP_HIGHEST_OUT_RELATES_TO, (axis2_char_t *) 
            relates_to_value); 
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
            response_relates_to_bean);
    }
    storage_key = axutil_uuid_gen(env);
    if(last_msg)
    {
        sandesha2_seq_property_bean_t *res_highest_msg_key_bean = NULL;
        sandesha2_seq_property_bean_t *res_last_msg_key_bean = NULL;
        
        res_highest_msg_key_bean = sandesha2_seq_property_bean_create_with_data(
            env, internal_seq_id, SANDESHA2_SEQ_PROP_HIGHEST_OUT_MSG_KEY,
            storage_key);
        res_last_msg_key_bean = sandesha2_seq_property_bean_create_with_data(
            env, internal_seq_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO,
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
            incoming_to = axutil_strdup(env, value);
            property = axutil_property_create_with_args(env, 0, 0, 0, incoming_to);
            axis2_msg_ctx_set_property(msg_ctx, env, SANDESHA2_SEQ_PROP_TO_EPR, 
                property);
        }
       
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        seq = sandesha2_msg_ctx_get_sequence(req_rm_msg_ctx, env);
        
        req_seq_id = sandesha2_identifier_get_identifier(
            sandesha2_seq_get_identifier(seq, env), env);
        spec_ver_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr,
            env, req_seq_id, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION);
        if(req_rm_msg_ctx)
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
        if(!spec_ver_bean)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Invalid spec version");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_SPEC_VERSION,
                AXIS2_FAILURE);
            if(internal_seq_id)
                AXIS2_FREE(env->allocator, internal_seq_id);
            if(seq_prop_mgr)
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            if(create_seq_mgr)
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            if(sender_mgr)
                sandesha2_sender_mgr_free(sender_mgr, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
        spec_ver = sandesha2_seq_property_bean_get_value(spec_ver_bean, env);
    }
    else
    {
        property = axis2_msg_ctx_get_property(msg_ctx, env, 
            SANDESHA2_CLIENT_RM_SPEC_VERSION);
        if(property)
            spec_ver = axutil_property_get_value(property, env);
    }
    if(!spec_ver)
        spec_ver = sandesha2_spec_specific_consts_get_default_spec_version(env);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Spec version:%s", spec_ver);
    if(1 == msg_number)
    {
        if(!out_seq_bean)
        {
            send_create_seq = AXIS2_TRUE;
        }
        if(is_svr_side)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Starting the server "\
                "sequence with internal sequence id %s", internal_seq_id);
            sandesha2_seq_mgr_setup_new_client_seq(env, msg_ctx, internal_seq_id, 
                spec_ver, seq_prop_mgr, AXIS2_TRUE);
        }
        else
        {
            {
                axutil_property_t *property = NULL;
                axis2_ctx_t *ctx = axis2_conf_ctx_get_base(conf_ctx, env);
                axutil_hash_t *msg_ctx_map = NULL;

                property = axis2_ctx_get_property(ctx, env, SANDESHA2_MSG_CTX_MAP);
                if(!property)
                {
                    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI,
                        "[sandesha2]msg_ctx_map not found in the conf_ctx");
                    msg_ctx_map = axutil_hash_make(env);
                    property = axutil_property_create_with_args(env, AXIS2_SCOPE_APPLICATION,
                        AXIS2_TRUE, axutil_hash_free_void_arg, msg_ctx_map);
                    axis2_ctx_set_property(ctx, env, SANDESHA2_MSG_CTX_MAP,
                        property);
                }
            }
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Starting the client "\
                "sequence with internal sequence id %s", internal_seq_id);
            sandesha2_seq_mgr_setup_new_client_seq(env, msg_ctx, internal_seq_id, 
                spec_ver, seq_prop_mgr, AXIS2_FALSE);
        }
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
            SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, seq_prop_mgr);
        anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
        if(addr_ns_uri)
            AXIS2_FREE(env->allocator, addr_ns_uri);
        if(!res_create_seq_added)
        {
            axis2_char_t *acks_to = NULL;
            
            res_create_seq_added = sandesha2_seq_property_bean_create_with_data(
                env, internal_seq_id, SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT, 
                AXIS2_VALUE_TRUE);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                res_create_seq_added);
            if(axis2_msg_ctx_get_svc_ctx(msg_ctx, env))
            {
                property = axis2_msg_ctx_get_property(msg_ctx, env, 
                    SANDESHA2_CLIENT_ACKS_TO);
                if(property)
                    acks_to = axutil_property_get_value(property, env);
            }
            if(is_svr_side)
            {
                axis2_endpoint_ref_t *acks_to_epr = NULL;

                acks_to_epr = axis2_msg_ctx_get_to(req_msg_ctx, env);
                acks_to = (axis2_char_t*)axis2_endpoint_ref_get_address(
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
                        acks_to = (axis2_char_t*)axis2_endpoint_ref_get_address(
                            acks_epr, env);
                }
            }
            /**
             * else if()
             * TODO handle acks_to == anon_uri case
             */
            sandesha2_app_msg_processor_add_create_seq_msg(env, rm_msg_ctx, 
                internal_seq_id, acks_to, storage_mgr, seq_prop_mgr, 
                create_seq_mgr, sender_mgr);
        }
    }
    soap_env = sandesha2_msg_ctx_get_soap_envelope(rm_msg_ctx, env);
    if(!soap_env)
    {
        soap_env = axiom_soap_envelope_create_default_soap_envelope(env, 
            AXIOM_SOAP12);
        sandesha2_msg_ctx_set_soap_envelope(rm_msg_ctx, env, soap_env);
    }
    if(!sandesha2_msg_ctx_get_msg_id(rm_msg_ctx, env))
    {
        axis2_char_t *msg_id = NULL;
        msg_id = axutil_uuid_gen(env);
        sandesha2_msg_ctx_set_msg_id(rm_msg_ctx, env, msg_id);
    }
        
    if(is_svr_side)
    {
        /* let the request end with 202 if a ack has not been
         * written in the incoming thread
         */
        axis2_ctx_t *ctx = NULL;
        axis2_char_t *written = NULL;
        
        ctx = axis2_op_ctx_get_base(op_ctx, env);
        property = axis2_ctx_get_property(ctx, env, SANDESHA2_ACK_WRITTEN);
        if(property)
            written = axutil_property_get_value(property, env);
        if(!written || 0 != axutil_strcmp(written, AXIS2_VALUE_TRUE))
        {
            if (op_ctx)
            {
                axis2_op_ctx_set_response_written(op_ctx, env, AXIS2_TRUE);
            }
        }        
    }
    op_name = axutil_qname_get_localpart(axis2_op_get_qname( axis2_op_ctx_get_op(
        axis2_msg_ctx_get_op_ctx(msg_ctx, env), env), env), env);
    if (to_epr)
        to_addr = (axis2_char_t*)axis2_endpoint_ref_get_address(to_epr, env);
    if(!axis2_msg_ctx_get_wsa_action(msg_ctx, env))
        axis2_msg_ctx_set_wsa_action(msg_ctx, env, to_addr);
    if(!axis2_msg_ctx_get_soap_action(msg_ctx, env))
    {
        axutil_string_t *soap_action = axutil_string_create(env, to_addr);
        axis2_msg_ctx_set_soap_action(msg_ctx, env, soap_action);
    }
    
    if(!dummy_msg)
        sandesha2_app_msg_processor_process_response_msg(env, rm_msg_ctx, 
            internal_seq_id, out_seq_id, msg_number, storage_key, storage_mgr,
            seq_prop_mgr, sender_mgr);
    axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);
    if(internal_seq_id)
        AXIS2_FREE(env->allocator, internal_seq_id);
    if(seq_prop_mgr)
        sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
    if(create_seq_mgr)
        sandesha2_create_seq_mgr_free(create_seq_mgr, env);
    if(sender_mgr)
        sandesha2_sender_mgr_free(sender_mgr, env);
    if(storage_mgr)
        sandesha2_storage_mgr_free(storage_mgr, env);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Exit:sandesha2_app_msg_processor_process_out_msg");
    return AXIS2_SUCCESS;
}
    
static axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_msg_num_is_in_list(
    const axutil_env_t *env, 
    axis2_char_t *str_list,
    long num)
{
    axutil_array_list_t *list = NULL;
    axis2_char_t str_long[32];
    
    AXIS2_PARAM_CHECK(env->error, str_list, AXIS2_FALSE);
    sprintf(str_long, "%ld", num);
    list = sandesha2_utils_get_array_list_from_string(env, str_list);
    if(list)
    {
        if(axutil_array_list_contains(list, env, str_long))
            return AXIS2_TRUE;
        axutil_array_list_free(list, env);
    }
    return AXIS2_FALSE;
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_send_ack_if_reqd(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *msg_str,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_sender_mgr_t *sender_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    sandesha2_seq_t *seq = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_ack_requested_t *ack_requested = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg = NULL;
    axis2_msg_ctx_t *ack_msg_ctx = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_bool_t sent = AXIS2_FALSE;
    sandesha2_seq_property_bean_t *int_seq_bean = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Entry:sandesha2_app_msg_processor_send_ack_if_reqd");
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_str, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    seq = sandesha2_msg_ctx_get_sequence(rm_msg_ctx, env);
    seq_id = sandesha2_identifier_get_identifier(
        sandesha2_seq_get_identifier(seq, env), env);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx,
        env), env);
    if(!conf_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2] cont_ctx is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    ack_requested = sandesha2_msg_ctx_get_ack_requested(rm_msg_ctx, env);
    if(ack_requested)
    {
        sandesha2_ack_requested_set_must_understand(ack_requested, env, 
            AXIS2_FALSE);
        sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    }
    ack_rm_msg = sandesha2_ack_mgr_generate_ack_msg(env, rm_msg_ctx, seq_id, 
        seq_prop_mgr);
    ack_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(ack_rm_msg, env);
    int_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        seq_id, SANDESHA2_SEQ_PROP_SPECIAL_INTERNAL_SEQ_ID);
    /* Following code block send the ack messages through sender. If we comment this
     * it will be sent directly avoiding the sender. We will enter this block only 
     * in application client side when sequence offer is on.
     */
    if(int_seq_bean)
    {
        axis2_transport_out_desc_t *transport_out = NULL;
        long send_time = -1;
        axis2_char_t *key = NULL;
        axutil_property_t *property = NULL;
        sandesha2_sender_bean_t *ack_bean = NULL;
        axis2_char_t *int_seq_id = NULL;
        key = axutil_uuid_gen(env);
        ack_bean = sandesha2_sender_bean_create(env);
        sandesha2_sender_bean_set_msg_ctx_ref_key(ack_bean, env, key);
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, key, 
            ack_msg_ctx);
        send_time = sandesha2_utils_get_current_time_in_millis(env);
        sandesha2_sender_bean_set_time_to_send(ack_bean, env, send_time);
        sandesha2_sender_bean_set_msg_id(ack_bean, env, 
            sandesha2_msg_ctx_get_msg_id(ack_rm_msg, env));
        sandesha2_sender_bean_set_send(ack_bean, env, AXIS2_TRUE);
        int_seq_id = sandesha2_seq_property_bean_get_value(int_seq_bean, env);
        sandesha2_sender_bean_set_internal_seq_id(ack_bean, env, int_seq_id);
        sandesha2_sender_bean_set_seq_id(ack_bean, env, seq_id);
        property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_FALSE);
        sandesha2_msg_ctx_set_property(ack_rm_msg, env, 
            SANDESHA2_QUALIFIED_FOR_SENDING, property);
        sandesha2_sender_bean_set_msg_type(ack_bean, env, 
            SANDESHA2_MSG_TYPE_ACK);
        sandesha2_sender_bean_set_resend(ack_bean, env, AXIS2_FALSE);
        sandesha2_sender_mgr_insert(sender_mgr, env, ack_bean);
        if(ack_bean)
            sandesha2_sender_bean_free(ack_bean, env);
        transport_out = axis2_msg_ctx_get_transport_out_desc(ack_msg_ctx, env);
        property = axutil_property_create_with_args(env, 0, 0,
            axis2_transport_out_desc_free_void_arg, transport_out);
        axis2_msg_ctx_set_property(ack_msg_ctx, env, 
            SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property);
        property = axutil_property_create_with_args(env, 0, AXIS2_TRUE, 0, key);
        axis2_msg_ctx_set_property(ack_msg_ctx, env, SANDESHA2_MESSAGE_STORE_KEY, 
            property);
        property = axutil_property_create_with_args(env, 0, 0, 0, 
            AXIS2_VALUE_TRUE);
        axis2_msg_ctx_set_property(ack_msg_ctx, env, SANDESHA2_SET_SEND_TO_TRUE, 
            property);
        axis2_msg_ctx_set_transport_out_desc(ack_msg_ctx, env, 
            sandesha2_utils_get_transport_out(env));
        {                
            axis2_endpoint_ref_t *reply_to_epr = NULL;
            reply_to_epr = axis2_msg_ctx_get_to(msg_ctx, env);
            if(reply_to_epr)
                axis2_msg_ctx_set_reply_to(ack_msg_ctx, env, reply_to_epr);
        }
        sandesha2_seq_property_bean_free(int_seq_bean, env);
    }
    if(ack_rm_msg)
    {
        axis2_engine_t *engine = NULL;
        engine = axis2_engine_create(env, conf_ctx);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]axis2_engine_send");
        sent = axis2_engine_send(engine, env, ack_msg_ctx);
        if(ack_rm_msg)
            sandesha2_msg_ctx_free(ack_rm_msg, env);
        if(engine)
            axis2_engine_free(engine, env);
    }
    if(!sent)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[Sandesha2]Engine Send failed");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_ACK, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Exit:sandesha2_app_msg_processor_send_ack_if_reqd");
    return AXIS2_SUCCESS;
}
                    	
static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_add_create_seq_msg(
     const axutil_env_t *env,
     sandesha2_msg_ctx_t *rm_msg_ctx,
     axis2_char_t *internal_seq_id,
     axis2_char_t *acks_to,
     sandesha2_storage_mgr_t *storage_mgr,
     sandesha2_seq_property_mgr_t *seq_prop_mgr,
     sandesha2_create_seq_mgr_t *create_seq_mgr,
     sandesha2_sender_mgr_t *sender_mgr)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    sandesha2_msg_ctx_t *create_seq_rm_msg = NULL;
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
    axutil_property_t *property = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_op_ctx_t *temp_opctx = NULL;   

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Entry:sandesha2_app_msg_processor_add_create_seq_msg");
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, acks_to, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, create_seq_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, sender_mgr, AXIS2_FAILURE);
    
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    create_seq_rm_msg = sandesha2_msg_creator_create_create_seq_msg(env,
        rm_msg_ctx, internal_seq_id, acks_to, seq_prop_mgr);
    if(!create_seq_rm_msg)
        return AXIS2_FAILURE;
    sandesha2_msg_ctx_set_flow(create_seq_rm_msg, env, 
        SANDESHA2_MSG_CTX_OUT_FLOW);
    create_seq_part = sandesha2_msg_ctx_get_create_seq(create_seq_rm_msg, env);
    {
        sandesha2_seq_property_bean_t *to_epr_bean = NULL;
        axis2_endpoint_ref_t *to_epr = axis2_msg_ctx_get_to(msg_ctx, env);
        if(to_epr)
        {
            axis2_char_t *to_str = 
                (axis2_char_t *)axis2_endpoint_ref_get_address(to_epr, env);
            to_epr_bean = sandesha2_seq_property_bean_create_with_data(env, 
                internal_seq_id, SANDESHA2_SEQ_PROP_TO_EPR, to_str);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, to_epr_bean);
        }
    }
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
    axis2_msg_ctx_set_relates_to(create_seq_msg, env, NULL);
    str_key = axutil_uuid_gen(env);
    create_seq_bean = sandesha2_create_seq_bean_create_with_data(env, 
        internal_seq_id, (axis2_char_t*)axis2_msg_ctx_get_wsa_message_id(
        create_seq_msg, env), NULL);
    sandesha2_create_seq_bean_set_ref_msg_store_key(create_seq_bean, env, 
        str_key);
    temp_opctx = axis2_msg_ctx_get_op_ctx(create_seq_msg, env);
    /*axis2_op_ctx_increment_ref(temp_opctx, env);*/
    sandesha2_create_seq_mgr_insert(create_seq_mgr, env, create_seq_bean);
    addr_ns_uri = sandesha2_utils_get_seq_property(env, internal_seq_id, 
        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, seq_prop_mgr);
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    if(addr_ns_uri)
        AXIS2_FREE(env->allocator, addr_ns_uri);
    if(!axis2_msg_ctx_get_reply_to(create_seq_msg, env))
    {
        axis2_endpoint_ref_t *cs_epr = NULL;
        cs_epr = axis2_endpoint_ref_create(env, anon_uri);
        axis2_msg_ctx_set_reply_to(create_seq_msg, env, cs_epr);
    }
    create_seq_entry = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_msg_ctx_ref_key(create_seq_entry, env, str_key);
    millisecs = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(create_seq_entry, env, millisecs);
    msg_id = sandesha2_msg_ctx_get_msg_id(create_seq_rm_msg, env);
    sandesha2_sender_bean_set_msg_id(create_seq_entry, env, msg_id);
    sandesha2_sender_bean_set_internal_seq_id(create_seq_entry, env, 
        internal_seq_id);
    sandesha2_sender_bean_set_send(create_seq_entry, env, AXIS2_TRUE);
    property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_FALSE);
    axis2_msg_ctx_set_property(create_seq_msg, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING, property);
    sandesha2_sender_bean_set_msg_type(create_seq_entry, env, 
        SANDESHA2_MSG_TYPE_CREATE_SEQ);
    sandesha2_sender_mgr_insert(sender_mgr, env, create_seq_entry);
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, str_key, create_seq_msg);
    property = axutil_property_create_with_args(env, 0, AXIS2_TRUE, 0, str_key);
    axis2_msg_ctx_set_property(create_seq_msg, env, SANDESHA2_MESSAGE_STORE_KEY,
        property);
    orig_trans_out = axis2_msg_ctx_get_transport_out_desc(create_seq_msg, env);
    property = axutil_property_create_with_args(env, 0, 0, 
        axis2_transport_out_desc_free_void_arg, orig_trans_out);
    axis2_msg_ctx_set_property(create_seq_msg, env, 
        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property);
    trans_out = sandesha2_utils_get_transport_out(env);
    property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
    axis2_msg_ctx_set_property(create_seq_msg, env, SANDESHA2_SET_SEND_TO_TRUE,
        property);
    axis2_msg_ctx_set_transport_out_desc(create_seq_msg, env, trans_out);
    engine = axis2_engine_create(env, axis2_msg_ctx_get_conf_ctx(create_seq_msg, 
        env));
    if(create_seq_rm_msg)
        sandesha2_msg_ctx_free(create_seq_rm_msg, env);
    if(!axis2_engine_resume_send(engine, env, create_seq_msg))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Engine Resume Send failed");
        return AXIS2_FAILURE;
    }
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Exit:sandesha2_app_msg_processor_add_create_seq_msg");
    return AXIS2_SUCCESS;
}

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_process_response_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *internal_seq_id,
    axis2_char_t *out_seq_id,
    long msg_num,
    axis2_char_t *storage_key,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_sender_mgr_t *sender_mgr)
{
    axis2_msg_ctx_t *app_msg_ctx = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *out_seq_bean = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_endpoint_ref_t *reply_to_epr = NULL;
    const axis2_char_t *to_addr = NULL;
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
    axutil_property_t *property = NULL;
    axis2_transport_sender_t *trs_sender = NULL;
    axis2_engine_t *engine = NULL;
    sandesha2_identifier_t *identifier = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_bool_t last_msg = AXIS2_FALSE;
    axis2_op_ctx_t *temp_op_ctx = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2] sandesha2_app_msg_processor_process_response_msg");
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_key, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, sender_mgr, AXIS2_FAILURE);
    
    app_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_TO_EPR);
    reply_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
    out_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_OUT_SEQ_ID);
    while(!out_seq_bean)
    {
        out_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            internal_seq_id, SANDESHA2_SEQ_PROP_OUT_SEQ_ID);
        AXIS2_SLEEP(1);
    }
    if (to_bean)
    {
        to_epr = axis2_endpoint_ref_create(env, 
            sandesha2_seq_property_bean_get_value(to_bean, env));
        sandesha2_seq_property_bean_free(to_bean, env);
    }
    
    if(reply_to_bean)
    {
        reply_to_epr = axis2_endpoint_ref_create(env, 
            sandesha2_seq_property_bean_get_value(reply_to_bean, env));
        sandesha2_seq_property_bean_free(reply_to_bean, env);
    }
    
    if (to_epr)
    {
        to_addr = axis2_endpoint_ref_get_address(to_epr, env);
    }
    
    if(axis2_msg_ctx_get_server_side(app_msg_ctx, env))
    {
        axis2_endpoint_ref_t *reply_to = NULL;
        
        req_msg =  axis2_op_ctx_get_msg_ctx(axis2_msg_ctx_get_op_ctx(app_msg_ctx, env), 
            env, AXIS2_WSDL_MESSAGE_LABEL_IN);
        if(req_msg)
        {
            reply_to = axis2_msg_ctx_get_reply_to(req_msg, env);
        }
        if(reply_to)
            new_to_str = (axis2_char_t*)axis2_endpoint_ref_get_address(
                reply_to, env);
    }
    if(new_to_str)
        sandesha2_msg_ctx_set_to(rm_msg_ctx, env, axis2_endpoint_ref_create(env, 
            new_to_str));
    else if (to_epr)
        sandesha2_msg_ctx_set_to(rm_msg_ctx, env, to_epr);

    rm_version = sandesha2_utils_get_rm_version(env, internal_seq_id, seq_prop_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Unable to find "\
            "RM spec version for the internal_seq_id %s", internal_seq_id);
        return AXIS2_FAILURE;
    }
    rm_ns_val = sandesha2_spec_specific_consts_get_rm_ns_val(env, rm_version);
    
    seq = sandesha2_seq_create(env, rm_ns_val);
    msg_number = sandesha2_msg_number_create(env, rm_ns_val);
    sandesha2_msg_number_set_msg_num(msg_number, env, msg_num);
    sandesha2_seq_set_msg_num(seq, env, msg_number);
    
    if(axis2_msg_ctx_get_server_side(app_msg_ctx, env))
    {
        sandesha2_msg_ctx_t *req_rm_msg = NULL;

        req_rm_msg = sandesha2_msg_init_init_msg(env, req_msg);
        req_seq = sandesha2_msg_ctx_get_sequence(req_rm_msg, env);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Sequence not found");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_SEQ, AXIS2_FAILURE);
            if(rm_version)
                AXIS2_FREE(env->allocator, rm_version);
            if(req_rm_msg)
                sandesha2_msg_ctx_free(req_rm_msg, env);
            return AXIS2_FAILURE;
        }
        if(sandesha2_seq_get_last_msg(req_seq, env))
        {
            last_msg = AXIS2_TRUE;
            sandesha2_seq_set_last_msg(seq, env, 
                sandesha2_last_msg_create(env, rm_ns_val));
        }
        if(req_rm_msg)
            sandesha2_msg_ctx_free(req_rm_msg, env);
    }
    else
    {
        axis2_op_ctx_t *op_ctx = NULL;
        axutil_property_t *property = NULL;
        
        op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
        if(op_ctx)
        {
            property = axis2_msg_ctx_get_property(app_msg_ctx, env, 
                SANDESHA2_CLIENT_LAST_MESSAGE);
            if(property)
            {
                axis2_char_t *value = axutil_property_get_value(property, env);
                if(value && 0 == axutil_strcmp(value, AXIS2_VALUE_TRUE))
                {
                    if(sandesha2_spec_specific_consts_is_last_msg_indicator_reqd
                        (env, rm_version))
                    {
                        last_msg = AXIS2_TRUE;
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
    sandesha2_msg_ctx_set_sequence(rm_msg_ctx, env, seq);
    /* TODO add_ack_requested */
    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    app_msg_entry = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_internal_seq_id(app_msg_entry, env, internal_seq_id);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "sandesha to_addr = %s ", to_addr);
    if(axis2_msg_ctx_get_server_side(app_msg_ctx, env) &&
       sandesha2_utils_is_single_channel(env, rm_version, to_addr))
    {

        axis2_char_t *incoming_seq_id = NULL;
        sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
        axis2_msg_ctx_t *msg_ctx = NULL;
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        axis2_op_ctx_t *op_ctx = NULL;
        sandesha2_seq_t *req_seq = NULL;
       
        msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
        op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
        req_msg_ctx =  axis2_op_ctx_get_msg_ctx(op_ctx, env, 
        AXIS2_WSDL_MESSAGE_LABEL_IN);
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        req_seq = sandesha2_msg_ctx_get_sequence(req_rm_msg_ctx, env);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Sequence is NULL");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, 
                AXIS2_FAILURE);
            if(rm_version)
                AXIS2_FREE(env->allocator, rm_version);
            if(req_rm_msg_ctx)
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            return AXIS2_FAILURE;
        }
        incoming_seq_id = sandesha2_identifier_get_identifier(
            sandesha2_seq_get_identifier(req_seq, env), env);
        if(!incoming_seq_id)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Sequence ID is NULL");
            if(rm_version)
                AXIS2_FREE(env->allocator, rm_version);
            if(req_rm_msg_ctx)
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            return AXIS2_FAILURE;
        }
        sandesha2_msg_creator_add_ack_msg(env, rm_msg_ctx, incoming_seq_id, 
            seq_prop_mgr);
        if(req_rm_msg_ctx)
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
        engine = axis2_engine_create(env, axis2_msg_ctx_get_conf_ctx(msg_ctx, 
            env));
        return axis2_engine_resume_send(engine, env, msg_ctx);
    }
    if(rm_version)
        AXIS2_FREE(env->allocator, rm_version);
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
        property = axutil_property_create_with_args(env, 0, 0, 0, 
            AXIS2_VALUE_TRUE);
        axis2_msg_ctx_set_property(app_msg_ctx, env, 
           SANDESHA2_SET_SEND_TO_TRUE, property);
    }
    temp_op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
    axis2_op_ctx_increment_ref(temp_op_ctx, env);
    property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_FALSE);
    axis2_msg_ctx_set_property(app_msg_ctx, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING, property);
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, storage_key, app_msg_ctx);
    sandesha2_sender_mgr_insert(sender_mgr, env, app_msg_entry);
    
    trs_sender = axis2_transport_out_desc_get_sender(
        axis2_msg_ctx_get_transport_out_desc(app_msg_ctx, env), env);
    if(trs_sender)
    {
        axis2_transport_out_desc_t *trans_out = NULL;

        property = axutil_property_create_with_args(env, 0, AXIS2_TRUE, 0, storage_key);
        axis2_msg_ctx_set_property(app_msg_ctx, env, 
            SANDESHA2_MESSAGE_STORE_KEY, property);
                        
        trans_out = axis2_msg_ctx_get_transport_out_desc(app_msg_ctx, env);
        property = axutil_property_create_with_args(env, 0, 0, 
            axis2_transport_out_desc_free_void_arg, trans_out);
        axis2_msg_ctx_set_property(app_msg_ctx, env, 
            SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property);
        axis2_msg_ctx_set_transport_out_desc(app_msg_ctx, env, 
            sandesha2_utils_get_transport_out(env));
    }
    axis2_msg_ctx_set_current_handler_index(app_msg_ctx, env, 
        axis2_msg_ctx_get_current_handler_index(app_msg_ctx, env) + 1);
    engine = axis2_engine_create(env, axis2_msg_ctx_get_conf_ctx(app_msg_ctx, 
        env));
    status = axis2_engine_resume_send(engine, env, app_msg_ctx);
    if(engine)
        axis2_engine_free(engine, env);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2] Exit:sandesha2_app_msg_processor_process_response_msg");
    return status;
}


static long AXIS2_CALL                 
sandesha2_app_msg_processor_get_prev_msg_no(
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id,
    sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    sandesha2_seq_property_bean_t *next_msg_no_bean = NULL;
    long next_msg_no = -1;
    
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    
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
        sandesha2_seq_property_bean_free(next_msg_no_bean, env);
    }
    return next_msg_no;
}

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id,
    long msg_num,
    sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    sandesha2_seq_property_bean_t *next_msg_no_bean = NULL;
    axis2_bool_t update = AXIS2_TRUE;
    axis2_char_t str_long[32];
    
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Entry:sandesha2_app_msg_processor_set_next_msg_no");
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    
    if(msg_num <= 0)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
        "[sandesha2] Invalid Message Number");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, 
            AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
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
    if(next_msg_no_bean)
        sandesha2_seq_property_bean_free(next_msg_no_bean, env);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_app_msg_processor_set_next_msg_no");
	
    return AXIS2_SUCCESS;
}

