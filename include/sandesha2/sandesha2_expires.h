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
 
#ifndef SANDESHA2_EXPIRES_H
#define SANDESHA2_EXPIRES_H

/**
  * @file sandesha2_expires.h
  * @brief 
  */

#include <axis2_defines.h>
#include <axis2_env.h>
#include <sandesha2/sandesha2_iom_rm_element.h>
#include <sandesha2/sandesha2_error.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_expires
 * @ingroup sandesha2_wsrm
 * @{
 */
typedef struct sandesha2_expires_ops sandesha2_expires_ops_t;
typedef struct sandesha2_expires sandesha2_expires_t;
 
/**
 * @brief Expires ops struct
 * Encapsulator struct for ops of sandesha2_expires
 */
AXIS2_DECLARE_DATA struct sandesha2_expires_ops
{
    axis2_char_t * (AXIS2_CALL *
        get_duration)
            (sandesha2_expires_t *expires,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_duration)
            (sandesha2_expires_t *expires,
            const axis2_env_t *env, 
            axis2_char_t *duration);
};

/**
 * @brief sandesha2_expires
 *    sandesha2_expires
 */
AXIS2_DECLARE_DATA struct sandesha2_expires
{
    sandesha2_iom_rm_element_t element;
    sandesha2_expires_ops_t *ops;
};
    
AXIS2_EXTERN sandesha2_expires_t* AXIS2_CALL
sandesha2_expires_create(
						const axis2_env_t *env, 
					    axis2_char_t *ns_value);

/************************** Start of function macros **************************/
#define SANDESHA2_EXPIRES_SET_ACKS_TO(expires, env, duration) \
    ((expires)->ops->set_duration (expires, env, duration))
#define SANDESHA2_EXPIRES_GET_ACKS_TO(expires, env) \
    ((expires)->ops->get_duration (expires, env))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_EXPIRES_H */
