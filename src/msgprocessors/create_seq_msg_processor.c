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
#include <sandesha2/sandesha2_create_seq_msg_processor.h>
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
#include <sandesha2/sandesha2_create_seq.h>
#include <axis2_conf_ctx.h>
#include <axis2_core_utils.h>
#include <sandesha2/sandesha2_create_seq_res.h>
#include <sandesha2/sandesha2_seq_offer.h>
#include <sandesha2/sandesha2_accept.h>
#include <sandesha2/sandesha2_address.h>
#include <sandesha2/sandesha2_acks_to.h>
#include <sandesha2/sandesha2_create_seq_res.h>
#include <axis2_uuid_gen.h>
#include <sandesha2_create_seq_bean.h>
#include <sandesha2_create_seq_mgr.h>
#include <axis2_endpoint_ref.h>
#include <axis2_op_ctx.h>
#include <sandesha2/sandesha2_spec_specific_consts.h>
#include <sandesha2_msg_creator.h>
#include <sandesha2_seq_mgr.h>

/** 
 * @brief Create Sequence Message Processor struct impl
 *	Sandesha2 Create Sequence Msg Processor
 */
typedef struct sandesha2_create_seq_msg_processor_impl 
                        sandesha2_create_seq_msg_processor_impl_t;  
  
struct sandesha2_create_seq_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(msg_proc) \
						((sandesha2_create_seq_msg_processor_impl_t *)(msg_proc))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx);
    
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx);
    
axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_offer_accepted(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_char_t *seq_id,
                        sandesha2_msg_ctx_t *rm_msg_ctx,
                        sandesha2_storage_mgr_t *storage_mgr);
                    	
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_create_seq_msg_processor_create(const axis2_env_t *env)
{
    sandesha2_create_seq_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
              
    msg_proc_impl =  (sandesha2_create_seq_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_create_seq_msg_processor_impl_t));
	
    if(NULL == msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(NULL == msg_proc_impl->msg_processor.ops)
	{
		sandesha2_create_seq_msg_processor_free((sandesha2_msg_processor_t*)
                         msg_proc_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops->process_in_msg = 
                        sandesha2_create_seq_msg_processor_process_in_msg;
    msg_proc_impl->msg_processor.ops->process_out_msg = 
    					sandesha2_create_seq_msg_processor_process_out_msg;
    msg_proc_impl->msg_processor.ops->free = sandesha2_create_seq_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


axis2_status_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_free (sandesha2_msg_processor_t *msg_processor, 
						const axis2_env_t *env)
{
    sandesha2_create_seq_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(NULL != msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_fault_mgr_t *fault_mgr  = NULL;
    sandesha2_msg_ctx_t *fault_rm_msg_ctx = NULL;
    axis2_msg_ctx_t *out_msg_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_property_t *property = NULL;
    axis2_char_t *new_seq_id = NULL;
    sandesha2_msg_ctx_t *create_seq_res_msg = NULL;
    sandesha2_create_seq_res_t *create_seq_res_part = NULL;
    sandesha2_seq_offer_t *seq_offer = NULL;
    axis2_endpoint_ref_t *acks_to = NULL;
    sandesha2_seq_property_bean_t *acks_to_bean = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    axis2_engine_t *engine = NULL;
    axis2_char_t *addr_ns_uri = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_ctx_t *ctx = NULL;
     
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    create_seq_part = (sandesha2_create_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_CREATE_SEQ);
    if(NULL == create_seq_part)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]create_seq_part"
                        " is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    
    fault_mgr = sandesha2_fault_mgr_create(env);
    fault_rm_msg_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_CREATE_SEQ_REFUSED(
                        fault_mgr, env, msg_ctx, storage_mgr);
    if(NULL != fault_rm_msg_ctx)
    {
        axis2_engine_t *engine = NULL;
        
        engine = axis2_engine_create(env, conf_ctx);
        AXIS2_ENGINE_SEND_FAULT(engine, env, SANDESHA2_MSG_CTX_GET_MSG_CTX(
                        fault_rm_msg_ctx, env));
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
        return AXIS2_SUCCESS;
    }
    out_msg_ctx = axis2_core_utils_create_out_msg_ctx(env, msg_ctx);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, 
                        env);
    new_seq_id = sandesha2_seq_mgr_set_up_new_seq(env, rm_msg_ctx, 
                        storage_mgr);
    create_seq_res_msg = sandesha2_msg_creator_create_create_seq_res_msg(env,
                        rm_msg_ctx, out_msg_ctx, new_seq_id, storage_mgr);
    AXIS2_MSG_CTX_SET_FLOW(out_msg_ctx, env, AXIS2_OUT_FLOW);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(out_msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property,
                        AXIS2_FALSE);
    
    create_seq_res_part = (sandesha2_create_seq_res_t*)
                        SANDESHA2_MSG_CTX_GET_MSG_PART(create_seq_res_msg, 
                        env, SANDESHA2_MSG_PART_CREATE_SEQ_RESPONSE);
    seq_offer = SANDESHA2_CREATE_SEQ_GET_SEQ_OFFER(create_seq_part, 
                        env);
    
    if(NULL != seq_offer)
    {
        sandesha2_accept_t *accept = NULL;
        axis2_char_t *offer_seq_id = NULL;
        axis2_bool_t offer_accepted = AXIS2_FALSE;

        
        accept = SANDESHA2_CREATE_SEQ_RES_GET_ACCEPT(create_seq_res_part, env);
        if(NULL == accept)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Accept part "
                        "has not genereated for a message with offer");
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING,
                        AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        offer_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_OFFER_GET_IDENTIFIER(seq_offer, env), env);
        offer_accepted = sandesha2_create_seq_msg_processor_offer_accepted(
                        msg_processor, env, offer_seq_id, rm_msg_ctx, 
                        storage_mgr);
        if(AXIS2_TRUE == offer_accepted)
        {
            sandesha2_create_seq_bean_t *create_seq_bean = NULL;
            axis2_char_t *int_seq_id = NULL;
            sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
            sandesha2_seq_property_bean_t *out_seq_bean = NULL;
            
            create_seq_bean = sandesha2_create_seq_bean_create(env);
            SANDESHA2_CREATE_SEQ_BEAN_SET_SEQ_ID(create_seq_bean, env, 
                        offer_seq_id);
            int_seq_id = sandesha2_utils_get_outgoing_internal_seq_id(env,
                        new_seq_id);
            SANDESHA2_CREATE_SEQ_BEAN_SET_INTERNAL_SEQ_ID(create_seq_bean, env,
                        int_seq_id);
            SANDESHA2_CREATE_SEQ_BEAN_SET_CREATE_SEQ_MSG_ID(create_seq_bean, env,
                        axis2_uuid_gen(env));
            create_seq_mgr = SANDESHA2_STORAGE_MGR_GET_CREATE_SEQ_MGR(
                        storage_mgr, env);
            SANDESHA2_CREATE_SEQ_MGR_INSERT(create_seq_mgr, env, create_seq_bean);
            
            out_seq_bean = sandesha2_seq_property_bean_create(env);
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(out_seq_bean, env, 
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID);
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(out_seq_bean, env, 
                        offer_seq_id);
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(out_seq_bean, env, 
                        int_seq_id);
            SANDESHA2_SEQ_PROPERTY_BEAN_MANAGER_INSERT(seq_prop_mgr, env, 
                        out_seq_bean);
        }
        else
        {
            SANDESHA2_CREATE_SEQ_SET_ACCEPT(create_seq_res_part, env, NULL);
            SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(create_seq_res_msg, env);
        }
    }
    acks_to = SANDESHA2_ADDRESS_GET_EPR(SANDESHA2_ACKS_TO_GET_ADDRESS(
                    SANDESHA2_CREATE_SEQ_GET_ACKS_TO(create_seq_part, env), 
                    env), env);
    if(NULL == acks_to || NULL == AXIS2_ENDPOINT_REF_GET_ADDRESS(acks_to, 
                    env))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Acks to is null"
                    " in create_seq message");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_EPR, 
                    AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    acks_to_bean = sandesha2_seq_property_bean_create_with_data(env, 
                    new_seq_id, SANDESHA2_SEQ_PROP_ACKS_TO_EPR, 
                    AXIS2_ENDPOINT_REF_GET_ADDRESS(acks_to, env));
    SANDESHA2_SEQ_PROPERTY_BEAN_MGR_INSERT(seq_prop_mgr, env, acks_to_bean);
    AXIS2_MSG_CTX_SET_RESPONSE_WRITTEN(out_msg_ctx, env, AXIS2_TRUE);
    sandesha2_seq_mgr_update_last_activated_time(env, new_seq_id, 
                    storage_mgr);
    engine = axis2_engine_create(env, conf_ctx);
    AXIS2_ENGINE_SEND(engine, env, out_msg_ctx);
    to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                    new_seq_id, SANDESHA2_SEQ_PROP_TO_EPR);
    if(NULL == to_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]wsa:To is not set");
        return AXIS2_FAILURE;
    }
    to_epr = axis2_endpoint_ref_create(env, 
                    SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(to_bean, env));
    addr_ns_uri = sandesha2_utils_get_seq_property(env, new_seq_id, 
                    SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE,
                    storage_mgr);
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    
    ctx = AXIS2_OP_CTX_GET_BASE(AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env), env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    if(0 == AXIS2_STRCMP(anon_uri, AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, 
                    env)))
        AXIS2_PROPERTY_SET_VALUE(property, env, "TRUE");
    else
        AXIS2_PROPERTY_SET_VALUE(property, env, "FALSE");
    AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN, property, 
                    AXIS2_FALSE);
    
    SANDESHA2_MSG_CTX_PAUSE(rm_msg_ctx, env);
    return AXIS2_SUCCESS;
    
}
    
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    /* TODO
     * adding the SANDESHA_LISTENER
     */
    return AXIS2_SUCCESS;
}


axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_msg_processor_offer_accepted(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_char_t *seq_id,
                        sandesha2_msg_ctx_t *rm_msg_ctx,
                        sandesha2_storage_mgr_t *storage_mgr)
{
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_create_seq_bean_t *find_bean = NULL;
    axis2_array_list_t *list = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FALSE);
    
    create_seq_mgr = SANDESHA2_STORAGE_MGR_GET_CREATE_SEQ_MGR(storage_mgr, env);
    find_bean = sandesha2_create_seq_bean_create_with_data(env, NULL, NULL, 
                        seq_id);
    list = SANDESHA2_CREATE_SEQ_MGR_FIND(create_seq_mgr, env, find_bean);
    /* Single char offerings are not accepted */
    if(1 >= AXIS2_STRLEN(seq_id))
        return AXIS2_FALSE;
        
    if(NULL != list && 0 < AXIS2_ARRAY_LIST_SIZE(list, env))
        return AXIS2_FALSE;
        
    return AXIS2_TRUE;
}
