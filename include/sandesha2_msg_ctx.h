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
 
#ifndef SANDESHA2_MSG_CTX_H
#define SANDESHA2_MSG_CTX_H

/**
  * @file sandesha2_msg_ctx.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axiom_soap_envelope.h>
#include <axis2_msg_ctx.h>
#include <axis2_endpoint_ref.h>
#include <axis2_relates_to.h>
#include <sandesha2_iom_rm_part.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_msg_ctx
 * @{
 */
 
 typedef struct sandesha2_msg_ctx_ops sandesha2_msg_ctx_ops_t;
 typedef struct sandesha2_msg_ctx sandesha2_msg_ctx_t;
 /**
 * @brief Sandesha2 Msg Context ops struct
 * Encapsulator struct for ops of sandesha2_msg_ctx
 */
AXIS2_DECLARE_DATA struct sandesha2_msg_ctx_ops
{
    axis2_status_t (AXIS2_CALL *
        set_msg_ctx) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_msg_ctx_t *msg_ctx);
            
    axis2_msg_ctx_t *(AXIS2_CALL *
        get_msg_ctx) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);
    
    axis2_status_t (AXIS2_CALL *
        add_soap_envelope) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);
            
    int (AXIS2_CALL *
        get_msg_type) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_msg_type) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            int msg_type);
            
    axis2_status_t (AXIS2_CALL *
        set_msg_part) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            int part_id,
            sandesha2_iom_rm_part_t *part);
            
    sandesha2_iom_rm_part_t *(AXIS2_CALL *
        get_msg_part) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            int part_id);
    
    axis2_endpoint_ref_t *(AXIS2_CALL *
        get_from) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_from) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_endpoint_ref_t *from);
    
    axis2_endpoint_ref_t *(AXIS2_CALL *
        get_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_endpoint_ref_t *to);
    
    axis2_endpoint_ref_t *(AXIS2_CALL *
        get_reply_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_reply_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_endpoint_ref_t *reply_to);
    
    axis2_endpoint_ref_t *(AXIS2_CALL *
        get_fault_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_fault_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_endpoint_ref_t *fault_to);
    
    axis2_relates_to_t *(AXIS2_CALL *
        get_relates_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_relates_to) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_relates_to_t *relates_to);
    
    axis2_char_t *(AXIS2_CALL *
        get_msg_id) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_msg_id) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_char_t *msg_id);
    
    axiom_soap_envelope_t *(AXIS2_CALL *
        get_soap_envelope) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_soap_envelope) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope);
            
    axis2_char_t *(AXIS2_CALL *
        get_wsa_action) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_wsa_action) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_char_t *action);
            
    void *(AXIS2_CALL *
        get_property) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_char_t *key);

    axis2_status_t (AXIS2_CALL *
        set_property) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_char_t *key,
            void *val);
    
    axis2_status_t (AXIS2_CALL *
        set_soap_action) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_char_t *soap_action);
    
    axis2_status_t (AXIS2_CALL *
        set_paused) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_bool_t paused);
    
    axis2_char_t *(AXIS2_CALL *
        get_rm_ns_val) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_rm_ns_val) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_char_t *ns_val);
    
    axis2_char_t *(AXIS2_CALL *
        get_addr_ns_val) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_addr_ns_val) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            axis2_char_t *ns_val);
            
    int (AXIS2_CALL *
        get_flow) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_flow) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env,
            int flow);
            
    axis2_char_t *(AXIS2_CALL *
    get_rm_spec_ver)
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);
                        
    axis2_status_t (AXIS2_CALL *
        free) 
            (sandesha2_msg_ctx_t *rm_msg_ctx,
            const axis2_env_t *env);
};

/**
 * @brief sandesha2_msg_ctx_ops
 *    sandesha2_msg_ctx_ops
 */
AXIS2_DECLARE_DATA struct sandesha2_msg_ctx
{
    sandesha2_msg_ctx_ops_t *ops;
};

AXIS2_EXTERN sandesha2_msg_ctx_t* AXIS2_CALL
sandesha2_msg_ctx_create(
						const axis2_env_t *env, 
					    axis2_msg_ctx_t *msg_ctx);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_MSG_CTX_FREE(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->free (rm_msg_ctx, env))
    
#define SANDESHA2_MSG_CTX_SET_MSG_CTX(rm_msg_ctx, env, msg_ctx) \
    ((rm_msg_ctx)->ops->set_msg_ctx(rm_msg_ctx, env, msg_ctx))
    
#define SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_msg_ctx(rm_msg_ctx, env))
    
#define SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->add_soap_envelope(rm_msg_ctx, env))
    
#define SANDESHA2_MSG_CTX_SET_MSG_TYPE(rm_msg_ctx, env, msg_type) \
    ((rm_msg_ctx)->ops->set_msg_type(rm_msg_ctx, env, msg_type))
    
#define SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_msg_type(rm_msg_ctx, env))

#define SANDESHA2_MSG_CTX_SET_MSG_PART(rm_msg_ctx, env, id, part) \
    ((rm_msg_ctx)->ops->set_msg_part(rm_msg_ctx, env, id, part))
    
#define SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, id) \
    ((rm_msg_ctx)->ops->get_msg_part(rm_msg_ctx, env, id))

#define SANDESHA2_MSG_CTX_SET_FROM(rm_msg_ctx, env, from) \
    ((rm_msg_ctx)->ops->set_from(rm_msg_ctx, env, from))
    
#define SANDESHA2_MSG_CTX_GET_FROM(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_from(rm_msg_ctx, env))

#define SANDESHA2_MSG_CTX_SET_TO(rm_msg_ctx, env, to) \
    ((rm_msg_ctx)->ops->set_to(rm_msg_ctx, env, to))

#define SANDESHA2_MSG_CTX_SET_TO(rm_msg_ctx, env, to) \
    ((rm_msg_ctx)->ops->set_to(rm_msg_ctx, env, to))
    
#define SANDESHA2_MSG_CTX_GET_TO(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_to(rm_msg_ctx, env))
    
#define SANDESHA2_MSG_CTX_GET_REPLY_TO(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_reply_to(rm_msg_ctx, env))

#define SANDESHA2_MSG_CTX_SET_FAULT_TO(rm_msg_ctx, env, fault_to) \
    ((rm_msg_ctx)->ops->set_fault_to(rm_msg_ctx, env, fault_to))
    
#define SANDESHA2_MSG_CTX_GET_FAULT_TO(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_fault_to(rm_msg_ctx, env))     

#define SANDESHA2_MSG_CTX_SET_RELATES_TO(rm_msg_ctx, env, relates_to) \
    ((rm_msg_ctx)->ops->set_relates_to(rm_msg_ctx, env, relates_to))
    
#define SANDESHA2_MSG_CTX_GET_RELATES_TO(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_relates_to(rm_msg_ctx, env))

#define SANDESHA2_MSG_CTX_SET_MSG_ID(rm_msg_ctx, env, msg_id) \
    ((rm_msg_ctx)->ops->set_msg_id(rm_msg_ctx, env, msg_id))
    
#define SANDESHA2_MSG_CTX_GET_MSG_ID(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_msg_id(rm_msg_ctx, env))
    
#define SANDESHA2_MSG_CTX_SET_SOAP_ENVELOPE(rm_msg_ctx, env, soap_envelope) \
    ((rm_msg_ctx)->ops->set_soap_envelope(rm_msg_ctx, env, soap_envelope))
    
#define SANDESHA2_MSG_CTX_GET_SOAP_ENVELOPE(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_soap_envelope(rm_msg_ctx, env))

#define SANDESHA2_MSG_CTX_SET_WSA_ACTION(rm_msg_ctx, env, wsa_action) \
    ((rm_msg_ctx)->ops->set_wsa_action(rm_msg_ctx, env, wsa_action))
    
#define SANDESHA2_MSG_CTX_GET_WSA_ACTION(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_wsa_action(rm_msg_ctx, env))

#define SANDESHA2_MSG_CTX_SET_PROPERTY(rm_msg_ctx, env, key, value) \
    ((rm_msg_ctx)->ops->set_property(rm_msg_ctx, env, key, value))
    
#define SANDESHA2_MSG_CTX_GET_PROPERTY(rm_msg_ctx, env, key) \
    ((rm_msg_ctx)->ops->get_property(rm_msg_ctx, env, key))
    
#define SANDESHA2_MSG_CTX_SET_SOAP_ACTION(rm_msg_ctx, env, soap_action) \
    ((rm_msg_ctx)->ops->set_soap_action(rm_msg_ctx, env, soap_action))
    
#define SANDESHA2_MSG_CTX_SET_PAUSED(rm_msg_ctx, env, paused) \
    ((rm_msg_ctx)->ops->set_paused(rm_msg_ctx, env, paused))
    
#define SANDESHA2_MSG_CTX_SET_RM_NS_VAL(rm_msg_ctx, env, ns_val) \
    ((rm_msg_ctx)->ops->set_rm_ns_val(rm_msg_ctx, env, ns_val))
    
#define SANDESHA2_MSG_CTX_GET_RM_NS_VAL(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_rm_ns_val(rm_msg_ctx, env))

#define SANDESHA2_MSG_CTX_SET_ADDR_NS_VAL(rm_msg_ctx, env, ns_val) \
    ((rm_msg_ctx)->ops->set_addr_ns_val(rm_msg_ctx, env, ns_val))
    
#define SANDESHA2_MSG_CTX_GET_ADDR_NS_VAL(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_addr_ns_val(rm_msg_ctx, env))
    
#define SANDESHA2_MSG_CTX_SET_FLOW(rm_msg_ctx, env, flow) \
    ((rm_msg_ctx)->ops->set_flow(rm_msg_ctx, env, flow))
    
#define SANDESHA2_MSG_CTX_GET_FLOW(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_flow(rm_msg_ctx, env))
    
#define SANDESHA2_MSG_CTX_GET_RM_SPEC_VER(rm_msg_ctx, env) \
    ((rm_msg_ctx)->ops->get_rm_spec_ver(rm_msg_ctx, env))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_MSG_CTX_H*/
