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
#include <sandesha2_seq_mgr.h>


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
static axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx);

static axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx);
/*    
static axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_offer_accepted(
    const axis2_env_t *env, 
    axis2_char_t *seq_id,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr);
*/                  
static axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
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
	
    if(!msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(!msg_proc_impl->msg_processor.ops)
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


static axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
    const axis2_env_t *env)
{
    sandesha2_create_seq_res_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


static axis2_status_t AXIS2_CALL 
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
    const axis2_char_t *create_seq_msg_id = NULL;
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
    AXIS2_LOG_INFO(env->log, 
            "[sandesha2] sandesha2_create_seq_res_msg_processor_process_in_msg .........");
    printf("sandesha2_create_seq_res_msg_processor_process_in_msg\n");
    
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
                        
    seq_ack = (sandesha2_seq_ack_t*)sandesha2_msg_ctx_get_msg_part(rm_msg_ctx, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    if(seq_ack)
    {
        sandesha2_msg_processor_t *ack_processor = NULL;
        ack_processor = sandesha2_ack_msg_processor_create(env);
        sandesha2_msg_processor_process_in_msg(ack_processor, env, rm_msg_ctx);
    }
    csr_part = (sandesha2_create_seq_res_t*)sandesha2_msg_ctx_get_msg_part(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_CREATE_SEQ_RESPONSE);
    if(!csr_part)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Create Sequence"
                        " Response part is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    new_out_seq_id = sandesha2_identifier_get_identifier(
                        sandesha2_create_seq_res_get_identifier(csr_part, env),
                        env);
    if(!new_out_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] new sequence id"
                        " is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    relates_to = AXIS2_MSG_CTX_GET_RELATES_TO(msg_ctx, env);
    if(!relates_to)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] invalid create"
                        " sequence message. relates_to part is not available");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_RELATES_TO,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    create_seq_msg_id = AXIS2_RELATES_TO_GET_VALUE(relates_to, env);
    retrans_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
    create_seq_mgr = sandesha2_storage_mgr_get_create_seq_mgr(storage_mgr, env);
    create_seq_bean = SANDESHA2_CREATE_SEQ_MGR_RETRIEVE(create_seq_mgr, env,
                        create_seq_msg_id);
    if(!create_seq_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Create Sequence "
                        "entry is not found");
        return AXIS2_FAILURE;
    }
    int_seq_id = sandesha2_create_seq_bean_get_internal_seq_id(create_seq_bean,
                        env);
    if(!int_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] temp_sequence_id"
                        " has is not set");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    sandesha2_create_seq_bean_set_seq_id(create_seq_bean, env, new_out_seq_id);
    SANDESHA2_CREATE_SEQ_MGR_UPDATE(create_seq_mgr, env, create_seq_bean);
    
    create_seq_sender_bean = sandesha2_sender_mgr_retrieve(retrans_mgr,
                        env, create_seq_msg_id);
    if(!create_seq_sender_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Create sequence"
                        " entry is not found");
        return AXIS2_FAILURE;
    }
    create_seq_storage_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
                        create_seq_sender_bean, env);
    sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
                        create_seq_storage_key);
    sandesha2_sender_mgr_remove(retrans_mgr, env, create_seq_msg_id);
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_mgr, env);
    
    out_seq_bean = sandesha2_seq_property_bean_create_with_data(env, int_seq_id,
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, new_out_seq_id);
    
    int_seq_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        new_out_seq_id, SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID,
                        int_seq_id);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, out_seq_bean);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, int_seq_bean);
    
    accept = sandesha2_create_seq_res_get_accept(csr_part, env);
    if(accept)
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
        
        offerd_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env,
                        int_seq_id, SANDESHA2_SEQ_PROP_OFFERED_SEQ);
        if(!offerd_seq_bean)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] No offered"
                        " sequence entry. But an accept was received");
            return AXIS2_FAILURE;
        }
        offered_seq_id = sandesha2_seq_property_bean_get_value(offerd_seq_bean,
                        env);
        acks_to_epr = SANDESHA2_ADDRESS_GET_EPR(sandesha2_acks_to_get_address(
                        sandesha2_accept_get_acks_to(accept, env), env), env);
        acks_to_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_name(acks_to_bean, env, 
                        SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
        sandesha2_seq_property_bean_set_seq_id(acks_to_bean, env, 
                        offered_seq_id);
        sandesha2_seq_property_bean_set_value(acks_to_bean, env, 
                        (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(
                        acks_to_epr, env));
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, acks_to_bean);
        
        next_bean = sandesha2_next_msg_bean_create(env);
        sandesha2_next_msg_bean_set_seq_id(next_bean, env, offered_seq_id);
        sandesha2_next_msg_bean_set_next_msg_no_to_process(next_bean, env, 1);
        next_bean_mgr = sandesha2_storage_mgr_get_next_msg_mgr(storage_mgr,
                        env);
        sandesha2_next_msg_mgr_insert(next_bean_mgr, env, next_bean);
        rm_spec_ver = sandesha2_msg_ctx_get_rm_spec_ver(rm_msg_ctx, env);
        
        spec_ver_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        offered_seq_id, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION,
                        rm_spec_ver);
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, spec_ver_bean);
        
        rcvd_msg_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        offered_seq_id, 
                        SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES, "");
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, rcvd_msg_bean);
        
        msgs_bean = sandesha2_seq_property_bean_create_with_data(env,
                        offered_seq_id, 
                        SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES, "");
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, msgs_bean);
        
        addr_ns_val = sandesha2_msg_ctx_get_addr_ns_val(rm_msg_ctx, env);
        addr_ver_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        offered_seq_id, 
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE,
                        addr_ns_val);
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, addr_ver_bean);
    }
    target_bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_internal_seq_id(target_bean, env, int_seq_id);
    sandesha2_sender_bean_set_send(target_bean, env, AXIS2_FALSE);
    sandesha2_sender_bean_set_resend(target_bean, env, AXIS2_TRUE);
    
    found_list = sandesha2_sender_mgr_find_by_sender_bean(retrans_mgr, env, 
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
        key = sandesha2_sender_bean_get_msg_ctx_ref_key(tmp_bean, env);
        app_msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env,
                        key, conf_ctx);
        if(!app_msg_ctx)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Unavailable"
                        " application message");
            return AXIS2_FAILURE;
        }
        rm_ver = sandesha2_utils_get_rm_version(env, int_seq_id, storage_mgr);
        if(!rm_ver)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Cant find the"
                        " rm_ver of the given message");
            return AXIS2_FAILURE;
        }
        assumed_rm_ns = sandesha2_spec_specific_consts_get_rm_ns_val(env,
                        rm_ver);
        app_rm_msg = sandesha2_msg_init_init_msg(env, app_msg_ctx);
        seq_part = (sandesha2_seq_t*)sandesha2_msg_ctx_get_msg_part(
                        app_rm_msg, env, SANDESHA2_MSG_PART_SEQ);
        if(!seq_part)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Sequence part"
                        " is null");
            return AXIS2_FAILURE;
        }
        ident = sandesha2_identifier_create(env, assumed_rm_ns);
        sandesha2_identifier_set_identifier(ident, env, new_out_seq_id);
        sandesha2_seq_set_identifier(seq_part, env, ident);
        
        ack_req_part = (sandesha2_ack_requested_t*)sandesha2_msg_ctx_get_msg_part(
                        app_rm_msg, env, SANDESHA2_MSG_PART_ACK_REQUEST);
        if(ack_req_part)
        {
            sandesha2_identifier_t *ident1 = NULL;
            ident1 = sandesha2_identifier_create(env, assumed_rm_ns);
            sandesha2_identifier_set_identifier(ident1, env, new_out_seq_id);
            sandesha2_ack_requested_set_identifier(ack_req_part, env, ident1);
        }
        sandesha2_msg_ctx_add_soap_envelope(app_rm_msg, env);
        sandesha2_sender_bean_set_send(tmp_bean, env, AXIS2_TRUE);
        sandesha2_sender_mgr_update(retrans_mgr, env, tmp_bean);
        sandesha2_storage_mgr_update_msg_ctx(storage_mgr, env, key, app_msg_ctx);
    }
    sandesha2_seq_mgr_update_last_activated_time(env, int_seq_id, storage_mgr);
    ctx = AXIS2_OP_CTX_GET_BASE(AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env), env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP("TRUE", env));
    AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN, property, 
                        AXIS2_FALSE);
    sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
    AXIS2_LOG_INFO(env->log, 
            "[sandesha2] Exit: sandesha2_create_seq_res_msg_processor_process_in_msg");
    printf("[sandesha2] Exit: sandesha2_create_seq_res_msg_processor_process_in_msg\n");
    return AXIS2_SUCCESS;
    
}
    
static axis2_status_t AXIS2_CALL 
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

