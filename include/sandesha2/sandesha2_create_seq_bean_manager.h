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

#ifndef SANDESHA2_CREATE_SEQ_BEAN_MANAGER_H
#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_array_list.h>

#include <sandesha2/sandesha2_create_seq_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_create_seq_bean_manager sandesha2_create_seq_bean_manager_t;
typedef struct sandesha2_create_seq_bean_manager_ops sandesha2_create_seq_bean_manager_ops_t;

AXIS2_DECLARE_DATA struct sandesha2_create_seq_bean_manager_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_create_seq_bean_manager_t *create_seq_manager,
			const axis2_env_t *env);

	axis2_bool_t (AXIS2_CALL *
		insert) ( sandesha2_create_seq_bean_manager_t *create_seq_manager,
			const axis2_env_t *env,
			sandesha2_create_seq_bean_t *create_seq);


	axis2_bool_t (AXIS2_CALL *
		delete) ( sandesha2_create_seq_bean_manager_t *create_seq_manager,
			const axis2_env_t *env,
			axis2_char_t *msg_id);

	sandesha2_create_seq_bean_t *(AXIS2_CALL *
		retrieve) ( sandesha2_create_seq_bean_manager_t *create_seq_manager,
			const axis2_env_t *env,
			axis2_char_t *msg_id);

	axis2_bool_t (AXIS2_CALL *
		update) ( sandesha2_create_seq_bean_manager_t *create_seq_manager,
			const axis2_env_t *env,
			sandesha2_create_seq_bean_t *create_seq);

	axis2_array_list_t* (AXIS2_CALL *
		find) (sandesha2_create_seq_bean_manager_t *create_seq_manager,
			const axis2_env_t *env,
			sandesha2_create_seq_bean_t *create_seq);

	sandesha2_create_seq_bean_t *(AXIS2_CALL *
		find_unique) (sandesha2_create_seq_bean_manager_t *create_seq_manager,
			const axis2_env_t *env,
			sandesha2_create_seq_bean_t *create_seq);

};


AXIS2_DECLARE_DATA struct sandesha2_create_seq_bean_manager{
	sandesha2_create_seq_bean_manager_ops_t *ops;
}; 


/* constructors 
 */
AXIS2_EXTERN sandesha2_create_seq_bean_manager_t* AXIS2_CALL
	sandesha2_create_seq_bean_manager_create(const axis2_env_t *env);


#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_FREE(create_seq_bean_manager, env) \
	((create_seq_bean_manager)->ops->free(create_seq_bean_manager, env))

#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_INSERT(create_seq_bean_manager, env, create_seq_bean) \
	((create_seq_bean_manager)->ops->insert(create_seq_bean_manager, env, create_seq_bean))

#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_DELETE(create_seq_bean_manager, env, msg_id) \
	((create_seq_bean_manager)->ops->delete(create_seq_bean_manager, env, msg_id))

#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_RETRIEVE(create_seq_bean_manager, env, msg_id) \
	((create_seq_bean_manager)->ops->retrieve(create_seq_bean_manager, env, msg_id))

#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_UPDATE(create_seq_bean_manager, env, create_seq_bean) \
	((create_seq_bean_manager)->ops->update(create_seq_bean_manager, env, create_seq_bean))

#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_FIND(create_seq_bean_manager, env, create_seq_bean) \
	((create_seq_bean_manager)->ops->find(create_seq_bean_manager, env, create_seq_bean))

#define SANDESHA2_CREATE_SEQ_BEAN_MANAGER_FIND_UNIQUE(create_seq_bean_manager, env, create_seq_bean) \
	((create_seq_bean_manager)->ops->find_unique(create_seq_bean_manager, env, create_seq_bean))

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_CREATE_SEQ_BEAN_MANAGER_H */
