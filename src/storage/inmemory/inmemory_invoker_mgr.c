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
 
#include <sandesha2_inmemory_invoker_mgr.h>
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

/** 
 * @brief Sandesha2 Inmemory Invoker Manager Struct Impl
 *   Sandesha2 Inmemory Invoker Manager 
 */ 
typedef struct sandesha2_inmemory_invoker_mgr
{
    sandesha2_invoker_mgr_t invoker_mgr;
    axis2_hash_t *table;
    axis2_thread_mutex_t *mutex;
}sandesha2_inmemory_invoker_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(invoker_mgr) \
    ((sandesha2_inmemory_invoker_mgr_t *) invoker_mgr)

axis2_status_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_free(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_insert(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_remove(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_retrieve(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_update(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_find(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean);

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_find_unique(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean);

static const sandesha2_invoker_mgr_ops_t invoker_mgr_ops = 
{
    sandesha2_inmemory_invoker_mgr_free,
    sandesha2_inmemory_invoker_mgr_insert,
    sandesha2_inmemory_invoker_mgr_remove,
    sandesha2_inmemory_invoker_mgr_retrieve,
    sandesha2_inmemory_invoker_mgr_update,
    sandesha2_inmemory_invoker_mgr_find,
    sandesha2_inmemory_invoker_mgr_find_unique
};

AXIS2_EXTERN sandesha2_invoker_mgr_t * AXIS2_CALL
sandesha2_inmemory_invoker_mgr_create(
    const axis2_env_t *env,
    axis2_ctx_t *ctx)
{
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    invoker_mgr_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_inmemory_invoker_mgr_t));

    invoker_mgr_impl->table = NULL;
    invoker_mgr_impl->mutex = NULL;

    invoker_mgr_impl->mutex = axis2_thread_mutex_create(env->allocator, 
            AXIS2_THREAD_MUTEX_DEFAULT);
    if(!invoker_mgr_impl->mutex) 
    {
        sandesha2_inmemory_invoker_mgr_free(&(invoker_mgr_impl->invoker_mgr), 
            env);
        return NULL;
    }

    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_BEAN_MAP_STORAGE_MAP, AXIS2_FALSE);
    if(property)
        invoker_mgr_impl->table = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(
            property, env);
    if(!invoker_mgr_impl->table)
    {
        axis2_property_t *property = NULL;

        property = axis2_property_create(env);
        invoker_mgr_impl->table = axis2_hash_make(env);
        if(!property || !invoker_mgr_impl->table)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, invoker_mgr_impl->table);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, axis2_hash_free_void_arg);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_BEAN_MAP_STORAGE_MAP, 
                property, AXIS2_FALSE);
    }
    invoker_mgr_impl->invoker_mgr.ops = &invoker_mgr_ops;

    return &(invoker_mgr_impl->invoker_mgr);
}

axis2_status_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_free(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env)
{
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);

    if(invoker_mgr_impl->mutex)
    {
        axis2_thread_mutex_destroy(invoker_mgr_impl->mutex);
        invoker_mgr_impl->mutex = NULL;
    }
    if(invoker_mgr_impl->table)
    {
        axis2_hash_free(invoker_mgr_impl->table, env);
        invoker_mgr_impl->table = NULL;
    }

    if(invoker_mgr_impl)
    {
        AXIS2_FREE(env->allocator, invoker_mgr_impl);
        invoker_mgr_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_insert(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean)
{
    axis2_char_t *ref_key = NULL;
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);

    ref_key = sandesha2_invoker_bean_get_msg_ctx_ref_key(bean, env);
    axis2_hash_set(invoker_mgr_impl->table, ref_key, AXIS2_HASH_KEY_STRING, 
        bean);
    return AXIS2_TRUE;

}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_remove(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, key, AXIS2_FALSE);
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);

    axis2_hash_set(invoker_mgr_impl->table, key, AXIS2_HASH_KEY_STRING, NULL);

    return AXIS2_TRUE;

}

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_retrieve(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    sandesha2_invoker_bean_t *bean = NULL;
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, key, AXIS2_FALSE);

    bean = (sandesha2_invoker_bean_t *) axis2_hash_get(invoker_mgr_impl->table, 
            key, AXIS2_HASH_KEY_STRING);

    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_invoker_mgr_update(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean)
{
    axis2_char_t *ref_key = NULL;
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);

    ref_key = sandesha2_invoker_bean_get_msg_ctx_ref_key(bean, env);
    if(!ref_key)
    {
        return AXIS2_FALSE;
    }
    axis2_hash_set(invoker_mgr_impl->table, ref_key, AXIS2_HASH_KEY_STRING, bean);

    return AXIS2_TRUE;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_find(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean)
{
    axis2_array_list_t *beans = NULL;
    axis2_hash_index_t *i = NULL;
    sandesha2_invoker_bean_t *temp = NULL;
    void *v = NULL;
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);

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
    for (i = axis2_hash_first (invoker_mgr_impl->table, env); i; 
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
        ref_key = sandesha2_invoker_bean_get_msg_ctx_ref_key(bean, env);
        temp_ref_key = sandesha2_invoker_bean_get_msg_ctx_ref_key(temp, env);
        if(ref_key && temp_ref_key && 0 != AXIS2_STRCMP(ref_key, temp_ref_key))
        {
            select = AXIS2_FALSE;
        }
        seq_id = sandesha2_invoker_bean_get_seq_id(bean, env);
        temp_seq_id = sandesha2_invoker_bean_get_seq_id(temp, env);
        if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
        {
            select = AXIS2_FALSE;
        }
        msg_no = sandesha2_invoker_bean_get_msg_no(bean, env);
        temp_msg_no = sandesha2_invoker_bean_get_msg_no(temp, env);
        if(msg_no != 0 && (msg_no != temp_msg_no))
        {
            select = AXIS2_FALSE;
        }
        is_invoked = sandesha2_invoker_bean_is_invoked(bean, env);
        temp_is_invoked = sandesha2_invoker_bean_is_invoked(temp, env);
        if(is_invoked != temp_is_invoked)
        {
            select = AXIS2_FALSE;
        }
        if(AXIS2_TRUE == select)
        {
            AXIS2_ARRAY_LIST_ADD(beans, env, temp);
        }
        
    }

    return beans;
}

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_find_unique(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean)
{
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0;
    sandesha2_invoker_bean_t *ret = NULL;
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);
    
    beans = sandesha2_inmemory_invoker_mgr_find(invoker_mgr, env, bean);
    if(beans)
        size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    if( size > 1)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Non-Unique result");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NON_UNIQUE_RESULT, AXIS2_FAILURE);
        return NULL;
    }
    for(i = 0; i < size; i++)
    {
       ret = AXIS2_ARRAY_LIST_GET(beans, env, i);
       break;
    }
    

    return ret;
}

