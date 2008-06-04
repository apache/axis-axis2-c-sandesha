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
#include <sandesha2_terminate_mgr.h>

#include <axis2_const.h>
#include <axis2_msg_ctx.h>
#include <axutil_string.h>
#include <axis2_engine.h>
#include <axutil_uuid_gen.h>
#include <axis2_relates_to.h>
#include <axiom_soap_const.h>
#include <axiom_soap_body.h>
#include <axis2_http_transport_utils.h>
#include <axis2_addr.h>

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
                  	
static axis2_bool_t AXIS2_CALL
sandesha2_app_msg_processor_check_for_response_msg(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *msg_ctx);

static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_process_create_seq_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *create_seq_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr);

static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_process_app_msg_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *app_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr);

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_app_msg_processor_send_create_seq_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx,
    axis2_char_t *internal_seq_id,
    axis2_char_t *acks_to,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_sender_mgr_t *sender_mgr);

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_send_app_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *msg_ctx,
    axis2_char_t *internal_seq_id,
    long msg_num,
    axis2_char_t *storage_key,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_sender_mgr_t *sender_mgr);

static axis2_status_t
sandesha2_app_msg_processor_resend(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id,
    axis2_bool_t is_svr_side,
    const axis2_char_t *internal_seq_id,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_sender_mgr_t *sender_mgr);

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

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_app_msg_processor_create(
    const axutil_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *msg_proc_impl = NULL;
          
    msg_proc_impl =  (sandesha2_app_msg_processor_impl_t *)AXIS2_MALLOC (env->allocator, 
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
    axis2_char_t *rmd_sequence_id = NULL;
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
    /*axis2_char_t *last_out_msg_no_str = NULL;*/
   
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
    if(processed && !axutil_strcmp(processed, "true"))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Message already processed. So returning here");
        return AXIS2_SUCCESS;
    }
    
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    /*axis2_op_ctx_set_in_use(op_ctx, env, AXIS2_TRUE);*/
    axis2_op_ctx_set_response_written(op_ctx, env, AXIS2_TRUE);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
    fault_ctx = sandesha2_fault_mgr_check_for_last_msg_num_exceeded(env, rm_msg_ctx, seq_prop_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);

		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]sandesha2_app_msg_processor_process_in_msg send Fault");

		if(!axis2_engine_send_fault(engine, env, sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
		    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]An error occured while sending the fault");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT, AXIS2_FAILURE);
            if(fault_ctx)
            {
                sandesha2_msg_ctx_free(fault_ctx, env);
            }
            if(engine)
            {
                axis2_engine_free(engine, env);
            }
            if(storage_mgr)
            {
                sandesha2_storage_mgr_free(storage_mgr, env);
            }
            return AXIS2_FAILURE;
        }

        if(fault_ctx)
        {
            sandesha2_msg_ctx_free(fault_ctx, env);
        }
        if(engine)
        {
            axis2_engine_free(engine, env);
        }
        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);
        if(storage_mgr)
        {
            sandesha2_storage_mgr_free(storage_mgr, env);
        }
        return AXIS2_SUCCESS;
    }

    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(env, dbname);
    next_msg_mgr = sandesha2_permanent_next_msg_mgr_create(env, dbname);
    invoker_mgr = sandesha2_permanent_invoker_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);

    seq = sandesha2_msg_ctx_get_sequence(rm_msg_ctx, env);
    sandesha2_seq_set_must_understand(seq, env, AXIS2_FALSE);
    rmd_sequence_id = sandesha2_identifier_get_identifier(sandesha2_seq_get_identifier(seq, env), env);
    fault_ctx = sandesha2_fault_mgr_check_for_unknown_seq(env,rm_msg_ctx, rmd_sequence_id, seq_prop_mgr, 
            create_seq_mgr, next_msg_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);

		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2] sandesha2_app_msg_processor_process_in_msg send Fault");

        if(seq_prop_mgr)
        {
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        }
        if(create_seq_mgr)
        {
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        }
        if(sender_mgr)
        {
            sandesha2_sender_mgr_free(sender_mgr, env);
        }
        if(next_msg_mgr)
        {
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        }
        if(invoker_mgr)
        {
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        }
        if(storage_mgr)
        {
            sandesha2_storage_mgr_free(storage_mgr, env);
        }

        if(!axis2_engine_send_fault(engine, env, sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
		    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]An error occured while sending the fault");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT, AXIS2_FAILURE);
            if(fault_ctx)
            {
                sandesha2_msg_ctx_free(fault_ctx, env);
            }
            if(engine)
            {
                axis2_engine_free(engine, env);
            }
            return AXIS2_FAILURE;
        }

        if(fault_ctx)
        {
            sandesha2_msg_ctx_free(fault_ctx, env);
        }
        if(engine)
        {
            axis2_engine_free(engine, env);
        }
        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);

        return AXIS2_SUCCESS;
    }

    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    fault_ctx = sandesha2_fault_mgr_check_for_seq_closed(env, rm_msg_ctx, rmd_sequence_id, seq_prop_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = axis2_engine_create(env, conf_ctx);

		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]sandesha2_app_msg_processor_process_in_msg send Fault");

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

        if(!axis2_engine_send_fault(engine, env, sandesha2_msg_ctx_get_msg_ctx(fault_ctx, env)))
        {
		    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]An error occured while sending the fault");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_FAULT, AXIS2_FAILURE);
            if(fault_ctx)
            {
                sandesha2_msg_ctx_free(fault_ctx, env);
            }
            if(engine)
            {
                axis2_engine_free(engine, env);
            }
            return AXIS2_FAILURE;
        }

        if(fault_ctx)
        {
            sandesha2_msg_ctx_free(fault_ctx, env);
        }
        if(engine)
        {
            axis2_engine_free(engine, env);
        }

        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);

        return AXIS2_SUCCESS;
    }

    sandesha2_seq_mgr_update_last_activated_time(env, rmd_sequence_id, seq_prop_mgr);
    msgs_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id, 
        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
    msg_no = sandesha2_msg_number_get_msg_num(sandesha2_seq_get_msg_num(seq, env), env);

    if(0 == msg_no)
    {
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Invalid message number");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, AXIS2_FAILURE);
        if(seq_prop_mgr)
        {
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        }
        if(create_seq_mgr)
        {
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        }
        if(sender_mgr)
        {
            sandesha2_sender_mgr_free(sender_mgr, env);
        }
        if(next_msg_mgr)
        {
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
        }
        if(invoker_mgr)
        {
            sandesha2_invoker_mgr_free(invoker_mgr, env);
        }
        if(storage_mgr)
        {
            sandesha2_storage_mgr_free(storage_mgr, env);
        }

        return AXIS2_FAILURE;
    }
    highest_in_msg_no_str = sandesha2_utils_get_seq_property(env, rmd_sequence_id, 
            SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, seq_prop_mgr);
    highest_in_msg_key_str = sandesha2_utils_get_seq_property(env, rmd_sequence_id, 
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
        
        highest_in_msg_no = msg_no;
        msg_id = axis2_msg_ctx_get_msg_id(msg_ctx, env);
        highest_msg_no_bean = sandesha2_seq_property_bean_create_with_data(env, 
            rmd_sequence_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, 
                msg_num_str);
        highest_msg_key_bean = sandesha2_seq_property_bean_create_with_data(env, 
            rmd_sequence_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, 
                highest_in_msg_key_str);
        highest_msg_id_bean = sandesha2_seq_property_bean_create_with_data(env, 
            rmd_sequence_id, SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_ID, 
                (axis2_char_t *)msg_id);
        sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
            highest_in_msg_key_str, conf_ctx, -1);
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, 
            highest_in_msg_key_str, msg_ctx);
        
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
        sandesha2_seq_property_bean_set_seq_id(msgs_bean, env, rmd_sequence_id);
        sandesha2_seq_property_bean_set_name(msgs_bean, env, 
            SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
        sandesha2_seq_property_bean_set_value(msgs_bean, env, msgs_str);
    }
    msg_no_present_in_list = sandesha2_app_msg_processor_msg_num_is_in_list(env, msgs_str, msg_no);
    if(msg_no_present_in_list && !axutil_strcmp(SANDESHA2_QOS_DEFAULT_INVOCATION_TYPE, 
                SANDESHA2_QOS_EXACTLY_ONCE))
    {
        sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
    }

    if(!msg_no_present_in_list)
    {
        if(msgs_str && 0 < axutil_strlen(msgs_str))
        {
            msgs_str = axutil_strcat(env, msgs_str, ",", msg_num_str, NULL);
        }
        else
        {
            msgs_str = axutil_strdup(env, msg_num_str);
        }
        sandesha2_seq_property_bean_set_value(msgs_bean, env, msgs_str);
        sandesha2_seq_property_mgr_update(seq_prop_mgr, env, msgs_bean);
        if(msgs_str)
        {
            AXIS2_FREE(env->allocator, msgs_str);
        }
    }

    next_msg_bean = sandesha2_next_msg_mgr_retrieve(next_msg_mgr, env, rmd_sequence_id);
    if(!next_msg_bean)
    {
		AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Sequence with seq_id:%s does not exist", rmd_sequence_id);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, AXIS2_FAILURE);
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

    in_order_invoke = sandesha2_property_bean_is_in_order(sandesha2_utils_get_property_bean(env, 
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
                env, rmd_sequence_id, SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_ID, msg_id);
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
    if(!axutil_strcmp(SANDESHA2_SPEC_2005_02_ACTION_LAST_MESSAGE, wsa_action) || 0 == axutil_strcmp(
                SANDESHA2_SPEC_2005_02_SOAP_ACTION_LAST_MESSAGE, soap_action)) 
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]Got WSRM 1.0 last message, aborting");
        sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, storage_mgr, 
                sender_mgr, seq_prop_mgr);

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
            incoming_seq_list, rmd_sequence_id))
        {
            axis2_char_t *str_seq_list = NULL;
            axutil_array_list_add(incoming_seq_list, env, rmd_sequence_id);
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
            msg_no, rmd_sequence_id, AXIS2_FALSE);
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
        sandesha2_utils_start_invoker_for_seq(env, conf_ctx, rmd_sequence_id);
    }

    rm_version = sandesha2_utils_get_rm_version(env, rmd_sequence_id, seq_prop_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Unable to find RM spec version for seq_id %s", rmd_sequence_id);
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
    {
        reply_to_addr = axis2_endpoint_ref_get_address(reply_to_epr, env);
    }
    if(to_epr)
    {
        to_addr = axis2_endpoint_ref_get_address(to_epr, env);
    }

    op = axis2_op_ctx_get_op(op_ctx, env);
    acks_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id,
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
        to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id,
            SANDESHA2_SEQ_PROP_TO_EPR);
        if(to_bean)
            reply_to_addr = sandesha2_seq_property_bean_get_value(to_bean, env); 
    }
    if(back_channel_free)
    {
        /* MEP is in only or duplex mode anonymous reply to address.
         * That means we can send an acknowledgment message in the http back channel.
         */
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Back channel is free");
        sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, storage_mgr, 
                sender_mgr, seq_prop_mgr);
    }
    else if(sandesha2_utils_is_single_channel(env, rm_version, acks_to_str))
    {
        /* This means acknowledgment address is anomymous and RM version is 1.0.
         * In other words single channel duplex mode. In this case we do not send the
         * acknowledgment message here. Instead we send it in the message out path.
         * See sandesha2_app_msg_processor_send_app_msg() code.
         */
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Single channel duplex mode");
    
        sandesha2_seq_t *sequence = sandesha2_msg_ctx_get_sequence(rm_msg_ctx, env);
        axis2_char_t *rmd_sequence_id = sandesha2_identifier_get_identifier(sandesha2_seq_get_identifier(sequence, 
                env), env);
        sandesha2_seq_property_bean_t *rms_internal_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            rmd_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_INTERNAL_SEQ_ID);
   
        axis2_char_t *rms_internal_sequence_id = sandesha2_seq_property_bean_get_value(rms_internal_sequence_bean, env);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "rms_internal_sequence_id:%s", rms_internal_sequence_id);
        
    } 
    else
    {
        /* This is most probably dual channel duplex mode. */
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Most probably dual channel duplex mode.");
        sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, storage_mgr, 
                sender_mgr, seq_prop_mgr);
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
    axis2_char_t *rms_internal_sequence_id = NULL;
    axis2_char_t *storage_key = NULL;
    axis2_bool_t last_msg = AXIS2_FALSE;
    axutil_property_t *property = NULL;
    long msg_num_lng = -1;
    long system_msg_num = -1;
    long msg_number = -1;
    axis2_char_t *dummy_msg_str = NULL;
    axis2_bool_t dummy_msg = AXIS2_FALSE;
    axis2_char_t *rmd_sequence_id = NULL;

    /*axis2_bool_t seq_timed_out = AXIS2_FALSE;*/

    sandesha2_seq_property_bean_t *res_highest_msg_bean = NULL;
    axis2_char_t msg_number_str[32];
    axis2_bool_t send_create_seq = AXIS2_FALSE;
    axis2_char_t *spec_ver = NULL;
    axiom_soap_envelope_t *soap_env = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    sandesha2_seq_property_bean_t *rms_sequence_bean = NULL;
    axis2_char_t *op_name = NULL;
    axis2_char_t *to_addr = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_msg_ctx_t *req_msg_ctx = NULL;
    axis2_relates_to_t *relates_to = NULL;
    axis2_char_t *dbname = NULL;
    sandesha2_seq_property_bean_t *seq_timeout_bean = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
    axis2_char_t *client_sequence_key = NULL;
    
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Entry:sandesha2_app_msg_processor_process_out_msg");
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
  
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    req_msg_ctx =  axis2_op_ctx_get_msg_ctx(op_ctx, env, AXIS2_WSDL_MESSAGE_LABEL_IN);
    /* TODO setting up fault callback */

    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    if(!seq_prop_mgr)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]seq_prop_mgr is NULL");
        if(storage_mgr)
        {
            sandesha2_storage_mgr_free(storage_mgr, env);
        }
        return AXIS2_FAILURE;
    }
    create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);

    is_svr_side = axis2_msg_ctx_get_server_side(msg_ctx, env);
    
    to_epr = axis2_msg_ctx_get_to(msg_ctx, env);
    if((!to_epr || !axis2_endpoint_ref_get_address(to_epr, env) || 0 == axutil_strlen(
                    axis2_endpoint_ref_get_address(to_epr, env))) && !is_svr_side)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]To epr is not set - a requirement in sandesha client side");

        if(seq_prop_mgr)
        {
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        }
        if(create_seq_mgr)
        {
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        }
        if(sender_mgr)
        {
            sandesha2_sender_mgr_free(sender_mgr, env);
        }
        if(storage_mgr)
        {
            sandesha2_storage_mgr_free(storage_mgr, env);
        }

        return AXIS2_FAILURE;
    }
    
    if(!axis2_msg_ctx_get_msg_id(msg_ctx, env))
    {
        axis2_msg_ctx_set_message_id(msg_ctx, env, axutil_uuid_gen(env));
    }

    if(is_svr_side)
    {
        sandesha2_seq_t *req_seq = NULL;
        long request_msg_no = -1;
        const axis2_relates_to_t *relates_to = NULL;
        axis2_char_t *relates_to_value = NULL;
        axis2_char_t *last_req_id = NULL;
       
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        req_seq = sandesha2_msg_ctx_get_sequence(req_rm_msg_ctx, env);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence is NULL");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, AXIS2_FAILURE);
            if(req_rm_msg_ctx)
            {
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            }
            if(seq_prop_mgr)
            {
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            }
            if(create_seq_mgr)
            {
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            }
            if(sender_mgr)
            {
                sandesha2_sender_mgr_free(sender_mgr, env);
            }
            if(storage_mgr)
            {
                sandesha2_storage_mgr_free(storage_mgr, env);
            }

            return AXIS2_FAILURE;
        }

        rmd_sequence_id = sandesha2_identifier_get_identifier(sandesha2_seq_get_identifier(req_seq, 
                    env), env);
        if(!rmd_sequence_id)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence ID is NULL");
            if(req_rm_msg_ctx)
            {
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            }
            if(seq_prop_mgr)
            {
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            }
            if(create_seq_mgr)
            {
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            }
            if(sender_mgr)
            {
                sandesha2_sender_mgr_free(sender_mgr, env);
            }
            if(storage_mgr)
            {
                sandesha2_storage_mgr_free(storage_mgr, env);
            }

            return AXIS2_FAILURE;
        }

        request_msg_no = sandesha2_msg_number_get_msg_num(sandesha2_seq_get_msg_num(req_seq, env), env);
        rms_internal_sequence_id = sandesha2_utils_get_rms_internal_sequence_id(env, is_svr_side, rmd_sequence_id);

        /* Deciding whether this is the last message. We assume it is if it 
         * relates to a message which arrived with the LastMessage flag on it.
         */
        last_req_id = sandesha2_utils_get_seq_property(env, rmd_sequence_id, 
            SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_ID, seq_prop_mgr);

        relates_to = axis2_msg_ctx_get_relates_to(msg_ctx, env);
        relates_to_value = (axis2_char_t *)axis2_relates_to_get_value(relates_to, env);
        if(relates_to && last_req_id && !axutil_strcmp(last_req_id, relates_to_value))
        {
            last_msg = AXIS2_TRUE;
        }

        if(last_req_id)
        {
            AXIS2_FREE(env->allocator, last_req_id);
        }
    }
    else /* Client side */
    {
        axis2_char_t *to = NULL;
        axis2_char_t *last_app_msg = NULL;
        sandesha2_seq_property_bean_t *client_sequence_key_bean = NULL;
        
        to = (axis2_char_t*)axis2_endpoint_ref_get_address(to_epr, env);
        property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY);
        if(property)
        {
            client_sequence_key = axutil_property_get_value(property, env);
        }

        if(!client_sequence_key)
        {
            client_sequence_key = axutil_uuid_gen(env);
            property = axutil_property_create_with_args(env, 0, 0, 0, client_sequence_key);
            axis2_msg_ctx_set_property(msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY, property);
        }

        client_sequence_key_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, client_sequence_key, 
                SANDESHA2_CLIENT_SEQ_KEY);

        if(client_sequence_key_bean)
        {
            rms_internal_sequence_id = sandesha2_seq_property_bean_get_value(client_sequence_key_bean, env);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "rms_internal_sequence_id:%s", rms_internal_sequence_id);
        }

        if(!rms_internal_sequence_id)
        {
            rms_internal_sequence_id = sandesha2_utils_get_client_rms_internal_sequence_id(env, to, 
                client_sequence_key);
        }

        property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_LAST_MESSAGE);
        if(property)
        {
            last_app_msg = axutil_property_get_value(property, env);
        }

        if(last_app_msg && !axutil_strcmp(last_app_msg, AXIS2_VALUE_TRUE))
        {
            axis2_char_t *spec_ver = NULL;
            spec_ver = sandesha2_utils_get_rm_version(env, rms_internal_sequence_id, seq_prop_mgr);
            if(!spec_ver)
            {
                axutil_property_t *spec_ver_prop = NULL;
                spec_ver_prop = axis2_msg_ctx_get_property(msg_ctx, env, 
                    SANDESHA2_CLIENT_RM_SPEC_VERSION);

                spec_ver = axutil_strdup(env, axutil_property_get_value(spec_ver_prop, env));
            }
            if(sandesha2_spec_specific_consts_is_last_msg_indicator_reqd(env, spec_ver))
            {
                last_msg = AXIS2_TRUE;
            }
            if(spec_ver)
            {
                AXIS2_FREE(env->allocator, spec_ver);
            }
        }
    }

    seq_timeout_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rms_internal_sequence_id, 
            SANDESHA2_SEQ_PROP_SEQ_TIMED_OUT);

    if(seq_timeout_bean && sandesha2_seq_property_bean_get_value(seq_timeout_bean, env) 
        && !axutil_strcmp(AXIS2_VALUE_TRUE, sandesha2_seq_property_bean_get_value(seq_timeout_bean, 
                env)))
    {
        axis2_char_t *temp_int_seq_id = sandesha2_seq_property_bean_get_seq_id(seq_timeout_bean, env);
        axis2_char_t *temp_name = sandesha2_seq_property_bean_get_name(seq_timeout_bean, env);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Removing the sequence property named %s in the sequence %s", temp_name, 
                temp_int_seq_id);
        sandesha2_seq_property_mgr_remove(seq_prop_mgr, env, temp_int_seq_id, temp_name);

        if(req_rm_msg_ctx)
        {
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
        }
        if(rms_internal_sequence_id)
        {
                AXIS2_FREE(env->allocator, rms_internal_sequence_id);
        }
        if(seq_prop_mgr)
        {
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        }
        if(create_seq_mgr)
        {
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        }
        if(sender_mgr)
        {
            sandesha2_sender_mgr_free(sender_mgr, env);
        }
        if(storage_mgr)
        {
            sandesha2_storage_mgr_free(storage_mgr, env);
        }

        /* We should halt the system here. Otherwise application client keep on sending messages. */
        exit(AXIS2_FAILURE);
    }

    property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_MESSAGE_NUMBER);
    if(property)
    {
        msg_num_lng = *(long*)(axutil_property_get_value(property, env));
        if(msg_num_lng <= 0)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Invalid message number");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, AXIS2_FAILURE);

            if(req_rm_msg_ctx)
            {
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            }
            if(rms_internal_sequence_id)
            {
                AXIS2_FREE(env->allocator, rms_internal_sequence_id);
            }
            if(seq_prop_mgr)
            {
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            }
            if(create_seq_mgr)
            {
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            }
            if(sender_mgr)
            {
                sandesha2_sender_mgr_free(sender_mgr, env);
            }
            if(storage_mgr)
            {
                sandesha2_storage_mgr_free(storage_mgr, env);
            }

            return AXIS2_FAILURE;
        }
    }

    system_msg_num = sandesha2_app_msg_processor_get_prev_msg_no(env, rms_internal_sequence_id, seq_prop_mgr);

    if(msg_num_lng > 0 && msg_num_lng <= system_msg_num)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Invalid Message Number");

        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, AXIS2_FAILURE);
        
        if(req_rm_msg_ctx)
        {
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
        }
        if(rms_internal_sequence_id)
        {
            AXIS2_FREE(env->allocator, rms_internal_sequence_id);
        }
        if(seq_prop_mgr)
        {
            sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
        }
        if(create_seq_mgr)
        {
            sandesha2_create_seq_mgr_free(create_seq_mgr, env);
        }
        if(sender_mgr)
        {
            sandesha2_sender_mgr_free(sender_mgr, env);
        }
        if(storage_mgr)
        {
            sandesha2_storage_mgr_free(storage_mgr, env);
        }

        return AXIS2_FAILURE;
    }

    if(msg_num_lng > 0)
    {
        msg_number = msg_num_lng;
    }
    else if(system_msg_num > 0)
    {
        msg_number = system_msg_num + 1;
    }
    else
    {
        msg_number = 1;
    }

    if(is_svr_side)
    {
        sandesha2_seq_property_bean_t *rmd_to_bean = NULL;
        
        rmd_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id, 
                SANDESHA2_SEQ_PROP_TO_EPR);
        if(rmd_to_bean)
        {
            axis2_char_t *rmd_to = NULL;
            axis2_char_t *value = NULL;
    
            value = sandesha2_seq_property_bean_get_value(rmd_to_bean, env);
            rmd_to = axutil_strdup(env, value);
            property = axutil_property_create_with_args(env, 0, 0, 0, rmd_to);
            axis2_msg_ctx_set_property(msg_ctx, env, SANDESHA2_SEQ_PROP_TO_EPR, property);
        }
    }



    rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            rms_internal_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);

    if(1 == msg_number)
    {
        if(!rms_sequence_bean)
        {
            send_create_seq = AXIS2_TRUE;
        }

        if(!is_svr_side)
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
                property = axutil_property_create_with_args(env, AXIS2_SCOPE_APPLICATION, AXIS2_TRUE, 
                        axutil_hash_free_void_arg, msg_ctx_map);

                axis2_ctx_set_property(ctx, env, SANDESHA2_MSG_CTX_MAP, property);
            }
        }

        /* Determine spec version */
        if(is_svr_side)
        {
            sandesha2_seq_property_bean_t *spec_ver_bean = NULL;
           
            spec_ver_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id, 
                    SANDESHA2_SEQ_PROP_RM_SPEC_VERSION);

            if(req_rm_msg_ctx)
            {
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            }
            if(!spec_ver_bean)
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                    "[sandesha2] Invalid spec version");
                AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_SPEC_VERSION,
                    AXIS2_FAILURE);
                if(rms_internal_sequence_id)
                {
                    AXIS2_FREE(env->allocator, rms_internal_sequence_id);
                }
                if(seq_prop_mgr)
                {
                    sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
                }
                if(create_seq_mgr)
                {
                    sandesha2_create_seq_mgr_free(create_seq_mgr, env);
                }
                if(sender_mgr)
                {
                    sandesha2_sender_mgr_free(sender_mgr, env);
                }
                if(storage_mgr)
                {
                    sandesha2_storage_mgr_free(storage_mgr, env);
                }

                return AXIS2_FAILURE;
            }

            spec_ver = sandesha2_seq_property_bean_get_value(spec_ver_bean, env);
        }
        else
        {
            property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_RM_SPEC_VERSION);
            if(property)
            {
                spec_ver = axutil_property_get_value(property, env);
            }
        } /* End of determining spec version */

        if(!spec_ver)
        {
            spec_ver = sandesha2_spec_specific_consts_get_default_spec_version(env);
        }

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Spec version:%s", spec_ver);
        sandesha2_seq_mgr_setup_temporary_rms_sequence(env, msg_ctx, rms_internal_sequence_id, spec_ver, 
            client_sequence_key, seq_prop_mgr);

        if(send_create_seq)
        {
            sandesha2_seq_property_bean_t *create_seq_added = NULL;
            sandesha2_create_seq_bean_t *updated_create_seq_bean = NULL;

            create_seq_added = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rms_internal_sequence_id, 
                    SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT);

            if(!create_seq_added)
            {
                axis2_char_t *acks_to = NULL;
                axis2_char_t *addr_ns_uri = NULL;
                axis2_char_t *anon_uri = NULL;
                
                create_seq_added = sandesha2_seq_property_bean_create_with_data(env, 
                        rms_internal_sequence_id, SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT, AXIS2_VALUE_TRUE);

                sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, create_seq_added);

                /*addr_ns_uri = sandesha2_utils_get_seq_property(env, rms_internal_sequence_id, 
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, seq_prop_mgr);*/
                property = axis2_msg_ctx_get_property(msg_ctx, env, AXIS2_WSA_VERSION);
                if(property)
                {
                    addr_ns_uri = axutil_property_get_value(property, env);
                }

                if(!addr_ns_uri)
                {
                    axis2_op_ctx_t *op_ctx = NULL;
                    axis2_msg_ctx_t *req_msg_ctx = NULL;

                    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
                    req_msg_ctx =  axis2_op_ctx_get_msg_ctx(op_ctx, env, AXIS2_WSDL_MESSAGE_LABEL_IN);

                    if(req_msg_ctx)
                    {
                        property = axis2_msg_ctx_get_property(req_msg_ctx, env, AXIS2_WSA_VERSION);
                        if(property)
                        {
                            addr_ns_uri = axutil_property_get_value(property, env);
                        }
                    }
                }

                if(!addr_ns_uri)
                {
                    addr_ns_uri = AXIS2_WSA_NAMESPACE;
                }

                anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);

                if(axis2_msg_ctx_get_svc_ctx(msg_ctx, env))
                {
                    property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_ACKS_TO);
                    if(property)
                    {
                        acks_to = axutil_property_get_value(property, env);
                    }
                }

                if(is_svr_side)
                {
                    axis2_endpoint_ref_t *acks_to_epr = NULL;

                    acks_to_epr = axis2_msg_ctx_get_to(req_msg_ctx, env);
                    acks_to = (axis2_char_t*)axis2_endpoint_ref_get_address(acks_to_epr, env);
                }
                else if(!acks_to)
                {
                    acks_to = anon_uri;
                }
                
                if(!acks_to && is_svr_side)
                {
                    sandesha2_seq_property_bean_t *reply_to_epr_bean = NULL;
                    
                    reply_to_epr_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
                            rmd_sequence_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
                    if(reply_to_epr_bean)
                    {
                        axis2_endpoint_ref_t *acks_epr = NULL;

                        acks_epr = axis2_endpoint_ref_create(env, 
                                sandesha2_seq_property_bean_get_value(reply_to_epr_bean, env));
                        if(acks_epr)
                        {
                            acks_to = (axis2_char_t*)axis2_endpoint_ref_get_address(acks_epr, env);
                        }
                    }
                }


                /**
                 * else if()
                 * TODO handle acks_to == anon_uri case
                 */
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "acks_to:%s", acks_to);
                updated_create_seq_bean = sandesha2_app_msg_processor_send_create_seq_msg(env, rm_msg_ctx, 
                        rms_internal_sequence_id, acks_to, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);
                rms_internal_sequence_id = sandesha2_create_seq_bean_get_rms_internal_sequence_id(
                        updated_create_seq_bean, env);
            }
        }
        sandesha2_seq_mgr_setup_new_rms_sequence(env, msg_ctx, rms_internal_sequence_id, spec_ver, 
                seq_prop_mgr);
        
    }

    /* A dummy message is a one which will not be processed as a actual 
     * application message. The RM handlers will simply let these go.
     */
    property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_DUMMY_MESSAGE);
    if(property)
    {
        dummy_msg_str = axutil_property_get_value(property, env);
    }

    if(dummy_msg_str && 0 == axutil_strcmp(dummy_msg_str, AXIS2_VALUE_TRUE))
    {
        dummy_msg = AXIS2_TRUE;
    }

    if(!dummy_msg)
    {
        sandesha2_app_msg_processor_set_next_msg_no(env, rms_internal_sequence_id, msg_number, seq_prop_mgr);
    }

    sprintf(msg_number_str, "%ld", msg_number); 
    res_highest_msg_bean = sandesha2_seq_property_bean_create_with_data(env, rms_internal_sequence_id, 
            SANDESHA2_SEQ_PROP_HIGHEST_OUT_MSG_NUMBER, msg_number_str);

    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, res_highest_msg_bean);
    if(res_highest_msg_bean)
    {
        sandesha2_seq_property_bean_free(res_highest_msg_bean, env);
    }

    relates_to = axis2_msg_ctx_get_relates_to(msg_ctx, env);
    if(relates_to)
    {
        sandesha2_seq_property_bean_t *response_relates_to_bean = NULL; 
        const axis2_char_t *relates_to_value = axis2_relates_to_get_value(relates_to, env);

        response_relates_to_bean = sandesha2_seq_property_bean_create_with_data(env, 
                rms_internal_sequence_id, SANDESHA2_SEQ_PROP_HIGHEST_OUT_RELATES_TO, 
                (axis2_char_t *) relates_to_value);

        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, response_relates_to_bean);
    }

    storage_key = axutil_uuid_gen(env);

    if(last_msg)
    {
        sandesha2_seq_property_bean_t *res_last_msg_key_bean = NULL;
       
        res_last_msg_key_bean = sandesha2_seq_property_bean_create_with_data(env, 
                rms_internal_sequence_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO, msg_number_str);

        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, res_last_msg_key_bean);
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
        /* Let the request end with 202 if a ack has not been
         * written in the incoming thread
         */
        axis2_ctx_t *ctx = NULL;
        axis2_char_t *written = NULL;
        
        ctx = axis2_op_ctx_get_base(op_ctx, env);
        property = axis2_ctx_get_property(ctx, env, SANDESHA2_ACK_WRITTEN);
        if(property)
        {
            written = axutil_property_get_value(property, env);
        }

        if(!written || axutil_strcmp(written, AXIS2_VALUE_TRUE))
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
    {
        to_addr = (axis2_char_t*)axis2_endpoint_ref_get_address(to_epr, env);
    }

    if(!axis2_msg_ctx_get_wsa_action(msg_ctx, env))
    {
        axis2_msg_ctx_set_wsa_action(msg_ctx, env, to_addr);
    }

    if(!axis2_msg_ctx_get_soap_action(msg_ctx, env))
    {
        axutil_string_t *soap_action = axutil_string_create(env, to_addr);
        axis2_msg_ctx_set_soap_action(msg_ctx, env, soap_action);
    }
    
    if(!dummy_msg)
    {
        status = sandesha2_app_msg_processor_send_app_msg(env, rm_msg_ctx, rms_internal_sequence_id, 
                msg_number, storage_key, storage_mgr, create_seq_mgr, seq_prop_mgr, sender_mgr);
    }

    axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);

    if(rms_internal_sequence_id)
    {
        AXIS2_FREE(env->allocator, rms_internal_sequence_id);
    }
    if(seq_prop_mgr)
    {
        sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
    }
    if(create_seq_mgr)
    {
        sandesha2_create_seq_mgr_free(create_seq_mgr, env);
    }
    if(sender_mgr)
    {
        sandesha2_sender_mgr_free(sender_mgr, env);
    }
    if(storage_mgr)
    {
        sandesha2_storage_mgr_free(storage_mgr, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Exit:sandesha2_app_msg_processor_process_out_msg");

    return status;
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
    sandesha2_seq_t *sequence = NULL;
    axis2_char_t *rmd_sequence_id = NULL;
    axis2_char_t *rms_internal_sequence_id = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_ack_requested_t *ack_requested = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg = NULL;
    axis2_msg_ctx_t *ack_msg_ctx = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_bool_t is_svr_side = AXIS2_FALSE;
    axis2_bool_t sent = AXIS2_FALSE;
    sandesha2_seq_property_bean_t *int_seq_bean = NULL;
    sandesha2_seq_property_bean_t *rms_internal_sequence_bean = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[Sandesha2] Entry:sandesha2_app_msg_processor_send_ack_if_reqd");

    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_str, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);

    sequence = sandesha2_msg_ctx_get_sequence(rm_msg_ctx, env);
    rmd_sequence_id = sandesha2_identifier_get_identifier(sandesha2_seq_get_identifier(sequence, 
                env), env);

    conf_ctx = axis2_msg_ctx_get_conf_ctx(sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env), env);
    if(!conf_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] cont_ctx is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    
    is_svr_side = sandesha2_msg_ctx_get_server_side(rm_msg_ctx, env);
    ack_requested = sandesha2_msg_ctx_get_ack_requested(rm_msg_ctx, env);
    if(ack_requested)
    {
        sandesha2_ack_requested_set_must_understand(ack_requested, env, AXIS2_FALSE);
        sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    }

    ack_rm_msg = sandesha2_ack_mgr_generate_ack_msg(env, rm_msg_ctx, rmd_sequence_id, seq_prop_mgr);
    ack_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(ack_rm_msg, env);

    rms_internal_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            rmd_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_INTERNAL_SEQ_ID);
   
    rms_internal_sequence_id = sandesha2_seq_property_bean_get_value(rms_internal_sequence_bean, env);

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "rms_internal_sequence_id:%s", rms_internal_sequence_id);

    int_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id, 
            SANDESHA2_SEQ_PROP_SPECIAL_INTERNAL_SEQ_ID);
    {
        const axis2_char_t *related_msg_id = NULL;
        const axis2_relates_to_t *relates_to = NULL;
        axis2_endpoint_ref_t *reply_to_epr = NULL;
        long send_time = -1;
        axis2_char_t *key = NULL;
        axutil_property_t *property = NULL;
        sandesha2_sender_bean_t *ack_bean = NULL;
        axis2_char_t *int_seq_id = NULL;

        /* Taking from epr and testing for it is temporary code until AXIS2C-1161 is fixed */
        axis2_endpoint_ref_t *from = sandesha2_msg_ctx_get_from(rm_msg_ctx, env);
        relates_to = sandesha2_msg_ctx_get_relates_to(rm_msg_ctx, env);
        if(relates_to && from)
        {
            related_msg_id = axis2_relates_to_get_value(relates_to, env);
            int_seq_id = sandesha2_utils_get_seq_property(env, related_msg_id, 
                SANDESHA2_MSG_CTX_PROP_INTERNAL_SEQUENCE_ID, seq_prop_mgr);

            key = axutil_uuid_gen(env);
            ack_bean = sandesha2_sender_bean_create(env);
            sandesha2_sender_bean_set_msg_ctx_ref_key(ack_bean, env, key);
            sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, key, ack_msg_ctx);
            send_time = sandesha2_utils_get_current_time_in_millis(env);
            sandesha2_sender_bean_set_time_to_send(ack_bean, env, send_time);
            sandesha2_sender_bean_set_msg_id(ack_bean, env, sandesha2_msg_ctx_get_msg_id(ack_rm_msg, env));
            sandesha2_sender_bean_set_send(ack_bean, env, AXIS2_TRUE);
            sandesha2_sender_bean_set_internal_seq_id(ack_bean, env, int_seq_id);
            sandesha2_sender_bean_set_seq_id(ack_bean, env, rmd_sequence_id);
            sandesha2_sender_bean_set_msg_type(ack_bean, env, SANDESHA2_MSG_TYPE_ACK);
            sandesha2_sender_bean_set_resend(ack_bean, env, AXIS2_FALSE);
            sandesha2_sender_mgr_insert(sender_mgr, env, ack_bean);
            if(ack_bean)
            {
                sandesha2_sender_bean_free(ack_bean, env);
            }
            property = axutil_property_create_with_args(env, 0, AXIS2_TRUE, 0, key);
            axis2_msg_ctx_set_property(ack_msg_ctx, env, SANDESHA2_MESSAGE_STORE_KEY, property);

            reply_to_epr = axis2_msg_ctx_get_to(msg_ctx, env);

            if(reply_to_epr)
            {
                axis2_msg_ctx_set_reply_to(ack_msg_ctx, env, reply_to_epr);
            }

            return AXIS2_SUCCESS;
        }
    }
    if(ack_rm_msg)
    {
        axis2_engine_t *engine = NULL;
        engine = axis2_engine_create(env, conf_ctx);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] axis2_engine_send");

        sent = axis2_engine_send(engine, env, ack_msg_ctx);
        if(ack_rm_msg)
        {
            sandesha2_msg_ctx_free(ack_rm_msg, env);
        }
        if(engine)
        {
            axis2_engine_free(engine, env);
        }
    }
    if(!sent)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[Sandesha2] Engine Send failed");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SENDING_ACK, AXIS2_FAILURE);
    
        return AXIS2_FAILURE;
    }
        
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
            "[Sandesha2] Exit:sandesha2_app_msg_processor_send_ack_if_reqd");
    return AXIS2_SUCCESS;
}
                    	
static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_app_msg_processor_send_create_seq_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *rms_internal_sequence_id,
    axis2_char_t *acks_to,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_sender_mgr_t *sender_mgr)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    sandesha2_seq_property_bean_t *rms_sequence_bean = NULL;
    sandesha2_msg_ctx_t *create_seq_rm_msg = NULL;
    sandesha2_seq_offer_t *seq_offer = NULL;
    axis2_msg_ctx_t *create_seq_msg = NULL;
    sandesha2_create_seq_bean_t *create_seq_bean = NULL;
    sandesha2_create_seq_bean_t *updated_create_seq_bean = NULL;
    axis2_char_t *addr_ns_uri = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_char_t *create_sequence_msg_store_key = NULL;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    axis2_engine_t *engine = NULL;
    axis2_op_t *create_seq_op = NULL;
    axis2_op_ctx_t *create_seq_op_ctx = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    axis2_char_t *create_seq_msg_id = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Entry:sandesha2_app_msg_processor_send_create_seq_msg");
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rms_internal_sequence_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, acks_to, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, create_seq_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, sender_mgr, AXIS2_FAILURE);
  
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);

    create_seq_rm_msg = sandesha2_msg_creator_create_create_seq_msg(env, rm_msg_ctx, rms_internal_sequence_id, 
            acks_to, seq_prop_mgr);

    if(!create_seq_rm_msg)
    {
        return AXIS2_FAILURE;
    }

    sandesha2_msg_ctx_set_flow(create_seq_rm_msg, env, SANDESHA2_MSG_CTX_OUT_FLOW);

    create_seq_part = sandesha2_msg_ctx_get_create_seq(create_seq_rm_msg, env);
    {
        sandesha2_seq_property_bean_t *to_epr_bean = NULL;

        axis2_endpoint_ref_t *to_epr = axis2_msg_ctx_get_to(msg_ctx, env);

        if(to_epr)
        {
            axis2_char_t *to_str = (axis2_char_t *)axis2_endpoint_ref_get_address(to_epr, env);

            to_epr_bean = sandesha2_seq_property_bean_create_with_data(env, rms_internal_sequence_id, 
                    SANDESHA2_SEQ_PROP_TO_EPR, to_str);

            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, to_epr_bean);
        }
    }

    seq_offer = sandesha2_create_seq_get_seq_offer(create_seq_part, env);
    if(seq_offer)
    {
        axis2_char_t *seq_offer_id = NULL;
        sandesha2_seq_property_bean_t *offer_seq_bean = NULL;
        
        seq_offer_id = sandesha2_identifier_get_identifier(sandesha2_seq_offer_get_identifier(
                    seq_offer, env), env);
        offer_seq_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_name(offer_seq_bean, env, SANDESHA2_SEQ_PROP_OFFERED_SEQ);
        sandesha2_seq_property_bean_set_seq_id(offer_seq_bean, env, rms_internal_sequence_id);
        sandesha2_seq_property_bean_set_value(offer_seq_bean, env, seq_offer_id);
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, offer_seq_bean);
    }

    create_seq_msg = sandesha2_msg_ctx_get_msg_ctx(create_seq_rm_msg, env);
    if(!create_seq_msg)
    {
        return AXIS2_FAILURE;
    }

    axis2_msg_ctx_set_relates_to(create_seq_msg, env, NULL);
    create_sequence_msg_store_key = axutil_uuid_gen(env);
    create_seq_msg_id = (axis2_char_t*)axis2_msg_ctx_get_wsa_message_id(create_seq_msg, env);
    create_seq_bean = sandesha2_create_seq_bean_create_with_data(env, rms_internal_sequence_id, 
            create_seq_msg_id, NULL);

    sandesha2_create_seq_bean_set_ref_msg_store_key(create_seq_bean, env, create_sequence_msg_store_key);
    sandesha2_create_seq_mgr_insert(create_seq_mgr, env, create_seq_bean);

    if(!addr_ns_uri)
    {
        addr_ns_uri = sandesha2_utils_get_seq_property(env, rms_internal_sequence_id, 
                SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, seq_prop_mgr);
    }

    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    if(addr_ns_uri)
    {
        AXIS2_FREE(env->allocator, addr_ns_uri);
    }

    if(!axis2_msg_ctx_get_reply_to(create_seq_msg, env))
    {
        axis2_endpoint_ref_t *cs_epr = NULL;
        cs_epr = axis2_endpoint_ref_create(env, anon_uri);
        axis2_msg_ctx_set_reply_to(create_seq_msg, env, cs_epr);
    }

    engine = axis2_engine_create(env, axis2_msg_ctx_get_conf_ctx(create_seq_msg, env));

    if(create_seq_rm_msg)
    {
        sandesha2_msg_ctx_free(create_seq_rm_msg, env);
    }

    if(axis2_engine_send(engine, env, create_seq_msg))
    {
        status = sandesha2_app_msg_processor_process_create_seq_response(env, create_seq_msg, 
                storage_mgr);
    }
    else
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Engine Send failed");
    }
    
    if(engine)
    {
        axis2_engine_free(engine, env);
    }

    updated_create_seq_bean = sandesha2_create_seq_mgr_retrieve(create_seq_mgr, env, create_seq_msg_id);
    rms_internal_sequence_id = sandesha2_create_seq_bean_get_rms_internal_sequence_id(
            updated_create_seq_bean, env);

    rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rms_internal_sequence_id, 
            SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);
    create_seq_op = axis2_msg_ctx_get_op(create_seq_msg, env);
    transport_out = axis2_msg_ctx_get_transport_out_desc(create_seq_msg, env);
    transport_sender = axis2_transport_out_desc_get_sender(transport_out, env);
    while(!rms_sequence_bean)
    {
        if(transport_sender)
        {
            /* This is neccessary to avoid a double free */
            axis2_msg_ctx_set_property(msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
            if(!AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, create_seq_msg))
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Transport sender invoke failed");
            }
        }

        status = sandesha2_app_msg_processor_process_create_seq_response(env, create_seq_msg, 
            storage_mgr);
    
        if(AXIS2_SUCCESS != status)
        {
            break;
        }

        updated_create_seq_bean = sandesha2_create_seq_mgr_retrieve(create_seq_mgr, env, create_seq_msg_id);
        rms_internal_sequence_id = sandesha2_create_seq_bean_get_rms_internal_sequence_id(
            updated_create_seq_bean, env);

        rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rms_internal_sequence_id, 
            SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);
    }

    create_seq_op_ctx = axis2_msg_ctx_get_op_ctx(create_seq_msg, env);
    if(create_seq_op_ctx)
    {
        axis2_op_ctx_free(create_seq_op_ctx, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Exit:sandesha2_app_msg_processor_send_create_seq_msg");
    return updated_create_seq_bean;
}

static axis2_bool_t AXIS2_CALL
sandesha2_app_msg_processor_check_for_response_msg(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *msg_ctx)
{
    axiom_soap_envelope_t *res_envelope = NULL;
    axis2_char_t *soap_ns_uri = NULL;
    axis2_bool_t svr_side = AXIS2_FALSE;
   
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Entry:sandesha2_app_msg_processor_check_for_response_msg");
    
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    svr_side = axis2_msg_ctx_get_server_side(msg_ctx, env);
    soap_ns_uri = axis2_msg_ctx_get_is_soap_11(msg_ctx, env) ?
         AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
         AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;

    res_envelope = axis2_msg_ctx_get_response_soap_envelope(msg_ctx, env);
    if(!res_envelope)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Response envelope not found");

        res_envelope = (axiom_soap_envelope_t *) axis2_http_transport_utils_create_soap_msg(env, 
                msg_ctx, soap_ns_uri);
        if(!res_envelope)
        {
            /* There is no response message context. Therefore it can be deduced that this is one 
             * way message. So return.
             */
            return AXIS2_FALSE;
        }
    }

    if(svr_side)
    {
        /* We check and process the sync response only in the application client 
         * side.
         */
        return AXIS2_FALSE;
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Exit:sandesha2_app_msg_processor_check_for_response_msg");

    return AXIS2_TRUE;
}

static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_process_create_seq_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *create_seq_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_msg_ctx_t *res_msg_ctx = NULL;
    axiom_soap_envelope_t *res_envelope = NULL;
    axis2_char_t *soap_ns_uri = NULL;
    axis2_svc_ctx_t *svc_ctx = NULL;
    axis2_svc_grp_t *svc_grp = NULL;
    axis2_svc_t *svc = NULL;
    axis2_op_t *op = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_op_ctx_t *create_seq_res_op_ctx = NULL;
   
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Entry:sandesha2_app_msg_processor_process_create_seq_response");
    
    AXIS2_PARAM_CHECK(env->error, create_seq_msg_ctx, AXIS2_FAILURE);
    
    soap_ns_uri = axis2_msg_ctx_get_is_soap_11(create_seq_msg_ctx, env) ?
         AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
         AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;

    res_envelope = axis2_msg_ctx_get_response_soap_envelope(create_seq_msg_ctx, env);
    if(!res_envelope)
    {
        res_envelope = (axiom_soap_envelope_t *) axis2_http_transport_utils_create_soap_msg(env, 
                create_seq_msg_ctx, soap_ns_uri);
        if(!res_envelope)
        {
            /* There is no response message context. But in single channel duplex scenario there should be
             * an CSR in the back channel. So return failure.
             */

            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Response envelope not found");

            return AXIS2_FAILURE;
        }
    }

    conf_ctx = axis2_msg_ctx_get_conf_ctx(create_seq_msg_ctx, env);
    conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    svc_grp = axis2_msg_ctx_get_svc_grp(create_seq_msg_ctx, env);
    svc = axis2_msg_ctx_get_svc(create_seq_msg_ctx, env);
    op = axis2_msg_ctx_get_op(create_seq_msg_ctx, env);
    res_msg_ctx = axis2_msg_ctx_create(env, conf_ctx, 
        axis2_msg_ctx_get_transport_in_desc(create_seq_msg_ctx, env), 
        axis2_msg_ctx_get_transport_out_desc(create_seq_msg_ctx, env));
    if(svc_grp)
    {
        axis2_char_t *svc_grp_name = (axis2_char_t *)  axis2_svc_grp_get_name(
            svc_grp, env);
        svc_grp = axis2_conf_get_svc_grp(conf, env, svc_grp_name);
        if(svc_grp)
            axis2_msg_ctx_set_svc_grp(res_msg_ctx, env, svc_grp);
    }
    if (svc) 
    {
        axis2_char_t *svc_name = (axis2_char_t *) axis2_svc_get_name(svc, env);
        svc = axis2_conf_get_svc(conf, env, svc_name);
        if(svc)
            axis2_msg_ctx_set_svc(res_msg_ctx, env, svc);
    }
    if(op)
    {
        axutil_qname_t *qname = (axutil_qname_t *) axis2_op_get_qname(op, env);
        axis2_char_t *op_mep_str = NULL;
        axis2_char_t *op_name_str = NULL; 
        if(qname)
        {
            op_name_str = axutil_qname_to_string(qname, env);
        }
        op_mep_str = (axis2_char_t *) axis2_op_get_msg_exchange_pattern(op, env);
        if((op_name_str || op_mep_str))
        {
            axis2_op_t *op = NULL;
            if(op_name_str)
            {
                axutil_qname_t *op_qname = axutil_qname_create_from_string(env, 
                    op_name_str);
                op = axis2_svc_get_op_with_qname(svc, env, op_qname);
                if(op_qname)
                    axutil_qname_free(op_qname, env);
            }
            if(!op && op_mep_str && svc)
            {
                axutil_hash_t *all_ops = NULL;
                axutil_hash_index_t *index = NULL;
                /* Finding an operation using the MEP */
                all_ops = axis2_svc_get_all_ops(svc, env);
                for (index = axutil_hash_first(all_ops, env); index; index = 
                    axutil_hash_next(env, index))
                {
                    void *v = NULL;
                    axis2_char_t *mep = NULL;
                    axis2_op_t *temp = NULL;
                    axutil_hash_this(index, NULL, NULL, &v);
                    temp = (axis2_op_t *) v;
                    mep = (axis2_char_t *) axis2_op_get_msg_exchange_pattern(temp, 
                        env);
                    if(0 == axutil_strcmp(mep, op_mep_str))
                    {
                        op = temp;
                        break;
                    }
                }
            }
            if(op)
                axis2_msg_ctx_set_op(res_msg_ctx, env, op);
            else
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Cant find a suitable "\
                    "operation for the generated message");
                AXIS2_ERROR_SET(env->error, 
                    SANDESHA2_ERROR_CANNOT_FIND_OP_FOR_GENERATED_MSG, AXIS2_FAILURE);
                return AXIS2_FAILURE;
            }
        }
    }
    /* Setting contexts TODO is this necessary? */
    op = axis2_msg_ctx_get_op(res_msg_ctx, env);
    if(op)
    {
        axis2_svc_ctx_t *svc_ctx = axis2_msg_ctx_get_svc_ctx(res_msg_ctx, env);
		create_seq_res_op_ctx = axis2_op_ctx_create(env, op, svc_ctx);
        if(create_seq_res_op_ctx)
        {
            axis2_op_ctx_set_parent(create_seq_res_op_ctx, env, svc_ctx);
            axis2_msg_ctx_set_op_ctx(res_msg_ctx, env, create_seq_res_op_ctx);
        }
    }
    /*
     * Setting the message as serverSide will let it go through the 
     * Message Receiver (may be callback MR).
     */
    axis2_msg_ctx_set_server_side(res_msg_ctx, env, AXIS2_TRUE);

    svc_ctx = axis2_msg_ctx_get_svc_ctx(create_seq_msg_ctx, env);
    axis2_msg_ctx_set_svc_ctx(res_msg_ctx, env, svc_ctx);
    axis2_msg_ctx_set_svc_grp_ctx(res_msg_ctx, env, axis2_msg_ctx_get_svc_grp_ctx(
                create_seq_msg_ctx, env));

    if(res_envelope)
    {
        axis2_engine_t *engine = NULL;

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Response envelope for CreateSequenceResponse message found");

        axis2_msg_ctx_set_soap_envelope(res_msg_ctx, env, res_envelope);

        engine = axis2_engine_create(env, axis2_msg_ctx_get_conf_ctx(create_seq_msg_ctx, env));

        if(sandesha2_util_is_fault_envelope(env, res_envelope))
        {
            axis2_engine_receive_fault(engine, env, res_msg_ctx);
        }
        else
        {
            axis2_engine_receive(engine, env, res_msg_ctx);
        }

        if(engine)
        {
            axis2_engine_free(engine, env);
        }
    }

    create_seq_res_op_ctx = axis2_msg_ctx_get_op_ctx(res_msg_ctx, env);
    if(create_seq_res_op_ctx)
    {
        axis2_op_ctx_free(create_seq_res_op_ctx, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Exit:sandesha2_app_msg_processor_process_create_seq_response");
    return AXIS2_SUCCESS;
}

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_send_app_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *rms_internal_sequence_id,
    long msg_num,
    axis2_char_t *storage_key,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_sender_mgr_t *sender_mgr)
{
    axis2_msg_ctx_t *app_msg_ctx = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *rms_sequence_bean = NULL;
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
    sandesha2_seq_property_bean_t *internal_seq_bean = NULL;
    long millisecs = 0;
    axutil_property_t *property = NULL;
    axis2_engine_t *engine = NULL;
    sandesha2_identifier_t *identifier = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_bool_t last_msg = AXIS2_FALSE;
    axis2_op_ctx_t *temp_op_ctx = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    int mep = -1;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_bool_t is_svr_side = AXIS2_FALSE;
    axis2_bool_t resend = AXIS2_TRUE;
    axis2_bool_t continue_sending = AXIS2_TRUE;
    int msg_type = -1;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[Sandesha2] Entry:sandesha2_app_msg_processor_send_app_msg");

    AXIS2_PARAM_CHECK(env->error, rms_internal_sequence_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_key, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, sender_mgr, AXIS2_FAILURE);
    
    app_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(app_msg_ctx, env);
    to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rms_internal_sequence_id, 
            SANDESHA2_SEQ_PROP_TO_EPR);

    reply_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rms_internal_sequence_id, 
            SANDESHA2_SEQ_PROP_REPLY_TO_EPR);

    rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            rms_internal_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);

    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "rms_internal_sequence_id2:%s", rms_internal_sequence_id);
    while(!rms_sequence_bean)
    {
        rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
                rms_internal_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);
        AXIS2_SLEEP(1);
    }

    if (to_bean)
    {
        to_epr = axis2_endpoint_ref_create(env, sandesha2_seq_property_bean_get_value(to_bean, env));
        sandesha2_seq_property_bean_free(to_bean, env);
    }
    
    if(reply_to_bean)
    {
        reply_to_epr = axis2_endpoint_ref_create(env, sandesha2_seq_property_bean_get_value(
                    reply_to_bean, env));

        sandesha2_seq_property_bean_free(reply_to_bean, env);
    }
    
    if (to_epr)
    {
        to_addr = axis2_endpoint_ref_get_address(to_epr, env);
    }
    
    if(axis2_msg_ctx_get_server_side(app_msg_ctx, env))
    {
        axis2_endpoint_ref_t *reply_to = NULL;
        
        req_msg =  axis2_op_ctx_get_msg_ctx(axis2_msg_ctx_get_op_ctx(app_msg_ctx, env), env, 
                AXIS2_WSDL_MESSAGE_LABEL_IN);

        if(req_msg)
        {
            reply_to = axis2_msg_ctx_get_reply_to(req_msg, env);
        }
        if(reply_to)
        {
            new_to_str = (axis2_char_t*)axis2_endpoint_ref_get_address(reply_to, env);
        }
    }

    if(new_to_str)
    {
        sandesha2_msg_ctx_set_to(rm_msg_ctx, env, axis2_endpoint_ref_create(env, new_to_str));
    }
    else if (to_epr)
    {
        sandesha2_msg_ctx_set_to(rm_msg_ctx, env, to_epr);
    }

    rm_version = sandesha2_utils_get_rm_version(env, rms_internal_sequence_id, seq_prop_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Unable to find RM spec version for the rms internal_sequence_id %s", 
                rms_internal_sequence_id);

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
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence not found");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_SEQ, AXIS2_FAILURE);
            if(rm_version)
            {
                AXIS2_FREE(env->allocator, rm_version);
            }
            if(req_rm_msg)
            {
                sandesha2_msg_ctx_free(req_rm_msg, env);
            }
            return AXIS2_FAILURE;
        }

        if(sandesha2_seq_get_last_msg(req_seq, env))
        {
            last_msg = AXIS2_TRUE;
            sandesha2_seq_set_last_msg(seq, env, sandesha2_last_msg_create(env, rm_ns_val));
        }

        if(req_rm_msg)
        {
            sandesha2_msg_ctx_free(req_rm_msg, env);
        }
    }
    else
    {
        axis2_op_ctx_t *op_ctx = NULL;
        axutil_property_t *property = NULL;
        
        op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
        if(op_ctx)
        {
            property = axis2_msg_ctx_get_property(app_msg_ctx, env, SANDESHA2_CLIENT_LAST_MESSAGE);
            if(property)
            {
                axis2_char_t *value = axutil_property_get_value(property, env);
                if(value && !axutil_strcmp(value, AXIS2_VALUE_TRUE))
                {
                    if(sandesha2_spec_specific_consts_is_last_msg_indicator_reqd(env, rm_version))
                    {
                        last_msg = AXIS2_TRUE;
                        sandesha2_seq_set_last_msg(seq, env, sandesha2_last_msg_create(env, 
                                    rm_ns_val));
                    }
                }
            }
        }
    }

    if(!rms_sequence_bean || !sandesha2_seq_property_bean_get_value(rms_sequence_bean, env))
    {
        str_identifier = SANDESHA2_TEMP_SEQ_ID;
    }
    else
    {
        str_identifier = sandesha2_seq_property_bean_get_value(rms_sequence_bean, env);
    }
        
    identifier = sandesha2_identifier_create(env, rm_ns_val);
    sandesha2_identifier_set_identifier(identifier, env, str_identifier);
    sandesha2_seq_set_identifier(seq, env, identifier);
    sandesha2_msg_ctx_set_sequence(rm_msg_ctx, env, seq);

    /* TODO add_ack_requested */

    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    app_msg_entry = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_internal_seq_id(app_msg_entry, env, rms_internal_sequence_id);

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "sandesha to_addr = %s ", to_addr);
    
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "rms_internal_sequence_id = %s ", rms_internal_sequence_id);

    is_svr_side = axis2_msg_ctx_get_server_side(app_msg_ctx, env);
    if(is_svr_side && sandesha2_utils_is_single_channel(env, rm_version, to_addr))
    {
        axis2_char_t *rmd_sequence_id = NULL;
        sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
        axis2_msg_ctx_t *msg_ctx = NULL;
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        axis2_op_ctx_t *op_ctx = NULL;
        sandesha2_seq_t *req_seq = NULL;
       
        msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
        op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
        req_msg_ctx =  axis2_op_ctx_get_msg_ctx(op_ctx, env, AXIS2_WSDL_MESSAGE_LABEL_IN);
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        req_seq = sandesha2_msg_ctx_get_sequence(req_rm_msg_ctx, env);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence is NULL");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SEQ_NOT_EXIST, AXIS2_FAILURE);
            if(rm_version)
            {
                AXIS2_FREE(env->allocator, rm_version);
            }
            if(req_rm_msg_ctx)
            {
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            }

            return AXIS2_FAILURE;
        }

        rmd_sequence_id = sandesha2_identifier_get_identifier(sandesha2_seq_get_identifier(req_seq, 
                    env), env);
        if(!rmd_sequence_id)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence ID is NULL");
            if(rm_version)
            {
                AXIS2_FREE(env->allocator, rm_version);
            }
            if(req_rm_msg_ctx)
            {
                sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
            }

            return AXIS2_FAILURE;
        }

        sandesha2_msg_creator_add_ack_msg(env, rm_msg_ctx, rmd_sequence_id, seq_prop_mgr);
        if(req_rm_msg_ctx)
        {
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
        }

        engine = axis2_engine_create(env, conf_ctx);
        status = axis2_engine_resume_send(engine, env, msg_ctx);
        if(engine)
        {
            axis2_engine_free(engine, env);
        }

        return status;
    }

    if(rm_version)
    {
        AXIS2_FREE(env->allocator, rm_version);
    }

    sandesha2_sender_bean_set_msg_ctx_ref_key(app_msg_entry, env, storage_key);
    millisecs = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(app_msg_entry, env, millisecs);
    msg_id = sandesha2_msg_ctx_get_msg_id(rm_msg_ctx, env);
    sandesha2_sender_bean_set_msg_id(app_msg_entry, env, msg_id);
    sandesha2_sender_bean_set_msg_no(app_msg_entry, env, msg_num);
    sandesha2_sender_bean_set_msg_type(app_msg_entry, env, SANDESHA2_MSG_TYPE_APPLICATION);

    internal_seq_bean = sandesha2_seq_property_bean_create_with_data(env, msg_id, 
        SANDESHA2_MSG_CTX_PROP_INTERNAL_SEQUENCE_ID, rms_internal_sequence_id);

    if(internal_seq_bean)
    {
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, internal_seq_bean);
    }

    if(!rms_sequence_bean || !sandesha2_seq_property_bean_get_value(rms_sequence_bean, env))
    {
        sandesha2_sender_bean_set_send(app_msg_entry, env, AXIS2_FALSE);
    }
    else
    {
        sandesha2_sender_bean_set_send(app_msg_entry, env, AXIS2_TRUE);
        property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
        axis2_msg_ctx_set_property(app_msg_ctx, env, SANDESHA2_SET_SEND_TO_TRUE, property);
    }

    temp_op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);

    /**
     * In RM one way out only(from client application side) we need to keep the
     * operation context of the application message context marked as in-use.
     * Otherwise when client send the next application message and free the
     * previous op_client this op_ctx is freed.
     */
    mep = axis2_op_get_axis_specific_mep_const(axis2_op_ctx_get_op(temp_op_ctx, env), env);
    if(AXIS2_MEP_CONSTANT_OUT_ONLY == mep)
    {
        axis2_ctx_t *ctx = axis2_conf_ctx_get_base(conf_ctx, env);
        axutil_property_t *temp_prop = axis2_ctx_get_property(ctx, env, SANDESHA2_MSG_CTX_MAP);

        if(temp_prop)
        {
            axis2_op_ctx_set_in_use(temp_op_ctx, env, AXIS2_TRUE);
        }
    }

    /**
     * When we store application message context as below it should be noted
     * that at Sandesha2/C client application side this is actually stored in
     * in-memory whereas in the web service side it is actually stored in
     * database only. In RM one way scenario since we call
     * axis2_op_ctx_set_in_use() for the operation context of the application
     * message in few lines above we need to free that operation context in the
     * sandesha2_storage_mgr_remove_msg_ctx() function.
     */
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, storage_key, app_msg_ctx);

    continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env, app_msg_entry, 
            conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);
    if(!continue_sending)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Do not continue sending the message");
        if(rm_version)
        {
            AXIS2_FREE(env->allocator, rm_version);
        }
        if(app_msg_entry)
        {
            sandesha2_sender_bean_free(app_msg_entry, env);
        }

        return AXIS2_FAILURE;
    }

    sandesha2_sender_mgr_insert(sender_mgr, env, app_msg_entry);
    
    axis2_msg_ctx_set_current_handler_index(app_msg_ctx, env, 
            axis2_msg_ctx_get_current_handler_index(app_msg_ctx, env) + 1);

    msg_type = sandesha2_msg_ctx_get_msg_type(rm_msg_ctx, env);
    
    if(!sandesha2_util_is_ack_already_piggybacked(env, rm_msg_ctx))
    {
        sandesha2_ack_mgr_piggyback_acks_if_present(env, rm_msg_ctx, storage_mgr, seq_prop_mgr, 
                sender_mgr);
    }

    engine = axis2_engine_create(env, conf_ctx);
    if(axis2_engine_resume_send(engine, env, app_msg_ctx))
    {

        if(sandesha2_app_msg_processor_check_for_response_msg(env, app_msg_ctx))
        {    
            status = sandesha2_app_msg_processor_process_app_msg_response(env, app_msg_ctx, 
                storage_mgr);
        }
    }
    else
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Engine resume send failed");
    }

    if(engine)
    {
        axis2_engine_free(engine, env);
    }


    while(resend)
    {
        sandesha2_sender_bean_t *sender_bean = NULL;
        axis2_char_t *temp_msg_id = NULL;

        sender_bean = sandesha2_sender_mgr_get_next_msg_to_send(sender_mgr, env, 
                rms_internal_sequence_id);
        if(!sender_bean)
        {
            /* There is no pending message to send. So exit from the loop. */
            break;
        }
        else
        {
            temp_msg_id = sandesha2_sender_bean_get_msg_id(sender_bean, env);
            if(!axutil_strcmp(temp_msg_id, msg_id))
            {
                if(sender_bean)
                {
                    sandesha2_sender_bean_free(sender_bean, env); 
                }

                continue;
            }
        }

        status = sandesha2_app_msg_processor_resend(env, conf_ctx, msg_id, is_svr_side,
                rms_internal_sequence_id, storage_mgr, seq_prop_mgr, create_seq_mgr, 
                sender_mgr);

        if(AXIS2_SUCCESS != status)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Resend failed for  message id %s in sequence %s", msg_id, 
                rms_internal_sequence_id);

            resend = AXIS2_FALSE;
        }

        if(sender_bean)
        {
            sandesha2_sender_bean_free(sender_bean, env); 
        }
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[Sandesha2] Exit:sandesha2_app_msg_processor_send_app_msg");

    return status;
}

static axis2_status_t
sandesha2_app_msg_processor_resend(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id,
    axis2_bool_t is_svr_side,
    const axis2_char_t *rms_internal_sequence_id,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_sender_mgr_t *sender_mgr)
{
    sandesha2_sender_bean_t *sender_worker_bean = NULL;
    sandesha2_sender_bean_t *bean1 = NULL;
    axis2_char_t *key = NULL;
    axis2_bool_t continue_sending = AXIS2_TRUE;
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    int msg_type = -1;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    axis2_bool_t successfully_sent = AXIS2_FALSE;
    axis2_status_t status = AXIS2_SUCCESS;
    axis2_bool_t resend = AXIS2_FALSE;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[sandesha2] Entry:sandesha2_app_msg_processor_resend");        
    
    sender_worker_bean = sandesha2_sender_mgr_retrieve(sender_mgr, env, msg_id);
    if(!sender_worker_bean)
    {
        AXIS2_LOG_WARNING(env->log, AXIS2_LOG_SI, "[sandesha2] sender_worker_bean is NULL");
        return AXIS2_FAILURE;
    }

    key = sandesha2_sender_bean_get_msg_ctx_ref_key(sender_worker_bean, env);
    if(is_svr_side)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Retrieving msg_ctx from database");
        msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, key, conf_ctx, AXIS2_TRUE);
    }
    else
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Retrieving msg_ctx from configuration context");
        msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, key, conf_ctx, 
                AXIS2_FALSE);
    }

    if(!msg_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] msg_ctx is not present in the store.");
        if(sender_worker_bean)
        {
            sandesha2_sender_bean_free(sender_worker_bean, env);
        }

        return AXIS2_FAILURE;
    }

    continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env, sender_worker_bean, 
            conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);
    sandesha2_sender_mgr_update(sender_mgr, env, sender_worker_bean);
    if(!continue_sending)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Do not continue sending the message");
        if(sender_worker_bean)
        {
            sandesha2_sender_bean_free(sender_worker_bean, env);
        }

        return AXIS2_FAILURE;
    }
    
    rm_msg_ctx = sandesha2_msg_init_init_msg(env, msg_ctx);
    
    msg_type = sandesha2_msg_ctx_get_msg_type(rm_msg_ctx, env);
    
    if(!sandesha2_util_is_ack_already_piggybacked(env, rm_msg_ctx))
    {
        sandesha2_ack_mgr_piggyback_acks_if_present(env, rm_msg_ctx, storage_mgr, seq_prop_mgr, 
                sender_mgr);
    }
    
    transport_out = axis2_msg_ctx_get_transport_out_desc(msg_ctx, env);
    if(transport_out)
    {
        transport_sender = axis2_transport_out_desc_get_sender(transport_out, env);
    }
    if(transport_sender)
    {
        /* This is neccessary to avoid a double free */
        axis2_msg_ctx_set_property(msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
        if(AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, msg_ctx))
		{
        	successfully_sent = AXIS2_TRUE;
		}else
		{
        	successfully_sent = AXIS2_FALSE;
		}
    }

    msg_id = sandesha2_sender_bean_get_msg_id(sender_worker_bean, env);
    bean1 = sandesha2_sender_mgr_retrieve(sender_mgr, env, msg_id);
    if(bean1)
    { 
        resend = sandesha2_sender_bean_is_resend(sender_worker_bean, env);
        if(resend)
        {
            sandesha2_sender_bean_set_sent_count(bean1, env, 
                sandesha2_sender_bean_get_sent_count(sender_worker_bean, env));
            sandesha2_sender_bean_set_time_to_send(bean1, env, 
                sandesha2_sender_bean_get_time_to_send(sender_worker_bean, env));
            sandesha2_sender_mgr_update(sender_mgr, env, bean1);
        }
    }

    if(sender_worker_bean)
    {
        sandesha2_sender_bean_free(sender_worker_bean, env);
    }

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]msg_type:%d", msg_type);

    if(successfully_sent)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]message of msg_type:%d successfully sent", msg_type);
        if(sandesha2_app_msg_processor_check_for_response_msg(env, msg_ctx))
        {
            status = sandesha2_app_msg_processor_process_app_msg_response(env, msg_ctx, storage_mgr);
            if(AXIS2_SUCCESS != status)
            {
                return status;
            }
        }
    }

    if(bean1)
    {
        sandesha2_sender_bean_free(bean1, env);
    }

    if(rm_msg_ctx)
    {
        sandesha2_msg_ctx_free(rm_msg_ctx, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[sandesha2] Exit:sandesha2_app_msg_processor_resend");

    return status;
}

static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_process_app_msg_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr)
{
    axutil_property_t *property = NULL;
    axis2_msg_ctx_t *res_msg_ctx = NULL;
    axiom_soap_envelope_t *res_envelope = NULL;
    axis2_char_t *soap_ns_uri = NULL;
    axis2_svc_ctx_t *svc_ctx = NULL;
    axis2_svc_grp_t *svc_grp = NULL;
    axis2_svc_t *svc = NULL;
    axis2_op_t *op = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_op_ctx_t *op_ctx = NULL; 
   
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Entry:sandesha2_app_msg_processor_process_app_msg_response");

    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    soap_ns_uri = axis2_msg_ctx_get_is_soap_11(msg_ctx, env) ?
         AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
         AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;

    res_envelope = axis2_msg_ctx_get_response_soap_envelope(msg_ctx, env);
    if(res_envelope)
    {
        /* When service client is freed by the application client operation
         * context and hence msg_ctx and therefore soap envelope is freed.
         * To avoid that we increment the soap envelope. This soap envelope
         * will be freed when we free the application op_ctx below.
         */

        axiom_soap_envelope_increment_ref(res_envelope, env);
        /* To avoid a second passing through incoming handlers at op_client*/
        property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "dam_handler_already_visited");
        axis2_msg_ctx_set_property(msg_ctx, env, AXIS2_HANDLER_ALREADY_VISITED, property);
    }

    if(!res_envelope)
    {
        res_envelope = axis2_http_transport_utils_create_soap_msg(env, msg_ctx, soap_ns_uri);
        if(!res_envelope)
        {
            /* There is no response message context.
             */
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Response envelope not found");
            return AXIS2_SUCCESS;
        }
    }

    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    svc_grp = axis2_msg_ctx_get_svc_grp(msg_ctx, env);
    svc = axis2_msg_ctx_get_svc(msg_ctx, env);
    op = axis2_msg_ctx_get_op(msg_ctx, env);
    res_msg_ctx = axis2_msg_ctx_create(env, conf_ctx, axis2_msg_ctx_get_transport_in_desc(msg_ctx, 
                env), axis2_msg_ctx_get_transport_out_desc(msg_ctx, env));
    if(svc_grp)
    {
        axis2_char_t *svc_grp_name = (axis2_char_t *)  axis2_svc_grp_get_name(
            svc_grp, env);
        svc_grp = axis2_conf_get_svc_grp(conf, env, svc_grp_name);
        if(svc_grp)
            axis2_msg_ctx_set_svc_grp(res_msg_ctx, env, svc_grp);
    }
    if (svc) 
    {
        axis2_char_t *svc_name = (axis2_char_t *) axis2_svc_get_name(svc, env);
        svc = axis2_conf_get_svc(conf, env, svc_name);
        if(svc)
            axis2_msg_ctx_set_svc(res_msg_ctx, env, svc);
    }
    if(op)
    {
        axutil_qname_t *qname = (axutil_qname_t *) axis2_op_get_qname(op, env);
        axis2_char_t *op_mep_str = NULL;
        axis2_char_t *op_name_str = NULL; 
        if(qname)
        {
            op_name_str = axutil_qname_to_string(qname, env);
        }
        op_mep_str = (axis2_char_t *) axis2_op_get_msg_exchange_pattern(op, env);
        if((op_name_str || op_mep_str))
        {
            axis2_op_t *op = NULL;
            if(op_name_str)
            {
                axutil_qname_t *op_qname = axutil_qname_create_from_string(env, 
                    op_name_str);
                op = axis2_svc_get_op_with_qname(svc, env, op_qname);
                if(op_qname)
                    axutil_qname_free(op_qname, env);
            }
            if(!op && op_mep_str && svc)
            {
                axutil_hash_t *all_ops = NULL;
                axutil_hash_index_t *index = NULL;
                /* Finding an operation using the MEP */
                all_ops = axis2_svc_get_all_ops(svc, env);
                for (index = axutil_hash_first(all_ops, env); index; index = 
                    axutil_hash_next(env, index))
                {
                    void *v = NULL;
                    axis2_char_t *mep = NULL;
                    axis2_op_t *temp = NULL;
                    axutil_hash_this(index, NULL, NULL, &v);
                    temp = (axis2_op_t *) v;
                    mep = (axis2_char_t *) axis2_op_get_msg_exchange_pattern(temp, 
                        env);
                    if(0 == axutil_strcmp(mep, op_mep_str))
                    {
                        op = temp;
                        break;
                    }
                }
            }
            if(op)
                axis2_msg_ctx_set_op(res_msg_ctx, env, op);
            else
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Cant find a suitable "\
                    "operation for the generated message");
                AXIS2_ERROR_SET(env->error, 
                    SANDESHA2_ERROR_CANNOT_FIND_OP_FOR_GENERATED_MSG, AXIS2_FAILURE);
                return AXIS2_FAILURE;
            }
        }
    }
    /* Setting contexts TODO is this necessary? */
    op = axis2_msg_ctx_get_op(res_msg_ctx, env);
    if(op)
    {
		axis2_op_ctx_t *response_op_ctx = NULL;

        axis2_svc_ctx_t *svc_ctx = axis2_msg_ctx_get_svc_ctx(res_msg_ctx, env);
		response_op_ctx = axis2_op_ctx_create(env, op, svc_ctx);
        if(response_op_ctx)
        {
            axis2_op_ctx_set_parent(response_op_ctx, env, svc_ctx);
            axis2_msg_ctx_set_op_ctx(res_msg_ctx, env, response_op_ctx);
        }
    }
    /*
     * Setting the message as server side will let it go through the message receiver (may be callback MR).
     */
    axis2_msg_ctx_set_server_side(res_msg_ctx, env, AXIS2_TRUE);

    svc_ctx = axis2_msg_ctx_get_svc_ctx(msg_ctx, env);
    axis2_msg_ctx_set_svc_ctx(res_msg_ctx, env, svc_ctx);
    axis2_msg_ctx_set_svc_grp_ctx(res_msg_ctx, env, axis2_msg_ctx_get_svc_grp_ctx(msg_ctx, env));

    if(res_envelope)
    {
        axis2_engine_t *engine = NULL;

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Response envelope found");

        axis2_msg_ctx_set_soap_envelope(res_msg_ctx, env, res_envelope);
        engine = axis2_engine_create(env, axis2_msg_ctx_get_conf_ctx(msg_ctx, env));
        if(sandesha2_util_is_fault_envelope(env, res_envelope))
        {
            axis2_engine_receive_fault(engine, env, res_msg_ctx);
        }
        else
        {
            axis2_engine_receive(engine, env, res_msg_ctx);
        }
        if(engine)
        {
            axis2_engine_free(engine, env);
        }
    }

    op_ctx = axis2_msg_ctx_get_op_ctx(res_msg_ctx, env);
    axis2_op_ctx_free(op_ctx, env);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Exit:sandesha2_app_msg_processor_process_app_msg_response");

    return AXIS2_SUCCESS;
}

long AXIS2_CALL                 
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

