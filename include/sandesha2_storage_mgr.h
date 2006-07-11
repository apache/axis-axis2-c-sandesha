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
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_invoker_mgr.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_storage_mgr
 * @{
 */
 
 typedef struct sandesha2_storage_mgr_ops sandesha2_storage_mgr_ops_t;
 typedef struct sandesha2_storage_mgr sandesha2_storage_mgr_t;
 /**
 * @brief Sandesha2 Storage Manager ops struct
 * Encapsulator struct for ops of sandesha2_storage_mgr
 */
AXIS2_DECLARE_DATA struct sandesha2_storage_mgr_ops
{
    
    axis2_conf_ctx_t* (AXIS2_CALL *
        get_context) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_context) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env,
            axis2_conf_ctx_t *conf_ctx);
            
    axis2_status_t (AXIS2_CALL *
        init_storage) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env,
            axis2_module_desc_t *module_desc);
            
    sandesha2_transaction_t* (AXIS2_CALL *
        get_transaction) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);
            
    sandesha2_create_seq_mgr_t* (AXIS2_CALL *
        get_create_seq_mgr) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);
    
    sandesha2_next_msg_mgr_t* (AXIS2_CALL *
        get_next_msg_mgr) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);

    sandesha2_sender_mgr_t* (AXIS2_CALL *
        get_retrans_mgr) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);
    
    sandesha2_seq_property_mgr_t* (AXIS2_CALL *
        get_seq_property_mgr) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);

    sandesha2_invoker_mgr_t* (AXIS2_CALL *
        get_storage_map_mgr) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        store_msg_ctx) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env,
            axis2_char_t *storage_key,
            axis2_msg_ctx_t *msg_ctx);
    
    axis2_status_t (AXIS2_CALL *
        update_msg_ctx) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env,
            axis2_char_t *storage_key,
            axis2_msg_ctx_t *msg_ctx);
    
    axis2_msg_ctx_t* (AXIS2_CALL *
        retrieve_msg_ctx) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env,
            axis2_char_t *storage_key,
            axis2_conf_ctx_t *conf_ctx);
            
    axis2_status_t (AXIS2_CALL *
        remove_msg_ctx) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env,
            axis2_char_t *storage_key);
            
    axis2_status_t (AXIS2_CALL *
        free) 
            (sandesha2_storage_mgr_t *storage_man,
            const axis2_env_t *env);
};

/**
 * @brief sandesha2_storage_mgr_ops
 *    sandesha2_storage_mgr_ops
 */
AXIS2_DECLARE_DATA struct sandesha2_storage_mgr
{
    sandesha2_storage_mgr_ops_t *ops;
};

AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_storage_mgr_create(
						const axis2_env_t *env, 
					    axis2_conf_ctx_t *conf_ctx);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_STORAGE_MGR_FREE(storage_man, env) \
    ((storage_man)->ops->free (storage_man, env))
    
#define SANDESHA2_STORAGE_MGR_SET_CONEXT(storage_man, env, conf_ctx) \
    ((storage_man)->ops->set_context(storage_man, env, conf_ctx))
    
#define SANDESHA2_STORAGE_MGR_GET_CONEXT(storage_man, env) \
    ((storage_man)->ops->get_context(storage_man, env))
    
#define SANDESHA2_STORAGE_MGR_INIT_STORAGE(storage_man, env, module_desc) \
    ((storage_man)->ops->init_storage(storage_man, env, module_desc))
    
#define SANDESHA2_STORAGE_MGR_GET_TRANSACTION(storage_man, env) \
    ((storage_man)->ops->get_transaction(storage_man, env))
    
#define SANDESHA2_STORAGE_MGR_GET_CREATE_SEQ_BEAN_MGR(storage_man, env)\
    ((storage_man)->ops->get_create_seq_mgr(storage_man, env))
    
#define SANDESHA2_STORAGE_MGR_GET_NEXT_MSG_BEAN_MGR(storage_man, env)\
    ((storage_man)->ops->get_next_msg_mgr(storage_man, env))
    
#define SANDESHA2_STORAGE_MGR_GET_RETRANS_BEAN_MGR(storage_man, env)\
    ((storage_man)->ops->get_retrans_mgr(storage_man, env))

#define SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_BEAN_MGR(storage_man, env)\
    ((storage_man)->ops->get_seq_property_mgr(storage_man, env))
    
#define SANDESHA2_STORAGE_MGR_GET_STORAGE_MAP_BEAN_MGR(storage_man, env)\
    ((storage_man)->ops->get_storage_map_mgr(storage_man, env))
    
#define SANDESHA2_STORAGE_MGR_STORE_MSG_CTX(storage_man, env, storage_key, \
    msg_ctx) \
    ((storage_man)->ops->store_msg_ctx(storage_man, env, storage_key, msg_ctx))

#define SANDESHA2_STORAGE_MGR_UPDATE_MSG_CTX(storage_man, env, storage_key,\
    msg_ctx) \
    ((storage_man)->ops->update_msg_ctx(storage_man, env, storage_key, msg_ctx))

#define SANDESHA2_STORAGE_MGR_RETRIEVE_MSG_CTX(storage_man, env, \
    storage_key, conf_ctx) \
    ((storage_man)->ops->retrieve_msg_ctx(storage_man, env, storage_key, \
    conf_ctx))

#define SANDESHA2_STORAGE_MGR_REMOVE_MSG_CTX(storage_man, env, storage_key)\
    ((storage_man)->ops->remove_msg_ctx(storage_man, env, storage_key))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_STORAGE_MGR_H*/
