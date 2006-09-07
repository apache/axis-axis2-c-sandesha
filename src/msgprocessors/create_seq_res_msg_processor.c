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
#include <sandesha2_create_seq_res_msg_processor.h>
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
#include <sandesha2_create_seq.h>
#include <sandesha2_create_seq_res.h>
#include <axis2_conf_ctx.h>
#include <axis2_core_utils.h>
#include <sandesha2_create_seq_res.h>
#include <sandesha2_seq_offer.h>
#include <sandesha2_seq_ack.h>
#include <sandesha2_create_seq_res.h>
#include <axis2_uuid_gen.h>
#include <sandesha2_create_seq_bean.h>
#include <sandesha2_create_seq_mgr.h>
#include <axis2_endpoint_ref.h>
#include <axis2_op_ctx.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_ack_msg_processor.h>
#include <sandesha2_seq.h>
#include <sandesha2_ack_requested.h>
#include <axis2_relates_to.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_msg_init.h>


/** 
 * @brief Create Sequence Response Message Processor struct impl
 *	Sandesha2 Create Sequence Response Msg Processor
 */
typedef struct sandesha2_create_seq_res_msg_processor_impl 
                        sandesha2_create_seq_res_msg_processor_impl_t;  
  
struct sandesha2_create_seq_res_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(msg_proc) \
						((sandesha2_create_seq_res_msg_processor_impl_t *)(msg_proc))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx);

axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx);
    
axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_offer_accepted(
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env, 
    axis2_char_t *seq_id,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr);
                    
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_free (
    sandesha2_msg_processor_t *element, 
    const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_create_seq_res_msg_processor_create(
    const axis2_env_t *env)
{
    sandesha2_create_seq_res_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
              
    msg_proc_impl =  (sandesha2_create_seq_res_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_create_seq_res_msg_processor_impl_t));
	
    if(NULL == msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(NULL == msg_proc_impl->msg_processor.ops)
	{
		sandesha2_create_seq_res_msg_processor_free((sandesha2_msg_processor_t*)
                         msg_proc_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops->process_in_msg = 
                        sandesha2_create_seq_res_msg_processor_process_in_msg;
    msg_proc_impl->msg_processor.ops->process_out_msg = 
    					sandesha2_create_seq_res_msg_processor_process_out_msg;
    msg_proc_impl->msg_processor.ops->free = 
                        sandesha2_create_seq_res_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
    const axis2_env_t *env)
{
    sandesha2_create_seq_res_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(NULL != msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_ack_t *seq_ack = NULL;
    sandesha2_create_seq_res_t *csr_part = NULL;
    axis2_char_t *new_out_seq_id = NULL;
    axis2_relates_to_t *relates_to = NULL;
    axis2_char_t *create_seq_msg_id = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_create_seq_bean_t *create_seq_bean = NULL;
    axis2_char_t *int_seq_id = NULL;
    sandesha2_sender_bean_t *create_seq_sender_bean = NULL;
    axis2_char_t *create_seq_storage_key = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *out_seq_bean = NULL;
    sandesha2_seq_property_bean_t *int_seq_bean = NULL;
    sandesha2_accept_t *accept = NULL;
    sandesha2_sender_bean_t *target_bean = NULL;
    axis2_array_list_t *found_list = NULL;
    int i = 0, size = 0;
    axis2_ctx_t *ctx = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
                        
    seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    if(NULL != seq_ack)
    {
        sandesha2_msg_processor_t *ack_processor = NULL;
        ack_processor = sandesha2_ack_msg_processor_create(env);
        SANDESHA2_MSG_PROCESSOR_PROCESS_IN_MSG(ack_processor, env, rm_msg_ctx);
    }
    csr_part = (sandesha2_create_seq_res_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_CREATE_SEQ_RESPONSE);
    if(NULL == csr_part)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Create Sequence"
                        " Response part is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    new_out_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_CREATE_SEQ_RES_GET_IDENTIFIER(csr_part, env),
                        env);
    if(NULL == new_out_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] new sequence id"
                        " is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    relates_to = AXIS2_MSG_CTX_GET_RELATES_TO(msg_ctx, env);
    if(NULL == relates_to)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] invalid create"
                        " sequence message. relates_to part is not available");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_RELATES_TO,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    create_seq_msg_id = AXIS2_RELATES_TO_GET_VALUE(relates_to, env);
    retrans_mgr = SANDESHA2_STORAGE_MGR_GET_RETRANS_MGR(storage_mgr, env);
    create_seq_mgr = SANDESHA2_STORAGE_MGR_GET_CREATE_SEQ_MGR(storage_mgr, env);
    create_seq_bean = SANDESHA2_CREATE_SEQ_MGR_RETRIEVE(create_seq_mgr, env,
                        create_seq_msg_id);
    if(NULL == create_seq_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Create Sequence "
                        "entry is not found");
        return AXIS2_FAILURE;
    }
    int_seq_id = SANDESHA2_CREATE_SEQ_BEAN_GET_INTERNAL_SEQ_ID(create_seq_bean,
                        env);
    if(NULL == int_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] temp_sequence_id"
                        " has is not set");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    SANDESHA2_CREATE_SEQ_BEAN_SET_SEQ_ID(create_seq_bean, env, new_out_seq_id);
    SANDESHA2_CREATE_SEQ_MGR_UPDATE(create_seq_mgr, env, create_seq_bean);
    
    create_seq_sender_bean = SANDESHA2_SENDER_MGR_RETRIEVE(retrans_mgr,
                        env, create_seq_msg_id);
    if(NULL == create_seq_sender_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Create sequence"
                        " entry is not found");
        return AXIS2_FAILURE;
    }
    create_seq_storage_key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(
                        create_seq_sender_bean, env);
    SANDESHA2_STORAGE_MGR_REMOVE_MSG_CTX(storage_mgr, env, 
                        create_seq_storage_key);
    SANDESHA2_SENDER_MGR_REMOVE(retrans_mgr, env, create_seq_msg_id);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    
    out_seq_bean = sandesha2_seq_property_bean_create_with_data(env, int_seq_id,
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, new_out_seq_id);
    
    int_seq_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        new_out_seq_id, SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID,
                        int_seq_id);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, out_seq_bean);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, int_seq_bean);
    
    accept = SANDESHA2_CREATE_SEQ_RES_GET_ACCEPT(csr_part, env);
    if(NULL != accept)
    {
        sandesha2_seq_property_bean_t *offerd_seq_bean = NULL;
        axis2_char_t *offered_seq_id = NULL;
        axis2_endpoint_ref_t *acks_to_epr = NULL;
        sandesha2_seq_property_bean_t *acks_to_bean = NULL;
        sandesha2_next_msg_bean_t *next_bean = NULL;
        sandesha2_next_msg_mgr_t *next_bean_mgr = NULL;
        sandesha2_seq_property_bean_t *spec_ver_bean = NULL;
        sandesha2_seq_property_bean_t *rcvd_msg_bean = NULL;
        sandesha2_seq_property_bean_t *msgs_bean = NULL;
        sandesha2_seq_property_bean_t *addr_ver_bean = NULL;
        axis2_char_t *rm_spec_ver = NULL;
        axis2_char_t *addr_ns_val = NULL;
        
        offerd_seq_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env,
                        int_seq_id, SANDESHA2_SEQ_PROP_OFFERED_SEQ);
        if(NULL == offerd_seq_bean)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] No offered"
                        " sequence entry. But an accept was received");
            return AXIS2_FAILURE;
        }
        offered_seq_id = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(offerd_seq_bean,
                        env);
        acks_to_epr = SANDESHA2_ADDRESS_GET_EPR(SANDESHA2_ACKS_TO_GET_ADDRESS(
                        SANDESHA2_ACCEPT_GET_ACKS_TO(accept, env), env), env);
        acks_to_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(acks_to_bean, env, 
                        SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(acks_to_bean, env, 
                        offered_seq_id);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(acks_to_bean, env, 
                        (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(
                        acks_to_epr, env));
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, acks_to_bean);
        
        next_bean = sandesha2_next_msg_bean_create(env);
        SANDESHA2_NEXT_MSG_BEAN_SET_SEQ_ID(next_bean, env, offered_seq_id);
        SANDESHA2_NEXT_MSG_BEAN_SET_NEXT_MSG_NO_TO_PROCESS(next_bean, env, 1);
        next_bean_mgr = SANDESHA2_STORAGE_MGR_GET_NEXT_MSG_MGR(storage_mgr,
                        env);
        SANDESHA2_NEXT_MSG_BEAN_MGR_INSERT(next_bean_mgr, env, next_bean);
        rm_spec_ver = SANDESHA2_MSG_CTX_GET_RM_SPEC_VER(rm_msg_ctx, env);
        
        spec_ver_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        offered_seq_id, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION,
                        rm_spec_ver);
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, spec_ver_bean);
        
        rcvd_msg_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        offered_seq_id, 
                        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES, "");
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, rcvd_msg_bean);
        
        msgs_bean = sandesha2_seq_property_bean_create_with_data(env,
                        offered_seq_id, 
                        SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES, "");
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, msgs_bean);
        
        addr_ns_val = SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(rm_msg_ctx, env);
        addr_ver_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        offered_seq_id, 
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE,
                        addr_ns_val);
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, addr_ver_bean);
    }
    target_bean = sandesha2_sender_bean_create(env);
    SANDESHA2_SENDER_BEAN_SET_INTERNAL_SEQ_ID(target_bean, env, int_seq_id);
    SANDESHA2_SENDER_BEAN_SET_SEND(target_bean, env, AXIS2_FALSE);
    SANDESHA2_SENDER_BEAN_SET_RESEND(target_bean, env, AXIS2_TRUE);
    
    found_list = SANDESHA2_SENDER_MGR_FIND_BY_SENDER_BEAN(retrans_mgr, env, 
                        target_bean);
    if(found_list)
        size = AXIS2_ARRAY_LIST_SIZE(found_list, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_sender_bean_t *tmp_bean = NULL;
        axis2_char_t *key = NULL;
        axis2_msg_ctx_t *app_msg_ctx = NULL;
        axis2_char_t *rm_ver = NULL;
        axis2_char_t *assumed_rm_ns = NULL;
        sandesha2_msg_ctx_t *app_rm_msg = NULL;
        sandesha2_seq_t *seq_part = NULL;
        sandesha2_identifier_t *ident = NULL;
        sandesha2_ack_requested_t *ack_req_part = NULL;
        
        tmp_bean = AXIS2_ARRAY_LIST_GET(found_list, env, i);
        key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(tmp_bean, env);
        app_msg_ctx = SANDESHA2_STORAGE_MGR_RETRIEVE_MSG_CTX(storage_mgr, env,
                        key, conf_ctx);
        if(NULL == app_msg_ctx)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Unavailable"
                        " application message");
            return AXIS2_FAILURE;
        }
        rm_ver = sandesha2_utils_get_rm_version(env, int_seq_id, storage_mgr);
        if(NULL == rm_ver)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Cant find the"
                        " rm_ver of the given message");
            return AXIS2_FAILURE;
        }
        assumed_rm_ns = sandesha2_spec_specific_consts_get_rm_ns_val(env,
                        rm_ver);
        app_rm_msg = sandesha2_msg_init_init_msg(env, app_msg_ctx);
        seq_part = (sandesha2_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        app_rm_msg, env, SANDESHA2_MSG_PART_SEQ);
        if(NULL == seq_part)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence part"
                        " is null");
            return AXIS2_FAILURE;
        }
        ident = sandesha2_identifier_create(env, assumed_rm_ns);
        SANDESHA2_IDENTIFIER_SET_IDENTIFIER(ident, env, new_out_seq_id);
        SANDESHA2_SEQ_SET_IDENTIFIER(seq_part, env, ident);
        
        ack_req_part = (sandesha2_ack_requested_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        app_rm_msg, env, SANDESHA2_MSG_PART_ACK_REQUEST);
        if(NULL != ack_req_part)
        {
            sandesha2_identifier_t *ident1 = NULL;
            ident1 = sandesha2_identifier_create(env, assumed_rm_ns);
            SANDESHA2_IDENTIFIER_SET_IDENTIFIER(ident1, env, new_out_seq_id);
            SANDESHA2_ACK_REQUESTED_SET_IDENTIFIER(ack_req_part, env, ident1);
        }
        SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(app_rm_msg, env);
        SANDESHA2_SENDER_BEAN_SET_SEND(tmp_bean, env, AXIS2_TRUE);
        SANDESHA2_SENDER_MGR_UPDATE(retrans_mgr, env, tmp_bean);
        SANDESHA2_STORAGE_MGR_UPDATE_MSG_CTX(storage_mgr, env, key, app_msg_ctx);
    }
    sandesha2_seq_mgr_update_last_activated_time(env, int_seq_id, storage_mgr);
    ctx = AXIS2_OP_CTX_GET_BASE(AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env), env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP("TRUE", env));
    AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN, property, 
                        AXIS2_FALSE);
    SANDESHA2_MSG_CTX_SET_PAUSED(rm_msg_ctx, env, AXIS2_TRUE);
    return AXIS2_SUCCESS;
    
}
    
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_out_msg(
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
