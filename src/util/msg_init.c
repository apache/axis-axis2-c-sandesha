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
 
#include <sandesha2_msg_init.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_utils.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_acks_to.h>
#include <sandesha2_address.h>
#include <sandesha2_seq_offer.h>
#include <sandesha2_accept.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_create_seq_res.h>
#include <sandesha2_seq.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_terminate_seq_res.h>
#include <sandesha2_ack_requested.h>
#include <sandesha2_close_seq.h>
#include <sandesha2_close_seq_res.h>
#include <sandesha2_rm_elements.h>
#include "../client/sandesha2_client_constants.h"

#include <axis2_conf_ctx.h>
#include <axis2_ctx.h>
#include <axis2_msg_ctx.h>
#include <axis2_property.h>
#include <axis2_log.h>
#include <axis2_uuid_gen.h>
#include <axis2_addr.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include <axiom_node.h>

/**
 * Adds the message parts to the sandesha2_msg_ctx.
 * 
 * @param msg_ctx
 * @param rm_msg_ctx
 */
static axis2_status_t
populate_rm_msg_ctx(
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx,
    sandesha2_msg_ctx_t *rm_msg_ctx);

/**
 * This is used to validate the message.
 * Also set an Message type. Possible types are given in the sandesha2_constants
 * 
 * @param rm_msg_ctx
 * @return
 */
static axis2_bool_t validate_msg(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx);

static void add_op_if_null(
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx);

/**
 * Called to create a rm_msg_ctx out of an message context. Finds out things 
 * like rm version and message type as well.
 * 
 * @param ctx
 * @param assumed_rm_ns
 * this is used for validation (to find out weather the rm_ns of the current 
 * message is equal to the regietered rm_ns of the sequence). 
 * If NULL validation will not happen.
 * 
 * @return
 */
sandesha2_msg_ctx_t *
sandesha2_msg_init_init_msg(
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx)
{
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;

    rm_msg_ctx = sandesha2_msg_ctx_create(env, msg_ctx);
    populate_rm_msg_ctx(env, msg_ctx, rm_msg_ctx);
    validate_msg(env, rm_msg_ctx);
    return rm_msg_ctx;
}

static axis2_status_t
populate_rm_msg_ctx(
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_char_t *addressing_ns = NULL;
    axis2_char_t *addressing_ns_value = NULL;
    axis2_char_t *rm_ns = NULL;
    axis2_char_t *action = NULL;
    axis2_property_t *prop = NULL;
    axis2_ctx_t *ctx = NULL;
    axiom_soap_envelope_t *envelope = NULL;
    sandesha2_rm_elements_t *elements = NULL;
    sandesha2_create_seq_t *create_seq = NULL;
    sandesha2_create_seq_res_t *create_seq_res = NULL;
    sandesha2_seq_t *seq = NULL;
    sandesha2_seq_ack_t *seq_ack = NULL;
    sandesha2_terminate_seq_t *terminate_seq = NULL;
    sandesha2_terminate_seq_res_t *terminate_seq_res = NULL;
    sandesha2_ack_requested_t *ack_request = NULL;
    sandesha2_close_seq_t *close_seq = NULL;
    sandesha2_close_seq_res_t *close_seq_res = NULL;
    
    /* If client side and the addressing version is not set. 
     * Assuming the default addressing version.
     */
    ctx = AXIS2_MSG_CTX_GET_BASE(msg_ctx, env);
    prop = AXIS2_CTX_GET_PROPERTY(ctx, env, AXIS2_WSA_VERSION, AXIS2_FALSE);
    if(NULL != prop)
        addressing_ns = (axis2_char_t *) AXIS2_PROPERTY_GET_VALUE(prop, env);
    
    if(addressing_ns == NULL && AXIS2_TRUE != AXIS2_MSG_CTX_GET_SERVER_SIDE(
        msg_ctx, env))
    {
        addressing_ns = AXIS2_STRDUP(AXIS2_WSA_NAMESPACE, env);
    }
    elements = sandesha2_rm_elements_create(env, addressing_ns);
    envelope = AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(msg_ctx, env);
    action = AXIS2_MSG_CTX_GET_WSA_ACTION(msg_ctx, env);
    SANDESHA2_RM_ELEMENTS_FROM_SOAP_ENVELOPE(elements, env, envelope, action);
    create_seq = SANDESHA2_RM_ELEMENTS_GET_CREATE_SEQ(elements, env);
    if(create_seq)
    {
        printf("create_seq\n");
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_CREATE_SEQ, (sandesha2_iom_rm_part_t *) 
                create_seq);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) create_seq, env);
    }
    create_seq_res = SANDESHA2_RM_ELEMENTS_GET_CREATE_SEQ_RES(elements, env);
    if(create_seq_res)
    {
        printf("create_seq_res\n");
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_CREATE_SEQ_RESPONSE, 
                (sandesha2_iom_rm_part_t *) create_seq_res);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) create_seq_res, env);
        add_op_if_null(env, msg_ctx);
    }
    seq = SANDESHA2_RM_ELEMENTS_GET_SEQ(elements, env);
    if(seq)
    {
        printf("seq\n");
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_SEQ, (sandesha2_iom_rm_part_t *) seq);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) seq, env);
    }
    seq_ack = SANDESHA2_RM_ELEMENTS_GET_SEQ_ACK(elements, env);
    if(seq_ack)
    {
        printf("seq_ack\n");
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT, 
                (sandesha2_iom_rm_part_t *) seq_ack);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) seq_ack, env);
        add_op_if_null(env, msg_ctx);
    }
    terminate_seq = SANDESHA2_RM_ELEMENTS_GET_TERMINATE_SEQ(elements, env);
    if(terminate_seq)
    {
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_TERMINATE_SEQ, 
                (sandesha2_iom_rm_part_t *) terminate_seq);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) terminate_seq, env);
    }
    terminate_seq_res = SANDESHA2_RM_ELEMENTS_GET_TERMINATE_SEQ_RES(elements, env);
    if(terminate_seq_res)
    {
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_TERMINATE_SEQ_RESPONSE, 
                (sandesha2_iom_rm_part_t *) terminate_seq_res);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) terminate_seq_res, env);
    }
    ack_request = SANDESHA2_RM_ELEMENTS_GET_ACK_REQUESTED(elements, env);
    if(ack_request)
    {
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_ACK_REQUEST, 
                (sandesha2_iom_rm_part_t *) ack_request);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) ack_request, env);
    }
    close_seq = SANDESHA2_RM_ELEMENTS_GET_CLOSE_SEQ(elements, env);
    if(close_seq)
    {
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_CLOSE_SEQ, 
                (sandesha2_iom_rm_part_t *) close_seq);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) close_seq, env);
    }
    close_seq_res = SANDESHA2_RM_ELEMENTS_GET_CLOSE_SEQ_RES(elements, env);
    if(close_seq_res)
    {
        SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_CLOSE_SEQ_RESPONSE, 
                (sandesha2_iom_rm_part_t *) close_seq_res);
        rm_ns = SANDESHA2_IOM_RM_ELEMENT_GET_NAMESPACE_VALUE(
                (sandesha2_iom_rm_element_t *) close_seq_res, env);
    }
    SANDESHA2_MSG_CTX_SET_RM_NS_VAL(rm_msg_ctx, env, rm_ns);
    addressing_ns_value = SANDESHA2_RM_ELEMENTS_GET_ADDR_NS_VAL(
            elements, env);
    if(addressing_ns_value)
    {
        SANDESHA2_MSG_CTX_SET_ADDR_NS_VAL(rm_msg_ctx, env, 
                addressing_ns_value);
    }
    return AXIS2_SUCCESS;
}

static axis2_bool_t validate_msg(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_msg_ctx_t *temp_msg_ctx = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *rm_ns = NULL;
    axis2_char_t *prop_key = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_create_seq_t *create_seq = NULL;
    sandesha2_create_seq_res_t *create_seq_res = NULL;
    sandesha2_terminate_seq_t *terminate_seq = NULL;
    sandesha2_terminate_seq_res_t *terminate_seq_res = NULL;
    sandesha2_seq_ack_t *seq_ack = NULL;
    sandesha2_seq_t *seq = NULL;
    sandesha2_ack_requested_t *ack_request = NULL;
    sandesha2_close_seq_t *close_seq = NULL;
    sandesha2_close_seq_res_t *close_seq_res = NULL;
    int temp_flow = -1;

    temp_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(temp_msg_ctx, env);
    conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, conf);
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env);
    create_seq = (sandesha2_create_seq_t *) SANDESHA2_MSG_CTX_GET_MSG_PART(
            rm_msg_ctx, env, SANDESHA2_MSG_PART_CREATE_SEQ);
    create_seq_res = (sandesha2_create_seq_res_t *) 
        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_CREATE_SEQ_RESPONSE);
    terminate_seq = (sandesha2_terminate_seq_t *) 
        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_TERMINATE_SEQ);
    terminate_seq_res = (sandesha2_terminate_seq_res_t *) 
        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_TERMINATE_SEQ_RESPONSE);
    seq_ack = (sandesha2_seq_ack_t *) 
        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
    seq = (sandesha2_seq_t *) SANDESHA2_MSG_CTX_GET_MSG_PART(
            rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ);
    ack_request = (sandesha2_ack_requested_t *) 
        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_ACK_REQUEST);
    close_seq = (sandesha2_close_seq_t *) SANDESHA2_MSG_CTX_GET_MSG_PART(
            rm_msg_ctx, env, SANDESHA2_MSG_PART_CLOSE_SEQ);
    close_seq_res = (sandesha2_close_seq_res_t *) SANDESHA2_MSG_CTX_GET_MSG_PART(
            rm_msg_ctx, env, SANDESHA2_MSG_PART_CLOSE_SEQ_RESPONSE);
    /* Setting message type */
    if(create_seq)
    {
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_CREATE_SEQ);
    }
    else if(create_seq_res)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_CREATE_SEQ_RESPONSE);
        idf = SANDESHA2_CREATE_SEQ_RES_GET_IDENTIFIER(create_seq_res, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else if(terminate_seq)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_TERMINATE_SEQ);
        idf = SANDESHA2_TERMINATE_SEQ_GET_IDENTIFIER(terminate_seq, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else if(terminate_seq_res)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_TERMINATE_SEQ_RESPONSE);
        idf = SANDESHA2_TERMINATE_SEQ_RES_GET_IDENTIFIER(terminate_seq_res, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else if(seq)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_APPLICATION);
        idf = SANDESHA2_SEQ_GET_IDENTIFIER(seq, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else if(seq_ack)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_ACK);
        idf = SANDESHA2_SEQ_ACK_GET_IDENTIFIER(seq_ack, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else if(ack_request)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_ACK_REQUEST);
        idf = SANDESHA2_ACK_REQUESTED_GET_IDENTIFIER(ack_request, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else if(close_seq)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_CLOSE_SEQ);
        idf = SANDESHA2_CLOSE_SEQ_GET_IDENTIFIER(close_seq, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else if(close_seq_res)
    {
        sandesha2_identifier_t *idf = NULL;
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, 
                SANDESHA2_MSG_TYPE_CLOSE_SEQ_RESPONSE);
        idf = SANDESHA2_CLOSE_SEQ_RES_GET_IDENTIFIER(close_seq_res, env);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(idf, env);
    }
    else
    {
        SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, SANDESHA2_MSG_TYPE_UNKNOWN);
    }
    temp_flow = AXIS2_MSG_CTX_GET_FLOW(temp_msg_ctx, env);
    if(temp_flow == AXIS2_IN_FLOW)
    {
        prop_key = AXIS2_STRDUP(seq_id, env);
    }
    else
    {
        sandesha2_seq_property_bean_t *internal_seq_id_bean = NULL;

        internal_seq_id_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, 
                env, seq_id, SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID);
        if(internal_seq_id_bean)
        {
            prop_key = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(
                    internal_seq_id_bean, env);
        }
    }
    rm_ns = SANDESHA2_MSG_CTX_GET_RM_NS_VAL(rm_msg_ctx, env);
    if(seq_id)
    {
        axis2_char_t *spec_version = NULL;
        axis2_char_t *seq_rm_ns = NULL;
        
        spec_version = sandesha2_utils_get_rm_version(env, prop_key, storage_mgr);
        printf("spec_version:%s\n", spec_version);
        if(spec_version)
        {
            seq_rm_ns = sandesha2_spec_specific_consts_get_rm_ns_val(env, 
                    spec_version);
        }
        if(seq_rm_ns && rm_ns)
        {
            if(0 != AXIS2_STRCMP(seq_rm_ns, rm_ns))
            {
                AXIS2_ERROR_SET(env->error, 
                    SANDESHA2_ERROR_RM_NS_VALUE_IS_DIFFERENT_FROM_REGISTERED_NS_FOR_SEQ, 
                    AXIS2_FAILURE);
                return AXIS2_FALSE;
            }
        }
    }
    printf("came2\n");
    return AXIS2_TRUE; 
}
    
static void add_op_if_null(
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_op_t *op = NULL;
        
    op = AXIS2_MSG_CTX_GET_OP(msg_ctx, env);
    if(NULL == op)
    {
        axis2_svc_t *svc = NULL;
        axis2_qname_t *tmp_qname = NULL;
        axis2_status_t status = AXIS2_FAILURE;
        axis2_conf_ctx_t *conf_ctx = NULL;
        axis2_conf_t *conf = NULL;
        axis2_phases_info_t *info = NULL;
    
        tmp_qname = axis2_qname_create(env, "__OPERATION_OUT_IN__", NULL, 
                NULL);
        if (!tmp_qname)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, 
                    AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
        op = axis2_op_create_with_qname(env, tmp_qname);
        AXIS2_QNAME_FREE(tmp_qname, env);
        AXIS2_OP_SET_MSG_EXCHANGE_PATTERN(op, env, AXIS2_MEP_URI_OUT_IN);
        conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
        conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);
        info = AXIS2_CONF_GET_PHASES_INFO(conf, env);
        AXIS2_PHASES_INFO_SET_OP_PHASES(info, env, op);
        svc = AXIS2_MSG_CTX_GET_SVC(msg_ctx, env);
        status = AXIS2_SVC_ADD_OP(svc, env, op);
        if(AXIS2_SUCCESS == status)
        {
            status = AXIS2_MSG_CTX_SET_OP(msg_ctx, env, op);
            if(AXIS2_SUCCESS != status)
            {
                AXIS2_OP_FREE(op, env);
                op = NULL;
            }
        }
        else
        {
            AXIS2_OP_FREE(op, env);
            op = NULL;
        }
    } 
}

