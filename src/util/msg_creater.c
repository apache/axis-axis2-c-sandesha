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
 
#include <sandesha2_msg_creater.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_utils.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_acks_to.h>
#include <sandesha2_address.h>
#include <sandesha2_seq_offer.h>

#include <axis2_conf_ctx.h>
#include <axis2_ctx.h>
#include <axis2_msg_ctx.h>
#include <axis2_property.h>
#include <axis2_log.h>
#include <axis2_uuid_gen.h>

/**
 * Create a new create_seq_msg
 * @param application_rm_msg
 * @param internal_seq_id
 * @param acks_to
 * @return
 */
sandesha2_msg_ctx_t *AXIS2_CALL
sandesha2_msg_creater_create_create_seq_msg(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *application_rm_msg, 
        axis2_char_t *internal_seq_id,
        axis2_char_t *acks_to,
        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_msg_ctx_t *application_msg_ctx = NULL;
    axis2_msg_ctx_t *create_seq_msg_ctx = NULL;
    axis2_conf_ctx_t *ctx = NULL;
    axis2_op_t *create_seq_op = NULL;
    axis2_op_t *app_msg_op_desc = NULL;
    axis2_op_ctx_t *create_seq_op_ctx = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_char_t *create_seq_msg_id = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_char_t *rm_ns_value = NULL;
    axis2_char_t *addressing_ns_value = NULL;
    axis2_char_t *anonymous_uri = NULL;
    axis2_char_t *temp_value = NULL;
    axis2_char_t *temp_action = NULL;
    axis2_char_t *temp_soap_action = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_endpoint_ref_t *reply_to_epr = NULL;
    axis2_endpoint_ref_t *acks_to_epr = NULL;
    axis2_endpoint_ref_t *temp_to = NULL;
    axis2_endpoint_ref_t *temp_reply_to = NULL;
    axis2_qname_t *qname = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_msg_ctx_t *create_seq_rm_msg = NULL;
    sandesha2_address_t *temp_address = NULL;
    sandesha2_acks_to_t *temp_acks_to = NULL;

    application_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(application_rm_msg, env);
    if(application_msg_ctx == NULL)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_APPLICATION_MSG_NULL, 
                AXIS2_FAILURE);
        return NULL;
    }
    ctx = AXIS2_MSG_CTX_GET_CONF_CTX(application_msg_ctx, env);
    if(ctx == NULL)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return NULL;
    }
    seq_pro_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    /* Creating by copying common contents. (This will not see contexts except
     * for conf_ctx)
     */
    create_seq_op = axis2_op_create(env);
    create_seq_msg_ctx = sandesha2_utils_create_new_related_msg_ctx(env, 
            application_rm_msg, create_seq_op);
    sandesha2_msg_creater_init_creation(env, application_msg_ctx, 
            create_seq_msg_ctx);
    create_seq_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(create_seq_msg_ctx, env);
    create_seq_msg_id = sandesha2_uuid_gen(env);
    AXIS2_MSG_CTX_SET_MSG_ID(create_seq_msg_ctx, env, create_seq_msg_id);
    AXIS2_CONF_CTX_REGISTER_OP_CTX(ctx, env, create_seq_msg_id, 
            create_seq_op_ctx);
    app_msg_op_desc = AXIS2_MSG_CTX_GET_OP(application_msg_ctx, env);
    create_seq_op = AXIS2_MSG_CTX_GET_OP(create_seq_msg_ctx, env);
    qname = axis2_qname_create(env, "CreateSequenceOperation", NULL, NULL);
    AXIS2_OP_SET_QNAME(create_seq_op, env, qname);
    AXIS2_QNAME_FREE(qname, env);
    if(app_msg_op_desc)
    {
        axis2_array_list_t *outflow = NULL;
        axis2_array_list_t *out_fault_flow = NULL;
        axis2_array_list_t *in_fault_flow = NULL;
        axis2_array_list_t *in_flow = NULL;

        outflow = AXIS2_OP_GET_PHASES_OUTFLOW(app_msg_op_desc, env);
        AXIS2_OP_SET_PHASES_OUTFLOW(create_seq_op, env, outflow);
        out_fault_flow = AXIS2_OP_GET_PHASES_OUT_FAULT_FLOW(app_msg_op_desc, env);
        AXIS2_OP_SET_PHASES_OUT_FAULT_FLOW(create_seq_op, env, out_fault_flow);
        in_fault_flow = AXIS2_OP_GET_PHASES_IN_FAULT_FLOW(app_msg_op_desc, env);
        AXIS2_OP_SET_PHASES_IN_FAULT_FLOW(create_seq_op, env, in_fault_flow);
        inflow = AXIS2_OP_GET_REMAINING_PHASES_INFLOW(app_msg_op_desc, env);
        AXIS2_OP_SET_REMAINING_PHASES_INFLOW(create_seq_op, env, inflow);
    }
    AXIS2_MSG_CTX_SET_OP(create_seq_msg_ctx, env, create_seq_op);
    temp_to = SANDESHA2_MSG_CTX_GET_TO(application_rm_msg, env); 
    AXIS2_MSG_CTX_SET_TO(create_seq_msg_ctx, env, create_seq_op, temp_to);
    temp_reply_to = SANDESHA2_MSG_CTX_GET_REPLY_TO(application_rm_msg, env); 
    AXIS2_MSG_CTX_SET_REPLY_TO(create_seq_msg_ctx, env, create_seq_op, temp_reply_to);
    create_seq_rm_msg = sandesha2_msg_ctx_create(env, create_seq_msg_ctx);
    rm_version = sandesha2_utils_get_rm_version(env, internal_seq_id, storage_mgr);
    if(rm_version == NULL)
    {
        AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CANNOT_FIND_RM_VERSION_OF_GIVEN_MSG, AXIS2_FAILURE);
        return NULL;
    }
    rm_ns_value = sandesha2_spec_specific_consts_get_rm_ns_val(env, rm_version);
    addressing_ns_value = sandesha2_utils_get_seq_property(env, internal_seq_id, 
            SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, storage_mgr);
    create_seq_part = sandesha2_create_seq_create(env, rm_ns_value, 
            addressing_ns_value);
    /* Adding sequence offer - if present */
    op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(application_msg_ctx, env);
    if(op_ctx)
    {
        axis2_char_t *offered_seq = NULL;
        axis2_property_t *property = NULL;

        property = AXIS2_MSG_CTX_GET_PROPERTY(application_msg_ctx, env, 
                SANDESHA2_CLIENT_OFFERED_SEQ_ID, AXIS2_FALSE);
        offered_seq = AXIS2_PROPERTY_GET_VALUE(property, env);
        if(offered_seq && 0 != AXIS2_STRCMP("", offered_seq))
        {
            sandesha2_seq_offer_t *offer_part = NULL;
            sandesha2_identifer_t *identifier = NULL;

            offer_part = sandesha2_seq_offer_create(env, rm_ns_value);
            identifier = sandesha2_identifier_create(env, rm_ns_value);
            SANDESHA2_IDENTIFIER_SET_IDENTIFER(identifier, env, offered_seq);
            SANDESHA2_SEQ_OFFER_SET_IDENTIFIER(offer_part, env, identifier);
            SANDESHA2_CREATE_SEQ_SET_SEQ_OFFER(create_seq_part, env, offer_part);
        }
    }
    reply_to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
            internal_seq_id, SANDESHA2_SEQ_PROP_REPLY_TO_EPR);
    to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
            internal_seq_id, SANDESHA2_SEQ_PROP_TO_EPR);
    temp_value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(to_bean, env);
    if(to_bean == NULL || NULL == temp_value)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_EPR_NOT_SET, AXIS2_FAILURE);
        return NULL;
    }
    to_epr = axis2_endpoint_ref_create(env, temp_value);
    anonymous_uri = sandesha2_spec_specific_consts_get_anon_uri(env, 
            addressing_ns_value);
    if(acks_to == NULL || 0 == AXIS2_STRCMP("", acks_to))
    {
        acks_to = AXIS2_STRDUP(anonymous_uri, env);
    }
    acks_to_epr = axis2_endpoint_ref_create(env, acks_to);
    temp_value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(reply_to_bean, env);
    if(reply_to_bean && temp_value)
    {
        reply_to_epr = axis2_endpoint_ref_create(env, temp_value);
    }
    temp_to = SANDESHA2_MSG_CTX_GET_TO(create_seq_rm_msg, env);
    if(temp_to == NULL)
        SANDESHA2_MSG_CTX_SET_TO(create_seq_rm_msg, env, to_epr);
    /* ReplyTo will be set only if not NULL */
    if(reply_to_epr)
    {
        SANDESHA2_MSG_CTX_SET_REPLY_TO(create_seq_rm_msg, env, reply_to_epr);
    }
    temp_address = sandesha2_address_create(env, addressing_ns_value, acks_to_epr);
    temp_acks_to = sandesha2_acks_to_create(env, temp_address, rm_ns_value, 
            addressing_ns_value);
    SANDESHA2_CREATE_SEQ_SET_ACKS_TO(create_seq_part, env,  temp_acks_to);
    SANDESHA2_MSG_CTX_SET_MSG_PART(create_seq_rm_msg, env, 
            SANDESHA2_MSG_PART_CREATE_SEQ, create_seq_part);
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(create_seq_rm_msg, env);
    temp_action = sandesha2_spec_specific_consts_get_create_seq_action(env, 
            rm_version);
    SANDESHA2_MSG_CTX_SET_ACTION(create_seq_rm_msg, env, temp_action);
    temp_soap_action = sandesha2_spec_specific_consts_get_create_seq_action(env, 
            rm_version);
    SANDESHA2_MSG_CTX_SET_SOAP_ACTION(create_seq_rm_msg, env, temp_soap_action);
    sandesha2_create_msg_finalize_creation(env, application_msg_ctx, 
            create_seq_msg_ctx);
    return create_seq_rm_msg;
}
    
