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
 
#ifndef SANDESHA2_SEQ_OFFER_H
#define SANDESHA2_SEQ_OFFER_H

/**
  * @file sandesha2_seq_offer.h
  * @brief 
  */

#include <sandesha2_iom_rm_element.h>
#include <sandesha2_error.h>
#include <sandesha2_identifier.h>
#include <sandesha2_expires.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_seq_offer
 * @ingroup sandesha2_wsrm
 * @{
 */
    
typedef struct sandesha2_seq_offer_ops sandesha2_seq_offer_ops_t;
typedef struct sandesha2_seq_offer sandesha2_seq_offer_t;
 
/**
 * @brief SequenceOffer ops struct
 * Encapsulator struct for ops of sandesha2_seq_offer
 */
AXIS2_DECLARE_DATA struct sandesha2_seq_offer_ops
{
    sandesha2_identifier_t * (AXIS2_CALL *
        get_identifier)
            (sandesha2_seq_offer_t *seq_offer,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
        set_identifier)
            (sandesha2_seq_offer_t *seq_offer,
            const axis2_env_t *env, 
            sandesha2_identifier_t *identifier);
            
    sandesha2_expires_t * (AXIS2_CALL *
        get_expires)
            (sandesha2_seq_offer_t *element,
            const axis2_env_t *env);
                    	
    axis2_status_t (AXIS2_CALL *
        set_expires)
            (sandesha2_seq_offer_t *element,
            const axis2_env_t *env, 
            sandesha2_expires_t *expires);
};

/**
 * @brief sandesha2_seq_offer
 *    sandesha2_seq_offer
 */
AXIS2_DECLARE_DATA struct sandesha2_seq_offer
{
    sandesha2_iom_rm_element_t element;
    sandesha2_seq_offer_ops_t *ops;
};

AXIS2_EXTERN sandesha2_seq_offer_t* AXIS2_CALL
sandesha2_seq_offer_create(
						const axis2_env_t *env, 
					    axis2_char_t *ns_value);

/************************** Start of function macros **************************/
#define SANDESHA2_SEQ_OFFER_SET_IDENTIFIER(seq_offer, env, identifier) \
    ((seq_offer)->ops->set_identifier (seq_offer, env, identifier))
#define SANDESHA2_SEQ_OFFER_GET_IDENTIFIER(seq_offer, env) \
    ((seq_offer)->ops->get_identifier (seq_offer, env))
#define SANDESHA2_SEQ_OFFER_SET_EXPIRES(seq_offer, env, expires) \
    ((seq_offer)->ops->set_expires (seq_offer, env, expires))
#define SANDESHA2_SEQ_OFFER_GET_EXPIRES(seq_offer, env) \
    ((seq_offer)->ops->get_expires (seq_offer, env))
/************************** End of function macros ****************************/
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_SEQ_OFFER_H */

