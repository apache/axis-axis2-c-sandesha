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

typedef struct sandesha2_invoker_bean_ops sandesha2_invoker_bean_ops_t;
typedef struct sandesha2_invoker_bean sandesha2_invoker_bean_t;

AXIS2_DECLARE_DATA struct sandesha2_invoker_bean_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env);

	axis2_char_t * (AXIS2_CALL *
		get_msg_context_ref_key) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_msg_context_ref_key) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env,
			axis2_char_t* context_ref_id);

	long (AXIS2_CALL *
		get_msg_no) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_msg_no) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env,
			long msg_no);

	axis2_char_t* (AXIS2_CALL *
		get_seq_id) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_seq_id) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env,
			axis2_char_t *seq_id);
	axis2_bool_t (AXIS2_CALL *
		is_invoked) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_invoked) (sandesha2_invoker_bean_t *invoker,
			const axis2_env_t *env,
			axis2_bool_t invoked);

}; 


AXIS2_DECLARE_DATA struct sandesha2_invoker_bean{
	/* ops related to invoker_bean */
	sandesha2_invoker_bean_ops_t *ops;

}; 

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

#define SANDESHA2_INVOKER_BEAN_FREE(invoker_bean, env) \
       ((invoker_bean)->ops->free(invoker_bean, env))

#define SANDESHA2_INVOKER_BEAN_GET_MSG_CONTEXT_REF_ID(invoker_bean, env) \
	((invoker_bean)->ops->get_msg_context_ref_key(invoker_bean, env))

#define SANDESHA2_INVOKER_BEAN_SET_MSG_CONTEXT_REF_ID(invoker_bean, env, ref_id) \
	((invoker_bean)->ops->set_msg_context_ref_key(invoker_bean, env, ref_id))

#define SANDESHA2_INVOKER_BEAN_GET_MSG_NO(invoker_bean, env) \
	((invoker_bean)->ops->get_msg_no(invoker_bean, env))

#define SANDESHA2_INVOKER_BEAN_SET_MSG_NO(invoker_bean, env, msg_no) \
	((invoker_bean)->ops->set_msg_no(invoker_bean, env, msg_no))

#define SANDESHA2_INVOKER_BEAN_GET_SEQ_ID(invoker_bean, env) \
	((invoker_bean)->ops->get_seq_id(invoker_bean, env))

#define SANDESHA2_INVOKER_BEAN_SET_SEQ_ID(invoker_bean, env, seq_id) \
	((invoker_bean)->ops->set_seq_id(invoker_bean, env, seq_id))

#define SANDESHA2_INVOKER_BEAN_IS_INVOKED(invoker_bean, env) \
	((invoker_bean)->ops->is_invoked(invoker_bean, env))

#define SANDESHA2_INVOKER_BEAN_SET_INVOKED(invoker_bean, env, invoked) \
	 ((invoker_bean)->ops->set_invoked(invoker_bean, env, invoked))


#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_INVOKER_BEAN */
