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

#include <axis2_handler_desc.h>
#include <axis2_array_list.h>
#include <axis2_svc.h>
#include <axis2_msg_ctx.h>
#include <axis2_property.h>
#include <axis2_conf_ctx.h>
#include <axis2_wsdl.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_transaction.h>
#include <sandesha2_msg_processor.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_app_msg_processor.h>
#include "../client/sandesha2_client_constants.h"

axis2_qname_t *AXIS2_CALL
sandesha2_out_handler_get_qname(
    struct axis2_handler *handler, 
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_out_handler_invoke(
    struct axis2_handler *handler, 
    const axis2_env_t *env,
    struct axis2_msg_ctx *msg_ctx);
                                         
/******************************************************************************/                         

AXIS2_EXTERN axis2_handler_t* AXIS2_CALL
sandesha2_out_handler_create(
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
        handler->ops->invoke = sandesha2_out_handler_invoke;

    return handler;
}


axis2_status_t AXIS2_CALL
sandesha2_out_handler_invoke(
    struct axis2_handler *handler, 
    const axis2_env_t *env,
    struct axis2_msg_ctx *msg_ctx)
{
    axis2_property_t *temp_prop = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_ctx_t *ctx = NULL;
    axis2_char_t *str_done = NULL;
    axis2_char_t *dummy_msg_str = NULL;
    axis2_char_t *within_transaction_str = NULL;
    axis2_bool_t within_transaction = AXIS2_FALSE;
    axis2_bool_t rolled_back = AXIS2_FALSE;
    axis2_bool_t dummy_msg = AXIS2_FALSE;
    axis2_svc_t *svc = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_transaction_t *transaction = NULL;
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    sandesha2_msg_processor_t *msg_processor = NULL;
    int msg_type = -1;
    
    AXIS2_ENV_CHECK( env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    AXIS2_LOG_INFO(env->log, "[sandesha2] Starting out handler .........");
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    if(!conf_ctx)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Configuration Context is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONF_CTX_NULL, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    svc = AXIS2_MSG_CTX_GET_SVC(msg_ctx, env);
    if(!svc)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Axis2 Service is NULL");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SVC_NULL, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    ctx = AXIS2_MSG_CTX_GET_BASE(msg_ctx, env);
    temp_prop = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_APPLICATION_PROCESSING_DONE, AXIS2_FALSE);
    if(temp_prop)
        str_done = (axis2_char_t *) AXIS2_PROPERTY_GET_VALUE(temp_prop, env); 
    if(str_done && 0 == AXIS2_STRCMP(SANDESHA2_VALUE_TRUE, str_done))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2] Exit: sandesha2_out_handler::invoke, Application \
                    Processing Done");
        return AXIS2_SUCCESS; 
    }
    temp_prop = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(temp_prop, env, AXIS2_SCOPE_APPLICATION);
    AXIS2_PROPERTY_SET_VALUE(temp_prop, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                env));
    AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_APPLICATION_PROCESSING_DONE, 
            temp_prop, AXIS2_FALSE);
    conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, conf);
    temp_prop = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_WITHIN_TRANSACTION, AXIS2_FALSE);
    if(temp_prop)
        within_transaction_str = (axis2_char_t *) AXIS2_PROPERTY_GET_VALUE(
                        temp_prop, env);
    if(within_transaction_str && 0 == AXIS2_STRCMP(SANDESHA2_VALUE_TRUE, 
                within_transaction_str))
    {
        within_transaction = AXIS2_TRUE;
    }
    if(!within_transaction)
    {
        axis2_property_t *prop = NULL;
        
        transaction = sandesha2_storage_mgr_get_transaction(storage_mgr, env);
        prop = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(prop, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(prop, env, SANDESHA2_VALUE_TRUE);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_WITHIN_TRANSACTION, prop, 
                AXIS2_FALSE);
    }
    /* Getting rm message */ 
    rm_msg_ctx = sandesha2_msg_init_init_msg(env, msg_ctx);
    temp_prop = AXIS2_CTX_GET_PROPERTY(ctx, env, SANDESHA2_CLIENT_DUMMY_MESSAGE, 
            AXIS2_FALSE);
    if(NULL != temp_prop)
        dummy_msg_str = (axis2_char_t *) AXIS2_PROPERTY_GET_VALUE(temp_prop, env); 
    if(dummy_msg_str && 0 == AXIS2_STRCMP(SANDESHA2_VALUE_TRUE, dummy_msg_str))
    {
        dummy_msg = AXIS2_TRUE;
    }
    msg_type = sandesha2_msg_ctx_get_msg_type(rm_msg_ctx, env);
    if(msg_type == SANDESHA2_MSG_TYPE_UNKNOWN)
    {
        axis2_msg_ctx_t *req_msg_ctx = NULL;
        axis2_op_ctx_t *op_ctx = NULL;

        op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env);
        req_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(op_ctx, env, 
                AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
        if(req_msg_ctx) /* For the server side */
        {
            sandesha2_msg_ctx_t *req_rm_msg_ctx = NULL;
            sandesha2_seq_t *seq_part = NULL;

            req_rm_msg_ctx = sandesha2_msg_init_init_msg(env, req_msg_ctx);
            seq_part = (sandesha2_seq_t *) sandesha2_msg_ctx_get_msg_part(
                    req_rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ);
            
            if(seq_part)
            {
                msg_processor = (sandesha2_msg_processor_t *) 
                sandesha2_app_msg_processor_create(env); /* rm intended msg */
            }
        }
        else if(!AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env))
        {
            msg_processor = (sandesha2_msg_processor_t *) 
            sandesha2_app_msg_processor_create(env);
        }
    }
    else
    {
        msg_processor = sandesha2_msg_processor_create_msg_processor(env, 
            rm_msg_ctx);
    }
    if(msg_processor)
    {
        sandesha2_msg_processor_process_out_msg(msg_processor, env, rm_msg_ctx);
    }
    if(AXIS2_SUCCESS != AXIS2_ERROR_GET_STATUS_CODE(env->error))
    {
        /* Message should not be sent in an exception situation */
        AXIS2_MSG_CTX_SET_PAUSED(msg_ctx, env, AXIS2_TRUE);
        /* Rolling back the transaction */
        if(!within_transaction)
        {
            axis2_property_t *prop = NULL;

            sandesha2_transaction_rollback(transaction, env);
            prop = axis2_property_create(env);
            AXIS2_PROPERTY_SET_SCOPE(prop, env, AXIS2_SCOPE_APPLICATION);
            AXIS2_PROPERTY_SET_VALUE(prop, env, SANDESHA2_VALUE_FALSE);
            AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_WITHIN_TRANSACTION, 
                    prop, AXIS2_FALSE);
            rolled_back = AXIS2_TRUE;
        }
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Error in processing the message");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_PROCESS_MSG, 
                AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    if(!within_transaction && !rolled_back)
    {
        /*axis2_property_t *prop = NULL;*/

        sandesha2_transaction_commit(transaction, env);
        /*prop = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(prop, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(prop, env, SANDESHA2_VALUE_FALSE);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_WITHIN_TRANSACTION, 
                prop, AXIS2_FALSE);*/
    }
    
    temp_prop = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_APPLICATION_PROCESSING_DONE, AXIS2_FALSE);
    if(temp_prop)
        AXIS2_PROPERTY_SET_VALUE(temp_prop, env, AXIS2_STRDUP(
            SANDESHA2_VALUE_FALSE, env));
    AXIS2_LOG_INFO(env->log, "[sandesha2] Exit: sandesha2_out_handler::invoke");
    return AXIS2_SUCCESS;
}

axis2_qname_t *AXIS2_CALL
sandesha2_out_handler_get_qname(
    struct axis2_handler *handler, 
    const axis2_env_t *env)
{
    return (axis2_qname_t *) axis2_qname_create(env, SANDESHA2_OUT_HANDLER_NAME, 
            NULL, NULL);
}

