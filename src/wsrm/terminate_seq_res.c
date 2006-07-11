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
 
#include <sandesha2/sandesha2_terminate_seq_res.h>
#include <sandesha2/sandesha2_constants.h>
#include <axiom_soap_body.h>

/** 
 * @brief TerminateSeqResponse struct impl
 *	Sandesha2 IOM TerminateSeqResponse
 */
typedef struct sandesha2_terminate_seq_res_impl 
						sandesha2_terminate_seq_res_impl_t;  
  
struct sandesha2_terminate_seq_res_impl
{
	sandesha2_terminate_seq_res_t terminate_seq_res;
	sandesha2_identifier_t *identifier;
	axis2_char_t *ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(terminate_seq_res) \
						((sandesha2_terminate_seq_res_impl_t *)\
						(terminate_seq_res))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_terminate_seq_res_get_namespace_value (
						sandesha2_iom_rm_element_t *terminate_seq_res,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_terminate_seq_res_from_om_node(
                        sandesha2_iom_rm_element_t *terminate_seq_res,
                    	const axis2_env_t *env, axiom_node_t *om_node);
    
axiom_node_t* AXIS2_CALL 
sandesha2_terminate_seq_res_to_om_node(
                        sandesha2_iom_rm_element_t *terminate_seq_res,
                    	const axis2_env_t *env, void *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_terminate_seq_res_is_namespace_supported(
						sandesha2_iom_rm_element_t *terminate_seq_res,
                    	const axis2_env_t *env, axis2_char_t *namespace);
                    	
sandesha2_identifier_t * AXIS2_CALL
sandesha2_terminate_seq_res_get_identifier(
                        sandesha2_terminate_seq_res_t *terminate_seq_res,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_terminate_seq_res_set_identifier(
                        sandesha2_terminate_seq_res_t *terminate_seq_res,
                    	const axis2_env_t *env, 
                        sandesha2_identifier_t *identifier);

axis2_status_t AXIS2_CALL
sandesha2_terminate_seq_res_to_soap_env(
                        sandesha2_iom_rm_part_t *terminate_seq_res,
                    	const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope);
                    	                    	
axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_res_free (sandesha2_iom_rm_element_t *terminate_seq_res, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_terminate_seq_res_t* AXIS2_CALL
sandesha2_terminate_seq_res_create(const axis2_env_t *env,  axis2_char_t *ns_val)
{
    sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ns_val, NULL);
    
    if(AXIS2_FALSE == sandesha2_terminate_seq_res_is_namespace_supported(
                        (sandesha2_iom_rm_element_t*)terminate_seq_res_impl, 
                        env, ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }    
    terminate_seq_res_impl =  (sandesha2_terminate_seq_res_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_terminate_seq_res_impl_t));
	
    if(NULL == terminate_seq_res_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    terminate_seq_res_impl->ns_val = NULL;
    terminate_seq_res_impl->identifier = NULL;
    terminate_seq_res_impl->terminate_seq_res.ops = NULL;
    terminate_seq_res_impl->terminate_seq_res.part.ops = NULL;
    terminate_seq_res_impl->terminate_seq_res.part.element.ops = NULL;
    
    terminate_seq_res_impl->terminate_seq_res.ops = AXIS2_MALLOC(
    						env->allocator,
        					sizeof(sandesha2_terminate_seq_res_ops_t));
    if(NULL == terminate_seq_res_impl->terminate_seq_res.ops)
	{
		sandesha2_terminate_seq_res_free((sandesha2_iom_rm_element_t*)
                         terminate_seq_res_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    terminate_seq_res_impl->terminate_seq_res.part.ops = AXIS2_MALLOC(
    						env->allocator,
        					sizeof(sandesha2_iom_rm_part_ops_t));
    if(NULL == terminate_seq_res_impl->terminate_seq_res.part.ops)
	{
		sandesha2_terminate_seq_res_free((sandesha2_iom_rm_element_t*)
                         terminate_seq_res_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    terminate_seq_res_impl->terminate_seq_res.part.element.ops = AXIS2_MALLOC(
    						env->allocator,
        					sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == terminate_seq_res_impl->terminate_seq_res.ops)
	{
		sandesha2_terminate_seq_res_free((sandesha2_iom_rm_element_t*)
                         terminate_seq_res_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    terminate_seq_res_impl->ns_val = (axis2_char_t *)AXIS2_STRDUP(ns_val, env);
    
    terminate_seq_res_impl->terminate_seq_res.part.element.ops->
                        get_namespace_value = 
                        sandesha2_terminate_seq_res_get_namespace_value;
    terminate_seq_res_impl->terminate_seq_res.part.element.ops->from_om_node = 
    					sandesha2_terminate_seq_res_from_om_node;
    terminate_seq_res_impl->terminate_seq_res.part.element.ops->to_om_node = 
    					sandesha2_terminate_seq_res_to_om_node;
    terminate_seq_res_impl->terminate_seq_res.part.element.ops->
                        is_namespace_supported = 
    					sandesha2_terminate_seq_res_is_namespace_supported;
    terminate_seq_res_impl->terminate_seq_res.part.ops->to_soap_env = 
    					sandesha2_terminate_seq_res_to_soap_env;
    terminate_seq_res_impl->terminate_seq_res.ops->set_identifier = 
    					sandesha2_terminate_seq_res_set_identifier;
    terminate_seq_res_impl->terminate_seq_res.ops->get_identifier = 
    					sandesha2_terminate_seq_res_get_identifier;
    terminate_seq_res_impl->terminate_seq_res.part.element.ops->free = 
    					sandesha2_terminate_seq_res_free;
                        
	return &(terminate_seq_res_impl->terminate_seq_res);
}


axis2_status_t AXIS2_CALL 
sandesha2_terminate_seq_res_free (sandesha2_iom_rm_element_t *terminate_seq_res, 
						const axis2_env_t *env)
{
    sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
    
    if(NULL != terminate_seq_res_impl->ns_val)
    {
        AXIS2_FREE(env->allocator, terminate_seq_res_impl->ns_val);
        terminate_seq_res_impl->ns_val = NULL;
    }
    terminate_seq_res_impl->identifier = NULL;
    if(NULL != terminate_seq_res->ops)
    {
        AXIS2_FREE(env->allocator, terminate_seq_res->ops);
        terminate_seq_res->ops = NULL;
    }
    if(NULL != terminate_seq_res_impl->terminate_seq_res.part.ops)
    {
        AXIS2_FREE(env->allocator, 
                        terminate_seq_res_impl->terminate_seq_res.part.ops);
        terminate_seq_res_impl->terminate_seq_res.part.ops = NULL;
    }
    if(NULL != terminate_seq_res_impl->terminate_seq_res.ops)
    {
        AXIS2_FREE(env->allocator, 
                        terminate_seq_res_impl->terminate_seq_res.ops);
        terminate_seq_res_impl->terminate_seq_res.part.ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(terminate_seq_res));
	return AXIS2_SUCCESS;
}

axis2_char_t* AXIS2_CALL 
sandesha2_terminate_seq_res_get_namespace_value (
						sandesha2_iom_rm_element_t *terminate_seq_res,
						const axis2_env_t *env)
{
	sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
	return terminate_seq_res_impl->ns_val;
}


void* AXIS2_CALL 
sandesha2_terminate_seq_res_from_om_node(
                        sandesha2_iom_rm_element_t *terminate_seq_res,
                    	const axis2_env_t *env, axiom_node_t *om_node)
{
	sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
    axiom_element_t *om_element = NULL;
    axiom_element_t *tsr_part = NULL;
    axiom_node_t *tsr_node = NULL;
    axis2_qname_t *tsr_qname = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
    om_element = AXIOM_NODE_GET_DATA_ELEMENT(om_node, env);
    if(NULL == om_element)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    tsr_qname = axis2_qname_create(env, 
                        SANDESHA2_WSRM_COMMON_TERMINATE_SEQ_RESPONSE,
                        terminate_seq_res_impl->ns_val, NULL);
    if(NULL == tsr_qname)
    {
        return NULL;
    }
    tsr_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(om_element, env,
                        tsr_qname, om_node, &tsr_node);
    if(NULL == tsr_part)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    terminate_seq_res_impl->identifier = sandesha2_identifier_create(env,
                        terminate_seq_res_impl->ns_val);
    if(NULL == terminate_seq_res_impl->identifier)
    {
        return NULL;
    }
    SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(((sandesha2_iom_rm_element_t*)
                        terminate_seq_res_impl->identifier), env, tsr_node);
    return terminate_seq_res;
}


axiom_node_t* AXIS2_CALL 
sandesha2_terminate_seq_res_to_om_node(
                        sandesha2_iom_rm_element_t *terminate_seq_res,
                    	const axis2_env_t *env, void *om_node)
{
	sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
    axiom_namespace_t *rm_ns = NULL;
    axiom_element_t *tsr_element = NULL;
    axiom_node_t *tsr_node = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
    if(NULL == terminate_seq_res_impl->identifier)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_OM_NULL_ELEMENT, 
                        AXIS2_FAILURE);
        return NULL;
    }
    rm_ns = axiom_namespace_create(env, terminate_seq_res_impl->ns_val,
                        SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
    if(NULL == rm_ns)
    {
        return NULL;
    }
    tsr_element = axiom_element_create(env, NULL, 
                        SANDESHA2_WSRM_COMMON_TERMINATE_SEQ_RESPONSE, 
                        rm_ns, &tsr_node);
    if(NULL == tsr_element)
    {
        return NULL;
    }
    SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        terminate_seq_res_impl->identifier), env, tsr_node);
    AXIOM_NODE_ADD_CHILD((axiom_node_t*)om_node, env, tsr_node);
    return (axiom_node_t*)om_node;
}

axis2_bool_t AXIS2_CALL 
sandesha2_terminate_seq_res_is_namespace_supported(
						sandesha2_iom_rm_element_t *terminate_seq_res,
                    	const axis2_env_t *env, axis2_char_t *namespace)
{
	sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_10_NS_URI))
    {
        return AXIS2_TRUE;
    }
    return AXIS2_FALSE;
}

sandesha2_identifier_t * AXIS2_CALL
sandesha2_terminate_seq_res_get_identifier(
                        sandesha2_terminate_seq_res_t *terminate_seq_res,
                    	const axis2_env_t *env)
{
	sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
	
	return terminate_seq_res_impl->identifier;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_terminate_seq_res_set_identifier(
                        sandesha2_terminate_seq_res_t *terminate_seq_res,
                    	const axis2_env_t *env, 
                        sandesha2_identifier_t *identifier)
{
	sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
 	if(NULL != terminate_seq_res_impl->identifier)
	{
		SANDESHA2_IDENTIFIER_FREE(terminate_seq_res_impl->identifier, env);
		terminate_seq_res_impl->identifier = NULL;
	}
	terminate_seq_res_impl->identifier = identifier;
 	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_terminate_seq_res_to_soap_env(
                        sandesha2_iom_rm_part_t *terminate_seq_res,
                    	const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope)
{
	sandesha2_terminate_seq_res_impl_t *terminate_seq_res_impl = NULL;
    axis2_qname_t *tsr_qname = NULL;
    axiom_node_t *body_node = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, envelope, AXIS2_FAILURE);
	
	terminate_seq_res_impl = SANDESHA2_INTF_TO_IMPL(terminate_seq_res);
    /**
     * Remove if old exists
     */
    tsr_qname = axis2_qname_create(env, 
                        SANDESHA2_WSRM_COMMON_TERMINATE_SEQ_RESPONSE, 
                        terminate_seq_res_impl->ns_val, NULL);
    if(NULL == tsr_qname)
    {
        return AXIS2_FAILURE;
    }
    sandesha2_utils_remove_soap_body_part(env, envelope, tsr_qname);
    body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(AXIOM_SOAP_ENVELOPE_GET_BODY(
                        envelope, env), env);  
    sandesha2_terminate_seq_res_to_om_node((sandesha2_iom_rm_element_t*)
                        terminate_seq_res, env, body_node);
	return AXIS2_SUCCESS;
}
