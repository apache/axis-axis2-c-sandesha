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
 
#ifndef SANDESHA2_ACCEPT_H
#define SANDESHA2_ACCEPT_H

/**
  * @file sandesha2_accept.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <sandesha2/sandesha2_iom_rm_element.h>
#include <sandesha2/sandesha2_acks_to.h>
#include <sandesha2/sandesha2_error.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_accept
 * @ingroup sandesha2_wsrm
 * @{
 */
typedef struct sandesha2_accept_ops sandesha2_accept_ops_t;
typedef struct sandesha2_accept sandesha2_accept_t;
 
/**
 * @brief Accept ops struct
 * Encapsulator struct for ops of sandesha2_accept
 */
AXIS2_DECLARE_DATA struct sandesha2_accept_ops
{
    axis2_status_t (AXIS2_CALL *
        set_acks_to) 
		    (sandesha2_accept_t *accept, const axis2_env_t *env, 
            sandesha2_acks_to_t *acks_to);
            
    sandesha2_acks_to_t* (AXIS2_CALL *
        get_acks_to) 
		    (sandesha2_accept_t *accept, const axis2_env_t *env);
};

/**
 * @brief sandesha2_accept
 *    sandesha2_accept
 */
AXIS2_DECLARE_DATA struct sandesha2_accept
{
    sandesha2_iom_rm_element_t element;
    sandesha2_accept_ops_t *ops;
};

AXIS2_EXTERN sandesha2_accept_t* AXIS2_CALL
sandesha2_accept_create(
						const axis2_env_t *env, 
					    axis2_char_t *rm_ns_value, 
					    axis2_char_t *addr_ns_value);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_ACCEPT_SET_ACKS_TO(accept, env, acks_to) \
    ((accept)->ops->set_acks_to (accept, env, acks_to))
#define SANDESHA2_ACCEPT_GET_ACKS_TO(accept, env) \
    ((accept)->ops->get_acks_to (accept, env))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_ACCEPT_H */

