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
 
#ifndef SANDESHA2_FAULT_MGR_H
#define SANDESHA2_FAULT_MGR_H

/**
  * @file sandesha2_fault_mgr.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_conf_ctx.h>
#include <sandesha2/sandesha2_msg_ctx.h>
#include <sandesha2/sandesha2_storage_mgr.h>
#include <sandesha2/sandesha2_fault_data.h>
#include <axis2_msg_ctx.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_util
 * @{
 */
 
 typedef struct sandesha2_fault_mgr_ops sandesha2_fault_mgr_ops_t;
 typedef struct sandesha2_fault_mgr sandesha2_fault_mgr_t;
 /**
 * @brief Sandesha2 Fault Manager ops struct
 * Encapsulator struct for ops of sandesha2_fault_mgr
 */
AXIS2_DECLARE_DATA struct sandesha2_fault_mgr_ops
{
    
    sandesha2_msg_ctx_t* (AXIS2_CALL *
        check_for_create_seq_refused) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            axis2_msg_ctx_t *create_seq_msg,
            sandesha2_storage_mgr_t *storage_man);
            
    sandesha2_msg_ctx_t* (AXIS2_CALL *
        check_for_last_msg_num_exceeded) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *app_rm_msg,
            sandesha2_storage_mgr_t *storage_man);
            
    sandesha2_msg_ctx_t* (AXIS2_CALL *
        check_for_msg_num_rollover) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *create_seq_msg,
            sandesha2_storage_mgr_t *storage_man);

    sandesha2_msg_ctx_t* (AXIS2_CALL *
        check_for_unknown_seq) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *create_seq_msg,
            axis2_char_t *seq_id,
            sandesha2_storage_mgr_t *storage_man);

    sandesha2_msg_ctx_t* (AXIS2_CALL *
        check_for_invalid_ack) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *ack_rm_msg,
            sandesha2_storage_mgr_t *storage_man);

    sandesha2_msg_ctx_t* (AXIS2_CALL *
        check_for_seq_closed) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *create_seq_msg,
            axis2_char_t *seq_id,
            sandesha2_storage_mgr_t *storage_man);

    sandesha2_msg_ctx_t* (AXIS2_CALL *
        get_fault) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env,
            sandesha2_msg_ctx_t *create_seq_msg,
            sandesha2_fault_data_t *fault_data,
            axis2_char_t *addr_ns_uri,
            sandesha2_storage_mgr_t *storage_man);

    axis2_status_t (AXIS2_CALL *
        free) 
            (sandesha2_fault_mgr_t *fault_mgr,
            const axis2_env_t *env);
};

/**
 * @brief sandesha2_fault_mgr_ops
 *    sandesha2_fault_mgr_ops
 */
AXIS2_DECLARE_DATA struct sandesha2_fault_mgr
{
    sandesha2_fault_mgr_ops_t *ops;
};

AXIS2_EXTERN sandesha2_fault_mgr_t* AXIS2_CALL
sandesha2_fault_mgr_create(
						const axis2_env_t *env);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_FAULT_MGR_FREE(fault_mgr, env) \
    ((fault_mgr)->ops->free (fault_mgr, env))
    
#define SANDESHA2_FAULT_MGR_CHECK_FOR_CREATE_SEQ_REFUSED(fault_mgr, env, \
    msg, storage_man) \
    ((fault_mgr)->ops->check_for_create_seq_refused(fault_mgr, env, msg, \
    storage_man))
    
#define SANDESHA2_FAULT_MGR_CHECK_FOR_LAST_MSG_NUM_EXCEEDED(fault_mgr, \
    env, msg, storage_man) \
    ((fault_mgr)->ops->check_for_last_msg_num_exceeded(fault_mgr, env, msg, \
    storage_man))
    
#define SANDESHA2_FAULT_MGR_CHECK_FOR_MSG_NUM_ROLLOVER(fault_mgr, \
    env, msg, storage_man) \
    ((fault_mgr)->ops->check_for_msg_num_rollover(fault_mgr, env, msg, \
    storage_man))
    
#define SANDESHA2_FAULT_MGR_CHECK_FOR_UNKNOWN_SEQ(fault_mgr, env, msg, \
    seq_id, storage_man) \
    ((fault_mgr)->ops->check_for_unknown_seq(fault_mgr, env, msg, seq_id, \
    storage_man))
    
#define SANDESHA2_FAULT_MGR_CHECK_FOR_INVALID_ACK(fault_mgr, env, msg, \
    storage_man) \
    ((fault_mgr)->ops->check_for_invalid_ack(fault_mgr, env, msg, storage_man))
    
#define SANDESHA2_FAULT_MGR_CHECK_FOR_SEQ_CLOSED(fault_mgr, env, msg, \
    seq_id, storage_man) \
    ((fault_mgr)->ops->check_for_seq_closed(fault_mgr, env, msg, seq_id, \
    storage_man))

#define SANDESHA2_FAULT_MGR_GET_FAULT(fault_mgr, env, msg, fault_data,\
    addr_ns_uri, storage_man) \
    ((fault_mgr)->ops->get_fault(fault_mgr, env, msg, fault_data, addr_ns_uri, \
    storage_man))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_FAULT_MGR_H*/
