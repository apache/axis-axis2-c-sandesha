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
 
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_utils.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_acks_to.h>
#include <sandesha2_address.h>

#include <axis2_conf_ctx.h>
#include <axis2_ctx.h>
#include <axis2_msg_ctx.h>
#include <axis2_property.h>
#include <axis2_log.h>
#include <axis2_uuid_gen.h>

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
sandesha2_seq_mgr_set_up_new_seq(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *create_seq_msg, 
        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_char_t *seq_id = NULL;
    axis2_endpoint_ref_t *to = NULL;
    axis2_endpoint_ref_t *reply_to = NULL;
    axis2_endpoint_ref_t *acks_to = NULL;
    sandesha2_acks_to_t *temp_acks_to = NULL;
    sandesha2_address_t *temp_address = NULL;
    sandesha2_create_seq_t *create_seq = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_next_msg_mgr_t *next_msg_mgr = NULL;
    sandesha2_seq_property_bean_t *received_msg_bean = NULL;
    sandesha2_seq_property_bean_t *addressing_ns_bean = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *acks_to_bean = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_bean_t *spec_version_bean = NULL;
    sandesha2_next_msg_bean_t *next_msg_bean = NULL;
    axis2_char_t *addressing_ns_value = NULL;
    axis2_char_t *anonymous_uri = NULL;
    axis2_char_t *create_seq_msg_action = NULL;
    axis2_char_t *msg_rm_ns = NULL;
    axis2_char_t *spec_version = NULL;
    axis2_char_t *address = NULL;

    seq_id = axis2_uuid_gen(env);
    to = SANDESHA2_MSG_CTX_GET_TO(create_seq_msg, env);
    if(!to)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "To is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_IS_NULL, AXIS2_FAILURE);
        return NULL; 
    }
    reply_to = SANDESHA2_MSG_CTX_GET_REPLY_TO(create_seq_msg, env);
    create_seq = (sandesha2_create_seq_t *) SANDESHA2_MSG_CTX_GET_MSG_PART(
            create_seq_msg, env, SANDESHA2_MSG_PART_CREATE_SEQ);
    if(!create_seq)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Create Sequence Part is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CREATE_SEQ_PART_IS_NULL, 
                AXIS2_FAILURE);
        return NULL; 
    }
    temp_acks_to = SANDESHA2_CREATE_SEQ_GET_ACKS_TO(create_seq, env);
    temp_address = SANDESHA2_ACKS_TO_GET_ADDRESS(temp_acks_to, env);
    acks_to = SANDESHA2_ADDRESS_GET_EPR(temp_address, env);
    if(!acks_to)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Acks To is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_ACKS_TO_IS_NULL, 
                AXIS2_FAILURE);
        return NULL; 
    }
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(create_seq_msg, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    received_msg_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id, 
            SANDESHA2_SEQ_PROP_SERVER_COMPLETED_MESSAGES, "");
    /* Setting the addressing version */
    addressing_ns_value = SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(create_seq_msg, env);
    addressing_ns_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id, 
            SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, addressing_ns_value);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, addressing_ns_bean);
    anonymous_uri = sandesha2_spec_specific_consts_get_anon_uri(env, 
            addressing_ns_value); 
    /* If no replyTo value. Send responses as sync. */
    if(reply_to)
    {
        address = AXIS2_ENDPOINT_REF_GET_ADDRESS(reply_to, env);
        to_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id, 
                SANDESHA2_SEQ_PROP_TO_EPR, address);
    }
    else
    {
        to_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id, 
                SANDESHA2_SEQ_PROP_TO_EPR, anonymous_uri);
    }
    address = AXIS2_ENDPOINT_REF_GET_ADDRESS(to, env);
    reply_to_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id, 
                SANDESHA2_SEQ_PROP_REPLY_TO_EPR, address);
    address = AXIS2_ENDPOINT_REF_GET_ADDRESS(acks_to, env);
    acks_to_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id, 
                SANDESHA2_SEQ_PROP_ACKS_TO_EPR, address);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, received_msg_bean);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, reply_to_bean);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, acks_to_bean);
    if(to_bean)
    {
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, to_bean);
    }
    next_msg_mgr = SANDESHA2_STORAGE_MGR_GET_NEXT_MSG_MGR(storage_mgr, env);
    next_msg_bean = sandesha2_next_msg_bean_create_with_data(env, seq_id, 1); 
                                                    /* 1 will be the next */
    SANDESHA2_NEXT_MSG_MGR_INSERT(next_msg_mgr, env, next_msg_bean);

    /* Message to invoke. This will apply for only in-order invocations */
    sandesha2_utils_start_sender_for_seq(env, conf_ctx, seq_id);
    /* Setting the RM Spec version for this sequence */
    create_seq_msg_action = SANDESHA2_MSG_CTX_GET_WSA_ACTION(create_seq_msg, env);
    if(create_seq_msg_action == NULL)
    {
        AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CREATE_SEQ_MSG_DOES_NOT_HAVE_WSA_ACTION_VALUE, 
                AXIS2_FAILURE);
        return NULL;
    }
    msg_rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
            (sandesha2_iom_rm_element_t *) create_seq, env);
    if(0 == AXIS2_STRCMP(SANDESHA2_SPEC_2005_02_NS_URI, msg_rm_ns))
    {
        spec_version = AXIS2_STRDUP(SANDESHA2_SPEC_VERSION_1_0, env);
    }
    else if(0 == AXIS2_STRCMP(SANDESHA2_SPEC_2005_10_NS_URI, msg_rm_ns))
    {
        spec_version = AXIS2_STRDUP(SANDESHA2_SPEC_VERSION_1_1, env);
    }
    else
    {
        AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CREATE_SEQ_MSG_DOES_NOT_HAVE_VALID_RM_NS_VALUE,
                AXIS2_FAILURE);
        return NULL;
    }
    spec_version_bean = sandesha2_seq_property_bean_create(env);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(spec_version_bean, env, seq_id);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(spec_version_bean, env, 
            SANDESHA2_SEQ_PROP_RM_SPEC_VERSION);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(spec_version_bean, env, spec_version);
   
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr,env, spec_version_bean);
    /* TODO Get the SOAP version from the creaet sequence message */
    return seq_id;
}
       
/**
 * Takes the internal_seq_id as the param. Not the seq_id
 * @param internal_seq_id
 * @param config_ctx
 */
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_seq_mgr_update_last_activated_time(
        const axis2_env_t *env,
        axis2_char_t *property_key,
        sandesha2_storage_mgr_t *storage_mgr)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *last_activated_bean = NULL;
    axis2_bool_t added = AXIS2_FALSE;
    long current_time = -1;
    axis2_char_t *current_time_str = NULL;
    
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    last_activated_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
            property_key, SANDESHA2_SEQ_PROP_LAST_ACTIVATED_TIME);
    if(last_activated_bean == NULL)
    {
        added = AXIS2_TRUE;
        last_activated_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_LAST_ACTIVATED_BEAN_SET_SEQ_ID(last_activated_bean, env, 
                property_key);
        SANDESHA2_LAST_ACTIVATED_BEAN_SET_NAME(last_activated_bean, env, 
                SANDESHA2_SEQ_PROP_LAST_ACTIVATED_TIME);
    }
    current_time = sandesha2_utils_get_current_time_in_millis(env);
    sprintf(current_time_str, "%ld", current_time);
    SANDESHA2_LAST_ACTIVATED_BEAN_SET_VALUE(last_activated_bean, env, current_time_str); 
    if(AXIS2_TRUE == added)
    {
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, last_activated_bean);
    }
    else
    {
        SANDESHA2_SEQ_PROPERTY_MGR_UPDATE(seq_prop_mgr, env, last_activated_bean);
    }
    return AXIS2_SUCCESS;
}

