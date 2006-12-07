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
#include <sandesha2_inmemory_seq_property_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

/** 
 * @brief Sandesha Inmemory Sequence Property Manager Struct Impl
 *   Sandesha Inmemory Sequence Property Manager 
 */ 
typedef struct sandesha2_inmemory_seq_property_mgr
{
    sandesha2_seq_property_mgr_t seq_prop_mgr;
    axis2_hash_t *table;
    axis2_array_list_t *values;
    axis2_thread_mutex_t *mutex;
} sandesha2_inmemory_seq_property_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(seq_property_mgr) \
    ((sandesha2_inmemory_seq_property_mgr_t *) seq_property_mgr)

static axis2_char_t *
sandesha2_inmemory_seq_property_mgr_get_id(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

axis2_status_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_free(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_insert(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_remove(
        sandesha2_seq_property_mgr_t *seq_prop_mgr,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name);

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_retrieve(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axis2_char_t *name);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_update(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_find(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_find_unique(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_retrieve_all(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_update_or_insert(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

static axis2_char_t *
sandesha2_inmemory_seq_property_mgr_get_id(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

static const sandesha2_seq_property_mgr_ops_t seq_property_mgr_ops = 
{
    sandesha2_inmemory_seq_property_mgr_free,
    sandesha2_inmemory_seq_property_mgr_insert,
    sandesha2_inmemory_seq_property_mgr_remove,
    sandesha2_inmemory_seq_property_mgr_retrieve,
    sandesha2_inmemory_seq_property_mgr_update,
    sandesha2_inmemory_seq_property_mgr_find,
    sandesha2_inmemory_seq_property_mgr_find_unique,
    sandesha2_inmemory_seq_property_mgr_retrieve_all,
    sandesha2_inmemory_seq_property_mgr_update_or_insert
};

AXIS2_EXTERN sandesha2_seq_property_mgr_t * AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_create(
    const axis2_env_t *env,
    axis2_ctx_t *ctx)
{
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    seq_prop_mgr_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_inmemory_seq_property_mgr_t));

    seq_prop_mgr_impl->table = NULL;
    seq_prop_mgr_impl->values = NULL;
    seq_prop_mgr_impl->mutex = NULL;

    seq_prop_mgr_impl->mutex = axis2_thread_mutex_create(env->allocator, 
            AXIS2_THREAD_MUTEX_DEFAULT);
    if(!seq_prop_mgr_impl->mutex) 
    {
        sandesha2_inmemory_seq_property_mgr_free(&(seq_prop_mgr_impl->
            seq_prop_mgr), env);
        return NULL;
    }
    
    seq_prop_mgr_impl->values = axis2_array_list_create(env, 0);
    if(!seq_prop_mgr_impl->values)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }

    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_BEAN_MAP_SEQ_PROPERTY, AXIS2_FALSE);
    if(NULL != property)
        seq_prop_mgr_impl->table = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(
            property, env);
    if(!seq_prop_mgr_impl->table)
    {
        axis2_property_t *property = NULL;

        property = axis2_property_create(env);
        seq_prop_mgr_impl->table = axis2_hash_make(env);
        if(!property || !seq_prop_mgr_impl->table)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, seq_prop_mgr_impl->table);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, axis2_hash_free_void_arg);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_BEAN_MAP_SEQ_PROPERTY, 
                property, AXIS2_FALSE);
    }
    seq_prop_mgr_impl->seq_prop_mgr.ops = &seq_property_mgr_ops;
    return &(seq_prop_mgr_impl->seq_prop_mgr);
}

axis2_status_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_free(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env)
{
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    if(seq_prop_mgr_impl->mutex)
    {
        axis2_thread_mutex_destroy(seq_prop_mgr_impl->mutex);
        seq_prop_mgr_impl->mutex = NULL;
    }
    if(seq_prop_mgr_impl->values)
    {
        AXIS2_ARRAY_LIST_FREE(seq_prop_mgr_impl->values, env);
        seq_prop_mgr_impl->values = NULL;
    }

    if(seq_prop_mgr_impl->table)
    {
        axis2_hash_free(seq_prop_mgr_impl->table, env);
        seq_prop_mgr_impl->table = NULL;
    }

    if(seq_prop_mgr_impl)
    {
        AXIS2_FREE(env->allocator, seq_prop_mgr_impl);
        seq_prop_mgr_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_insert(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *seq_id = NULL;
    axis2_char_t *id = NULL;
    axis2_char_t *name = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    seq_id = sandesha2_seq_property_bean_get_seq_id(bean, env);
    if(!seq_id)
    {
        return AXIS2_FALSE;
    }
    name = sandesha2_seq_property_bean_get_name(bean, env);
    if(!name)
    {
        return AXIS2_FALSE;
    }
    id = axis2_strcat(env, seq_id, ":", name, NULL);
    axis2_hash_set(seq_prop_mgr_impl->table, id, AXIS2_HASH_KEY_STRING, bean);

    return AXIS2_TRUE;

}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_remove(
        sandesha2_seq_property_mgr_t *seq_prop_mgr,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name)
{
    sandesha2_seq_property_bean_t *bean = NULL;
    axis2_char_t *key = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    
    bean = sandesha2_inmemory_seq_property_mgr_retrieve(seq_prop_mgr, env, 
            seq_id, name);
    key = axis2_strcat(env, seq_id, ":", name, NULL); 
    axis2_hash_set(seq_prop_mgr_impl->table, key, AXIS2_HASH_KEY_STRING, NULL);
    AXIS2_FREE(env->allocator, key);

    return AXIS2_TRUE;

}

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_retrieve(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axis2_char_t *name)
{
    sandesha2_seq_property_bean_t *bean = NULL;
    axis2_char_t *key = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    
    key = axis2_strcat(env, seq_id, ":", name, NULL); 
    bean = (sandesha2_seq_property_bean_t *) axis2_hash_get(seq_prop_mgr_impl->table, 
            key, AXIS2_HASH_KEY_STRING);

    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_update(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *id = NULL;
    sandesha2_seq_property_bean_t *bean_l = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    id = sandesha2_inmemory_seq_property_mgr_get_id(seq_prop_mgr, env, bean);
    if(!id)
    {
        return AXIS2_FALSE;
    }
    bean_l = axis2_hash_get(seq_prop_mgr_impl->table, id, 
            AXIS2_HASH_KEY_STRING);
    if(!bean_l)
    {
        return AXIS2_FALSE;
    }
    axis2_hash_set(seq_prop_mgr_impl->table, id, 
            AXIS2_HASH_KEY_STRING, bean);

    return AXIS2_TRUE;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_find(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_array_list_t *beans = NULL;
    axis2_hash_index_t *i = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

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
    for (i = axis2_hash_first (seq_prop_mgr_impl->table, env); i; 
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
        seq_id = sandesha2_seq_property_bean_get_seq_id(bean, env);
        temp_seq_id = sandesha2_seq_property_bean_get_seq_id(temp, env);
        if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
        {
            equal = AXIS2_FALSE;
        }
        name = sandesha2_seq_property_bean_get_name(bean, env);
        temp_name = sandesha2_seq_property_bean_get_name(temp, env);
        if(name && temp_name && 0 != AXIS2_STRCMP(name, temp_name))
        {
            equal = AXIS2_FALSE;
        }
        value = sandesha2_seq_property_bean_get_value(bean, env);
        temp_value = sandesha2_seq_property_bean_get_value(temp, 
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

    return beans;
}

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_find_unique(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0;
    sandesha2_seq_property_bean_t *ret = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    
    beans = sandesha2_inmemory_seq_property_mgr_find(seq_prop_mgr, env, 
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

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_retrieve_all(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env)
{
    axis2_hash_index_t *i = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    for (i = axis2_hash_first (seq_prop_mgr_impl->table, env); i; 
            i = axis2_hash_next (env, i))
    {
        sandesha2_seq_property_bean_t *bean = NULL;
        void *v = NULL;
        
        axis2_hash_this (i, NULL, NULL, &v);
        bean = (sandesha2_seq_property_bean_t *) v;
        AXIS2_ARRAY_LIST_ADD(seq_prop_mgr_impl->values, env, bean);
    }

    return seq_prop_mgr_impl->values;
}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_seq_property_mgr_update_or_insert(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *id = NULL;
    sandesha2_seq_property_bean_t *bean_l = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    id = sandesha2_inmemory_seq_property_mgr_get_id(seq_prop_mgr, env, bean);
    if(!id)
    {
        return AXIS2_FALSE;
    }
    bean_l = axis2_hash_get(seq_prop_mgr_impl->table, id, 
            AXIS2_HASH_KEY_STRING);
    if(!bean_l)
    {
        axis2_hash_set(seq_prop_mgr_impl->table, id, 
            AXIS2_HASH_KEY_STRING, bean);
    }
    axis2_hash_set(seq_prop_mgr_impl->table, id, 
            AXIS2_HASH_KEY_STRING, bean);

    return AXIS2_TRUE;
}

static axis2_char_t *
sandesha2_inmemory_seq_property_mgr_get_id(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *seq_id = NULL;
    axis2_char_t *name = NULL;
    axis2_char_t *id = NULL;
    sandesha2_inmemory_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    seq_id = sandesha2_seq_property_bean_get_seq_id(bean, env);
    name = sandesha2_seq_property_bean_get_name(bean, env);
    id = axis2_strcat(env, seq_id, ":", name, NULL);

    return id;
}

