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

#ifndef SANDESHA2_INVOKER_BEAN_MANAGER_H
#define SANDESHA2_INVOKER_BEAN_MANAGER_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_array_list.h>

#include <sandesha2/sandesha2_invoker_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_invoker_bean_manager sandesha2_invoker_bean_manager_t;
typedef struct sandesha2_invoker_bean_manager_ops sandesha2_invoker_bean_manager_ops_t;


AXIS2_DECLARE_DATA struct sandesha2_invoker_bean_manager_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_invoker_bean_manager_t *invoker_manager,
			const axis2_env_t *env);

	axis2_bool_t (AXIS2_CALL *
		insert) (sandesha2_invoker_bean_manager_t *invoker_manager,
			const axis2_env_t *env,
			sandesha2_invoker_bean_t *invoker);

	axis2_bool_t (AXIS2_CALL *
		delete) (sandesha2_invoker_bean_manager_t *invoker_manager,
			const axis2_env_t *env,
			axis2_char_t *key);

	sandesha2_invoker_bean_t* (AXIS2_CALL *
		retrieve) (sandesha2_invoker_bean_manager_t *invoker_manager,
			const axis2_env_t *env,
			axis2_char_t *key);

	axis2_array_list_t* (AXIS2_CALL *
		find) (sandesha2_invoker_bean_manager_t *invoker_manager,
			const axis2_env_t *env,
			sandesha2_invoker_bean_t *invoker);

	sandesha2_invoker_bean_t* (AXIS2_CALL *
		find_unique) (sandesha2_invoker_bean_manager_t *invoker_manager,
			const axis2_env_t *env,
			sandesha2_invoker_bean_t *invoker);

	axis2_bool_t (AXIS2_CALL *
		update) (sandesha2_invoker_bean_manager_t *invoker_manager,
			const axis2_env_t *env,
			sandesha2_invoker_bean_t *invoker);

};

AXIS2_DECLARE_DATA struct sandesha2_invoker_bean_manager{
	sandesha2_invoker_bean_manager_ops_t *ops;
};


/* constructors 
 */
AXIS2_EXTERN sandesha2_invoker_bean_manager_t* AXIS2_CALL
	sandesha2_invoker_bean_manager_create(const axis2_env_t *env);

#define SANDESHA2_INVOKER_BEAN_MANAGER_FREE(invoker_bean_manager, env) \
	((invoker_bean_manager)->ops->free(invoker_bean_manager, env))

#define SANDESHA2_INVOKER_BEAN_MANAGER_INSERT(invoker_bean_manager, env, invoker) \
	((invoker_bean_manager)->ops->insert(invoker_bean_manager, env, invoker))

#define SANDESHA2_INVOKER_BEAN_MANAGER_DELETE(invoker_bean_manager, env, key) \
	((invoker_bean_manager)->ops->delete(invoker_bean_manager, env, key))

#define SANDESHA2_INVOKER_BEAN_MANAGER_RETRIEVE(invoker_bean_manager, env, key) \
	((invoker_bean_manager)->ops->retrieve(invoker_bean_manager, env, key))

#define SANDESHA2_INVOKER_BEAN_MANAGER_FIND(invoker_bean_manager, env, invoker) \
	((invoker_bean_manager)->ops->find(invoker_bean_manager, env, invoker))

#define SANDESHA2_INVOKER_BEAN_MANAGER_FIND_UNIQUE(invoker_bean_manager, env, invoker) \
	((invoker_bean_manager)->ops->find_unique(invoker_bean_manager, env, invoker))

#define SANDESHA2_INVOKER_BEAN_MANAGER_UPDATE(invoker_bean_manager, env, invoker) \
	((invoker_bean_manager)->ops->update(invoker_bean_manager, env, invoker))

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_INVOKER_BEAN_MANAGER_H*/
