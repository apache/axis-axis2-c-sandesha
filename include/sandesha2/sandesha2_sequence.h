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
 
#ifndef SANDESHA2_SEQUENCE_H
#define SANDESHA2_SEQUENCE_H

/**
  * @file sandesha2_sequence.h
  * @brief 
  */

#include <axiom_soap_envelope.h>
#include <sandesha2/sandesha2_iom_rm_part.h>
#include <sandesha2/sandesha2_identifier.h>
#include <sandesha2/sandesha2_msg_number.h>
#include <sandesha2/sandesha2_last_msg.h>
#include <sandesha2/sandesha2_error.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_sequence
 * @ingroup sandesha2_wsrm
 * @{
 */
    
typedef struct sandesha2_sequence_ops sandesha2_sequence_ops_t;
typedef struct sandesha2_sequence sandesha2_sequence_t;
 
/**
 * @brief Sequence ops struct
 * Encapsulator struct for ops of sandesha2_sequence
 */
AXIS2_DECLARE_DATA struct sandesha2_sequence_ops
{
    sandesha2_identifier_t * (AXIS2_CALL *
        get_identifier)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_identifier)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env, 
            sandesha2_identifier_t *identifier);
            
    sandesha2_msg_number_t * (AXIS2_CALL *
        get_msg_num)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_msg_num)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env, 
            sandesha2_msg_number_t *msg_num);
                    	
    sandesha2_last_msg_t * (AXIS2_CALL *
        get_last_msg)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_last_msg)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env, 
            sandesha2_last_msg_t *last_msg);

    axis2_bool_t (AXIS2_CALL *
        is_must_understand)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_must_understand)
            (sandesha2_sequence_t *sequence,
            const axis2_env_t *env, 
            axis2_bool_t mu);
};

/**
 * @brief sandesha2_sequence
 *    sandesha2_sequence
 */
AXIS2_DECLARE_DATA struct sandesha2_sequence
{
    sandesha2_iom_rm_part_t part;
    sandesha2_sequence_ops_t *ops;
};

AXIS2_EXTERN sandesha2_sequence_t* AXIS2_CALL
sandesha2_sequence_create(
						const axis2_env_t *env, 
					    axis2_char_t *ns_value);
/************************** Start of function macros **************************/
#define SANDESHA2_SEQUENCE_SET_IDENTIFIER(sequence, env, identifier) \
    ((sequence)->ops->set_identifier (sequence, env, identifier))
#define SANDESHA2_SEQUENCE_GET_IDENTIFIER(sequence, env) \
    ((sequence)->ops->get_identifier (sequence, env))
#define SANDESHA2_SEQUENCE_GET_MSG_NUM(sequence, env) \
    ((sequence)->ops->get_msg_num (sequence, env))
#define SANDESHA2_SEQUENCE_SET_MSG_NUM(sequence, env, msg_num) \
    ((sequence)->ops->set_msg_num (sequence, env, msg_num))
#define SANDESHA2_SEQUENCE_SET_LAST_MSG(sequence, env, last_msg) \
    ((sequence)->ops->set_last_msg (sequence, env, last_msg))
#define SANDESHA2_SEQUENCE_GET_LAST_MSG(sequence, env) \
    ((sequence)->ops->get_last_msg (sequence, env))
/************************** End of function macros ****************************/
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_SEQUENCE_H */

