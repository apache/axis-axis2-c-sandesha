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
 
#ifndef SANDESHA2_RM_ELEMENTS_H
#define SANDESHA2_RM_ELEMENTS_H

/**
  * @file sandesha2_rm_elements.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <sandesha2/sandesha2_create_seq.h>
#include <sandesha2/sandesha2_create_seq_res.h>
#include <sandesha2/sandesha2_terminate_seq.h>
#include <sandesha2/sandesha2_terminate_seq_res.h>
#include <sandesha2/sandesha2_close_seq.h>
#include <sandesha2/sandesha2_close_seq_res.h>
#include <sandesha2/sandesha2_seq_ack.h>
#include <sandesha2/sandesha2_ack_requested.h>
#include <sandesha2/sandesha2_seq.h>
#include <sandesha2/sandesha2_identifier.h>
#include <sandesha2/sandesha2_msg_number.h>



#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_util
 * @{
 */
 
 typedef struct sandesha2_rm_elements_ops sandesha2_rm_elements_ops_t;
 typedef struct sandesha2_rm_elements sandesha2_rm_elements_t;
 /**
 * @brief Sandesha2 RM Elements ops struct
 * Encapsulator struct for ops of sandesha2_rm_elements
 */
AXIS2_DECLARE_DATA struct sandesha2_rm_elements_ops
{
    
    axis2_status_t (AXIS2_CALL *
        from_soap_envelope) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope,
            axis2_char_t *action);
            
    axiom_soap_envelope_t* (AXIS2_CALL *
        to_soap_envelope) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            axiom_soap_envelope_t *soap_envelope);
            
    sandesha2_create_seq_t* (AXIS2_CALL *
        get_create_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_create_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_create_seq_t *create_seq);
    
    sandesha2_create_seq_res_t* (AXIS2_CALL *
        get_create_seq_res) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_create_seq_res) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_create_seq_res_t *create_seq_res);
            
    sandesha2_seq_t* (AXIS2_CALL *
        get_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_seq_t *seq);
            
    sandesha2_seq_ack_t* (AXIS2_CALL *
        get_seq_ack) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_seq_ack) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_seq_ack_t *seq_ack);
            
    sandesha2_terminate_seq_t* (AXIS2_CALL *
        get_terminate_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_terminate_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_terminate_seq_t *terminate_seq);
            
    sandesha2_terminate_seq_res_t* (AXIS2_CALL *
        get_terminate_seq_res) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_terminate_seq_res) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_terminate_seq_res_t *terminate_seq_res);
            
    sandesha2_ack_requested_t* (AXIS2_CALL *
        get_ack_requested) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_ack_requested) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_ack_requested_t *ack_requested);
    
    sandesha2_close_seq_t* (AXIS2_CALL *
        get_close_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_close_seq) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_close_seq_t *close_seq);
    
    sandesha2_close_seq_res_t* (AXIS2_CALL *
        get_close_seq_res) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_close_seq_res) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env,
            sandesha2_close_seq_res_t *close_seq_res);
    
    axis2_char_t* (AXIS2_CALL *
        get_addr_ns_val) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
    
    axis2_status_t (AXIS2_CALL *
        free) 
            (sandesha2_rm_elements_t *rm_elements,
            const axis2_env_t *env);
};

/**
 * @brief sandesha2_rm_elements_ops
 *    sandesha2_rm_elements_ops
 */
AXIS2_DECLARE_DATA struct sandesha2_rm_elements
{
    sandesha2_rm_elements_ops_t *ops;
};

AXIS2_EXTERN sandesha2_rm_elements_t* AXIS2_CALL
sandesha2_rm_elements_create(
						const axis2_env_t *env,
                        axis2_char_t *addr_ns_uri);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_RM_ELEMENTS_FREE(rm_elements, env) \
    ((rm_elements)->ops->free (rm_elements, env))

#define SANDESHA2_RM_ELEMENTS_FROM_SOAP_ENVELOPE(rm_elements, env, envelope, action) \
    ((rm_elements)->ops->from_soap_envelope (rm_elements, env, envelope, action))
    
#define SANDESHA2_RM_ELEMENTS_TO_SOAP_ENVELOPE(rm_elements, env, envelope) \
    ((rm_elements)->ops->to_soap_envelope (rm_elements, env, envelope))
    
#define SANDESHA2_RM_ELEMENTS_GET_CREATE_SEQ(rm_elements, env) \
    ((rm_elements)->ops->get_create_seq (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_CREATE_SEQ(rm_elements, env, element) \
    ((rm_elements)->ops->set_create_seq (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_CREATE_SEQ_RES(rm_elements, env) \
    ((rm_elements)->ops->get_create_seq_res (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_CREATE_SEQ_RES(rm_elements, env, element) \
    ((rm_elements)->ops->set_create_seq_res (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_SEQ(rm_elements, env) \
    ((rm_elements)->ops->get_sequence (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_SEQ(rm_elements, env, element) \
    ((rm_elements)->ops->set_sequence (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_SEQ_ACK(rm_elements, env) \
    ((rm_elements)->ops->get_seq_ack (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_SEQ_ACK(rm_elements, env, element) \
    ((rm_elements)->ops->set_seq_ack (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_TERMINATE_SEQ(rm_elements, env) \
    ((rm_elements)->ops->get_terminate_seq (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_TERMINATE_SEQ(rm_elements, env, element) \
    ((rm_elements)->ops->set_terminate_seq (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_TERMINATE_SEQ_RES(rm_elements, env) \
    ((rm_elements)->ops->get_terminate_seq_res (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_TERMINATE_SEQ_RES(rm_elements, env, element) \
    ((rm_elements)->ops->set_terminate_seq_res (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_ACK_REQUESTED(rm_elements, env) \
    ((rm_elements)->ops->get_ack_reqested (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_ACK_REQUESTED(rm_elements, env, element) \
    ((rm_elements)->ops->set_ack_reqested (rm_elements, env, element)
    
#define SANDESHA2_RM_ELEMENTS_GET_CLOSE_SEQ(rm_elements, env) \
    ((rm_elements)->ops->get_close_seq (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_CLOSE_SEQ(rm_elements, env, element) \
    ((rm_elements)->ops->set_close_seq (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_CLOSE_SEQ_RES(rm_elements, env) \
    ((rm_elements)->ops->get_close_seq_res (rm_elements, env))
    
#define SANDESHA2_RM_ELEMENTS_SET_CLOSE_SEQ_RES(rm_elements, env, element) \
    ((rm_elements)->ops->set_close_seq_res (rm_elements, env, element))
    
#define SANDESHA2_RM_ELEMENTS_GET_ADDR_NS_VAL(rm_elements, env) \
    ((rm_elements)->ops->get_addr_ns_val (rm_elements, env))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_RM_ELEMENTS_H*/
