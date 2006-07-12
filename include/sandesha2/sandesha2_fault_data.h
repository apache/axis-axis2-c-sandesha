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
 
#ifndef SANDESHA2_FAULT_DATA_H
#define SANDESHA2_FAULT_DATA_H

/**
  * @file sandesha2_fault_data.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_conf_ctx.h>
#include <axiom_node.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_util
 * @{
 */
 
 typedef struct sandesha2_fault_data_ops sandesha2_fault_data_ops_t;
 typedef struct sandesha2_fault_data sandesha2_fault_data_t;
 /**
 * @brief Sandesha2 Fault Data ops struct
 * Encapsulator struct for ops of sandesha2_fault_data
 */
AXIS2_DECLARE_DATA struct sandesha2_fault_data_ops
{
    
    axiom_node_t *(AXIS2_CALL *
        get_detail) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_detail) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env,
            axiom_node_t *detail);
            
    axis2_char_t *(AXIS2_CALL *
        get_reason) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_reason) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env,
            axis2_char_t *reason);

    axis2_char_t *(AXIS2_CALL *
        get_sub_code) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_sub_code) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env,
            axis2_char_t *sub_code);
            
    axis2_char_t *(AXIS2_CALL *
        get_code) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_code) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env,
            axis2_char_t *sub_code);
            
    int (AXIS2_CALL *
        get_type) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_type) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env,
            int type);
            
    axis2_char_t *(AXIS2_CALL *
        get_seq_id) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_seq_id) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env,
            axis2_char_t *seq_id);
    
    axis2_status_t (AXIS2_CALL *
        free) 
            (sandesha2_fault_data_t *data,
            const axis2_env_t *env);
};

/**
 * @brief sandesha2_fault_data_ops
 *    sandesha2_fault_data_ops
 */
AXIS2_DECLARE_DATA struct sandesha2_fault_data
{
    sandesha2_fault_data_ops_t *ops;
};

AXIS2_EXTERN sandesha2_fault_data_t* AXIS2_CALL
sandesha2_fault_data_create(
						const axis2_env_t *env);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_FAULT_DATA_FREE(data, env) \
    ((data)->ops->free (data, env))
    
#define SANDESHA2_FAULT_DATA_GET_DETAIL(data, env) \
    ((data)->ops->get_detail(data, env))
    
#define SANDESHA2_FAULT_DATA_SET_DETAIL(data, env, detail) \
    ((data)->ops->set_detail(data, env, detail))
    
#define SANDESHA2_FAULT_DATA_GET_REASON(data, env) \
    ((data)->ops->get_reason(data, env))
    
#define SANDESHA2_FAULT_DATA_SET_REASON(data, env, reason) \
    ((data)->ops->set_reason(data, env, reason))
    
#define SANDESHA2_FAULT_DATA_GET_SUB_CODE(data, env) \
    ((data)->ops->get_sub_code(data, env))
    
#define SANDESHA2_FAULT_DATA_SET_SUB_CODE(data, env, sub_code) \
    ((data)->ops->set_sub_code(data, env, sub_code))
    
#define SANDESHA2_FAULT_DATA_GET_CODE(data, env) \
    ((data)->ops->get_sub_code(data, env))
    
#define SANDESHA2_FAULT_DATA_SET_CODE(data, env, code) \
    ((data)->ops->set_code(data, env, code))
    
#define SANDESHA2_FAULT_DATA_GET_TYPE(data, env) \
    ((data)->ops->get_type(data, env))
    
#define SANDESHA2_FAULT_DATA_SET_TYPE(data, env, type) \
    ((data)->ops->set_type(data, env, type))

#define SANDESHA2_FAULT_DATA_GET_SEQ_ID(data, env) \
    ((data)->ops->get_seq_id(data, env))
    
#define SANDESHA2_FAULT_DATA_SET_SEQ_ID(data, env, seq_id) \
    ((data)->ops->set_seq_id(data, env, seq_id)
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_FAULT_DATA_H*/
