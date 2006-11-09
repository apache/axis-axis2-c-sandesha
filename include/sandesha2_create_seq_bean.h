/*
 * copyright 1999-2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef SANDESHA2_CREATE_SEQ_BEAN_H
#define SANDESHA2_CREATE_SEQ_BEAN_H

#include <axis2_qname.h>
#include <axis2_env.h>
#include <axis2_utils.h>
#include <axis2_utils_defines.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_create_seq_bean_t sandesha2_create_seq_bean_t;

/* constructors 
 */
AXIS2_EXTERN sandesha2_create_seq_bean_t* AXIS2_CALL
sandesha2_create_seq_bean_create(
    const axis2_env_t *env);

AXIS2_EXTERN sandesha2_create_seq_bean_t* AXIS2_CALL
sandesha2_create_seq_bean_create_with_data(
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id,
    axis2_char_t *create_seq_msg_id, 
    axis2_char_t *seq_id);

axis2_status_t AXIS2_CALL 
sandesha2_create_seq_bean_free  (
    sandesha2_create_seq_bean_t *create_seq,
    const axis2_env_t *env);

axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_bean_get_create_seq_msg_id  (
    sandesha2_create_seq_bean_t *create_seq,
    const axis2_env_t *env);

void AXIS2_CALL 
sandesha2_create_seq_bean_set_create_seq_msg_id (
    sandesha2_create_seq_bean_t *create_seq,
    const axis2_env_t *env, axis2_char_t* seq_msg_id) ;

axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_bean_get_seq_id (
    sandesha2_create_seq_bean_t *create_seq,
    const axis2_env_t *env);
	
void AXIS2_CALL
sandesha2_create_seq_bean_set_seq_id (
    sandesha2_create_seq_bean_t *create_seq,
    const axis2_env_t *env, axis2_char_t* seq_id) ;

axis2_char_t* AXIS2_CALL
sandesha2_create_seq_bean_get_internal_seq_id (
    sandesha2_create_seq_bean_t *create_seq,
    const axis2_env_t *env);

void AXIS2_CALL
sandesha2_create_seq_bean_set_internal_seq_id (
    sandesha2_create_seq_bean_t *create_seq,
    const axis2_env_t *env, axis2_char_t* int_seq_id);



#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_CREATE_SEQ_BEAN_H */
