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
 
#ifndef SANDESHA2_SEQ_H
#define SANDESHA2_SEQ_H

/**
  * @file sandesha2_seq.h
  * @brief 
  */

#include <axiom_soap_envelope.h>
#include <sandesha2_iom_rm_part.h>
#include <sandesha2_identifier.h>
#include <sandesha2_msg_number.h>
#include <sandesha2_last_msg.h>
#include <sandesha2_error.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_seq
 * @ingroup sandesha2_wsrm
 * @{
 */
    
typedef struct sandesha2_seq_ops sandesha2_seq_ops_t;
typedef struct sandesha2_seq sandesha2_seq_t;
 
/**
 * @brief Sequence ops struct
 * Encapsulator struct for ops of sandesha2_seq
 */
AXIS2_DECLARE_DATA struct sandesha2_seq_ops
{
    sandesha2_identifier_t * (AXIS2_CALL *
        get_identifier)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_identifier)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env, 
            sandesha2_identifier_t *identifier);
            
    sandesha2_msg_number_t * (AXIS2_CALL *
        get_msg_num)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_msg_num)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env, 
            sandesha2_msg_number_t *msg_num);
                    	
    sandesha2_last_msg_t * (AXIS2_CALL *
        get_last_msg)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_last_msg)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env, 
            sandesha2_last_msg_t *last_msg);

    axis2_bool_t (AXIS2_CALL *
        is_must_understand)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_must_understand)
            (sandesha2_seq_t *seq,
            const axis2_env_t *env, 
            axis2_bool_t mu);
};

/**
 * @brief sandesha2_seq
 *    sandesha2_seq
 */
AXIS2_DECLARE_DATA struct sandesha2_seq
{
    sandesha2_iom_rm_part_t part;
    sandesha2_seq_ops_t *ops;
};

AXIS2_EXTERN sandesha2_seq_t* AXIS2_CALL
sandesha2_seq_create(
						const axis2_env_t *env, 
					    axis2_char_t *ns_value);
/************************** Start of function macros **************************/
#define SANDESHA2_SEQ_SET_IDENTIFIER(seq, env, identifier) \
    ((seq)->ops->set_identifier (seq, env, identifier))
#define SANDESHA2_SEQ_GET_IDENTIFIER(seq, env) \
    ((seq)->ops->get_identifier (seq, env))
#define SANDESHA2_SEQ_GET_MSG_NUM(seq, env) \
    ((seq)->ops->get_msg_num (seq, env))
#define SANDESHA2_SEQ_SET_MSG_NUM(seq, env, msg_num) \
    ((seq)->ops->set_msg_num (seq, env, msg_num))
#define SANDESHA2_SEQ_SET_LAST_MSG(seq, env, last_msg) \
    ((seq)->ops->set_last_msg (seq, env, last_msg))
#define SANDESHA2_SEQ_GET_LAST_MSG(seq, env) \
    ((seq)->ops->get_last_msg (seq, env))
#define SANDESHA2_SEQ_SET_MUST_UNDERSTAND(seq, env, mu) \
    ((seq)->ops->set_must_understand (seq, env, mu))
#define SANDESHA2_SEQ_IS_MUST_UNDERSTAND(seq, env) \
    ((seq)->ops->is_must_understand (seq, env))
/************************** End of function macros ****************************/
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_SEQ_H */

