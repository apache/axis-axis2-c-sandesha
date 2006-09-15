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
 
#include <sandesha2_utils.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_property_bean.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_ack_range.h>
#include <sandesha2_spec_specific_consts.h>
#include <axis2_string.h>
#include <axis2_uuid_gen.h>
#include <axis2_addr.h>
#include <axis2_property.h>
#include <axis2_array_list.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_seq_ack.h>
#include <axis2_op.h>
#include <sandesha2_msg_creator.h>

AXIS2_EXTERN sandesha2_msg_ctx_t *AXIS2_CALL
sandesha2_ack_mgr_generate_ack_msg(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *ref_rm_msg,
        axis2_char_t *seq_id,
        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_msg_ctx_t *ref_msg = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *acks_to_bean = NULL;
    axis2_endpoint_ref_t *acks_to = NULL;
    axis2_char_t *acks_to_str = NULL;
    axis2_op_t *ack_op = NULL;
    axis2_op_t *ref_op = NULL;
    axis2_msg_ctx_t *ack_msg_ctx = NULL;
    axis2_char_t *addr_ns_uri = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_property_t *property = NULL;
    sandesha2_msg_ctx_t *ack_rm_msg = NULL;
    axiom_soap_envelope_t *soap_env = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, seq_id, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, NULL);
    
    ref_msg = SANDESHA2_MSG_CTX_GET_MSG_CTX(ref_rm_msg, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(ref_msg, env);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr,
                        env);
    acks_to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env,
                        seq_id, SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
    acks_to = axis2_endpoint_ref_create(env, 
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(acks_to_bean, 
                        env));
    if(NULL != acks_to)
        acks_to_str = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(acks_to, env);
    if(NULL == acks_to_str)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_INVALID_EPR, AXIS2_FAILURE);
        return NULL;
    }
    ack_op = axis2_op_create(env);
    AXIS2_OP_SET_MSG_EXCHANGE_PATTERN(ack_op, env, AXIS2_MEP_URI_OUT_ONLY);
    
    ref_op = AXIS2_MSG_CTX_GET_OP(ref_msg, env);
    if(NULL != ref_op)
    {
        axis2_array_list_t *out_flows = NULL;
        axis2_array_list_t *out_fault_flows = NULL;
        out_flows = AXIS2_OP_GET_OUT_FLOW(ref_op, env);
        out_fault_flows = AXIS2_OP_GET_FAULT_OUT_FLOW(ref_op, env);
        if(NULL != out_flows)
        {
            AXIS2_OP_SET_OUT_FLOW(ack_op, env, out_flows);
            AXIS2_OP_SET_FAULT_OUT_FLOW(ack_op, env, out_fault_flows);
        }
    }
    ack_msg_ctx = sandesha2_utils_create_new_related_msg_ctx(env, ref_rm_msg,
                        ack_op);
    property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, AXIS2_WSA_VERSION, 
                        AXIS2_FALSE);
    if(property)
        AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env, AXIS2_WSA_VERSION, property,
                        AXIS2_FALSE);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property,
                        AXIS2_FALSE);
    ack_rm_msg = sandesha2_msg_init_init_msg(env, ack_msg_ctx);
    SANDESHA2_MSG_CTX_SET_RM_NS_VAL(ack_rm_msg, env, 
                        SANDESHA2_MSG_CTX_GET_RM_NS_VAL(ref_rm_msg, env));
    AXIS2_MSG_CTX_SET_WSA_MESSAGE_ID(ack_msg_ctx, env, axis2_uuid_gen(env));
    soap_env = axiom_soap_envelope_create_default_soap_envelope(env, 
                        sandesha2_utils_get_soap_version(env, 
                        AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(ref_msg, env)));
    AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(ack_msg_ctx, env, soap_env);
    AXIS2_MSG_CTX_SET_TO(ack_msg_ctx, env, acks_to);
    /* Adding the sequence acknowledgement part */
    sandesha2_msg_creator_add_ack_msg(env, ack_rm_msg, seq_id, storage_mgr);
    AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env, AXIS2_TRANSPORT_IN, NULL, 
            AXIS2_FALSE);
    addr_ns_uri = sandesha2_utils_get_seq_property(env, seq_id, 
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, 
                        storage_mgr);
    if(addr_ns_uri)
    {
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(addr_ns_uri, env));
    }
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    if(property)
    {
        AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env, AXIS2_WSA_VERSION, property, 
            AXIS2_FALSE);
    }
    if(0 == AXIS2_STRCMP(acks_to_str, anon_uri))
    {
        axis2_ctx_t *ref_ctx = NULL;
        axis2_op_ctx_t *op_ctx = NULL;

        op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(ref_msg, env);
        if(NULL == op_ctx)
        {
            axis2_op_t *op = axis2_op_create(env);
            AXIS2_OP_SET_MSG_EXCHANGE_PATTERN(op, env, AXIS2_MEP_URI_IN_OUT);
            op_ctx = axis2_op_ctx_create(env, op, NULL);
            AXIS2_MSG_CTX_SET_OP(ref_msg, env, op);
            AXIS2_MSG_CTX_SET_OP_CTX(ref_msg, env, op_ctx);            
        }
        ref_ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP("TRUE", env));
        AXIS2_CTX_SET_PROPERTY(ref_ctx, env, AXIS2_RESPONSE_WRITTEN,
                        property, AXIS2_FALSE);
        
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_TRUE, env));
        /* AXIS2_MSG_CTX_SET_PROPERTY(ref_msg, env, SANDESHA2_ACK_WRITTEN, property,
                        AXIS2_FALSE);*/
        AXIS2_CTX_SET_PROPERTY(ref_ctx, env, SANDESHA2_ACK_WRITTEN, property, 
                AXIS2_FALSE);
        AXIS2_MSG_CTX_SET_SERVER_SIDE(ack_msg_ctx, env, AXIS2_TRUE);
        return ack_rm_msg;
    }
    else
    {
        sandesha2_sender_mgr_t *retrans_mgr = NULL;
        sandesha2_sender_bean_t *ack_bean = NULL;
        sandesha2_sender_bean_t *find_bean = NULL;
        axis2_char_t *key = NULL;
        sandesha2_property_bean_t *ack_int_bean = NULL;
        long ack_interval = -1;
        long time_to_send = -1;
        axis2_array_list_t *bean_list = NULL;
        int i = 0;
        axis2_transport_out_desc_t *orig_trans_out = NULL;
        axis2_transport_out_desc_t *trans_out = NULL;
        sandesha2_msg_ctx_t *ret_rm_msg = NULL;
        
        retrans_mgr = SANDESHA2_STORAGE_MGR_GET_RETRANS_MGR(storage_mgr, env);
        key = axis2_uuid_gen(env);
        ack_bean = sandesha2_sender_bean_create(env);
        SANDESHA2_SENDER_BEAN_SET_MSG_CONTEXT_REF_KEY(ack_bean, env, key);
        SANDESHA2_SENDER_BEAN_SET_MSG_ID(ack_bean, env, 
                        (axis2_char_t*)AXIS2_MSG_CTX_GET_WSA_MESSAGE_ID(
                        ack_msg_ctx, env));
        SANDESHA2_SENDER_BEAN_SET_RESEND(ack_bean, env, AXIS2_FALSE);
        SANDESHA2_SENDER_BEAN_SET_SEQ_ID(ack_bean, env, seq_id);
        SANDESHA2_SENDER_BEAN_SET_SEND(ack_bean, env, AXIS2_TRUE);
        
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_FALSE, env));
        AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env, 
                        SANDESHA2_QUALIFIED_FOR_SENDING, property, AXIS2_FALSE);
        SANDESHA2_SENDER_BEAN_SET_MSG_TYPE(ack_bean, env, 
                        SANDESHA2_MSG_TYPE_ACK);

        ack_int_bean = sandesha2_utils_get_property_bean_from_op(env,
                        AXIS2_MSG_CTX_GET_OP(ref_msg, env));
        ack_interval = SANDESHA2_PROPERTY_BEAN_GET_ACK_INTERVAL(ack_int_bean, 
                        env);
        time_to_send = sandesha2_utils_get_current_time_in_millis(env) + 
                        ack_interval;
        find_bean = sandesha2_sender_bean_create(env);
        SANDESHA2_SENDER_BEAN_SET_MSG_TYPE(find_bean, env, 
                        SANDESHA2_MSG_TYPE_ACK);
        SANDESHA2_SENDER_BEAN_SET_SEND(find_bean, env, AXIS2_TRUE);
        SANDESHA2_SENDER_BEAN_SET_RESEND(find_bean, env, AXIS2_FALSE);
        bean_list = SANDESHA2_SENDER_MGR_FIND_BY_SENDER_BEAN(retrans_mgr, env, 
                        find_bean);
        
        for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(bean_list, env); i++)
        {
            sandesha2_sender_bean_t *old_ack_bean = NULL;
            old_ack_bean = AXIS2_ARRAY_LIST_GET(bean_list, env, i);
            time_to_send = SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(old_ack_bean,
                        env);
            SANDESHA2_SENDER_MGR_REMOVE(retrans_mgr, env, 
                        SANDESHA2_SENDER_BEAN_GET_MSG_ID(old_ack_bean, env));
            SANDESHA2_STORAGE_MGR_REMOVE_MSG_CTX(storage_mgr, env, 
                        SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(
                        old_ack_bean, env));
        }
        SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(ack_bean, env, time_to_send);
        SANDESHA2_STORAGE_MGR_STORE_MSG_CTX(storage_mgr, env, key, ack_msg_ctx);
        SANDESHA2_SENDER_MGR_INSERT(retrans_mgr, env, ack_bean);
        
        orig_trans_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(ack_msg_ctx, 
                        env);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, orig_trans_out);
        AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env,
                            SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property,
                            AXIS2_FALSE);
        
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_TRUE, env));
        AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env, 
                        SANDESHA2_SET_SEND_TO_TRUE, property, AXIS2_FALSE);
                        
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        key, env));
        AXIS2_MSG_CTX_SET_PROPERTY(ack_msg_ctx, env, 
                        SANDESHA2_MESSAGE_STORE_KEY, property, AXIS2_FALSE);
        
        trans_out = sandesha2_utils_get_transport_out(env);
        AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(ack_msg_ctx, env, trans_out);
        
        ret_rm_msg = sandesha2_msg_init_init_msg(env, ack_msg_ctx);
        sandesha2_utils_start_sender_for_seq(env, conf_ctx, seq_id);
        AXIS2_MSG_CTX_SET_PAUSED(ref_msg, env, AXIS2_TRUE);
        return ret_rm_msg;
    }
    return NULL;
}

/**
 * This is used to get the acked messages of a sequence. If this is an outgoing 
 * message the sequence_identifier should be the internal sequenceID.
 * 
 * @param sequence_identifier
 * @param out_going_msg
 * @return
 */
AXIS2_EXTERN axis2_array_list_t *AXIS2_CALL
sandesha2_ack_mgr_get_client_completed_msgs_list(
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    sandesha2_seq_property_bean_t *internal_seq_bean = NULL;
    axis2_char_t *internal_seq_id = NULL;
    sandesha2_seq_property_bean_t *completed_msgs_bean = NULL;
    axis2_array_list_t *completed_msg_list = NULL;
    
    /* First trying to get it from the internal sequence id.*/
    internal_seq_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
            seq_id, SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID);
    if(internal_seq_bean != NULL)
    {
        internal_seq_id = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                internal_seq_bean, env);
    }
    if(internal_seq_id != NULL)
    {
        completed_msgs_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, 
                env, internal_seq_id, 
                SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES);
    }
    if(completed_msgs_bean == NULL)
    {
        completed_msgs_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, 
                env, seq_id, 
                SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES);
    }
    if(completed_msgs_bean != NULL)
    {
        axis2_char_t *value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                completed_msgs_bean, env);
        completed_msg_list = sandesha2_utils_get_array_list_from_string(env, value);
    }
    else
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_COMPLETED_MSGS_BEAN_IS_NULL, 
                AXIS2_FAILURE);
        return NULL;
    }
    return completed_msg_list;
}
 
AXIS2_EXTERN axis2_array_list_t *AXIS2_CALL
sandesha2_ack_mgr_get_svr_completed_msgs_list(
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        sandesha2_seq_property_mgr_t *seq_prop_mgr)
{
    sandesha2_seq_property_bean_t *completed_msgs_bean = NULL;
    axis2_array_list_t *completed_msg_list = NULL;
    
    completed_msgs_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, 
            env, seq_id, 
            SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES);
    if(completed_msgs_bean != NULL)
    {
        axis2_char_t *value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                completed_msgs_bean, env);
        completed_msg_list = sandesha2_utils_get_array_list_from_string(env, value);
    }
    else
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_COMPLETED_MSGS_BEAN_IS_NULL, 
                AXIS2_FAILURE);
        return NULL;
    }
    return completed_msg_list;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_ack_mgr_verify_seq_completion(
    const axis2_env_t *env,
    axis2_array_list_t *ack_ranges,
    long last_msg_no)
{
    axis2_hash_t *hash = NULL;
    axis2_char_t tmp[32];
    int i = 0;
    long start = 1;
    
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, ack_ranges, AXIS2_FALSE);
    
    hash = axis2_hash_make(env);
    for(i  = 0; i< AXIS2_ARRAY_LIST_SIZE(ack_ranges, env); i++)
    {
        sandesha2_ack_range_t *ack_range = NULL;
        
        ack_range = AXIS2_ARRAY_LIST_GET(ack_ranges, env, i);
        sprintf(tmp, "%ld", SANDESHA2_ACK_RANGE_GET_LOWER_VALUE(ack_range, env));
        axis2_hash_set(hash, tmp, AXIS2_HASH_KEY_STRING, ack_range);
    }
    
    while(AXIS2_TRUE)
    {
        sandesha2_ack_range_t *ack_range = NULL;
        sprintf(tmp, "%ld", start);
        ack_range = axis2_hash_get(hash, tmp, AXIS2_HASH_KEY_STRING);
        
        if(NULL == ack_range)
            break;
        if(SANDESHA2_ACK_RANGE_GET_UPPER_VALUE(ack_range, env) >= last_msg_no)
            return AXIS2_TRUE;
        start = SANDESHA2_ACK_RANGE_GET_UPPER_VALUE(ack_range, env) + 1;        
    }
    return AXIS2_FALSE;
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_ack_mgr_piggyback_acks_if_present(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_sender_bean_t *find_bean = NULL;
    axis2_char_t *to_str = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_endpoint_ref_t *to = NULL;
    axis2_array_list_t *found_list = NULL;
    int i = 0;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    retrans_mgr = SANDESHA2_STORAGE_MGR_GET_RETRANS_MGR(storage_mgr, env);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    
    find_bean = sandesha2_sender_bean_create(env);
    SANDESHA2_SENDER_BEAN_SET_MSG_TYPE(find_bean, env, SANDESHA2_MSG_TYPE_ACK);
    SANDESHA2_SENDER_BEAN_SET_SEND(find_bean, env, AXIS2_TRUE);
    SANDESHA2_SENDER_BEAN_SET_RESEND(find_bean, env, AXIS2_FALSE);
   
    to = SANDESHA2_MSG_CTX_GET_TO(rm_msg_ctx, env);
    if(to)
        to_str = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(to, env);
                        
    found_list = SANDESHA2_SENDER_MGR_FIND_BY_SENDER_BEAN(retrans_mgr, env, 
                        find_bean);
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(found_list, env); i++)
    {
        sandesha2_sender_bean_t *sender_bean = NULL;
        long timenow = 0;
        
        
        timenow = sandesha2_utils_get_current_time_in_millis(env);
        sender_bean = AXIS2_ARRAY_LIST_GET(found_list, env, i);
        
        if(SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(sender_bean, env) > timenow)
        {
            axis2_msg_ctx_t *msg_ctx1 = NULL;
            axis2_char_t *to = NULL;
            sandesha2_msg_ctx_t *ack_rm_msg = NULL;
            sandesha2_seq_ack_t *seq_ack = NULL;
            
            msg_ctx1 = SANDESHA2_STORAGE_MGR_RETRIEVE_MSG_CTX(storage_mgr, env,
                        SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(
                        sender_bean, env), conf_ctx);
            to = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(
                        AXIS2_MSG_CTX_GET_TO(msg_ctx1, env), env);
            if(0 == AXIS2_STRCMP(to, to_str))
                continue; 
            SANDESHA2_SENDER_MGR_REMOVE(retrans_mgr, env, 
                        SANDESHA2_SENDER_BEAN_GET_MSG_ID(sender_bean, env));
            ack_rm_msg = sandesha2_msg_init_init_msg(env, msg_ctx1);
            if(SANDESHA2_MSG_TYPE_ACK != SANDESHA2_MSG_CTX_GET_MSG_TYPE(ack_rm_msg, 
                        env))
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Invalid"
                        " ack message entry");
                return AXIS2_FAILURE;
            }
            seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        ack_rm_msg, env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
            SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT, seq_ack);
            SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(rm_msg_ctx, env);
            break;
        }
        
    }
    return AXIS2_SUCCESS;
}
