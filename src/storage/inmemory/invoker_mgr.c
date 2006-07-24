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
 
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

typedef struct sandesha2_invoker_mgr_impl sandesha2_invoker_mgr_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_invoker_mgr_impl
{
    sandesha2_invoker_mgr_t invoker;
	
    axis2_hash_t *table;
    axis2_thread_mutex_t *mutex;

};

#define SANDESHA2_INTF_TO_IMPL(invoker) ((sandesha2_invoker_mgr_impl_t *) invoker)

axis2_status_t AXIS2_CALL 
sandesha2_invoker_mgr_free(
        void *invoker,
        const axis2_env_t *envv);

axis2_bool_t AXIS2_CALL
sandesha2_invoker_mgr_insert(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_invoker_mgr_remove(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        axis2_char_t *ref_key);

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_invoker_mgr_retrieve(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        axis2_char_t *ref_key);

axis2_bool_t AXIS2_CALL
sandesha2_invoker_mgr_update(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_invoker_mgr_find(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean);

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_invoker_mgr_find_unique(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean);

AXIS2_EXTERN sandesha2_invoker_mgr_t * AXIS2_CALL
sandesha2_invoker_mgr_create(
        const axis2_env_t *env,
        axis2_ctx_t *ctx)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    invoker_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_invoker_mgr_impl_t));

    invoker_impl->table = NULL;
    invoker_impl->mutex = NULL;

    invoker_impl->invoker.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_invoker_mgr_ops_t)); 
    
    invoker_impl->mutex = axis2_thread_mutex_create(env->allocator, 
            AXIS2_THREAD_MUTEX_DEFAULT);
    if(!invoker_impl->mutex) 
    {
        sandesha2_invoker_mgr_free(&(invoker_impl->invoker), env);
        return NULL;
    }

    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_BEAN_MAP_STORAGE_MAP, AXIS2_FALSE);
    if(NULL != property)
        invoker_impl->table = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!invoker_impl->table)
    {
        axis2_property_t *property = NULL;

        property = axis2_property_create(env);
        invoker_impl->table = axis2_hash_make(env);
        if(!property || !invoker_impl->table)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        AXIS2_PROPERTY_SET_VALUE(property, env, invoker_impl->table);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, invoker_impl->invoker.ops->free);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_BEAN_MAP_STORAGE_MAP, 
                property, AXIS2_FALSE);
    }
    invoker_impl->invoker.ops->free = sandesha2_invoker_mgr_free;
    invoker_impl->invoker.ops->insert = 
        sandesha2_invoker_mgr_insert;
    invoker_impl->invoker.ops->remove = 
        sandesha2_invoker_mgr_remove;
    invoker_impl->invoker.ops->retrieve = 
        sandesha2_invoker_mgr_retrieve;
    invoker_impl->invoker.ops->update = 
        sandesha2_invoker_mgr_update;
    invoker_impl->invoker.ops->find = 
        sandesha2_invoker_mgr_find;
    invoker_impl->invoker.ops->find_unique = 
        sandesha2_invoker_mgr_find_unique;

    return &(invoker_impl->invoker);
}

axis2_status_t AXIS2_CALL
sandesha2_invoker_mgr_free(
        void *invoker,
        const axis2_env_t *env)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    invoker_impl = SANDESHA2_INTF_TO_IMPL(invoker);

    if(invoker_impl->mutex)
    {
        axis2_thread_mutex_destry(invoker_impl->mutex);
        invoker_impl->mutex = NULL;
    }
    if(invoker_impl->table)
    {
        axis2_hash_free(invoker_impl->table, env);
        invoker_impl->table = NULL;
    }

    if((&(invoker_impl->invoker))->ops)
    {
        AXIS2_FREE(env->allocator, (&(invoker_impl->invoker))->ops);
        (&(invoker_impl->invoker))->ops = NULL;
    }

    if(invoker_impl)
    {
        AXIS2_FREE(env->allocator, invoker_impl);
        invoker_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_invoker_mgr_insert(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;
    axis2_char_t *ref_key = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    invoker_impl = SANDESHA2_INTF_TO_IMPL(invoker);

    axis2_thread_mutex_lock(invoker_impl->mutex);
    ref_key = SANDESHA2_INVOKER_BEAN_GET_MSG_CONTEXT_REF_KEY(bean, env);
    axis2_hash_set(invoker_impl->table, ref_key, AXIS2_HASH_KEY_STRING, bean);
    axis2_thread_mutex_unlock(invoker_impl->mutex);

    return AXIS2_TRUE;

}

axis2_bool_t AXIS2_CALL
sandesha2_invoker_mgr_remove(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        axis2_char_t *key)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, key, AXIS2_FALSE);
    invoker_impl = SANDESHA2_INTF_TO_IMPL(invoker);

    axis2_thread_mutex_lock(invoker_impl->mutex);
    axis2_hash_set(invoker_impl->table, key, AXIS2_HASH_KEY_STRING, NULL);
    axis2_thread_mutex_unlock(invoker_impl->mutex);

    return AXIS2_TRUE;

}

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_invoker_mgr_retrieve(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        axis2_char_t *key)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;
    sandesha2_invoker_bean_t *bean = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, key, AXIS2_FALSE);
    invoker_impl = SANDESHA2_INTF_TO_IMPL(invoker);

    axis2_thread_mutex_lock(invoker_impl->mutex);
    bean = (sandesha2_invoker_bean_t *) axis2_hash_get(invoker_impl->table, 
            key, AXIS2_HASH_KEY_STRING);
    axis2_thread_mutex_unlock(invoker_impl->mutex);

    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_invoker_mgr_update(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;
    axis2_char_t *ref_key = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    invoker_impl = SANDESHA2_INTF_TO_IMPL(invoker);

    axis2_thread_mutex_lock(invoker_impl->mutex);
    ref_key = SANDESHA2_INVOKER_BEAN_GET_MSG_CONTEXT_REF_KEY(bean, env);
    if(!ref_key)
    {
        axis2_thread_mutex_unlock(invoker_impl->mutex);
        return AXIS2_FALSE;
    }
    axis2_hash_set(invoker_impl->table, ref_key, AXIS2_HASH_KEY_STRING, bean);
    axis2_thread_mutex_unlock(invoker_impl->mutex);

    return AXIS2_TRUE;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_invoker_mgr_find(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;
    axis2_array_list_t *beans = NULL;
    axis2_hash_index_t *i = NULL;
    sandesha2_invoker_bean_t *temp = NULL;
    void *v = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    invoker_impl = SANDESHA2_INTF_TO_IMPL(invoker);

    axis2_thread_mutex_lock(invoker_impl->mutex);
    beans = axis2_array_list_create(env, 0);
    if(!beans)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        axis2_thread_mutex_unlock(invoker_impl->mutex);
        return NULL;
    }
    if(!bean)
    {
        axis2_thread_mutex_unlock(invoker_impl->mutex);
        return beans;
    }
    for (i = axis2_hash_first (invoker_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        axis2_bool_t select = AXIS2_TRUE;
        axis2_char_t *ref_key = NULL;
        axis2_char_t *temp_ref_key = NULL;
        axis2_char_t *seq_id = NULL;
        axis2_char_t *temp_seq_id = NULL;
        long msg_no = 0;
        long temp_msg_no = 0;
        axis2_bool_t is_invoked = AXIS2_FALSE;
        axis2_bool_t temp_is_invoked = AXIS2_FALSE;
        
        axis2_hash_this (i, NULL, NULL, &v);
        temp = (sandesha2_invoker_bean_t *) v;
        ref_key = SANDESHA2_INVOKER_BEAN_GET_MSG_CONTEXT_REF_KEY(bean, env);
        temp_ref_key = SANDESHA2_INVOKER_BEAN_GET_MSG_CONTEXT_REF_KEY(temp, env);
        if(ref_key && temp_ref_key && 0 != AXIS2_STRCMP(ref_key, temp_ref_key))
        {
            select = AXIS2_FALSE;
        }
        seq_id = SANDESHA2_INVOKER_BEAN_GET_SEQ_ID(bean, env);
        temp_seq_id = SANDESHA2_INVOKER_BEAN_GET_SEQ_ID(temp, env);
        if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
        {
            select = AXIS2_FALSE;
        }
        msg_no = SANDESHA2_INVOKER_BEAN_GET_MSG_NO(bean, env);
        temp_msg_no = SANDESHA2_INVOKER_BEAN_GET_MSG_NO(temp, env);
        if(msg_no != 0 && (msg_no != temp_msg_no))
        {
            select = AXIS2_FALSE;
        }
        is_invoked = SANDESHA2_INVOKER_BEAN_IS_INVOKED(bean, env);
        temp_is_invoked = SANDESHA2_INVOKER_BEAN_IS_INVOKED(temp, env);
        if(is_invoked != temp_is_invoked)
        {
            select = AXIS2_FALSE;
        }
        if(AXIS2_TRUE == select)
        {
            AXIS2_ARRAY_LIST_ADD(beans, env, temp);
        }
        
    }
    axis2_thread_mutex_unlock(invoker_impl->mutex);

    return beans;
}

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_invoker_mgr_find_unique(
        sandesha2_invoker_mgr_t *invoker,
        const axis2_env_t *env,
        sandesha2_invoker_bean_t *bean)
{
    sandesha2_invoker_mgr_impl_t *invoker_impl = NULL;
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0;
    sandesha2_invoker_bean_t *ret = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    invoker_impl = SANDESHA2_INTF_TO_IMPL(invoker);

    axis2_thread_mutex_lock(invoker_impl->mutex);
    
    beans = sandesha2_invoker_mgr_find(invoker, env, bean);
    if(beans)
        size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    if( size > 1)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Non-Unique result");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NON_UNIQUE_RESULT, AXIS2_FAILURE);
        axis2_thread_mutex_unlock(invoker_impl->mutex);
        return NULL;
    }
    for(i = 0; i < size; i++)
    {
       ret = AXIS2_ARRAY_LIST_GET(beans, env, i);
       break;
    }
    
    axis2_thread_mutex_unlock(invoker_impl->mutex);

    return ret;
}

