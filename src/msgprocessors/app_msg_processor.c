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
#include <sandesha2_msg_retrans_adjuster.h>

#include <axis2_const.h>
#include <axutil_types.h>
#include <axis2_msg_ctx.h>
#include <axutil_string.h>
#include <axis2_engine.h>
#include <axutil_uuid_gen.h>
#include <axis2_relates_to.h>
#include <axis2_core_utils.h>
#include <axiom_soap_const.h>
#include <axiom_soap_body.h>
#include <axis2_http_transport_utils.h>
#include <platforms/axutil_platform_auto_sense.h>

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

typedef struct sandesha2_app_msg_processor_args sandesha2_app_msg_processor_args_t;

struct sandesha2_app_msg_processor_args
{
    axutil_env_t *env;
    axis2_conf_ctx_t *conf_ctx;
    axis2_char_t *internal_sequence_id;
    axis2_char_t *msg_id;
    axis2_bool_t is_server_side;
    int retrans_interval;
    void *bean;
    void *msg_ctx;
};

static void AXIS2_CALL                 
sandesha2_app_msg_processor_is_last_out_msg(
    const axutil_env_t *env,
    axis2_msg_ctx_t *msg_ctx,
    axis2_char_t *rmd_sequence_id,
    axis2_char_t *internal_sequence_id,
    long msg_num,
    sandesha2_seq_property_mgr_t *seq_prop_mgr);

static void * AXIS2_THREAD_FUNC
sandesha2_app_msg_processor_create_seq_msg_worker_function(
    axutil_thread_t *thd, 
    void *data);

static axis2_status_t
sandesha2_app_msg_processor_start_create_seq_msg_resender(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *internal_sequence_id,
    axis2_char_t *msg_id,
    const axis2_bool_t is_server_side,
    int retrans_interval,
    sandesha2_sender_bean_t *create_sequence_sender_bean,
    axis2_msg_ctx_t *create_seq_msg_ctx);

static void * AXIS2_THREAD_FUNC
sandesha2_app_msg_processor_application_msg_worker_function(
    axutil_thread_t *thd, 
    void *data);

static axis2_status_t
sandesha2_app_msg_processor_start_application_msg_resender(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *internal_sequence_id,
    axis2_char_t *msg_id,
    const axis2_bool_t is_server_side,
    int retrans_interval,
    axis2_msg_ctx_t *app_msg_ctx);

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
sandesha2_app_msg_processor_process_create_seq_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *create_seq_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr);

static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_process_app_msg_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *app_msg_ctx);

static axis2_status_t AXIS2_CALL
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
    sandesha2_sender_mgr_t *sender_mgr,
    axis2_msg_ctx_t *app_msg_ctx);

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_next_msg_no(
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id,
    long msg_num,
    sandesha2_seq_property_mgr_t *seq_prop_mgr);
                        
static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_last_out_msg_no(
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
    {
        AXIS2_FREE(env->allocator, msg_processor->ops);
    }
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));

	return AXIS2_SUCCESS;
}


static axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *app_msg_ctx = NULL;
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
    axis2_char_t *msgs_str = NULL;
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
    const axutil_string_t *str_soap_action = NULL;
    const axis2_char_t *wsa_action = NULL;
    const axis2_char_t *soap_action = NULL;
    axis2_char_t *dbname = NULL;
   
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Entry:sandesha2_app_msg_processor_process_in_msg");
 
    app_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    if(!app_msg_ctx)
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
    
    op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
    /*axis2_op_ctx_set_in_use(op_ctx, env, AXIS2_TRUE);*/
    axis2_op_ctx_set_response_written(op_ctx, env, AXIS2_TRUE);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(app_msg_ctx, env);
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
        axis2_msg_ctx_set_paused(app_msg_ctx, env, AXIS2_TRUE);
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
            "[sandesha2]sandesha2_app_msg_processor_process_in_msg send Fault");

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
        axis2_msg_ctx_set_paused(app_msg_ctx, env, AXIS2_TRUE);

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

        axis2_msg_ctx_set_paused(app_msg_ctx, env, AXIS2_TRUE);

        return AXIS2_SUCCESS;
    }

    sandesha2_seq_mgr_update_last_activated_time(env, rmd_sequence_id, seq_prop_mgr);
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
        /*axiom_soap_envelope_t *response_envelope = NULL;*/
        /*int soap_version = -1;*/
        axutil_property_t *property = NULL;
        axis2_char_t *client_seq_key = NULL;
        
        highest_in_msg_no = msg_no;
        msg_id = axis2_msg_ctx_get_msg_id(app_msg_ctx, env);
        highest_msg_no_bean = sandesha2_seq_property_bean_create_with_data(env, rmd_sequence_id, 
                SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, msg_num_str);

        highest_msg_key_bean = sandesha2_seq_property_bean_create_with_data(env, rmd_sequence_id, 
                SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, highest_in_msg_key_str);

        highest_msg_id_bean = sandesha2_seq_property_bean_create_with_data(env, rmd_sequence_id, 
                SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_ID, (axis2_char_t *)msg_id);

        /*sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
            highest_in_msg_key_str, conf_ctx, -1);
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, highest_in_msg_key_str, app_msg_ctx, 
                AXIS2_TRUE);*/

        property = axis2_msg_ctx_get_property(app_msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY);
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
    {
        AXIS2_FREE(env->allocator, highest_in_msg_no_str);
    }

    if(highest_in_msg_key_str)
    {
        AXIS2_FREE(env->allocator, highest_in_msg_key_str);
    }

    msgs_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id, 
        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
    if(msgs_bean)
    {
        axis2_char_t *temp_value = sandesha2_seq_property_bean_get_value(msgs_bean, env);
        if(temp_value)
        {
            msgs_str = axutil_strdup(env, temp_value);
        }
    }
    else
    {
        msgs_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_seq_id(msgs_bean, env, rmd_sequence_id);
        sandesha2_seq_property_bean_set_name(msgs_bean, env, 
            SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);

        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, msgs_bean);
    }

    if(msgs_str)
    {
        msg_no_present_in_list = sandesha2_app_msg_processor_msg_num_is_in_list(env, msgs_str, msg_no);
    }

    if(msg_no_present_in_list && !axutil_strcmp(SANDESHA2_QOS_DEFAULT_INVOCATION_TYPE, 
                SANDESHA2_QOS_EXACTLY_ONCE))
    {
        sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
    }

    if(!msg_no_present_in_list)
    {
        if(msgs_str)
        {
            axis2_char_t *tmp_str = NULL;

            tmp_str = axutil_strcat(env, msgs_str, ",", msg_num_str, NULL);
            AXIS2_FREE(env->allocator, msgs_str);
            msgs_str = tmp_str;
        }
        else
        {
            msgs_str = axutil_strdup(env, msg_num_str);
        }

        sandesha2_seq_property_bean_set_value(msgs_bean, env, msgs_str);
        sandesha2_seq_property_mgr_update(seq_prop_mgr, env, msgs_bean);
    }
    
    sandesha2_seq_property_bean_free(msgs_bean, env);

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
        
        if(msgs_str)
        {
            AXIS2_FREE(env->allocator, msgs_str);
        }

        return AXIS2_FAILURE;
    }

    sandesha2_next_msg_bean_free(next_msg_bean, env);

    in_order_invoke = sandesha2_property_bean_is_in_order(sandesha2_utils_get_property_bean(env, 
            axis2_conf_ctx_get_conf(conf_ctx, env)), env);
    /* test code */
    if(axis2_msg_ctx_get_server_side(app_msg_ctx, env))
    {
        sandesha2_last_msg_t *last_msg = sandesha2_seq_get_last_msg(seq, env);
        axis2_char_t *msg_id = (axis2_char_t *)axis2_msg_ctx_get_msg_id(app_msg_ctx, env);
        if(last_msg)
        {
            sandesha2_seq_property_bean_t *seq_prop_bean = NULL;
            
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Last message");

            seq_prop_bean = sandesha2_seq_property_bean_create_with_data(
                env, rmd_sequence_id, SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_ID, msg_id);
            if(seq_prop_bean)
            {
                sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, seq_prop_bean);
                sandesha2_seq_property_bean_free(seq_prop_bean, env);
            }
        }
    }
    /* end test code */

    /*
     * If this message matches the WSRM 1.0 pattern for an empty last message (e.g.
     * the sender wanted to signal the last message, but didn't have an application
     * message to send) then we do not need to send the message on to the application.
     */
    str_soap_action = axis2_msg_ctx_get_soap_action(app_msg_ctx, env);
    soap_action = axutil_string_get_buffer(str_soap_action, env);
    wsa_action = axis2_msg_ctx_get_wsa_action(app_msg_ctx, env);
    if(!axutil_strcmp(SANDESHA2_SPEC_2005_02_ACTION_LAST_MESSAGE, wsa_action) || 0 == axutil_strcmp(
                SANDESHA2_SPEC_2005_02_SOAP_ACTION_LAST_MESSAGE, soap_action)) 
    {
        axis2_status_t status = AXIS2_FAILURE;
        axis2_bool_t send_response = AXIS2_TRUE;
        axis2_svc_t *temp_svc = NULL;
        axis2_op_ctx_t *op_ctx = NULL;

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Got WSRM 1.0 last message. Send ack and aborting");

        sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, rmd_sequence_id, 
                storage_mgr, sender_mgr, seq_prop_mgr);

        op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
        if(op_ctx)
        {
            int mep = -1;
            axis2_op_t *op = NULL;
            
            op = axis2_op_ctx_get_op(op_ctx, env);
            mep = axis2_op_get_axis_specific_mep_const(op, env);
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] mep:%d", mep);
            if(mep == AXIS2_MEP_CONSTANT_IN_ONLY)
            {
                send_response = AXIS2_FALSE;
            }
        }

        temp_svc = axis2_msg_ctx_get_svc(app_msg_ctx, env);
        if(temp_svc)
        {
            const axis2_char_t *svc_name = axis2_svc_get_name(temp_svc, env);

            if(!axutil_strcmp("__ANONYMOUS_SERVICE__", svc_name))
            {
                send_response = AXIS2_FALSE;
                status =  AXIS2_SUCCESS;
            }
        }
        else
        {
            send_response = AXIS2_FALSE;
            status =  AXIS2_SUCCESS;
        }
        
        if(send_response)
        {
            axis2_char_t *storage_key = NULL;
            axis2_char_t *outgoing_int_seq_id = NULL;
            axis2_msg_ctx_t *out_msg_ctx = NULL;
            sandesha2_msg_ctx_t *out_rm_msg_ctx = NULL;
            axis2_op_ctx_t *op_ctx = NULL;
            axutil_qname_t *temp_qname = NULL;
            axis2_op_t *anon_out_op = NULL;

            outgoing_int_seq_id = sandesha2_utils_get_internal_sequence_id(env, rmd_sequence_id);

            out_msg_ctx = axis2_core_utils_create_out_msg_ctx(env, app_msg_ctx);
            if(out_msg_ctx)
            {
                out_rm_msg_ctx = sandesha2_msg_init_init_msg(env, out_msg_ctx);
            }
            else
            {
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
                
                if(msgs_str)
                {
                    AXIS2_FREE(env->allocator, msgs_str);
                }

                return AXIS2_FAILURE;
            }
        
            temp_qname = axutil_qname_create(env, "__OPERATION_OUT_ONLY__", NULL, NULL);
            if (!temp_qname)
            {
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
                
                if(msgs_str)
                {
                    AXIS2_FREE(env->allocator, msgs_str);
                }

                return AXIS2_FAILURE;
            }
        
            anon_out_op = axis2_op_create_with_qname(env, temp_qname);
            axutil_qname_free(temp_qname, env);

            op_ctx = axis2_op_ctx_create(env, anon_out_op, NULL);
            status = axis2_op_ctx_add_msg_ctx(op_ctx, env, out_msg_ctx);
            status = axis2_op_ctx_add_msg_ctx(op_ctx, env, app_msg_ctx);
            axis2_msg_ctx_set_op_ctx(out_msg_ctx, env, op_ctx);

            storage_key = axutil_uuid_gen(env);
            status = sandesha2_app_msg_processor_send_app_msg(env, out_rm_msg_ctx, outgoing_int_seq_id, 
                msg_no, storage_key, storage_mgr, create_seq_mgr, seq_prop_mgr, sender_mgr);

            if(out_rm_msg_ctx)
            {
                sandesha2_msg_ctx_free(out_rm_msg_ctx, env);
            }

            if(storage_key)
            {
                AXIS2_FREE(env->allocator, storage_key);
            }
        }

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
        
        if(msgs_str)
        {
            AXIS2_FREE(env->allocator, msgs_str);
        }

        return status;
    }

    if(axis2_msg_ctx_get_server_side(app_msg_ctx, env) && in_order_invoke)
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
                if(msgs_str)
                {
                    AXIS2_FREE(env->allocator, msgs_str);
                }

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
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, str_key, app_msg_ctx, AXIS2_TRUE);
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
        /*sandesha2_utils_start_invoker_for_seq(env, conf_ctx, rmd_sequence_id);*/
    }

    if(!sandesha2_app_msg_processor_send_ack_if_reqd(env, rm_msg_ctx, msgs_str, rmd_sequence_id, storage_mgr, 
                sender_mgr, seq_prop_mgr))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sending acknowledgment failed");

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
        
        if(msgs_str)
        {
            AXIS2_FREE(env->allocator, msgs_str);
        }

        return AXIS2_FAILURE;
    }

    if(msgs_str)
    {
        AXIS2_FREE(env->allocator, msgs_str);
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
    axis2_char_t *internal_sequence_id = NULL;
    axis2_char_t *storage_key = NULL;
    /*axis2_bool_t last_msg = AXIS2_FALSE;*/
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
    sandesha2_seq_property_bean_t *spec_ver_bean = NULL;
    axis2_char_t *spec_ver = NULL;
    axiom_soap_envelope_t *soap_env = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    sandesha2_seq_property_bean_t *rms_sequence_bean = NULL;
    axis2_char_t *op_name = NULL;
    axis2_char_t *to_addr = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_msg_ctx_t *req_msg_ctx = NULL;
    /*axis2_relates_to_t *relates_to = NULL;*/
    axis2_char_t *dbname = NULL;
    sandesha2_seq_property_bean_t *seq_timeout_bean = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
    
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
        /*const axis2_relates_to_t *relates_to = NULL;*/
        /*axis2_char_t *relates_to_value = NULL;*/
        /*axis2_char_t *last_req_id = NULL;*/
       
        req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
        req_seq = sandesha2_msg_ctx_get_sequence(req_rm_msg_ctx, env);
        if(!req_seq)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Sequence is NULL");
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
        internal_sequence_id = sandesha2_utils_get_internal_sequence_id(env, rmd_sequence_id);

        /* Deciding whether this is the last message. We assume it is if it 
         * relates to a message which arrived with the LastMessage flag on it.
         */
        /*last_req_id = sandesha2_utils_get_seq_property(env, rmd_sequence_id, 
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
        }*/
        
        /*last_msg = sandesha2_app_msg_processor_is_last_out_msg(env, msg_ctx, rmd_sequence_id, 
                internal_sequence_id, seq_prop_mgr);*/
    }
    else /* Client side */
    {
        axis2_char_t *to = NULL;
        axis2_char_t *seq_key = NULL;
        /*axis2_char_t *last_app_msg = NULL;*/
        
        to = (axis2_char_t*)axis2_endpoint_ref_get_address(to_epr, env);
        property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY);
        if(property)
        {
            seq_key = axutil_property_get_value(property, env);
        }

        if(!seq_key)
        {
            seq_key = axutil_uuid_gen(env);
            property = axutil_property_create_with_args(env, 0, 0, 0, seq_key);
            axis2_msg_ctx_set_property(msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY, property);
        }

        internal_sequence_id = sandesha2_utils_get_client_internal_sequence_id(env, to, 
                seq_key);
       
        /*last_msg = sandesha2_app_msg_processor_is_last_out_msg(env, msg_ctx, NULL, 
                internal_sequence_id, seq_prop_mgr);*/
        
        /*property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_LAST_MESSAGE);
        if(property)
        {
            last_app_msg = axutil_property_get_value(property, env);
        }

        if(last_app_msg && !axutil_strcmp(last_app_msg, AXIS2_VALUE_TRUE))
        {
            spec_ver = sandesha2_utils_get_rm_version(env, internal_sequence_id, seq_prop_mgr);
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
                spec_ver = NULL;
            }
        }*/
    }

    seq_timeout_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQ_PROP_SEQ_TIMED_OUT);

    if(seq_timeout_bean)
    {
        axis2_bool_t exit_system = AXIS2_FALSE;
        axis2_char_t *str_timeout = sandesha2_seq_property_bean_get_value(seq_timeout_bean, env);

        if(str_timeout && !axutil_strcmp(AXIS2_VALUE_TRUE, str_timeout))
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
            if(internal_sequence_id)
            {
                    AXIS2_FREE(env->allocator, internal_sequence_id);
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
            exit_system = AXIS2_TRUE;
        }

        sandesha2_seq_property_bean_free(seq_timeout_bean, env);
        if(exit_system)
        {
            exit(AXIS2_FAILURE);
        }
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
            if(internal_sequence_id)
            {
                AXIS2_FREE(env->allocator, internal_sequence_id);
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

    system_msg_num = sandesha2_app_msg_processor_get_prev_msg_no(env, internal_sequence_id, seq_prop_mgr);

    if(msg_num_lng > 0 && msg_num_lng <= system_msg_num)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Invalid Message Number");

        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, AXIS2_FAILURE);
        
        if(req_rm_msg_ctx)
        {
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
        }
        if(internal_sequence_id)
        {
            AXIS2_FREE(env->allocator, internal_sequence_id);
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
        sandesha2_app_msg_processor_set_next_msg_no(env, internal_sequence_id, msg_number, seq_prop_mgr);
    }

    sprintf(msg_number_str, "%ld", msg_number); 
    res_highest_msg_bean = sandesha2_seq_property_bean_create_with_data(env, internal_sequence_id, 
            SANDESHA2_SEQ_PROP_HIGHEST_OUT_MSG_NUMBER, msg_number_str);

    if(res_highest_msg_bean)
    {
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, res_highest_msg_bean);
        sandesha2_seq_property_bean_free(res_highest_msg_bean, env);
    }


    /*if(last_msg)
    {
        sandesha2_seq_property_bean_t *res_last_msg_key_bean = NULL;
       
        res_last_msg_key_bean = sandesha2_seq_property_bean_create_with_data(env, 
                internal_sequence_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO, msg_number_str);

        if(res_last_msg_key_bean)
        {
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, res_last_msg_key_bean);
            sandesha2_seq_property_bean_free(res_last_msg_key_bean, env);
        }
    }*/


    if(is_svr_side)
    {
        sandesha2_seq_property_bean_t *rmd_to_bean = NULL;

        rmd_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id, 
                SANDESHA2_SEQ_PROP_TO_EPR);
        if(rmd_to_bean)
        {
            axis2_char_t *rmd_to = NULL;
    
            rmd_to = axutil_strdup(env, sandesha2_seq_property_bean_get_value(rmd_to_bean, env));
            property = axutil_property_create_with_args(env, 0, AXIS2_TRUE, 0, rmd_to);
            axis2_msg_ctx_set_property(msg_ctx, env, SANDESHA2_SEQ_PROP_TO_EPR, property);
            sandesha2_seq_property_bean_free(rmd_to_bean, env);
        }
       
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
            if(internal_sequence_id)
            {
                AXIS2_FREE(env->allocator, internal_sequence_id);
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
    }

    if(!spec_ver)
    {
        spec_ver = sandesha2_spec_specific_consts_get_default_spec_version(env);
    }

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Spec version:%s", spec_ver);
    
    rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            internal_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);

    if(1 == msg_number)
    {
        if(!rms_sequence_bean)
        {
            send_create_seq = AXIS2_TRUE;
        }

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "Starting the rms sequence with rms rms internal sequence id %s", 
                internal_sequence_id);

        sandesha2_seq_mgr_setup_new_rms_sequence(env, msg_ctx, internal_sequence_id, spec_ver, 
                seq_prop_mgr);
    }

    if(rms_sequence_bean)
    {
        sandesha2_seq_property_bean_free(rms_sequence_bean, env);
    }

    if(spec_ver_bean)
    {
        sandesha2_seq_property_bean_free(spec_ver_bean, env);
    }

    if(send_create_seq)
    {
        sandesha2_seq_property_bean_t *create_seq_added = NULL;
        axis2_char_t *addr_ns_uri = NULL;
        axis2_char_t *anon_uri = NULL;
       
        create_seq_added = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
                SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT);

        addr_ns_uri = sandesha2_utils_get_seq_property(env, internal_sequence_id, 
                SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, seq_prop_mgr);

        anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
        if(addr_ns_uri)
        {
            AXIS2_FREE(env->allocator, addr_ns_uri);
        }

        if(!create_seq_added)
        {
            axis2_char_t *acks_to = NULL;
            sandesha2_seq_property_bean_t *reply_to_epr_bean = NULL;
            
            create_seq_added = sandesha2_seq_property_bean_create_with_data(env, 
                    internal_sequence_id, SANDESHA2_SEQ_PROP_OUT_CREATE_SEQ_SENT, AXIS2_VALUE_TRUE);

            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, create_seq_added);

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
                reply_to_epr_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
                        rmd_sequence_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
                if(reply_to_epr_bean)
                {
                    acks_to = sandesha2_seq_property_bean_get_value(reply_to_epr_bean, env);
                }
            }

            /**
             * else if()
             * TODO handle acks_to == anon_uri case
             */
            sandesha2_app_msg_processor_send_create_seq_msg(env, rm_msg_ctx, internal_sequence_id, 
                    acks_to, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);
            
            if(reply_to_epr_bean)
            {
                sandesha2_seq_property_bean_free(reply_to_epr_bean, env);
            }
        }
        
        sandesha2_seq_property_bean_free(create_seq_added, env);
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
        if(soap_action)
        {
            axis2_msg_ctx_set_soap_action(msg_ctx, env, soap_action);
            axutil_string_free(soap_action, env);
        }
    }
    
    if(!dummy_msg)
    {
        storage_key = axutil_uuid_gen(env);

        status = sandesha2_app_msg_processor_send_app_msg(env, rm_msg_ctx, internal_sequence_id, 
                msg_number, storage_key, storage_mgr, create_seq_mgr, seq_prop_mgr, sender_mgr);
        if(storage_key)
        {
            AXIS2_FREE(env->allocator, storage_key);
        }
    }
   
    if(axis2_msg_ctx_get_server_side(msg_ctx, env))
    {
        axis2_core_utils_reset_out_msg_ctx(env, msg_ctx);
    }

    axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_TRUE);

    if(internal_sequence_id)
    {
        AXIS2_FREE(env->allocator, internal_sequence_id);
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
    axis2_bool_t ret = AXIS2_FALSE;
    
    AXIS2_PARAM_CHECK(env->error, str_list, AXIS2_FALSE);
    sprintf(str_long, "%ld", num);
    list = sandesha2_utils_get_array_list_from_string(env, str_list);
    if(list)
    {
        int i = 0, size = 0;

        if(axutil_array_list_contains(list, env, str_long))
        {
            ret =  AXIS2_TRUE;
        }

        size = axutil_array_list_size(list, env);
        for(i = 0; i < size; i++)
        {
            axis2_char_t *str = axutil_array_list_get(list, env, i);
            if(str)
            {
                AXIS2_FREE(env->allocator, str);
                str = NULL;
            }
        }
        axutil_array_list_free(list, env);
    }

    return ret;
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_send_ack_if_reqd(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *msg_str,
    axis2_char_t *rmd_sequence_id,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_sender_mgr_t *sender_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    /*axis2_endpoint_ref_t *to_epr = NULL;
    axis2_endpoint_ref_t *temp_to_epr = NULL;*/
    const axis2_char_t *reply_to_addr = NULL;
    sandesha2_seq_property_bean_t *acks_to_bean = NULL;
    axis2_char_t *acks_to_str = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_ack_requested_t *ack_requested = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg_ctx = NULL;
    axis2_msg_ctx_t *ack_msg_ctx = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_endpoint_ref_t *reply_to_epr = NULL;
    long send_time = -1;
    axis2_char_t *key = NULL;
    axutil_property_t *property = NULL;
    sandesha2_sender_bean_t *ack_bean = NULL;
    sandesha2_seq_property_bean_t *rms_internal_sequence_bean = NULL;
    axis2_char_t *internal_sequence_id = NULL;
    axis2_bool_t sent = AXIS2_FALSE;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_op_t *op = NULL;
    int mep = -1;
    axis2_char_t *rm_version = NULL;
    axis2_bool_t one_way = AXIS2_FALSE;
    axis2_bool_t is_anonymous_reply_to = AXIS2_FALSE;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[Sandesha2] Entry:sandesha2_app_msg_processor_send_ack_if_reqd");

    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_str, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);

    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);

    acks_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id,
        SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
    if(acks_to_bean)
    {
        acks_to_str = axutil_strdup(env, sandesha2_seq_property_bean_get_value(acks_to_bean, env));
        sandesha2_seq_property_bean_free(acks_to_bean, env);
    }
    else
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] acknowledgment bean is null");
        return AXIS2_FAILURE;
    }

    /*temp_to_epr = axis2_msg_ctx_get_to(msg_ctx, env);
    if(temp_to_epr)
    {
        const axis2_char_t *temp_to_addr = NULL;

        temp_to_addr = axis2_endpoint_ref_get_address(temp_to_epr, env);
        if(temp_to_addr)
        {
            to_epr = axis2_endpoint_ref_create(env, temp_to_addr);
        }
    }*/

    reply_to_epr = axis2_msg_ctx_get_reply_to(msg_ctx, env);
    if(reply_to_epr)
    {
       reply_to_addr = axis2_endpoint_ref_get_address(reply_to_epr, env);
    }

    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    if(op_ctx)
    {
        op = axis2_op_ctx_get_op(op_ctx, env);
        mep = axis2_op_get_axis_specific_mep_const(op, env);
    }

    one_way = AXIS2_MEP_CONSTANT_IN_ONLY == mep;
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "MEP:%d", mep);

    rm_version = sandesha2_utils_get_rm_version(env, rmd_sequence_id, seq_prop_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Unable to find RM spec version for seq_id %s", rmd_sequence_id);
        if(acks_to_str)
        {
            AXIS2_FREE(env->allocator, acks_to_str);
        }
        return AXIS2_FAILURE;
    }

    is_anonymous_reply_to = !reply_to_addr || (reply_to_addr && sandesha2_utils_is_anon_uri(env, reply_to_addr));
    /*if(sandesha2_utils_is_rm_1_0_anonymous_acks_to(env, rm_version, acks_to_str) 
            && is_anonymous_reply_to && !one_way)*/
    if(sandesha2_utils_is_anon_uri(env, acks_to_str) && is_anonymous_reply_to && !one_way)
    {
        /* This means acknowledgment address is anomymous and RM version is 1.0. Flow comes to
         * this block only in the server side.
         * In other words this is RM 1.0 replay model in application server side. In this case 
         * we do not send the acknowledgment message here. Instead we send it in the message out path.
         * See sandesha2_app_msg_processor_send_app_msg() code.
         */
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] RM 1.0 replay model in application server side");

        if(acks_to_str)
        {
            AXIS2_FREE(env->allocator, acks_to_str);
        }
        
        if(rm_version)
        {
            AXIS2_FREE(env->allocator, rm_version);
        }

        AXIS2_LOG_INFO(env->log, "[Sandesha2] Exit:sandesha2_app_msg_processor_send_ack_if_reqd");

        return AXIS2_SUCCESS;
    } 

    if(acks_to_str)
    {
        AXIS2_FREE(env->allocator, acks_to_str);
    }
        
    if(rm_version)
    {
        AXIS2_FREE(env->allocator, rm_version);
    }

    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    if(!conf_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] cont_ctx is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    
    ack_requested = sandesha2_msg_ctx_get_ack_requested(rm_msg_ctx, env);
    if(ack_requested)
    {
        sandesha2_ack_requested_set_must_understand(ack_requested, env, AXIS2_FALSE);
        sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    }

    ack_rm_msg_ctx = sandesha2_ack_mgr_generate_ack_msg(env, rm_msg_ctx, rmd_sequence_id, seq_prop_mgr);
    ack_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(ack_rm_msg_ctx, env);

    /* If it is not one way message we piggyback the acknowledgment messages on the application messages
     * or terminate message. So here we store them in the database so that when the application/terminate
     * message sent it pick it up from the database to piggyback. See app_msg_send() function.
     */
    if(!one_way)
    {
        axis2_relates_to_t *relates_to = NULL;
        const axis2_char_t *related_msg_id = NULL;
        axis2_char_t *outgoing_seq_id = NULL;
        sandesha2_seq_property_bean_t *relates_to_bean = NULL;
        sandesha2_seq_property_bean_t *outgoing_seq_id_bean = NULL;

        rms_internal_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
                rmd_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_INTERNAL_SEQ_ID);
      
        if(rms_internal_sequence_bean)
        {
            internal_sequence_id = sandesha2_seq_property_bean_get_value(rms_internal_sequence_bean, env);
        }

        key = axutil_uuid_gen(env);
        ack_bean = sandesha2_sender_bean_create(env);

        relates_to = axis2_msg_ctx_get_relates_to(msg_ctx, env);
        if(relates_to)
        {

            related_msg_id = axis2_relates_to_get_value(relates_to, env);
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "related_msg_id:%s", related_msg_id);
            relates_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env,
                    related_msg_id, SANDESHA2_SEQ_PROP_RELATED_MSG_ID);
            if(relates_to_bean)
            {

                outgoing_seq_id = sandesha2_seq_property_bean_get_value(relates_to_bean, env);

                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "outgoing_seq_id:%s", outgoing_seq_id);
                sandesha2_sender_bean_set_seq_id(ack_bean, env, outgoing_seq_id);
                sandesha2_seq_property_mgr_remove(seq_prop_mgr, env, (axis2_char_t *) related_msg_id, 
                        SANDESHA2_SEQ_PROP_RELATED_MSG_ID);
            }
        }

        if(!outgoing_seq_id)
        {
            axis2_char_t *outgoing_internal_seq_id = NULL;
            axis2_char_t *outgoing_seq_id = NULL;

            outgoing_internal_seq_id = sandesha2_utils_get_internal_sequence_id(env, rmd_sequence_id);
            outgoing_seq_id_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env,
                    outgoing_internal_seq_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);
            if(outgoing_seq_id_bean)
            {
                outgoing_seq_id = sandesha2_seq_property_bean_get_value(outgoing_seq_id_bean, env);
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "outgoing_seq_id:%s", outgoing_seq_id);
                sandesha2_sender_bean_set_seq_id(ack_bean, env, outgoing_seq_id);
            }
        }

        sandesha2_sender_bean_set_msg_ctx_ref_key(ack_bean, env, key);
        send_time = sandesha2_utils_get_current_time_in_millis(env);
        sandesha2_sender_bean_set_time_to_send(ack_bean, env, send_time);
        sandesha2_sender_bean_set_msg_id(ack_bean, env, sandesha2_msg_ctx_get_msg_id(ack_rm_msg_ctx, env));
        sandesha2_sender_bean_set_send(ack_bean, env, AXIS2_TRUE);
        sandesha2_sender_bean_set_internal_seq_id(ack_bean, env, internal_sequence_id);
        /*sandesha2_sender_bean_set_seq_id(ack_bean, env, rmd_sequence_id);*/
        sandesha2_sender_bean_set_msg_type(ack_bean, env, SANDESHA2_MSG_TYPE_ACK);
        sandesha2_sender_bean_set_resend(ack_bean, env, AXIS2_FALSE);
        sandesha2_sender_mgr_insert(sender_mgr, env, ack_bean);

        if(relates_to_bean)
        {
            sandesha2_seq_property_bean_free(relates_to_bean, env);
        }
        
        if(outgoing_seq_id_bean)
        {
            sandesha2_seq_property_bean_free(outgoing_seq_id_bean, env);
        }

        if(rms_internal_sequence_bean)
        {
            sandesha2_seq_property_bean_free(rms_internal_sequence_bean, env);
        }

        if(ack_bean)
        {
            sandesha2_sender_bean_free(ack_bean, env);
        }

        property = axutil_property_create_with_args(env, 0, AXIS2_TRUE, 0, key);
        axis2_msg_ctx_set_property(ack_msg_ctx, env, SANDESHA2_MESSAGE_STORE_KEY, property);
    }

    /*if(to_epr)
    {
        axis2_msg_ctx_set_reply_to(ack_msg_ctx, env, to_epr);
    }*/

    /* If it is one way message in server side this is the only place we can send the acknowledgment.
     * In all other cases we do not send the acknowledgment directly, but piggyback it on application
     * messages or terminate sequence message.
     */
    /*if(ack_rm_msg_ctx && one_way)*/
    if(ack_rm_msg_ctx)
    {
        axis2_engine_t *engine = NULL;
        engine = axis2_engine_create(env, conf_ctx);

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Back channel is free");

        sent = axis2_engine_send(engine, env, ack_msg_ctx);
        if(engine)
        {
            axis2_engine_free(engine, env);
        }

        /* Reset the message context to avoid double freeing of transport out stream */
        if(ack_msg_ctx)
        {
            axis2_core_utils_reset_out_msg_ctx(env, ack_msg_ctx);
        }
    }

    /* We don't store the acknowledgment in memory. To indicate that it should be stored only in the
     * database we pass AXIS2_FALSE as last argument. This measure is taken to avoid memory corruption.
     */
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, key, ack_msg_ctx, AXIS2_FALSE);

    if(ack_rm_msg_ctx)
    {
        sandesha2_msg_ctx_free(ack_rm_msg_ctx, env);
    }

    /* Since we have stored this in database and when piggybacking it is only taken from database
     * we can free this now.
     */
    if(ack_msg_ctx)
    {
        axis2_endpoint_ref_t *temp_epr = NULL;

        temp_epr = axis2_msg_ctx_get_to(ack_msg_ctx, env);
        if(temp_epr)
        {
            axis2_endpoint_ref_free(temp_epr, env);
        }

        axis2_core_utils_reset_out_msg_ctx(env, ack_msg_ctx);
        axis2_msg_ctx_free(ack_msg_ctx, env);
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
                    	
static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_send_create_seq_msg(
     const axutil_env_t *env,
     sandesha2_msg_ctx_t *rm_msg_ctx,
     axis2_char_t *internal_sequence_id,
     axis2_char_t *acks_to,
     sandesha2_storage_mgr_t *storage_mgr,
     sandesha2_seq_property_mgr_t *seq_prop_mgr,
     sandesha2_create_seq_mgr_t *create_seq_mgr,
     sandesha2_sender_mgr_t *sender_mgr)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    sandesha2_seq_property_bean_t *rms_sequence_bean = NULL;
    sandesha2_msg_ctx_t *create_seq_rm_msg_ctx = NULL;
    sandesha2_seq_offer_t *seq_offer = NULL;
    axis2_msg_ctx_t *create_seq_msg_ctx = NULL;
    sandesha2_create_seq_bean_t *create_seq_bean = NULL;
    axis2_char_t *addr_ns_uri = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_char_t *create_sequence_msg_store_key = NULL;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    axis2_engine_t *engine = NULL;
    axis2_op_t *create_seq_op = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    axis2_bool_t continue_sending = AXIS2_TRUE;
    long retrans_interval = -1;
    sandesha2_property_bean_t *property_bean = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_char_t *msg_id = NULL;
    sandesha2_sender_bean_t *create_sequence_sender_bean = NULL;
    long millisecs = 0;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    axis2_char_t *reply_to_addr = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_bool_t is_svr_side = AXIS2_FALSE;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Entry:sandesha2_app_msg_processor_send_create_seq_msg");

    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, internal_sequence_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, acks_to, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, create_seq_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, sender_mgr, AXIS2_FAILURE);
    
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    create_seq_rm_msg_ctx = sandesha2_msg_creator_create_create_seq_msg(env, rm_msg_ctx, internal_sequence_id, 
            acks_to, seq_prop_mgr);
    if(!create_seq_rm_msg_ctx)
    {
        return AXIS2_FAILURE;
    }

    sandesha2_msg_ctx_set_flow(create_seq_rm_msg_ctx, env, SANDESHA2_MSG_CTX_OUT_FLOW);

    create_seq_part = sandesha2_msg_ctx_get_create_seq(create_seq_rm_msg_ctx, env);
    {
        sandesha2_seq_property_bean_t *to_epr_bean = NULL;

        axis2_endpoint_ref_t *to_epr = axis2_msg_ctx_get_to(msg_ctx, env);

        if(to_epr)
        {
            axis2_char_t *to_str = (axis2_char_t *)axis2_endpoint_ref_get_address(to_epr, env);

            to_epr_bean = sandesha2_seq_property_bean_create_with_data(env, internal_sequence_id, 
                    SANDESHA2_SEQ_PROP_TO_EPR, to_str);
            if(to_epr_bean)
            {
                sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, to_epr_bean);
                sandesha2_seq_property_bean_free(to_epr_bean, env);
            }
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
        sandesha2_seq_property_bean_set_seq_id(offer_seq_bean, env, internal_sequence_id);
        sandesha2_seq_property_bean_set_value(offer_seq_bean, env, seq_offer_id);
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, offer_seq_bean);
        sandesha2_seq_property_bean_free(offer_seq_bean, env);
    }

    create_seq_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(create_seq_rm_msg_ctx, env);
    if(!create_seq_msg_ctx)
    {
        if(create_seq_rm_msg_ctx)
        {
            sandesha2_msg_ctx_free(create_seq_rm_msg_ctx, env);
        }

        return AXIS2_FAILURE;
    }

    axis2_msg_ctx_set_relates_to(create_seq_msg_ctx, env, NULL);

    /* Create sequence message created here will be used by create sequence response message processor
     * to retrieve message id
     */
    create_sequence_msg_store_key = axutil_uuid_gen(env);
    create_seq_bean = sandesha2_create_seq_bean_create_with_data(env, internal_sequence_id, 
            (axis2_char_t*)axis2_msg_ctx_get_wsa_message_id(create_seq_msg_ctx, env), NULL);

    if(create_seq_bean)
    {
        sandesha2_create_seq_bean_set_ref_msg_store_key(create_seq_bean, env, create_sequence_msg_store_key);
        sandesha2_create_seq_mgr_insert(create_seq_mgr, env, create_seq_bean);
        sandesha2_create_seq_bean_free(create_seq_bean, env);
    }

    addr_ns_uri = sandesha2_utils_get_seq_property(env, internal_sequence_id, 
            SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, seq_prop_mgr);

    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    if(addr_ns_uri)
    {
        AXIS2_FREE(env->allocator, addr_ns_uri);
    }

    if(!axis2_msg_ctx_get_reply_to(create_seq_msg_ctx, env))
    {
        axis2_endpoint_ref_t *cs_epr = NULL;
        cs_epr = axis2_endpoint_ref_create(env, anon_uri);
        axis2_msg_ctx_set_reply_to(create_seq_msg_ctx, env, cs_epr);
    }

    create_sequence_sender_bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_msg_ctx_ref_key(create_sequence_sender_bean, env, create_sequence_msg_store_key);
    millisecs = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(create_sequence_sender_bean, env, millisecs);
    msg_id = sandesha2_msg_ctx_get_msg_id(create_seq_rm_msg_ctx, env);
    sandesha2_sender_bean_set_msg_id(create_sequence_sender_bean, env, msg_id);
    sandesha2_sender_bean_set_internal_seq_id(create_sequence_sender_bean, env, internal_sequence_id);
    sandesha2_sender_bean_set_send(create_sequence_sender_bean, env, AXIS2_TRUE);
    sandesha2_sender_bean_set_msg_type(create_sequence_sender_bean, env, SANDESHA2_MSG_TYPE_CREATE_SEQ);
    sandesha2_sender_mgr_insert(sender_mgr, env, create_sequence_sender_bean);
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, create_sequence_msg_store_key, create_seq_msg_ctx, AXIS2_TRUE);
    AXIS2_FREE(env->allocator, create_sequence_msg_store_key);

    conf_ctx = axis2_msg_ctx_get_conf_ctx(create_seq_msg_ctx, env);
    engine = axis2_engine_create(env, conf_ctx);

    if(create_seq_rm_msg_ctx)
    {
        sandesha2_msg_ctx_free(create_seq_rm_msg_ctx, env);
    }


    property_bean = sandesha2_utils_get_property_bean(env, axis2_conf_ctx_get_conf(conf_ctx, env));
    retrans_interval = sandesha2_property_bean_get_retrans_interval(property_bean, env); 

    create_seq_op = axis2_msg_ctx_get_op(create_seq_msg_ctx, env);
    transport_out = axis2_msg_ctx_get_transport_out_desc(create_seq_msg_ctx, env);
    transport_sender = axis2_transport_out_desc_get_sender(transport_out, env);

    reply_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQ_PROP_REPLY_TO_EPR);

    if(reply_to_bean)
    {
        reply_to_addr = sandesha2_seq_property_bean_get_value(reply_to_bean, env);
    }

    rm_version = sandesha2_utils_get_rm_version(env, internal_sequence_id, seq_prop_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Unable to find RM spec version for the rms internal_sequence_id %s", 
                internal_sequence_id);

        return AXIS2_FAILURE;
    }
    
    is_svr_side = axis2_msg_ctx_get_server_side(create_seq_msg_ctx, env);

    if(!is_svr_side && (!reply_to_addr || sandesha2_utils_is_rm_1_0_anonymous_acks_to(env, 
            rm_version, reply_to_addr)))
    {
        if(axis2_engine_send(engine, env, create_seq_msg_ctx))
        {
            if(!axis2_msg_ctx_get_server_side(create_seq_msg_ctx, env))
            {
                status = sandesha2_app_msg_processor_process_create_seq_response(env, create_seq_msg_ctx, 
                        storage_mgr);
            }
        }
        else
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Engine Send failed");
        }
        
        if(engine)
        {
            axis2_engine_free(engine, env);
        }

        rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);

        while(!rms_sequence_bean)
        {
            continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env, create_sequence_sender_bean, 
                    conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);

            sandesha2_sender_mgr_update(sender_mgr, env, create_sequence_sender_bean);

            if(!continue_sending)
            {
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                        "[sandesha2] Do not continue sending the create sequence message");
                break;
            }

            AXIS2_SLEEP(retrans_interval);

            if(transport_sender)
            {
                /* This is neccessary to avoid a double free */
                axis2_msg_ctx_set_property(msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
                if(!AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, create_seq_msg_ctx))
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Transport sender invoke failed");
                }
            }

            if(!axis2_msg_ctx_get_server_side(create_seq_msg_ctx, env))
            {
                status = sandesha2_app_msg_processor_process_create_seq_response(env, create_seq_msg_ctx, 
                    storage_mgr);
        
                if(AXIS2_SUCCESS != status)
                {
                    break;
                }
            }

            rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
                SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);
        }

        if(rms_sequence_bean)
        {
            sandesha2_seq_property_bean_free(rms_sequence_bean, env);
        }

        if(create_sequence_sender_bean)
        {
            sandesha2_sender_bean_free(create_sequence_sender_bean, env);
        }

        /* We have created this message context using sandesha2_utils_create_new_related_msg_ctx(). It is our
         * reponsiblity to free if after use.
         */
        if(create_seq_msg_ctx)
        {
            axis2_msg_ctx_free(create_seq_msg_ctx, env);
        }
    }
    else
    {
        if(axis2_engine_send(engine, env, create_seq_msg_ctx))
        {
            if(!axis2_msg_ctx_get_server_side(create_seq_msg_ctx, env))
            {
                status = sandesha2_app_msg_processor_process_create_seq_response(env, create_seq_msg_ctx, 
                        storage_mgr);
            }
        }
        else
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Engine Send failed");
        }
        
        if(engine)
        {
            axis2_engine_free(engine, env);
        }

        rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);
        /* Dual channel */
        sandesha2_app_msg_processor_start_create_seq_msg_resender(env, conf_ctx, internal_sequence_id, 
            msg_id, is_svr_side, retrans_interval, create_sequence_sender_bean, create_seq_msg_ctx);
    }

    if(rm_version)
    {
        AXIS2_FREE(env->allocator, rm_version);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2]Exit:sandesha2_app_msg_processor_send_create_seq_msg");

    return status;
}

static axis2_status_t
sandesha2_app_msg_processor_start_create_seq_msg_resender(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *internal_sequence_id,
    axis2_char_t *msg_id,
    const axis2_bool_t is_server_side,
    int retrans_interval,
    sandesha2_sender_bean_t *create_sequence_sender_bean,
    axis2_msg_ctx_t *create_seq_msg_ctx)
{
    axutil_thread_t *worker_thread = NULL;
    sandesha2_app_msg_processor_args_t *args = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Entry:sandesha2_app_msg_processor_start_create_seq_msg_resender");
    
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_app_msg_processor_args_t));
    args->env = axutil_init_thread_env(env);
    args->conf_ctx = conf_ctx;
    args->internal_sequence_id = internal_sequence_id;
    args->msg_id = msg_id;
    args->retrans_interval = retrans_interval;
    args->is_server_side = is_server_side;
    args->bean = create_sequence_sender_bean;
    args->msg_ctx = create_seq_msg_ctx;

    worker_thread = axutil_thread_pool_get_thread(env->thread_pool, 
            sandesha2_app_msg_processor_create_seq_msg_worker_function, (void*)args);
    if(!worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Thread creation failed for sandesha2_app_msg_processor_start_create_seq_msg_resender");
        return AXIS2_FAILURE;
    }

    axutil_thread_pool_thread_detach(env->thread_pool, worker_thread);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
            "[sandesha2] Exit:sandesha2_app_msg_processor_start_create_seq_msg_resender");
    return AXIS2_SUCCESS;
}

static void * AXIS2_THREAD_FUNC
sandesha2_app_msg_processor_create_seq_msg_worker_function(
    axutil_thread_t *thd, 
    void *data)
{
    sandesha2_app_msg_processor_args_t *args;
    axutil_env_t *env = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_sender_mgr_t *sender_mgr = NULL;
    int retrans_interval = 0;
    axis2_char_t *dbname = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_char_t *internal_sequence_id = NULL;
    axis2_bool_t is_server_side = AXIS2_FALSE;
    sandesha2_sender_bean_t *create_sequence_sender_bean = NULL;
    axis2_char_t *msg_id = NULL;
    /*sandesha2_seq_property_bean_t *rms_sequence_bean = NULL;*/
    axis2_bool_t continue_sending = AXIS2_TRUE;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    axis2_op_t *create_seq_op = NULL;
    axis2_msg_ctx_t *create_seq_msg_ctx = NULL;
    sandesha2_sender_bean_t *find_sender_bean = NULL;
    sandesha2_sender_bean_t *sender_bean = NULL;

    args = (sandesha2_app_msg_processor_args_t*) data;
    env = args->env;
    axutil_allocator_switch_to_global_pool(env->allocator);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2] Entry:sandesha2_app_msg_processor_create_seq_msg_worker_function");

    conf_ctx = args->conf_ctx;
    msg_id = args->msg_id;
    internal_sequence_id = axutil_strdup(env, args->internal_sequence_id);
    is_server_side = args->is_server_side;
    retrans_interval = args->retrans_interval;
    create_sequence_sender_bean = (sandesha2_sender_bean_t *) args->bean;
    create_seq_msg_ctx = (axis2_msg_ctx_t *) args->msg_ctx;

    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);

    AXIS2_SLEEP(retrans_interval);

    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "dam_internal_sequence_id:%s", internal_sequence_id);

    find_sender_bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_msg_type(find_sender_bean, env, SANDESHA2_MSG_TYPE_CREATE_SEQ);
    sandesha2_sender_bean_set_internal_seq_id(find_sender_bean, env, internal_sequence_id);
    sandesha2_sender_bean_set_send(find_sender_bean, env, AXIS2_TRUE);

    sender_bean = sandesha2_sender_mgr_find_unique(sender_mgr, env, find_sender_bean);

    /*rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);*/

    create_seq_op = axis2_msg_ctx_get_op(create_seq_msg_ctx, env);
    transport_out = axis2_msg_ctx_get_transport_out_desc(create_seq_msg_ctx, env);
    transport_sender = axis2_transport_out_desc_get_sender(transport_out, env);


    while(sender_bean)
    {
        if(sender_bean)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "sender bean found");
            sandesha2_sender_bean_free(sender_bean, env);
            sender_bean = NULL;
        }

        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "rms_sequence_bean is NULL");

        continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env, create_sequence_sender_bean, 
                conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);

        sandesha2_sender_mgr_update(sender_mgr, env, create_sequence_sender_bean);

        if(!continue_sending)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                    "[sandesha2] Do not continue sending the create sequence message");
            break;
        }

        AXIS2_SLEEP(retrans_interval);

        if(transport_sender)
        {
            /* This is neccessary to avoid a double free */
            axis2_msg_ctx_set_property(create_seq_msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
            if(!AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, create_seq_msg_ctx))
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Transport sender invoke failed");
            }
        }

        /*if(!axis2_msg_ctx_get_server_side(create_seq_msg_ctx, env))
        {
            status = sandesha2_app_msg_processor_process_create_seq_response(env, create_seq_msg_ctx, 
                storage_mgr);
    
            if(AXIS2_SUCCESS != status)
            {
                break;
            }
        }*/

        sender_bean = sandesha2_sender_mgr_find_unique(sender_mgr, env, find_sender_bean);
        /*rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);*/
    }

    /*if(rms_sequence_bean)
    {
        sandesha2_seq_property_bean_free(rms_sequence_bean, env);
    }*/
    
    if(find_sender_bean)
    {
        sandesha2_sender_bean_free(find_sender_bean, env);
    }

    if(create_seq_msg_ctx)
    {
        axis2_msg_ctx_free(create_seq_msg_ctx, env);
    }

    if(storage_mgr)
    {
        sandesha2_storage_mgr_free(storage_mgr, env);
    }
    
    if(create_seq_mgr)
    {
        sandesha2_create_seq_mgr_free(create_seq_mgr, env);
    }
    
    if(sender_mgr)
    {
        sandesha2_sender_mgr_free(sender_mgr, env);
    }
    
    if(seq_prop_mgr)
    {
        sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2] Exit:sandesha2_app_msg_processor_create_seq_msg_worker_function");
    
    axutil_allocator_switch_to_local_pool(env->allocator);

    return NULL;
}

static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_process_create_seq_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *create_seq_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_msg_ctx_t *response_msg_ctx = NULL;
    axiom_soap_envelope_t *response_envelope = NULL;
    axis2_char_t *soap_ns_uri = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_engine_t *engine = NULL;
    axis2_status_t status = AXIS2_FAILURE;
   
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Entry:sandesha2_app_msg_processor_process_create_seq_response");
    
    AXIS2_PARAM_CHECK(env->error, create_seq_msg_ctx, AXIS2_FAILURE);
   
    conf_ctx = axis2_msg_ctx_get_conf_ctx(create_seq_msg_ctx, env);

    soap_ns_uri = axis2_msg_ctx_get_is_soap_11(create_seq_msg_ctx, env) ?
         AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
         AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;

    response_envelope = axis2_msg_ctx_get_response_soap_envelope(create_seq_msg_ctx, env);
    if(!response_envelope)
    {
        response_envelope = (axiom_soap_envelope_t *) axis2_http_transport_utils_create_soap_msg(env, 
                create_seq_msg_ctx, soap_ns_uri);
        if(!response_envelope)
        {
            /* There is no response message context. */

            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Response envelope not found");

            return AXIS2_SUCCESS;
        }
    }
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
        "[sandesha2] Response envelope for CreateSequenceResponse message found");

    response_msg_ctx = axis2_msg_ctx_create(env, conf_ctx, 
        axis2_msg_ctx_get_transport_in_desc(create_seq_msg_ctx, env), 
        axis2_msg_ctx_get_transport_out_desc(create_seq_msg_ctx, env));

    axis2_msg_ctx_set_status_code (response_msg_ctx, env, axis2_msg_ctx_get_status_code (create_seq_msg_ctx, env));

    /* Note that we set here as client side to indicate that we are in the application client side. */
    axis2_msg_ctx_set_server_side(response_msg_ctx, env, AXIS2_FALSE);

    axis2_msg_ctx_set_op_ctx(response_msg_ctx, env, axis2_msg_ctx_get_op_ctx(create_seq_msg_ctx, env));
    axis2_msg_ctx_set_conf_ctx(response_msg_ctx, env, conf_ctx);
    axis2_msg_ctx_set_svc_ctx(response_msg_ctx, env, axis2_msg_ctx_get_svc_ctx(create_seq_msg_ctx, env));
    axis2_msg_ctx_set_svc_grp_ctx(response_msg_ctx, env, axis2_msg_ctx_get_svc_grp_ctx(create_seq_msg_ctx, 
                env));

    axis2_msg_ctx_set_soap_envelope(response_msg_ctx, env, response_envelope);

    engine = axis2_engine_create(env, conf_ctx);
    if(engine)
    {
        if(sandesha2_util_is_fault_envelope(env, response_envelope))
        {
            status = axis2_engine_receive_fault(engine, env, response_msg_ctx);
        }
        else
        {
            /* Note that this engine flow does not end with an message receiver, because
             * when hit sandesha2_create_seq_response_msg_processor_process_in_msg()
             * function it pause message context at the end of the function.
             */
            status = axis2_engine_receive(engine, env, response_msg_ctx);
        }

        axis2_engine_free(engine, env);
    }

    /* Note that as explained above this message context is not added to the operation context, 
     * therefore will not be freed when operation context's msg_ctx_map is freed. So we need to 
     * free the response message here.
     */
    axis2_msg_ctx_free(response_msg_ctx, env);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Exit:sandesha2_app_msg_processor_process_create_seq_response");

    return status;
}

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_send_app_msg(
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *internal_sequence_id,
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
    sandesha2_seq_property_bean_t *from_acks_to_bean = NULL;
    sandesha2_seq_property_bean_t *rms_sequence_bean = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_endpoint_ref_t *reply_to_epr = NULL;
    axis2_char_t *from_acks_to_addr = NULL;
    axis2_char_t *to_addr = NULL;
    axis2_char_t *reply_to_addr = NULL;
    axis2_char_t *new_to_str = NULL;
    sandesha2_seq_t *seq = NULL;
    sandesha2_seq_t *req_seq = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_char_t *rm_ns_val = NULL;
    sandesha2_msg_number_t *msg_number = NULL;
    axis2_msg_ctx_t *req_msg = NULL;
    axis2_char_t *rms_sequence_id = NULL;
    sandesha2_sender_bean_t *app_msg_sender_bean = NULL;
    long millisecs = 0;
    /*axutil_property_t *property = NULL;*/
    axis2_engine_t *engine = NULL;
    sandesha2_identifier_t *identifier = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_bool_t last_msg = AXIS2_FALSE;
    axis2_op_ctx_t *temp_op_ctx = NULL;
    axis2_status_t status = AXIS2_SUCCESS;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_bool_t is_svr_side = AXIS2_FALSE;
    axis2_bool_t continue_sending = AXIS2_TRUE;
    sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
    axis2_msg_ctx_t *req_msg_ctx = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_char_t *rmd_sequence_id = NULL;
    axis2_module_desc_t *module_desc = NULL;
    axutil_qname_t *qname = NULL;
    axutil_param_t *retrans_interval_param = NULL;
    int retrans_interval = 0;
    axis2_conf_t *conf = NULL;
    const axis2_char_t *mep = NULL;
    axis2_relates_to_t *relates_to = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,   
        "[Sandesha2] Entry:sandesha2_app_msg_processor_send_app_msg");

    AXIS2_PARAM_CHECK(env->error, internal_sequence_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_key, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, sender_mgr, AXIS2_FAILURE);
 
    app_msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(app_msg_ctx, env);

    relates_to = axis2_msg_ctx_get_relates_to(app_msg_ctx, env);
    if(relates_to)
    {
        sandesha2_seq_property_bean_t *response_relates_to_bean = NULL; 
        const axis2_char_t *relates_to_value = axis2_relates_to_get_value(relates_to, env);

        response_relates_to_bean = sandesha2_seq_property_bean_create_with_data(env, 
                internal_sequence_id, SANDESHA2_SEQ_PROP_HIGHEST_OUT_RELATES_TO, 
                (axis2_char_t *) relates_to_value);
        
        if(response_relates_to_bean)
        {
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, response_relates_to_bean);
            sandesha2_seq_property_bean_free(response_relates_to_bean, env);
        }
    }
  
    sandesha2_app_msg_processor_set_last_out_msg_no(env, internal_sequence_id, msg_num, seq_prop_mgr);

    to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQ_PROP_TO_EPR);

    reply_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, internal_sequence_id, 
            SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
    
    rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            internal_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);

    while(!rms_sequence_bean)
    {
        rms_sequence_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
                internal_sequence_id, SANDESHA2_SEQUENCE_PROPERTY_RMS_SEQ_ID);
        AXIS2_SLEEP(1);
    }

    if(rms_sequence_bean)
    {
        rms_sequence_id = axutil_strdup(env, sandesha2_seq_property_bean_get_value(rms_sequence_bean, 
                    env));
        sandesha2_seq_property_bean_free(rms_sequence_bean, env);
    }

    if (to_bean)
    {
        to_addr = axutil_strdup(env, sandesha2_seq_property_bean_get_value(to_bean, env));
        to_epr = axis2_endpoint_ref_create(env, to_addr);
        sandesha2_seq_property_bean_free(to_bean, env);
    }
    
    if(reply_to_bean)
    {
        reply_to_addr = axutil_strdup(env, sandesha2_seq_property_bean_get_value(reply_to_bean, env));
        reply_to_epr = axis2_endpoint_ref_create(env, reply_to_addr);
        sandesha2_msg_ctx_set_reply_to(rm_msg_ctx, env, reply_to_epr);

        sandesha2_seq_property_bean_free(reply_to_bean, env);
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
        axis2_endpoint_ref_t *temp_to_epr = NULL;
        
        temp_to_epr = axis2_endpoint_ref_create(env, new_to_str);
        /*sandesha2_msg_ctx_set_to(rm_msg_ctx, env, temp_to_epr);*/
        if(to_epr)
        {
            axis2_endpoint_ref_free(to_epr, env);
        }
    }
    else if (to_epr)
    {
        sandesha2_msg_ctx_set_to(rm_msg_ctx, env, to_epr);
    }

    rm_version = sandesha2_utils_get_rm_version(env, internal_sequence_id, seq_prop_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Unable to find RM spec version for the rms internal_sequence_id %s", 
                internal_sequence_id);

        if(to_addr)
        {
            AXIS2_FREE(env->allocator, to_addr);
        }

        if(reply_to_addr)
        {
            AXIS2_FREE(env->allocator, reply_to_addr);
        }

        if(rms_sequence_id)
        {
            AXIS2_FREE(env->allocator, rms_sequence_id);
        }

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

            if(to_addr)
            {
                AXIS2_FREE(env->allocator, to_addr);
            }
            if(reply_to_addr)
            {
                AXIS2_FREE(env->allocator, reply_to_addr);
            }

            if(rms_sequence_id)
            {
                AXIS2_FREE(env->allocator, rms_sequence_id);
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

    if(!rms_sequence_id)
    {
        rms_sequence_id = axutil_strdup(env, SANDESHA2_TEMP_SEQ_ID); /* Why should we do this?:damitha */
    }
        
    identifier = sandesha2_identifier_create(env, rm_ns_val);
    sandesha2_identifier_set_identifier(identifier, env, rms_sequence_id);
    sandesha2_seq_set_identifier(seq, env, identifier);
    sandesha2_msg_ctx_set_sequence(rm_msg_ctx, env, seq);

    /* TODO add_ack_requested */

    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
   
    op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
    if(op_ctx)
    {
        req_msg_ctx =  axis2_op_ctx_get_msg_ctx(op_ctx, env, AXIS2_WSDL_MESSAGE_LABEL_IN);
        if(req_msg_ctx)
        {
            req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
            req_seq = sandesha2_msg_ctx_get_sequence(req_rm_msg_ctx, env);
        }
    }

    if(req_seq)
    {
        rmd_sequence_id = sandesha2_identifier_get_identifier(sandesha2_seq_get_identifier(req_seq, 
                env), env);
    }
    if(rmd_sequence_id)
    {
        from_acks_to_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, rmd_sequence_id, 
            SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
    }

    sandesha2_app_msg_processor_is_last_out_msg(env, app_msg_ctx, rmd_sequence_id, 
        internal_sequence_id, msg_num, seq_prop_mgr);

    if(from_acks_to_bean)
    {
        axis2_endpoint_ref_t *from_acks_to_epr = NULL;

        from_acks_to_addr = axutil_strdup(env, sandesha2_seq_property_bean_get_value(from_acks_to_bean, env));
        from_acks_to_epr = axis2_endpoint_ref_create(env, from_acks_to_addr);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "from_acks_to_address:%s", from_acks_to_addr);

        if(from_acks_to_epr)
        {
            axis2_endpoint_ref_free(from_acks_to_epr, env);
        }
        sandesha2_seq_property_bean_free(from_acks_to_bean, env);
    }

    app_msg_sender_bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_internal_seq_id(app_msg_sender_bean, env, internal_sequence_id);
    sandesha2_sender_bean_set_msg_ctx_ref_key(app_msg_sender_bean, env, storage_key);
    millisecs = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(app_msg_sender_bean, env, millisecs);
    msg_id = sandesha2_msg_ctx_get_msg_id(rm_msg_ctx, env);
    sandesha2_sender_bean_set_msg_id(app_msg_sender_bean, env, msg_id);
    sandesha2_sender_bean_set_msg_no(app_msg_sender_bean, env, msg_num);
    sandesha2_sender_bean_set_msg_type(app_msg_sender_bean, env, SANDESHA2_MSG_TYPE_APPLICATION);

    if(!rms_sequence_id)
    {
        sandesha2_sender_bean_set_send(app_msg_sender_bean, env, AXIS2_FALSE);
    }
    else
    {
        sandesha2_sender_bean_set_send(app_msg_sender_bean, env, AXIS2_TRUE);
        /*property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
        axis2_msg_ctx_set_property(app_msg_ctx, env, SANDESHA2_SET_SEND_TO_TRUE, property);*/
    }

    sandesha2_sender_mgr_insert(sender_mgr, env, app_msg_sender_bean);

    is_svr_side = axis2_msg_ctx_get_server_side(app_msg_ctx, env);

    /* 
     * If server side and anonymous acknowledgment in RM 1.0. In other words this is RM 1.0 replay mode.
     * Note that in this case to_addr is NULL. In duplex mode to_addr cannot be NULL. 
     * */
    /*if(is_svr_side && sandesha2_utils_is_rm_1_0_anonymous_acks_to(env, rm_version, from_acks_to_addr) 
            && !to_addr)*/

    if(is_svr_side && sandesha2_utils_is_anon_uri(env, from_acks_to_addr) && (!to_addr || 
            sandesha2_utils_is_anon_uri(env, to_addr)))
    {
        sandesha2_msg_creator_add_ack_msg(env, rm_msg_ctx, rmd_sequence_id, seq_prop_mgr);
        if(req_rm_msg_ctx)
        {
            sandesha2_msg_ctx_free(req_rm_msg_ctx, env);
        }

        engine = axis2_engine_create(env, conf_ctx);
        status = axis2_engine_resume_send(engine, env, app_msg_ctx);

        if(rm_version)
        {
            AXIS2_FREE(env->allocator, rm_version);
        }

        if(app_msg_sender_bean)
        {
            sandesha2_sender_bean_free(app_msg_sender_bean, env);
        }

        if(engine)
        {
            axis2_engine_free(engine, env);
        }

        if(to_addr)
        {
            AXIS2_FREE(env->allocator, to_addr);
        }
        if(reply_to_addr)
        {
            AXIS2_FREE(env->allocator, reply_to_addr);
        }

        if(from_acks_to_addr)
        {
            AXIS2_FREE(env->allocator, from_acks_to_addr);
        }
        if(rms_sequence_id)
        {
            AXIS2_FREE(env->allocator, rms_sequence_id);
        }
        
        return status;
    }

    if(to_addr)
    {
        AXIS2_FREE(env->allocator, to_addr);
    }

    temp_op_ctx = axis2_msg_ctx_get_op_ctx(app_msg_ctx, env);
    if(temp_op_ctx)
    {
        axis2_op_t *op = NULL;

        op = axis2_op_ctx_get_op(temp_op_ctx, env);
        mep = axis2_op_get_msg_exchange_pattern(op, env);
    }

    continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env, app_msg_sender_bean, 
            conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);

    if(app_msg_sender_bean)
    {
        sandesha2_sender_bean_free(app_msg_sender_bean, env);
    }

    if(!continue_sending)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Do not continue sending the message");

        if(rm_version)
        {
            AXIS2_FREE(env->allocator, rm_version);
        }

        if(reply_to_addr)
        {
            AXIS2_FREE(env->allocator, reply_to_addr);
        }

        if(from_acks_to_addr)
        {
            AXIS2_FREE(env->allocator, from_acks_to_addr);
        }

        if(rms_sequence_id)
        {
            AXIS2_FREE(env->allocator, rms_sequence_id);
        }

        return AXIS2_FAILURE;
    }
 
    axis2_msg_ctx_set_current_handler_index(app_msg_ctx, env, 
            axis2_msg_ctx_get_current_handler_index(app_msg_ctx, env) + 1);

    if(!sandesha2_util_is_ack_already_piggybacked(env, rm_msg_ctx))
    {
        sandesha2_ack_mgr_piggyback_acks_if_present(env, rms_sequence_id, rm_msg_ctx, storage_mgr, 
                seq_prop_mgr, sender_mgr);
    }

    if(!is_svr_side && (!reply_to_addr || sandesha2_utils_is_rm_1_0_anonymous_acks_to(env, rm_version, reply_to_addr)))
    {
        if(!axutil_strcmp(mep, AXIS2_MEP_URI_OUT_IN))
        {
            sandesha2_seq_property_bean_t *replay_bean = NULL;

            replay_bean = sandesha2_seq_property_bean_create_with_data(env, rms_sequence_id, 
                    SANDESHA2_SEQ_PROP_1_0_REPLAY, NULL);
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, replay_bean);
            if(replay_bean)
            {
                sandesha2_seq_property_bean_free(replay_bean, env);
            }
        }
    }

    conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    qname = axutil_qname_create(env, SANDESHA2_MODULE, NULL, NULL);
    module_desc = axis2_conf_get_module(conf, env, qname);
    retrans_interval_param = axis2_module_desc_get_param(module_desc, env, 
            SANDESHA2_PROPERTIES_RETRANSMISSION_INTERVAL);
    if(retrans_interval_param)
    {
        retrans_interval = AXIS2_ATOI(axutil_param_get_value(retrans_interval_param, env));
    }
    if(qname)
    {
        axutil_qname_free(qname, env);
    }

    if(!is_svr_side && (!reply_to_addr || sandesha2_utils_is_rm_1_0_anonymous_acks_to(env, rm_version, reply_to_addr)))
    {
        axis2_transport_out_desc_t *transport_out = NULL;
        axis2_transport_sender_t *transport_sender = NULL;
        sandesha2_sender_bean_t *sender_bean = NULL;
        
        engine = axis2_engine_create(env, conf_ctx);
        if(axis2_engine_resume_send(engine, env, app_msg_ctx))
        {
            if(!axis2_msg_ctx_get_server_side(app_msg_ctx, env))
            {    
                status = sandesha2_app_msg_processor_process_app_msg_response(env, app_msg_ctx);
            }
        }
        else
        {
            AXIS2_LOG_WARNING(env->log, AXIS2_LOG_SI, "[sandesha2] Engine resume send failed");
        }

        if(engine)
        {
            axis2_engine_free(engine, env);
        }

        /* If application client side and single channel, resend is done in the same 
         * thread as the application client.
         */

        sender_bean = sandesha2_sender_mgr_get_application_msg_to_send(sender_mgr, env, 
                internal_sequence_id, msg_id);
        if(!sender_bean)
        {
            /* There is no pending message to send. */
            status = AXIS2_SUCCESS;
        }
        else
        {
            transport_out = axis2_msg_ctx_get_transport_out_desc(app_msg_ctx, env);
            if(transport_out)
            {
                transport_sender = axis2_transport_out_desc_get_sender(transport_out, env);
            }
            if(!transport_sender)
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                        "[sandesha2] Transport sender could not be retrieved from transport_out");
                status = AXIS2_FAILURE;
            }

            while(AXIS2_TRUE && transport_sender)
            {
                continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env, sender_bean, 
                        conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);

                sandesha2_sender_mgr_update(sender_mgr, env, sender_bean);
                
                if(sender_bean)
                {
                    sandesha2_sender_bean_free(sender_bean, env);
                }

                if(!continue_sending)
                {
                    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                            "[sandesha2] Do not continue sending the application message");
                    break;
                }

                AXIS2_SLEEP(retrans_interval);

                /* This is neccessary to avoid a double free */
                axis2_msg_ctx_set_property(app_msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
                if(!AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, app_msg_ctx))
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "[sandesha2] Transport sender invoke failed in sending application message");
                }

                if(!axis2_msg_ctx_get_server_side(app_msg_ctx, env))
                {
                    status = sandesha2_app_msg_processor_process_app_msg_response(env, app_msg_ctx);
            
                    if(AXIS2_SUCCESS != status)
                    {
                        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "[sandesha2] Resend failed for  message id %s in sequence %s", msg_id, 
                            internal_sequence_id);

                        break;
                    }
                }
                
                sender_bean = sandesha2_sender_mgr_get_application_msg_to_send(sender_mgr, env, 
                    internal_sequence_id, msg_id);
                if(!sender_bean)
                {
                    /* There is no pending message to send. So exit from the loop. */
                    break;
                }
            }
        }

        if(rm_version)
        {
            AXIS2_FREE(env->allocator, rm_version);
        }

        if(reply_to_addr)
        {
            AXIS2_FREE(env->allocator, reply_to_addr);
        }

        if(from_acks_to_addr)
        {
            AXIS2_FREE(env->allocator, from_acks_to_addr);
        }

        if(rms_sequence_id)
        {
            AXIS2_FREE(env->allocator, rms_sequence_id);
        }

        return status;
    }
    else
    {
        sandesha2_seq_property_bean_t *relates_to_bean = NULL;

        relates_to_bean = sandesha2_seq_property_bean_create_with_data(env, msg_id, 
                SANDESHA2_SEQ_PROP_RELATED_MSG_ID, rms_sequence_id);
        if(relates_to_bean)
        {
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, relates_to_bean);
            sandesha2_seq_property_bean_free(relates_to_bean, env);
        }

        axis2_msg_ctx_increment_ref(app_msg_ctx, env);
        engine = axis2_engine_create(env, conf_ctx);
        if(axis2_engine_resume_send(engine, env, app_msg_ctx))
        {
            if(!axis2_msg_ctx_get_server_side(app_msg_ctx, env))
            {    
                status = sandesha2_app_msg_processor_process_app_msg_response(env, app_msg_ctx);
            }
        }
        else
        {
            AXIS2_LOG_WARNING(env->log, AXIS2_LOG_SI, "[sandesha2] Engine resume send failed");
        }

        if(engine)
        {
            axis2_engine_free(engine, env);
        }
        
        sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, storage_key, app_msg_ctx, AXIS2_TRUE);

        /* If not (single channel) spawn a thread and see whether acknowledgment has arrived through the 
         * sandesha2_sender_mgr_get_application_msg_to_send() function. If it has arrived exit from
         * the thread.*/
        sandesha2_app_msg_processor_start_application_msg_resender(env, conf_ctx, internal_sequence_id, 
                msg_id, is_svr_side, retrans_interval, app_msg_ctx);
    }
   
    if(rm_version)
    {
        AXIS2_FREE(env->allocator, rm_version);
    }

    if(reply_to_addr)
    {
        AXIS2_FREE(env->allocator, reply_to_addr);
    }

    if(from_acks_to_addr)
    {
        AXIS2_FREE(env->allocator, from_acks_to_addr);
    }
    if(rms_sequence_id)
    {
        AXIS2_FREE(env->allocator, rms_sequence_id);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[Sandesha2] Exit:sandesha2_app_msg_processor_send_app_msg");

    return status;
}

static axis2_status_t
sandesha2_app_msg_processor_start_application_msg_resender(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *internal_sequence_id,
    axis2_char_t *msg_id,
    const axis2_bool_t is_server_side,
    int retrans_interval,
    axis2_msg_ctx_t *app_msg_ctx)
{
    axutil_thread_t *worker_thread = NULL;
    sandesha2_app_msg_processor_args_t *args = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Entry:sandesha2_app_msg_processor_start_application_msg_resender");
    
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_app_msg_processor_args_t));
    args->env = axutil_init_thread_env(env);
    args->conf_ctx = conf_ctx;
    args->internal_sequence_id = internal_sequence_id;
    args->msg_id = msg_id;
    args->retrans_interval = retrans_interval;
    args->is_server_side = is_server_side;
    args->msg_ctx = app_msg_ctx;

    worker_thread = axutil_thread_pool_get_thread(env->thread_pool, 
            sandesha2_app_msg_processor_application_msg_worker_function, (void*)args);
    if(!worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2] Thread creation failed for sandesha2_app_msg_processor_start_application_msg_resender");
        return AXIS2_FAILURE;
    }

    axutil_thread_pool_thread_detach(env->thread_pool, worker_thread);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
            "[sandesha2] Exit:sandesha2_app_msg_processor_start_application_msg_resender");
    return AXIS2_SUCCESS;
}

static void * AXIS2_THREAD_FUNC
sandesha2_app_msg_processor_application_msg_worker_function(
    axutil_thread_t *thd, 
    void *data)
{
    sandesha2_app_msg_processor_args_t *args;
    axutil_env_t *env = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_sender_mgr_t *sender_mgr = NULL;
    int retrans_interval = 0;
    axis2_char_t *dbname = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_char_t *internal_sequence_id = NULL;
    axis2_bool_t is_server_side = AXIS2_FALSE;
    sandesha2_sender_bean_t *sender_bean = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_status_t status = AXIS2_FAILURE;

    args = (sandesha2_app_msg_processor_args_t*) data;
    env = args->env;
    axutil_allocator_switch_to_global_pool(env->allocator);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2] Entry:sandesha2_app_msg_processor_application_msg_worker_function");
    conf_ctx = args->conf_ctx;
    msg_id = axutil_strdup(env, args->msg_id);
    internal_sequence_id = axutil_strdup(env, args->internal_sequence_id);
    is_server_side = args->is_server_side;
    retrans_interval = args->retrans_interval;
    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);

    AXIS2_SLEEP(retrans_interval);
    sender_bean = sandesha2_sender_mgr_get_application_msg_to_send(sender_mgr, env, 
            internal_sequence_id, msg_id);
    if(!sender_bean)
    {
        /* There is no pending message to send. So exit from the thread. */
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] There is no pending message to send. So exit from the thread");
        return NULL;
    }

    while(AXIS2_TRUE)
    {
        axis2_char_t *key = NULL;
        axis2_msg_ctx_t *app_msg_ctx = NULL;

        AXIS2_SLEEP(retrans_interval);
        sender_bean = sandesha2_sender_mgr_get_application_msg_to_send(sender_mgr, env, 
                internal_sequence_id, msg_id);
        if(!sender_bean)
        {
            /* There is no pending message to send. So exit from the thread. */
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                    "[sandesha2] There is no pending message to send. So exit from the thread");
            break;
        }

        key = sandesha2_sender_bean_get_msg_ctx_ref_key(sender_bean, env);
        app_msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, key, conf_ctx, 
                AXIS2_TRUE);

        if(!app_msg_ctx)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                    "[sandesha2] msg_ctx is not present in the store yet.");

            /*msg_ctx is still not stored so try again later.*/
            if(sender_bean)
            {
                sandesha2_sender_bean_free(sender_bean, env);
            }

            break;
        }
        status = sandesha2_app_msg_processor_resend(env, conf_ctx, msg_id, is_server_side,
                internal_sequence_id, storage_mgr, seq_prop_mgr, create_seq_mgr, 
                sender_mgr, app_msg_ctx);

        if(AXIS2_SUCCESS != status)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Resend failed for  message id %s in sequence %s", msg_id, 
                internal_sequence_id);

            if(app_msg_ctx)
            {
                axis2_msg_ctx_free(app_msg_ctx, env);
            }

            if(sender_bean)
            {
                sandesha2_sender_bean_free(sender_bean, env); 
            }
            break;
        }

        if(app_msg_ctx)
        {
            axis2_msg_ctx_free(app_msg_ctx, env);
        }

        if(sender_bean)
        {
            sandesha2_sender_bean_free(sender_bean, env); 
        }
    }

    /*if(internal_sequence_id)
    {
        AXIS2_FREE(env->allocator, internal_sequence_id);
    }
    
    if(msg_id)
    {
        AXIS2_FREE(env->allocator, msg_id);
    }*/

    if(storage_mgr)
    {
        sandesha2_storage_mgr_free(storage_mgr, env);
    }
    
    if(create_seq_mgr)
    {
        sandesha2_create_seq_mgr_free(create_seq_mgr, env);
    }
    
    if(sender_mgr)
    {
        sandesha2_sender_mgr_free(sender_mgr, env);
    }
    
    if(seq_prop_mgr)
    {
        sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2] Exit:sandesha2_app_msg_processor_application_msg_worker_function");
    
    axutil_allocator_switch_to_local_pool(env->allocator);
    
    return NULL;
}

/* This function will be called in the duplex mode only from within the application message sender thread. */
static axis2_status_t
sandesha2_app_msg_processor_resend(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id,
    axis2_bool_t is_svr_side,
    const axis2_char_t *internal_sequence_id,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_sender_mgr_t *sender_mgr,
    axis2_msg_ctx_t *app_msg_ctx)
{
    sandesha2_sender_bean_t *sender_worker_bean = NULL;
    sandesha2_sender_bean_t *bean1 = NULL;
    /*axis2_char_t *key = NULL;*/
    axis2_bool_t continue_sending = AXIS2_TRUE;
    /*sandesha2_msg_ctx_t *rm_msg_ctx = NULL;*/
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

    continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env, sender_worker_bean, 
            conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);
    sandesha2_sender_mgr_update(sender_mgr, env, sender_worker_bean);
    if(!continue_sending)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Do not continue sending the application message");
        if(sender_worker_bean)
        {
            sandesha2_sender_bean_free(sender_worker_bean, env);
        }

        return AXIS2_FAILURE;
    }
    
    /*rm_msg_ctx = sandesha2_msg_init_init_msg(env, app_msg_ctx);
    
    if(!sandesha2_util_is_ack_already_piggybacked(env, app_rm_msg_ctx))
    {
        sandesha2_ack_mgr_piggyback_acks_if_present(env, app_rm_msg_ctx, storage_mgr, seq_prop_mgr, 
                sender_mgr);
    }*/
    
    transport_out = axis2_msg_ctx_get_transport_out_desc(app_msg_ctx, env);
    if(transport_out)
    {
        transport_sender = axis2_transport_out_desc_get_sender(transport_out, env);
    }
    if(transport_sender)
    {
        /* This is neccessary to avoid a double free at http_sender.c */
        axis2_msg_ctx_set_property(app_msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
        if(AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, app_msg_ctx))
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
            int sent_count = -1;

            sent_count = sandesha2_sender_bean_get_sent_count(sender_worker_bean, env);

            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "sent_count:%d", sent_count);

            sandesha2_sender_bean_set_sent_count(bean1, env, sent_count);
            sandesha2_sender_bean_set_time_to_send(bean1, env, 
                sandesha2_sender_bean_get_time_to_send(sender_worker_bean, env));
            sandesha2_sender_mgr_update(sender_mgr, env, bean1);
        }
    }

    if(sender_worker_bean)
    {
        sandesha2_sender_bean_free(sender_worker_bean, env);
    }

    if(successfully_sent)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Application message successfully sent");

        if(!axis2_msg_ctx_get_server_side(app_msg_ctx, env))
        {
            /*status = sandesha2_app_msg_processor_process_app_msg_response(env, app_msg_ctx, storage_mgr);
            if(AXIS2_SUCCESS != status)
            {
                return status;
            }*/
        }
    }

    if(bean1)
    {
        sandesha2_sender_bean_free(bean1, env);
    }

    /*if(rm_msg_ctx)
    {
        sandesha2_msg_ctx_free(rm_msg_ctx, env);
    }*/

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, "[sandesha2] Exit:sandesha2_app_msg_processor_resend");

    return status;
}

static axis2_status_t AXIS2_CALL
sandesha2_app_msg_processor_process_app_msg_response(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_msg_ctx_t *response_msg_ctx = NULL;
    axiom_soap_envelope_t *response_envelope = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_engine_t *engine = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    axutil_property_t *property = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    const axis2_char_t *mep = NULL;
 
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Entry:sandesha2_app_msg_processor_process_app_msg_response");

    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);

    response_envelope = axis2_msg_ctx_get_response_soap_envelope(msg_ctx, env);
    if(!response_envelope)
    {
        axis2_char_t *soap_ns_uri = NULL;

        soap_ns_uri = axis2_msg_ctx_get_is_soap_11(msg_ctx, env) ?
             AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
                AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;
        response_envelope = axis2_http_transport_utils_create_soap_msg(env, msg_ctx, soap_ns_uri);
        if(!response_envelope)
        {
            /* There is no response message context. */

            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Response envelope not found");
            return AXIS2_SUCCESS;
        }
    }

    /* create the response */
    response_msg_ctx = axis2_msg_ctx_create(env, conf_ctx, axis2_msg_ctx_get_transport_in_desc(msg_ctx, 
                env), axis2_msg_ctx_get_transport_out_desc(msg_ctx, env));

    if (!response_msg_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Could not create response message context");
        return AXIS2_FAILURE;
    }

    /* Note that we set here as client side to indicate that we are in the application client side.
     * This knowledge is importatnt within app_msg_processor_process_in_msg() function.*/
    axis2_msg_ctx_set_server_side(response_msg_ctx, env, AXIS2_FALSE);
    axis2_msg_ctx_set_op_ctx(response_msg_ctx, env, axis2_msg_ctx_get_op_ctx(msg_ctx, env));
    axis2_msg_ctx_set_conf_ctx(response_msg_ctx, env, conf_ctx);
    axis2_msg_ctx_set_svc_grp_ctx(response_msg_ctx, env, axis2_msg_ctx_get_svc_grp_ctx(msg_ctx, env));

    axis2_msg_ctx_set_status_code (response_msg_ctx, env, axis2_msg_ctx_get_status_code (msg_ctx, env));

    /* To avoid a second passing through incoming handlers at op_client */
    property = axis2_msg_ctx_get_property(msg_ctx, env, AXIS2_HANDLER_ALREADY_VISITED);
    if(property)
    {
        axutil_property_set_value(property, env, AXIS2_VALUE_TRUE);
    }
    else
    {
        property = axutil_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
        axis2_msg_ctx_set_property(msg_ctx, env, AXIS2_HANDLER_ALREADY_VISITED, property);
    }

    axis2_msg_ctx_set_soap_envelope(response_msg_ctx, env, response_envelope);
    engine = axis2_engine_create(env, conf_ctx);
    if (engine)
    {
        /* Note that this flow does not hit a message receiver because we have set the 
         * message context to be in client side. Consequently message context will not
         * be added to the operation context(which is normally done at msg_recv_receive()
         * function).
         */
        status = axis2_engine_receive(engine, env, response_msg_ctx);

        axis2_engine_free(engine, env);
    }

    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    if(op_ctx)
    {
        axis2_op_t *op = NULL;

        op = axis2_op_ctx_get_op(op_ctx, env);
        mep = axis2_op_get_msg_exchange_pattern(op, env);
    }
    
    if(!axutil_strcmp(mep, AXIS2_MEP_URI_OUT_IN))
    {
        /* Note that as explained above this message context is not added to the operation context, 
         * therefore will not be freed when operation context's msg_ctx_map is freed. So we need to 
         * free the response message here. Note that we copied this response soap envelope from the
         * outgoing message context from application client. This response envelope will be freed
         * at operation client. So to avoid double freeing we increment its ref.
         */
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Increment the soap envelope ref counter");
        axiom_soap_envelope_increment_ref(response_envelope, env);
    }
    
    axis2_msg_ctx_free(response_msg_ctx, env);

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2] Exit:sandesha2_app_msg_processor_process_app_msg_response");

    return status;
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

static axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_last_out_msg_no(
    const axutil_env_t *env,
    axis2_char_t *internal_seq_id,
    long msg_num,
    sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    sandesha2_seq_property_bean_t *last_out_msg_no_bean = NULL;
    axis2_bool_t update = AXIS2_TRUE;
    axis2_char_t str_long[32];
    
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Entry:sandesha2_app_msg_processor_set_last_out_msg_no");
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_prop_mgr, AXIS2_FAILURE);
    
    if(msg_num <= 0)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Invalid Message Number");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_MSG_NUM, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }

    last_out_msg_no_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env, 
        internal_seq_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NUMBER);

    if(!last_out_msg_no_bean)
    {
        update = AXIS2_FALSE;
        last_out_msg_no_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_seq_id(last_out_msg_no_bean, env, internal_seq_id);

        sandesha2_seq_property_bean_set_name(last_out_msg_no_bean, env,
            SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NUMBER);        
    }

    sprintf(str_long, "%ld", msg_num);
    sandesha2_seq_property_bean_set_value(last_out_msg_no_bean, env, str_long);
    if(update)
    {
        sandesha2_seq_property_mgr_update(seq_prop_mgr, env, last_out_msg_no_bean);
    }
    else
    {
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, last_out_msg_no_bean);
    }
    if(last_out_msg_no_bean)
    {
        sandesha2_seq_property_bean_free(last_out_msg_no_bean, env);
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2] Exit:sandesha2_app_msg_processor_set_last_out_msg_no");
	
    return AXIS2_SUCCESS;
}


static void AXIS2_CALL                 
sandesha2_app_msg_processor_is_last_out_msg(
    const axutil_env_t *env,
    axis2_msg_ctx_t *msg_ctx,
    axis2_char_t *rmd_sequence_id,
    axis2_char_t *internal_sequence_id,
    long msg_num,
    sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    axis2_bool_t is_svr_side = AXIS2_FALSE;
    axis2_bool_t last_msg = AXIS2_FALSE;

    is_svr_side = axis2_msg_ctx_get_server_side(msg_ctx, env);
    if(is_svr_side)
    {
        axis2_char_t *last_req_id = NULL;
        axis2_char_t *relates_to_value = NULL;
        const axis2_relates_to_t *relates_to = NULL;

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
    else
    {
        axutil_property_t *property = NULL;
        axis2_char_t *last_app_msg = NULL;

        property = axis2_msg_ctx_get_property(msg_ctx, env, SANDESHA2_CLIENT_LAST_MESSAGE);
        if(property)
        {
            last_app_msg = axutil_property_get_value(property, env);
        }

        if(last_app_msg && !axutil_strcmp(last_app_msg, AXIS2_VALUE_TRUE))
        {
            axis2_char_t *spec_ver = NULL;

            spec_ver = sandesha2_utils_get_rm_version(env, internal_sequence_id, seq_prop_mgr);
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
                spec_ver = NULL;
            }
        }
    }

    if(last_msg)
    {
        axis2_char_t msg_number_str[32];
        sandesha2_seq_property_bean_t *res_last_msg_key_bean = NULL;
       
        sprintf(msg_number_str, "%ld", msg_num); 
        res_last_msg_key_bean = sandesha2_seq_property_bean_create_with_data(env, 
                internal_sequence_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO, msg_number_str);

        if(res_last_msg_key_bean)
        {
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, res_last_msg_key_bean);
            sandesha2_seq_property_bean_free(res_last_msg_key_bean, env);
        }
    }
}

