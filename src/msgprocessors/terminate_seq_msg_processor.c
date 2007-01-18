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
#include <sandesha2_terminate_seq_msg_processor.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_sender_bean.h>
#include <sandesha2_sender_mgr.h>
#include <axis2_msg_ctx.h>
#include <axis2_string.h>
#include <axis2_engine.h>
#include <axiom_soap_const.h>
#include <stdio.h>
#include <sandesha2_storage_mgr.h>
#include <axis2_msg_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_core_utils.h>
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
#include <sandesha2_client_constants.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_msg_creator.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_seq_mgr.h>
#include <axis2_transport_out_desc.h>
/** 
 * @brief Terminate Sequence Message Processor struct impl
 *	Sandesha2 Terminate Sequence Msg Processor
 */
typedef struct sandesha2_terminate_seq_msg_processor_impl 
                        sandesha2_terminate_seq_msg_processor_impl_t;  
  
struct sandesha2_terminate_seq_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(msg_proc) \
						((sandesha2_terminate_seq_msg_processor_impl_t *)\
                        (msg_proc))

/***************************** Function headers *******************************/
static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx);

static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx);
    
static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_setup_highest_msg_nums(
    const axis2_env_t *env, 
    axis2_conf_ctx_t *conf_ctx,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_char_t *seq_id,
    sandesha2_msg_ctx_t *rm_msg_ctx);

static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_add_terminate_seq_res(
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *seq_id,
    sandesha2_storage_mgr_t *storage_mgr);
                    
static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
	const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_terminate_seq_msg_processor_create(
    const axis2_env_t *env)
{
    sandesha2_terminate_seq_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
              
    msg_proc_impl =  ( sandesha2_terminate_seq_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof( sandesha2_terminate_seq_msg_processor_impl_t));
	
    if(!msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(!msg_proc_impl->msg_processor.ops)
	{
        sandesha2_terminate_seq_msg_processor_free((sandesha2_msg_processor_t*)
                         msg_proc_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops->process_in_msg = 
                        sandesha2_terminate_seq_msg_processor_process_in_msg;
    msg_proc_impl->msg_processor.ops->process_out_msg = 
    					sandesha2_terminate_seq_msg_processor_process_out_msg;
    msg_proc_impl->msg_processor.ops->free = 
                        sandesha2_terminate_seq_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
    const axis2_env_t *env)
{
    sandesha2_terminate_seq_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_seq_ack_t *seq_ack = NULL;
    sandesha2_terminate_seq_t *term_seq = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_msg_ctx_t *fault_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *term_rcvd_bean = NULL;
    sandesha2_seq_property_bean_t *transmit_bean = NULL;
    axis2_char_t *spec_version = NULL;
   
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_LOG_INFO(env->log, 
            "[sandesha2] sandesha2_terminate_msg_processor_process_in_msg .........");
    
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    
    seq_ack = (sandesha2_seq_ack_t*)sandesha2_msg_ctx_get_msg_part(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    
    if(seq_ack)
    {
        sandesha2_msg_processor_t *ack_processor = NULL;
        ack_processor = sandesha2_ack_msg_processor_create(env);
        sandesha2_msg_processor_process_in_msg(ack_processor, env, rm_msg_ctx);
    }
    term_seq = (sandesha2_terminate_seq_t*)sandesha2_msg_ctx_get_msg_part(
                    rm_msg_ctx, env, SANDESHA2_MSG_PART_TERMINATE_SEQ);
    if(!term_seq)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Terminate "
                    "Sequence part is not available");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING, 
                    AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    seq_id = sandesha2_identifier_get_identifier(
                    sandesha2_terminate_seq_get_identifier(term_seq, env), env);
    if(!seq_id || 0 == AXIS2_STRLEN(seq_id))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Invalid "
                    "sequence id");
        return AXIS2_FAILURE;
    }
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    fault_ctx = sandesha2_fault_mgr_check_for_unknown_seq(env, 
                        rm_msg_ctx, seq_id, storage_mgr);
    if(fault_ctx)
    {
        axis2_engine_t *engine = NULL;
        engine = axis2_engine_create(env, conf_ctx);
        
        AXIS2_ENGINE_SEND_FAULT(engine, env, sandesha2_msg_ctx_get_msg_ctx(
                    fault_ctx, env));
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
        return AXIS2_SUCCESS;
    }
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_mgr, env);
    term_rcvd_bean = sandesha2_seq_property_bean_create(env);
    sandesha2_seq_property_bean_set_seq_id(term_rcvd_bean, env, seq_id);
    sandesha2_seq_property_bean_set_name(term_rcvd_bean, env, 
                        SANDESHA2_SEQ_PROP_TERMINATE_RECEIVED);
    sandesha2_seq_property_bean_set_value(term_rcvd_bean, env, 
                        SANDESHA2_VALUE_TRUE);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, term_rcvd_bean);
   
    spec_version = sandesha2_msg_ctx_get_rm_spec_ver(rm_msg_ctx, env);
    if(AXIS2_TRUE == sandesha2_spec_specific_consts_is_term_seq_res_reqd(env, 
                spec_version))
        sandesha2_terminate_seq_msg_processor_add_terminate_seq_res(env, 
                rm_msg_ctx, seq_id, storage_mgr);
    sandesha2_terminate_seq_msg_processor_setup_highest_msg_nums(env, conf_ctx, 
            storage_mgr, seq_id, rm_msg_ctx);
    sandesha2_terminate_mgr_clean_recv_side_after_terminate_msg(env, conf_ctx,
                        seq_id, storage_mgr);
    transmit_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id,
                        SANDESHA2_SEQ_PROP_SEQ_TERMINATED, SANDESHA2_VALUE_TRUE);
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, transmit_bean);
    sandesha2_seq_mgr_update_last_activated_time(env, seq_id, storage_mgr);
    
    sandesha2_msg_ctx_set_paused(rm_msg_ctx, env, AXIS2_TRUE);
    AXIS2_LOG_INFO(env->log, 
            "[sandesha2] Exit: sandesha2_terminate_msg_processor_process_in_msg");
    return AXIS2_SUCCESS;
}


static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_setup_highest_msg_nums(
    const axis2_env_t *env, 
    axis2_conf_ctx_t *conf_ctx,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_char_t *seq_id,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_char_t *highest_msg_num_str = NULL;
    axis2_char_t *highest_msg_key = NULL;
    long highest_in_msg_num = 0;
    long highest_out_msg_num = 0;
    axis2_char_t *res_side_int_seq_id = NULL;
    axis2_bool_t add_res_side_term = AXIS2_FALSE;
    axis2_char_t *out_seq_id = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_mgr, env);
    
    highest_msg_num_str = sandesha2_utils_get_seq_property(env, seq_id,
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, storage_mgr);
    highest_msg_key = sandesha2_utils_get_seq_property(env, seq_id,
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, storage_mgr);
    if(highest_msg_num_str)
    {
        if(!highest_msg_key)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Key of the "
                        "highest message number has not been stored");
            return AXIS2_FAILURE;
        }
        highest_in_msg_num = atol(highest_msg_num_str);
    }
    res_side_int_seq_id = sandesha2_utils_get_outgoing_internal_seq_id(env,
                        seq_id);
    if(0 == highest_in_msg_num)
        add_res_side_term = AXIS2_FALSE;
    else
    {
        sandesha2_seq_property_bean_t *last_in_msg_bean = NULL;
        axis2_msg_ctx_t *highest_in_msg = NULL;
        axis2_msg_ctx_t *highest_out_msg = NULL;
        axis2_op_ctx_t *op_ctx = NULL;
        
        last_in_msg_bean = sandesha2_seq_property_bean_create_with_data(env,
                        seq_id, SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_NO,
                        highest_msg_num_str);
        sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, last_in_msg_bean);
        highest_in_msg = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env,
                        highest_msg_key, conf_ctx);
        if(highest_in_msg)
            op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(highest_in_msg, env);
        if(op_ctx)
            highest_out_msg = AXIS2_OP_CTX_GET_MSG_CTX(op_ctx, env, 
                AXIS2_WSDL_MESSAGE_LABEL_OUT);
        if(highest_out_msg)
        {
            sandesha2_msg_ctx_t *highest_out_rm_msg = NULL;
            sandesha2_seq_t *seq_of_out_msg = NULL;
            highest_out_rm_msg = sandesha2_msg_init_init_msg(env, 
                        highest_out_msg);
            seq_of_out_msg = (sandesha2_seq_t*)
                        sandesha2_msg_ctx_get_msg_part(highest_out_rm_msg, env,
                        SANDESHA2_MSG_PART_SEQ);
            if(seq_of_out_msg)
            {
                axis2_char_t long_str[32];
                sandesha2_seq_property_bean_t *highest_out_msg_bean = NULL;
                
                highest_out_msg_num = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(
                        sandesha2_seq_get_msg_num(seq_of_out_msg, env), env);
                sprintf(long_str, "%ld", highest_out_msg_num);
                highest_out_msg_bean = 
                        sandesha2_seq_property_bean_create_with_data(env, 
                        res_side_int_seq_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO,
                        long_str);
                sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, 
                        highest_out_msg_bean);
                add_res_side_term = AXIS2_TRUE;
            }
        }
    }
    out_seq_id = sandesha2_utils_get_seq_property(env, res_side_int_seq_id,
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, storage_mgr);
    if(AXIS2_TRUE == add_res_side_term && highest_out_msg_num > 0 &&
                res_side_int_seq_id && out_seq_id)
    {
        axis2_bool_t all_acked = AXIS2_FALSE;
        all_acked = sandesha2_utils_is_all_msgs_acked_upto(env, 
                        highest_out_msg_num, res_side_int_seq_id, storage_mgr);
        if(AXIS2_TRUE == all_acked)
            sandesha2_terminate_mgr_add_terminate_seq_msg(env, rm_msg_ctx,
                        out_seq_id, res_side_int_seq_id, storage_mgr);
    }
                        
    return AXIS2_SUCCESS;    
}

static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_add_terminate_seq_res(
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *seq_id,
    sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_msg_ctx_t *out_msg_ctx = NULL;
    sandesha2_msg_ctx_t *out_rm_msg = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg = NULL;
    sandesha2_seq_ack_t *seq_ack = NULL;
    axis2_property_t *property = NULL;
    axis2_engine_t *engine = NULL;
    axis2_char_t *addr_ns_uri = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_ctx_t *ctx = NULL;
    axis2_transport_out_desc_t *orig_trans_out = NULL;
    axis2_transport_out_desc_t *trans_out = NULL;
    sandesha2_sender_bean_t *term_res_bean = NULL;
    axis2_char_t *key = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    out_msg_ctx = axis2_core_utils_create_out_msg_ctx(env, msg_ctx);
    out_rm_msg = sandesha2_msg_creator_create_terminate_seq_res_msg(env, 
                        rm_msg_ctx, out_msg_ctx, storage_mgr);
   
    ack_rm_msg = sandesha2_ack_mgr_generate_ack_msg(env, rm_msg_ctx, seq_id,
                        storage_mgr);
    seq_ack = (sandesha2_seq_ack_t*)sandesha2_msg_ctx_get_msg_part(ack_rm_msg, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    sandesha2_msg_ctx_set_msg_part(out_rm_msg, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT,
                        (sandesha2_iom_rm_part_t*)seq_ack);
    sandesha2_msg_ctx_add_soap_envelope(out_rm_msg, env);
    sandesha2_msg_ctx_set_flow(out_rm_msg, env, AXIS2_OUT_FLOW);
    
    property = axis2_property_create_with_args(env, 0, 0, 0, 
        SANDESHA2_VALUE_TRUE);
    AXIS2_MSG_CTX_SET_PROPERTY(out_msg_ctx, env, 
        SANDESHA2_APPLICATION_PROCESSING_DONE, property, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_RESPONSE_WRITTEN(out_msg_ctx, env, AXIS2_TRUE);
    
    /* test code */

    orig_trans_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(out_msg_ctx, env);
    property = axis2_property_create_with_args(env, 0, 0, 0, orig_trans_out);
    AXIS2_MSG_CTX_SET_PROPERTY(out_msg_ctx, env,
        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property, AXIS2_FALSE);
    trans_out = sandesha2_utils_get_transport_out(env);
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(out_msg_ctx, env, trans_out);


    key = axis2_uuid_gen(env);
    term_res_bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_msg_ctx_ref_key(term_res_bean, env, key);
    /*AXIS2_MSG_CTX_SET_KEEP_ALIVE(msg_ctx, env, AXIS2_TRUE);*/
    property = axis2_property_create_with_args(env, 0, 0, 0, key);
    AXIS2_MSG_CTX_SET_PROPERTY(out_msg_ctx, env, SANDESHA2_MESSAGE_STORE_KEY, 
        property, AXIS2_FALSE);
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, key, out_msg_ctx);

    /* TODO: refine the terminate delay */
    sandesha2_sender_bean_set_time_to_send(term_res_bean, env,
                        sandesha2_utils_get_current_time_in_millis(env) +
                        SANDESHA2_TERMINATE_DELAY);
    sandesha2_sender_bean_set_msg_id(term_res_bean, env,
            (axis2_char_t *) AXIS2_MSG_CTX_GET_MSG_ID(out_msg_ctx, env));
    sandesha2_sender_bean_set_send(term_res_bean, env, AXIS2_TRUE);

    property = axis2_property_create_with_args(env, 0, 0, 0, 
        SANDESHA2_VALUE_TRUE);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, SANDESHA2_QUALIFIED_FOR_SENDING,
        property, AXIS2_FALSE);
    sandesha2_sender_bean_set_resend(term_res_bean, env, AXIS2_FALSE);
    retrans_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
    sandesha2_sender_mgr_insert(retrans_mgr, env, term_res_bean);

    /* end test code */
    
    engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env));
    AXIS2_ENGINE_SEND(engine, env, out_msg_ctx);
    
    addr_ns_uri = sandesha2_utils_get_seq_property(env, seq_id,
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE,
                        storage_mgr);
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    to_epr = AXIS2_MSG_CTX_GET_TO(msg_ctx, env);
    
    ctx = AXIS2_OP_CTX_GET_BASE(AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env), env);
    
    if(0 == AXIS2_STRCMP(anon_uri, AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env)))
    {
        property = axis2_property_create_with_args(env, 0, 0, 0, "TRUE");
    }
    else
    {
        property = axis2_property_create_with_args(env, 0, 0, 0, "FALSE");
    }
        
    AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN, property,
                        AXIS2_FALSE);
    return AXIS2_SUCCESS;
}
    
static axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_process_out_msg(
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_char_t *to_address = NULL;
    axis2_char_t *seq_key = NULL;
    axis2_char_t *int_seq_id = NULL;
    axis2_char_t *out_seq_id = NULL;
    axis2_property_t *property = NULL;
    axis2_char_t *terminated = NULL;
    axis2_op_t *old_op = NULL;
    axis2_op_t *out_in_op = NULL;
    axis2_qname_t *qname = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    sandesha2_terminate_seq_t *term_seq_part = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_char_t *transport_to = NULL;
    axis2_char_t *key = NULL;
    sandesha2_sender_bean_t *term_bean = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_seq_property_bean_t *term_added = NULL;
    axis2_transport_out_desc_t *out_desc = NULL;
    axis2_transport_out_desc_t *sandesha2_out_desc = NULL;
    axis2_engine_t *engine = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_LOG_INFO(env->log, 
            "[sandesha2] sandesha2_terminate_msg_processor_process_out_msg .........");
    
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_mgr, env);
    to_address = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(
                        AXIS2_MSG_CTX_GET_TO(msg_ctx, env), env);
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY,
                        AXIS2_FALSE);
    if(property)
        seq_key = AXIS2_PROPERTY_GET_VALUE(property, env);
    int_seq_id = sandesha2_utils_get_internal_seq_id(env, to_address, seq_key);
    out_seq_id = sandesha2_utils_get_seq_property(env, int_seq_id, 
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, storage_mgr);
    if(!out_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] seq_id was not"
                        " found. Cannot send the terminate message");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    terminated = sandesha2_utils_get_seq_property(env, int_seq_id,
                        SANDESHA2_SEQ_PROP_TERMINATE_ADDED, storage_mgr);
    old_op = AXIS2_MSG_CTX_GET_OP(msg_ctx, env);
    
    qname = axis2_qname_create(env, "temp", NULL, NULL);
    
    out_in_op = axis2_op_create_with_qname(env, qname);
    AXIS2_OP_SET_MSG_EXCHANGE_PATTERN(out_in_op, env, AXIS2_MEP_URI_OUT_IN);
    AXIS2_OP_SET_IN_FLOW(out_in_op, env, 
                        AXIS2_OP_GET_IN_FLOW(old_op, env));
    op_ctx = axis2_op_ctx_create(env, out_in_op, NULL);
    AXIS2_OP_CTX_SET_PARENT(op_ctx, env, AXIS2_MSG_CTX_GET_SVC_CTX(msg_ctx, env));
    AXIS2_CONF_CTX_REGISTER_OP_CTX(conf_ctx, env, sandesha2_msg_ctx_get_msg_id(
                        rm_msg_ctx, env), op_ctx);
    
    if(terminated && 0 == AXIS2_STRCMP(terminated, SANDESHA2_VALUE_TRUE))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Terminate was "
                        "added previously");
        return AXIS2_SUCCESS;
    }
    term_seq_part = (sandesha2_terminate_seq_t*)sandesha2_msg_ctx_get_msg_part(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_TERMINATE_SEQ);
    sandesha2_identifier_set_identifier(sandesha2_terminate_seq_get_identifier(
                        term_seq_part, env), env, out_seq_id);
    sandesha2_msg_ctx_set_flow(rm_msg_ctx, env, AXIS2_OUT_FLOW);
    property = axis2_property_create_with_args(env, 0, 0, 0, SANDESHA2_VALUE_TRUE);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property,
                        AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_TO(msg_ctx, env, axis2_endpoint_ref_create(env, to_address));
    rm_version = sandesha2_utils_get_rm_version(env, int_seq_id, storage_mgr);
    if(!rm_version)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Cant find the"
                        " rm_version of the given message");
        return AXIS2_FAILURE;
    }
    AXIS2_MSG_CTX_SET_WSA_ACTION(msg_ctx, env, 
                        sandesha2_spec_specific_consts_get_terminate_seq_action(
                        env, rm_version));
    AXIS2_MSG_CTX_SET_SOAP_ACTION(msg_ctx, env,
                        sandesha2_spec_specific_consts_get_terminate_seq_soap_action
                        (env, rm_version));
    transport_to = sandesha2_utils_get_seq_property(env, int_seq_id, 
                        SANDESHA2_SEQ_PROP_TRANSPORT_TO, storage_mgr);

    if(transport_to)
    {
        property = axis2_property_create_with_args(env, 0, 0, 0, transport_to);
        AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        AXIS2_TRANSPORT_URL, property, AXIS2_FALSE);
    }
    sandesha2_msg_ctx_add_soap_envelope(rm_msg_ctx, env);
    
    key = axis2_uuid_gen(env);
    term_bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_msg_ctx_ref_key(term_bean, env, key);
    /*AXIS2_MSG_CTX_SET_KEEP_ALIVE(msg_ctx, env, AXIS2_TRUE);*/
    sandesha2_storage_mgr_store_msg_ctx(storage_mgr, env, key, msg_ctx);
    
    /* TODO: refine the terminate delay */
    sandesha2_sender_bean_set_time_to_send(term_bean, env, 
                        sandesha2_utils_get_current_time_in_millis(env) + 
                        SANDESHA2_TERMINATE_DELAY);
    sandesha2_sender_bean_set_msg_id(term_bean, env, 
            (axis2_char_t *) AXIS2_MSG_CTX_GET_MSG_ID(msg_ctx, env));
    sandesha2_sender_bean_set_send(term_bean, env, AXIS2_TRUE);
    
    property = axis2_property_create_with_args(env, 0, 0, 0, 
        SANDESHA2_VALUE_TRUE);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, SANDESHA2_QUALIFIED_FOR_SENDING, 
        property, AXIS2_FALSE); 
    sandesha2_sender_bean_set_resend(term_bean, env, AXIS2_FALSE);
    retrans_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
    sandesha2_sender_mgr_insert(retrans_mgr, env, term_bean);
    
    term_added = sandesha2_seq_property_bean_create(env);
    sandesha2_seq_property_bean_set_name(term_added, env, 
                        SANDESHA2_SEQ_PROP_TERMINATE_ADDED);
    sandesha2_seq_property_bean_set_seq_id(term_added, env, out_seq_id);
    sandesha2_seq_property_bean_set_value(term_added, env, SANDESHA2_VALUE_TRUE);
    
    sandesha2_seq_property_mgr_insert(seq_prop_mgr, env, term_added);
    
    out_desc = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx, env);
    
    property = axis2_property_create_with_args(env, 0, 0, 
        out_desc->ops->free_void_arg, out_desc);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property, 
                        AXIS2_FALSE);
    
    property = axis2_property_create_with_args(env, 0, 0, 0, key);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_MESSAGE_STORE_KEY, property, AXIS2_FALSE);
    
    property = axis2_property_create_with_args(env, 0, 0, 0, 
        SANDESHA2_VALUE_TRUE);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_SET_SEND_TO_TRUE, property, AXIS2_FALSE);
    sandesha2_out_desc = sandesha2_utils_get_transport_out(env);                   
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(msg_ctx, env, sandesha2_out_desc);
    engine = axis2_engine_create(env, conf_ctx);
    /*AXIS2_ENGINE_SEND(engine, env, msg_ctx);*/
    transport_sender = AXIS2_TRANSPORT_OUT_DESC_GET_SENDER(sandesha2_out_desc, 
        env);
    AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, msg_ctx);
    sandesha2_terminate_mgr_terminate_sending_side(env, conf_ctx, int_seq_id, 
        AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env), storage_mgr);

    AXIS2_LOG_INFO(env->log, 
        "[sandesha2] Exit: sandesha2_terminate_msg_processor_process_out_msg");
    return AXIS2_SUCCESS;
}

