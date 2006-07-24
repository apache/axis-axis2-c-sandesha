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
 
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

typedef struct sandesha2_next_msg_mgr_impl sandesha2_next_msg_mgr_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_next_msg_mgr_impl
{
    sandesha2_next_msg_mgr_t next_msg;
	
    axis2_hash_t *table;
    axis2_array_list_t *values;
    axis2_thread_mutex_t *mutex;

};

#define SANDESHA2_INTF_TO_IMPL(next_msg) ((sandesha2_next_msg_mgr_impl_t *) next_msg)

axis2_status_t AXIS2_CALL 
sandesha2_next_msg_mgr_free(
        void *next_msg,
        const axis2_env_t *envv);

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_insert(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_remove(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        axis2_char_t *seq_id);

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_next_msg_mgr_retrieve(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        axis2_char_t *seq_id);

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_update(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_next_msg_mgr_find(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean);

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_next_msg_mgr_find_unique(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_next_msg_mgr_retrieve_all(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env);

AXIS2_EXTERN sandesha2_next_msg_mgr_t * AXIS2_CALL
sandesha2_next_msg_mgr_create(
        const axis2_env_t *env,
        axis2_ctx_t *ctx)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    next_msg_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_next_msg_mgr_impl_t));

    next_msg_impl->table = NULL;
    next_msg_impl->values = NULL;
    next_msg_impl->mutex = NULL;

    next_msg_impl->next_msg.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_next_msg_mgr_ops_t)); 
    
    next_msg_impl->mutex = axis2_thread_mutex_create(env->allocator, 
            AXIS2_THREAD_MUTEX_DEFAULT);
    if(!next_msg_impl->mutex) 
    {
        sandesha2_next_msg_mgr_free(&(next_msg_impl->next_msg), env);
        return NULL;
    }

    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_BEAN_MAP_NEXT_MESSAGE, AXIS2_FALSE);
    if(NULL != property)
        next_msg_impl->table = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!next_msg_impl->table)
    {
        axis2_property_t *property = NULL;

        property = axis2_property_create(env);
        next_msg_impl->table = axis2_hash_make(env);
        if(!property || !next_msg_impl->table)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        AXIS2_PROPERTY_SET_VALUE(property, env, next_msg_impl->table);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, next_msg_impl->next_msg.ops->free);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_BEAN_MAP_NEXT_MESSAGE, 
                property, AXIS2_FALSE);
    }
    next_msg_impl->next_msg.ops->free = sandesha2_next_msg_mgr_free;
    next_msg_impl->next_msg.ops->insert = 
        sandesha2_next_msg_mgr_insert;
    next_msg_impl->next_msg.ops->remove = 
        sandesha2_next_msg_mgr_remove;
    next_msg_impl->next_msg.ops->retrieve = 
        sandesha2_next_msg_mgr_retrieve;
    next_msg_impl->next_msg.ops->update = 
        sandesha2_next_msg_mgr_update;
    next_msg_impl->next_msg.ops->find = 
        sandesha2_next_msg_mgr_find;
    next_msg_impl->next_msg.ops->find_unique = 
        sandesha2_next_msg_mgr_find_unique;
    next_msg_impl->next_msg.ops->retrieve_all = 
        sandesha2_next_msg_mgr_retrieve_all;

    return &(next_msg_impl->next_msg);
}

axis2_status_t AXIS2_CALL
sandesha2_next_msg_mgr_free(
        void *next_msg,
        const axis2_env_t *env)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    if(next_msg_impl->mutex)
    {
        axis2_thread_mutex_destry(next_msg_impl->mutex);
        next_msg_impl->mutex = NULL;
    }
    if(next_msg_impl->table)
    {
        axis2_hash_free(next_msg_impl->table, env);
        next_msg_impl->table = NULL;
    }
    if(next_msg_impl->values)
    {
        AXIS2_ARRAY_LIST_FREE(next_msg_impl->values, env);
        next_msg_impl->values = NULL;
    }


    if((&(next_msg_impl->next_msg))->ops)
    {
        AXIS2_FREE(env->allocator, (&(next_msg_impl->next_msg))->ops);
        (&(next_msg_impl->next_msg))->ops = NULL;
    }

    if(next_msg_impl)
    {
        AXIS2_FREE(env->allocator, next_msg_impl);
        next_msg_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_insert(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;
    axis2_char_t *seq_id = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    axis2_thread_mutex_lock(next_msg_impl->mutex);

    seq_id = SANDESHA2_NEXT_MSG_BEAN_GET_SEQ_ID(bean, env);
    if(!seq_id)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_KEY_IS_NULL, AXIS2_FAILURE);
        axis2_thread_mutex_unlock(next_msg_impl->mutex);
        return AXIS2_FALSE;
    }
    axis2_hash_set(next_msg_impl->table, seq_id, AXIS2_HASH_KEY_STRING, bean);
    axis2_thread_mutex_unlock(next_msg_impl->mutex);

    return AXIS2_TRUE;

}

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_remove(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        axis2_char_t *seq_id)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    axis2_thread_mutex_lock(next_msg_impl->mutex);
    axis2_hash_set(next_msg_impl->table, seq_id, AXIS2_HASH_KEY_STRING, NULL);
    axis2_thread_mutex_unlock(next_msg_impl->mutex);

    return AXIS2_TRUE;

}

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_next_msg_mgr_retrieve(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        axis2_char_t *seq_id)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;
    sandesha2_next_msg_bean_t *bean = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    axis2_thread_mutex_lock(next_msg_impl->mutex);
    bean = (sandesha2_next_msg_bean_t *) axis2_hash_get(next_msg_impl->table, 
            seq_id, AXIS2_HASH_KEY_STRING);
    axis2_thread_mutex_unlock(next_msg_impl->mutex);

    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_update(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;
    axis2_char_t *seq_id = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    axis2_thread_mutex_lock(next_msg_impl->mutex);
    seq_id = SANDESHA2_NEXT_MSG_BEAN_GET_SEQ_ID(bean, env);
    if(!seq_id)
    {
        axis2_thread_mutex_unlock(next_msg_impl->mutex);
        return AXIS2_FALSE;
    }
    axis2_hash_set(next_msg_impl->table, seq_id, AXIS2_HASH_KEY_STRING, bean);
    axis2_thread_mutex_unlock(next_msg_impl->mutex);
    return AXIS2_TRUE;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_next_msg_mgr_find(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;
    axis2_array_list_t *beans = NULL;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    axis2_thread_mutex_lock(next_msg_impl->mutex);
    beans = axis2_array_list_create(env, 0);
    if(!beans)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        axis2_thread_mutex_unlock(next_msg_impl->mutex);
        return NULL;
    }
    if(!bean)
        return beans;
    for (i = axis2_hash_first (next_msg_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_next_msg_bean_t *temp = NULL;
        void *v = NULL;
        axis2_bool_t equal = AXIS2_TRUE;
        long next_msg_no = 0;
        long temp_next_msg_no = 0;
        axis2_char_t *seq_id = NULL;
        axis2_char_t *temp_seq_id = NULL;
        
        axis2_hash_this (i, NULL, NULL, &v);
        temp = (sandesha2_next_msg_bean_t *) v;
        next_msg_no = SANDESHA2_NEXT_MSG_BEAN_GET_NEXT_MSG_NO_TO_PROCESS(bean, env);
        temp_next_msg_no = SANDESHA2_NEXT_MSG_BEAN_GET_NEXT_MSG_NO_TO_PROCESS(temp, env);
        if(next_msg_no > 0 && (next_msg_no != temp_next_msg_no))
        {
            equal = AXIS2_FALSE;
        }
        seq_id = SANDESHA2_NEXT_MSG_BEAN_GET_SEQ_ID(bean, env);
        temp_seq_id = SANDESHA2_NEXT_MSG_BEAN_GET_SEQ_ID(temp, env);
        if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
        {
            equal = AXIS2_FALSE;
        }
        if(AXIS2_TRUE == equal)
        {
            AXIS2_ARRAY_LIST_ADD(beans, env, temp);
        }
        
    }
    axis2_thread_mutex_unlock(next_msg_impl->mutex);

    return beans;
}

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_next_msg_mgr_find_unique(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env,
        sandesha2_next_msg_bean_t *bean)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0;
    sandesha2_next_msg_bean_t *ret = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    axis2_thread_mutex_lock(next_msg_impl->mutex);
    
    beans = sandesha2_next_msg_mgr_find(next_msg, env, 
            bean);
    if(beans)
        size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    if( size > 1)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Non-Unique result");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NON_UNIQUE_RESULT, 
                AXIS2_FAILURE);
        axis2_thread_mutex_unlock(next_msg_impl->mutex);
        return NULL;
    }
    for(i = 0; i < size; i++)
    {
       ret = AXIS2_ARRAY_LIST_GET(beans, env, i);
       break;
    }
    
    axis2_thread_mutex_unlock(next_msg_impl->mutex);

    return ret;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_next_msg_mgr_retrieve_all(
        sandesha2_next_msg_mgr_t *next_msg,
        const axis2_env_t *env)
{
    sandesha2_next_msg_mgr_impl_t *next_msg_impl = NULL;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    next_msg_impl = SANDESHA2_INTF_TO_IMPL(next_msg);

    axis2_thread_mutex_lock(next_msg_impl->mutex);
    for (i = axis2_hash_first (next_msg_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_next_msg_bean_t *bean = NULL;
        void *v = NULL;
        
        axis2_hash_this (i, NULL, NULL, &v);
        bean = (sandesha2_next_msg_bean_t *) v;
        AXIS2_ARRAY_LIST_ADD(next_msg_impl->values, env, bean);
    }

    axis2_thread_mutex_unlock(next_msg_impl->mutex);

    return next_msg_impl->values;
}

