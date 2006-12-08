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
#include <sandesha2_inmemory_bean_mgr.h>
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_storage_mgr.h>
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
    sandesha2_inmemory_bean_mgr_t *bean_mgr;
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

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_invoker_mgr_match(
    sandesha2_inmemory_bean_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

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
    sandesha2_storage_mgr_t *storage_mgr,
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
    invoker_mgr_impl->bean_mgr = sandesha2_inmemory_bean_mgr_create(env,
        storage_mgr, ctx, SANDESHA2_BEAN_MAP_CREATE_SEQUECE);
    invoker_mgr_impl->bean_mgr->ops.match = sandesha2_inmemory_create_invoker_mgr_match;
    invoker_mgr_impl->invoker_mgr.ops = invoker_mgr_ops;

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
    if(invoker_mgr_impl->bean_mgr)
    {
        sandesha2_inmemory_bean_mgr_free(invoker_mgr_impl->bean_mgr, env);
        invoker_mgr_impl->bean_mgr = NULL;
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
    return sandesha2_inmemory_bean_mgr_insert(invoker_mgr_impl->bean_mgr, env,
        ref_key, (sandesha2_rm_bean_t *) bean);
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
    return sandesha2_inmemory_bean_mgr_remove(invoker_mgr_impl->bean_mgr, env,
        key);
}

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_retrieve(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, key, AXIS2_FALSE);
    return (sandesha2_invoker_bean_t *) sandesha2_inmemory_bean_mgr_retrieve(
        invoker_mgr_impl->bean_mgr, env, key);
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
    return sandesha2_inmemory_bean_mgr_update(invoker_mgr_impl->bean_mgr, env,
        ref_key, (sandesha2_rm_bean_t *) bean);
}

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_find(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean)
{
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);
    return sandesha2_inmemory_bean_mgr_find(invoker_mgr_impl->bean_mgr, env,
        (sandesha2_rm_bean_t *) bean);
}

sandesha2_invoker_bean_t *AXIS2_CALL
sandesha2_inmemory_invoker_mgr_find_unique(
    sandesha2_invoker_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_invoker_bean_t *bean)
{
    sandesha2_inmemory_invoker_mgr_t *invoker_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    invoker_mgr_impl = SANDESHA2_INTF_TO_IMPL(invoker_mgr);
    return (sandesha2_invoker_bean_t *)
        sandesha2_inmemory_bean_mgr_find_unique(invoker_mgr_impl->bean_mgr, env,
        (sandesha2_rm_bean_t *) bean); 
}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_invoker_mgr_match(
    sandesha2_inmemory_bean_mgr_t *invoker_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate)
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
    
    ref_key = sandesha2_invoker_bean_get_msg_ctx_ref_key(
        (sandesha2_invoker_bean_t *) bean, env);
    temp_ref_key = sandesha2_invoker_bean_get_msg_ctx_ref_key(
        (sandesha2_invoker_bean_t *) candidate, env);
    if(ref_key && temp_ref_key && 0 != AXIS2_STRCMP(ref_key, temp_ref_key))
    {
        select = AXIS2_FALSE;
    }
    seq_id = sandesha2_invoker_bean_get_seq_id(
        (sandesha2_invoker_bean_t *) bean, env);
    temp_seq_id = sandesha2_invoker_bean_get_seq_id(
        (sandesha2_invoker_bean_t *) candidate, env);
    if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
    {
        select = AXIS2_FALSE;
    }
    msg_no = sandesha2_invoker_bean_get_msg_no(
        (sandesha2_invoker_bean_t *) bean, env);
    temp_msg_no = sandesha2_invoker_bean_get_msg_no(
        (sandesha2_invoker_bean_t *) candidate, env);
    if(msg_no != 0 && (msg_no != temp_msg_no))
    {
        select = AXIS2_FALSE;
    }
    is_invoked = sandesha2_invoker_bean_is_invoked(
        (sandesha2_invoker_bean_t *) bean, env);
    temp_is_invoked = sandesha2_invoker_bean_is_invoked(
        (sandesha2_invoker_bean_t *) candidate, env);
    if(is_invoked != temp_is_invoked)
    {
        select = AXIS2_FALSE;
    }
    return select;
}

