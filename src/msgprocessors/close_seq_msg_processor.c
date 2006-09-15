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
#include <sandesha2_close_seq_msg_processor.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_msg_ctx.h>
#include <axis2_msg_ctx.h>
#include <axis2_string.h>
#include <axis2_engine.h>
#include <axiom_soap_const.h>
#include <stdio.h>
#include <sandesha2_storage_mgr.h>
#include <axis2_msg_ctx.h>
#include <axis2_conf_ctx.h>
#include <sandesha2_seq_ack.h>
#include <sandesha2_close_seq.h>
#include <axis2_uuid_gen.h>
#include <sandesha2_create_seq_bean.h>
#include <sandesha2_create_seq_mgr.h>
#include <axis2_endpoint_ref.h>
#include <axis2_op_ctx.h>
#include <sandesha2_spec_specific_consts.h>
#include <axis2_core_utils.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_msg_creator.h>

/** 
 * @brief Close Sequence Message Processor struct impl
 *	Sandesha2 Close Sequence Msg Processor
 */
typedef struct sandesha2_close_seq_msg_processor_impl 
                        sandesha2_close_seq_msg_processor_impl_t;  
  
struct sandesha2_close_seq_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(msg_proc) \
						((sandesha2_close_seq_msg_processor_impl_t *)(msg_proc))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_close_seq_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx);
    
axis2_status_t AXIS2_CALL 
sandesha2_close_seq_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx);
                    	
axis2_status_t AXIS2_CALL 
sandesha2_close_seq_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_close_seq_msg_processor_create(const axis2_env_t *env)
{
    sandesha2_close_seq_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
              
    msg_proc_impl =  (sandesha2_close_seq_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_close_seq_msg_processor_impl_t));
	
    if(NULL == msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(NULL == msg_proc_impl->msg_processor.ops)
	{
		sandesha2_close_seq_msg_processor_free((sandesha2_msg_processor_t*)
                         msg_proc_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops->process_in_msg = 
                        sandesha2_close_seq_msg_processor_process_in_msg;
    msg_proc_impl->msg_processor.ops->process_out_msg = 
    					sandesha2_close_seq_msg_processor_process_out_msg;
    msg_proc_impl->msg_processor.ops->free = sandesha2_close_seq_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


axis2_status_t AXIS2_CALL 
sandesha2_close_seq_msg_processor_free (sandesha2_msg_processor_t *msg_processor, 
						const axis2_env_t *env)
{
    sandesha2_close_seq_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(NULL != msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL 
sandesha2_close_seq_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_close_seq_t *close_seq = NULL;
    axis2_char_t *seq_id = NULL;
    sandesha2_fault_mgr_t *fault_mgr = NULL;
    sandesha2_msg_ctx_t *fault_rm_msg_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *close_seq_bean = NULL;
    axis2_msg_ctx_t *ack_msg_ctx = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg = NULL;
    axis2_char_t *rm_ns_val = NULL;
    axiom_soap_envelope_t *envelope = NULL;
    sandesha2_seq_ack_t *seq_ack  = NULL;
    axis2_msg_ctx_t *close_seq_res_msg = NULL;
    sandesha2_msg_ctx_t *close_seq_res_rm_msg = NULL;
    axis2_property_t *property = NULL;
    axis2_engine_t *engine = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    close_seq = (sandesha2_close_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_CLOSE_SEQ);
    
    seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_CLOSE_SEQ_GET_IDENTIFIER(close_seq, env), env);
    
    fault_mgr = sandesha2_fault_mgr_create(env);
    fault_rm_msg_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_UNKNOWN_SEQ(fault_mgr, env,
                        rm_msg_ctx, seq_id, storage_mgr);
    if(NULL != fault_rm_msg_ctx)
    {
        engine = axis2_engine_create(env, conf_ctx);
        AXIS2_ENGINE_SEND(engine, env, SANDESHA2_MSG_CTX_GET_MSG_CTX(
                        fault_rm_msg_ctx, env));
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
        return AXIS2_SUCCESS;
    }
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    close_seq_bean = sandesha2_seq_property_bean_create(env);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(close_seq_bean, env, seq_id);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(close_seq_bean, env, 
                        SANDESHA2_SEQ_PROP_SEQ_CLOSED);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(close_seq_bean, env, 
                        SANDESHA2_VALUE_TRUE);
    
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, close_seq_bean);
    ack_rm_msg = sandesha2_ack_mgr_generate_ack_msg(env, rm_msg_ctx, seq_id,
                        storage_mgr);
    ack_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(ack_rm_msg, env);
    rm_ns_val = SANDESHA2_MSG_CTX_GET_RM_NS_VAL(ack_rm_msg, env);
    SANDESHA2_MSG_CTX_GET_RM_NS_VAL(rm_msg_ctx, env);
    SANDESHA2_MSG_CTX_SET_RM_NS_VAL(ack_rm_msg, env, rm_ns_val);
    
    envelope = axiom_soap_envelope_create_default_soap_envelope(env,
                        sandesha2_utils_get_soap_version(env,
                        AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(msg_ctx, env)));
    AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(ack_msg_ctx, env, envelope);
    
    seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(ack_rm_msg, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    close_seq_res_msg = axis2_core_utils_create_out_msg_ctx(env, msg_ctx);
    
    close_seq_res_rm_msg = sandesha2_msg_creator_create_close_seq_res_msg(env, 
                        rm_msg_ctx, close_seq_res_msg, storage_mgr);
    
    SANDESHA2_MSG_CTX_SET_MSG_PART(close_seq_res_rm_msg, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT,
                        (sandesha2_iom_rm_part_t*)seq_ack);
    SANDESHA2_MSG_CTX_SET_FLOW(close_seq_res_rm_msg, env, AXIS2_OUT_FLOW);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    SANDESHA2_MSG_CTX_SET_PROPERTY(close_seq_res_rm_msg, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property);
    AXIS2_MSG_CTX_SET_RESPONSE_WRITTEN(close_seq_res_msg, env, AXIS2_TRUE);
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(close_seq_res_rm_msg, env);
    
    engine = axis2_engine_create(env, conf_ctx);
    AXIS2_ENGINE_SEND(engine, env, close_seq_res_msg);
    return AXIS2_SUCCESS;
}
    
axis2_status_t AXIS2_CALL 
sandesha2_close_seq_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    return AXIS2_SUCCESS;
}
