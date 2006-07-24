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
#include <sandesha2/sandesha2_property_bean.h>
#include <sandesha2/sandesha2_constants.h>
#include <stdio.h>
#include <axis2_string.h>
#include <axis2_utils.h>

/** 
 * @brief Property Bean struct impl
 *	Sandesha2 Property Bean
 */
typedef struct sandesha2_property_bean_impl sandesha2_property_bean_impl_t;  
  
struct sandesha2_property_bean_impl
{
    sandesha2_property_bean_t bean;
    long inactive_timeout_interval;
    long ack_interval;
    long retrans_interval;
    axis2_bool_t is_exp_backoff;
    axis2_char_t *in_mem_storage_mgr;
    axis2_char_t *permanent_storage_mgr;
    axis2_bool_t is_in_order;
    axis2_array_list_t *msg_types_to_drop;
    int max_retrans_count;
};

#define SANDESHA2_INTF_TO_IMPL(bean) \
                        ((sandesha2_property_bean_impl_t *)(bean))

/***************************** Function headers *******************************/
long AXIS2_CALL 
sandesha2_property_bean_get_inactive_timeout_interval
                        (sandesha2_property_bean_t *bean, 
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval_with_units 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval,
                        axis2_char_t *units);
            
long AXIS2_CALL
sandesha2_property_bean_get_ack_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_ack_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval);
            
long AXIS2_CALL
sandesha2_property_bean_get_retrans_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_retrans_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval);
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_exp_backoff 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_exp_backoff 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_bool_t exp_backoff);
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_in_mem_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_mem_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_char_t *manager);
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_permanent_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_permanent_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_char_t *manager);
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_in_order 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_order 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_bool_t in_order);
            
axis2_array_list_t* AXIS2_CALL
sandesha2_property_bean_get_msg_types_to_drop 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_msg_types_to_drop 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_array_list_t *msg_types);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_add_msg_type_to_drop 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, int msg_type);
            
int AXIS2_CALL
sandesha2_property_bean_get_max_retrans_count 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_max_retrans_count 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, int count);
            
axis2_status_t AXIS2_CALL 
sandesha2_property_bean_free(sandesha2_property_bean_t *bean, 
                        const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_bean_create(const axis2_env_t *env)
{
    sandesha2_property_bean_impl_t *property_bean_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    property_bean_impl =  (sandesha2_property_bean_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_property_bean_impl_t));
	
    if(NULL == property_bean_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    property_bean_impl->inactive_timeout_interval = 0;
    property_bean_impl->ack_interval = 0;
    property_bean_impl->retrans_interval = 0;
    property_bean_impl->is_exp_backoff = 0;
    property_bean_impl->in_mem_storage_mgr = NULL;
    property_bean_impl->permanent_storage_mgr= NULL;
    property_bean_impl->is_in_order = AXIS2_FALSE;
    property_bean_impl->msg_types_to_drop = NULL;
    property_bean_impl->max_retrans_count = 0;
    
    property_bean_impl->bean.ops = AXIS2_MALLOC(env->allocator,
                        sizeof(sandesha2_property_bean_ops_t));
    if(NULL == property_bean_impl->bean.ops)
	{
		sandesha2_property_bean_free((sandesha2_property_bean_t*)
                        property_bean_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    property_bean_impl->bean.ops->get_inactive_timeout_interval = 
        sandesha2_property_bean_get_inactive_timeout_interval;
    property_bean_impl->bean.ops->set_inactive_timeout_interval = 
        sandesha2_property_bean_set_inactive_timeout_interval;
    property_bean_impl->bean.ops->set_inactive_timeout_interval_with_units = 
        sandesha2_property_bean_set_inactive_timeout_interval_with_units;
    property_bean_impl->bean.ops->get_ack_interval = 
        sandesha2_property_bean_get_ack_interval;
    property_bean_impl->bean.ops->set_ack_interval = 
        sandesha2_property_bean_set_ack_interval;
    property_bean_impl->bean.ops->get_retrans_interval = 
        sandesha2_property_bean_get_retrans_interval;
    property_bean_impl->bean.ops->set_retrans_interval = 
        sandesha2_property_bean_set_retrans_interval;
    property_bean_impl->bean.ops->is_exp_backoff = 
        sandesha2_property_bean_is_exp_backoff;
    property_bean_impl->bean.ops->set_exp_backoff = 
        sandesha2_property_bean_set_exp_backoff;
    property_bean_impl->bean.ops->get_in_mem_storage_mgr = 
        sandesha2_property_bean_get_in_mem_storage_mgr;
    property_bean_impl->bean.ops->set_in_mem_storage_mgr = 
        sandesha2_property_bean_set_in_mem_storage_mgr;
    property_bean_impl->bean.ops->get_permanent_storage_mgr = 
        sandesha2_property_bean_get_permanent_storage_mgr;
    property_bean_impl->bean.ops->set_permanent_storage_mgr = 
        sandesha2_property_bean_set_permanent_storage_mgr;
    property_bean_impl->bean.ops->is_in_order = 
        sandesha2_property_bean_is_in_order;
    property_bean_impl->bean.ops->set_in_order = 
        sandesha2_property_bean_set_in_order;
    property_bean_impl->bean.ops->get_msg_types_to_drop = 
        sandesha2_property_bean_get_msg_types_to_drop;
    property_bean_impl->bean.ops->set_msg_types_to_drop = 
        sandesha2_property_bean_set_msg_types_to_drop;
    property_bean_impl->bean.ops->add_msg_type_to_drop = 
        sandesha2_property_bean_add_msg_type_to_drop;
    property_bean_impl->bean.ops->get_max_retrans_count = 
        sandesha2_property_bean_get_max_retrans_count;
    property_bean_impl->bean.ops->set_max_retrans_count = 
        sandesha2_property_bean_set_max_retrans_count;
    property_bean_impl->bean.ops->free = sandesha2_property_bean_free;
                        
	return &(property_bean_impl->bean);
}


axis2_status_t AXIS2_CALL 
sandesha2_property_bean_free(sandesha2_property_bean_t *bean, const axis2_env_t *env)
{
    sandesha2_property_bean_impl_t *property_bean_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    property_bean_impl = SANDESHA2_INTF_TO_IMPL(bean);
    
    if(NULL != property_bean_impl->msg_types_to_drop)
    {
        int count = AXIS2_ARRAY_LIST_SIZE(property_bean_impl->msg_types_to_drop,
                        env);
        int i = 0;
        for(i = 0; i < count; i++)
        {
            int *msg_type = AXIS2_ARRAY_LIST_GET(
                        property_bean_impl->msg_types_to_drop, env, i);
            if(NULL != msg_type)
                AXIS2_FREE(env->allocator, msg_type);
        }
        AXIS2_ARRAY_LIST_FREE(property_bean_impl->msg_types_to_drop, env);
    }
    if(NULL != property_bean_impl->in_mem_storage_mgr)
    {
        AXIS2_FREE(env->allocator, property_bean_impl->in_mem_storage_mgr);
        property_bean_impl->in_mem_storage_mgr = NULL;
    }
    if(NULL != property_bean_impl->permanent_storage_mgr)
    {
        AXIS2_FREE(env->allocator, property_bean_impl->permanent_storage_mgr);
        property_bean_impl->permanent_storage_mgr = NULL;
    }
    if(NULL != bean->ops)
    {
        AXIS2_FREE(env->allocator, bean->ops);
        bean->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(bean));
	return AXIS2_SUCCESS;
}

long AXIS2_CALL 
sandesha2_property_bean_get_inactive_timeout_interval
                        (sandesha2_property_bean_t *bean, 
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return SANDESHA2_INTF_TO_IMPL(bean)->inactive_timeout_interval;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->inactive_timeout_interval = interval;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval_with_units 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval,
                        axis2_char_t *units)
{
    long multiplier = -1;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, units, AXIS2_FAILURE);
    
    if(0 == AXIS2_STRCMP(units, "seconds"))
        multiplier = 1000;
    else if(0 == AXIS2_STRCMP(units, "minutes"))
        multiplier = 60*1000;
    else if(0 == AXIS2_STRCMP(units, "hours"))
        multiplier = 60*60*1000;
    else if(0 == AXIS2_STRCMP(units, "days"))
        multiplier = 24*60*60*1000;
    else
        return AXIS2_FAILURE;
        
    return sandesha2_property_bean_set_inactive_timeout_interval(bean, env, 
                        multiplier*interval);
}
            
long AXIS2_CALL
sandesha2_property_bean_get_ack_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return SANDESHA2_INTF_TO_IMPL(bean)->ack_interval;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_ack_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->ack_interval = interval;
    return AXIS2_SUCCESS;
}
            
long AXIS2_CALL
sandesha2_property_bean_get_retrans_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return SANDESHA2_INTF_TO_IMPL(bean)->retrans_interval;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_retrans_interval 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, long interval)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->retrans_interval = interval;
    return AXIS2_SUCCESS;
}
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_exp_backoff 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)

{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    return SANDESHA2_INTF_TO_IMPL(bean)->is_exp_backoff;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_exp_backoff 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_bool_t exp_backoff)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->is_exp_backoff = exp_backoff;
    return AXIS2_SUCCESS;
}
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_in_mem_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(bean)->in_mem_storage_mgr;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_mem_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_char_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, manager, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->in_mem_storage_mgr = AXIS2_STRDUP(manager,
                        env);
    return AXIS2_SUCCESS;
}
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_permanent_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(bean)->permanent_storage_mgr;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_permanent_storage_mgr 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_char_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, manager, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->permanent_storage_mgr = AXIS2_STRDUP(
                        manager, env);
    return AXIS2_SUCCESS;
}
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_in_order 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    return SANDESHA2_INTF_TO_IMPL(bean)->is_in_order;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_order 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_bool_t in_order)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->is_in_order = in_order;
    return AXIS2_SUCCESS;
}
            
axis2_array_list_t* AXIS2_CALL
sandesha2_property_bean_get_msg_types_to_drop 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(bean)->msg_types_to_drop;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_msg_types_to_drop 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, axis2_array_list_t *msg_types)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_types, AXIS2_FAILURE);
    
    SANDESHA2_INTF_TO_IMPL(bean)->msg_types_to_drop = msg_types;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_add_msg_type_to_drop 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, int msg_type)
{
    int *_msg_type = NULL;
    sandesha2_property_bean_impl_t *property_bean_impl = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    property_bean_impl = SANDESHA2_INTF_TO_IMPL(bean);
    if(NULL == property_bean_impl->msg_types_to_drop)
        return AXIS2_FAILURE;
    _msg_type = AXIS2_MALLOC(env->allocator, sizeof(int));
    AXIS2_ARRAY_LIST_ADD(property_bean_impl->msg_types_to_drop, env, _msg_type);
    return AXIS2_SUCCESS;
}
            
int AXIS2_CALL
sandesha2_property_bean_get_max_retrans_count 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return SANDESHA2_INTF_TO_IMPL(bean)->max_retrans_count;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_max_retrans_count 
                        (sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, int count)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(bean)->max_retrans_count = count;
    return AXIS2_SUCCESS;
}
