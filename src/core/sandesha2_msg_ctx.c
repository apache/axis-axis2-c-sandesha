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
struct sandesha2_msg_ctx_t
{
    axis2_hash_t *msg_parts;
    int msg_type;
    axis2_char_t *rm_ns_val;
    axis2_char_t *addr_ns_val;
    axis2_char_t *spec_ver;
	axis2_msg_ctx_t *msg_ctx;
};

AXIS2_EXTERN sandesha2_msg_ctx_t* AXIS2_CALL
sandesha2_msg_ctx_create(
    const axis2_env_t *env, 
    axis2_msg_ctx_t *msg_ctx)
{
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    rm_msg_ctx =  (sandesha2_msg_ctx_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_msg_ctx_t));
	
    if(!rm_msg_ctx)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    rm_msg_ctx->rm_ns_val = NULL;
    rm_msg_ctx->addr_ns_val = NULL;
    rm_msg_ctx->msg_parts = NULL;
    rm_msg_ctx->msg_ctx = NULL;
    rm_msg_ctx->spec_ver = NULL;
    
    rm_msg_ctx->msg_parts = axis2_hash_make(env);
    rm_msg_ctx->msg_type = SANDESHA2_MSG_TYPE_UNKNOWN;
    rm_msg_ctx->msg_ctx = msg_ctx;

	return rm_msg_ctx;
}


axis2_status_t AXIS2_CALL 
sandesha2_msg_ctx_free(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env)
{
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(rm_msg_ctx->addr_ns_val)
    {
        AXIS2_FREE(env->allocator, rm_msg_ctx->addr_ns_val);
        rm_msg_ctx->addr_ns_val = NULL;
    }
    if(rm_msg_ctx->rm_ns_val)
    {
        AXIS2_FREE(env->allocator, rm_msg_ctx->rm_ns_val);
        rm_msg_ctx->rm_ns_val = NULL;
    }
    if(rm_msg_ctx->spec_ver)
    {
        AXIS2_FREE(env->allocator, rm_msg_ctx->spec_ver);
        rm_msg_ctx->spec_ver = NULL;
    }
    if(rm_msg_ctx->msg_parts)
    {
        axis2_hash_free(rm_msg_ctx->msg_parts, env);
        rm_msg_ctx->msg_parts = NULL;
    }
	AXIS2_FREE(env->allocator, rm_msg_ctx);
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_ctx(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env, 
    axis2_msg_ctx_t *msg_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    rm_msg_ctx->msg_ctx = msg_ctx;
    return AXIS2_SUCCESS;
}
            
axis2_msg_ctx_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_ctx(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return rm_msg_ctx->msg_ctx;
}
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_add_soap_envelope(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    int soap_ver = AXIOM_SOAP11;
    axis2_hash_index_t *hi = NULL;
    axiom_soap_envelope_t *soap_envelope = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(AXIS2_FALSE == AXIS2_MSG_CTX_GET_IS_SOAP_11(rm_msg_ctx->msg_ctx, env))
        soap_ver = AXIOM_SOAP12;
    soap_envelope = AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(rm_msg_ctx->msg_ctx, env);
    if(NULL == soap_envelope)
    {
        soap_envelope = axiom_soap_envelope_create_default_soap_envelope(env,
                        soap_ver);
        AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(rm_msg_ctx->msg_ctx, env, 
                        soap_envelope);
    }
    
    for(hi = axis2_hash_first(rm_msg_ctx->msg_parts, env); NULL != hi;
                        hi = axis2_hash_next(env, hi))
    {
        sandesha2_iom_rm_part_t *part = NULL;
        void *value = NULL;

        axis2_hash_this(hi, NULL, NULL, &value);
        part = (sandesha2_iom_rm_part_t*)value;
        sandesha2_iom_rm_part_to_soap_envelope(part, env, soap_envelope);
    }
    return AXIS2_SUCCESS;
}
            
int AXIS2_CALL
sandesha2_msg_ctx_get_msg_type (
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return rm_msg_ctx->msg_type;
}
            
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_type (
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, int msg_type)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    rm_msg_ctx->msg_type = msg_type;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_part (
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, int part_id,
    sandesha2_iom_rm_part_t *part)
{
    axis2_char_t *part_id_str = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, part, AXIS2_FAILURE);
    
    part_id_str = AXIS2_MALLOC(env->allocator, 32 * sizeof(axis2_char_t));
    sprintf(part_id_str, "%d", part_id);
    axis2_hash_set(rm_msg_ctx->msg_parts, part_id_str, AXIS2_HASH_KEY_STRING,
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
    AXIS2_ENV_CHECK(env, NULL);
    
    sprintf(part_id_str, "%d", part_id);
    return axis2_hash_get(rm_msg_ctx->msg_parts, part_id_str, 
                        AXIS2_HASH_KEY_STRING);
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_from(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return AXIS2_MSG_CTX_GET_FROM(rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_from(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, 
    axis2_endpoint_ref_t *from)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, from, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_TO(rm_msg_ctx->msg_ctx, env, from);;
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_to (
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return AXIS2_MSG_CTX_GET_TO(rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_to(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env, 
    axis2_endpoint_ref_t *to)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, to, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_TO(rm_msg_ctx->msg_ctx, env, to);
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_reply_to(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return AXIS2_MSG_CTX_GET_REPLY_TO(rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_reply_to(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, axis2_endpoint_ref_t *reply_to)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, reply_to, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_REPLY_TO(rm_msg_ctx->msg_ctx, env, reply_to);
}
    
axis2_endpoint_ref_t *AXIS2_CALL
sandesha2_msg_ctx_get_fault_to(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return AXIS2_MSG_CTX_GET_FAULT_TO(rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_fault_to(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, axis2_endpoint_ref_t *fault_to)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, fault_to, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_FAULT_TO(rm_msg_ctx->msg_ctx, env, fault_to);
}
    
axis2_relates_to_t *AXIS2_CALL
sandesha2_msg_ctx_get_relates_to(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return AXIS2_MSG_CTX_GET_RELATES_TO(rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_relates_to(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, axis2_relates_to_t *relates_to)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, relates_to, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_RELATES_TO(rm_msg_ctx->msg_ctx, env, relates_to);
}
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_msg_id(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return (axis2_char_t*)AXIS2_MSG_CTX_GET_WSA_MESSAGE_ID(
                        rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_msg_id(
    sandesha2_msg_ctx_t *rm_msg_ctx, 
    const axis2_env_t *env, 
    axis2_char_t *msg_id)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_MESSAGE_ID(rm_msg_ctx->msg_ctx, env, msg_id);
}
    
axiom_soap_envelope_t *AXIS2_CALL
sandesha2_msg_ctx_get_soap_envelope(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    
    return AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_soap_envelope(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, 
    axiom_soap_envelope_t *soap_envelope)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(rm_msg_ctx->msg_ctx, env, 
                        soap_envelope);
}
            
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_wsa_action(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
        
    return (axis2_char_t*)AXIS2_MSG_CTX_GET_WSA_ACTION(rm_msg_ctx->msg_ctx, env);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_wsa_action(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, axis2_char_t *action)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, action, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_WSA_ACTION(rm_msg_ctx->msg_ctx, env, action);
}
            
void *AXIS2_CALL
sandesha2_msg_ctx_get_property(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, axis2_char_t *key)
{
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, key, NULL);
    
    return AXIS2_MSG_CTX_GET_PROPERTY(rm_msg_ctx->msg_ctx, env, key,
                        AXIS2_FALSE);
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_property(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, 
    axis2_char_t *key, 
    void *val)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, key, AXIS2_FAILURE);
    
    return AXIS2_MSG_CTX_SET_PROPERTY(rm_msg_ctx->msg_ctx, env, key, val,
                        AXIS2_FALSE);
}
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_soap_action(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, axis2_char_t *soap_action)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_action, AXIS2_FAILURE);
    
    if(NULL == rm_msg_ctx->msg_ctx)
        return AXIS2_FAILURE;
    
    return AXIS2_MSG_CTX_SET_SOAP_ACTION(rm_msg_ctx->msg_ctx, env, 
                        soap_action);
}
    
axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_paused(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, 
    axis2_bool_t paused)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(NULL == rm_msg_ctx->msg_ctx)
        return AXIS2_FAILURE;
    return AXIS2_MSG_CTX_SET_PAUSED(rm_msg_ctx->msg_ctx, env, paused);    
}
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_rm_ns_val(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return rm_msg_ctx->rm_ns_val;
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_rm_ns_val(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, 
    axis2_char_t *ns_val)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, ns_val, AXIS2_FAILURE);
    
    if(NULL != rm_msg_ctx->rm_ns_val)
    {
        AXIS2_FREE(env->allocator, rm_msg_ctx->rm_ns_val);
        rm_msg_ctx->rm_ns_val = NULL;
    }
    rm_msg_ctx->rm_ns_val = AXIS2_STRDUP(ns_val, env);
    if(0 == AXIS2_STRCMP(ns_val, SANDESHA2_SPEC_2005_02_NS_URI))
        rm_msg_ctx->spec_ver = AXIS2_STRDUP(SANDESHA2_SPEC_VERSION_1_0, env);
    if(0 == AXIS2_STRCMP(ns_val, SANDESHA2_SPEC_2005_10_NS_URI))
        rm_msg_ctx->spec_ver = AXIS2_STRDUP(SANDESHA2_SPEC_VERSION_1_1, env);
        
    return AXIS2_SUCCESS;
}
    
axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_addr_ns_val(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return rm_msg_ctx->addr_ns_val;
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_addr_ns_val(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, axis2_char_t *ns_val)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, ns_val, AXIS2_FAILURE);
    
    if(NULL != rm_msg_ctx->addr_ns_val)
    {
        AXIS2_FREE(env->allocator, rm_msg_ctx->addr_ns_val);
        rm_msg_ctx->addr_ns_val = NULL;
    }
    rm_msg_ctx->addr_ns_val = AXIS2_STRDUP(ns_val, env);
    return AXIS2_SUCCESS;
}
            
int AXIS2_CALL
sandesha2_msg_ctx_get_flow(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    
    if(NULL == rm_msg_ctx->msg_ctx)
        return -1;
    return AXIS2_MSG_CTX_GET_FLOW(rm_msg_ctx->msg_ctx, env);     
}

axis2_status_t AXIS2_CALL
sandesha2_msg_ctx_set_flow(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env, int flow)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(NULL == rm_msg_ctx->msg_ctx)
        return AXIS2_FAILURE;
        
    return AXIS2_MSG_CTX_SET_FLOW(rm_msg_ctx->msg_ctx, env, flow); 
}

axis2_char_t *AXIS2_CALL
sandesha2_msg_ctx_get_rm_spec_ver(
    sandesha2_msg_ctx_t *rm_msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return rm_msg_ctx->spec_ver;
}

