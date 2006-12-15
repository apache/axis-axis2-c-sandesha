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
 
#include <sandesha2_permanent_bean_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_permanent_bean_mgr.h>
#include <sandesha2_error.h>
#include <sandesha2_storage_mgr.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <sqlite3.h>

#define DB_NAME "sandesha2"
/** 
 * @brief Sandesha2 Permanent Bean Manager Struct Impl
 *   Sandesha2 Permanent Bean Manager
 */ 
typedef struct sandesha2_permanent_bean_mgr_impl
{
    sandesha2_permanent_bean_mgr_t bean_mgr;
    sandesha2_storage_mgr_t *storage_mgr;
    axis2_thread_mutex_t *mutex;
    sqlite3 *db;

}sandesha2_permanent_bean_mgr_impl_t;

#define SANDESHA2_INTF_TO_IMPL(bean_mgr) \
    ((sandesha2_permanent_bean_mgr_impl_t *) bean_mgr)

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_match(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate)
{
    return bean_mgr->ops.match(bean_mgr, env, bean, candidate);
}

AXIS2_EXTERN sandesha2_permanent_bean_mgr_t * AXIS2_CALL
sandesha2_permanent_bean_mgr_create(
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_ctx_t *ctx,
    axis2_char_t *key)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    bean_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_bean_mgr_impl_t));

    bean_mgr_impl->storage_mgr = storage_mgr;
    bean_mgr_impl->mutex = NULL;
    bean_mgr_impl->db = NULL;

    bean_mgr_impl->mutex = axis2_thread_mutex_create(env->allocator, 
        AXIS2_THREAD_MUTEX_DEFAULT);
    if(!bean_mgr_impl->mutex) 
    {
        sandesha2_permanent_bean_mgr_free(&(bean_mgr_impl->bean_mgr), env);
        return NULL;
    }
    return &(bean_mgr_impl->bean_mgr);
}

void AXIS2_CALL
sandesha2_permanent_bean_mgr_free(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Start:sandesha2_permanent_bean_mgr_free_impl");
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);

    if(bean_mgr_impl->mutex)
    {
        axis2_thread_mutex_destroy(bean_mgr_impl->mutex);
        bean_mgr_impl->mutex = NULL;
    }
    if(bean_mgr_impl)
    {
        AXIS2_FREE(env->allocator, bean_mgr_impl);
        bean_mgr_impl = NULL;
    }
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_bean_mgr_free_impl");
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_insert(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *sql_stmt_insert)
{
    int rc = -1;
    axis2_char_t *error_msg = NULL;
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    /*sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);*/
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_insert, 0, 0, &error_msg);
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    sqlite3_close(bean_mgr_impl->db);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_remove(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve,
    axis2_char_t *sql_stmt_remove)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    axis2_char_t *error_msg = NULL;
    int rc = -1;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = env;
    args->data = bean;
    rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_retrieve, retrieve_func, args, 
        &error_msg);
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    /*if(sandesha2_rm_bean_get_key(bean, env))
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);*/
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_remove, 0, 0, &error_msg);
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    sqlite3_close(bean_mgr_impl->db);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_retrieve(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    axis2_char_t *error_msg = NULL;
    int rc = -1;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    rc = sqlite3_open(DB_NAME, &(bean_mgr_impl->db));
    if(rc)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Can't open database: %s\n", 
            sqlite3_errmsg(bean_mgr_impl->db));
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_OPEN_DATABASE, 
            AXIS2_FAILURE);
        sqlite3_close(bean_mgr_impl->db);
        return NULL;
    }
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = (axis2_env_t*)env;
    args->data = bean;
    rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_retrieve, retrieve_func, args, 
        &error_msg);
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    /*if(bean)
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);*/
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = (axis2_env_t*)env;
    args->data = bean;
    rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_retrieve, retrieve_func, args, 
        &error_msg);
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    sqlite3_close(bean_mgr_impl->db);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_update(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *old_bean,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve_old_bean,
    axis2_char_t *sql_stmt_update)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    axis2_char_t *error_msg = NULL;
    int rc = -1;
    axis2_char_t *key = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    /*if(bean)
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);*/
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    rc = sqlite3_open(DB_NAME, &(bean_mgr_impl->db));
    if(rc)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Can't open database: %s\n", 
            sqlite3_errmsg(bean_mgr_impl->db));
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_OPEN_DATABASE, 
            AXIS2_FAILURE);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    if(old_bean)
    {
        args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
        args->env = (axis2_env_t*)env;
        args->data = old_bean;
        rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_retrieve_old_bean, 
           retrieve_func, args, &error_msg);
    }
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_update, 0, 0, &error_msg);
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return AXIS2_FALSE;
    }
    sqlite3_close(bean_mgr_impl->db);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    key = sandesha2_rm_bean_get_key(old_bean, env);
    if(!key)
        return AXIS2_FALSE;
    /*sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, old_bean);*/
    return AXIS2_TRUE;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_find(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*find_func)(void *, int, char **, char **),
    int (*count_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_find,
    axis2_char_t *sql_stmt_count)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0, rc = -1;
    axis2_array_list_t *data_array = axis2_array_list_create(env, 0);
    axis2_char_t *error_msg = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    beans = axis2_array_list_create(env, 0);
    if(!beans)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    rc = sqlite3_open(DB_NAME, &(bean_mgr_impl->db));
    if(rc)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Can't open database: %s\n", 
            sqlite3_errmsg(bean_mgr_impl->db));
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_OPEN_DATABASE, 
            AXIS2_FAILURE);
        sqlite3_close(bean_mgr_impl->db);
        return NULL;
    }
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = (axis2_env_t*)env;
    args->data = data_array;
    rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_find, find_func, args, 
        &error_msg);
    if( rc!=SQLITE_OK )
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
            error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->db);
        return NULL;
    }
    if(data_array)
        size = AXIS2_ARRAY_LIST_SIZE(data_array, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_rm_bean_t *candidate = NULL;
        candidate = (sandesha2_rm_bean_t *) AXIS2_ARRAY_LIST_GET(data_array, 
            env, i);
         if(!candidate)
            continue;
        if(bean && sandesha2_permanent_bean_mgr_match(bean_mgr, env, bean,
            candidate))
        {
            AXIS2_ARRAY_LIST_ADD(beans, env, candidate);
        }
        if(!bean)
            AXIS2_ARRAY_LIST_ADD(beans, env, candidate);
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    /* Now we have a point-in-time view of the beans, lock them all.*/
    size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    for(i = 0; i < size; i++)
    {
        /*sandesha2_rm_bean_t *temp = AXIS2_ARRAY_LIST_GET(beans, env, i);
        if(temp)
            sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, 
                temp);*/
    }
    /* Finally remove any beans that are no longer in the table */
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_rm_bean_t *temp = AXIS2_ARRAY_LIST_GET(beans, env, i);
        if(temp)
        {
            sandesha2_bean_mgr_args_t *args = NULL;
            int count = -1;
            args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
            args->env = (axis2_env_t*)env;
            args->data = &count;
            rc = sqlite3_exec(bean_mgr_impl->db, sql_stmt_count, count_func, args, 
                &error_msg);
            if( rc!=SQLITE_OK )
            {
                AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
                    error_msg);
                sqlite3_free(error_msg);
                sqlite3_close(bean_mgr_impl->db);
                return NULL;
            }
            if(count == 0)
            {
                AXIS2_ARRAY_LIST_REMOVE(beans, env, i);
            }
        }
    }
    sqlite3_close(bean_mgr_impl->db);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return beans;
}

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_find_unique(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*find_func)(void *, int, char **, char **),
    int (*count_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_find,
    axis2_char_t *sql_stmt_count)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    axis2_array_list_t *beans = NULL;
    int size = 0;
    sandesha2_rm_bean_t *ret = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    beans = sandesha2_permanent_bean_mgr_find(bean_mgr, env, bean, find_func, 
        count_func, sql_stmt_find, sql_stmt_count);
    if(beans)
        size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    if( size > 1)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Non-Unique result");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NON_UNIQUE_RESULT, 
            AXIS2_FAILURE);
        return NULL;
    }
    if(size == 1)
       ret = AXIS2_ARRAY_LIST_GET(beans, env, 0);
    return ret;
}

