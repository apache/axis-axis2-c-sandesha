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

#include <axis2_sandesha2_mod.h>
#include <axis2_sandesha2.h>
#include <axis2_handler_desc.h>
#include <axis2_array_list.h>
#include <axis2_svc.h>
#include <axis2_msg_ctx.h>
#include <axis2_property.h>
#include <axis2_conf_ctx.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_transaction.h>
#include <sandesha2_msg_processor.h>
#include <sandesha2_constants.h>


axis2_status_t AXIS2_CALL
axis2_sandesha2_in_handler_invoke(
        struct axis2_handler *handler, 
        const axis2_env_t *env,
        struct axis2_msg_ctx *msg_ctx);
                                             
/******************************************************************************/                         

AXIS2_EXTERN axis2_handler_t* AXIS2_CALL
axis2_sandesha2_in_handler_create(
        const axis2_env_t *env, 
        axis2_qname_t *qname) 
{
    axis2_handler_t *handler = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    
    handler = axis2_handler_create(env);
    if (!handler)
    {
        return NULL;
    }
   
    /* handler init is handled by conf loading, so no need to do it here */
    
    /* set the base struct's invoke op */
    if (handler->ops) 
        handler->ops->invoke = axis2_sandesha2_in_handler_invoke;

    return handler;
}


axis2_status_t AXIS2_CALL
axis2_sandesha2_in_handler_invoke(
        struct axis2_handler *handler, 
        const axis2_env_t *env,
        struct axis2_msg_ctx *msg_ctx)
{
    axis2_property_t *property = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_char_t *done = NULL;
    axis2_char_t *reinjected_msg = NULL;
    axis2_char_t *within_transaction_str = NULL;
    axis2_bool_t within_transaction = AXIS2_FALSE;
    axis2_bool_t rolled_back = AXIS2_FALSE;
    axis2_svc_t *svc = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_transaction_t *transaction = NULL;
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    sandesha2_msg_processor_t *msg_processor = NULL;
    
    AXIS2_ENV_CHECK( env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    AXIS2_LOG_INFO(env->log, "Starting sandesha2 in handler .........");

    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    if(conf_ctx == NULL)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Configuration Context is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }

    return AXIS2_SUCCESS;
}

