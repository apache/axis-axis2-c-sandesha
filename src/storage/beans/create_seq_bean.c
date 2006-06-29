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

#include <sandesha2/sandesha2_create_seq_bean.h>
#include <axis2_env.h>
#include <axis2_utils.h>
#include <axis2_utils_defines.h>
#include <string.h>


/*create_seq_bean struct */
typedef struct sandesha2_create_seq_bean_impl sandesha2_create_seq_bean_impl_t;

struct sandesha2_create_seq_bean_impl
{
	sandesha2_create_seq_bean_t o_bean;

	/*  a unique identifier that can be used to identify the messages of a certain sequence */
	axis2_char_t *internal_seq_id;

	/* This is the message ID of the create sequence message. */
	axis2_char_t *create_seq_msg_id;

	/*  This is the actual Sequence ID of the sequence.*/
	axis2_char_t *seq_id;

};


#define AXIS2_INTF_TO_IMPL(create_bean) ((sandesha2_create_seq_bean_impl_t*)create_bean)
/* FUNCTION prototypes */

axis2_status_t AXIS2_CALL 
sandesha2_create_seq_bean_free  (sandesha2_create_seq_bean_t *create_seq,
		const axis2_env_t *env);

axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_bean_get_create_seq_msg_id  (sandesha2_create_seq_bean_t *create_seq,
		const axis2_env_t *env);

void AXIS2_CALL 
sandesha2_create_seq_bean_set_create_seq_msg_id (sandesha2_create_seq_bean_t *create_seq,
		const axis2_env_t *env, axis2_char_t* seq_msg_id) ;

axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_bean_get_seq_id (sandesha2_create_seq_bean_t *create_seq,
		const axis2_env_t *env);
	
void AXIS2_CALL
sandesha2_create_seq_bean_set_seq_id (sandesha2_create_seq_bean_t *create_seq,
		const axis2_env_t *env, axis2_char_t* seq_id) ;

axis2_char_t* AXIS2_CALL
sandesha2_create_seq_bean_get_internal_seq_id (sandesha2_create_seq_bean_t *create_seq,
		const axis2_env_t *env);

void AXIS2_CALL
sandesha2_create_seq_bean_set_internal_seq_id (sandesha2_create_seq_bean_t *create_seq,
		const axis2_env_t *env, axis2_char_t* int_seq_id);



AXIS2_EXTERN sandesha2_create_seq_bean_t * AXIS2_CALL
sandesha2_create_seq_bean_create(const axis2_env_t *env )
{
	sandesha2_create_seq_bean_impl_t *bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_create_seq_bean_impl_t *) AXIS2_MALLOC(env->allocator, 
				sizeof(sandesha2_create_seq_bean_impl_t) );

	if (!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	bean->internal_seq_id = NULL;
	bean->create_seq_msg_id = NULL;
	bean->seq_id = NULL;


	bean->o_bean.ops = NULL;
	bean->o_bean.ops = (sandesha2_create_seq_bean_ops_t *)AXIS2_MALLOC(env->allocator,
								sizeof(sandesha2_create_seq_bean_ops_t));

	if(!bean->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean);
		return NULL;
	}

	bean->o_bean.ops->free = sandesha2_create_seq_bean_free;
	bean->o_bean.ops->get_create_seq_msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id;
	bean->o_bean.ops->set_create_seq_msg_id = sandesha2_create_seq_bean_set_create_seq_msg_id ;
	bean->o_bean.ops->get_seq_id = sandesha2_create_seq_bean_get_seq_id;
	bean->o_bean.ops->set_seq_id = sandesha2_create_seq_bean_set_seq_id;
	bean->o_bean.ops->get_internal_seq_id = sandesha2_create_seq_bean_get_internal_seq_id;
	bean->o_bean.ops->set_internal_seq_id = sandesha2_create_seq_bean_set_internal_seq_id;

	return &(bean->o_bean);
}
	


AXIS2_EXTERN sandesha2_create_seq_bean_t* AXIS2_CALL 
sandesha2_create_seq_bean_create_with_data(const axis2_env_t *env,
				axis2_char_t *internal_seq_id,
				axis2_char_t *create_seq_msg_id,
				axis2_char_t *seq_id)
{
	sandesha2_create_seq_bean_impl_t *bean = NULL;

	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_create_seq_bean_impl_t*) AXIS2_MALLOC(env->allocator, 
				sizeof(sandesha2_create_seq_bean_impl_t));

	if (!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	bean->internal_seq_id = internal_seq_id;
	bean->create_seq_msg_id = create_seq_msg_id;
	bean->seq_id = seq_id;


	bean->o_bean.ops = NULL;
	bean->o_bean.ops = (sandesha2_create_seq_bean_ops_t *)AXIS2_MALLOC(env->allocator,
								sizeof(sandesha2_create_seq_bean_ops_t));

	if(!bean->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean);
		return NULL;
	}

	bean->o_bean.ops->free = sandesha2_create_seq_bean_free;
	bean->o_bean.ops->get_create_seq_msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id;
	bean->o_bean.ops->set_create_seq_msg_id = sandesha2_create_seq_bean_set_create_seq_msg_id ;
	bean->o_bean.ops->get_seq_id = sandesha2_create_seq_bean_get_seq_id;
	bean->o_bean.ops->set_seq_id = sandesha2_create_seq_bean_set_seq_id;
	bean->o_bean.ops->get_internal_seq_id = sandesha2_create_seq_bean_get_internal_seq_id;
	bean->o_bean.ops->set_internal_seq_id = sandesha2_create_seq_bean_set_internal_seq_id;
	bean->o_bean.ops->free = sandesha2_create_seq_bean_free;
	bean->o_bean.ops->free = sandesha2_create_seq_bean_free;

	return &(bean->o_bean);
}


axis2_status_t AXIS2_CALL
sandesha2_create_seq_bean_free  (sandesha2_create_seq_bean_t *create_seq,
		                const axis2_env_t *env)
{
	sandesha2_create_seq_bean_impl_t *bean_impl = NULL;
	bean_impl = (sandesha2_create_seq_bean_impl_t*)create_seq;

	if(bean_impl->internal_seq_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->internal_seq_id);
		bean_impl->internal_seq_id = NULL;
	}
		
	if(bean_impl->create_seq_msg_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->create_seq_msg_id);
		bean_impl->create_seq_msg_id= NULL;
	}

	if(bean_impl->seq_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->seq_id);
		bean_impl->seq_id= NULL;
	}
    return AXIS2_SUCCESS;
}

axis2_char_t * AXIS2_CALL
sandesha2_create_seq_bean_get_create_seq_msg_id( sandesha2_create_seq_bean_t* create_seq,
		const axis2_env_t *env)
{
	sandesha2_create_seq_bean_impl_t *bean_impl = NULL;
	bean_impl = (sandesha2_create_seq_bean_impl_t*)create_seq;
	return bean_impl->create_seq_msg_id;

}
	

void AXIS2_CALL
sandesha2_create_seq_bean_set_create_seq_msg_id (sandesha2_create_seq_bean_t *create_seq,
			                const axis2_env_t *env, axis2_char_t* seq_msg_id)

{
	sandesha2_create_seq_bean_impl_t *bean_impl = NULL;
	bean_impl = (sandesha2_create_seq_bean_impl_t*)create_seq;
	bean_impl->create_seq_msg_id = AXIS2_STRDUP(seq_msg_id, env);
}

axis2_char_t * AXIS2_CALL
sandesha2_create_seq_bean_get_seq_id(sandesha2_create_seq_bean_t *create_seq,
					const axis2_env_t *env)
{
	sandesha2_create_seq_bean_impl_t *bean_impl = NULL;
	bean_impl = (sandesha2_create_seq_bean_impl_t*)create_seq;
	return bean_impl->seq_id;
}

void AXIS2_CALL
sandesha2_create_seq_bean_set_seq_id(sandesha2_create_seq_bean_t *create_seq,
					const axis2_env_t *env, axis2_char_t *seq_id)
{
	sandesha2_create_seq_bean_impl_t *bean_impl = NULL;
	bean_impl = (sandesha2_create_seq_bean_impl_t*)create_seq;
	bean_impl->seq_id = AXIS2_STRDUP(seq_id, env);
}

axis2_char_t * AXIS2_CALL
sandesha2_create_seq_bean_get_internal_seq_id( sandesha2_create_seq_bean_t *create_seq,
					const axis2_env_t *env)
{
	sandesha2_create_seq_bean_impl_t *bean_impl = NULL;
	bean_impl = (sandesha2_create_seq_bean_impl_t*)create_seq;
	return bean_impl->internal_seq_id;
}

void AXIS2_CALL
sandesha2_create_seq_bean_set_internal_seq_id( sandesha2_create_seq_bean_t *create_seq,
					const axis2_env_t *env, axis2_char_t *int_seq_id)
{
	sandesha2_create_seq_bean_impl_t *bean_impl = NULL;
	bean_impl = (sandesha2_create_seq_bean_impl_t*)create_seq;
	bean_impl->internal_seq_id = AXIS2_STRDUP(int_seq_id, env);
}

