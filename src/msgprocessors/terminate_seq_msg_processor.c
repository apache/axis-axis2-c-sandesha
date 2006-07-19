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
#include <sandesha2/sandesha2_terminate_seq_msg_processor.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2/sandesha2_constants.h>
#include <sandesha2/sandesha2_utils.h>
#include <sandesha2/sandesha2_msg_ctx.h>
#include <sandesha2/sandesha2_terminate_seq.h>
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
#include <sandesha2/sandesha2_ack_msg_processor.h>
#include <sandesha2/sandesha2_seq.h>
#include "../client/sandesha2_client_constants.h"

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
axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_process_in_msg (
                        sandesha2_msg_processor_t *msg_processor,
						const axis2_env_t *env,
                        sandesha2_msg_ctx_t *rm_msg_ctx);
    
axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx);
    
axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_setup_highest_msg_nums(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_conf_ctx_t *conf_ctx,
                        sandesha2_storage_mgr_t *storage_man,
                        axis2_char_t *seq_id,
                        sandesha2_msg_ctx_t *rm_msg_ctx);

axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_add_terminate_seq_res(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx,
                        axis2_char_t *seq_id,
                        sandesha2_storage_mgr_t *storage_man);
                    	
axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_terminate_seq_msg_processor_create(const axis2_env_t *env)
{
    sandesha2_terminate_seq_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
              
    msg_proc_impl =  ( sandesha2_terminate_seq_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof( sandesha2_terminate_seq_msg_processor_impl_t));
	
    if(NULL == msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(NULL == msg_proc_impl->msg_processor.ops)
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


axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_free (sandesha2_msg_processor_t *msg_processor, 
						const axis2_env_t *env)
{
    sandesha2_terminate_seq_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(NULL != msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL 
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
    sandesha2_storage_mgr_t *storage_man = NULL;
    sandesha2_fault_mgr_t *fault_mgr = NULL;
    sandesha2_msg_ctx_t *fault_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *term_rcvd_bean = NULL;
    sandesha2_seq_property_bean_t *transmit_bean = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    
    seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    
    if(NULL != seq_ack)
    {
        sandesha2_msg_processor_t *ack_processor = NULL;
        ack_processor = sandesha2_ack_msg_processor_create(env);
        SANDESHA2_MSG_PROCESSOR_PROCESS_IN_MSG(ack_processor, env, rm_msg_ctx);
    }
    term_seq = (sandesha2_terminate_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                    rm_msg_ctx, env, SANDESHA2_MSG_PART_TERMINATE_SEQ);
    if(NULL == term_seq)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Terminate "
                    "Sequence part is not available");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING, 
                    AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                    SANDESHA2_TERMINATE_SEQ_GET_IDENTIFIER(term_seq, env), env);
    if(NULL == seq_id || 0 == AXIS2_STRLEN(seq_id))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Invalid "
                    "sequence id");
        return AXIS2_FAILURE;
    }
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    storage_man = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    fault_mgr = sandesha2_fault_mgr_create(env);
    fault_ctx = SANDESHA2_FAULT_MGR_CHECK_FOR_UNKNOWN_SEQ(fault_mgr, env, 
                        rm_msg_ctx, seq_id, storage_man);
    if(NULL != fault_ctx)
    {
        axis2_engine_t *engine = NULL;
        engine = axis2_engine_create(env, conf_ctx);
        
        AXIS2_ENGINE_SEND_FAULT(engine, env, SANDESHA2_MSG_CTX_GET_MSG_CTX(
                    fault_ctx, env));
        AXIS2_MSG_CTX_PAUSE(msg_ctx, env);
        return AXIS2_SUCCESS;
    }
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_man, env);
    term_rcvd_bean = sandesha2_seq_property_bean_create(env);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(term_rcvd_bean, env, seq_id);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(term_rcvd_bean, env, 
                        SANDESHA2_SEQ_PROP_TERMINATE_RECEIVED);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(term_rcvd_bean, env, 
                        SANDESHA2_VALUE_TRUE);
    SANDESHA2_SEQ_PRPERTY_MGR_INSERT(seq_prop_mgr, env, term_rcvd_bean);
    
    if(AXIS2_TRUE == sandesha2_spec_specific_consts_is_term_seq_res_reqd(env, 
                        SANDESHA2_MSG_CTX_GET_RM_SPEC_VER(rm_msg_ctx, env)))
        sandesha2_terminate_seq_msg_processor_add_terminate_seq_res(
                        msg_processor, env, rm_msg_ctx, seq_id, storage_man);
    sandesha2_terminate_seq_msg_processor_setup_highest_msg_nums(msg_processor,
                        env, conf_ctx, storage_man, seq_id, rm_msg_ctx);
    sandesha2_terminate_mgr_clean_recv_side_on_terminate_msg(env, conf_ctx,
                        seq_id, storage_man);
    transmit_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id,
                        SANDESHA2_SEQ_PROP_SEQ_TERMINATED, SANDESHA2_VALUE_TRUE);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, transmit_bean);
    sandesha2_seq_mgr_update_last_activated_time(env, seq_id, storage_man);
    
    SANDESHA2_MSG_CTX_PAUSE(msg_ctx, env);
    
    return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_setup_highest_msg_nums(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        axis2_conf_ctx_t *conf_ctx,
                        sandesha2_storage_mgr_t *storage_man,
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
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_man, env);
    
    highest_msg_num_str = sandesha2_utils_get_seq_property(env, seq_id,
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_NUMBER, storage_man);
    highest_msg_key = sandesha2_utils_get_seq_property(env, seq_id,
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, storage_man);
    if(NULL != highest_msg_num_str)
    {
        if(NULL == highest_msg_key)
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
        
        last_in_msg_bean = sandesha2_seq_property_bean_create_with_data(env,
                        seq_id, SANDESHA2_SEQ_PROP_LAST_IN_MESSAGE_NO,
                        highest_msg_num_str);
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, last_in_msg_bean);
        highest_in_msg = SANDESHA2_STORAGE_MGR_RETRIEVE_MSG_CTX(storage_man, env,
                        highest_msg_key, conf_ctx);
        highest_out_msg = AXIS2_OP_CTX_GET_MSG_CTX(AXIS2_MSG_CTX_GET_OP_CTX(
                        highest_in_msg, env), env, 
                        AXIS2_WSDL_MESSAGE_LABEL_OUT_VALUE);
        if(NULL != highest_out_msg)
        {
            sandesha2_msg_ctx_t *highest_out_rm_msg = NULL;
            sandesha2_seq_t *seq_of_out_msg = NULL;
            highest_out_rm_msg = sandesha2_msg_initilizer_init_msg(env, 
                        highest_out_msg);
            seq_of_out_msg = (sandesha2_seq_t*)
                        SANDESHA2_MSG_CTX_GET_MSG_PART(highest_out_rm_msg, env,
                        SANDESHA2_MSG_PART_SEQ);
            if(NULL != seq_of_out_msg)
            {
                axis2_char_t long_str[32];
                sandesha2_seq_property_bean_t *highest_out_msg_bean = NULL;
                
                highest_out_msg_num = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(
                        SANDESHA2_SEQ_GET_MSG_NUM(seq_of_out_msg, env), env);
                sprintf(long_str, "%ld", highest_out_msg_num);
                highest_out_msg_bean = 
                        sandesha2_seq_property_bean_create_with_data(env, 
                        res_side_int_seq_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO,
                        long_str);
                SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, 
                        highest_out_msg_bean);
                add_res_side_term = AXIS2_TRUE;
            }
        }
    }
    out_seq_id = sandesha2_utils_get_seq_property(env, res_side_int_seq_id,
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, storage_man);
    if(AXIS2_TRUE == add_res_side_term && highest_out_msg_num > 0 &&
                NULL != res_side_int_seq_id && NULL != out_seq_id)
    {
        axis2_bool_t all_acked = AXIS2_FALSE;
        all_acked = sandesha2_utils_is_all_msgs_acked_upto(env, 
                        highest_out_msg_num, res_side_int_seq_id, storage_man);
        if(AXIS2_TRUE == all_acked)
            sandesha2_terminate_mgr_add_terminate_seq_msg(env, rm_msg_ctx,
                        out_seq_id, res_side_int_seq_id, storage_man);
    }
                        
    return AXIS2_SUCCESS;    
}

axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_add_terminate_seq_res(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx,
                        axis2_char_t *seq_id,
                        sandesha2_storage_mgr_t *storage_man)
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
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    out_msg_ctx = axis2_core_utils_create_out_msg_ctx(env, msg_ctx);
    out_rm_msg = sandesha2_msg_creator_create_terminate_seq_res_msg(env, 
                        rm_msg_ctx, out_msg_ctx, storage_man);
    
    ack_rm_msg = sandesha2_ack_mgr_generate_ack_msg(env, rm_msg_ctx, seq_id,
                        storage_man);
    seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(ack_rm_msg, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    SANDESHA2_MSG_CTX_SET_MSG_PART(out_rm_msg, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT,
                        (sandesha2_iom_rm_part_t*)seq_ack);
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(out_rm_msg, env);
    SANDESHA2_MSG_CTX_SET_FLOW(out_rm_msg, env, AXIS2_OUT_FLOW);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(out_msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property,
                        AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_RESPONSE_WRITTEN(out_msg_ctx, env, AXIS2_TRUE);
    
    engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env));
    AXIS2_ENGINE_SEND(engine, env, out_msg_ctx);
    
    addr_ns_uri = sandesha2_utils_get_seq_property(env, seq_id,
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE,
                        storage_man);
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    to_epr = AXIS2_MSG_CTX_GET_TO(msg_ctx, env);
    
    ctx = AXIS2_OP_CTX_GET_BASE(AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env), env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    
    if(0 == AXIS2_STRCMP(anon_uri, AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env)))
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP("TRUE", env));
    else
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP("FALSE", env));
        
    AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN, property,
                        AXIS2_FALSE);
    return AXIS2_SUCCESS;
}
    
axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_msg_processor_process_out_msg(
                        sandesha2_msg_processor_t *msg_processor,
                    	const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_storage_mgr_t *storage_man = NULL;
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
    axis2_engine_t *engine = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    
    storage_man = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_man, env);
    to_address = AXIS2_ENDPOINT_REF_GET_ADDRESS(AXIS2_MSG_CTX_GET_TO(msg_ctx,
                        env), env);
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, SANDESHA2_CLIENT_SEQ_KEY,
                        AXIS2_FALSE);
    seq_key = AXIS2_PROPERTY_GET_VALUE(property, env);
    int_seq_id = sandesha2_utils_get_internal_seq_id(env, to_address, seq_key);
    out_seq_id = sandesha2_utils_get_seq_property(env, int_seq_id, 
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, storage_man);
    if(NULL == out_seq_id)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] seq_id was not"
                        " found. Cannot send the terminate message");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    terminated = sandesha2_utils_get_seq_property(env, int_seq_id,
                        SANDESHA2_SEQ_PROP_TERMINATE_ADDED, storage_man);
    old_op = AXIS2_MSG_CTX_GET_OP(msg_ctx, env);
    
    qname = axis2_qname_create(env, "temp", NULL, NULL);
    
    out_in_op = axis2_op_create_with_qname(env, qname);
    AXIS2_OP_SET_MSG_EXCHANGE_PATTERN(out_in_op, env, AXIS2_MEP_URI_OUT_IN);
    AXIS2_OP_SET_REMAINING_PHASES_INFLOW(out_in_op, env, 
                        AXIS2_OP_GET_REMAINING_PHASES_INFLOW(old_op, env));
    op_ctx = axis2_op_ctx_create(env, out_in_op, NULL);
    AXIS2_OP_CTX_SET_PARENT(op_ctx, env, AXIS2_MSG_CTX_GET_SVC_CTX(msg_ctx, env));
    AXIS2_CONF_CTX_REGISTER_OP_CTX(conf_ctx, env, SANDESHA2_MSG_CTX_GET_MSG_ID(
                        rm_msg_ctx, env), op_ctx);
    
    if(NULL != terminated && 0 == AXIS2_STRCMP(terminated, SANDESHA2_VALUE_TRUE))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Terminate was "
                        "added previously");
        return AXIS2_SUCCESS;
    }
    term_seq_part = (sandesha2_terminate_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_TERMINATE_SEQ);
    SANDESHA2_IDENTIFIER_SET_IDENTIFIER(SANDESHA2_TERMINATE_SEQ_GET_IDENTIFIER(
                        term_seq_part, env), env, out_seq_id);
    SANDESHA2_MSG_CTX_SET_FLOW(rm_msg_ctx, env, AXIS2_OUT_FLOW);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property,
                        AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_TO(msg_ctx, env, axis2_endpoint_ref_create(env, to_address));
    rm_version = sandesha2_utils_get_rm_version(env, int_seq_id, storage_man);
    if(NULL == rm_version)
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
                        SANDESHA2_SEQ_PROP_TRANSPORT_TO, storage_man);

    if(NULL != transport_to)
    {
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(transport_to, 
                            env));
        AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        AXIS2_TRANSPORT_URL, property, AXIS2_FALSE);
    }
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(rm_msg_ctx, env);
    
    key = axis2_uuid_gen(env);
    term_bean = sandesha2_sender_bean_create(env);
    SANDESHA2_SENDER_BEAN_SET_MSG_CTX_REF_KEY(term_bean, env, key);
    SANDESHA2_STORAGE_MGR_STORE_MSG_CTX(storage_man, env, key, msg_ctx);
    
    /* TODO: refine the terminate delay */
    SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(term_bean, env, 
                        sandesha2_utils_get_current_time_in_millis(env) + 
                        SANDESHA2_TERMINATE_DELAY);
    SANDESHA2_SENDER_BEAN_MSG_ID(term_bean, env, AXIS2_MSG_CTX_GET_MSG_ID(
                        msg_ctx, env));
    SANDESHA2_SENDER_BEAN_SET_SEND(term_bean, env, AXIS2_TRUE);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_QUALIFIED_FOR_SENDING, property, AXIS2_FALSE);
    
    SANDESHA2_SENDER_BEAN_SET_RESEND(term_bean, env, AXIS2_FALSE);
    
    retrans_mgr = SANDESHA2_STORAGE_MGR_GET_RETRANS_MGR(storage_man, env);
    SANDESHA2_SENDER_MGR_INSERT(retrans_mgr, env, term_bean);
    
    term_added = sandesha2_seq_property_bean_create(env);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(term_added, env, 
                        SANDESHA2_SEQ_PROP_TERMINATE_ADDED);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(term_added, env, out_seq_id);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(term_added, env, SANDESHA2_VALUE_TRUE);
    
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, term_added);
    
    out_desc = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx, env);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, out_desc);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property, 
                        AXIS2_FALSE);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(key,env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_MESSAGE_STORE_KEY, property, AXIS2_FALSE);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_SET_SEND_TO_TRUE, property, AXIS2_FALSE);
                        
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(msg_ctx, env, 
                        sandesha2_utils_get_transport_out(env));
    engine = axis2_engine_create(env, conf_ctx);
    AXIS2_ENGINE_SEND(engine, env, msg_ctx);
    return AXIS2_SUCCESS;
}
