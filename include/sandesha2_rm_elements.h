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
#include <axiom_envelope.h>
#include <sandesha2_seq.h>
#include <sandesha2_seq_ack.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_create_seq_res.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_terminate_seq_res.h>
#include <sandesha2_close_seq.h>
#include <sandesha2_close_seq_res.h>
#include <sandesha2_ack_requested.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_rm_elements
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
    free) (
        void *rm_elements,
        const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    from_soap_envelope) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        axiom_soap_envelope_t *envelope,
        axis2_char_t *action);

    axiom_soap_envelope_t *(AXIS2_CALL *
    to_soap_envelope) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        axiom_soap_envelope_t *envelope);
	
    sandesha2_create_seq_t *(AXIS2_CALL *
    get_create_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    sandesha2_create_seq_res_t *(AXIS2_CALL *
    get_create_seq_response) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    sandesha22_seq_t *(AXIS2_CALL *
    get_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    sandesha2_seq_ack_t *(AXIS2_CALL *
    get_seq_ack) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    sandesha2_terminate_seq_t *(AXIS2_CALL *
    get_terminate_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    sandesha2_terminate_seq_res_t *(AXIS2_CALL *
    get_terminate_seq_response) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);
	
    axis2_status_t (AXIS2_CALL *
    set_create_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_create_seq_t *create_seq);

    axis2_status_t (AXIS2_CALL *
    set_create_seq_response) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_create_seq_res_t *create_seq_res);

    axis2_status_t (AXIS2_CALL *
    set_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_seq_t *seq);

    axis2_status_t (AXIS2_CALL *
    set_seq_ack) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_seq_ack_t *seq_ack);

    axis2_status_t (AXIS2_CALL *
    set_terminate_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_terminate_seq_t *terminate_seq);

    axis2_status_t (AXIS2_CALL *
    set_termiate_seq_response) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_terminate_seq_res_t *terminate_seq_res);
	
    sandesha2_ack_requested_t *(AXIS2_CALL *
    get_ack_requested) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    set_ack_requested) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_ack_requested_t *ack_requested);

    sandesha2_close_seq_t *(AXIS2_CALL *
    get_close_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    set_close_seq) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_close_seq_t *close_seq);

    sandesha2_close_seq_res_t *(AXIS2_CALL *
    get_close_seq_response) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    set_close_seq_response) (
        sandesha2_rm_elements_t *rm_elements,
        const axis2_env_t *env,
        sandesha2_close_seq_res_t *close_seq_res);

    axis2_char_t *(AXIS2_CALL *
    get_addressing_ns_value) (
        sandesha2_rm_elements_t *rm_elements,
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
        const axis2_env_t *env);

/************************** Start of function macros **************************/
#define SANDESHA2_RM_ELEMENTS_FREE(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->free (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_FROM_SOAP_ENVELOPE(storage_man, env, envelope, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     from_soap_envelope (storage_man, env, envelope, env))

#define SANDESHA2_RM_ELEMENTS_TO_SOAP_ENVELOPE(storage_man, env, envelope) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     to_soap_envelope (storage_man, env, envelope))

#define SANDESHA2_RM_ELEMENTS_GET_CREATE_SEQ(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_create_seq (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_GET_CREATE_SEQ_RESPONSE(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_create_seq_response (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_GET_SEQ(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_seq (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_GET_SEQ_ACK(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_seq_ack (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_GET_TERMINATE_SEQ(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_terminate_seq (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_GET_TERMINATE_SEQ_RESPONSE(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_terminate_seq_response (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_SET_CREATE_SEQ(storage_man, env, create_seq) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_create_seq (storage_man, env, create_seq))

#define SANDESHA2_RM_ELEMENTS_SET_CREATE_SEQ_RESPONSE(storage_man, env, create_seq_res) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_create_seq_response (storage_man, env, create_seq_res))

#define SANDESHA2_RM_ELEMENTS_SET_SEQ(storage_man, env, seq) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_seq (storage_man, env, seq))

#define SANDESHA2_RM_ELEMENTS_SET_SEQ_ACK(storage_man, env, seq_ack) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_seq_ack (storage_man, env, seq_ack))

#define SANDESHA2_RM_ELEMENTS_SET_TERMINATE_SEQ(storage_man, env, terminate_seq) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_terminate_seq (storage_man, env, terminate_seq))

#define SANDESHA2_RM_ELEMENTS_SET_TERMINATE_SEQ_RESPONSE(storage_man, env, terminate_seq_ack) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_terminate_seq_response (storage_man, env, terminate_seq_ack))

#define SANDESHA2_RM_ELEMENTS_GET_ACK_REQUESTED(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_ack_requested (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_SET_ACK_REQUESTED(storage_man, env, ack_requested) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_ack_requested (storage_man, env, ack_requested))

#define SANDESHA2_RM_ELEMENTS_GET_CLOSE_SEQ(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_close_seq (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_SET_CLOSE_SEQ(storage_man, env, close_seq) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_close_seq (storage_man, env, close_seq))

#define SANDESHA2_RM_ELEMENTS_GET_CLOSE_SEQ_RESPONSE(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_close_seq_response (storage_man, env))

#define SANDESHA2_RM_ELEMENTS_SET_CLOSE_SEQ_RESPONSE(storage_man, env, close_seq_res) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     set_close_seq_response (storage_man, env, close_seq_res))

#define SANDESHA2_RM_ELEMENTS_GET_ADDRESSING_NS_VALUE(storage_man, env) \
    (((sandesha2_rm_elements_t *) storage_man)->ops->\
     get_addressing_ns_value (storage_man, env))

/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_RM_ELEMENTS_H*/
