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
#include <sandesha2/sandesha2_ack_msg_processor.h>
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
#include <axiom_soap_const.h>
#include <stdio.h>
#include <sandesha2_storage_mgr.h>
#include <axis2_msg_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_core_utils.h>
#include <sandesha2/sandesha2_seq_ack.h>
#include <sandesha2/sandesha2_create_seq_res.h>
#include <axis2_uuid_gen.h>
#include <sandesha2_create_seq_bean.h>
#include <sandesha2_create_seq_mgr.h>
#include <axis2_endpoint_ref.h>
#include <axis2_op_ctx.h>
#include <sandesha2/sandesha2_spec_specific_consts.h>

/** 
 * @brief Acknowledgement Message Processor struct impl
 *	Sandesha2 Acknowledgement Msg Processor
 */
typedef struct sandesha2_ack_msg_processor_impl 
                        sandesha2_ack_msg_processor_impl_t;  
  
struct sandesha2_ack_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(msg_proc) \
						((sandesha2_ack_msg_processor_impl_t *)(msg_proc))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_ack_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx);
    
axis2_status_t AXIS2_CALL 
sandesha2_ack_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx);
    
sandesha2_sender_bean_t* AXIS2_CALL 
sandesha2_ack_msg_processor_get_retrans_entry(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_array_list_t *list,
                        long msg_no);

long AXIS2_CALL 
sandesha2_ack_msg_processor_get_no_of_msgs_acked(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_array_list_t *list);
                    	
axis2_status_t AXIS2_CALL 
sandesha2_ack_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_ack_msg_processor_create(const axis2_env_t *env)
{
    sandesha2_ack_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
              
    msg_proc_impl =  (sandesha2_ack_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_ack_msg_processor_impl_t));
	
    if(NULL == msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(NULL == msg_proc_impl->msg_processor.ops)
	{
		sandesha2_ack_msg_processor_free((sandesha2_msg_processor_t*)
                         msg_proc_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops->process_in_msg = 
                        sandesha2_ack_msg_processor_process_in_msg;
    msg_proc_impl->msg_processor.ops->process_out_msg = 
    					sandesha2_ack_msg_processor_process_out_msg;
    msg_proc_impl->msg_processor.ops->free = sandesha2_ack_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


axis2_status_t AXIS2_CALL 
sandesha2_ack_msg_processor_free (sandesha2_msg_processor_t *msg_processor, 
						const axis2_env_t *env)
{
    sandesha2_ack_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(NULL != msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL 
sandesha2_ack_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    sandesha2_seq_ack_t *seq_ack = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_array_list_t *ack_range_list = NULL;
    axis2_array_list_t *nack_list = NULL;
    axis2_char_t *out_seq_id = NULL;
    sandesha2_fault_mgr_t *fault_mgr = NULL;
    sandesha2_msg_ctx_t *fault_msg_ctx = NULL;
    axis2_char_t *int_seq_id = NULL;
    sandesha2_seq_property_bean_t *int_seq_bean = NULL;
    axis2_char_t *internal_seq_id = NULL;
    axis2_property_t *property = NULL;
    sandesha2_sender_bean_t *input_bean = NULL;
    axis2_array_list_t *retrans_list = NULL;
    axis2_array_list_t *acked_list = NULL;
    int i = 0;
    long no_of_msgs_acked = 0;
    sandesha2_seq_property_bean_t *no_of_msgs_acked_bean = NULL;
    axis2_char_t str_long[32];
    sandesha2_seq_property_bean_t *completed_bean = NULL;
    axis2_char_t *str_list = NULL;
    axis2_char_t *last_out_msg_no_str = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    if(NULL == seq_ack)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence"
                        " acknowledgement part is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;        
    }
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    SANDESHA2_SEQ_ACK_SET_MUST_UNDERSTAND(seq_ack, env, AXIS2_FALSE);
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(rm_msg_ctx, env);
    
    retrans_mgr = SANDESHA2_STORAGE_MGR_GET_RETRANS_MGR(storage_mgr, env);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    
    ack_range_list = SANDESHA2_SEQ_ACK_GET_ACK_RANGE_LIST(seq_ack, env);
    nack_list = SANDESHA2_SEQ_ACK_GET_NACK_LIST(seq_ack, env);
    out_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_ACK_GET_IDENTIFIER(seq_ack, env), env);
    if(NULL == out_seq_id || 0 == AXIS2_STRLEN(out_seq_id))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] OutSequenceId is"
                        " null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_SEQ, AXIS2_FAILURE);
        return AXIS2_FAILURE;        
    }
    fault_mgr = sandesha2_fault_mgr_create(env);
    fault_msg_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_UNKNOWN_SEQ(fault_mgr, env,
                        rm_msg_ctx, out_seq_id, storage_mgr);
    if(NULL != fault_msg_ctx)
    {
        axis2_engine_t *engine = NULL;
        engine = axis2_engine_create(env, conf_ctx);
        AXIS2_ENGINE_SEND_FAULT(engine, env, SANDESHA2_MSG_CTX_GET_MSG_CTX(
                        fault_msg_ctx, env));
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
    }
    fault_msg_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_INVALID_ACK(fault_mgr, env,
                        rm_msg_ctx, storage_mgr);
    if(NULL != fault_msg_ctx)
    {
        axis2_engine_t *engine = NULL;
        engine = axis2_engine_create(env, conf_ctx);
        AXIS2_ENGINE_SEND_FAULT(engine, env, SANDESHA2_MSG_CTX_GET_MSG_CTX(
                        fault_msg_ctx, env));
        AXIS2_MSG_CTX_SET_PAUSE(msg_ctx, env, AXIS2_TRUE);
    }
    int_seq_id = sandesha2_utils_get_seq_property(env, out_seq_id, 
                        SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID, storage_mgr);
    sandesha2_seq_manager_update_last_activated_time(env, int_seq_id, 
                        storage_mgr);
    int_seq_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        out_seq_id, SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID);
    if(NULL == int_seq_bean || NULL == SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                        int_seq_bean, env))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] TempSequenceId "
                        "is not set correctly");
        return AXIS2_FAILURE;
    }
    internal_seq_id  = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(int_seq_bean, env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, SANDESHA2_ACK_PROCSSED, property,
                        AXIS2_FALSE);
                        
    input_bean = sandesha2_sender_bean_create(env);
    SANDESHA2_SENDER_BEAN_SET_SEND(input_bean, env, AXIS2_TRUE);
    SANDESHA2_SENDER_BEAN_SET_RESEND(input_bean, env, AXIS2_TRUE);
    retrans_list = SANDESHA2_SENDER_MGR_FIND_BY_SENDER_BEAN(retrans_mgr, env, 
                        input_bean);
    
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(ack_range_list, env); i++)
    {
        sandesha2_ack_range_t *ack_range  = NULL;
        long lower = -1;
        long upper = -1;
        int j = 0;
        
        ack_range = AXIS2_ARRAY_LIST_GET(ack_range_list, env, i);
        lower = SANDESHA2_ACK_RANGE_GET_LOWER_VALUE(ack_range, env);
        upper = SANDESHA2_ACK_RANGE_GET_UPPER_VALUE(ack_range, env);
        for(j = lower; j < upper; j++)
        {
            sandesha2_sender_bean_t *retrans_bean = NULL;
            long *add_no = NULL;
            
            retrans_bean = sandesha2_ack_msg_processor_get_retrans_entry(
                        msg_processor, env, retrans_list, j);
            if(NULL != retrans_bean)
            {
                SANDESHA2_SENDER_MGR_REMOVE(retrans_mgr, env, 
                        SANDESHA2_SENDER_BEAN_GET_MSG_ID(retrans_bean, env));
                SANDESHA2_STORAGE_MGR_REMOVE_MSG_CTX(storage_mgr, env,
                        SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(
                        retrans_bean, env));
            }
            add_no = AXIS2_MALLOC(env->allocator, sizeof(long));
            *add_no = (long)j;
            AXIS2_ARRAY_LIST_ADD(acked_list, env, add_no);
        }
    }
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(nack_list, env); i++)
    {
        sandesha2_nack_t *nack = NULL;
        nack = AXIS2_ARRAY_LIST_GET(nack_list, env, i);
        /* TODO processing nacks */
    }
    no_of_msgs_acked = sandesha2_ack_msg_processor_get_no_of_msgs_acked(
                        msg_processor, env, ack_range_list);
    no_of_msgs_acked_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, 
                        env, out_seq_id, 
                        SANDESHA2_SEQ_PROP_NO_OF_OUTGOING_MSGS_ACKED);
    sprintf(str_long, "%ld", no_of_msgs_acked);
    if(NULL == no_of_msgs_acked_bean)
    {
        no_of_msgs_acked_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(no_of_msgs_acked_bean, env,
                        SANDESHA2_SEQ_PROP_NO_OF_OUTGOING_MSGS_ACKED);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(no_of_msgs_acked_bean, env,
                        out_seq_id);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(no_of_msgs_acked_bean, env, 
                        str_long);
        SANDESHA2_SEQ_PROPERY_MGR_INSERT(seq_prop_mgr, env, 
                        no_of_msgs_acked_bean); 
    }
    else
    {
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(no_of_msgs_acked_bean, env, 
                        str_long);
        SANDESHA2_SEQ_PROPERY_MGR_UPDATE(seq_prop_mgr, env, 
                        no_of_msgs_acked_bean); 
    }
    completed_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, 
                        env, int_seq_id,
                        SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES);
    if(NULL == completed_bean)
    {
        completed_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(completed_bean, env, int_seq_id);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(completed_bean, env, 
                        SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES);
        SANDESHA2_SEQ_PROPERY_MGR_INSERT(seq_prop_mgr, env, completed_bean);
    }
    str_list = sandesha2_utils_array_list_to_string(env, acked_list,
                        SANDESHA2_ARRAY_LIST_LONG);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(completed_bean, env, str_list);
    SANDESHA2_SEQ_PROPERY_MGR_UPDATE(seq_prop_mgr, env, completed_bean);
    
    last_out_msg_no_str = sandesha2_utils_get_seq_property(env, int_seq_id,
                        SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO, storage_mgr);
    if(NULL != last_out_msg_no_str)
    {
        long highest_out_msg_no = 0;
        highest_out_msg_no = atol(last_out_msg_no_str);
        if(highest_out_msg_no > 0)
        {
            axis2_bool_t completed = AXIS2_FALSE;
            completed = sandesha2_ack_mgr_verify_seq_completion(env, 
                        ack_range_list, highest_out_msg_no);
            if(AXIS2_TRUE == completed)
                sandesha2_terminate_mgr_add_terminate_seq_msg(env, rm_msg_ctx,
                        out_seq_id, int_seq_id, storage_mgr);
        }
    }
    SANDESHA2_MSG_CTX_PAUSE(rm_msg_ctx, env);
    return AXIS2_SUCCESS;
}
    
axis2_status_t AXIS2_CALL 
sandesha2_ack_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    return AXIS2_SUCCESS;
}


sandesha2_sender_bean_t* AXIS2_CALL 
sandesha2_ack_msg_processor_get_retrans_entry(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_array_list_t *list,
                        long msg_no)
{
    int i = 0;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, list, NULL);
    
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(list, env); i++)
    {
        sandesha2_sender_bean_t *bean = NULL;
        bean = AXIS2_ARRAY_LIST_GET(list, env, i);
        if(SANDESHA2_SENDER_BEAN_GET_MSG_NO(bean, env) == msg_no)
            return bean;
    }
    return NULL;    
}

long AXIS2_CALL 
sandesha2_ack_msg_processor_get_no_of_msgs_acked(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_array_list_t *list)
{
    long no_of_msgs = 0;
    int i = 0;
    
    AXIS2_ENV_CHECK(env, -1);
    AXIS2_PARAM_CHECK(env->error, list, -1);
    
    for(i = 0; i <  AXIS2_ARRAY_LIST_SIZE(list, env); i++)
    {
        sandesha2_ack_range_t *ack_range = NULL;
        long upper = 0;
        long lower = 0;
        long diff = 0;
        
        ack_range = AXIS2_ARRAY_LIST_GET(list, env, i);
        lower = SANDESHA2_ACK_RANGE_GET_LOWER_VALUE(ack_range, env);
        upper = SANDESHA2_ACK_RANGE_GET_UPPER_VALUE(ack_range, env);
        
        diff = upper - lower;
        if(diff > 0)
            no_of_msgs += diff;
    }
    return no_of_msgs;
}
