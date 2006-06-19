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
 
#include <sandesha2/sandesha2_app_msg_processor.h>
/** 
 * @brief Application Message Processor struct impl
 *	Sandesha2 App Msg Processor
 */
typedef struct sandesha2_app_msg_processor_impl sandesha2_app_msg_processor_impl_t;  
  
struct sandesha2_app_msg_processor_impl
{
	sandesha2_msg_processor_t identifier;
	axis2_char_t *str_id;
	axis2_char_t *ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(identifier) \
						((sandesha2_app_msg_processor_impl_t *)(identifier))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_app_msg_processor_get_namespace_value (sandesha2_msg_processor_t *element,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_app_msg_processor_from_om_node(sandesha2_msg_processor_t *element,
                    	const axis2_env_t *env, axis2_om_node_t *om_node);
    
axis2_om_element_t* AXIS2_CALL 
sandesha2_app_msg_processor_to_om_node(sandesha2_msg_processor_t *element,
                    	const axis2_env_t *env, axis2_om_node_t *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_app_msg_processor_is_namespace_supported(sandesha2_msg_processor_t *element,
                    	const axis2_env_t *env, axis2_char_t *namespace);
                    	
axis2_char_t * AXIS2_CALL
sandesha2_app_msg_processor_get_identifier(sandesha2_msg_processor_t *element,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_app_msg_processor_set_identifier(sandesha2_msg_processor_t *element,
                    	const axis2_env_t *env, axis2_char_t *identifier);

axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_DECLARE(sandesha2_msg_processor_t*)
sandesha2_app_msg_processor_create(const axis2_env_t *env,  axis2_char_t *ns_val)
{
    sandesha2_app_msg_processor_impl_t *identifier_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ns_val, NULL);
    
    if(AXIS2_FALSE == sandesha2_app_msg_processor_is_namespace_supported(
                        identifier_impl, env, ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }        
    identifier_impl =  (sandesha2_app_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_app_msg_processor_impl_t));
	
    if(NULL == identifier_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    identifier_impl->ns_val = (axis2_char_t *)AXIS2_STRDUP(ns_val, env);
    identifier_impl->str_id = NULL;
    
    identifier_impl->identifier.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == identifier_impl->identifier.ops)
	{
		sandesha2_app_msg_processor_free((sandesha2_msg_processor_t*)
                         identifier_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    identifier_impl->identifier.ops->get_namespace_value = 
                        sandesha2_app_msg_processor_get_namespace_value;
    identifier_impl->identifier.ops->from_om_node = 
    					sandesha2_app_msg_processor_from_om_node;
    identifier_impl->identifier.ops->to_om_node = 
    					sandesha2_app_msg_processor_to_om_node;
    identifier_impl->identifier.ops->is_namespace_supported = 
    					sandesha2_app_msg_processor_is_namespace_supported
    identifier_impl->identifier.ops->free = sandesha2_app_msg_processor_free;
                        
	return &(identifier_impl->identifier);
}


axis2_status_t AXIS2_CALL 
sandesha2_app_msg_processor_free (sandesha2_msg_processor_t *element, 
						const axis2_env_t *env)
{
    sandesha2_app_msg_processor_impl_t *identifier_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    identifier_impl = SANDESHA2_INTF_TO_IMPL(element);
    
    if(NULL != identifier_impl->ns_val)
    {
        AXIS2_FREE(env->allocator, identifier_impl->ns_val);
        identifier_impl->ns_val = NULL;
    }
    if(NULL != identifier_impl->str_id)
    {
    	AXIS2_FREE(env->allocator, identifier_impl->str_id);
        identifier_impl->str_id = NULL;
    }
    if(NULL != element->ops)
        AXIS2_FREE(env->allocator, element->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(element));
	return AXIS2_SUCCESS;
}

