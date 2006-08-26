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
 
#ifndef SANDESHA2_SEQ_FAULT_H
#define SANDESHA2_SEQ_FAULT_H

/**
  * @file sandesha2_seq_fault.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <sandesha2_iom_rm_element.h>
#include <sandesha2_error.h>
#include <sandesha2_fault_code.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_seq_fault
 * @ingroup sandesha2_wsrm
 * @{
 */
    
typedef struct sandesha2_seq_fault_ops sandesha2_seq_fault_ops_t;
typedef struct sandesha2_seq_fault sandesha2_seq_fault_t;
 
/**
 * @brief Sequence Fault ops struct
 * Encapsulator struct for ops of sandesha2_seq_fault
 */
AXIS2_DECLARE_DATA struct sandesha2_seq_fault_ops
{
    sandesha2_fault_code_t* (AXIS2_CALL *
        get_fault_code)
            (sandesha2_seq_fault_t *seq_fault,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_fault_code)
            (sandesha2_seq_fault_t *seq_fault,
            const axis2_env_t *env, 
            sandesha2_fault_code_t *fault_code);
};

/**
 * @brief sandesha2_seq_fault
 *    sandesha2_seq_fault
 */
AXIS2_DECLARE_DATA struct sandesha2_seq_fault
{
    sandesha2_iom_rm_element_t element;
    sandesha2_seq_fault_ops_t *ops;
};
    
AXIS2_EXTERN sandesha2_seq_fault_t* AXIS2_CALL
sandesha2_seq_fault_create(
						const axis2_env_t *env, 
					    axis2_char_t *ns_value);

/************************** Start of function macros **************************/
#define SANDESHA2_SEQ_FAULT_SET_FAULT_CODE(seq_fault, env, fault_code) \
    ((seq_fault)->ops->set_fault_code (seq_fault, env, fault_code))
#define SANDESHA2_SEQ_FAULT_GET_FAULT_CODE(seq_fault, env) \
    ((seq_fault)->ops->get_fault_code (seq_fault, env))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_SEQ_FAULT_H */

