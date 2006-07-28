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
#include <sandesha2/sandesha2_utils.h>
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
#include <axis2_msg_ctx.h>
#include <axis2_addr.h>
#include "../client/sandesha2_client_constants.h"
#include <axis2_options.h>
#include <axis2_listener_manager.h>
#include <axis2_ctx.h>

/******************************************************************************/
long AXIS2_CALL
sandesha2_seq_mgr_get_last_activated_time(const axis2_env_t *env,
        axis2_char_t *property_key,
        sandesha2_storage_mgr_t *storage_mgr);

/******************************************************************************/

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
    axis2_char_t current_time_str[32];
    
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    last_activated_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
            property_key, SANDESHA2_SEQ_PROP_LAST_ACTIVATED_TIME);
    if(last_activated_bean == NULL)
    {
        added = AXIS2_TRUE;
        last_activated_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(last_activated_bean, env, 
                property_key);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(last_activated_bean, env, 
                SANDESHA2_SEQ_PROP_LAST_ACTIVATED_TIME);
    }
    current_time = sandesha2_utils_get_current_time_in_millis(env);
    sprintf(current_time_str, "%ld", current_time);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(last_activated_bean, env, current_time_str); 
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

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_seq_mgr_has_seq_timedout(const axis2_env_t *env,
        axis2_char_t *property_key,
        sandesha2_msg_ctx_t *rm_msg_ctx,
        sandesha2_storage_mgr_t *storage_mgr)
{
    sandesha2_property_bean_t *property_bean = NULL;
    axis2_bool_t seq_timedout = AXIS2_FALSE;
    long last_activated_time = -1;
    long current_time = -1;
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, property_key, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FALSE);
    
    property_bean = sandesha2_utils_get_property_bean_from_op(env, 
                        AXIS2_MSG_CTX_GET_OP(SANDESHA2_MSG_CTX_GET_MSG_CTX(
                        rm_msg_ctx, env), env));
    if(SANDESHA2_PROPERTY_BEAN_GET_INACTIVE_TIMEOUT_INTERVAL(property_bean, env)
                        <= 0)
        return AXIS2_FALSE;
    last_activated_time = sandesha2_seq_mgr_get_last_activated_time(env, 
                        property_key, storage_mgr);
    current_time = sandesha2_utils_get_current_time_in_millis(env);
    if(last_activated_time > 0 && (last_activated_time + 
                        SANDESHA2_PROPERTY_BEAN_GET_INACTIVE_TIMEOUT_INTERVAL(
                        property_bean, env) < current_time))
        seq_timedout = AXIS2_TRUE;
    return seq_timedout;
}


long AXIS2_CALL
sandesha2_seq_mgr_get_last_activated_time(const axis2_env_t *env,
        axis2_char_t *property_key,
        sandesha2_storage_mgr_t *storage_mgr)
{
    long last_activated_time = -1;
    
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *seq_prop_bean = NULL;
    
    AXIS2_ENV_CHECK(env, -1);
    AXIS2_PARAM_CHECK(env->error, property_key, -1);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, -1);
    
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    seq_prop_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        property_key, SANDESHA2_SEQ_PROP_LAST_ACTIVATED_TIME);
    if(NULL != seq_prop_bean)
    {
        axis2_char_t *value = NULL;
        
        value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(seq_prop_bean, env);
        if(NULL != value)
            last_activated_time = atol(value);
    }
    return last_activated_time;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_seq_mgr_setup_new_client_seq(const axis2_env_t *env,
                        axis2_msg_ctx_t *first_app_msg,
                        axis2_char_t *int_seq_id,
                        axis2_char_t *spec_version,
                        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    axis2_char_t *addr_ns_val = NULL;
    axis2_property_t *property = NULL;
    sandesha2_seq_property_bean_t *addr_ns_bean = NULL;
    axis2_char_t *anon_uri = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_char_t *acks_to_str = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *acks_to_bean = NULL;
    axis2_ctx_t *ctx = NULL;
    /*axis2_options_t *options = NULL;*/
    axis2_char_t *transport_in_protocol = NULL;
    axis2_listener_manager_t *listener_manager = NULL;
    sandesha2_seq_property_bean_t *msgs_bean = NULL;
    axis2_char_t *transport_to = NULL;
    sandesha2_seq_property_bean_t *spec_version_bean = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, first_app_msg, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, int_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, spec_version, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_mgr, AXIS2_FAILURE);
    
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(first_app_msg, env);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(first_app_msg, env, AXIS2_WSA_VERSION,
                        AXIS2_FALSE);
    if(NULL != property)
        addr_ns_val = AXIS2_PROPERTY_GET_VALUE(property, env);
    if(NULL == addr_ns_val)
    {
        axis2_op_ctx_t *op_ctx = NULL;
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(first_app_msg, env);
        req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(op_ctx, env, 
                        AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
        if(NULL != req_msg_ctx)
        {
            property = AXIS2_MSG_CTX_GET_PROPERTY(req_msg_ctx, env, 
                        AXIS2_WSA_VERSION, AXIS2_FALSE);
            if(NULL != property)
                addr_ns_val = AXIS2_PROPERTY_GET_VALUE(property, env);
        }
    }
    if(NULL == addr_ns_val)
        addr_ns_val = AXIS2_WSA_NAMESPACE;
        
    addr_ns_bean = sandesha2_seq_property_bean_create_with_data(env, 
                        int_seq_id, 
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE,
                        addr_ns_val);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, addr_ns_bean);
    anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_val);
    
    to_epr = AXIS2_MSG_CTX_GET_TO(first_app_msg, env);
    property = AXIS2_MSG_CTX_GET_PROPERTY(first_app_msg, env, 
                        SANDESHA2_CLIENT_ACKS_TO, AXIS2_FALSE);
    if(NULL != property)
        acks_to_str = AXIS2_PROPERTY_GET_VALUE(property, env);
    if(NULL == to_epr)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] WSA To is NULL");
        return AXIS2_FAILURE;
    }
    
    to_bean = sandesha2_seq_property_bean_create_with_data(env, int_seq_id,
                       SANDESHA2_SEQ_PROP_TO_EPR, 
                       AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env));
    
    if(AXIS2_TRUE == AXIS2_MSG_CTX_GET_SERVER_SIDE(first_app_msg, env))
    {
        axis2_op_ctx_t *op_ctx = NULL;
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        axis2_endpoint_ref_t *reply_to_epr = NULL;
        
        op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(first_app_msg, env);
        req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(op_ctx, env, 
                        AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
        if(NULL == req_msg_ctx)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Cannot find"
                        " the request message from the operation context");
            return AXIS2_FAILURE;
        }
        reply_to_epr = AXIS2_MSG_CTX_GET_TO(req_msg_ctx, env);
        if(NULL != reply_to_epr)
            reply_to_bean = sandesha2_seq_property_bean_create_with_data(env,
                        int_seq_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR,
                        AXIS2_ENDPOINT_REF_GET_ADDRESS(reply_to_epr, env));
        else
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Cannot get"
                        " request message from the operation context");
            return AXIS2_FAILURE;
        }        
    }
    if(NULL == acks_to_str)
        acks_to_str = anon_uri;
    
    acks_to_bean = sandesha2_seq_property_bean_create_with_data(env,
                        int_seq_id, SANDESHA2_SEQ_PROP_ACKS_TO_EPR,
                        acks_to_str);
    if(AXIS2_FALSE == AXIS2_MSG_CTX_GET_SERVER_SIDE(first_app_msg, env) &&
                        0 != AXIS2_STRCMP(acks_to_str, anon_uri))
    {
        ctx = AXIS2_MSG_CTX_GET_BASE(first_app_msg, env);
        /* TODO currently axis2 doesn't expose the *options* at msg ctx level.
         * Once it is finished change to get from there
         */
        /*options = AXIS2_CTX_GET_OPTIONS(ctx, env);
        transport_in_protocol = AXIS2_OPTIONS_GET_TRANSPORT_IN_PROTOCOL(options, env);*/
        if (NULL == transport_in_protocol)
            transport_in_protocol = AXIS2_TRANSPORT_HTTP;
            
        AXIS2_LISTNER_MANAGER_MAKE_SURE_STARTED(listener_manager, env, 
                        transport_in_protocol, conf_ctx);
    }
    
    msgs_bean = sandesha2_seq_property_bean_create_with_data(env, int_seq_id, 
                        SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES, "");
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, msgs_bean);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, to_bean);
    
    if(NULL != acks_to_bean)
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, acks_to_bean);
    if(NULL != reply_to_bean)
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, reply_to_bean);
        
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(first_app_msg, env, AXIS2_TRANSPORT_URL,
                        AXIS2_FALSE);
    if(NULL != property)
        transport_to = AXIS2_PROPERTY_GET_VALUE(property, env);
    
    if(NULL != transport_to)
    {
        sandesha2_seq_property_bean_t *transport_to_bean = NULL;
        transport_to_bean = sandesha2_seq_property_bean_create_with_data(env,
                        int_seq_id, SANDESHA2_SEQ_PROP_TRANSPORT_TO,
                        transport_to);
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, transport_to_bean);
    }
    spec_version_bean = sandesha2_seq_property_bean_create_with_data(env,
                        int_seq_id, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION,
                        spec_version);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, spec_version_bean);
    sandesha2_seq_mgr_update_last_activated_time(env, int_seq_id, storage_mgr);
    sandesha2_utils_start_sender_for_seq(env, conf_ctx, int_seq_id);
    sandesha2_seq_mgr_update_client_side_listener_if_needed(env, first_app_msg,
                        anon_uri);
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_seq_mgr_update_client_side_listener_if_needed(const axis2_env_t *env,
                        axis2_msg_ctx_t *msg_ctx, axis2_char_t *addr_anon_uri)
{
    axis2_char_t *transport_in_protocol = NULL;
    axis2_ctx_t *ctx = NULL;
    /*axis2_options_t *options = NULL;*/
    axis2_char_t *acks_to = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, addr_anon_uri, AXIS2_FAILURE);
    
    if(AXIS2_FALSE == AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env))
        return AXIS2_SUCCESS;
    
    ctx = AXIS2_MSG_CTX_GET_BASE(msg_ctx, env);
    /* TODO currently axis2 doesn't expose the *options* at msg ctx level.
     * Once it is finished change to get from there
     */
    /* options = AXIS2_CTX_GET_OPTIONS(ctx, env);
    transport_in_protocol = AXIS2_OPTIONS_GET_TRANSPORT_IN_PROTOCOL(options, env);*/
    transport_in_protocol = AXIS2_TRANSPORT_HTTP;
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, SANDESHA2_CLIENT_ACKS_TO,
                        AXIS2_FALSE);
    if(NULL != property)
        acks_to = AXIS2_PROPERTY_GET_VALUE(property, env);
    
    
    return AXIS2_SUCCESS;
}
