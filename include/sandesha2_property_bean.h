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
 
typedef struct sandesha2_property_bean_t sandesha2_property_bean_t;

AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_bean_create(
	const axis2_env_t *env);
 
long AXIS2_CALL 
sandesha2_property_bean_get_inactive_timeout_interval(
    sandesha2_property_bean_t *bean, 
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    long interval);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval_with_units(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    long interval,
    axis2_char_t *units);
            
long AXIS2_CALL
sandesha2_property_bean_get_ack_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_ack_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    long interval);
            
long AXIS2_CALL
sandesha2_property_bean_get_retrans_interval( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_retrans_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    long interval);
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_exp_backoff(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_exp_backoff( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    axis2_bool_t exp_backoff);
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_in_mem_storage_mgr( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_mem_storage_mgr(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    axis2_char_t *manager);
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_permanent_storage_mgr( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_permanent_storage_mgr(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    axis2_char_t *manager);
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_in_order(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_order( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    axis2_bool_t in_order);
            
axis2_array_list_t* AXIS2_CALL
sandesha2_property_bean_get_msg_types_to_drop( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_msg_types_to_drop( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    axis2_array_list_t *msg_types);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_add_msg_type_to_drop( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    int msg_type);
            
int AXIS2_CALL
sandesha2_property_bean_get_max_retrans_count( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_max_retrans_count( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, 
    int count);
            
axis2_status_t AXIS2_CALL 
sandesha2_property_bean_free(
    sandesha2_property_bean_t *bean, 
    const axis2_env_t *env);								

                       
/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_PROPERTY_BEAN_H*/
