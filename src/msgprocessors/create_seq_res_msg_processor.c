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
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_permanent_seq_property_mgr.h>
#include <sandesha2_permanent_create_seq_mgr.h>
#include <sandesha2_permanent_sender_mgr.h>
#include <sandesha2_permanent_next_msg_mgr.h>
#include <sandesha2_sender_bean.h>
#include <axis2_msg_ctx.h>
#include <axutil_string.h>
#include <axis2_engine.h>
#include <axiom_soap_const.h>
#include <stdio.h>
#include <axis2_msg_ctx.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_create_seq_res.h>
#include <axis2_conf_ctx.h>
#include <axis2_core_utils.h>
#include <sandesha2_create_seq_res.h>
#include <sandesha2_seq_offer.h>
#include <sandesha2_seq_ack.h>
#include <sandesha2_create_seq_res.h>
#include <axutil_uuid_gen.h>
#include <sandesha2_create_seq_bean.h>
#include <axis2_endpoint_ref.h>
#include <axis2_op_ctx.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_ack_msg_processor.h>
#include <sandesha2_seq.h>
#include <sandesha2_ack_requested.h>
#include <axis2_relates_to.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_seq_mgr.h>
#include <sandesha2_msg_creator.h>


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
    const axutil_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx);

static axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axutil_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx);
/*    
static axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_offer_accepted(
    const axutil_env_t *env, 
    axis2_char_t *seq_id,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr);
*/                  
static axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
    const axutil_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_create_seq_res_msg_processor_create(
    const axutil_env_t *env)
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
    const axutil_env_t *env)
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
    const axutil_env_t *env,
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
    sandesha2_sender_mgr_t *sender_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_create_seq_bean_t *create_seq_bean = NULL;
    axis2_char_t *internal_seq_id = NULL;
    sandesha2_sender_bean_t *create_seq_sender_bean = NULL;
    axis2_char_t *create_seq_storage_key = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *out_seq_bean = NULL;
    sandesha2_seq_property_bean_t *int_seq_bean = NULL;
    sandesha2_accept_t *accept = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_bool_t polling_mode = AXIS2_FALSE;
    axis2_char_t *dbname = NULL;
    
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Entry:sandesha2_create_seq_res_msg_processor_process_in_msg");
   
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    
    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, dbname);
                        
    seq_ack = sandesha2_msg_ctx_get_seq_ack(rm_msg_ctx, env);
    if(seq_ack)
    {
        sandesha2_msg_processor_t *ack_processor = NULL;
        ack_processor = sandesha2_ack_msg_processor_create(env);
        sandesha2_msg_processor_process_in_msg(ack_processor, env, rm_msg_ctx);
    }
    csr_part = sandesha2_msg_ctx_get_create_seq_res(rm_msg_ctx, env);
    if(!csr_part)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Create Sequence Response part is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING,
            AXIS2_FAILURE);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    new_out_seq_id = sandesha2_identifier_get_identifier(
                        sandesha2_create_seq_res_get_identifier(csr_part, env),
                        env);
    if(!new_out_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]New sequence id is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
            AXIS2_FAILURE);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    relates_to = axis2_msg_ctx_get_relates_to(msg_ctx, env);
    if(!relates_to)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Invalid create"\
            " sequence message. relates_to part is not available");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_RELATES_TO,
            AXIS2_FAILURE);
        if(storage_mgr)
            sandesha2_storage_mgr_free(storage_mgr, env);
        return AXIS2_FAILURE;
    }
    create_seq_msg_id = (axis2_char_t *) axis2_relates_to_get_value(relates_to, env);
    seq_prop_mgr = sandesha2_permanent_seq_property_mgr_create(env, dbname);
    create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(env, dbname);
    sender_mgr = sandesha2_permanent_sender_mgr_create(env, dbname);
    create_seq_bean = sandesha2_create_seq_mgr_retrieve(create_seq_mgr, env,
        create_seq_msg_id);
    if(!create_seq_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Create Sequence entry not found");
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
    internal_seq_id = sandesha2_create_seq_bean_get_internal_seq_id(create_seq_bean,
        env);
    if(!internal_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Internal sequence id is not set");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
            AXIS2_FAILURE);
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
    sandesha2_create_seq_bean_set_seq_id(create_seq_bean, env, new_out_seq_id);
    sandesha2_create_seq_mgr_update(create_seq_mgr, env, create_seq_bean);
    
    create_seq_sender_bean = sandesha2_sender_mgr_retrieve(sender_mgr,
                        env, create_seq_msg_id);
    if(!create_seq_sender_bean)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Create sequence entry not found");
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
    create_seq_storage_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
                        create_seq_sender_bean, env);
    sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
        create_seq_storage_key, conf_ctx, SANDESHA2_MSG_TYPE_CREATE_SEQ);
    if(create_seq_sender_bean)
        sandesha2_sender_bean_free(create_seq_sender_bean, env);
    sandesha2_sender_mgr_remove(sender_mgr, env, create_seq_msg_id);
    
    out_seq_bean = sandesha2_seq_property_bean_create_with_data(env, internal_seq_id,
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, new_out_seq_id);
    
    int_seq_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        new_out_seq_id, SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID,
                        internal_seq_id);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, out_seq_bean);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, int_seq_bean);
    accept = sandesha2_create_seq_res_get_accept(csr_part, env);
    if(accept)
    {
        sandesha2_seq_property_bean_t *special_int_seq_bean = NULL;
        sandesha2_seq_property_bean_t *offerd_seq_bean = NULL;
        axis2_char_t *offered_seq_id = NULL;
        axis2_endpoint_ref_t *acks_to_epr = NULL;
        sandesha2_seq_property_bean_t *acks_to_bean = NULL;
        sandesha2_next_msg_bean_t *next_bean = NULL;
        sandesha2_next_msg_mgr_t *next_msg_mgr = NULL;
        sandesha2_seq_property_bean_t *spec_ver_bean = NULL;
        sandesha2_seq_property_bean_t *rcvd_msg_bean = NULL;
        sandesha2_seq_property_bean_t *msgs_bean = NULL;
        sandesha2_seq_property_bean_t *addr_ver_bean = NULL;
        axis2_char_t *rm_spec_ver = NULL;
        axis2_char_t *addr_ns_val = NULL;
        axis2_char_t *new_msg_store_key = NULL;
        
        next_msg_mgr = sandesha2_permanent_next_msg_mgr_create(env, dbname);
        offerd_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env,
                        internal_seq_id, SANDESHA2_SEQ_PROP_OFFERED_SEQ);
        if(!offerd_seq_bean)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[sandesha2]No offered sequence entry. But an accept was received");
            if(seq_prop_mgr)
                sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
            if(create_seq_mgr)
                sandesha2_create_seq_mgr_free(create_seq_mgr, env);
            if(sender_mgr)
                sandesha2_sender_mgr_free(sender_mgr, env);
            if(next_msg_mgr)
                sandesha2_next_msg_mgr_free(next_msg_mgr, env);
            if(storage_mgr)
                sandesha2_storage_mgr_free(storage_mgr, env);
            return AXIS2_FAILURE;
        }
        offered_seq_id = sandesha2_seq_property_bean_get_value(offerd_seq_bean,
                        env);
        special_int_seq_bean = sandesha2_seq_property_bean_create_with_data(env, 
            offered_seq_id, SANDESHA2_SEQ_PROP_SPECIAL_INTERNAL_SEQ_ID, internal_seq_id);
        if(special_int_seq_bean)
        {
            sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                special_int_seq_bean);
            sandesha2_seq_property_bean_free(special_int_seq_bean, env);
        }
        acks_to_epr = sandesha2_address_get_epr(sandesha2_acks_to_get_address(
                        sandesha2_accept_get_acks_to(accept, env), env), env);
        acks_to_bean = sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_name(acks_to_bean, env, 
                        SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
        sandesha2_seq_property_bean_set_seq_id(acks_to_bean, env, 
                        offered_seq_id);
        if (acks_to_epr)
        {
            sandesha2_seq_property_bean_set_value(acks_to_bean, env, 
                        (axis2_char_t*)axis2_endpoint_ref_get_address(
                        acks_to_epr, env));
        }
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, acks_to_bean);
        
        next_bean = sandesha2_next_msg_bean_create(env);
        sandesha2_next_msg_bean_set_seq_id(next_bean, env, offered_seq_id);
        sandesha2_next_msg_bean_set_internal_seq_id(next_bean, env, internal_seq_id);
        sandesha2_next_msg_bean_set_next_msg_no_to_process(next_bean, env, 1);
        
        rm_spec_ver = sandesha2_msg_ctx_get_rm_spec_ver(rm_msg_ctx, env);
        if(0 == axutil_strcmp(SANDESHA2_SPEC_VERSION_1_1, rm_spec_ver))
        {
            axis2_char_t *reply_to_addr = sandesha2_utils_get_seq_property(env, 
                internal_seq_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR, seq_prop_mgr); 
            if(reply_to_addr)
            {
                polling_mode = sandesha2_utils_is_anon_uri(env, reply_to_addr);
                if(reply_to_addr)
                    AXIS2_FREE(env->allocator, reply_to_addr);
            }
        }
        {
            sandesha2_msg_ctx_t *create_seq_rm_msg = NULL;
            axis2_msg_ctx_t *create_seq_msg = NULL;
            axis2_char_t *acks_to = NULL;
            axis2_endpoint_ref_t *to = NULL;
            sandesha2_seq_property_bean_t *to_seq_bean = NULL;
            acks_to = (axis2_char_t *) axis2_endpoint_ref_get_address(acks_to_epr, env);
            create_seq_rm_msg = sandesha2_msg_creator_create_create_seq_msg(env,
                rm_msg_ctx, internal_seq_id, acks_to, seq_prop_mgr);
            if(!create_seq_rm_msg)
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                    "[sandesha2]create_seq_rm_msg is NULL");
                if(seq_prop_mgr)
                    sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
                if(create_seq_mgr)
                    sandesha2_create_seq_mgr_free(create_seq_mgr, env);
                if(sender_mgr)
                    sandesha2_sender_mgr_free(sender_mgr, env);
                if(next_msg_mgr)
                    sandesha2_next_msg_mgr_free(next_msg_mgr, env);
                if(storage_mgr)
                    sandesha2_storage_mgr_free(storage_mgr, env);
                if(create_seq_rm_msg)
                    sandesha2_msg_ctx_free(create_seq_rm_msg, env);
                return AXIS2_FAILURE;

            }
            sandesha2_msg_ctx_set_flow(create_seq_rm_msg, env, 
                SANDESHA2_MSG_CTX_OUT_FLOW);
            create_seq_msg = sandesha2_msg_ctx_get_msg_ctx(create_seq_rm_msg, env);
            to_seq_bean = sandesha2_seq_property_mgr_retrieve(seq_prop_mgr, env,
                internal_seq_id, SANDESHA2_SEQ_PROP_TO_EPR);
            if(to_seq_bean)
            {
                axis2_char_t *to_addr = sandesha2_seq_property_bean_get_value(
                    to_seq_bean, env);
                to = axis2_endpoint_ref_create(env, to_addr); 
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]to:%s", 
                    to_addr);
            }
            else
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                    "[sandesha2]to_seq_bean is NULL");
                if(seq_prop_mgr)
                    sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
                if(create_seq_mgr)
                    sandesha2_create_seq_mgr_free(create_seq_mgr, env);
                if(sender_mgr)
                    sandesha2_sender_mgr_free(sender_mgr, env);
                if(next_msg_mgr)
                    sandesha2_next_msg_mgr_free(next_msg_mgr, env);
                if(storage_mgr)
                    sandesha2_storage_mgr_free(storage_mgr, env);
                if(create_seq_rm_msg)
                    sandesha2_msg_ctx_free(create_seq_rm_msg, env);
                return AXIS2_FAILURE;
            }
            axis2_msg_ctx_set_to(create_seq_msg, env, to);
            axis2_msg_ctx_set_relates_to(create_seq_msg, env, NULL);
            new_msg_store_key = axutil_uuid_gen(env);
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Storing msg_ctx with msg_id:%s", 
                axis2_msg_ctx_get_msg_id(create_seq_msg, env));
            sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, new_msg_store_key, 
                create_seq_msg);
            if(create_seq_rm_msg)
                sandesha2_msg_ctx_free(create_seq_rm_msg, env);
        }
        sandesha2_next_msg_bean_set_ref_msg_key(next_bean, env, 
            new_msg_store_key);
        sandesha2_next_msg_bean_set_polling_mode(next_bean, env, polling_mode);
        /* If polling_mode is true, starting the polling manager */
        if(polling_mode)
        {
            sandesha2_utils_start_polling_mgr(env, conf_ctx, internal_seq_id);
        }
        sandesha2_next_msg_mgr_insert(next_msg_mgr, env, next_bean);
        
        spec_ver_bean = sandesha2_seq_property_bean_create_with_data(env, 
            offered_seq_id, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION, rm_spec_ver);
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
        if(next_msg_mgr)
            sandesha2_next_msg_mgr_free(next_msg_mgr, env);
    }
    sandesha2_seq_mgr_update_last_activated_time(env, internal_seq_id, seq_prop_mgr);
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    axis2_op_ctx_set_response_written(op_ctx, env, AXIS2_TRUE);
    sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
    if(seq_prop_mgr)
        sandesha2_seq_property_mgr_free(seq_prop_mgr, env);
    if(create_seq_mgr)
        sandesha2_create_seq_mgr_free(create_seq_mgr, env);
    if(sender_mgr)
        sandesha2_sender_mgr_free(sender_mgr, env);
    if(storage_mgr)
        sandesha2_storage_mgr_free(storage_mgr, env);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,  
        "[sandesha2]Exit:sandesha2_create_seq_res_msg_processor_process_in_msg");
    return AXIS2_SUCCESS;
    
}
    
static axis2_status_t AXIS2_CALL 
sandesha2_create_seq_res_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axutil_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    /* TODO
     * adding the SANDESHA_LISTENER
     */
    return AXIS2_SUCCESS;
}

