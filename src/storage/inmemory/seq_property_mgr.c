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
 
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

typedef struct sandesha2_seq_property_mgr_impl sandesha2_seq_property_mgr_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_seq_property_mgr_impl
{
    sandesha2_seq_property_mgr_t seq_property;
	
    axis2_hash_t *table;
    axis2_array_list_t *values;
    axis2_thread_mutex_t *mutex;

};

#define SANDESHA2_INTF_TO_IMPL(seq_property) ((sandesha2_seq_property_mgr_impl_t *) seq_property)

axis2_status_t AXIS2_CALL 
sandesha2_seq_property_mgr_free(
        void *seq_property,
        const axis2_env_t *envv);

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_insert(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_remove(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name);

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_seq_property_mgr_retrieve(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name);

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_update(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_seq_property_mgr_find(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean);

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_seq_property_mgr_find_unique(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_seq_property_mgr_retrieve_all(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_update_or_insert(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean);

static axis2_char_t *
sandesha2_seq_property_mgr_get_id(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean);

AXIS2_EXTERN sandesha2_seq_property_mgr_t * AXIS2_CALL
sandesha2_seq_property_mgr_create(
        const axis2_env_t *env,
        axis2_ctx_t *ctx)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    seq_property_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_seq_property_mgr_impl_t));

    seq_property_impl->table = NULL;
    seq_property_impl->values = NULL;
    seq_property_impl->mutex = NULL;

    seq_property_impl->seq_property.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_seq_property_mgr_ops_t)); 
    
    seq_property_impl->mutex = axis2_thread_mutex_create(env->allocator, 
            AXIS2_THREAD_MUTEX_DEFAULT);
    if(!seq_property_impl->mutex) 
    {
        sandesha2_seq_property_mgr_free(&(seq_property_impl->seq_property), env);
        return NULL;
    }

    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_BEAN_MAP_SEQ_PROPERTY, AXIS2_FALSE);
    if(NULL != property)
        seq_property_impl->table = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!seq_property_impl->table)
    {
        axis2_property_t *property = NULL;

        property = axis2_property_create(env);
        seq_property_impl->table = axis2_hash_make(env);
        if(!property || !seq_property_impl->table)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        AXIS2_PROPERTY_SET_VALUE(property, env, seq_property_impl->table);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, axis2_hash_free_void_arg);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_BEAN_MAP_SEQ_PROPERTY, 
                property, AXIS2_FALSE);
    }
    seq_property_impl->seq_property.ops->free = sandesha2_seq_property_mgr_free;
    seq_property_impl->seq_property.ops->insert = 
        sandesha2_seq_property_mgr_insert;
    seq_property_impl->seq_property.ops->remove = 
        sandesha2_seq_property_mgr_remove;
    seq_property_impl->seq_property.ops->retrieve = 
        sandesha2_seq_property_mgr_retrieve;
    seq_property_impl->seq_property.ops->update = 
        sandesha2_seq_property_mgr_update;
    seq_property_impl->seq_property.ops->find = 
        sandesha2_seq_property_mgr_find;
    seq_property_impl->seq_property.ops->find_unique = 
        sandesha2_seq_property_mgr_find_unique;
    seq_property_impl->seq_property.ops->retrieve_all = 
        sandesha2_seq_property_mgr_retrieve_all;
    seq_property_impl->seq_property.ops->update_or_insert = 
        sandesha2_seq_property_mgr_update_or_insert;

    return &(seq_property_impl->seq_property);
}

axis2_status_t AXIS2_CALL
sandesha2_seq_property_mgr_free(
        void *seq_property,
        const axis2_env_t *env)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    if(seq_property_impl->mutex)
    {
        axis2_thread_mutex_destry(seq_property_impl->mutex);
        seq_property_impl->mutex = NULL;
    }
    if(seq_property_impl->values)
    {
        AXIS2_ARRAY_LIST_FREE(seq_property_impl->values, env);
        seq_property_impl->values = NULL;
    }

    if(seq_property_impl->table)
    {
        axis2_hash_free(seq_property_impl->table, env);
        seq_property_impl->table = NULL;
    }

    if((&(seq_property_impl->seq_property))->ops)
    {
        AXIS2_FREE(env->allocator, (&(seq_property_impl->seq_property))->ops);
        (&(seq_property_impl->seq_property))->ops = NULL;
    }

    if(seq_property_impl)
    {
        AXIS2_FREE(env->allocator, seq_property_impl);
        seq_property_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_insert(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *id = NULL;
    axis2_char_t *name = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);

    seq_id = SANDESHA2_SEQ_PROPERTY_BEAN_GET_SEQ_ID(bean, env);
    if(!seq_id)
    {
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return AXIS2_FALSE;
    }
    name = SANDESHA2_SEQ_PROPERTY_BEAN_GET_NAME(bean, env);
    if(!name)
    {
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return AXIS2_FALSE;
    }
    id = axis2_strcat(env, seq_id, ":", name, NULL);
    axis2_hash_set(seq_property_impl->table, id, AXIS2_HASH_KEY_STRING, bean);
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return AXIS2_TRUE;

}

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_remove(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    sandesha2_seq_property_bean_t *bean = NULL;
    axis2_char_t *key = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    
    bean = sandesha2_seq_property_mgr_retrieve(seq_property, env, 
            seq_id, name);
    key = axis2_strcat(env, seq_id, ":", name, NULL); 
    axis2_hash_set(seq_property_impl->table, key, AXIS2_HASH_KEY_STRING, NULL);
    AXIS2_FREE(env->allocator, key);
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return AXIS2_TRUE;

}

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_seq_property_mgr_retrieve(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    sandesha2_seq_property_bean_t *bean = NULL;
    axis2_char_t *key = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    
    key = axis2_strcat(env, seq_id, ":", name, NULL); 
    bean = (sandesha2_seq_property_bean_t *) axis2_hash_get(seq_property_impl->table, 
            key, AXIS2_HASH_KEY_STRING);
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_update(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_char_t *id = NULL;
    sandesha2_seq_property_bean_t *bean_l = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    id = sandesha2_seq_property_mgr_get_id(seq_property, env, bean);
    if(!id)
    {
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return AXIS2_FALSE;
    }
    bean_l = axis2_hash_get(seq_property_impl->table, id, 
            AXIS2_HASH_KEY_STRING);
    if(!bean_l)
    {
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return AXIS2_FALSE;
    }
    axis2_hash_set(seq_property_impl->table, id, 
            AXIS2_HASH_KEY_STRING, bean);
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return AXIS2_TRUE;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_seq_property_mgr_find(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_array_list_t *beans = NULL;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    beans = axis2_array_list_create(env, 0);
    if(!beans)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return NULL;
    }
    if(!bean)
    {
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return beans;
    }
    for (i = axis2_hash_first (seq_property_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_seq_property_bean_t *temp = NULL;
        void *v = NULL;
        axis2_bool_t equal = AXIS2_TRUE;
        axis2_char_t *seq_id = NULL;
        axis2_char_t *temp_seq_id = NULL;
        axis2_char_t *name = NULL;
        axis2_char_t *temp_name = NULL;
        axis2_char_t *value = NULL;
        axis2_char_t *temp_value = NULL;
        
        axis2_hash_this (i, NULL, NULL, &v);
        temp = (sandesha2_seq_property_bean_t *) v;
        seq_id = SANDESHA2_SEQ_PROPERTY_BEAN_GET_SEQ_ID(bean, env);
        temp_seq_id = SANDESHA2_SEQ_PROPERTY_BEAN_GET_SEQ_ID(temp, env);
        if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
        {
            equal = AXIS2_FALSE;
        }
        name = SANDESHA2_SEQ_PROPERTY_BEAN_GET_NAME(bean, env);
        temp_name = SANDESHA2_SEQ_PROPERTY_BEAN_GET_NAME(temp, env);
        if(name && temp_name && 0 != AXIS2_STRCMP(name, temp_name))
        {
            equal = AXIS2_FALSE;
        }
        value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(bean, env);
        temp_value = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(temp, 
                env);
        if(value && temp_value && 0 != AXIS2_STRCMP(value, temp_value))
        {
            equal = AXIS2_FALSE;
        }
        if(AXIS2_TRUE == equal)
        {
            AXIS2_ARRAY_LIST_ADD(beans, env, temp);
        }
        
    }
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return beans;
}

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_seq_property_mgr_find_unique(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0;
    sandesha2_seq_property_bean_t *ret = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    
    beans = sandesha2_seq_property_mgr_find(seq_property, env, 
            bean);
    if(beans)
        size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    if( size > 1)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Non-Unique result");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NON_UNIQUE_RESULT, 
                AXIS2_FAILURE);
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return NULL;
    }
    for(i = 0; i < size; i++)
    {
       ret = AXIS2_ARRAY_LIST_GET(beans, env, i);
       break;
    }
    
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return ret;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_seq_property_mgr_retrieve_all(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_hash_index_t *i = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    for (i = axis2_hash_first (seq_property_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_seq_property_bean_t *bean = NULL;
        void *v = NULL;
        
        axis2_hash_this (i, NULL, NULL, &v);
        bean = (sandesha2_seq_property_bean_t *) v;
        AXIS2_ARRAY_LIST_ADD(seq_property_impl->values, env, bean);
    }

    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return seq_property_impl->values;
}

axis2_bool_t AXIS2_CALL
sandesha2_seq_property_mgr_update_or_insert(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_char_t *id = NULL;
    sandesha2_seq_property_bean_t *bean_l = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    id = sandesha2_seq_property_mgr_get_id(seq_property, env, bean);
    if(!id)
    {
        axis2_thread_mutex_unlock(seq_property_impl->mutex);
        return AXIS2_FALSE;
    }
    bean_l = axis2_hash_get(seq_property_impl->table, id, 
            AXIS2_HASH_KEY_STRING);
    if(!bean_l)
    {
        axis2_hash_set(seq_property_impl->table, id, 
            AXIS2_HASH_KEY_STRING, bean);
    }
    axis2_hash_set(seq_property_impl->table, id, 
            AXIS2_HASH_KEY_STRING, bean);
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return AXIS2_TRUE;
}

static axis2_char_t *
sandesha2_seq_property_mgr_get_id(
        sandesha2_seq_property_mgr_t *seq_property,
        const axis2_env_t *env,
        sandesha2_seq_property_bean_t *bean)
{
    sandesha2_seq_property_mgr_impl_t *seq_property_impl = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *name = NULL;
    axis2_char_t *id = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_property_impl = SANDESHA2_INTF_TO_IMPL(seq_property);

    axis2_thread_mutex_lock(seq_property_impl->mutex);
    seq_id = SANDESHA2_SEQ_PROPERTY_BEAN_GET_SEQ_ID(bean, env);
    name = SANDESHA2_SEQ_PROPERTY_BEAN_GET_NAME(bean, env);
    id = axis2_strcat(env, seq_id, ":", name, NULL);
    axis2_thread_mutex_unlock(seq_property_impl->mutex);

    return id;
}

