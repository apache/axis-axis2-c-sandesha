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

#ifndef SANDESHA2_SENDER_BEAN_H
#define SANDESHA2_SENDER_BEAN_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_sender_bean sandesha2_sender_bean_t;
typedef struct sandesha2_sender_bean_ops sandesha2_sender_bean_ops_t;

AXIS2_DECLARE_DATA struct sandesha2_sender_bean_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	axis2_char_t *(AXIS2_CALL *
		get_msg_context_ref_key) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_msg_context_ref_key) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env, 
			axis2_char_t *ref_key);

	axis2_char_t *(AXIS2_CALL *
		get_msg_id) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_msg_id) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env, 
			axis2_char_t *msg_id);

	axis2_bool_t (AXIS2_CALL *
		is_send) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_send) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env, 
			axis2_bool_t send);	

	axis2_char_t *(AXIS2_CALL *
		get_internal_seq_id) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_internal_seq_id) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env, 
			axis2_char_t *int_seq_id);

	int (AXIS2_CALL *
		get_sent_count) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_sent_count) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env, 
			int sent_count);

	long (AXIS2_CALL *
		get_msg_no) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_msg_no) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env, 
			long msg_no);

	axis2_bool_t (AXIS2_CALL *
		is_resend) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);	

	void (AXIS2_CALL *
		set_resend) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env,
			axis2_bool_t resend);

	long (AXIS2_CALL *
		get_time_to_send) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_time_to_send) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env,
			long time_to_send);

	int (AXIS2_CALL *
		get_msg_type) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void(AXIS2_CALL *
		set_msg_type) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env,
			int msg_type);

	axis2_char_t *(AXIS2_CALL *
		get_sequence_id)(sandesha2_sender_bean_t *sender,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_sequence_id) (sandesha2_sender_bean_t *sender,
			const axis2_env_t *env,
			axis2_char_t *sequence_id);

};

AXIS2_DECLARE_DATA struct sandesha2_sender_bean{
	sandesha2_sender_bean_ops_t *ops;
};

/* constructors 
 */
AXIS2_EXTERN sandesha2_sender_bean_t* AXIS2_CALL
	sandesha2_sender_bean_create(const axis2_env_t *env);

AXIS2_EXTERN sandesha2_sender_bean_t* AXIS2_CALL
	sandesha2_sender_bean_create_with_data(const axis2_env_t *env,
			axis2_char_t *msg_id,
			axis2_char_t *key,
			axis2_bool_t send,
			long	time_to_send,
			axis2_char_t *int_seq_id,
			long msg_no);


#define SANDESHA2_SENDER_BEAN_FREE(sender_bean, env) \
	((sender_bean)->ops->free(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(sender, env) \
      (((sandesha2_sender_bean_t *) sender)->ops->\
       get_msg_context_ref_key (sender, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_MSG_CONTEXT_REF_KEY(sender_bean, env, ref_key) \
	((sender_bean)->ops->set_msg_context_ref_key(sender_bean, env, ref_key))
	 
#define SANDESHA2_SENDER_BEAN_GET_MSG_ID(sender, env) \
	(((sandesha2_sender_bean_t *)sender)->ops->\
     get_msg_id(sender, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_MSG_ID(sender_bean, env, msg_id) \
	((sender_bean)->ops->set_msg_id(sender_bean, env, msg_id))
	 
#define SANDESHA2_SENDER_BEAN_IS_SEND(sender_bean, env) \
	((sender_bean)->ops->is_send(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_SEND(sender_bean, env, send) \
	((sender_bean)->ops->set_send(sender_bean, env, send))
	 
#define SANDESHA2_SENDER_BEAN_GET_INTERNAL_SEQ_ID(sender, env) \
      (((sandesha2_sender_bean_t *) sender)->ops->\
       get_internal_seq_id (sender, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_INTERNAL_SEQ_ID(sender_bean, env, int_seq_id) \
	((sender_bean)->ops->set_internal_seq_id(sender_bean, env, int_seq_id))
	 
#define SANDESHA2_SENDER_BEAN_GET_SENT_COUNT(sender_bean, env) \
	((sender_bean)->ops->get_sent_count(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_SENT_COUNT(sender_bean, env, sent_count) \
	((sender_bean)->ops->set_sent_count(sender_bean, env, sent_count))
	 
#define SANDESHA2_SENDER_BEAN_GET_MSG_NO(sender_bean, env) \
	((sender_bean)->ops->get_msg_no(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_MSG_NO(sender_bean, env, msg_no) \
	((sender_bean)->ops->set_msg_no(sender_bean, env, msg_no))
	 
#define SANDESHA2_SENDER_BEAN_IS_RESEND(sender_bean, env) \
	((sender_bean)->ops->is_resend(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_RESEND(sender_bean, env, resend) \
	((sender_bean)->ops->set_resend(sender_bean, env, resend))
	 
#define SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(sender_bean, env) \
	((sender_bean)->ops->get_time_to_send(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(sender_bean, env, time_to_send) \
	((sender_bean)->ops->set_time_to_send(sender_bean, env, time_to_send))
	 
#define SANDESHA2_SENDER_BEAN_GET_MSG_TYPE(sender_bean, env) \
	((sender_bean)->ops->get_msg_type(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_MSG_TYPE(sender_bean, env, msg_type) \
	((sender_bean)->ops->set_msg_type(sender_bean, env, msg_type))
	 
#define SANDESHA2_SENDER_BEAN_GET_SEQUENCE_ID(sender_bean, env) \
	((sender_bean)->ops->get_sequence_id(sender_bean, env))
	 
#define SANDESHA2_SENDER_BEAN_SET_SEQUENCE_ID(sender_bean, env, seq_id) \
	((sender_bean)->ops->set_sequence_id(sender_bean, env, seq_id))
	 

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_SENDER_BEAN_H */
