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
 
#ifndef SANDESHA2_ACK_RANGE_H
#define SANDESHA2_ACK_RANGE_H

/**
  * @file sandesha2_ack_range.h
  * @brief 
  */

#include <sandesha2_iom_rm_element.h>
#include <sandesha2_error.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_ack_range
 * @ingroup sandesha2_wsrm
 * @{
 */
typedef struct sandesha2_ack_range_ops sandesha2_ack_range_ops_t;
typedef struct sandesha2_ack_range sandesha2_ack_range_t;
 
/**
 * @brief Acknowledgement Range ops struct
 * Encapsulator struct for ops of sandesha2_ack_range
 */
AXIS2_DECLARE_DATA struct sandesha2_ack_range_ops
{
    long (AXIS2_CALL *
        get_lower_value)
            (sandesha2_ack_range_t *ack_range,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_lower_value)
            (sandesha2_ack_range_t *ack_range,
            const axis2_env_t *env, 
            long value);

    long (AXIS2_CALL *
        get_upper_value)
            (sandesha2_ack_range_t *ack_range,
            const axis2_env_t *env);
                    	
    axis2_status_t (AXIS2_CALL *
        set_upper_value)
            (sandesha2_ack_range_t *ack_range,
            const axis2_env_t *env, 
            long value);
};

/**
 * @brief sandesha2_ack_range
 *    sandesha2_ack_range
 */
AXIS2_DECLARE_DATA struct sandesha2_ack_range
{
    sandesha2_iom_rm_element_t element;
    sandesha2_ack_range_ops_t *ops;
};

AXIS2_EXTERN sandesha2_ack_range_t* AXIS2_CALL 
sandesha2_ack_range_create(
    const axis2_env_t *env, 
    axis2_char_t *ns_value,
    axis2_char_t *prefix);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_ACK_RANGE_SET_LOWER_VALUE(ack_range, env, value) \
    ((ack_range)->ops->set_lower_value (ack_range, env, value))
#define SANDESHA2_ACK_RANGE_GET_LOWER_VALUE(ack_range, env) \
    ((ack_range)->ops->get_lower_value (ack_range, env))
#define SANDESHA2_ACK_RANGE_SET_UPPER_VALUE(ack_range, env, value) \
    ((ack_range)->ops->set_upper_value(ack_range, env, value))
#define SANDESHA2_ACK_RANGE_GET_UPPER_VALUE(ack_range, env) \
    ((ack_range)->ops->get_upper_value (ack_range, env))
/************************** End of function macros ****************************/    
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_ACK_RANGE_H */

