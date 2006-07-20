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
#include <stdio.h>


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
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, action, AXIS2_FAILURE);
    
    return AXIS2_SUCCESS;
}
            
axiom_soap_envelope_t* AXIS2_CALL 
sandesha2_rm_elements_to_soap_envelope
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope)
{
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, NULL);
    
    return NULL;
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
