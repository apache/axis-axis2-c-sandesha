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
#include <sandesha2_fault_data.h>
#include <sandesha2_constants.h>
#include <axis2_string.h>


/** 
 * @brief Fault Data struct impl
 *	Sandesha2 Fault Data
 */
typedef struct sandesha2_fault_data_impl sandesha2_fault_data_impl_t;  
  
struct sandesha2_fault_data_impl
{
    sandesha2_fault_data_t data;
    int type;
    axis2_char_t *code;
    axis2_char_t *sub_code;
    axis2_char_t *reason;
    axis2_char_t *seq_id;
    axiom_node_t *detail;
};

#define SANDESHA2_INTF_TO_IMPL(data) \
                        ((sandesha2_fault_data_impl_t *)(data))

AXIS2_EXTERN sandesha2_fault_data_t* AXIS2_CALL
sandesha2_fault_data_create(const axis2_env_t *env)
{
    sandesha2_fault_data_impl_t *fault_data_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    fault_data_impl =  (sandesha2_fault_data_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_fault_data_impl_t));
	
    if(NULL == fault_data_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    fault_data_impl->type = -1;
    fault_data_impl->code = NULL;
    fault_data_impl->sub_code = NULL;
    fault_data_impl->reason = NULL;
    fault_data_impl->seq_id = NULL;
    fault_data_impl->detail = NULL;
    
	return &(fault_data_impl->data);
}


axis2_status_t AXIS2_CALL 
sandesha2_fault_data_free(
    sandesha2_fault_data_t *data, 
    const axis2_env_t *env)
{
    sandesha2_fault_data_impl_t *fault_data_impl = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    fault_data_impl = SANDESHA2_INTF_TO_IMPL(data);
    fault_data_impl->type = -1;
    if(NULL != fault_data_impl->code)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->code);
        fault_data_impl->code = NULL;
    }
    if(NULL != fault_data_impl->sub_code)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->sub_code);
        fault_data_impl->sub_code = NULL;
    }
    if(NULL != fault_data_impl->reason)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->reason);
        fault_data_impl->reason = NULL;
    }
    if(NULL != fault_data_impl->seq_id)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->seq_id);
        fault_data_impl->seq_id = NULL;
    }
    /* Not sure we have to free this.
     * TODO consider in memory cleaning stage
     */
    fault_data_impl->detail = NULL;
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(data));
	return AXIS2_SUCCESS;
}

axiom_node_t *AXIS2_CALL 
sandesha2_fault_data_get_detail(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(data)->detail;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_fault_data_set_detail(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env,
    axiom_node_t *detail)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, detail, AXIS2_FAILURE);
    
    SANDESHA2_INTF_TO_IMPL(data)->detail = detail;
    return AXIS2_SUCCESS;
}
            
axis2_char_t *AXIS2_CALL 
sandesha2_fault_data_get_reason(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(data)->reason;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_fault_data_set_reason(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env,
    axis2_char_t *reason)
{
    sandesha2_fault_data_impl_t *fault_data_impl = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, reason, AXIS2_FAILURE);
    
    fault_data_impl = SANDESHA2_INTF_TO_IMPL(data);
    if(NULL != fault_data_impl->reason)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->reason);
        fault_data_impl->reason = NULL;
    }
    fault_data_impl->reason = AXIS2_STRDUP(reason, env);
    return AXIS2_SUCCESS;
}

axis2_char_t *AXIS2_CALL 
sandesha2_fault_data_get_sub_code( 
    sandesha2_fault_data_t *data,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(data)->sub_code;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_fault_data_set_sub_code(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env,
    axis2_char_t *sub_code)
{
    sandesha2_fault_data_impl_t *fault_data_impl = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, sub_code, AXIS2_FAILURE);
    
    fault_data_impl = SANDESHA2_INTF_TO_IMPL(data);
    if(NULL != fault_data_impl->sub_code)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->sub_code);
        fault_data_impl->sub_code = NULL;
    }
    fault_data_impl->sub_code = AXIS2_STRDUP(sub_code, env);
    return AXIS2_SUCCESS;
}
            
axis2_char_t *AXIS2_CALL 
sandesha2_fault_data_get_code(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(data)->code;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_fault_data_set_code(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env,
    axis2_char_t *code)
{
    sandesha2_fault_data_impl_t *fault_data_impl = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, code, AXIS2_FAILURE);
    
    fault_data_impl = SANDESHA2_INTF_TO_IMPL(data);
    if(NULL != fault_data_impl->code)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->code);
        fault_data_impl->code = NULL;
    }
    fault_data_impl->code = AXIS2_STRDUP(code, env);
    return AXIS2_SUCCESS;
}
            
int AXIS2_CALL 
sandesha2_fault_data_get_type(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return SANDESHA2_INTF_TO_IMPL(data)->type;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_fault_data_set_type(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env,
    int type)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    SANDESHA2_INTF_TO_IMPL(data)->type = type;
    return AXIS2_SUCCESS;
}
            
axis2_char_t *AXIS2_CALL 
sandesha2_fault_data_get_seq_id(
    sandesha2_fault_data_t *data,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(data)->seq_id;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_fault_data_set_seq_id( 
    sandesha2_fault_data_t *data,
    const axis2_env_t *env,
    axis2_char_t *seq_id)
{
    sandesha2_fault_data_impl_t *fault_data_impl = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    
    fault_data_impl = SANDESHA2_INTF_TO_IMPL(data);
    if(NULL != fault_data_impl->seq_id)
    {
        AXIS2_FREE(env->allocator, fault_data_impl->seq_id);
        fault_data_impl->seq_id = NULL;
    }
    fault_data_impl->seq_id = AXIS2_STRDUP(seq_id, env);
    return AXIS2_SUCCESS;
    
}
