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

#ifndef SANDESHA2_INVOKER_BEAN_H
#define SANDESHA2_INVOKER_BEAN_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_invoker_bean_t sandesha2_invoker_bean_t;
struct sandesha2_rm_bean_t;

/* constructors 
 */
AXIS2_EXTERN sandesha2_invoker_bean_t* AXIS2_CALL
	sandesha2_invoker_bean_create(
	const axis2_env_t *env);

AXIS2_EXTERN sandesha2_invoker_bean_t* AXIS2_CALL
	sandesha2_invoker_bean_create_with_data(const axis2_env_t *env,
    axis2_char_t * ref_key,
	long msg_no,
	axis2_char_t *seq_id,
	axis2_bool_t invoked);

axis2_status_t AXIS2_CALL 
sandesha2_invoker_bean_free  (
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env);

struct sandesha2_rm_bean_t * AXIS2_CALL
sandesha2_invoker_bean_get_base( 
    sandesha2_invoker_bean_t* invoker,
    const axis2_env_t *env);

void AXIS2_CALL
sandesha2_invoker_bean_set_base (
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env, 
    struct sandesha2_rm_bean_t* rm_bean);

axis2_char_t* AXIS2_CALL 
sandesha2_invoker_bean_get_msg_ctx_ref_key(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env);

void AXIS2_CALL 
sandesha2_invoker_bean_set_msg_context_ref_key(
    sandesha2_invoker_bean_t *invoker,
	const axis2_env_t *env, axis2_char_t* context_ref_id) ;

long AXIS2_CALL 
sandesha2_invoker_bean_get_msg_no(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env);
	
void AXIS2_CALL
sandesha2_invoker_bean_set_msg_no(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env, long msgno) ;

axis2_char_t* AXIS2_CALL
sandesha2_invoker_bean_get_seq_id(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env);

void AXIS2_CALL
sandesha2_invoker_bean_set_seq_id(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env, axis2_char_t* int_seq_id);

axis2_bool_t AXIS2_CALL
sandesha2_invoker_bean_is_invoked (
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env);

void AXIS2_CALL 
sandesha2_invoker_bean_set_invoked( 
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env,
    axis2_bool_t invoked);


#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_INVOKER_BEAN */
