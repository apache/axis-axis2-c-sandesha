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

#ifndef SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_H
#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_array_list.h>

#include <sandesha2/sandesha2_sequence_property_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_sequence_property_bean_manager sandesha2_sequence_property_bean_manager_t;
typedef struct sandesha2_sequence_property_bean_manager_ops sandesha2_sequence_property_bean_manager_ops_t;

AXIS2_DECLARE_DATA struct sandesha2_sequence_property_bean_manager_ops
{

	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env);

	axis2_bool_t (AXIS2_CALL *
		insert) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env,
			sandesha2_sequence_property_bean_t *prop_bean);


	axis2_bool_t (AXIS2_CALL *
		delete) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager, 
			const axis2_env_t *env,
			axis2_char_t *seq_id,
			axis2_char_t *name);

	sandesha2_sequence_property_bean_t* (AXIS2_CALL *
		retrieve) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env,
			axis2_char_t *seq_id,
			axis2_char_t *name);

	axis2_bool_t (AXIS2_CALL *
		update) ( sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env,
			sandesha2_sequence_property_bean_t *prop_bean);

	axis2_array_list_t* (AXIS2_CALL *
		find) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env,
			sandesha2_sequence_property_bean_t *prop_bean);	

	sandesha2_sequence_property_bean_t* (AXIS2_CALL *
		find_unique) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env,
			sandesha2_sequence_property_bean_t *prop_bean);

	axis2_bool_t (AXIS2_CALL *
		update_or_insert) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env,
			sandesha2_sequence_property_bean_t *prop_bean);

	axis2_array_list_t* (AXIS2_CALL *
		retrieve_all) (sandesha2_sequence_property_bean_manager_t *prop_bean_manager,
			const axis2_env_t *env);

};


AXIS2_DECLARE_DATA struct sandesha2_sequence_property_bean_manager
{
	sandesha2_sequence_property_bean_manager_ops_t ops;
};


/* constructors 
 */

AXIS2_EXTERN sandesha2_sequence_property_bean_manager_t* AXIS2_CALL
	sandesha2_sequence_property_bean_manager_create(const axis2_env_t *env);

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_FREE(prop_bean_manager, env) \
	((prop_bean_manager)->ops->free(prop_bean_manager, env))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_INSERT(prop_bean_manager, env, prop_bean) \
	((prop_bean_manager)->ops->insert(prop_bean_manager, env, prop_bean))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_DELETE(prop_bean_manager, env, seq_id, name) \
	((prop_bean_manager)->ops->delete(prop_bean_manager, env, seq_id, name))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_RETRIEVE(prop_bean_manager, env, seq_id, name) \
	((prop_bean_manager)->ops->retrieve(prop_bean_manager, env, seq_id, name))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_UPDATE(prop_bean_manager, env, prop_bean) \
	((prop_bean_manager)->ops->update(prop_bean_manager, env, prop_bean))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_FIND(prop_bean_manager, env, prop_bean) \
	((prop_bean_manager)->ops->find(prop_bean_manager, env, prop_bean))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_FIND_UNIQUE(prop_bean_manager, env, prop_bean) \
	((prop_bean_manager)->ops->find_unique(prop_bean_manager, env, prop_bean))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_UPDATE_OR_INSERT(prop_bean_manager, env, prop_bean) \
	((prop_bean_manager)->ops->update_or_insert(prop_bean_manager, env, prop_bean))

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_RETRIEVE_ALL(prop_bean_manager, env) \
	((prop_bean_manager)->ops->retrieve_all(prop_bean_manager, env))

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_SEQUENCE_PROPERTY_BEAN_MANAGER_H*/
