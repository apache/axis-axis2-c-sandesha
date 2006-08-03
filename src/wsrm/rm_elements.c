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
#include <sandesha2_rm_elements.h>
#include <sandesha2/sandesha2_constants.h>
#include <axis2_hash.h>
#include <axiom_soap_const.h>
#include <sandesha2/sandesha2_iom_rm_part.h>
#include <sandesha2/sandesha2_utils.h>
#include <axiom_soap_header.h>
#include <axiom_soap_body.h>
#include <stdio.h>
#include <axis2_addr.h>


/** 
 * @brief RM Elements struct impl
 *	Sandesha2 RM Elements
 */
typedef struct sandesha2_rm_elements_impl sandesha2_rm_elements_impl_t;  
  
struct sandesha2_rm_elements_impl
{
    sandesha2_rm_elements_t elements;
    sandesha2_seq_t *seq;
    sandesha2_seq_ack_t *seq_ack;
    sandesha2_create_seq_t *create_seq;
    sandesha2_create_seq_res_t *create_seq_res;
    sandesha2_terminate_seq_t *terminate_seq;
    sandesha2_terminate_seq_res_t *terminate_seq_res;
    sandesha2_close_seq_t *close_seq;
    sandesha2_close_seq_res_t *close_seq_res;
    sandesha2_ack_requested_t *ack_requested;
    axis2_char_t *rm_ns_val;
    axis2_char_t *addr_ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(elements) \
                        ((sandesha2_rm_elements_impl_t *)(elements))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_from_soap_envelope
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope,
            axis2_char_t *action);
            
axiom_soap_envelope_t* AXIS2_CALL 
sandesha2_rm_elements_to_soap_envelope
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope);
            
sandesha2_create_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_create_seq 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_create_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_create_seq_t *create_seq);
    
sandesha2_create_seq_res_t* AXIS2_CALL 
sandesha2_rm_elements_get_create_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_create_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_create_seq_res_t *create_seq_res);
            
sandesha2_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_seq_t *seq);
            
sandesha2_seq_ack_t* AXIS2_CALL 
sandesha2_rm_elements_get_seq_ack
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_seq_ack
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_seq_ack_t *seq_ack);
            
sandesha2_terminate_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_terminate_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_terminate_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_terminate_seq_t *terminate_seq);
            
sandesha2_terminate_seq_res_t* AXIS2_CALL 
sandesha2_rm_elements_get_terminate_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_terminate_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_terminate_seq_res_t *terminate_seq_res);
            
sandesha2_ack_requested_t* AXIS2_CALL 
sandesha2_rm_elements_get_ack_requested
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_ack_requested
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_ack_requested_t *ack_reqested);
    
sandesha2_close_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_close_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_close_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_close_seq_t *close_seq);
    
sandesha2_close_seq_res_t* AXIS2_CALL 
sandesha2_rm_elements_get_close_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_close_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_close_seq_res_t *close_seq_res);
    
axis2_char_t* AXIS2_CALL 
sandesha2_rm_elements_get_addr_ns_val
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);

axis2_char_t* AXIS2_CALL 
sandesha2_rm_elements_get_rm_ns_val
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope,
            axis2_char_t *action);

axis2_char_t* AXIS2_CALL 
sandesha2_rm_elements_get_addr_ns_val_from_env
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope,
            axis2_char_t *action);
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_free(sandesha2_rm_elements_t *mgr, 
                        const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_rm_elements_t* AXIS2_CALL
sandesha2_rm_elements_create(const axis2_env_t *env,
                        axis2_char_t *addr_ns_val)
{
    sandesha2_rm_elements_impl_t *rm_elements_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    rm_elements_impl =  (sandesha2_rm_elements_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_rm_elements_impl_t));
	
    if(NULL == rm_elements_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    rm_elements_impl->seq = NULL;
    rm_elements_impl->seq_ack = NULL;
    rm_elements_impl->create_seq = NULL;
    rm_elements_impl->create_seq_res = NULL;
    rm_elements_impl->terminate_seq = NULL;
    rm_elements_impl->terminate_seq_res = NULL;
    rm_elements_impl->close_seq = NULL;
    rm_elements_impl->close_seq_res = NULL;
    rm_elements_impl->ack_requested = NULL;
    rm_elements_impl->rm_ns_val = NULL;
    rm_elements_impl->addr_ns_val = NULL;
    
    rm_elements_impl->elements.ops = AXIS2_MALLOC(env->allocator,
                        sizeof(sandesha2_rm_elements_ops_t));
    if(NULL == rm_elements_impl->elements.ops)
	{
		sandesha2_rm_elements_free((sandesha2_rm_elements_t*)rm_elements_impl, 
                        env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    rm_elements_impl->addr_ns_val = AXIS2_STRDUP(addr_ns_val, env);
    
    
    rm_elements_impl->elements.ops->from_soap_envelope = 
                        sandesha2_rm_elements_from_soap_envelope;
    rm_elements_impl->elements.ops->to_soap_envelope = 
                        sandesha2_rm_elements_to_soap_envelope;
    rm_elements_impl->elements.ops->get_create_seq = 
                        sandesha2_rm_elements_get_create_seq;
    rm_elements_impl->elements.ops->set_create_seq = 
                        sandesha2_rm_elements_set_create_seq;
    rm_elements_impl->elements.ops->get_create_seq_res = 
                        sandesha2_rm_elements_get_create_seq_res;
    rm_elements_impl->elements.ops->set_create_seq_res = 
                        sandesha2_rm_elements_set_create_seq_res;
    rm_elements_impl->elements.ops->get_seq = 
                        sandesha2_rm_elements_get_seq;
    rm_elements_impl->elements.ops->set_seq = 
                        sandesha2_rm_elements_set_seq;
    rm_elements_impl->elements.ops->get_seq_ack = 
                        sandesha2_rm_elements_get_seq_ack;
    rm_elements_impl->elements.ops->set_seq_ack = 
                        sandesha2_rm_elements_set_seq_ack;
    rm_elements_impl->elements.ops->get_terminate_seq = 
                        sandesha2_rm_elements_get_terminate_seq;
    rm_elements_impl->elements.ops->set_terminate_seq = 
                        sandesha2_rm_elements_set_terminate_seq;
    rm_elements_impl->elements.ops->get_terminate_seq_res = 
                        sandesha2_rm_elements_get_terminate_seq_res;
    rm_elements_impl->elements.ops->set_terminate_seq_res = 
                        sandesha2_rm_elements_set_terminate_seq_res;
    rm_elements_impl->elements.ops->get_ack_requested = 
                        sandesha2_rm_elements_get_ack_requested;
    rm_elements_impl->elements.ops->set_ack_requested = 
                        sandesha2_rm_elements_set_ack_requested;
    rm_elements_impl->elements.ops->get_close_seq = 
                        sandesha2_rm_elements_get_close_seq;
    rm_elements_impl->elements.ops->set_close_seq = 
                        sandesha2_rm_elements_set_close_seq;
    rm_elements_impl->elements.ops->get_close_seq_res = 
                        sandesha2_rm_elements_get_close_seq_res;
    rm_elements_impl->elements.ops->set_close_seq_res = 
                        sandesha2_rm_elements_set_close_seq_res;
    rm_elements_impl->elements.ops->get_addr_ns_val = 
                        sandesha2_rm_elements_get_addr_ns_val;
    rm_elements_impl->elements.ops->free = sandesha2_rm_elements_free;
                        
	return &(rm_elements_impl->elements);
}


axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_free(sandesha2_rm_elements_t *elements, 
                        const axis2_env_t *env)
{
    sandesha2_rm_elements_impl_t *elements_impl = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    elements_impl = SANDESHA2_INTF_TO_IMPL(elements);
    
    if(NULL != elements_impl->addr_ns_val)
    {
        AXIS2_FREE(env->allocator, elements_impl->addr_ns_val);
        elements_impl->addr_ns_val = NULL;
    }
    if(NULL != elements->ops)
    {
        AXIS2_FREE(env->allocator, elements->ops);
        elements->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(elements));
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_from_soap_envelope
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope,
            axis2_char_t *action)
{
    int soap_version = -1;
    sandesha2_rm_elements_impl_t *elements_impl = NULL;
    axiom_soap_header_t *soap_header = NULL;
    axiom_soap_body_t *soap_body = NULL;
    axiom_node_t *header_node = NULL;
    axiom_element_t *body_element = NULL;
    axiom_node_t *body_node = NULL;
    axiom_element_t *header_element = NULL;
    axiom_element_t *seq_element = NULL;
    axiom_node_t *seq_node = NULL;
    axiom_element_t *seq_ack_element = NULL;
    axiom_node_t *seq_ack_node = NULL;
    axiom_element_t *create_seq_element = NULL;
    axiom_node_t *create_seq_node = NULL;
    axiom_element_t *create_seq_res_element = NULL;
    axiom_node_t *create_seq_res_node = NULL;
    axiom_element_t *terminate_seq_element = NULL;
    axiom_node_t *terminate_seq_node = NULL;
    axiom_element_t *terminate_seq_res_element = NULL;
    axiom_node_t *terminate_seq_res_node = NULL;
    axiom_element_t *close_seq_element = NULL;
    axiom_node_t *close_seq_node = NULL;
    axiom_element_t *close_seq_res_element = NULL;
    axiom_node_t *close_seq_res_node = NULL;
    axiom_element_t *ack_requested_element = NULL;
    axiom_node_t *ack_requested_node = NULL;
    axis2_qname_t *qname = NULL;
    axis2_char_t *rm_ns_val = NULL;
    axis2_char_t *addr_ns_val = NULL;
    
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, action, AXIS2_FAILURE);
    
    elements_impl = SANDESHA2_INTF_TO_IMPL(rm_elements);
    soap_version = AXIOM_SOAP_ENVELOPE_GET_SOAP_VERSION(soap_envelope, env);
    
    elements_impl->rm_ns_val = sandesha2_rm_elements_get_rm_ns_val(rm_elements,
                        env, soap_envelope, action);
                        
    if(NULL == elements_impl->rm_ns_val)
        return AXIS2_SUCCESS;
        
    addr_ns_val =  sandesha2_rm_elements_get_addr_ns_val_from_env(
                        rm_elements, env, soap_envelope, action);
    if(NULL != addr_ns_val)
        elements_impl->addr_ns_val = addr_ns_val;

    addr_ns_val = NULL;
                        
    if(NULL == elements_impl->addr_ns_val)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Cant find the"
                        " addressing version");
        return AXIS2_FAILURE;
    }
    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(soap_envelope, env);    
    header_node = AXIOM_SOAP_HEADER_GET_BASE_NODE(soap_header, env);
    header_element = AXIOM_NODE_GET_DATA_ELEMENT(header_node, env);
    
    soap_body = AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope, env);
    body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(soap_body, env);
    body_element = AXIOM_NODE_GET_DATA_ELEMENT(body_node, env);
    
    
                        
    rm_ns_val = elements_impl->rm_ns_val;
    addr_ns_val = elements_impl->addr_ns_val;
    
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQ, rm_ns_val, NULL);
    seq_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(header_element, env,
                        qname, header_node, &seq_node);
    if(NULL != seq_node)
    {
        elements_impl->seq = sandesha2_seq_create(env, rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->seq, env, 
                        header_node);
    }
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQ_ACK, rm_ns_val, 
                        NULL);
    seq_ack_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(header_element,
                        env, qname, header_node, &seq_ack_node);
    if(NULL != seq_ack_element)
    {
        elements_impl->seq_ack = sandesha2_seq_ack_create(env, rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->seq_ack, env,
                        header_node);
    }
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_CREATE_SEQ, rm_ns_val,
                        NULL);
    create_seq_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(body_element,
                        env, qname, body_node, &create_seq_node);
    if(NULL != create_seq_node)
    {
        elements_impl->create_seq = sandesha2_create_seq_create(env,
                        addr_ns_val, rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->create_seq, env,
                        body_node);
    }
    
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_CREATE_SEQ_RESPONSE, 
                        rm_ns_val, NULL);
    create_seq_res_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(
                        body_element, env, qname, body_node, 
                        &create_seq_res_node);
    if(NULL != create_seq_res_node)
    {
        elements_impl->create_seq_res = sandesha2_create_seq_res_create(env, 
                        rm_ns_val, addr_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->create_seq_res, env,
                        body_node);
    }
    
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_TERMINATE_SEQ, 
                        rm_ns_val, NULL);
    terminate_seq_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(
                        body_element, env, qname, body_node, 
                        &terminate_seq_node);
    if(NULL != terminate_seq_node)
    {
        elements_impl->terminate_seq = sandesha2_terminate_seq_create(env, 
                        rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->terminate_seq, env,
                        body_node);
    }
    
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_TERMINATE_SEQ_RESPONSE, 
                        rm_ns_val, NULL);
    terminate_seq_res_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(
                        body_element, env, qname, body_node, 
                        &terminate_seq_res_node);
    if(NULL != terminate_seq_res_node)
    {
        elements_impl->terminate_seq_res = sandesha2_terminate_seq_res_create(
                        env, rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->terminate_seq_res, 
                        env, body_node);
    }
    
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_CLOSE_SEQ, 
                        rm_ns_val, NULL);
    close_seq_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(
                        body_element, env, qname, body_node, 
                        &close_seq_node);
    if(NULL != close_seq_node)
    {
        elements_impl->close_seq = sandesha2_close_seq_create(env, 
                        rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->close_seq, env,
                        body_node);
    }
    
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_CLOSE_SEQ_RESPONSE, 
                        rm_ns_val, NULL);
    close_seq_res_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(
                        body_element, env, qname, body_node, 
                        &close_seq_res_node);
    if(NULL != close_seq_res_node)
    {
        elements_impl->close_seq_res = sandesha2_close_seq_res_create(env, 
                        rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->close_seq_res, env,
                        body_node);
    }
    
    qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_ACK_REQUESTED, 
                        rm_ns_val, NULL);
    ack_requested_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(
                        header_element, env, qname, header_node, 
                        &ack_requested_node);
    if(NULL != ack_requested_node)
    {
        elements_impl->ack_requested = sandesha2_ack_requested_create(env, 
                        rm_ns_val);
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(elements_impl->ack_requested, env,
                        header_node);
    }
    return AXIS2_SUCCESS;
}
            
axiom_soap_envelope_t* AXIS2_CALL 
sandesha2_rm_elements_to_soap_envelope
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope)
{
    sandesha2_rm_elements_impl_t *rm_elements_impl = NULL;
    axiom_soap_header_t *soap_header = NULL;
    axiom_soap_body_t *soap_body = NULL;
    axiom_node_t *body_node = NULL;
    
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, NULL);
    
    rm_elements_impl = SANDESHA2_INTF_TO_IMPL(rm_elements);
    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(soap_envelope, env);
    soap_body = AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope, env);
    body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(soap_body, env);
    
    if(NULL != rm_elements_impl->seq)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->seq, env, 
                        soap_header);
    }
    if(NULL != rm_elements_impl->seq_ack)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->seq_ack, env, 
                        soap_header);
    }
    if(NULL != rm_elements_impl->create_seq)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->create_seq, env, 
                        body_node);
    }
    if(NULL != rm_elements_impl->create_seq_res)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->create_seq_res, 
                        env, body_node);
    }
    if(NULL != rm_elements_impl->terminate_seq)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->terminate_seq, 
                        env, body_node);
    }
    if(NULL != rm_elements_impl->terminate_seq_res)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->terminate_seq_res, 
                        env, body_node);
    }
    if(NULL != rm_elements_impl->close_seq)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->close_seq, 
                        env, body_node);
    }
    if(NULL != rm_elements_impl->close_seq_res)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(rm_elements_impl->close_seq_res, 
                        env, body_node);
    }
    return soap_envelope;
}
            
sandesha2_create_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_create_seq 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->create_seq;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_create_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_create_seq_t *create_seq)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, create_seq, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->create_seq = create_seq;
    return AXIS2_SUCCESS;
}
    
sandesha2_create_seq_res_t* AXIS2_CALL 
sandesha2_rm_elements_get_create_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->create_seq_res;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_create_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_create_seq_res_t *create_seq_res)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, create_seq_res, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->create_seq_res = create_seq_res;
    return AXIS2_SUCCESS;
}
            
sandesha2_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->seq;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_seq_t *seq)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->seq = seq;
    return AXIS2_SUCCESS;
}
            
sandesha2_seq_ack_t* AXIS2_CALL 
sandesha2_rm_elements_get_seq_ack
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->seq_ack;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_seq_ack
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_seq_ack_t *seq_ack)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_ack, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->seq_ack = seq_ack;
    return AXIS2_SUCCESS;
}
            
sandesha2_terminate_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_terminate_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->terminate_seq;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_terminate_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_terminate_seq_t *terminate_seq)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, terminate_seq, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->terminate_seq = terminate_seq;
    return AXIS2_SUCCESS;
}
            
sandesha2_terminate_seq_res_t* AXIS2_CALL 
sandesha2_rm_elements_get_terminate_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->terminate_seq_res;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_terminate_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_terminate_seq_res_t *terminate_seq_res)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, terminate_seq_res, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->terminate_seq_res = terminate_seq_res;
    return AXIS2_SUCCESS;
}
            
sandesha2_ack_requested_t* AXIS2_CALL 
sandesha2_rm_elements_get_ack_requested
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->ack_requested;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_ack_requested
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_ack_requested_t *ack_requested)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, ack_requested, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->ack_requested = ack_requested;
    return AXIS2_SUCCESS;
}
    
sandesha2_close_seq_t* AXIS2_CALL 
sandesha2_rm_elements_get_close_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->close_seq;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_close_seq
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_close_seq_t *close_seq)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, close_seq, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->close_seq = close_seq;
    return AXIS2_SUCCESS;
}
    
sandesha2_close_seq_res_t* AXIS2_CALL 
sandesha2_rm_elements_get_close_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->close_seq_res;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_rm_elements_set_close_seq_res
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_close_seq_res_t *close_seq_res)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, close_seq_res, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(rm_elements)->close_seq_res = close_seq_res;
    return AXIS2_SUCCESS;
}
    
axis2_char_t* AXIS2_CALL 
sandesha2_rm_elements_get_addr_ns_val
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return SANDESHA2_INTF_TO_IMPL(rm_elements)->addr_ns_val;
}

axis2_char_t* AXIS2_CALL 
sandesha2_rm_elements_get_rm_ns_val
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope,
            axis2_char_t *action)
{
    axiom_soap_header_t *soap_header = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, action, AXIS2_FAILURE);
    
    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(soap_envelope, env);
    if(NULL != soap_header)
    {
        axis2_array_list_t *headers = NULL;
        
        headers = AXIOM_SOAP_HEADER_GET_HEADER_BLOCKS_WITH_NAMESPACE_URI(
                        soap_header, env, SANDESHA2_SPEC_2005_02_NS_URI);
        if(NULL != headers && 0 < AXIS2_ARRAY_LIST_SIZE(headers, env))
            return SANDESHA2_SPEC_2005_02_NS_URI;
        headers = AXIOM_SOAP_HEADER_GET_HEADER_BLOCKS_WITH_NAMESPACE_URI(
                        soap_header, env, SANDESHA2_SPEC_2005_10_NS_URI);
                
        if(NULL != headers && 0 < AXIS2_ARRAY_LIST_SIZE(headers, env))
            return SANDESHA2_SPEC_2005_10_NS_URI;
    }
    if(NULL == action)
        return NULL;
    
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_02_ACTION_CREATE_SEQ))
        return SANDESHA2_SPEC_2005_02_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_02_ACTION_CREATE_SEQ_RESPONSE))
        return SANDESHA2_SPEC_2005_02_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_02_ACTION_SEQ_ACKNOWLEDGEMENT))
        return SANDESHA2_SPEC_2005_02_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_02_ACTION_TERMINATE_SEQ))
        return SANDESHA2_SPEC_2005_02_NS_URI;
        
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_CREATE_SEQ))
        return SANDESHA2_SPEC_2005_10_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_CREATE_SEQ_RESPONSE))
        return SANDESHA2_SPEC_2005_10_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_SEQ_ACKNOWLEDGEMENT))
        return SANDESHA2_SPEC_2005_10_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_TERMINATE_SEQ))
        return SANDESHA2_SPEC_2005_10_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_CLOSE_SEQ))
        return SANDESHA2_SPEC_2005_10_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_TERMINATE_SEQ_RESPONSE))
        return SANDESHA2_SPEC_2005_10_NS_URI;
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_CLOSE_SEQ_RESPONSE))
        return SANDESHA2_SPEC_2005_10_NS_URI;
    
    return NULL;
    
}

axis2_char_t* AXIS2_CALL 
sandesha2_rm_elements_get_addr_ns_val_from_env
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope,
            axis2_char_t *action)
{
    axiom_soap_header_t *soap_header = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, action, AXIS2_FAILURE);
    
    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(soap_envelope, env);
    if(NULL != soap_header)
    {
        axis2_array_list_t *headers = NULL;
        
        headers = AXIOM_SOAP_HEADER_GET_HEADER_BLOCKS_WITH_NAMESPACE_URI(
                        soap_header, env, AXIS2_WSA_NAMESPACE);
        if(NULL != headers && 0 < AXIS2_ARRAY_LIST_SIZE(headers, env))
            return AXIS2_WSA_NAMESPACE;
            
        headers = AXIOM_SOAP_HEADER_GET_HEADER_BLOCKS_WITH_NAMESPACE_URI(
                        soap_header, env, AXIS2_WSA_NAMESPACE_SUBMISSION);
                
        if(NULL != headers && 0 < AXIS2_ARRAY_LIST_SIZE(headers, env))
            return AXIS2_WSA_NAMESPACE_SUBMISSION;
    }
    return NULL;
    
    
}
