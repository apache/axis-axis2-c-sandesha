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
#include <sandesha2_msg_ctx.h>
#include <sandesha2_constants.h>
#include <axis2_hash.h>
#include <axiom_soap_const.h>
#include <sandesha2_iom_rm_part.h>
#include <stdio.h>


/** 
 * @brief Msg Ctx struct impl
 *	Sandesha2 Message Context
 */
typedef struct sandesha2_msg_ctx_impl sandesha2_msg_ctx_impl_t;  
  
struct sandesha2_msg_ctx_impl
{
    sandesha2_msg_ctx_t rm_msg_ctx;
    axis2_hash_t *msg_parts;
    int msg_type;
    axis2_char_t *rm_ns_val;
    axis2_char_t *addr_ns_val;
    axis2_char_t *spec_ver;
	axis2_msg_ctx_t *msg_ctx;
};

#define SANDESHA2_INTF_TO_IMPL(rm_msg_ctx) \
                        ((sandesha2_msg_ctx_impl_t *)(rm_msg_ctx))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_ctx(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env, axis2_msg_ctx_t *msg_ctx);
            
axis2_msg_ctx_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_ctx(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env);
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_add_soap_envelope(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);
            
int AXIS2_CALL
sandesha2_msg_ctx_get_msg_type (sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_type (sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, int msg_type);
            
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_part (sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, int part_id,
                        sandesha2_iom_rm_part_t *part);
            
sandesha2_iom_rm_part_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_part(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, int part_id);
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_from(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_from(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_endpoint_ref_t *from);
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_to (sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_to(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env, axis2_endpoint_ref_t *to);
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_reply_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_reply_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_endpoint_ref_t *reply_to);
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_fault_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_fault_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_endpoint_ref_t *fault_to);
    
axis2_relates_to_t *AXIS2_CALL
sandesha2_msg_ctx_get_relates_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_relates_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_relates_to_t *relates_to);
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_id(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_id(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env, 
    axis2_char_t *msg_id);
    
axiom_soap_envelope_t *AXIS2_CALL
sandesha2_msg_ctx_get_soap_envelope(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_soap_envelope(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, 
                        axiom_soap_envelope_t *soap_envelope);
            
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_wsa_action(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_wsa_action(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *action);
            
void *AXIS2_CALL
sandesha2_msg_ctx_get_property(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_property(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *key, void *val);
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_soap_action(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *soap_action);
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_paused(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_bool_t paused);
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_rm_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_rm_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *ns_val);
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_addr_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_addr_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *ns_val);
            
int AXIS2_CALL
sandesha2_msg_ctx_get_flow(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_flow(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, int flow);
                        
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_rm_spec_ver(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_msg_ctx_free(sandesha2_msg_ctx_t *rm_msg_ctx, const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_ctx_t* AXIS2_CALL
sandesha2_msg_ctx_create(const axis2_env_t *env, axis2_msg_ctx_t *msg_ctx)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl =  (sandesha2_msg_ctx_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_msg_ctx_impl_t));
	
    if(NULL == msg_ctx_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    msg_ctx_impl->rm_ns_val = NULL;
    msg_ctx_impl->addr_ns_val = NULL;
    msg_ctx_impl->msg_parts = NULL;
    msg_ctx_impl->msg_ctx = NULL;
    msg_ctx_impl->spec_ver = NULL;
    msg_ctx_impl->rm_msg_ctx.ops = NULL;
    
    msg_ctx_impl->rm_msg_ctx.ops = AXIS2_MALLOC(env->allocator,
                        sizeof(sandesha2_msg_ctx_ops_t));
    if(NULL == msg_ctx_impl->rm_msg_ctx.ops)
	{
		sandesha2_msg_ctx_free((sandesha2_msg_ctx_t*)msg_ctx_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_ctx_impl->msg_parts = axis2_hash_make(env);
    msg_ctx_impl->msg_type = SANDESHA2_MSG_TYPE_UNKNOWN;
    msg_ctx_impl->msg_ctx = msg_ctx;

    
    msg_ctx_impl->rm_msg_ctx.ops->set_msg_ctx = sandesha2_msg_ctx_set_msg_ctx;
    msg_ctx_impl->rm_msg_ctx.ops->get_msg_ctx = sandesha2_msg_ctx_get_msg_ctx;
    msg_ctx_impl->rm_msg_ctx.ops->add_soap_envelope = 
                        sandesha2_msg_ctx_add_soap_envelope;
    msg_ctx_impl->rm_msg_ctx.ops->get_msg_type = sandesha2_msg_ctx_get_msg_type;
    msg_ctx_impl->rm_msg_ctx.ops->set_msg_type = sandesha2_msg_ctx_set_msg_type;
    msg_ctx_impl->rm_msg_ctx.ops->set_msg_part = sandesha2_msg_ctx_set_msg_part;
    msg_ctx_impl->rm_msg_ctx.ops->get_msg_part = sandesha2_msg_ctx_get_msg_part;
    msg_ctx_impl->rm_msg_ctx.ops->get_from = sandesha2_msg_ctx_get_from;
    msg_ctx_impl->rm_msg_ctx.ops->set_from = sandesha2_msg_ctx_set_from;
    msg_ctx_impl->rm_msg_ctx.ops->get_to = sandesha2_msg_ctx_get_to;
    msg_ctx_impl->rm_msg_ctx.ops->set_to = sandesha2_msg_ctx_set_to;
    msg_ctx_impl->rm_msg_ctx.ops->get_reply_to = sandesha2_msg_ctx_get_reply_to;
    msg_ctx_impl->rm_msg_ctx.ops->set_reply_to = sandesha2_msg_ctx_set_reply_to;
    msg_ctx_impl->rm_msg_ctx.ops->get_fault_to = sandesha2_msg_ctx_get_fault_to;
    msg_ctx_impl->rm_msg_ctx.ops->set_fault_to = sandesha2_msg_ctx_set_fault_to;
    msg_ctx_impl->rm_msg_ctx.ops->get_relates_to = 
                        sandesha2_msg_ctx_get_relates_to;
    msg_ctx_impl->rm_msg_ctx.ops->set_relates_to = 
                        sandesha2_msg_ctx_set_relates_to;
    msg_ctx_impl->rm_msg_ctx.ops->get_msg_id = sandesha2_msg_ctx_get_msg_id;
    msg_ctx_impl->rm_msg_ctx.ops->set_msg_id = sandesha2_msg_ctx_set_msg_id;
    msg_ctx_impl->rm_msg_ctx.ops->get_soap_envelope = 
                        sandesha2_msg_ctx_get_soap_envelope;
    msg_ctx_impl->rm_msg_ctx.ops->set_soap_envelope = 
                        sandesha2_msg_ctx_set_soap_envelope;
    msg_ctx_impl->rm_msg_ctx.ops->get_wsa_action = 
                        sandesha2_msg_ctx_get_wsa_action;
    msg_ctx_impl->rm_msg_ctx.ops->set_wsa_action = 
                        sandesha2_msg_ctx_set_wsa_action;
    msg_ctx_impl->rm_msg_ctx.ops->get_property = sandesha2_msg_ctx_get_property;
    msg_ctx_impl->rm_msg_ctx.ops->set_property = sandesha2_msg_ctx_set_property;
    msg_ctx_impl->rm_msg_ctx.ops->set_soap_action = 
                        sandesha2_msg_ctx_set_soap_action;
    msg_ctx_impl->rm_msg_ctx.ops->set_paused = sandesha2_msg_ctx_set_paused;
    msg_ctx_impl->rm_msg_ctx.ops->get_rm_ns_val = 
                        sandesha2_msg_ctx_get_rm_ns_val;
    msg_ctx_impl->rm_msg_ctx.ops->set_rm_ns_val = 
                        sandesha2_msg_ctx_set_rm_ns_val;
    msg_ctx_impl->rm_msg_ctx.ops->get_addr_ns_val = 
                        sandesha2_msg_ctx_get_addr_ns_val;
    msg_ctx_impl->rm_msg_ctx.ops->set_addr_ns_val = 
                        sandesha2_msg_ctx_set_addr_ns_val;
    msg_ctx_impl->rm_msg_ctx.ops->get_flow = sandesha2_msg_ctx_get_flow;
    msg_ctx_impl->rm_msg_ctx.ops->set_flow = sandesha2_msg_ctx_set_flow;
    msg_ctx_impl->rm_msg_ctx.ops->get_rm_spec_ver = 
                        sandesha2_msg_ctx_get_rm_spec_ver;
    msg_ctx_impl->rm_msg_ctx.ops->free = sandesha2_msg_ctx_free;
                        
	return &(msg_ctx_impl->rm_msg_ctx);
}


axis2_status_t AXIS2_CALL 
sandesha2_msg_ctx_free(sandesha2_msg_ctx_t *rm_msg_ctx, const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    
    if(NULL != msg_ctx_impl->addr_ns_val)
    {
        AXIS2_FREE(env->allocator, msg_ctx_impl->addr_ns_val);
        msg_ctx_impl->addr_ns_val = NULL;
    }
    if(NULL != msg_ctx_impl->rm_ns_val)
    {
        AXIS2_FREE(env->allocator, msg_ctx_impl->rm_ns_val);
        msg_ctx_impl->rm_ns_val = NULL;
    }
    if(NULL != msg_ctx_impl->spec_ver)
    {
        AXIS2_FREE(env->allocator, msg_ctx_impl->spec_ver);
        msg_ctx_impl->spec_ver = NULL;
    }
    if(NULL != msg_ctx_impl->msg_parts)
    {
        axis2_hash_free(msg_ctx_impl->msg_parts, env);
        msg_ctx_impl->msg_parts = NULL;
    }
    if(NULL != rm_msg_ctx->ops)
    {
        AXIS2_FREE(env->allocator, rm_msg_ctx->ops);
        rm_msg_ctx->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(rm_msg_ctx));
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_ctx(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env, axis2_msg_ctx_t *msg_ctx)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    msg_ctx_impl->msg_ctx = msg_ctx;
    return AXIS2_SUCCESS;
}
            
axis2_msg_ctx_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_ctx(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(rm_msg_ctx)->msg_ctx;
}
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_add_soap_envelope(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    int soap_ver = AXIOM_SOAP11;
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    axis2_hash_index_t *hi = NULL;
    axiom_soap_envelope_t *soap_envelope = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    
    if(AXIS2_FALSE == AXIS2_MSG_CTX_GET_IS_SOAP_11(msg_ctx_impl->msg_ctx, env))
        soap_ver = AXIOM_SOAP12;
    soap_envelope = AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(msg_ctx_impl->msg_ctx, env);
    if(NULL == soap_envelope)
    {
        soap_envelope = axiom_soap_envelope_create_default_soap_envelope(env,
                        soap_ver);
        AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(msg_ctx_impl->msg_ctx, env, 
                        soap_envelope);
    }
    
    for(hi = axis2_hash_first(msg_ctx_impl->msg_parts, env); NULL != hi;
                        hi = axis2_hash_next(env, hi))
    {
        sandesha2_iom_rm_part_t *part = NULL;
        void *value = NULL;

        axis2_hash_this(hi, NULL, NULL, &value);
        part = (sandesha2_iom_rm_part_t*)value;
        SANDESHA2_IOM_RM_PART_TO_SOAP_ENVELOPE(part, env, soap_envelope);
    }
    return AXIS2_SUCCESS;
}
            
int AXIS2_CALL
sandesha2_msg_ctx_get_msg_type (sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return SANDESHA2_INTF_TO_IMPL(rm_msg_ctx)->msg_type;
}
            
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_type (sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, int msg_type)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    msg_ctx_impl->msg_type = msg_type;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_part (sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, int part_id,
                        sandesha2_iom_rm_part_t *part)
{
    axis2_char_t *part_id_str = NULL;
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, part, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    part_id_str = AXIS2_MALLOC(env->allocator, 32 * sizeof(axis2_char_t));
    sprintf(part_id_str, "%d", part_id);
    axis2_hash_set(msg_ctx_impl->msg_parts, part_id_str, AXIS2_HASH_KEY_STRING,
                        (const void*)part);
    return AXIS2_SUCCESS;
}
            
sandesha2_iom_rm_part_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_part(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, 
    int part_id)
{
    axis2_char_t part_id_str[32];
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    sprintf(part_id_str, "%d", part_id);
    return axis2_hash_get(msg_ctx_impl->msg_parts, part_id_str, 
                        AXIS2_HASH_KEY_STRING);
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_from(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_FROM(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_from(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_endpoint_ref_t *from)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, from, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_TO(msg_ctx_impl->msg_ctx, env, from);;
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_to (sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_TO(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_to(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env, axis2_endpoint_ref_t *to)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, to, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_TO(msg_ctx_impl->msg_ctx, env, to);
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_reply_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_REPLY_TO(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_reply_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_endpoint_ref_t *reply_to)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, reply_to, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_REPLY_TO(msg_ctx_impl->msg_ctx, env, reply_to);
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_fault_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_FAULT_TO(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_fault_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_endpoint_ref_t *fault_to)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, fault_to, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_FAULT_TO(msg_ctx_impl->msg_ctx, env, fault_to);
}
    
axis2_relates_to_t *AXIS2_CALL
sandesha2_msg_ctx_get_relates_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_RELATES_TO(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_relates_to(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_relates_to_t *relates_to)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, relates_to, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_RELATES_TO(msg_ctx_impl->msg_ctx, env, relates_to);
}
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_id(sandesha2_msg_ctx_t *rm_msg_ctx, 
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_WSA_MESSAGE_ID(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_id(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env, 
    axis2_char_t *msg_id)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_MESSAGE_ID(msg_ctx_impl->msg_ctx, env, msg_id);
}
    
axiom_soap_envelope_t *AXIS2_CALL
sandesha2_msg_ctx_get_soap_envelope(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_soap_envelope(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, 
                        axiom_soap_envelope_t *soap_envelope)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(msg_ctx_impl->msg_ctx, env, 
                        soap_envelope);
}
            
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_wsa_action(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
        
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    
    return AXIS2_MSG_CTX_GET_WSA_ACTION(msg_ctx_impl->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_wsa_action(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *action)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, action, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    
    return AXIS2_MSG_CTX_SET_WSA_ACTION(msg_ctx_impl->msg_ctx, env, action);
}
            
void *AXIS2_CALL
sandesha2_msg_ctx_get_property(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *key)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, key, NULL);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx_impl->msg_ctx, env, key,
                        AXIS2_FALSE);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_property(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, 
    axis2_char_t *key, 
    void *val)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, key, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    return AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx_impl->msg_ctx, env, key, val,
                        AXIS2_FALSE);
}
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_soap_action(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *soap_action)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_action, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    if(NULL == msg_ctx_impl->msg_ctx)
        return AXIS2_FAILURE;
    
    return AXIS2_MSG_CTX_SET_SOAP_ACTION(msg_ctx_impl->msg_ctx, env, 
                        soap_action);
}
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_paused(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_bool_t paused)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    if(NULL == msg_ctx_impl->msg_ctx)
        return AXIS2_FAILURE;
    return AXIS2_MSG_CTX_SET_PAUSED(msg_ctx_impl->msg_ctx, env, paused);    
}
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_rm_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(rm_msg_ctx)->rm_ns_val;
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_rm_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *ns_val)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, ns_val, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    if(NULL != msg_ctx_impl->rm_ns_val)
    {
        AXIS2_FREE(env->allocator, msg_ctx_impl->rm_ns_val);
        msg_ctx_impl->rm_ns_val = NULL;
    }
    msg_ctx_impl->rm_ns_val = AXIS2_STRDUP(ns_val, env);
    if(0 == AXIS2_STRCMP(ns_val, SANDESHA2_SPEC_2005_02_NS_URI))
        msg_ctx_impl->spec_ver = AXIS2_STRDUP(SANDESHA2_SPEC_VERSION_1_0, env);
    if(0 == AXIS2_STRCMP(ns_val, SANDESHA2_SPEC_2005_10_NS_URI))
        msg_ctx_impl->spec_ver = AXIS2_STRDUP(SANDESHA2_SPEC_VERSION_1_1, env);
        
    return AXIS2_SUCCESS;
}
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_addr_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(rm_msg_ctx)->addr_ns_val;
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_addr_ns_val(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, axis2_char_t *ns_val)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, ns_val, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    if(NULL != msg_ctx_impl->addr_ns_val)
    {
        AXIS2_FREE(env->allocator, msg_ctx_impl->addr_ns_val);
        msg_ctx_impl->addr_ns_val = NULL;
    }
    msg_ctx_impl->addr_ns_val = AXIS2_STRDUP(ns_val, env);
    return AXIS2_SUCCESS;
}
            
int AXIS2_CALL
sandesha2_msg_ctx_get_flow(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, -1);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    if(NULL == msg_ctx_impl->msg_ctx)
        return -1;
    return AXIS2_MSG_CTX_GET_FLOW(msg_ctx_impl->msg_ctx, env);     
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_flow(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env, int flow)
{
    sandesha2_msg_ctx_impl_t *msg_ctx_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    msg_ctx_impl = SANDESHA2_INTF_TO_IMPL(rm_msg_ctx);
    if(NULL == msg_ctx_impl->msg_ctx)
        return AXIS2_FAILURE;
        
    return AXIS2_MSG_CTX_SET_FLOW(msg_ctx_impl->msg_ctx, env, flow); 
}

axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_rm_spec_ver(sandesha2_msg_ctx_t *rm_msg_ctx,
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return SANDESHA2_INTF_TO_IMPL(rm_msg_ctx)->spec_ver;
}
