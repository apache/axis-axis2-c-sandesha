/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sandesha2_next_msg_bean.h>
#include <string.h>
#include <axis2_string.h>
#include <axis2_utils.h>

/* invoker_bean struct */
typedef struct sandesha2_next_msg_bean_impl sandesha2_next_msg_bean_impl_t;

struct sandesha2_next_msg_bean_impl
{
	sandesha2_next_msg_bean_t o_bean;
	axis2_char_t *seq_id;
	long msg_no;
};

#define AXIS2_INTF_TO_IMPL(next_msg) ((sandesha2_next_msg_bean_impl_t*)next_msg)


/************* FUNCTION prototypes ***************/

axis2_status_t AXIS2_CALL
	sandesha2_next_msg_bean_free (sandesha2_next_msg_bean_t *next_msg_bean,
		 const axis2_env_t *env);

axis2_char_t* AXIS2_CALL
	sandesha2_next_msg_bean_get_seq_id(sandesha2_next_msg_bean_t *next_msg_bean,
		const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_next_msg_bean_set_seq_id(sandesha2_next_msg_bean_t *next_msg_bean,
		const axis2_env_t *env, axis2_char_t *seq_id);

long AXIS2_CALL
	sandesha2_next_msg_bean_get_next_msg_no_to_process(sandesha2_next_msg_bean_t *next_msg_bean,
		const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_next_msg_bean_set_next_msg_no_to_process(sandesha2_next_msg_bean_t *next_msg_bean,
		const axis2_env_t *env, long next_msg_no);




AXIS2_EXTERN sandesha2_next_msg_bean_t* AXIS2_CALL
sandesha2_next_msg_bean_create(const axis2_env_t *env)
{
	sandesha2_next_msg_bean_impl_t *bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_next_msg_bean_impl_t *)AXIS2_MALLOC(env->allocator,
							sizeof(sandesha2_next_msg_bean_impl_t));

	if(!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	bean->seq_id = NULL;
	bean->msg_no = -1;

	bean->o_bean.ops = NULL;
	bean->o_bean.ops = (sandesha2_next_msg_bean_ops_t*) AXIS2_MALLOC(env->allocator,
								sizeof(sandesha2_next_msg_bean_ops_t));

	if(!bean->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean);
		return NULL;
	}

	bean->o_bean.ops->free = sandesha2_next_msg_bean_free;
	bean->o_bean.ops->get_seq_id = sandesha2_next_msg_bean_get_seq_id;
	bean->o_bean.ops->set_seq_id = sandesha2_next_msg_bean_set_seq_id;
	bean->o_bean.ops->set_next_msg_no_to_process = sandesha2_next_msg_bean_set_next_msg_no_to_process;

	return &(bean->o_bean);
}

AXIS2_EXTERN sandesha2_next_msg_bean_t* AXIS2_CALL
	sandesha2_next_msg_bean_create_with_data(const axis2_env_t *env,
						axis2_char_t *seq_id,
						long msg_no)
{
	sandesha2_next_msg_bean_impl_t *bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_next_msg_bean_impl_t *)AXIS2_MALLOC(env->allocator,
							sizeof(sandesha2_next_msg_bean_impl_t));

	if(!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	bean->seq_id = (axis2_char_t*)AXIS2_STRDUP(seq_id, env);
	bean->msg_no = msg_no;

	bean->o_bean.ops = NULL;
	bean->o_bean.ops = (sandesha2_next_msg_bean_ops_t*)AXIS2_MALLOC(env->allocator,
								sizeof(sandesha2_next_msg_bean_ops_t));

	if(!bean->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean);
		return NULL;
	}

	bean->o_bean.ops->free = sandesha2_next_msg_bean_free;
	bean->o_bean.ops->get_seq_id = sandesha2_next_msg_bean_get_seq_id;
	bean->o_bean.ops->set_seq_id = sandesha2_next_msg_bean_set_seq_id;
	bean->o_bean.ops->set_next_msg_no_to_process = sandesha2_next_msg_bean_set_next_msg_no_to_process;

	return &(bean->o_bean);
}

axis2_status_t AXIS2_CALL
sandesha2_next_msg_bean_free (sandesha2_next_msg_bean_t *next_msg,
				const axis2_env_t *env)
{
	sandesha2_next_msg_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(next_msg);

	if(bean_impl->seq_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->seq_id);
		bean_impl->seq_id= NULL;
	}
    return AXIS2_SUCCESS;
		
}


axis2_char_t* AXIS2_CALL
sandesha2_next_msg_bean_get_seq_id(sandesha2_next_msg_bean_t *next_msg_bean,
				const axis2_env_t *env)
{
	sandesha2_next_msg_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(next_msg_bean);

	return bean_impl->seq_id;
}


void AXIS2_CALL
sandesha2_next_msg_bean_set_seq_id(sandesha2_next_msg_bean_t *next_msg_bean,
				const axis2_env_t *env, axis2_char_t *seq_id)
{
	sandesha2_next_msg_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(next_msg_bean);

	if(bean_impl->seq_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->seq_id);
		bean_impl->seq_id = NULL;
	}

	bean_impl->seq_id = (axis2_char_t*) AXIS2_STRDUP(seq_id, env); 
}


long AXIS2_CALL
sandesha2_next_msg_bean_get_next_msg_no_to_process(sandesha2_next_msg_bean_t *next_msg_bean,
				const axis2_env_t *env)
{
	sandesha2_next_msg_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(next_msg_bean);

	return bean_impl->msg_no;
}

void AXIS2_CALL
        sandesha2_next_msg_bean_set_next_msg_no_to_process(sandesha2_next_msg_bean_t *next_msg_bean,
				const axis2_env_t *env, long next_msg_no)
{
	sandesha2_next_msg_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(next_msg_bean);

	bean_impl->msg_no = next_msg_no;
}



