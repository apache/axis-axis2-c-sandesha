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

#include <sandesha2_seq_property_bean.h>
#include <sandesha2_rm_bean.h>
#include <string.h>
#include <axis2_string.h>
#include <axis2_utils.h>


/*seq_property_bean struct */
struct sandesha2_seq_property_bean_t
{
    sandesha2_rm_bean_t *rm_bean;
	axis2_char_t *seq_id;
	axis2_char_t *name;
	axis2_char_t *value;
};

AXIS2_EXTERN sandesha2_seq_property_bean_t* AXIS2_CALL
sandesha2_seq_property_bean_create(
    const axis2_env_t *env)
{
	sandesha2_seq_property_bean_t *seq_property = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	seq_property = (sandesha2_seq_property_bean_t *)AXIS2_MALLOC(env->allocator,
				sizeof(sandesha2_seq_property_bean_t));

	if(!seq_property)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* initialize properties */
	seq_property->seq_id = NULL;
	seq_property->name = NULL;
	seq_property->value = NULL;
    seq_property->rm_bean = NULL;

	return seq_property;
}

AXIS2_EXTERN sandesha2_seq_property_bean_t* AXIS2_CALL
sandesha2_seq_property_bean_create_with_data(
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axis2_char_t *prop_name,
    axis2_char_t *value)

{
	sandesha2_seq_property_bean_t *seq_property = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	seq_property = (sandesha2_seq_property_bean_t *)AXIS2_MALLOC(env->allocator,
				sizeof(sandesha2_seq_property_bean_t));

	if(!seq_property)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* initialize properties */
	seq_property->seq_id = (axis2_char_t *)AXIS2_STRDUP(seq_id, env);
	seq_property->name = (axis2_char_t *)AXIS2_STRDUP(prop_name, env);
	seq_property->value = (axis2_char_t *)AXIS2_STRDUP(value, env);
    seq_property->rm_bean = NULL;

	return seq_property;
}


axis2_status_t AXIS2_CALL
sandesha2_seq_property_bean_free (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env)
{
	if(seq_property->rm_bean)
	{
		sandesha2_rm_bean_free(seq_property->rm_bean, env);
		seq_property->rm_bean= NULL;
	}
	if(seq_property->seq_id)
	{
		AXIS2_FREE(env->allocator, seq_property->seq_id);
		seq_property->seq_id = NULL;
	}

	if(seq_property->name)
	{
		AXIS2_FREE(env->allocator, seq_property->name);
		seq_property->name = NULL;
	}

	if(seq_property->value)
	{
		AXIS2_FREE(env->allocator,  seq_property->value);
		seq_property->value = NULL;
	}
    return AXIS2_SUCCESS;
}

sandesha2_rm_bean_t * AXIS2_CALL
sandesha2_seq_property_bean_get_base( 
    sandesha2_seq_property_bean_t* seq_property,
    const axis2_env_t *env)
{
	return seq_property->rm_bean;
}	

void AXIS2_CALL
sandesha2_seq_property_bean_set_base (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env, 
    sandesha2_rm_bean_t* rm_bean)
{
	seq_property->rm_bean = rm_bean;
}

axis2_char_t *AXIS2_CALL
sandesha2_seq_property_bean_get_name (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env)
{
	return seq_property->name;
}

void AXIS2_CALL 
sandesha2_seq_property_bean_set_name (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env,
    axis2_char_t *name)
{
	if(seq_property->name)
	{
		AXIS2_FREE(env->allocator, seq_property->name);
		seq_property->name = NULL;
	}

	seq_property->name = (axis2_char_t *)AXIS2_STRDUP(name, env);
}

axis2_char_t *AXIS2_CALL
sandesha2_seq_property_bean_get_seq_id (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env)
{
	return seq_property->seq_id;
}


void AXIS2_CALL
sandesha2_seq_property_bean_set_seq_id (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env,
    axis2_char_t *seq_id)
{
	if(seq_property->seq_id)
	{
		AXIS2_FREE(env->allocator, seq_property->seq_id);
		seq_property->seq_id = NULL;
	}

	seq_property->seq_id = (axis2_char_t *)AXIS2_STRDUP(seq_id, env);
}

axis2_char_t* AXIS2_CALL
sandesha2_seq_property_bean_get_value (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env)
{
	return seq_property->value;
}


void AXIS2_CALL
sandesha2_seq_property_bean_set_value (
    sandesha2_seq_property_bean_t *seq_property,
    const axis2_env_t *env,
    axis2_char_t *value)
{
	if(seq_property->value)
	{
		AXIS2_FREE(env->allocator, seq_property->value);
		seq_property->value = NULL;
	}

	seq_property->value = (axis2_char_t *)AXIS2_STRDUP(value, env);

}

