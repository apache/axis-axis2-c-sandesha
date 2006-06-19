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
 
#ifndef SANDESHA2_IOM_RM_PART_H
#define SANDESHA2_IOM_RM_PART_H

/**
  * @file sandesha2_iom_rm_part.h
  * @brief 
  */

#include <axis2_defines.h>
#include <axis2_env.h>
#include <axiom_soap_envelope.h>
#include <sandesha2/sandesha2_iom_rm_element.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_wsrm
 * @{
 */
 
 typedef struct sandesha2_iom_rm_part_ops sandesha2_iom_rm_part_ops_t;
 typedef struct sandesha2_iom_rm_part sandesha2_iom_rm_part_t;
 /**
 * @brief IOM RM Part ops struct
 * Encapsulator struct for ops of sandesha2_iom_rm_part
 */
AXIS2_DECLARE_DATA struct sandesha2_iom_rm_part_ops
{
	axis2_status_t (AXIS2_CALL *
        to_soap_env) 
    	    (sandesha2_iom_rm_part_t *part,
             const axis2_env_t *env, 
             axiom_soap_envelope_t *envelope);
};

/**
 * @brief sandesha2_iom_rm_part
 *    sandesha2_iom_rm_part
 */
AXIS2_DECLARE_DATA struct sandesha2_iom_rm_part
{
    sandesha2_iom_rm_element_t element;
    sandesha2_iom_rm_part_ops_t *ops;
};

/************************** Start of function macros **************************/
#define SANDESHA2_IOM_RM_PART_TO_SOAP_ENVELOPE(part, env, envelope) \
    ((part)->ops-> (part, env, envelope))
#define SANDESHA2_IOM_RM_PART_FREE(part, env) \
    ((part)->element.ops->free (part, env))
/************************** End of function macros ****************************/
    
 /** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_IOM_RM_PART_H*/
