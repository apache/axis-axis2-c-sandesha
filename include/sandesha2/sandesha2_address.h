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
 
#ifndef SANDESHA2_ADDRESS_H
#define SANDESHA2_ADDRESS_H

/**
  * @file sandesha2_address.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axiom_soap_envelope.h>
#include <sandesha2/sandesha2_iom_rm_element.h>
#include <axis2_endpoint_ref.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_address
 * @ingroup sandesha2_wsrm
 * @{
 */
typedef struct sandesha2_address_ops sandesha2_address_ops_t;
typedef struct sandesha2_address sandesha2_address_t;

/**
 * @brief Address ops struct
 * Encapsulator struct for ops of sandesha2_address
 */
AXIS2_DECLARE_DATA struct sandesha2_address_ops
{
    axis2_endpoint_ref_t * (AXIS2_CALL *
        get_epr)
            (sandesha2_address_t *address,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_epr)
            (sandesha2_address_t *address,
            const axis2_env_t *env, 
            axis2_endpoint_ref_t *epr);
};

/**
 * @brief sandesha2_address
 *    sandesha2_address
 */
AXIS2_DECLARE_DATA struct sandesha2_address
{
    sandesha2_iom_rm_element_t element;
    sandesha2_address_ops_t *ops;
};
    
AXIS2_EXTERN sandesha2_address_t* AXIS2_CALL
sandesha2_address_create(
						const axis2_env_t *env, 
                        axis2_char_t *ns_value,
                        axis2_endpoint_ref_t *epr);

/************************** Start of function macros **************************/
#define SANDESHA2_ADDRESS_GET_EPR(address, env)\
    ((address)->ops->get_epr(address, env))
#define SANDESHA2_ADDRESS_SET_EPR(address, env, epr)\
    ((address)->ops->set_epr(address, env, epr))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_ADDRESS_H */

