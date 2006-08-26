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
#include <sandesha2_transport_sender.h>
#include <sandesha2_constants.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_utils.h>
#include <axis2_conf.h>
#include <axis2_string.h>


/** 
 * @brief Transport Sender struct impl
 *	Sandesha2 Transport Sender
 */
typedef struct sandesha2_transport_sender_impl sandesha2_transport_sender_impl_t;  
  
struct sandesha2_transport_sender_impl
{
    axis2_transport_sender_t transport_sender;
};

#define SANDESHA2_INTF_TO_IMPL(transport_sender) \
                        ((sandesha2_transport_sender_impl_t *)(transport_sender))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL
sandesha2_transport_sender_init
                        (axis2_transport_sender_t *transport_sender,
                        const axis2_env_t *env, 
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_transport_out_desc_t *transport_out);
            
axis2_status_t AXIS2_CALL
sandesha2_transport_sender_cleanup
                        (axis2_transport_sender_t *transport_sender,
                        const axis2_env_t *env,
                        axis2_msg_ctx_t *msg_ctx);
            
axis2_status_t AXIS2_CALL
sandesha2_transport_sender_invoke 
                        (axis2_transport_sender_t *transport_sender,
                        const axis2_env_t *env,
                        axis2_msg_ctx_t *msg_ctx);
            
axis2_status_t AXIS2_CALL 
sandesha2_transport_sender_free(axis2_transport_sender_t *transport_sender,
                        const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN axis2_transport_sender_t* AXIS2_CALL
sandesha2_transport_sender_create(const axis2_env_t *env)
{
    sandesha2_transport_sender_impl_t *transport_sender_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    transport_sender_impl =  (sandesha2_transport_sender_impl_t *)
                        AXIS2_MALLOC (env->allocator, 
                        sizeof(sandesha2_transport_sender_impl_t));
	
    if(NULL == transport_sender_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
        
    transport_sender_impl->transport_sender.ops = AXIS2_MALLOC(env->allocator,
                        sizeof(axis2_transport_sender_ops_t));
    if(NULL == transport_sender_impl->transport_sender.ops)
	{
		sandesha2_transport_sender_free((axis2_transport_sender_t*)
                        transport_sender_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    transport_sender_impl->transport_sender.ops->init = 
        sandesha2_transport_sender_init;
    transport_sender_impl->transport_sender.ops->cleanup = 
        sandesha2_transport_sender_cleanup;
    transport_sender_impl->transport_sender.ops->invoke = 
        sandesha2_transport_sender_invoke;
    transport_sender_impl->transport_sender.ops->free = 
        sandesha2_transport_sender_free;
                        
	return &(transport_sender_impl->transport_sender);
}


axis2_status_t AXIS2_CALL 
sandesha2_transport_sender_free(axis2_transport_sender_t *transport_sender, 
                        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
       
    if(NULL != transport_sender->ops)
    {
        AXIS2_FREE(env->allocator, transport_sender->ops);
        transport_sender->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(transport_sender));
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_transport_sender_init
                        (axis2_transport_sender_t *transport_sender,
                        const axis2_env_t *env, 
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_transport_out_desc_t *transport_out)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, transport_out, AXIS2_FAILURE);
    
    /* we have nothing to do here */
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL
sandesha2_transport_sender_cleanup
                        (axis2_transport_sender_t *transport_sender,
                        const axis2_env_t *env,
                        axis2_msg_ctx_t *msg_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
        
    /* we have nothing to do here */
    return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL
sandesha2_transport_sender_invoke 
                        (axis2_transport_sender_t *transport_sender,
                        const axis2_env_t *env,
                        axis2_msg_ctx_t *msg_ctx)
{
    axis2_property_t *property = NULL;
    axis2_transport_out_desc_t *out_desc = NULL;
    axis2_char_t *key = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *conf = NULL;
    sandesha2_storage_mgr_t *storage_man = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
        
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, AXIS2_FALSE);
                        
    if(NULL == property || NULL == AXIS2_PROPERTY_GET_VALUE(property, env))
        return AXIS2_FAILURE;
    out_desc = AXIS2_PROPERTY_GET_VALUE(property, env);
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(msg_ctx, env, out_desc);
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_MESSAGE_STORE_KEY, AXIS2_FALSE);
                        
    if(NULL == property || NULL == AXIS2_PROPERTY_GET_VALUE(property, env))
        return AXIS2_FAILURE;
    
    key = AXIS2_PROPERTY_GET_VALUE(property, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);
    storage_man = sandesha2_utils_get_storage_mgr(env, conf_ctx, conf);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, SANDESHA2_QUALIFIED_FOR_SENDING,
                        property, AXIS2_FALSE);
    SANDESHA2_STORAGE_MGR_UPDATE_MSG_CTX(storage_man, env, key, msg_ctx);
    return AXIS2_SUCCESS;
}

