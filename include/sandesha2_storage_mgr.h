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
 
#ifndef SANDESHA2_STORAGE_MGR_H
#define SANDESHA2_STORAGE_MGR_H

/**
  * @file sandesha2_storage_mgr.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axiom_soap_envelope.h>
#include <axis2_conf_ctx.h>
#include <axis2_module_desc.h>
#include <sandesha2_transaction.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_storage
 * @{
 */
 
typedef struct sandesha2_storage_mgr sandesha2_storage_mgr_t;
typedef struct sandesha2_storage_mgr_ops sandesha2_storage_mgr_ops_t;
struct sandesha2_rm_bean;
struct sandesha2_sender_mgr;
struct sandesha2_create_seq_mgr;
struct sandesha2_invoker_mgr;
struct sandesha2_seq_property_mgr;
struct sandesha2_next_msg_mgr;

 /**
 * @brief Storage Manager ops struct
 * Encapsulator struct for ops of sandesha2_storage_mgr
 */
AXIS2_DECLARE_DATA struct sandesha2_storage_mgr_ops
{ 
    axis2_status_t (AXIS2_CALL * 
            free)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *envv);

    AXIS2_EXTERN axis2_status_t (AXIS2_CALL *
            free_void_arg)(
                void *storage_mgr,
                const axis2_env_t *env);

    struct sandesha2_transaction *(AXIS2_CALL *
            get_transaction)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env);

    void (AXIS2_CALL *
            enlist_bean)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                struct sandesha2_rm_bean *rm_bean);

    struct sandesha2_create_seq_mgr *(AXIS2_CALL *
            get_create_seq_mgr)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env);

    struct sandesha2_next_msg_mgr *(AXIS2_CALL *
            get_next_msg_mgr)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env);

    struct sandesha2_sender_mgr *(AXIS2_CALL *
            get_retrans_mgr)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env);

    struct sandesha2_seq_property_mgr *(AXIS2_CALL *
            get_seq_property_mgr)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env);

    struct sandesha2_invoker_mgr *(AXIS2_CALL *
            get_storage_map_mgr)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
            set_ctx)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_conf_ctx_t *conf_ctx);

    axis2_conf_ctx_t *(AXIS2_CALL *
            get_ctx)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
            init)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_conf_ctx_t *conf_ctx);
        
    axis2_msg_ctx_t *(AXIS2_CALL *
            retrieve_msg_ctx)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_char_t *key,
                axis2_conf_ctx_t *conf_ctx);
            
    axis2_status_t (AXIS2_CALL *
            store_msg_ctx)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_char_t *key,
                axis2_msg_ctx_t *msg_ctx);
                
    axis2_status_t (AXIS2_CALL *
            update_msg_ctx)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_char_t *key,
                axis2_msg_ctx_t *msg_ctx);

    axis2_status_t (AXIS2_CALL *
            remove_msg_ctx)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_char_t *key);

    axis2_status_t (AXIS2_CALL *
            init_storage)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_module_desc_t *module_desc);

    axiom_soap_envelope_t *(AXIS2_CALL *
            retrieve_soap_envelope)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axis2_char_t *key);

    axis2_status_t (AXIS2_CALL *
            store_soap_envelope)(
                sandesha2_storage_mgr_t *storage_mgr,
                const axis2_env_t *env,
                axiom_soap_envelope_t *soap_env,
                axis2_char_t *key);
};

AXIS2_DECLARE_DATA struct sandesha2_storage_mgr
{
    const sandesha2_storage_mgr_ops_t *ops;
};

AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_storage_mgr_create(
    const axis2_env_t *env, 
    axis2_conf_ctx_t *conf_ctx);
 
/**
 * Frees the storage_mgr given as a void pointer. This method would cast the 
 * void parameter to an storage_mgr pointer and then call free method.
 * @param storage_mgr pointer to storage_mgr as a void pointer
 * @param env pointer to environment struct
 * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
 */
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_free_void_arg(
    void *storage_mgr,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL 
sandesha2_storage_mgr_free(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *envv);

struct sandesha2_transaction *AXIS2_CALL
sandesha2_storage_mgr_get_transaction(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env);

void AXIS2_CALL
sandesha2_storage_mgr_enlist_bean(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    struct sandesha2_rm_bean *rm_bean);

struct sandesha2_create_seq_mgr *AXIS2_CALL
sandesha2_storage_mgr_get_create_seq_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env);

struct sandesha2_next_msg_mgr *AXIS2_CALL
sandesha2_storage_mgr_get_next_msg_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env);

struct sandesha2_sender_mgr *AXIS2_CALL
sandesha2_storage_mgr_get_retrans_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env);

struct sandesha2_seq_property_mgr *AXIS2_CALL
sandesha2_storage_mgr_get_seq_property_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env);

struct sandesha2_invoker_mgr *AXIS2_CALL
sandesha2_storage_mgr_get_storage_map_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_set_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx);

axis2_conf_ctx_t *AXIS2_CALL
sandesha2_storage_mgr_get_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_init(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx);
	
axis2_msg_ctx_t *AXIS2_CALL
sandesha2_storage_mgr_retrieve_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx);
		
axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_store_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);
			
axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_update_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);

axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_remove_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_init_storage(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_module_desc_t *module_desc);

axiom_soap_envelope_t *AXIS2_CALL
sandesha2_storage_mgr_retrieve_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_storage_mgr_store_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_env,
    axis2_char_t *key);

                       
/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_STORAGE_MGR_H*/
