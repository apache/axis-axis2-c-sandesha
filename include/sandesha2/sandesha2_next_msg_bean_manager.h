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

#ifndef SANDESHA2_NEXT_MSG_BEAN_MANAGER_H
#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_array_list.h>

#include <sandesha2/sandesha2_next_msg_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_next_msg_bean_manager sandesha2_next_msg_bean_manager_t;
typedef struct sandesha2_next_msg_bean_manager_ops sandesha2_next_msg_bean_manager_ops_t;

AXIS2_DECLARE_DATA struct sandesha2_next_msg_bean_manager_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env);
	
	axis2_bool_t (AXIS2_CALL *
		insert) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env,
			sandesha2_next_msg_bean_t *next_msg);

	axis2_bool_t (AXIS2_CALL *
		delete) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env,
			axis2_char_t *seq_id);

	sandesha2_next_msg_bean_t* (AXIS2_CALL *
		retrieve) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env,
			axis2_char_t *seq_id);
	
	axis2_array_list_t* (AXIS2_CALL *
		find) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env,
			sandesha2_next_msg_bean_t *next_msg);

	sandesha2_next_msg_bean_t* (AXIS2_CALL *
		find_unique) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env,
			sandesha2_next_msg_bean_t *next_msg);

	axis2_bool_t (AXIS2_CALL *
		update) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env,
			sandesha2_next_msg_bean_t *next_msg);

	axis2_array_list_t* (AXIS2_CALL *
		retrieve_all) (sandesha2_next_msg_bean_manager_t *next_msg_manager,
			const axis2_env_t *env);
			
};

AXIS2_DECLARE_DATA struct sandesha2_next_msg_bean_manager
{
	sandesha2_next_msg_bean_manager_ops_t *ops;
};


/* constructors 
 */
AXIS2_EXTERN sandesha2_next_msg_bean_manager_t* AXIS2_CALL
	sandesha2_next_msg_bean_manager_create(const axis2_env_t *env);

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_FREE(next_msg_manager, env) \
	((next_msg_manager)->ops->free(next_msg_manager, env))

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_INSERT(next_msg_manager, env, next_msg) \
	((next_msg_manager)->ops->insert(next_msg_manager, env, next_msg))

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_DELETE(next_msg_manager, env, seq_id) \
	((next_msg_manager)->ops->delete(next_msg_manager, env, seq_id))

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_RETRIEVE(next_msg_manager, env, seq_id) \
	((next_msg_manager)->ops->retrieve(next_msg_manager, env, seq_id))

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_FIND(next_msg_manager, env, next_msg) \
	((next_msg_manager)->ops->find(next_msg_manager, env, next_msg))

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_FIND_UNIQUE(next_msg_manager, env, next_msg) \
	((next_msg_manager)->ops->find_unique(next_msg_manager, env, next_msg))

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_UPDATE(next_msg_manager, env, next_msg) \
	((next_msg_manager)->ops->update(next_msg_manager, env, next_msg))

#define SANDESHA2_NEXT_MSG_BEAN_MANAGER_RETRIEVE_ALL(next_msg_manager, env) \
	((next_msg_manager)->ops->retrieve_all(next_msg_manager, env))

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_NEXT_MSG_BEAN_MANAGER_H*/
