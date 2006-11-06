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
#include <sandesha2_fault_mgr.h>
#include <sandesha2_constants.h>
#include <axis2_hash.h>
#include <axis2_core_utils.h>
#include <axiom_soap_const.h>
#include <sandesha2_iom_rm_part.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_seq_ack.h>
#include <sandesha2_seq.h>
#include <sandesha2_identifier.h>
#include <sandesha2_msg_number.h>
#include <sandesha2_utils.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_msg_init.h>
#include <stdio.h>
#include <string.h>


/** 
 * @brief Fault Manager struct impl
 *	Sandesha2 Fault Manager
 */
typedef struct sandesha2_fault_mgr_impl sandesha2_fault_mgr_impl_t;  
  
struct sandesha2_fault_mgr_impl
{
    sandesha2_fault_mgr_t mgr;
};

#define SANDESHA2_INTF_TO_IMPL(mgr) \
                        ((sandesha2_fault_mgr_impl_t *)(mgr))

/***************************** Function headers *******************************/
sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_create_seq_refused 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            axis2_msg_ctx_t *create_seq_msg,
            sandesha2_storage_mgr_t *storage_man);
            
sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_last_msg_num_exceeded
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *app_rm_msg,
            sandesha2_storage_mgr_t *storage_man);
            
sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_msg_num_rollover
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *rm_msg_ctx,
            sandesha2_storage_mgr_t *storage_man);

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_unknown_seq
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *rm_msg_ctx,
            axis2_char_t *seq_id,
            sandesha2_storage_mgr_t *storage_man);

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_invalid_ack
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *ack_rm_msg,
            sandesha2_storage_mgr_t *storage_man);

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_seq_closed
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *rm_msg_ctx,
            axis2_char_t *seq_id,
            sandesha2_storage_mgr_t *storage_man);

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_get_fault
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *rm_msg_ctx,
            sandesha2_fault_data_t *fault_data,
            axis2_char_t *addr_ns_uri,
            sandesha2_storage_mgr_t *storage_man);

axis2_status_t AXIS2_CALL 
sandesha2_fault_mgr_free(sandesha2_fault_mgr_t *mgr, 
                        const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_fault_mgr_t* AXIS2_CALL
sandesha2_fault_mgr_create(const axis2_env_t *env)
{
    sandesha2_fault_mgr_impl_t *fault_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    fault_mgr_impl =  (sandesha2_fault_mgr_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_fault_mgr_impl_t));
	
    if(NULL == fault_mgr_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    fault_mgr_impl->mgr.ops = AXIS2_MALLOC(env->allocator,
                        sizeof(sandesha2_fault_mgr_ops_t));
    if(NULL == fault_mgr_impl->mgr.ops)
	{
		sandesha2_fault_mgr_free((sandesha2_fault_mgr_t*)fault_mgr_impl, 
                        env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    fault_mgr_impl->mgr.ops->check_for_create_seq_refused = 
                        sandesha2_fault_mgr_check_for_create_seq_refused;
    fault_mgr_impl->mgr.ops->check_for_last_msg_num_exceeded = 
                        sandesha2_fault_mgr_check_for_last_msg_num_exceeded;
    fault_mgr_impl->mgr.ops->check_for_msg_num_rollover = 
                        sandesha2_fault_mgr_check_for_msg_num_rollover;
    fault_mgr_impl->mgr.ops->check_for_unknown_seq = 
                        sandesha2_fault_mgr_check_for_unknown_seq;
    fault_mgr_impl->mgr.ops->check_for_invalid_ack = 
                        sandesha2_fault_mgr_check_for_invalid_ack;
    fault_mgr_impl->mgr.ops->check_for_seq_closed = 
                        sandesha2_fault_mgr_check_for_seq_closed;
    fault_mgr_impl->mgr.ops->get_fault = 
                        sandesha2_fault_mgr_get_fault;
    fault_mgr_impl->mgr.ops->free = sandesha2_fault_mgr_free;
                        
	return &(fault_mgr_impl->mgr);
}


axis2_status_t AXIS2_CALL 
sandesha2_fault_mgr_free(sandesha2_fault_mgr_t *mgr, 
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(NULL != mgr->ops)
    {
        AXIS2_FREE(env->allocator, mgr->ops);
        mgr->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(mgr));
	return AXIS2_SUCCESS;
}

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_create_seq_refused 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            axis2_msg_ctx_t *create_seq_msg,
            sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    sandesha2_create_seq_t *create_seq = NULL;
    axis2_bool_t refuse_seq = AXIS2_FALSE;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, create_seq_msg, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    
    rm_msg_ctx = sandesha2_msg_init_init_msg(env, create_seq_msg);
    create_seq = (sandesha2_create_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_CREATE_SEQ);
    
    if(NULL == create_seq)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING, 
                        AXIS2_FAILURE);
        return NULL;
    }
    if(AXIS2_TRUE == refuse_seq)
    {
        sandesha2_fault_data_t *fault_data = NULL;
        fault_data = sandesha2_fault_data_create(env);
        SANDESHA2_FAULT_DATA_SET_TYPE(fault_data, env, 
                        SANDESHA2_SOAP_FAULT_TYPE_CREATE_SEQ_REFUSED);
        if(SANDESHA2_SOAP_VERSION_1_1 == sandesha2_utils_get_soap_version(env, 
                        SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(rm_msg_ctx, env)))
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP11_FAULT_CODE_SENDER);
        else
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP12_FAULT_CODE_SENDER);
        SANDESHA2_FAULT_DATA_SET_SUB_CODE(fault_data, env, 
                        SANDESHA2_SOAP_FAULT_SUBCODE_CREATE_SEQ_REFUSED);
        SANDESHA2_FAULT_DATA_SET_REASON(fault_data, env, "");
        return sandesha2_fault_mgr_get_fault(fault_mgr, env, rm_msg_ctx,
                        fault_data, SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(rm_msg_ctx,
                        env), storage_man);
    }
    return NULL;
}
            
sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_last_msg_num_exceeded
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *app_rm_msg,
            sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_t *sequence = NULL;
    long msg_num = -1;
    axis2_char_t *seq_id = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *last_msg_bean = NULL;
    axis2_bool_t exceeded = AXIS2_FALSE;
    axis2_char_t reason[256];
        
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, app_rm_msg, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    
    sequence = (sandesha2_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        app_rm_msg, env, SANDESHA2_MSG_PART_SEQ);
    msg_num = SANDESHA2_MSG_NUMBER_GET_MSG_NUM(SANDESHA2_SEQ_GET_MSG_NUM(
                        sequence, env), env);
    seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_GET_IDENTIFIER(sequence, env), env);
                        
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_man, env);
    last_msg_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr,
                        env, seq_id, SANDESHA2_SEQ_PROP_LAST_OUT_MESSAGE_NO);
    if(NULL != last_msg_bean)
    {
        long last_msg_no = -1;
        last_msg_no = atol(SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(last_msg_bean,
                        env));
        if(msg_num > last_msg_no)
        {
            exceeded = AXIS2_TRUE;
            sprintf(reason, "The message number of the message %ld exceeded the"
                        " last message number %ld which was mentioned as last"
                        " message in a previosly received application message",
                        msg_num, last_msg_no);
        }
    }
    if(AXIS2_TRUE == exceeded)
    {
        sandesha2_fault_data_t *fault_data = NULL;
        fault_data = sandesha2_fault_data_create(env);
        SANDESHA2_FAULT_DATA_SET_TYPE(fault_data, env, 
                        SANDESHA2_SOAP_FAULT_TYPE_LAST_MESSAGE_NO_EXCEEDED);
        if(SANDESHA2_SOAP_VERSION_1_1 == sandesha2_utils_get_soap_version(env, 
                        SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(app_rm_msg, env)))
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP11_FAULT_CODE_SENDER);
        else
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP12_FAULT_CODE_SENDER);
        SANDESHA2_FAULT_DATA_SET_SUB_CODE(fault_data, env, 
                        SANDESHA2_SOAP_FAULT_SUBCODE_LAST_MESSAGE_NO_EXCEEDED);
        SANDESHA2_FAULT_DATA_SET_REASON(fault_data, env, reason);
        return sandesha2_fault_mgr_get_fault(fault_mgr, env, app_rm_msg,
                        fault_data, SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(app_rm_msg,
                        env), storage_man);
    }
    return NULL;    
}
            
sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_msg_num_rollover
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *rm_msg_ctx,
            sandesha2_storage_mgr_t *storage_man)
{
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    
    return NULL;
}

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_unknown_seq
    (sandesha2_fault_mgr_t *fault_mgr,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *seq_id,
    sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    int type = -1;
    axis2_bool_t valid_seq = AXIS2_TRUE;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    AXIS2_PARAM_CHECK(env->error, seq_id, NULL);
    
    create_seq_mgr = SANDESHA2_STORAGE_MGR_GET_CREATE_SEQ_MGR(
                        storage_man, env);
    type = SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, env);
    if(SANDESHA2_MSG_TYPE_ACK == type || 
        SANDESHA2_MSG_TYPE_CREATE_SEQ_RESPONSE == type ||
        SANDESHA2_MSG_TYPE_TERMINATE_SEQ_RESPONSE == type ||
        SANDESHA2_MSG_TYPE_CLOSE_SEQ_RESPONSE == type)
    {
        sandesha2_create_seq_bean_t *find_bean = NULL;
        axis2_array_list_t *list = NULL;
        AXIS2_LOG_INFO(env->log, "[sandesha2] came10 .........");
        find_bean = sandesha2_create_seq_bean_create(env);
        SANDESHA2_CREATE_SEQ_BEAN_SET_SEQ_ID(find_bean, env, seq_id);
        list = SANDESHA2_CREATE_SEQ_MGR_FIND(create_seq_mgr, env, 
                        find_bean);
        if(list)
        {
            if(0 == AXIS2_ARRAY_LIST_SIZE(list, env))
                valid_seq = AXIS2_FALSE;
        }
        else
            valid_seq = AXIS2_FALSE;        
    }
    else
    {
        sandesha2_next_msg_mgr_t *next_msg_mgr = NULL;
        axis2_array_list_t *list = NULL;
        axis2_bool_t contains = AXIS2_FALSE;
        
        next_msg_mgr = SANDESHA2_STORAGE_MGR_GET_NEXT_MSG_MGR(
                        storage_man, env);
        list = SANDESHA2_NEXT_MSG_MGR_RETRIEVE_ALL(next_msg_mgr, env);
        if(list)
        {
            int size = AXIS2_ARRAY_LIST_SIZE(list, env);
            int i = 0;
			for(i = 0; i < size; i++)
            {
                sandesha2_next_msg_bean_t *next_bean = NULL;
                axis2_char_t *tmp_id = NULL;
                
                next_bean = AXIS2_ARRAY_LIST_GET(list, env, i);
                tmp_id = SANDESHA2_NEXT_MSG_BEAN_GET_SEQ_ID(next_bean, env);
                if(0 == AXIS2_STRCMP(seq_id, tmp_id))
                {
                    contains = AXIS2_TRUE;
                    break;
                }
            }
        }
        if(AXIS2_TRUE == contains)
            valid_seq = AXIS2_TRUE;
        else
            valid_seq = AXIS2_FALSE;
    }
    if(!valid_seq)
    {
        sandesha2_fault_data_t *fault_data = NULL;
        axis2_char_t *rm_ns_val = NULL;
        axiom_element_t *detail_ele = NULL;
        axiom_node_t *detail_node = NULL;
        axis2_qname_t *qname = NULL;
        
        fault_data = sandesha2_fault_data_create(env);
        rm_ns_val = SANDESHA2_MSG_CTX_GET_RM_NS_VAL(rm_msg_ctx, env);
        if(SANDESHA2_SOAP_VERSION_1_1 == sandesha2_utils_get_soap_version(env, 
                        SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(rm_msg_ctx, env)))
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP11_FAULT_CODE_SENDER);
        else
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP12_FAULT_CODE_SENDER);
        SANDESHA2_FAULT_DATA_SET_SUB_CODE(fault_data, env, 
                        SANDESHA2_SOAP_FAULT_SUBCODE_UNKNOWN_SEQ);
        qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_IDENTIFIER,
                        rm_ns_val, SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
        detail_ele = axiom_element_create_with_qname(env, NULL, qname, 
                        &detail_node);
        SANDESHA2_FAULT_DATA_SET_DETAIL(fault_data, env, detail_node);
        SANDESHA2_FAULT_DATA_SET_REASON(fault_data, env, "A sequence with the"
                        " given sequenceID has NOT been established");
        return sandesha2_fault_mgr_get_fault(fault_mgr, env, rm_msg_ctx,
                        fault_data, SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(rm_msg_ctx,
                        env), storage_man);
    }
    return NULL;
}

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_invalid_ack
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *ack_rm_msg,
            sandesha2_storage_mgr_t *storage_man)
{
    axis2_bool_t invalid_ack = AXIS2_FALSE;
    axis2_char_t reason[256];
    sandesha2_seq_ack_t *seq_ack = NULL;
    axis2_array_list_t *ack_range_list = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ack_rm_msg, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    
    if(SANDESHA2_MSG_TYPE_ACK != SANDESHA2_MSG_CTX_GET_MSG_TYPE(ack_rm_msg, env))
        return NULL;
    seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(ack_rm_msg, 
                        env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    ack_range_list = SANDESHA2_SEQ_ACK_GET_ACK_RANGE_LIST(seq_ack, env);
    if(NULL != ack_range_list)
    {
        int i = 0;
        for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(ack_range_list, env); i++)
        {
            sandesha2_ack_range_t *ack_range = NULL;
            long upper = -1;
            long lower = -1;
            ack_range = AXIS2_ARRAY_LIST_GET(ack_range_list, env, i);
            lower = SANDESHA2_ACK_RANGE_GET_LOWER_VALUE(ack_range, env);
            upper = SANDESHA2_ACK_RANGE_GET_UPPER_VALUE(ack_range, env);
            if(lower > upper)
            {
                invalid_ack = AXIS2_TRUE;
                sprintf(reason, "The SequenceAcknowledgement is invalid. "
                        "Lower value is larger than upper value");
            }            
        }
    }
    if(AXIS2_TRUE == invalid_ack)
    {
        sandesha2_fault_data_t *fault_data = NULL;
        axis2_char_t *rm_ns_val = NULL;
        axiom_element_t *detail_ele = NULL;
        axiom_node_t *detail_node = NULL;
        axiom_element_t *dummy_ele = NULL;
        axiom_node_t *dummy_node = NULL;
        axis2_qname_t *qname = NULL;
                
        fault_data = sandesha2_fault_data_create(env);
        rm_ns_val = SANDESHA2_MSG_CTX_GET_RM_NS_VAL(ack_rm_msg, env);
        if(SANDESHA2_SOAP_VERSION_1_1 == sandesha2_utils_get_soap_version(env, 
                        SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(ack_rm_msg, env)))
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP11_FAULT_CODE_SENDER);
        else
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP12_FAULT_CODE_SENDER);
        SANDESHA2_FAULT_DATA_SET_SUB_CODE(fault_data, env, 
                        SANDESHA2_SOAP_FAULT_SUBCODE_INVALID_ACKNOWLEDGEMENT);
        dummy_ele = axiom_element_create(env, NULL, "dummy_ele", NULL, 
                        &dummy_node);
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(seq_ack, env, dummy_node);
        qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQ_ACK, 
                        NULL, NULL);
        detail_ele = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(dummy_ele, env,
                        qname, dummy_node, &detail_node);
        SANDESHA2_FAULT_DATA_SET_DETAIL(fault_data, env, detail_node);
        SANDESHA2_FAULT_DATA_SET_REASON(fault_data, env, reason);
        return sandesha2_fault_mgr_get_fault(fault_mgr, env, ack_rm_msg,
                        fault_data, SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(ack_rm_msg,
                        env), storage_man);
    }
    return NULL;
}

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_check_for_seq_closed
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *rm_msg_ctx,
            axis2_char_t *seq_id,
            sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *closed_bean = NULL;
    axis2_bool_t seq_closed = AXIS2_FALSE;
    axis2_char_t reason[256];
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    AXIS2_PARAM_CHECK(env->error, seq_id, NULL);
    
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_man, env);
    closed_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env,
                        seq_id, SANDESHA2_SEQ_PROP_SEQ_CLOSED);
    if(NULL != closed_bean && 0 == AXIS2_STRCMP(SANDESHA2_VALUE_TRUE,
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(closed_bean, env)))
    {
        seq_closed = AXIS2_TRUE;
        sprintf(reason, "The sequence with the id %s was closed previously."
                        " Cannot accept this message", seq_id);
    }
    if(AXIS2_TRUE == seq_closed)
    {
        sandesha2_fault_data_t *fault_data = NULL;
        
        fault_data = sandesha2_fault_data_create(env);
        if(SANDESHA2_SOAP_VERSION_1_1 == sandesha2_utils_get_soap_version(env, 
                        SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(rm_msg_ctx, env)))
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP11_FAULT_CODE_SENDER);
        else
            SANDESHA2_FAULT_DATA_SET_CODE(fault_data, env, 
                        AXIOM_SOAP12_FAULT_CODE_SENDER);
        
        SANDESHA2_FAULT_DATA_SET_REASON(fault_data, env, reason);
        return sandesha2_fault_mgr_get_fault(fault_mgr, env, rm_msg_ctx,
                        fault_data, SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(rm_msg_ctx,
                        env), storage_man);
    }
    return NULL;
}

sandesha2_msg_ctx_t* AXIS2_CALL 
sandesha2_fault_mgr_get_fault
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *rm_msg_ctx,
            sandesha2_fault_data_t *fault_data,
            axis2_char_t *addr_ns_uri,
            sandesha2_storage_mgr_t *storage_man)
{
    axis2_msg_ctx_t *fault_msg_ctx = NULL;
    axis2_msg_ctx_t *ref_msg = NULL;
    axis2_op_t *op = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_char_t *acks_to_str = NULL;
    axis2_char_t *anon_uri = NULL;
    int soap_ver = -1;
    sandesha2_msg_ctx_t *fault_rm_msg = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    AXIS2_PARAM_CHECK(env->error, addr_ns_uri, NULL);
    AXIS2_PARAM_CHECK(env->error, fault_data, NULL);
    
    ref_msg = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    fault_msg_ctx = axis2_core_utils_create_out_msg_ctx(env, ref_msg);
    
    /* Setting context hierachy - be caereful about freeing
     */
    AXIS2_MSG_CTX_SET_SVC_GRP(fault_msg_ctx, env, AXIS2_MSG_CTX_GET_SVC_GRP(
                        ref_msg, env));
    AXIS2_MSG_CTX_SET_SVC(fault_msg_ctx, env, AXIS2_MSG_CTX_GET_SVC(ref_msg, 
                        env));
    AXIS2_MSG_CTX_SET_SVC_GRP_CTX(fault_msg_ctx, env, 
                        AXIS2_MSG_CTX_GET_SVC_GRP_CTX(ref_msg, env));
    AXIS2_MSG_CTX_SET_SVC_CTX(fault_msg_ctx, env, AXIS2_MSG_CTX_GET_SVC_CTX(
                        ref_msg, env));
    AXIS2_MSG_CTX_SET_SVC_GRP_CTX_ID(fault_msg_ctx, env, 
                        AXIS2_MSG_CTX_GET_SVC_GRP_CTX_ID(ref_msg, env));
    AXIS2_MSG_CTX_SET_SVC_CTX_ID(fault_msg_ctx, env, 
                        AXIS2_MSG_CTX_GET_SVC_CTX_ID(ref_msg, env));
    op = axis2_op_create(env);
    AXIS2_OP_SET_MSG_EXCHANGE_PATTERN(op, env, AXIS2_MEP_URI_OUT_ONLY);
    op_ctx = axis2_op_ctx_create(env, op, NULL);
    AXIS2_MSG_CTX_SET_OP(fault_msg_ctx, env, op);
    AXIS2_MSG_CTX_SET_OP_CTX(fault_msg_ctx, env, op_ctx);
    
    if(SANDESHA2_MSG_TYPE_CREATE_SEQ == SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, 
                        env))
    {
        sandesha2_create_seq_t *create_seq = NULL;
        axis2_endpoint_ref_t *epr = NULL;
        
        create_seq = (sandesha2_create_seq_t *)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_CREATE_SEQ);
        epr =  SANDESHA2_ADDRESS_GET_EPR(SANDESHA2_ACKS_TO_GET_ADDRESS(
                        SANDESHA2_CREATE_SEQ_GET_ACKS_TO(create_seq, env), 
                        env), env);
        acks_to_str = (axis2_char_t*)AXIS2_ENDPOINT_REF_GET_ADDRESS(epr, env);
    }
    else
    {
        sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
        sandesha2_seq_property_bean_t *acks_to_bean = NULL;
        axis2_char_t *seq_id = NULL;
        
        seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_man, env);
        seq_id = SANDESHA2_FAULT_DATA_GET_SEQ_ID(fault_data, env);
        acks_to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr,
                        env, seq_id, SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
        if(NULL != acks_to_bean)
            acks_to_str = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(acks_to_bean, 
                        env);
    }
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
    if(NULL!= acks_to_str && 0 != AXIS2_STRCMP(acks_to_str, anon_uri))
        AXIS2_MSG_CTX_SET_TO(fault_msg_ctx, env, axis2_endpoint_ref_create(env,
                        acks_to_str));
    soap_ver = sandesha2_utils_get_soap_version(env, 
                        SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(rm_msg_ctx, env));
    /* this method is not implemented. Uncomment this when implemented */
    /* sandesha2_soap_env_creator_add_soap_env(env, fault_msg_ctx, soap_ver, 
                        fault_data, SANDESHA2_MSG_CTX_GET_RM_NS_VAL(rm_msg_ctx,
                        env));*/
    
    fault_rm_msg = sandesha2_msg_init_init_msg(env, fault_msg_ctx);
    return fault_rm_msg;
}
