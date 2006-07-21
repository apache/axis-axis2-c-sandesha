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
 
#include <sandesha2_msg_creator.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_utils.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_create_seq_res.h>
#include <sandesha2_close_seq.h>
#include <sandesha2_close_seq_res.h>
#include <sandesha2_acks_to.h>
#include <sandesha2_address.h>
#include <sandesha2_seq_offer.h>
#include <sandesha2_accept.h>
#include <sandesha2_msg_init.h>
#include "../client/sandesha2_client_constants.h"

#include <axis2_conf_ctx.h>
#include <axis2_ctx.h>
#include <axis2_msg_ctx.h>
#include <axis2_property.h>
#include <axis2_log.h>
#include <axis2_uuid_gen.h>
#include <axis2_addr.h>
#include <axis2_options.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include <axiom_node.h>

  
static axis2_status_t
sandesha2_msg_creator_finalize_creation(
        const axis2_env_t *env,
        axis2_msg_ctx_t *related_msg,
        axis2_msg_ctx_t *new_msg);
 
static axis2_status_t
sandesha2_msg_creator_init_creation(
        const axis2_env_t *env,
        axis2_msg_ctx_t *related_msg,
        axis2_msg_ctx_t *new_msg);

/**
 * Create a new create_seq_msg
 * @param application_rm_msg
 * @param internal_seq_id
 * @param acks_to
 * @return
 */
sandesha2_msg_ctx_t *AXIS2_CALL
sandesha2_msg_creator_create_create_seq_msg(
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
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    /* Creating by copying common contents. (This will not see contexts except
     * for conf_ctx)
     */
    create_seq_op = axis2_op_create(env);
    create_seq_msg_ctx = sandesha2_utils_create_new_related_msg_ctx(env, 
            application_rm_msg, create_seq_op);
    sandesha2_msg_creator_init_creation(env, application_msg_ctx, 
            create_seq_msg_ctx);
    create_seq_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(create_seq_msg_ctx, env);
    create_seq_msg_id = axis2_uuid_gen(env);
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
        axis2_array_list_t *inflow = NULL;

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
    AXIS2_MSG_CTX_SET_TO(create_seq_msg_ctx, env, temp_to);
    temp_reply_to = SANDESHA2_MSG_CTX_GET_REPLY_TO(application_rm_msg, env); 
    AXIS2_MSG_CTX_SET_REPLY_TO(create_seq_msg_ctx, env, temp_reply_to);
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
        axis2_ctx_t *ctx = NULL;

        ctx = AXIS2_MSG_CTX_GET_BASE(application_msg_ctx, env);
        property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
                SANDESHA2_CLIENT_OFFERED_SEQ_ID, AXIS2_FALSE);
        offered_seq = AXIS2_PROPERTY_GET_VALUE(property, env);
        if(offered_seq && 0 != AXIS2_STRCMP("", offered_seq))
        {
            sandesha2_seq_offer_t *offer_part = NULL;
            sandesha2_identifier_t *identifier = NULL;

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
            SANDESHA2_MSG_PART_CREATE_SEQ, (sandesha2_iom_rm_part_t *) 
            create_seq_part);
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

/**
 * Create a new create_seq_response message.
 * @param create_seq_msg
 * @param out_msg
 * @param new_seq_id
 * @return
 */
sandesha2_msg_ctx_t *
sandesha2_msg_creator_create_create_seq_response_msg(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *create_seq_msg,
        axis2_msg_ctx_t *out_msg,
        axis2_char_t *new_seq_id,
        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_ctx_t *ctx = NULL;
    axis2_msg_ctx_t *temp_msg_ctx = NULL;
    axis2_property_t *prop = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_char_t *rm_ns_value = NULL;
    axis2_char_t *addressing_ns_value = NULL;
    axis2_char_t *new_msg_id = NULL;
    axis2_char_t *temp_action = NULL;
    axiom_soap_envelope_t *envelope = NULL;
    axiom_soap_envelope_t *temp_envelope = NULL;
    axiom_soap_body_t *temp_soap_body = NULL;
    axiom_node_t *temp_om_node = NULL;
    sandesha2_create_seq_res_t *response = NULL;
    sandesha2_identifier_t *identifier = NULL;
    sandesha2_seq_offer_t *offer = NULL;
    sandesha2_msg_ctx_t *create_seq_response = NULL;
    sandesha2_iom_rm_element_t *msg_part = NULL;
    sandesha2_create_seq_t *cs = NULL;
    int soap_version = -1;

    temp_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(create_seq_msg, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(temp_msg_ctx, env);
    msg_part = (sandesha2_iom_rm_element_t *) SANDESHA2_MSG_CTX_GET_MSG_PART(
            create_seq_msg, env, SANDESHA2_MSG_PART_CREATE_SEQ);
    cs = (sandesha2_create_seq_t *) msg_part;
    rm_version = sandesha2_utils_get_rm_version(env, new_seq_id, storage_mgr);
    if(rm_version == NULL)
    {
        AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CANNOT_FIND_RM_VERSION_OF_GIVEN_MSG, 
                AXIS2_FAILURE);
        return NULL;
    }
    rm_ns_value = sandesha2_spec_specific_consts_get_rm_ns_val(env, rm_version);
    addressing_ns_value = sandesha2_utils_get_seq_property(env, new_seq_id, 
            SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, storage_mgr);
    response = sandesha2_create_seq_res_create(env, rm_ns_value, 
            addressing_ns_value);
    identifier = sandesha2_identifier_create(env, rm_ns_value);
    SANDESHA2_IDENTIFIER_SET_IDENTIFIER(identifier, env, new_seq_id);
    SANDESHA2_CREATE_SEQ_RES_SET_IDENTIFIER(response, env, identifier);
    offer = SANDESHA2_CREATE_SEQ_GET_SEQ_OFFER(cs, env);
    if(offer)
    {
        axis2_char_t *out_seq_id = NULL;
        sandesha2_identifier_t *temp_identifier = NULL;
        
        temp_identifier = SANDESHA2_SEQ_OFFER_GET_IDENTIFIER(offer, env);
        out_seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(temp_identifier, env);
        if(out_seq_id && 0 != AXIS2_STRCMP("", out_seq_id))
        {
            sandesha2_accept_t *accept = NULL;
            axis2_endpoint_ref_t *acks_to_epr = NULL;
            sandesha2_acks_to_t *acks_to = NULL;
            sandesha2_address_t *address = NULL;

            accept = sandesha2_accept_create(env, rm_ns_value, addressing_ns_value);
            acks_to_epr = SANDESHA2_MSG_CTX_GET_TO(create_seq_msg, env);
            address = sandesha2_address_create(env, addressing_ns_value, acks_to_epr);
            acks_to = sandesha2_acks_to_create(env, address, rm_ns_value, addressing_ns_value);
            SANDESHA2_ADDRESS_SET_EPR(address, env, acks_to_epr);
            SANDESHA2_ACKS2_TO_SET_ADDRESS(acks_to, env, address);
            SANDESHA2_ACCEPT_SET_ACKS_TO(accept, env, acks_to);
            SANDESHA2_CREATE_SEQ_RES_SET_ACCEPT(response, env, accept);
        }
    }
    temp_envelope = SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(create_seq_msg, env); 
    soap_version = sandesha2_utils_get_soap_version(env, temp_envelope);
    envelope = axiom_soap_envelope_create_default_soap_envelope(env, soap_version);
    temp_soap_body = AXIOM_SOAP_ENVELOPE_GET_BODY(envelope, env);
    temp_om_node = AXIOM_SOAP_BODY_GET_BASE_NODE(temp_soap_body, env);
    SANDESHA2_IMO_RM_ELEMENT_TO_OM_NODE((sandesha2_iom_rm_element_t *) response, 
            env, temp_om_node);
    temp_action = sandesha2_spec_specific_consts_get_create_seq_res_action(
            env, rm_version);
    AXIS2_MSG_CTX_SET_WSA_ACTION(out_msg, env, temp_action);
    temp_action = 
        sandesha2_spec_specific_consts_get_create_seq_response_soap_action(env, 
                rm_version);
    AXIS2_MSG_CTX_SET_SOAP_ACTION(out_msg, env, temp_action);
    ctx = AXIS2_MSG_CTX_GET_BASE(out_msg, env);
    prop = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(prop, env, AXIS2_SCOPE_APPLICATION);
    AXIS2_PROPERTY_SET_VALUE(prop, env, addressing_ns_value);
    AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_WSA_VERSION, prop, AXIS2_FALSE);
    new_msg_id = axis2_uuid_gen(env);
    AXIS2_MSG_CTX_SET_MSG_ID(out_msg, env, new_msg_id);
    AXIS2_MSG_CTX_SET_ENVELOPE(out_msg, env, envelope);
    temp_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(create_seq_msg, env);
    sandesha2_msg_creator_init_creation(env, temp_msg_ctx,out_msg);
    create_seq_response = sandesha2_msg_init_init_msg(env, out_msg);
    SANDESHA2_MSG_CTX_SET_MSG_PART(create_seq_response, env, 
            SANDESHA2_MSG_PART_CREATE_SEQ_RESPONSE, (sandesha2_iom_rm_part_t *) 
            response);
    temp_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(create_seq_msg, env);
    sandesha2_msg_creator_finalize_creation(env, temp_msg_ctx, out_msg);
    AXIS2_MSG_CTX_SET_SERVER_SIDE(temp_msg_ctx, env, AXIS2_TRUE);
    return create_seq_response;
}
 
/**
 * Create a new close_seq_response message.
 * @param close_seq_msg
 * @param out_msg
 * @param storage_mgr
 * @return
 */
sandesha2_msg_ctx_t *
sandesha2_msg_creator_create_close_seq_response_msg(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *close_seq_msg,
        axis2_msg_ctx_t *out_msg,
        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_msg_ctx_t *temp_msg_ctx = NULL;
    axis2_char_t *rm_version = NULL;
    axis2_char_t *ns = NULL;
    axis2_char_t *temp_action = NULL;
    axis2_char_t *seq_id = NULL;
    axiom_soap_envelope_t *envelope = NULL;
    axiom_soap_envelope_t *temp_envelope = NULL;
    sandesha2_close_seq_t *cs = NULL;
    sandesha2_msg_ctx_t *close_seq_response = NULL;
    sandesha2_close_seq_res_t *response = NULL;
    sandesha2_identifier_t *identifier = NULL;
    sandesha2_identifier_t *temp_identifier = NULL;
    int soap_version = -1;

    close_seq_response = sandesha2_msg_ctx_create(env, out_msg);
    temp_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(close_seq_msg, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(temp_msg_ctx, env);
    cs = (sandesha2_close_seq_t *) SANDESHA2_MSG_CTX_GET_MSG_PART(
            close_seq_msg, env, SANDESHA2_MSG_PART_CLOSE_SEQ);
    temp_identifier = SANDESHA2_CLOSE_SEQ_GET_IDENTIFIER(cs, env);
    seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(temp_identifier, env);
    ns = SANDESHA2_MSG_CTX_GET_RM_NS_VAL(close_seq_msg, env);
    SANDESHA2_MSG_CTX_SET_RM_NS_VAL(close_seq_response, env, ns);
    response = sandesha2_close_seq_res_create(env, ns);
    identifier = sandesha2_identifier_create(env, ns);
    SANDESHA2_IDENTIFIER_SET_IDENTIFIER(identifier, env, seq_id);
    SANDESHA2_CLOSE_SEQ_RES_SET_IDENTIFIER(response, env, identifier);
    temp_envelope = SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(close_seq_msg, env); 
    soap_version = sandesha2_utils_get_soap_version(env, temp_envelope);
    envelope = axiom_soap_envelope_create_default_soap_envelope(env, soap_version);
    SANDESHA2_MSG_CTX_SET_ENVELOPE(close_seq_response, env, envelope);
    SANDESHA2_MSG_CTX_SET_SET_MSG_PART(close_seq_response, env, 
            SANDESHA2_MSG_PART_CLOSE_SEQ_RESPONSE, response);
    rm_version = sandesha2_utils_get_rm_version(env, seq_id, storage_mgr);
    temp_action = sandesha2_spec_specific_consts_get_close_seq_res_action(
            env, rm_version);
    AXIS2_MSG_CTX_SET_WSA_ACTION(out_msg, env, temp_action);
    temp_action = 
        sandesha2_spec_specific_consts_get_close_seq_res_action(env, 
                rm_version);
    AXIS2_MSG_CTX_SET_SOAP_ACTION(out_msg, env, temp_action);


    temp_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(close_seq_msg, env);
    sandesha2_msg_creator_init_creation(env, temp_msg_ctx,out_msg);
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(close_seq_response, env);
    sandesha2_msg_creator_finalize_creation(env, temp_msg_ctx, out_msg);
    AXIS2_MSG_CTX_SET_SERVER_SIDE(temp_msg_ctx, env, AXIS2_TRUE);
    return close_seq_response;
}
  
/**
 * Create a new create_terminate_seq_msg
 * @param application_rm_msg
 * @param seq_id
 * @param internal_seq_id
 * @param acks_to
 * @return
 */
sandesha2_msg_ctx_t *AXIS2_CALL
sandesha2_msg_creator_create_terminate_seq_msg(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *ref_rm_msg, 
        axis2_char_t *seq_id,
        axis2_char_t *internal_seq_id,
        axis2_char_t *acks_to,
        sandesha2_storage_mgr_t *storage_mgr)
{
    axis2_msg_ctx_t *ref_msg_ctx = NULL;
    axis2_msg_ctx_t *terminate_seq_msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_op_t *terminate_seq_op = NULL;
    axis2_op_t *app_msg_op_desc = NULL;
    axis2_op_ctx_t *create_seq_op_ctx = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_char_t *terminate_seq_msg_id = NULL;
    axis2_char_t *temp_msg_id = NULL;
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
    axis2_bool_t is_seq_res_reqd = AXIS2_FALSE;
    axiom_soap_envelope_t *envelope = NULL;
    axiom_soap_envelope_t *temp_envelope = NULL;
    sandesha2_create_seq_t *create_seq_part = NULL;
    sandesha2_seq_property_bean_t *reply_to_bean = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_msg_ctx_t *terminate_rm_msg = NULL;
    sandesha2_address_t *temp_address = NULL;
    sandesha2_acks_to_t *temp_acks_to = NULL;

    ref_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(ref_rm_msg, env);
    if(ref_msg_ctx == NULL)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX,, 
                AXIS2_FAILURE);
        return NULL;
    }
    terminate_seq_op = axis2_op_create(env);
    if(terminate_seq_op == NULL)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(ref_msg_ctx, env);
    if(conf_ctx == NULL)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return NULL;
    }
    terminate_seq_msg_ctx = sandesha2_utils_create_new_related_msg_ctx(env, 
            ref_rm_msg, terminate_seq_op);
    terminate_seq_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(terminate_seq_msg_ctx, env);
    terminate_seq_msg_id = AXIS2_MSG_CTX_GET_MSG_ID(terminate_seq_msg_ctx, env);
    AXIS2_CONF_CTX_REGISTER_OP_CTX(conf_ctx, env, terminate_seq_msg_id, 
            terminate_seq_op_ctx);
    ref_msg_op_desc = AXIS2_MSG_CTX_GET_OP(ref_msg_ctx, env);
    terminate_seq_op = AXIS2_MSG_CTX_GET_OP(terminate_seq_msg_ctx, env);
    if(ref_msg_op_desc)
    {
        axis2_array_list_t *outflow = NULL;
        axis2_array_list_t *out_fault_flow = NULL;
        axis2_array_list_t *in_fault_flow = NULL;
        axis2_array_list_t *inflow = NULL;

        outflow = AXIS2_OP_GET_PHASES_OUTFLOW(ref_msg_op_desc, env);
        AXIS2_OP_SET_PHASES_OUTFLOW(terminate_seq_op, env, outflow);
        out_fault_flow = AXIS2_OP_GET_PHASES_OUT_FAULT_FLOW(ref_msg_op_desc, env);
        AXIS2_OP_SET_PHASES_OUT_FAULT_FLOW(terminate_seq_op, env, out_fault_flow);
        in_fault_flow = AXIS2_OP_GET_PHASES_IN_FAULT_FLOW(ref_msg_op_desc, env);
        AXIS2_OP_SET_PHASES_IN_FAULT_FLOW(terminate_seq_op, env, in_fault_flow);
        inflow = AXIS2_OP_GET_REMAINING_PHASES_INFLOW(ref_msg_op_desc, env);
        AXIS2_OP_SET_REMAINING_PHASES_INFLOW(terminate_seq_op, env, inflow);
    }
    rm_version = sandesha2_utils_get_rm_version(env, internal_seq_id, storage_mgr);
    if(rm_version == NULL)
    {
        AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CANNOT_FIND_RM_VERSION_OF_GIVEN_MSG, AXIS2_FAILURE);
        return NULL;
    }
    rm_ns_value = sandesha2_spec_specific_consts_get_rm_ns_val(env, rm_version);
    sandesha2_msg_creator_init_creation(env, ref_msg_ctx, terminate_seq_msg_ctx);
    is_seq_res_reqd = sandesha2_spec_specific_consts_is_term_seq_res_reqd(env, rm_version);

    if(AXIS2_TRUE != is_seq_res_reqd)
    {
        AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_TRANSPORT_IN, NULL, AXIS2_FALSE);
    }
    terminate_rm_msg = sandesha2_msg_init_init_msg(env, terminate_msg);
    if(terminate_rm_msg == NULL)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_MSG_CTX, AXIS2_FAILURE);
        return NULL;
    }
    temp_msg_id = axis2_uuid_gen(env);
    AXIS2_MSG_SET_MSG_ID(terminate_msg, env, temp_msg_id);
    ref_msg_op = AXIS2_MSG_CTX_GET_OP(ref_msg, env);
    if(ref_msg_op)
    {
        axis2_array_list_t *outphases = NULL;

        outphases = AXIS2_OP_GET_PHASES_OUTFLOW(ref_msg_op, env);
        if(outphases)
        {
            AXIS2_OP_SET_PHASES_OUTFLOW(terminate_msg_op, env, outphases);
            AXIS2_OP_SET_PHASES_OUT_FAULT_FLOW(terminate_msg_op, env, outphases);
        }
    }
    temp_envelope = SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(ref_msg, env);
    soap_version = sandesha2_utils_get_soap_version(env, temp_envelope);
    envelope = axiom_soap_envelope_create_default_soap_envelope(env, soap_version);
    SANDESHA2_MSG_CTX_SET_SOAP_ENVELOPE(terminate_rm_msg, env, envelope);


}


static axis2_status_t
sandesha2_msg_creator_finalize_creation(
        const axis2_env_t *env,
        axis2_msg_ctx_t *related_msg,
        axis2_msg_ctx_t *new_msg)
{
    axis2_op_t *old_op = NULL;
    axis2_op_ctx_t *old_op_ctx = NULL;
    axis2_bool_t temp_bool = AXIS2_FALSE;
    axis2_hash_t *related_msg_props = NULL;
    axis2_hash_t *new_msg_props = NULL;
    axis2_ctx_t *related_ctx = NULL;
    axis2_ctx_t *new_ctx = NULL;

    temp_bool = AXIS2_MSG_CTX_IS_SERVER_SIDE(related_msg, env);
    AXIS2_MSG_CTX_SET_SERVER_SIDE(new_msg, env, temp_bool);
    /* Adding all parameters from old message to the new one */
    old_op = AXIS2_MSG_CTX_GET_OP(related_msg, env);
    if(old_op)
    {
        axis2_array_list_t *op_params = NULL;

        op_params = AXIS2_OP_GET_PARAMS(old_op, env);
        if(op_params)
        {
            axis2_op_t *new_op = NULL;
            int i = 0, size = 0;

            new_op = AXIS2_MSG_CTX_GET_OP(new_msg, env);
            size = AXIS2_ARRAY_LIST_SIZE(op_params, env);
            for(i = 0; i < size; i++)
            {
                axis2_param_t *next_param = NULL;
                axis2_param_t *new_param = NULL;
                axis2_char_t *temp_name = NULL;
                void *temp_value = NULL;

                next_param = (axis2_param_t *) AXIS2_ARRAY_LIST_GET(op_params, 
                        env, i);
                temp_name = AXIS2_PARAM_GET_NAME(next_param, env);
                temp_value = AXIS2_PARAM_GET_VALUE(next_param, env);
                new_param = axis2_param_create(env, temp_name, temp_value);
                new_param->ops->value_free = next_param->ops->value_free;
                AXIS2_OP_SET_PARAM(new_op, env, new_param); 
            }
        }
    }
    /* Operation context properties */
    old_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(related_msg, env);
    if(old_op_ctx)
    {
        axis2_hash_t *old_op_ctx_props = NULL;
        axis2_ctx_t *ctx = NULL;

        ctx = AXIS2_OP_CTX_GET_BASE(old_op_ctx, env);
        old_op_ctx_props = AXIS2_CTX_GET_PROPERTIES(ctx, env);
        if(old_op_ctx_props)
        {
            axis2_op_ctx_t *new_op_ctx = NULL;
            axis2_hash_index_t *i = NULL;

            new_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(new_msg, env);
            for (i = axis2_hash_first (old_op_ctx_props, env); i; i = 
                    axis2_hash_next (env, i))
            {
                void *v = NULL;
                void *k = NULL;
                axis2_char_t *key = NULL;
                axis2_property_t *prop = NULL;
                axis2_ctx_t *ctx = NULL;

                axis2_hash_this (i, &k, NULL, &v);
                key = (axis2_char_t *) k;
                prop = (axis2_property_t *) v;
                ctx = AXIS2_OP_CTX_GET_BASE(new_op_ctx, env);
                AXIS2_CTX_SET_PROPERTY(ctx, env, key, prop, AXIS2_FALSE);
            }
        }
    }
    /* Message Context properties */
    if(related_msg && new_msg)
    {
        axis2_hash_t *old_msg_ctx_props = NULL;
        axis2_ctx_t *ctx = NULL;
        
        ctx = AXIS2_MSG_CTX_GET_BASE(related_msg, env);
        old_msg_ctx_props = AXIS2_CTX_GET_PROPERTIES(ctx, env);
        if(old_msg_ctx_props)
        {
            axis2_hash_index_t *i = NULL;

            for (i = axis2_hash_first (old_msg_ctx_props, env); i; i = 
                    axis2_hash_next (env, i))
            {
                void *v = NULL;
                void *k = NULL;
                axis2_char_t *key = NULL;
                axis2_property_t *prop = NULL;
                axis2_ctx_t *ctx = NULL;

                axis2_hash_this (i, &k, NULL, &v);
                key = (axis2_char_t *) k;
                prop = (axis2_property_t *) v;
                ctx = AXIS2_MSG_CTX_GET_BASE(new_msg, env);
                AXIS2_CTX_SET_PROPERTY(ctx, env, key, prop, AXIS2_FALSE);
            }
        }
    }
    /* Setting options with properties copied from the old one */

    related_ctx = AXIS2_MSG_CTX_GET_BASE(related_msg, env);
    related_msg_props = AXIS2_CTX_GET_PROPERTIES(related_ctx, env);
    new_ctx = AXIS2_MSG_CTX_GET_BASE(new_msg, env);
    new_msg_props = AXIS2_CTX_GET_PROPERTIES(new_ctx, env);
    if(!new_msg_props)
    {
        new_msg_props = axis2_hash_make(env);
    }
    if(related_msg_props)
    {
        axis2_hash_index_t *i = NULL;

        for (i = axis2_hash_first (related_msg_props, env); i; i = 
                axis2_hash_next (env, i))
        {
            void *v = NULL;
            void *k = NULL;
            axis2_char_t *key = NULL;
            axis2_property_t *prop = NULL;

            axis2_hash_this (i, &k, NULL, &v);
            key = (axis2_char_t *) k;
            prop = (axis2_property_t *) v;
            axis2_hash_set(new_msg_props, key, AXIS2_HASH_KEY_STRING, prop);
        }
    }
    return AXIS2_SUCCESS;
}
 
static axis2_status_t
sandesha2_msg_creator_init_creation(
        const axis2_env_t *env,
        axis2_msg_ctx_t *related_msg,
        axis2_msg_ctx_t *new_msg)
{
    axis2_svc_t *related_svc = NULL;
    axis2_svc_t *new_svc = NULL;

    related_svc = AXIS2_MSG_CTX_GET_SVC(related_msg, env);
    new_svc = AXIS2_MSG_CTX_GET_SVC(new_msg, env);
    if(related_svc && new_svc && related_svc != new_svc)
    {
        axis2_param_t *ref_policy_param = NULL;

        ref_policy_param = AXIS2_SVC_GET_PARAM(related_svc, env, SANDESHA2_SANDESHA_PROPERTY_BEAN);
        if(ref_policy_param)
        {
            void *value = NULL;
            axis2_param_t *new_policy_param = NULL;

            value = AXIS2_PARAM_GET_VALUE(ref_policy_param, env);
            new_policy_param = axis2_param_create(env, 
                    SANDESHA2_SANDESHA_PROPERTY_BEAN, value);
        }
        
    }
    return AXIS2_SUCCESS;
}

