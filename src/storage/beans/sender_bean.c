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

#include <sandesha2_sender_bean.h>
#include <string.h>
#include <axis2_string.h>
#include <axis2_utils.h>


/*sender_bean struct */
typedef struct sandesha2_sender_bean_impl sandesha2_sender_bean_impl_t;

struct sandesha2_sender_bean_impl
{
	sandesha2_sender_bean_t o_bean;

	axis2_char_t *msg_context_ref_key;
	axis2_char_t *msg_id;
	axis2_char_t *internal_seq_id;
	int sent_count;
	long msg_no;
	axis2_bool_t send;
	axis2_bool_t resend;
	long time_to_send;
	int msg_type;
	axis2_char_t *seq_id;

};
	
#define AXIS2_INTF_TO_IMPL(sender) ((sandesha2_sender_bean_impl_t*)sender)

/************* FUNCTION prototypes ***************/
axis2_status_t AXIS2_CALL
	sandesha2_sender_bean_free (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_msg_context_ref_key (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_sender_bean_set_msg_context_ref_key (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *ref_key);

axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_msg_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL 
	sandesha2_sender_bean_set_msg_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *msg_id);

axis2_bool_t AXIS2_CALL
	sandesha2_sender_bean_is_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL 
	sandesha2_sender_bean_set_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_bool_t send);

axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_internal_seq_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL 
	sandesha2_sender_bean_set_internal_seq_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *int_seq_id);

int AXIS2_CALL 
	sandesha2_sender_bean_get_sent_count (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL 
	sandesha2_sender_bean_set_sent_count (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				int sent_count);

long AXIS2_CALL
	sandesha2_sender_bean_get_msg_no (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_sender_bean_set_msg_no (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				long msg_no);

axis2_bool_t AXIS2_CALL
	sandesha2_sender_bean_is_resend (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_sender_bean_set_resend (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_bool_t resend);

long AXIS2_CALL
	sandesha2_sender_bean_get_time_to_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_sender_bean_set_time_to_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				long time_to_send);
int AXIS2_CALL
       sandesha2_sender_bean_get_msg_type (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL
	 sandesha2_sender_bean_set_msg_type (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				int msg_type);

axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_seq_id(sandesha2_sender_bean_t *sender,
				const axis2_env_t *env);

void AXIS2_CALL
	sandesha2_sender_bean_set_seq_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *seq_id);


AXIS2_EXTERN sandesha2_sender_bean_t* AXIS2_CALL
sandesha2_sender_bean_create(const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean_impl = (sandesha2_sender_bean_impl_t*) AXIS2_MALLOC(env->allocator,
				sizeof(sandesha2_sender_bean_impl_t));

	if(!bean_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init properties */
	bean_impl->msg_context_ref_key = NULL;
	bean_impl->msg_id = NULL;
	bean_impl->internal_seq_id = NULL;
	bean_impl->sent_count = -1;
	bean_impl->msg_no = -1;
	bean_impl->send = AXIS2_FALSE;
	bean_impl->resend = AXIS2_TRUE;
	bean_impl->time_to_send = -1;
	bean_impl->msg_type = 0;
	bean_impl->seq_id = NULL;

	bean_impl->o_bean.ops = NULL;
	bean_impl->o_bean.ops = (sandesha2_sender_bean_ops_t*) AXIS2_MALLOC(env->allocator,
								sizeof(sandesha2_sender_bean_ops_t));

	if(!bean_impl->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean_impl);

		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	bean_impl->o_bean.ops->free = sandesha2_sender_bean_free;
	bean_impl->o_bean.ops->get_msg_context_ref_key = sandesha2_sender_bean_get_msg_context_ref_key;
	bean_impl->o_bean.ops->set_msg_context_ref_key = sandesha2_sender_bean_set_msg_context_ref_key;
	bean_impl->o_bean.ops->get_msg_id = sandesha2_sender_bean_get_msg_id;
	bean_impl->o_bean.ops->set_msg_id = sandesha2_sender_bean_set_msg_id;
	bean_impl->o_bean.ops->is_send =  sandesha2_sender_bean_is_send;
	bean_impl->o_bean.ops->set_send = sandesha2_sender_bean_set_send;
	bean_impl->o_bean.ops->get_internal_seq_id = sandesha2_sender_bean_get_internal_seq_id;
	bean_impl->o_bean.ops->set_internal_seq_id = sandesha2_sender_bean_set_internal_seq_id;
	bean_impl->o_bean.ops->get_sent_count = sandesha2_sender_bean_get_sent_count;
	bean_impl->o_bean.ops->set_sent_count = sandesha2_sender_bean_set_sent_count;
	bean_impl->o_bean.ops->get_msg_no = sandesha2_sender_bean_get_msg_no;
	bean_impl->o_bean.ops->set_msg_no = sandesha2_sender_bean_set_msg_no;
	bean_impl->o_bean.ops->is_resend = sandesha2_sender_bean_is_resend;
	bean_impl->o_bean.ops->set_resend = sandesha2_sender_bean_set_resend;
	bean_impl->o_bean.ops->get_time_to_send = sandesha2_sender_bean_get_time_to_send;
	bean_impl->o_bean.ops->set_time_to_send = sandesha2_sender_bean_set_time_to_send;
	bean_impl->o_bean.ops->get_msg_type = sandesha2_sender_bean_get_msg_type;
	bean_impl->o_bean.ops->set_msg_type = sandesha2_sender_bean_set_msg_type;
	bean_impl->o_bean.ops->get_seq_id = sandesha2_sender_bean_get_seq_id;
	bean_impl->o_bean.ops->set_seq_id = sandesha2_sender_bean_set_seq_id;

	return &(bean_impl->o_bean);
}


AXIS2_EXTERN sandesha2_sender_bean_t* AXIS2_CALL
sandesha2_sender_bean_create_with_data(const axis2_env_t *env,
			       axis2_char_t *msg_id,
			       axis2_char_t *key,
			       axis2_bool_t send,
			       long    time_to_send,
			       axis2_char_t *int_seq_id,
			       long msg_no)

{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean_impl = (sandesha2_sender_bean_impl_t*) AXIS2_MALLOC(env->allocator,
				sizeof(sandesha2_sender_bean_impl_t));

	if(!bean_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init properties */
	bean_impl->msg_context_ref_key = (axis2_char_t *)AXIS2_STRDUP(key, env);
	bean_impl->msg_id = (axis2_char_t *)AXIS2_STRDUP(msg_id, env);
	bean_impl->internal_seq_id = (axis2_char_t *)AXIS2_STRDUP(int_seq_id, env);
	bean_impl->sent_count = -1;
	bean_impl->msg_no = msg_no;
	bean_impl->send = send; 
	bean_impl->resend = AXIS2_TRUE;
	bean_impl->time_to_send = time_to_send;
	bean_impl->msg_type = 0;
	bean_impl->seq_id = NULL;

	bean_impl->o_bean.ops = NULL;
	bean_impl->o_bean.ops = (sandesha2_sender_bean_ops_t*) AXIS2_MALLOC(env->allocator,
								sizeof(sandesha2_sender_bean_ops_t));

	if(!bean_impl->o_bean.ops)
	{
		AXIS2_FREE(env->allocator, bean_impl);

		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	bean_impl->o_bean.ops->free = sandesha2_sender_bean_free;
	bean_impl->o_bean.ops->get_msg_context_ref_key = sandesha2_sender_bean_get_msg_context_ref_key;
	bean_impl->o_bean.ops->set_msg_context_ref_key = sandesha2_sender_bean_set_msg_context_ref_key;
	bean_impl->o_bean.ops->get_msg_id = sandesha2_sender_bean_get_msg_id;
	bean_impl->o_bean.ops->set_msg_id = sandesha2_sender_bean_set_msg_id;
	bean_impl->o_bean.ops->is_send =  sandesha2_sender_bean_is_send;
	bean_impl->o_bean.ops->set_send = sandesha2_sender_bean_set_send;
	bean_impl->o_bean.ops->get_internal_seq_id = sandesha2_sender_bean_get_internal_seq_id;
	bean_impl->o_bean.ops->set_internal_seq_id = sandesha2_sender_bean_set_internal_seq_id;
	bean_impl->o_bean.ops->get_sent_count = sandesha2_sender_bean_get_sent_count;
	bean_impl->o_bean.ops->set_sent_count = sandesha2_sender_bean_set_sent_count;
	bean_impl->o_bean.ops->get_msg_no = sandesha2_sender_bean_get_msg_no;
	bean_impl->o_bean.ops->set_msg_no = sandesha2_sender_bean_set_msg_no;
	bean_impl->o_bean.ops->is_resend = sandesha2_sender_bean_is_resend;
	bean_impl->o_bean.ops->set_resend = sandesha2_sender_bean_set_resend;
	bean_impl->o_bean.ops->get_time_to_send = sandesha2_sender_bean_get_time_to_send;
	bean_impl->o_bean.ops->set_time_to_send = sandesha2_sender_bean_set_time_to_send;
	bean_impl->o_bean.ops->get_msg_type = sandesha2_sender_bean_get_msg_type;
	bean_impl->o_bean.ops->set_msg_type = sandesha2_sender_bean_set_msg_type;
	bean_impl->o_bean.ops->get_seq_id = sandesha2_sender_bean_get_seq_id;
	bean_impl->o_bean.ops->set_seq_id = sandesha2_sender_bean_set_seq_id;

	return &(bean_impl->o_bean);
}

axis2_status_t AXIS2_CALL
sandesha2_sender_bean_free (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env)
{

	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	if(bean_impl->msg_context_ref_key)
	{
		AXIS2_FREE(env->allocator, bean_impl->msg_context_ref_key);
		bean_impl->msg_context_ref_key = NULL;
	}

	if(bean_impl->msg_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->msg_id);
		bean_impl->msg_id = NULL;
	}

	if(bean_impl->internal_seq_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->internal_seq_id);
		bean_impl->internal_seq_id = NULL;
	}

	if(bean_impl->seq_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->seq_id);
		bean_impl->seq_id = NULL;
	}
    return AXIS2_SUCCESS;

}

axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_msg_context_ref_key (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->msg_context_ref_key;
}


void AXIS2_CALL
	sandesha2_sender_bean_set_msg_context_ref_key (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *ref_key)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	if(bean_impl->msg_context_ref_key)
	{
		AXIS2_FREE(env->allocator, bean_impl->msg_context_ref_key);
		bean_impl->msg_context_ref_key = NULL;
	}

	bean_impl->msg_context_ref_key = (axis2_char_t *)AXIS2_STRDUP(ref_key, env);

}

axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_msg_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->msg_id;
}

void AXIS2_CALL 
	sandesha2_sender_bean_set_msg_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *msg_id)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	if(bean_impl->msg_id)
	{
		AXIS2_FREE(env->allocator, msg_id);
		bean_impl->msg_id = NULL;
	}

	bean_impl->msg_id = (axis2_char_t *)AXIS2_STRDUP(msg_id, env);
}


axis2_bool_t AXIS2_CALL
	sandesha2_sender_bean_is_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->send;
}



void AXIS2_CALL 
	sandesha2_sender_bean_set_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_bool_t send)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	bean_impl->send = send;
}


axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_internal_seq_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->internal_seq_id;
}

void AXIS2_CALL 
	sandesha2_sender_bean_set_internal_seq_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *int_seq_id)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	if(bean_impl->internal_seq_id)
	{
		AXIS2_FREE(env->allocator, int_seq_id);
		bean_impl->internal_seq_id = NULL;
	}

	bean_impl->internal_seq_id = (axis2_char_t *)AXIS2_STRDUP(int_seq_id, env);
}

int AXIS2_CALL 
	sandesha2_sender_bean_get_sent_count (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->sent_count;
}


void AXIS2_CALL 
	sandesha2_sender_bean_set_sent_count (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				int sent_count)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	bean_impl->sent_count = sent_count;
}


long AXIS2_CALL
	sandesha2_sender_bean_get_msg_no (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->msg_no;
}

void AXIS2_CALL
	sandesha2_sender_bean_set_msg_no (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				long msg_no)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	bean_impl->msg_no = msg_no;
}


axis2_bool_t AXIS2_CALL
	sandesha2_sender_bean_is_resend (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->resend;
}

void AXIS2_CALL
	sandesha2_sender_bean_set_resend (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_bool_t resend)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	bean_impl->resend = resend;
}

long AXIS2_CALL
	sandesha2_sender_bean_get_time_to_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->time_to_send;
}


void AXIS2_CALL
	sandesha2_sender_bean_set_time_to_send (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				long time_to_send)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	bean_impl->time_to_send = time_to_send;
}

int AXIS2_CALL
       sandesha2_sender_bean_get_msg_type (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->msg_type;
}

void AXIS2_CALL
	sandesha2_sender_bean_set_msg_type (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				int msg_type)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);
	
	bean_impl->msg_type = msg_type;
}

axis2_char_t* AXIS2_CALL
	sandesha2_sender_bean_get_seq_id(sandesha2_sender_bean_t *sender,
				const axis2_env_t *env)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	return bean_impl->seq_id;
}



void AXIS2_CALL
	sandesha2_sender_bean_set_seq_id (sandesha2_sender_bean_t *sender,
				const axis2_env_t *env,
				axis2_char_t *seq_id)
{
	sandesha2_sender_bean_impl_t *bean_impl = NULL;
	bean_impl = AXIS2_INTF_TO_IMPL(sender);

	if(bean_impl->seq_id)
	{
		AXIS2_FREE(env->allocator, bean_impl->seq_id);
		bean_impl->seq_id = NULL;
	}

	bean_impl->seq_id = (axis2_char_t *)AXIS2_STRDUP(seq_id, env);
}

