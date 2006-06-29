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

#include <sandesha2/sandesha2_sequence_property_bean.h>
#include <string.h>

/*sequence_property_bean struct */
typedef struct sandesha2_sequence_property_bean_impl sandesha2_sequence_property_bean_impl_t;

struct sandesha2_sequence_property_bean_impl
{
	sandesha2_sequence_property_bean_t o_bean;
	axis2_char_t *sequence_id;
	axis2_char_t *name;
	axis2_char_t *value;
};

#define AXIS2_INTF_TO_IMPL(prop_bean) ((sandesha2_sequence_property_bean_impl_t*)prop_bean)


/************* FUNCTION prototypes ***************/
axis2_status_t AXIS2_CALL
	sandesha2_sequence_property_bean_free (struct sandesha2_sequence_property_bean *seq_property,
		  const axis2_env_t *env);

axis2_char_t *AXIS2_CALL
	sandesha2_sequence_property_bean_get_name (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env);

void AXIS2_CALL 
	sandesha2_sequence_property_bean_set_name (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env,
		axis2_char_t *name);

axis2_char_t *AXIS2_CALL
	sandesha2_sequence_property_bean_get_sequence_id (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_sequence_property_bean_set_sequence_id (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env,
		axis2_char_t *seq_id);

axis2_char_t* AXIS2_CALL
	sandesha2_sequence_property_bean_get_value (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_sequence_property_bean_set_value (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env,
		axis2_char_t *value);


AXIS2_EXTERN sandesha2_sequence_property_bean_t* AXIS2_CALL
        sandesha2_sequence_property_bean_create(const axis2_env_t *env)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean_impl = (sandesha2_sequence_property_bean_impl_t *)AXIS2_MALLOC(env->allocator,
				sizeof(sandesha2_sequence_property_bean_impl_t));

	if(!bean_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* initialize properties */
	bean_impl->sequence_id = NULL;
	bean_impl->name = NULL;
	bean_impl->value = NULL;

	bean_impl->o_bean.ops = NULL;
	bean_impl->o_bean.ops = (sandesha2_sequence_property_bean_ops_t*)AXIS2_MALLOC(env->allocator,
					sizeof(sandesha2_sequence_property_bean_ops_t));

	if(!bean_impl->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean_impl);
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);

		return NULL;
	}

	bean_impl->o_bean.ops->free = sandesha2_sequence_property_bean_free;
	bean_impl->o_bean.ops->get_name = sandesha2_sequence_property_bean_get_name;
	bean_impl->o_bean.ops->set_name = sandesha2_sequence_property_bean_set_name;
	bean_impl->o_bean.ops->get_sequence_id = sandesha2_sequence_property_bean_get_sequence_id;
	bean_impl->o_bean.ops->set_sequence_id = sandesha2_sequence_property_bean_set_sequence_id;
	bean_impl->o_bean.ops->get_value = sandesha2_sequence_property_bean_get_value;
	bean_impl->o_bean.ops->set_value = sandesha2_sequence_property_bean_set_value;

	return &(bean_impl->o_bean);
}

AXIS2_EXTERN sandesha2_sequence_property_bean_t* AXIS2_CALL
        sandesha2_sequence_property_bean_create_with_data(const axis2_env_t *env,
					axis2_char_t *seq_id,
					axis2_char_t *prop_name,
					axis2_char_t *value)

{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean_impl = (sandesha2_sequence_property_bean_impl_t *)AXIS2_MALLOC(env->allocator,
				sizeof(sandesha2_sequence_property_bean_impl_t));

	if(!bean_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* initialize properties */
	bean_impl->sequence_id = (axis2_char_t *)AXIS2_STRDUP(env, seq_id);
	bean_impl->name = (axis2_char_t *)AXIS2_STRDUP(env, prop_name);
	bean_impl->value = (axis2_char_t *)AXIS2_STRDUP(env, value);

	bean_impl->o_bean.ops = NULL;
	bean_impl->o_bean.ops = (sandesha2_sequence_property_bean_ops_t*)AXIS2_MALLOC(env->allocator,
					sizeof(sandesha2_sequence_property_bean_ops_t));

	if(!bean_impl->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean_impl);
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);

		return NULL;
	}

	bean_impl->o_bean.ops->free = sandesha2_sequence_property_bean_free;
	bean_impl->o_bean.ops->get_name = sandesha2_sequence_property_bean_get_name;
	bean_impl->o_bean.ops->set_name = sandesha2_sequence_property_bean_set_name;
	bean_impl->o_bean.ops->get_sequence_id = sandesha2_sequence_property_bean_get_sequence_id;
	bean_impl->o_bean.ops->set_sequence_id = sandesha2_sequence_property_bean_set_sequence_id;
	bean_impl->o_bean.ops->get_value = sandesha2_sequence_property_bean_get_value;
	bean_impl->o_bean.ops->set_value = sandesha2_sequence_property_bean_set_value;

	return &(bean_impl->o_bean);
}


axis2_status_t AXIS2_CALL
	sandesha2_sequence_property_bean_free (struct sandesha2_sequence_property_bean *seq_property,
		  const axis2_env_t *env)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(seq_property);

	if(bean_impl->sequence_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->sequence_id);
		bean_impl->sequence_id = NULL;
	}

	if(bean_impl->name)
	{
		AXIS2_FREE(env->allocator, bean_impl->name);
		bean_impl->name = NULL;
	}

	if(bean_impl->value)
	{
		AXIS2_FREE(env->allocator,  bean_impl->value);
		bean_impl->value = NULL;
	}
    return AXIS2_SUCCESS;
}

axis2_char_t *AXIS2_CALL
	sandesha2_sequence_property_bean_get_name (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(seq_property);

	return bean_impl->name;
}

void AXIS2_CALL 
	sandesha2_sequence_property_bean_set_name (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env,
		axis2_char_t *name)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(seq_property);

	if(bean_impl->name)
	{
		AXIS2_FREE(env->allocator, bean_impl->name);
		bean_impl->name = NULL;
	}

	bean_impl->name = (axis2_char_t *)AXIS2_STRDUP(env, name);
}

axis2_char_t *AXIS2_CALL
	sandesha2_sequence_property_bean_get_sequence_id (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(seq_property);

	return bean_impl->sequence_id;
}


void AXIS2_CALL
	sandesha2_sequence_property_bean_set_sequence_id (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env,
		axis2_char_t *seq_id)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(seq_property);

	if(bean_impl->sequence_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->sequence_id);
		bean_impl->sequence_id = NULL;
	}

	bean_impl->sequence_id = (axis2_char_t *)AXIS2_STRDUP(env, seq_id);
}

axis2_char_t* AXIS2_CALL
	sandesha2_sequence_property_bean_get_value (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(seq_property);

	return bean_impl->value;
}


void AXIS2_CALL
	sandesha2_sequence_property_bean_set_value (struct sandesha2_sequence_property_bean *seq_property,
		const axis2_env_t *env,
		axis2_char_t *value)
{
	sandesha2_sequence_property_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(seq_property);

	if(bean_impl->value)
	{
		AXIS2_FREE(env->allocator, bean_impl->value);
		bean_impl->value = NULL;
	}

	bean_impl->value = (axis2_char_t *)AXIS2_STRDUP(env, value);

}

