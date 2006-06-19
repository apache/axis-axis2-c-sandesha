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
 
#ifndef SANDESHA2_SEQ_ACK_H
#define SANDESHA2_SEQ_ACK_H

/**
  * @file sandesha2_seq_ack.h
  * @brief 
  */

#include <axis2_defines.h>
#include <axis2_env.h>
#include <axiom_soap_envelope.h>
#include <sandesha2/sandesha2_iom_rm_part.h>
#include <sandesha2/sandesha2_error.h>
#include <sandesha2/sandesha2_identifier.h>
#include <sandesha2/sandesha2_nack.h>
#include <sandesha2/sandesha2_ack_range.h>
#include <sandesha2/sandesha2_ack_final.h>
#include <sandesha2/sandesha2_ack_none.h>
#include <axis2_array_list.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_seq_ack
 * @ingroup sandesha2_wsrm
 * @{
 */
    
typedef struct sandesha2_seq_ack_ops sandesha2_seq_ack_ops_t;
typedef struct sandesha2_seq_ack sandesha2_seq_ack_t;
 
/**
 * @brief SequenceAck ops struct
 * Encapsulator struct for ops of sandesha2_seq_ack
 */
AXIS2_DECLARE_DATA struct sandesha2_seq_ack_ops
{
    sandesha2_identifier_t * (AXIS2_CALL *
        get_identifier)
            (sandesha2_seq_ack_t *seq_ack,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_identifier)
            (sandesha2_seq_ack_t *seq_ack,
            const axis2_env_t *env, 
            sandesha2_identifier_t *identifier);
            
    axis2_array_list_t * (AXIS2_CALL *
        get_nack_list)
            (sandesha2_seq_ack_t *seq_ack,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_ack_final)
            (sandesha2_seq_ack_t *seq_ack,
            const axis2_env_t *env, 
            sandesha2_ack_final_t *ack_final);
                    	
    axis2_array_list_t * (AXIS2_CALL *
        get_ack_range_list)
            (sandesha2_seq_ack_t *seq_ack,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        add_ack_range)
            (sandesha2_seq_ack_t *seq_ack,
            const axis2_env_t *env, 
            sandesha2_ack_range_t *ack_range);
            
    axis2_bool_t (AXIS2_CALL *
        is_must_understand)
            (sandesha2_seq_ack_t *element,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_must_understand)
            (sandesha2_seq_ack_t *element,
            const axis2_env_t *env, 
            axis2_bool_t mu);
};

/**
 * @brief sandesha2_seq_ack
 *    sandesha2_seq_ack
 */
AXIS2_DECLARE_DATA struct sandesha2_seq_ack
{
    sandesha2_iom_rm_part_t part;
    sandesha2_seq_ack_ops_t *ops;
};

AXIS2_EXTERN sandesha2_seq_ack_t* AXIS2_CALL
sandesha2_seq_ack_create(
						const axis2_env_t *env,
					    axis2_char_t *ns_value);

/************************** Start of function macros **************************/
#define SANDESHA2_SEQ_ACK_SET_IDENTIFIER(seq_ack, env, identifier) \
    ((seq_ack)->ops->set_identifier (seq_ack, env, identifier))
#define SANDESHA2_SEQ_ACK_GET_IDENTIFIER(seq_ack, env) \
    ((seq_ack)->ops->get_identifier (seq_ack, env))
#define SANDESHA2_SEQ_ACK_GET_NACK_LIST(seq_ack, env) \
    ((seq_ack)->ops->get_nack_list (seq_ack, env))
#define SANDESHA2_SEQ_ACK_SET_ACK_FINAL(seq_ack, env, ack_final) \
    ((seq_ack)->ops->set_ack_final (seq_ack, env, ack_final))
#define SANDESHA2_SEQ_ACK_GET_ACK_RANGE_LIST(seq_ack, env) \
    ((seq_ack)->ops->get_ack_range_list (seq_ack, env))
#define SANDESHA2_SEQ_ACK_ADD_ACK_RANGE(seq_ack, env, ack_range) \
    ((seq_ack)->ops->add_ack_range (seq_ack, env, ack_range))
#define SANDESHA2_SEQ_ACK_IS_MUST_UNDERSTAND(seq_ack, env) \
    ((seq_ack)->ops->is_must_understand (seq_ack, env))
#define SANDESHA2_SEQ_ACK_SET_MUST_UNDERSTAND(seq_ack, env, mu) \
    ((seq_ack)->ops->set_must_understand (seq_ack, env, mu))
/************************** End of function macros ****************************/
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_SEQ_ACK_H */

