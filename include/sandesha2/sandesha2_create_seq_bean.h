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

typedef struct sandesha2_create_seq_bean sandesha2_create_seq_bean_t;
typedef struct sandesha2_create_seq_bean_ops sandesha2_create_seq_bean_ops_t;

AXIS2_DECLARE_DATA struct sandesha2_create_seq_bean_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_create_seq_bean_t *create_seq,
			const axis2_env_t *env);

	axis2_char_t* (AXIS2_CALL *
		get_create_seq_msg_id) (sandesha2_create_seq_bean_t *create_seq,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_create_seq_msg_id) (sandesha2_create_seq_bean_t *create_seq,
			const axis2_env_t *env, axis2_char_t* seq_msg_id) ;

	axis2_char_t* (AXIS2_CALL *
		get_seq_id) (sandesha2_create_seq_bean_t *create_seq,
			const axis2_env_t *env);
	
	void (AXIS2_CALL *
		set_seq_id) (sandesha2_create_seq_bean_t *create_seq,
			const axis2_env_t *env, axis2_char_t* seq_id) ;

	axis2_char_t* (AXIS2_CALL *
		get_internal_seq_id) (sandesha2_create_seq_bean_t *create_seq,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_internal_seq_id) (sandesha2_create_seq_bean_t *create_seq,
			const axis2_env_t *env, axis2_char_t* int_seq_id);
};

AXIS2_DECLARE_DATA struct sandesha2_create_seq_bean
{
	sandesha2_create_seq_bean_ops_t *ops;
};

/* constructors 
 */
AXIS2_EXTERN sandesha2_create_seq_bean_t* AXIS2_CALL
	sandesha2_create_seq_bean_create(const axis2_env_t *env);

AXIS2_EXTERN sandesha2_create_seq_bean_t* AXIS2_CALL
	sandesha2_create_seq_bean_create_with_data(const axis2_env_t *env,
				axis2_char_t *internal_seq_id,
				axis2_char_t *create_seq_msg_id, 
				axis2_char_t *seq_id);


#define SANDESHA2_CREATE_SEQ_BEAN_FREE(create_seq_bean, env) \
	((create_seq_bean)->ops->free(create_seq_bean, env))

#define SANDESHA2_CREATE_SEQ_BEAN_GET_CREATE_SEQ_MSG_ID(create_seq_bean,env) \
	((create_seq_bean)->ops->get_create_seq_msg_id(create_seq_bean, env))

#define SANDESHA2_CREATE_SEQ_BEAN_SET_CREATE_SEQ_MSG_ID(create_seq_bean, env, create_msg_seq_id) \
	((create_seq_bean)->ops->set_create_seq_msg_id(create_seq_bean, env, create_msg_seq_id))

#define SANDESHA2_CREATE_SEQ_BEAN_GET_SEQ_ID(create_seq_bean, env) \
	((create_seq_bean)->ops->get_seq_id(create_seq_bean, env))

#define SANDESHA2_CREATE_SEQ_BEAN_SET_SEQ_ID(create_seq_bean, env, seq_id) \
	((create_seq_bean)->ops->set_seq_id(create_seq_bean, env, seq_id))

#define SANDESHA2_CREATE_SEQ_BEAN_GET_INTERNAL_SEQ_ID(create_seq_bean, env) \
	((create_seq_bean)->ops->get_internal_seq_id(create_seq_bean, env))

#define SANDESHA2_CREATE_SEQ_BEAN_SET_INTERNAL_SEQ_ID(create_seq_bean, env, int_seq_id) \
	((create_seq_bean)->ops->set_internal_seq_id(create_seq_bean, env, int_seq_id))

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_CREATE_SEQ_BEAN_H */
