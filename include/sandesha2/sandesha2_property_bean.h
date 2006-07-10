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
 
#ifndef SANDESHA2_PROPERTY_BEAN_H
#define SANDESHA2_PROPERTY_BEAN_H

/**
  * @file sandesha2_property_bean.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_array_list.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_property_bean
 * @{
 */
 
 typedef struct sandesha2_property_bean_ops sandesha2_property_bean_ops_t;
 typedef struct sandesha2_property_bean sandesha2_property_bean_t;
 /**
 * @brief Sandesha2 Property Bean ops struct
 * Encapsulator struct for ops of sandesha2_property_bean
 */
AXIS2_DECLARE_DATA struct sandesha2_property_bean_ops
{
    
    long (AXIS2_CALL *
        get_inactive_timeout_interval) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_inactive_timeout_interval) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            long interval);
            
    axis2_status_t (AXIS2_CALL *
        set_inactive_timeout_interval_with_units) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            long interval,
            axis2_char_t *units);
            
    long (AXIS2_CALL *
        get_ack_interval) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_ack_interval) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            long interval);
            
    long (AXIS2_CALL *
        get_retrans_interval) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_retrans_interval) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            long interval);
            
    axis2_bool_t (AXIS2_CALL *
        is_exp_backoff) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_exp_backoff) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            axis2_bool_t exp_backoff);
            
    axis2_char_t* (AXIS2_CALL *
        get_in_mem_storage_manager) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_in_mem_storage_manager) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            axis2_char_t *manager);
            
    axis2_char_t* (AXIS2_CALL *
        get_permanent_storage_manager) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_permanent_storage_manager) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            axis2_char_t *manager);
            
    axis2_bool_t (AXIS2_CALL *
        is_in_order) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_in_order) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            axis2_bool_t in_order);
            
    axis2_array_list_t* (AXIS2_CALL *
        get_msg_types_to_drop) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_msg_types_to_drop) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            axis2_array_list_t *msg_types);
            
    axis2_status_t (AXIS2_CALL *
        add_msg_type_to_drop) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            int msg_type);
            
    int (AXIS2_CALL *
        get_max_retrans_count) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        set_max_retrans_count) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env,
            int count);
            
    axis2_status_t (AXIS2_CALL *
        free) 
            (sandesha2_property_bean_t *bean,
            const axis2_env_t *env);
};

/**
 * @brief sandesha2_property_bean_ops
 *    sandesha2_property_bean_ops
 */
AXIS2_DECLARE_DATA struct sandesha2_property_bean
{
    sandesha2_property_bean_ops_t *ops;
};

AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_bean_create(
						const axis2_env_t *env);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_PROPERTY_BEAN_FREE(bean, env) \
    ((bean)->ops->free (bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_GET_INACTIVE_TIMEOUT_INTERVAL(bean, env)\
    ((bean)->ops->get_inactive_timeout_interval(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_INACTIVE_TIMEOUT_INTERVAL(bean, env, interval)\
    ((bean)->ops->set_inactive_timeout_interval(bean, env, interval))
    
#define SANDESHA2_PROPERTY_BEAN_SET_INACTIVE_TIMEOUT_INTERVAL_WITH_UNITS(bean, \
    env, interval, units)\
    ((bean)->ops->set_inactive_timeout_interval_with_units(bean, \
    env, interval, units))    
    
#define SANDESHA2_PROPERTY_BEAN_GET_ACK_INTERVAL(bean, env)\
    ((bean)->ops->get_ack_interval(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_ACK_INTERVAL(bean, env, interval)\
    ((bean)->ops->set_ack_interval(bean, env, interval))
    
#define SANDESHA2_PROPERTY_BEAN_GET_RETRANS_INTERVAL(bean, env)\
    ((bean)->ops->get_retrans_interval(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_RETRANS_INTERVAL(bean, env, interval)\
    ((bean)->ops->set_retrans_interval(bean, env, interval))
    
#define SANDESHA2_PROPERTY_BEAN_IS_EXP_BACKOFF(bean, env)\
    ((bean)->ops->is_exp_backoff(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_EXP_BACKOFF(bean, env, backoff)\
    ((bean)->ops->set_exp_backoff(bean, env, backoff))
    
#define SANDESHA2_PROPERTY_BEAN_GET_IN_MEM_STORAGE_MANAGER(bean, env)\
    ((bean)->ops->get_in_mem_storage_manager(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_IN_MEM_STORAGE_MANAGER(bean, env, manager)\
    ((bean)->ops->set_in_mem_storage_manager(bean, env, manager))
    
#define SANDESHA2_PROPERTY_BEAN_GET_PERMENENT_STORAGE_MANAGER(bean, env)\
    ((bean)->ops->get_permanent_storage_manager(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_PERMENENT_STORAGE_MANAGER(bean, env, manager)\
    ((bean)->ops->get_permanent_storage_manager(bean, env, manager))
    
#define SANDESHA2_PROPERTY_BEAN_IS_IN_ORDER(bean, env)\
    ((bean)->ops->is_in_order(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_IN_ORDER(bean, env, in_order)\
    ((bean)->ops->set_in_order(bean, env, in_order))
    
#define SANDESHA2_PROPERTY_BEAN_GET_MSG_TYPES_TO_DROP(bean, env)\
    ((bean)->ops->get_msg_types_to_drop(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_MSG_TYPES_TO_DROP(bean, env, msg_types)\
    ((bean)->ops->set_msg_types_to_drop(bean, env, msg_types))
    
#define SANDESHA2_PROPERTY_BEAN_ADD_MSG_TYPE_TO_DROP(bean, env, msg_type)\
    ((bean)->ops->add_msg_type_to_drop(bean, env, msg_type))
    
#define SANDESHA2_PROPERTY_BEAN_GET_MAX_RETRANS_COUNT(bean, env)\
    ((bean)->ops->get_max_retrans_count(bean, env))
    
#define SANDESHA2_PROPERTY_BEAN_SET_MAX_RETRANS_COUNT(bean, env, count)\
    ((bean)->ops->set_max_retrans_count(bean, env, count))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_PROPERTY_BEAN_H*/
