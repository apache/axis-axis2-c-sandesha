/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <sandesha2_sender_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

typedef struct sandesha2_sender_mgr_impl sandesha2_sender_mgr_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_sender_mgr_impl
{
    sandesha2_sender_mgr_t sender;
	
    axis2_hash_t *table;
    axis2_thread_mutex_t *mutex;

};

#define SANDESHA2_INTF_TO_IMPL(sender) ((sandesha2_sender_mgr_impl_t *) sender)

axis2_status_t AXIS2_CALL 
sandesha2_sender_mgr_free(
        void *sender,
        const axis2_env_t *envv);

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_insert(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_remove(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        const axis2_char_t *msg_id);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_retrieve(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        const axis2_char_t *msg_id);

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_update(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_sender_mgr_find_by_internal_seq_id(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        axis2_char_t *internal_seq_id);

axis2_array_list_t *AXIS2_CALL
sandesha2_sender_mgr_find_by_sender_bean(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_find_unique(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_get_next_msg_to_send(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_retrieve_from_msg_ref_key(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        axis2_char_t *msg_ctx_ref_key);

static axis2_status_t
sandesha2_sender_mgr_update_next_sending_time(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean);

/*
static axis2_array_list_t *
sandesha2_sender_mgr_find_beans_with_msg_no(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        axis2_array_list_t *list,
        long msg_no);
*/

AXIS2_EXTERN sandesha2_sender_mgr_t * AXIS2_CALL
sandesha2_sender_mgr_create(
        const axis2_env_t *env,
        axis2_ctx_t *ctx)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    sender_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_sender_mgr_impl_t));

    sender_impl->table = NULL;
    sender_impl->mutex = NULL;

    sender_impl->sender.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_sender_mgr_ops_t)); 
    
    sender_impl->mutex = axis2_thread_mutex_create(env->allocator, 
            AXIS2_THREAD_MUTEX_DEFAULT);
    if(!sender_impl->mutex) 
    {
        sandesha2_sender_mgr_free(&(sender_impl->sender), env);
        return NULL;
    }

    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_BEAN_MAP_RETRANSMITTER, AXIS2_FALSE);
    if(NULL != property)
        sender_impl->table = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!sender_impl->table)
    {
        axis2_property_t *property = NULL;

        property = axis2_property_create(env);
        sender_impl->table = axis2_hash_make(env);
        if(!property || !sender_impl->table)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, sender_impl->table);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, axis2_hash_free_void_arg);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_BEAN_MAP_RETRANSMITTER, 
                property, AXIS2_FALSE);
    }
    sender_impl->sender.ops->free = sandesha2_sender_mgr_free;
    sender_impl->sender.ops->insert = 
        sandesha2_sender_mgr_insert;
    sender_impl->sender.ops->remove = 
        sandesha2_sender_mgr_remove;
    sender_impl->sender.ops->retrieve = 
        sandesha2_sender_mgr_retrieve;
    sender_impl->sender.ops->update = 
        sandesha2_sender_mgr_update;
    sender_impl->sender.ops->find_by_internal_seq_id = 
        sandesha2_sender_mgr_find_by_internal_seq_id;
    sender_impl->sender.ops->find_by_sender_bean = 
        sandesha2_sender_mgr_find_by_sender_bean;
    sender_impl->sender.ops->find_unique = 
        sandesha2_sender_mgr_find_unique;
    sender_impl->sender.ops->get_next_msg_to_send = 
        sandesha2_sender_mgr_get_next_msg_to_send;
    sender_impl->sender.ops->retrieve_from_msg_ref_key = 
        sandesha2_sender_mgr_retrieve_from_msg_ref_key;

    return &(sender_impl->sender);
}

axis2_status_t AXIS2_CALL
sandesha2_sender_mgr_free(
        void *sender,
        const axis2_env_t *env)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    if(sender_impl->mutex)
    {
        axis2_thread_mutex_destroy(sender_impl->mutex);
        sender_impl->mutex = NULL;
    }
    if(sender_impl->table)
    {
        axis2_hash_free(sender_impl->table, env);
        sender_impl->table = NULL;
    }

    if((&(sender_impl->sender))->ops)
    {
        AXIS2_FREE(env->allocator, (&(sender_impl->sender))->ops);
        (&(sender_impl->sender))->ops = NULL;
    }

    if(sender_impl)
    {
        AXIS2_FREE(env->allocator, sender_impl);
        sender_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_insert(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_char_t *msg_id = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);


    msg_id = SANDESHA2_SENDER_BEAN_GET_MSG_ID(bean, env);
    if(!msg_id)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_KEY_IS_NULL, AXIS2_FAILURE);
        return AXIS2_FALSE;
    }
    axis2_hash_set(sender_impl->table, msg_id, AXIS2_HASH_KEY_STRING, bean);

    return AXIS2_TRUE;

}

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_remove(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        const axis2_char_t *msg_id)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    axis2_hash_set(sender_impl->table, msg_id, AXIS2_HASH_KEY_STRING, NULL);

    return AXIS2_TRUE;

}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_retrieve(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        const axis2_char_t *msg_id)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    sandesha2_sender_bean_t *bean = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);


    bean = (sandesha2_sender_bean_t *) axis2_hash_get(sender_impl->table, 
            msg_id, AXIS2_HASH_KEY_STRING);

    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_update(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_char_t *msg_id = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    msg_id = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(bean, env);
    if(!msg_id)
    {
        return AXIS2_FALSE;
    }

    return AXIS2_TRUE; /* No need to update. Being a reference does the job */
}

axis2_array_list_t *AXIS2_CALL
sandesha2_sender_mgr_find_by_internal_seq_id(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        axis2_char_t *internal_seq_id)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_array_list_t *list = NULL;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    
    list = axis2_array_list_create(env, 0);
    if(!list)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    if(!internal_seq_id || 0 == AXIS2_STRCMP(internal_seq_id, ""))
        return list;
    for (i = axis2_hash_first (sender_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_sender_bean_t *sender_bean = NULL;
        void *v = NULL;
        axis2_char_t *temp_internal_seq_id = NULL;
        
        axis2_hash_this (i, NULL, NULL, &v);
        sender_bean = (sandesha2_sender_bean_t *) v;
        temp_internal_seq_id = SANDESHA2_SENDER_BEAN_GET_INTERNAL_SEQ_ID(
                sender_bean, env);
        if(internal_seq_id && temp_internal_seq_id && 0 == AXIS2_STRCMP(
                    internal_seq_id, temp_internal_seq_id))
        {
            AXIS2_ARRAY_LIST_ADD(list, env, sender_bean);
        }
        
    }

    return list;
}


axis2_array_list_t *AXIS2_CALL
sandesha2_sender_mgr_find_by_sender_bean(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_array_list_t *beans = NULL;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    beans = axis2_array_list_create(env, 0);
    if(!beans)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    if(!bean)
    {
        return beans;
    }
    for (i = axis2_hash_first (sender_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_sender_bean_t *temp = NULL;
        void *v = NULL;
        axis2_bool_t add = AXIS2_TRUE;
        axis2_char_t *ref_key = NULL;
        axis2_char_t *temp_ref_key = NULL;
        long time_to_send = 0;
        long temp_time_to_send = 0;
        axis2_char_t *msg_id = NULL;
        axis2_char_t *temp_msg_id = NULL;
        axis2_char_t *internal_seq_id = NULL;
        axis2_char_t *temp_internal_seq_id = NULL;
        long msg_no = 0;
        long temp_msg_no = 0;
        int msg_type = 0;
        int temp_msg_type = 0;
        axis2_bool_t is_send = AXIS2_FALSE;
        axis2_bool_t temp_is_send = AXIS2_FALSE;
        axis2_bool_t is_resend = AXIS2_FALSE;
        axis2_bool_t temp_is_resend = AXIS2_FALSE;
        
        axis2_hash_this (i, NULL, NULL, &v);
        temp = (sandesha2_sender_bean_t *) v;
        ref_key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(bean, env);
        temp_ref_key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(temp, env);
        if(ref_key && temp_ref_key && 0 != AXIS2_STRCMP(ref_key, temp_ref_key))
        {
            add = AXIS2_FALSE;
        }
        time_to_send = SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(bean, env);
        temp_time_to_send = SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(temp, env);
        if(time_to_send > 0 && (time_to_send != temp_time_to_send))
        {
            add = AXIS2_FALSE;
        }
        msg_id = SANDESHA2_SENDER_BEAN_GET_MSG_ID(bean, env);
        temp_msg_id = SANDESHA2_SENDER_BEAN_GET_MSG_ID(temp, env);
        if(msg_id && temp_msg_id && 0 != AXIS2_STRCMP(msg_id, temp_msg_id))
        {
            add = AXIS2_FALSE;
        }
        internal_seq_id = SANDESHA2_SENDER_BEAN_GET_INTERNAL_SEQ_ID(bean, env);
        temp_internal_seq_id = SANDESHA2_SENDER_BEAN_GET_INTERNAL_SEQ_ID(temp, 
                env);
        if(internal_seq_id && temp_internal_seq_id && 0 != AXIS2_STRCMP(
                    internal_seq_id, temp_internal_seq_id))
        {
            add = AXIS2_FALSE;
        }
        msg_no = SANDESHA2_SENDER_BEAN_GET_MSG_NO(bean, env);
        temp_msg_no = SANDESHA2_SENDER_BEAN_GET_MSG_NO(temp, env);
        if(msg_no > 0 && (msg_no != temp_msg_no))
        {
            add = AXIS2_FALSE;
        }
        msg_type = SANDESHA2_SENDER_BEAN_GET_MSG_TYPE(bean, env);
        temp_msg_type = SANDESHA2_SENDER_BEAN_GET_MSG_TYPE(temp, env);
        if(msg_type != SANDESHA2_MSG_TYPE_UNKNOWN  && (msg_type != temp_msg_type))
        {
            add = AXIS2_FALSE;
        }
        is_send = SANDESHA2_SENDER_BEAN_IS_SEND(bean, env);
        temp_is_send = SANDESHA2_SENDER_BEAN_IS_SEND(temp, env);
        if(is_send != temp_is_send)
        {
            add = AXIS2_FALSE;
        }
        is_resend = SANDESHA2_SENDER_BEAN_IS_RESEND(bean, env);
        temp_is_resend = SANDESHA2_SENDER_BEAN_IS_RESEND(temp, env);
        if(is_resend != temp_is_resend)
        {
            add = AXIS2_FALSE;
        }
        if(AXIS2_TRUE == add)
        {
            AXIS2_ARRAY_LIST_ADD(beans, env, temp);
        }
        
    }

    return beans;
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_find_unique(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0;
    sandesha2_sender_bean_t *ret = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    
    beans = sandesha2_sender_mgr_find_by_sender_bean(sender, env, 
            bean);
    if(beans)
        size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    if( size > 1)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Non-Unique result");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NON_UNIQUE_RESULT, 
                AXIS2_FAILURE);
        return NULL;
    }
    for(i = 0; i < size; i++)
    {
       ret = AXIS2_ARRAY_LIST_GET(beans, env, i);
       break;
    }
    

    return ret;
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_get_next_msg_to_send(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    long lowest_app_msg_no = 0;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    
    for (i = axis2_hash_first (sender_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_sender_bean_t *temp = NULL;
        void *v = NULL;
        axis2_bool_t is_send = AXIS2_FALSE;
        
        axis2_hash_this (i, NULL, NULL, &v);
        temp = (sandesha2_sender_bean_t *) v;
        is_send = SANDESHA2_SENDER_BEAN_IS_SEND(temp, env);
        if(AXIS2_TRUE == is_send)
        {
            long time_to_send = 0;
            long time_now = 0;

            time_to_send = SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(temp, env);
            time_now = sandesha2_utils_get_current_time_in_millis(env);
            if(time_now >= time_to_send)
            {
                int msg_type = SANDESHA2_SENDER_BEAN_GET_MSG_TYPE(temp, env);
                if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
                {
                    long msg_no = SANDESHA2_SENDER_BEAN_GET_MSG_NO(temp, env);
                    if(lowest_app_msg_no == 0 || msg_no < lowest_app_msg_no)
                        lowest_app_msg_no = msg_no;
                }
            }
        }
    }
    
    for (i = axis2_hash_first (sender_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_sender_bean_t *temp = NULL;
        void *v = NULL;
        axis2_char_t *msg_id = NULL;
        axis2_bool_t is_send = AXIS2_FALSE;
        
        axis2_hash_this (i, NULL, NULL, &v);
        temp = (sandesha2_sender_bean_t *) v;
        /* test code */
        msg_id = SANDESHA2_SENDER_BEAN_GET_MSG_ID(temp, env);
        /* end of test code */
        is_send = SANDESHA2_SENDER_BEAN_IS_SEND(temp, env);
        if(AXIS2_TRUE == is_send)
        {
            long time_to_send = 0;
            long time_now = 0;

            time_to_send = SANDESHA2_SENDER_BEAN_GET_TIME_TO_SEND(temp, env);
            time_now = sandesha2_utils_get_current_time_in_millis(env);
            if(time_now >= time_to_send)
            {
                axis2_bool_t valid = AXIS2_FALSE;
                int msg_type = SANDESHA2_SENDER_BEAN_GET_MSG_TYPE(temp, env);
                
                if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
                {
                    long msg_no = SANDESHA2_SENDER_BEAN_GET_MSG_NO(temp, env);
                    
                    if(msg_no == lowest_app_msg_no)
                        valid = AXIS2_TRUE;
                }
                else
                    valid = AXIS2_TRUE;
                if(AXIS2_TRUE == valid)
                {
                    sandesha2_sender_mgr_update_next_sending_time(
                            sender, env, temp);
                    return temp;
                }
            }
        }
    }
    return NULL;
}

static axis2_status_t
sandesha2_sender_mgr_update_next_sending_time(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        sandesha2_sender_bean_t *bean)
{
    return AXIS2_SUCCESS;
}

/*
static axis2_array_list_t *
sandesha2_sender_mgr_find_beans_with_msg_no(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        axis2_array_list_t *list,
        long msg_no)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_array_list_t *beans = NULL;
    int i = 0, size =0;

    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, list, NULL);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    
    beans = axis2_array_list_create(env, 0);
    if(!beans)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    size = AXIS2_ARRAY_LIST_SIZE(list, env); 
    for(i = 0; i < size; i++)
    {
        long msg_no_l = 0;
        sandesha2_sender_bean_t *bean = NULL;
        
        bean = (sandesha2_sender_bean_t *) AXIS2_ARRAY_LIST_GET(list, 
                env, i);
        msg_no_l = SANDESHA2_SENDER_BEAN_GET_MSG_NO(bean, env);
        if(msg_no_l == msg_no)
        {
            AXIS2_ARRAY_LIST_ADD(beans, env, bean);
        }
        
    }

    return beans;
}
*/

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_retrieve_from_msg_ref_key(
        sandesha2_sender_mgr_t *sender,
        const axis2_env_t *env,
        axis2_char_t *msg_ctx_ref_key)
{
    sandesha2_sender_mgr_impl_t *sender_impl = NULL;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, msg_ctx_ref_key, NULL);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);

    
    for (i = axis2_hash_first (sender_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_sender_bean_t *bean = NULL;
        axis2_char_t *msg_ctx_ref_key_l = NULL;
        void *v = NULL;
        
        axis2_hash_this (i, NULL, NULL, &v);
        bean = (sandesha2_sender_bean_t *) v;
        msg_ctx_ref_key_l = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(bean, env);
        if(msg_ctx_ref_key_l && 0 == AXIS2_STRCMP(msg_ctx_ref_key_l, 
                    msg_ctx_ref_key))
        {
            return bean;
        }
    }    

    return NULL;
}




