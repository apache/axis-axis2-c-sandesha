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
 
#include <sandesha2/sandesha2_sequence.h>
#include <sandesha2/sandesha2_constants.h>
#include <axiom_soap_header.h>
#include <axiom_soap_header_block.h>

/** 
 * @brief Sequence struct impl
 *	Sandesha2 IOM Sequence
 */
typedef struct sandesha2_sequence_impl sandesha2_sequence_impl_t;  
  
struct sandesha2_sequence_impl
{
	sandesha2_sequence_t sequence;
	sandesha2_identifier_t *identifier;
	sandesha2_msg_number_t *msg_num;
	sandesha2_last_msg_t *last_msg;
	axis2_bool_t must_understand;
	axis2_char_t *ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(sequence) \
						((sandesha2_sequence_impl_t *)(sequence))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_sequence_get_namespace_value (sandesha2_iom_rm_element_t *sequence,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_sequence_from_om_node(sandesha2_iom_rm_element_t *sequence,
                    	const axis2_env_t *env, axiom_node_t *om_node);
    
axiom_node_t* AXIS2_CALL 
sandesha2_sequence_to_om_node(sandesha2_iom_rm_element_t *sequence,
                    	const axis2_env_t *env, void *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_sequence_is_namespace_supported(sandesha2_iom_rm_element_t *sequence,
                    	const axis2_env_t *env, axis2_char_t *namespace);
                    	
sandesha2_identifier_t * AXIS2_CALL
sandesha2_sequence_get_identifier(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_identifier(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, 
                        sandesha2_identifier_t *identifier);
                    	
sandesha2_msg_number_t * AXIS2_CALL
sandesha2_sequence_get_msg_num(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_msg_num(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, 
                        sandesha2_msg_number_t *msg_num);
                    	
sandesha2_last_msg_t * AXIS2_CALL
sandesha2_sequence_get_last_msg(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_last_msg(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, 
                        sandesha2_last_msg_t *last_msg);
                    	
axis2_bool_t AXIS2_CALL
sandesha2_sequence_is_must_understand(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_must_understand(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, axis2_bool_t mu);

axis2_status_t AXIS2_CALL
sandesha2_sequence_to_soap_env(sandesha2_iom_rm_part_t *sequence,
                    	const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope);
                    	                    	
axis2_status_t AXIS2_CALL 
sandesha2_sequence_free (sandesha2_iom_rm_element_t *sequence, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_sequence_t* AXIS2_CALL
sandesha2_sequence_create(const axis2_env_t *env,  axis2_char_t *ns_val)
{
    sandesha2_sequence_impl_t *sequence_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ns_val, NULL);
    
    if(AXIS2_FALSE == sandesha2_sequence_is_namespace_supported(
                        (sandesha2_iom_rm_element_t*)sequence_impl, env, 
                        ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }    
    sequence_impl =  (sandesha2_sequence_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_sequence_impl_t));
	
    if(NULL == sequence_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sequence_impl->ns_val = NULL;
    sequence_impl->identifier = NULL;
    sequence_impl->msg_num = NULL;
    sequence_impl->last_msg = NULL;
    sequence_impl->must_understand = AXIS2_TRUE;
    sequence_impl->sequence.ops = NULL;
    sequence_impl->sequence.part.ops = NULL;
    sequence_impl->sequence.part.element.ops = NULL;
    
    sequence_impl->sequence.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_sequence_ops_t));
    if(NULL == sequence_impl->sequence.ops)
	{
		sandesha2_sequence_free((sandesha2_iom_rm_element_t*)
                         sequence_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sequence_impl->sequence.part.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_part_ops_t));
    if(NULL == sequence_impl->sequence.part.ops)
	{
		sandesha2_sequence_free((sandesha2_iom_rm_element_t*)
                         sequence_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sequence_impl->sequence.part.element.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == sequence_impl->sequence.part.element.ops)
	{
		sandesha2_sequence_free((sandesha2_iom_rm_element_t*)
                         sequence_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    sequence_impl->ns_val = (axis2_char_t *)AXIS2_STRDUP(ns_val, env);
    
    sequence_impl->sequence.part.element.ops->get_namespace_value = 
                        sandesha2_sequence_get_namespace_value;
    sequence_impl->sequence.part.element.ops->from_om_node = 
    					sandesha2_sequence_from_om_node;
    sequence_impl->sequence.part.element.ops->to_om_node = 
    					sandesha2_sequence_to_om_node;
    sequence_impl->sequence.part.element.ops->is_namespace_supported = 
    					sandesha2_sequence_is_namespace_supported;
    sequence_impl->sequence.ops->set_identifier = 
                        sandesha2_sequence_set_identifier;
    sequence_impl->sequence.ops->get_identifier = 
                        sandesha2_sequence_get_identifier;
    sequence_impl->sequence.ops->set_last_msg = 
                        sandesha2_sequence_set_last_msg;
    sequence_impl->sequence.ops->get_last_msg = 
                        sandesha2_sequence_get_last_msg;
    sequence_impl->sequence.ops->set_must_understand = 
                        sandesha2_sequence_set_must_understand;
    sequence_impl->sequence.ops->is_must_understand = 
                        sandesha2_sequence_is_must_understand;
    sequence_impl->sequence.part.element.ops->free = sandesha2_sequence_free;
    
                        
	return &(sequence_impl->sequence);
}


axis2_status_t AXIS2_CALL 
sandesha2_sequence_free (sandesha2_iom_rm_element_t *sequence, 
						const axis2_env_t *env)
{
    sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
    
    if(NULL != sequence_impl->ns_val)
    {
        AXIS2_FREE(env->allocator, sequence_impl->ns_val);
        sequence_impl->ns_val = NULL;
    }
    sequence_impl->identifier = NULL;
    sequence_impl->msg_num = NULL;
    sequence_impl->last_msg = NULL;
    if(NULL != sequence->ops)
        AXIS2_FREE(env->allocator, sequence->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(sequence));
	return AXIS2_SUCCESS;
}

axis2_char_t* AXIS2_CALL 
sandesha2_sequence_get_namespace_value (sandesha2_iom_rm_element_t *sequence,
						const axis2_env_t *env)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	return sequence_impl->ns_val;
}


void* AXIS2_CALL 
sandesha2_sequence_from_om_node(sandesha2_iom_rm_element_t *sequence,
                    	const axis2_env_t *env, axiom_node_t *om_node)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
    axiom_element_t *om_element = NULL;
    axiom_element_t *seq_part = NULL;
    axiom_element_t *lm_part = NULL;
    axiom_node_t *seq_node = NULL;
    axiom_node_t *lm_node = NULL;
    axis2_qname_t *seq_qname = NULL; 
    axis2_qname_t *lm_qname = NULL; 
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
    om_element = AXIOM_NODE_GET_DATA_ELEMENT(om_node, env);
    if(NULL == om_element)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    seq_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQUENCE,
                        sequence_impl->ns_val, NULL);
    if(NULL == seq_qname)
    {
        return NULL;
    }
    seq_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(om_element, env,
                        seq_qname, om_node, &seq_node);
    if(NULL == seq_part)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    sequence_impl->identifier = sandesha2_identifier_create(env, 
                        sequence_impl->ns_val);
    if(NULL == sequence_impl->identifier)
    {
        return NULL;
    }
    SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(((sandesha2_iom_rm_element_t*)
                        sequence_impl->identifier), env, seq_node);
    sequence_impl->msg_num= sandesha2_msg_number_create(env, 
                        sequence_impl->ns_val);
    if(NULL == sequence_impl->msg_num)
    {
        return NULL;
    }
    SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(((sandesha2_iom_rm_element_t*)
                        sequence_impl->msg_num), env, seq_node);
    lm_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_LAST_MSG,
                        sequence_impl->ns_val, NULL);
    if(NULL == lm_qname)
    {
        return NULL;
    }
    lm_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(seq_part, env, 
                        lm_qname, seq_node, &lm_node);
    if(NULL != lm_part)
    {
        sequence_impl->last_msg = sandesha2_last_msg_create(env, 
                        sequence_impl->ns_val);
        if(NULL == sequence_impl->last_msg)
        {
            return NULL;
        }
        SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(((sandesha2_iom_rm_element_t*)
                        sequence_impl->last_msg), env, lm_node);
    }
    return sequence;
}


axiom_node_t* AXIS2_CALL 
sandesha2_sequence_to_om_node(sandesha2_iom_rm_element_t *sequence,
                    	const axis2_env_t *env, void *om_node)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
    axiom_namespace_t *rm_ns = NULL;
    axiom_soap_header_t *soap_header = NULL;
    axiom_soap_header_block_t *seq_block = NULL;
    axiom_node_t *seq_node = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
    soap_header = (axiom_soap_header_t*)om_node;
    if(NULL == sequence_impl->identifier || NULL == sequence_impl->msg_num)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_OM_NULL_ELEMENT, 
                        AXIS2_FAILURE);
        return NULL;
    }
    rm_ns = axiom_namespace_create(env, sequence_impl->ns_val,
                        SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
    if(NULL == rm_ns)
    {
        return NULL;
    }
    seq_block = AXIOM_SOAP_HEADER_ADD_HEADER_BLOCK(soap_header, env, 
                        SANDESHA2_WSRM_COMMON_SEQUENCE, rm_ns);
    if(NULL == seq_block)
    {
        return NULL;
    }
    AXIOM_SOAP_HEADER_BLOCK_SET_MUST_UNDERSTAND_WITH_BOOL(seq_block, env, 
                        sequence_impl->must_understand);
    seq_node = AXIOM_SOAP_HEADER_BLOCK_GET_BASE_NODE(seq_block, env);
    SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        sequence_impl->identifier), env, seq_node);
    SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        sequence_impl->msg_num), env, seq_node);
    if(NULL != sequence_impl->last_msg)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        sequence_impl->last_msg), env, seq_node);
    }
    return seq_node;
}

axis2_bool_t AXIS2_CALL 
sandesha2_sequence_is_namespace_supported(sandesha2_iom_rm_element_t *sequence,
                    	const axis2_env_t *env, axis2_char_t *namespace)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_02_NS_URI))
    {
        return AXIS2_TRUE;
    }
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_10_NS_URI))
    {
        return AXIS2_TRUE;
    }
    return AXIS2_FALSE;
}

sandesha2_identifier_t * AXIS2_CALL
sandesha2_sequence_get_identifier(sandesha2_sequence_t *element,
                    	const axis2_env_t *env)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(element);
	
	return sequence_impl->identifier;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_identifier(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, 
                        sandesha2_identifier_t *identifier)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	sequence_impl->identifier = identifier;
 	return AXIS2_SUCCESS;
}

sandesha2_msg_number_t * AXIS2_CALL
sandesha2_sequence_get_msg_num(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	
	return sequence_impl->msg_num;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_msg_num(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, sandesha2_msg_number_t *msg_num)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	sequence_impl->msg_num = msg_num;
 	return AXIS2_SUCCESS;
}

sandesha2_last_msg_t * AXIS2_CALL
sandesha2_sequence_get_last_msg(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	
	return sequence_impl->last_msg;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_last_msg(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, sandesha2_last_msg_t *last_msg)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	sequence_impl->last_msg = last_msg;
 	return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_sequence_is_must_understand(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FALSE);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	return sequence_impl->must_understand;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_sequence_set_must_understand(sandesha2_sequence_t *sequence,
                    	const axis2_env_t *env, axis2_bool_t mu)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
	sequence_impl->must_understand = mu;
 	return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL
sandesha2_sequence_to_soap_env(sandesha2_iom_rm_part_t *sequence,
                    	const axis2_env_t *env, axiom_soap_envelope_t *envelope)
{
	sandesha2_sequence_impl_t *sequence_impl = NULL;
	axiom_soap_header_t *soap_header = NULL;
    axis2_qname_t *seq_qname = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, envelope, AXIS2_FAILURE);
	
	sequence_impl = SANDESHA2_INTF_TO_IMPL(sequence);
    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(envelope, env);
    /**
     * Remove if old exists
     */
    seq_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQUENCE, 
                        sequence_impl->ns_val, NULL);
    if(NULL == seq_qname)
    {
        return AXIS2_FAILURE;
    }
    AXIOM_SOAP_HEADER_REMOVE_HEADER_BLOCK(soap_header, env, seq_qname);
    sandesha2_sequence_to_om_node((sandesha2_iom_rm_element_t*)sequence, env, 
                        soap_header);
	return AXIS2_SUCCESS;
}
