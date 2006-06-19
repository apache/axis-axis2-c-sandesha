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
 
#ifndef SANDESHA2_ACKS_TO_H
#define SANDESHA2_ACKS_TO_H

/**
  * @file sandesha2_acks_to.h
  * @brief 
  */

#include <axis2_defines.h>
#include <axis2_env.h>
#include <axiom_soap_envelope.h>
#include <sandesha2/sandesha2_iom_rm_element.h>
#include <sandesha2/sandesha2_address.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_acks_to
 * @ingroup sandesha2_wsrm
 * @{
 */
typedef struct sandesha2_acks_to_ops sandesha2_acks_to_ops_t;
typedef struct sandesha2_acks_to sandesha2_acks_to_t;

/**
 * @brief Acks To ops struct
 * Encapsulator struct for ops of sandesha2_acks_to
 */
AXIS2_DECLARE_DATA struct sandesha2_acks_to_ops
{
    sandesha2_address_t * (AXIS2_CALL *
        get_address)
            (sandesha2_acks_to_t *acks_to,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_address)
            (sandesha2_acks_to_t *acks_to,
            const axis2_env_t *env, 
            sandesha2_address_t *address);
};

/**
 * @brief sandesha2_acks_to
 *    sandesha2_acks_to
 */
AXIS2_DECLARE_DATA struct sandesha2_acks_to
{
    sandesha2_iom_rm_element_t element;
    sandesha2_acks_to_ops_t *ops;
};

AXIS2_EXTERN sandesha2_acks_to_t * AXIS2_CALL
sandesha2_acks_to_create(
						const axis2_env_t *env, 
                        sandesha2_address_t *address,
					    axis2_char_t *rm_ns_value, 
					    axis2_char_t *addr_ns_value);

/************************** Start of function macros **************************/
#define SANDESHA2_ACKS_TO_GET_ADDRESS(acks_to, env)\
    ((acks_to)->ops->get_address(acks_to, env))
#define SANDESHA2_ACKS_TO_SET_ADDRESS(acks_to, env, str_id)\
    ((acks_to)->ops->set_address(acks_to, env, address))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_ACKS_TO_H */

