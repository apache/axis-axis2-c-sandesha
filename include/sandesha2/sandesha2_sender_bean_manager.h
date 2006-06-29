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

#ifndef SANDESHA2_SENDER_BEAN_MANAGER_H
#define SANDESHA2_SENDER_BEAN_MANAGER_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_array_list.h>

#include <sandesha2/sandesha2_sender_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_sender_bean_manager sandesha2_sender_bean_manager_t;
typedef struct sandesha2_sender_bean_manager_ops sandesha2_sender_bean_manager_ops_t;


AXIS2_DECLARE_DATA struct sandesha2_sender_bean_manager_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env);

	axis2_bool_t (AXIS2_CALL *
		insert) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env,
			sandesha2_sender_bean_t *sender_bean);

	axis2_bool_t (AXIS2_CALL *
		delete) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env,
			axis2_char_t *msg_id);

	sandesha2_sender_bean_manager_t* (AXIS2_CALL *
		retrieve) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env,
			axis2_char_t *msg_id);

	axis2_bool_t (AXIS2_CALL *
		update) (sandesha2_sender_bean_manager_t* sender_manager,
			const axis2_env_t *env,
			sandesha2_sender_bean_t *sender_bean);

	axis2_array_list_t* (AXIS2_CALL *
		find) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env,
			sandesha2_sender_bean_t *sender_bean);

	sandesha2_sender_bean_t* (AXIS2_CALL *
		find_unique) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env,
			sandesha2_sender_bean_t *sender_bean);

	sandesha2_sender_bean_t* (AXIS2_CALL *
		get_next_msg_to_send) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env);

	sandesha2_sender_bean_t* (AXIS2_CALL *
		retrieve_from_msg_ref_key) (sandesha2_sender_bean_manager_t *sender_manager,
			const axis2_env_t *env,
			axis2_char_t *msg_ref_key);
};


AXIS2_DECLARE_DATA struct sandesha2_sender_bean_manager{
	sandesha2_sender_bean_manager_ops_t *ops;
}; 


/* constructors 
 */
AXIS2_EXTERN sandesha2_sender_bean_manager_t* AXIS2_CALL
	sandesha2_sender_bean_manager_create(const axis2_env_t *env);

#define SANDESHA2_SENDER_BEAN_MANAGER_FREE(sender_manager, env) \
	((sender_manager)->ops->free(sender_manager, env))

#define SANDESHA2_SENDER_BEAN_MANAGER_INSERT(sender_manager, env, sender_bean) \
	((sender_manager)->ops->insert(sender_manager, env, sender_bean))

#define SANDESHA2_SENDER_BEAN_MANAGER_DELETE(sender_manager, env, msg_id) \
	((sender_manager)->ops->delete(sender_manager, env, msg_id))

#define SANDESHA2_SENDER_BEAN_MANAGER_RETRIEVE(sender_manager, env, msg_id) \
	((sender_manager)->ops->retrieve(sender_manager, env, msg_id))

#define SANDESHA2_SENDER_BEAN_MANAGER_UPDATE(sender_manager, env, sender_bean) \
	((sender_manager)->ops->update(sender_manager, env, sender_bean))

#define SANDESHA2_SENDER_BEAN_MANAGER_FIND(sender_manager, env, sender_bean) \
	((sender_manager)->ops->find(sender_manager, env, sender_bean))

#define SANDESHA2_SENDER_BEAN_MANAGER_FIND_UNIQUE(sender_manager, env, sender_bean) \
	((sender_manager)->ops->find_unique(sender_manager, env, sender_bean))

#define SANDESHA2_SENDER_BEAN_MANAGER_GET_NEXT_MSG_TO_SEND(sender_manager, env) \
	((sender_manager)->ops->get_next_msg_to_send(sender_manager, env))

#define SANDESHA2_SENDER_BEAN_MANAGER_RETRIEVE_FROM_MSG_REF_KEY(sender_manager, env, msg_ref_key) \
	((sender_manager)->ops->retrieve_from_msg_ref_key(sender_manager, env, msg_ref_key))

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_SENDER_BEAN_MANAGER_H*/
