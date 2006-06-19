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
 
#include <sandesha2/sandesha2_msg_number.h>
#include <sandesha2/sandesha2_constants.h>
#include <stdio.h>

/** 
 * @brief MessageNumber struct impl
 *	Sandesha2 IOM MessageNumber
 */
typedef struct sandesha2_msg_number_impl sandesha2_msg_number_impl_t;  
  
struct sandesha2_msg_number_impl
{
	sandesha2_msg_number_t msg_number;
	long msg_num;
	axis2_char_t *ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(msg_number) \
						((sandesha2_msg_number_impl_t *)(msg_number))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_msg_number_get_namespace_value (sandesha2_iom_rm_element_t *msg_num,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_msg_number_from_om_node(sandesha2_iom_rm_element_t *msg_num,
                    	const axis2_env_t *env, axiom_node_t *om_node);
    
axiom_node_t* AXIS2_CALL 
sandesha2_msg_number_to_om_node(sandesha2_iom_rm_element_t *msg_num,
                    	const axis2_env_t *env, void *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_msg_number_is_namespace_supported(sandesha2_iom_rm_element_t *msg_num,
                    	const axis2_env_t *env, axis2_char_t *namespace);
                    	
long AXIS2_CALL
sandesha2_msg_number_get_msg_num(sandesha2_msg_number_t *msg_num,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_msg_number_set_msg_num(sandesha2_msg_number_t *msg_num,
                    	const axis2_env_t *env, long value);

axis2_status_t AXIS2_CALL 
sandesha2_msg_number_free (sandesha2_iom_rm_element_t *msg_num, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_number_t* AXIS2_CALL
sandesha2_msg_number_create(const axis2_env_t *env,  axis2_char_t *ns_val)
{
    sandesha2_msg_number_impl_t *msg_number_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ns_val, NULL);
    
    if(AXIS2_FALSE == sandesha2_msg_number_is_namespace_supported(
                        (sandesha2_iom_rm_element_t*)msg_number_impl, env, 
                        ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }    
    msg_number_impl =  (sandesha2_msg_number_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_msg_number_impl_t));
	
    if(NULL == msg_number_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    msg_number_impl->ns_val = NULL;
    msg_number_impl->msg_num = 0;
    msg_number_impl->msg_number.element.ops = NULL;
    msg_number_impl->msg_number.ops = NULL;
    
    msg_number_impl->msg_number.element.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == msg_number_impl->msg_number.element.ops)
	{
		sandesha2_msg_number_free((sandesha2_iom_rm_element_t*)
                         msg_number_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    msg_number_impl->msg_number.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_number_ops_t));
    if(NULL == msg_number_impl->msg_number.ops)
	{
		sandesha2_msg_number_free((sandesha2_iom_rm_element_t*)
                         msg_number_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    msg_number_impl->ns_val = (axis2_char_t *)AXIS2_STRDUP(ns_val, env);
    
    msg_number_impl->msg_number.element.ops->get_namespace_value = 
                        sandesha2_msg_number_get_namespace_value;
    msg_number_impl->msg_number.element.ops->from_om_node = 
    					sandesha2_msg_number_from_om_node;
    msg_number_impl->msg_number.element.ops->to_om_node = 
    					sandesha2_msg_number_to_om_node;
    msg_number_impl->msg_number.element.ops->is_namespace_supported = 
    					sandesha2_msg_number_is_namespace_supported;
    msg_number_impl->msg_number.ops->get_msg_num = 
                        sandesha2_msg_number_get_msg_num;
    msg_number_impl->msg_number.ops->set_msg_num = 
                        sandesha2_msg_number_set_msg_num;
    msg_number_impl->msg_number.element.ops->free = sandesha2_msg_number_free;
                        
	return &(msg_number_impl->msg_number);
}


axis2_status_t AXIS2_CALL 
sandesha2_msg_number_free (sandesha2_iom_rm_element_t *msg_num, 
						const axis2_env_t *env)
{
    sandesha2_msg_number_impl_t *msg_number_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_number_impl = SANDESHA2_INTF_TO_IMPL(msg_num);
    
    if(NULL != msg_number_impl->ns_val)
    {
        AXIS2_FREE(env->allocator, msg_number_impl->ns_val);
        msg_number_impl->ns_val = NULL;
    }
    msg_number_impl->msg_num = 0;
    
    if(NULL != msg_number_impl->msg_number.element.ops)
    {
        AXIS2_FREE(env->allocator, msg_number_impl->msg_number.element.ops);
        msg_number_impl->msg_number.element.ops = NULL;
    }        
    if(NULL != msg_num->ops)
    {
        AXIS2_FREE(env->allocator, msg_num->ops);
        msg_num->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_num));
	return AXIS2_SUCCESS;
}

axis2_char_t* AXIS2_CALL 
sandesha2_msg_number_get_namespace_value (sandesha2_iom_rm_element_t *msg_num,
						const axis2_env_t *env)
{
	sandesha2_msg_number_impl_t *msg_number_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	msg_number_impl = SANDESHA2_INTF_TO_IMPL(msg_num);
	return msg_number_impl->ns_val;
}


void* AXIS2_CALL 
sandesha2_msg_number_from_om_node(sandesha2_iom_rm_element_t *msg_num,
                    	const axis2_env_t *env, axiom_node_t *om_node)
{
	sandesha2_msg_number_impl_t *msg_number_impl = NULL;
    axiom_element_t *om_element = NULL;
    axiom_element_t *mn_part = NULL;
    axiom_node_t *mn_node = NULL;
    axis2_qname_t *mn_qname = NULL;
    axis2_char_t *mn_str = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    msg_number_impl = SANDESHA2_INTF_TO_IMPL(msg_num);
    om_element = AXIOM_NODE_GET_DATA_ELEMENT(om_node, env);
    if(NULL == om_element)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    mn_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_MSG_NUMBER, 
                        msg_number_impl->ns_val, NULL);
    if(NULL == mn_qname)
    {
        return NULL;
    }
    mn_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(om_element, env,
                        mn_qname, om_node, &mn_node);
    if(NULL == mn_part)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    mn_str = AXIOM_ELEMENT_GET_TEXT(mn_part, env, mn_node);
    if(NULL == mn_str)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_EMPTY_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    } 
    msg_number_impl->msg_num = atol(mn_str); 
    return msg_num;
    
}


axiom_node_t* AXIS2_CALL 
sandesha2_msg_number_to_om_node(sandesha2_iom_rm_element_t *msg_num,
                    	const axis2_env_t *env, void *om_node)
{
	sandesha2_msg_number_impl_t *msg_number_impl = NULL;
    axiom_namespace_t *rm_ns = NULL;
    axiom_element_t *mn_element = NULL;
    axiom_node_t *mn_node = NULL;
    axis2_char_t *str_num = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    msg_number_impl = SANDESHA2_INTF_TO_IMPL(msg_num);
    if(msg_number_impl->msg_num <= 0)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_OM_INVALID_NUMBER, 
                        AXIS2_FAILURE);
        return NULL;
    }
    rm_ns = axiom_namespace_create(env, msg_number_impl->ns_val,
                        SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
    if(NULL == rm_ns)
    {
        return NULL;
    }
    mn_element = axiom_element_create(env, NULL, 
                        SANDESHA2_WSRM_COMMON_MSG_NUMBER, rm_ns, &mn_node);
    if(NULL == mn_element)
    {
        return NULL;
    }
    str_num = AXIS2_MALLOC(env->allocator, 32*sizeof(axis2_char_t));
    sprintf(str_num, "%ld", msg_number_impl->msg_num);
    AXIOM_ELEMENT_SET_TEXT(mn_element, env, str_num, mn_node);
    AXIOM_NODE_ADD_CHILD((axiom_node_t*)om_node, env, mn_node);
    
    return (axiom_node_t*)om_node;
 }

axis2_bool_t AXIS2_CALL 
sandesha2_msg_number_is_namespace_supported(sandesha2_iom_rm_element_t *msg_num,
                    	const axis2_env_t *env, axis2_char_t *namespace)
{
	sandesha2_msg_number_impl_t *msg_number_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    msg_number_impl = SANDESHA2_INTF_TO_IMPL(msg_num);
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

long AXIS2_CALL
sandesha2_msg_number_get_msg_num(sandesha2_msg_number_t *msg_num,
                    	const axis2_env_t *env)
{
	sandesha2_msg_number_impl_t *msg_number_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	msg_number_impl = SANDESHA2_INTF_TO_IMPL(msg_num);
	return msg_number_impl->msg_num;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_msg_number_set_msg_num(sandesha2_msg_number_t *msg_num,
                    	const axis2_env_t *env, long value)
{
	sandesha2_msg_number_impl_t *msg_number_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	msg_number_impl = SANDESHA2_INTF_TO_IMPL(msg_num);
 	msg_number_impl->msg_num = value;
 	return AXIS2_SUCCESS;
}
