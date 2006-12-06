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

#include <sandesha2_invoker_bean.h>
#include <sandesha2_rm_bean.h>
#include <string.h>
#include <axis2_string.h>
#include <axis2_utils.h>

struct sandesha2_invoker_bean_t
{
    sandesha2_rm_bean_t *rm_bean;
	/*  This is the messageContextRefKey that is obtained after saving a message context in a storage. */
	axis2_char_t *msg_context_ref_key;

	/* The message number of the message. */
	long msg_no;

	/*  The seq ID of the seq the message belong to. */
	axis2_char_t *seq_id;

	/* Weather the message has been invoked by the invoker.*/
	axis2_bool_t invoked;

};

AXIS2_EXTERN sandesha2_invoker_bean_t* AXIS2_CALL
sandesha2_invoker_bean_create(
    const axis2_env_t *env )
{
	sandesha2_invoker_bean_t *bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_invoker_bean_t *) AXIS2_MALLOC( env->allocator, 
				sizeof(sandesha2_invoker_bean_t) );

	if (!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	bean->msg_context_ref_key = NULL;
	bean->msg_no = -1;
	bean->seq_id = NULL;	
	bean->invoked = AXIS2_FALSE;
    bean->rm_bean = NULL;

	return bean;
}

AXIS2_EXTERN sandesha2_invoker_bean_t* AXIS2_CALL
sandesha2_invoker_bean_create_with_data(
    const axis2_env_t *env,
    axis2_char_t *ref_key,
    long msg_no,
    axis2_char_t *seq_id,
    axis2_bool_t invoked)
{
	sandesha2_invoker_bean_t *bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_invoker_bean_t *) AXIS2_MALLOC( env->allocator, 
				sizeof(sandesha2_invoker_bean_t) );

	if (!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	if(!ref_key)
		bean->msg_context_ref_key = NULL;
	else
		bean->msg_context_ref_key = (axis2_char_t*)AXIS2_STRDUP(ref_key, env);

	if(!seq_id)
		bean->seq_id = NULL;	
	else
		bean->seq_id = (axis2_char_t*)AXIS2_STRDUP(seq_id, env);

	bean->msg_no = msg_no;
	bean->invoked = invoked;
    bean->rm_bean = NULL;

	return bean;
}

axis2_status_t AXIS2_CALL
sandesha2_invoker_bean_free(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env)
{
	if(invoker->rm_bean)
	{
		sandesha2_rm_bean_free(invoker->rm_bean, env);
		invoker->rm_bean= NULL;
	}
	if(invoker->msg_context_ref_key)
	{
		AXIS2_FREE(env->allocator, invoker->msg_context_ref_key);
		invoker->msg_context_ref_key= NULL;
	}
		
	if(!invoker->seq_id)
	{
		AXIS2_FREE(env->allocator, invoker->seq_id);
		invoker->seq_id= NULL;
	}
    return AXIS2_SUCCESS;
}

sandesha2_rm_bean_t * AXIS2_CALL
sandesha2_invoker_bean_get_base( 
    sandesha2_invoker_bean_t* invoker,
    const axis2_env_t *env)
{
	return invoker->rm_bean;

}	

void AXIS2_CALL
sandesha2_invoker_bean_set_base (
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env, 
    sandesha2_rm_bean_t* rm_bean)
{
	invoker->rm_bean = rm_bean;
}

axis2_char_t* AXIS2_CALL 
sandesha2_invoker_bean_get_msg_ctx_ref_key(
        sandesha2_invoker_bean_t *invoker,
		const axis2_env_t *env)
{
	return invoker->msg_context_ref_key;
}

void AXIS2_CALL 
sandesha2_invoker_bean_set_msg_context_ref_key(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env, axis2_char_t* context_ref_id)
{
	if(invoker->msg_context_ref_key)
		AXIS2_FREE(env->allocator, invoker->msg_context_ref_key);

	invoker->msg_context_ref_key = 
        (axis2_char_t*)AXIS2_STRDUP(context_ref_id, env);
}
	

long AXIS2_CALL 
sandesha2_invoker_bean_get_msg_no(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env)
{
	return invoker->msg_no;
}
	
void AXIS2_CALL
sandesha2_invoker_bean_set_msg_no(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env, long msgno)
{
	invoker->msg_no = msgno;
}

axis2_char_t* AXIS2_CALL
sandesha2_invoker_bean_get_seq_id(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env)
{
	return invoker->seq_id;
}

void AXIS2_CALL
sandesha2_invoker_bean_set_seq_id(
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env, axis2_char_t* int_seq_id)
{
	invoker->seq_id = (axis2_char_t*)AXIS2_STRDUP(int_seq_id, env);

}



axis2_bool_t AXIS2_CALL
sandesha2_invoker_bean_is_invoked (
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env)
{
	return invoker->invoked;
}

void AXIS2_CALL 
sandesha2_invoker_bean_set_invoked( 
    sandesha2_invoker_bean_t *invoker,
    const axis2_env_t *env,
    axis2_bool_t invoked)
{
	invoker->invoked = invoked;
}

