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
#include <sandesha2_property_bean.h>
#include <sandesha2_utils.h>
#include <sandesha2_property_bean.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <axis2_conf_ctx.h>
#include <sqlite3.h>
#include <axis2_types.h>
#include <platforms/axis2_platform_auto_sense.h>

/** 
 * @brief Sandesha2 Permanent Bean Manager Struct Impl
 *   Sandesha2 Permanent Bean Manager
 */
typedef struct sandesha2_permanent_bean_mgr_impl
{
    sandesha2_permanent_bean_mgr_t bean_mgr;
    sandesha2_storage_mgr_t *storage_mgr;
    axis2_thread_mutex_t *mutex;
    sqlite3 *dbconn;
    axis2_char_t *db_name;

}sandesha2_permanent_bean_mgr_impl_t;

#define SANDESHA2_INTF_TO_IMPL(bean_mgr) \
    ((sandesha2_permanent_bean_mgr_impl_t *) bean_mgr)

static axis2_status_t sandesha2_permanent_bean_mgr_get_dbconn(
    const axis2_env_t *env,
    axis2_char_t *db_name,
    sqlite3 **db);

static int
sandesha2_permanent_bean_mgr_busy_handler(
    sqlite3* dbconn,
    char *sql_stmt,
    int (*callback_func)(void *, int, char **, char **),
    void *arg,
    char **error_msg,
    int rc);

static int 
sandesha2_msg_store_bean_retrieve_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int i = 0;
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    const axis2_env_t *env = args->env;
    sandesha2_msg_store_bean_t *bean = NULL;
    if(argc < 1)
    {
        args->data = NULL;
        return 0;
    }
    bean = (sandesha2_msg_store_bean_t *) args->data;
    if(!bean)
    {
        bean = sandesha2_msg_store_bean_create(env);
        args->data = bean;
    }
    for(i = 0; i < argc; i++)
    {
        if(0 == AXIS2_STRCMP(col_name[i], "stored_key"))
            if(argv[i])
                sandesha2_msg_store_bean_set_stored_key(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "msg_id"))
            if(argv[i])
                sandesha2_msg_store_bean_set_msg_id(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "soap_env_str"))
            if(argv[i])
                sandesha2_msg_store_bean_set_soap_envelope_str(bean, env, 
                    argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "soap_version"))
            if(argv[i])
                sandesha2_msg_store_bean_set_soap_version(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == AXIS2_STRCMP(col_name[i], "transport_out"))
            if(argv[i])
                sandesha2_msg_store_bean_set_transport_out(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "op"))
            if(argv[i])
                sandesha2_msg_store_bean_set_op(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "svc"))
            if(argv[i])
                sandesha2_msg_store_bean_set_svc(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "svc_grp"))
            if(argv[i])
                sandesha2_msg_store_bean_set_svc_grp(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "op_mep"))
            if(argv[i])
                sandesha2_msg_store_bean_set_op_mep(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "to_url"))
            if(argv[i])
                sandesha2_msg_store_bean_set_to_url(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "transport_to"))
            if(argv[i])
                sandesha2_msg_store_bean_set_transport_to(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "execution_chain_str"))
            if(argv[i])
                sandesha2_msg_store_bean_set_execution_chain_str(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "flow"))
            if(argv[i])
                sandesha2_msg_store_bean_set_flow(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == AXIS2_STRCMP(col_name[i], "msg_recv_str"))
            if(argv[i])
                sandesha2_msg_store_bean_set_msg_recv_str(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "svr_side"))
            if(argv[i])
                sandesha2_msg_store_bean_set_svr_side(bean, env, AXIS2_ATOI(argv[i]));
        if(0 == AXIS2_STRCMP(col_name[i], "in_msg_store_key"))
            if(argv[i])
                sandesha2_msg_store_bean_set_in_msg_store_key(bean, env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "prop_str"))
            if(argv[i])
                sandesha2_msg_store_bean_set_persistent_property_str(bean, 
                    env, argv[i]);
        if(0 == AXIS2_STRCMP(col_name[i], "action"))
            if(argv[i])
                sandesha2_msg_store_bean_set_action(bean, env, argv[i]);
    }
    return 0;
}


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
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *key)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    axis2_conf_t *conf = NULL;
    sandesha2_property_bean_t *prop_bean = NULL;
    axis2_char_t *path = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    bean_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_bean_mgr_impl_t));

    bean_mgr_impl->storage_mgr = storage_mgr;
    bean_mgr_impl->mutex = NULL;
    bean_mgr_impl->dbconn = NULL;
    conf = AXIS2_CONF_CTX_GET_CONF((const axis2_conf_ctx_t *) conf_ctx, env);
    prop_bean = sandesha2_utils_get_property_bean(env, conf);
    path = sandesha2_property_bean_get_db_path(prop_bean, env);
    bean_mgr_impl->db_name = axis2_strcat(env, path, AXIS2_PATH_SEP_STR,
        "sandesha2", NULL);

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
    sandesha2_rm_bean_t *bean,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve,
    axis2_char_t *sql_stmt_update,
    axis2_char_t *sql_stmt_insert)
{
    axis2_char_t *error_msg = NULL;
    int rc = -1;
    sandesha2_transaction_t *transaction = NULL;
    sqlite3 *dbconn = NULL;
    sandesha2_rm_bean_t *retrieve_bean = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    
    sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = env;
    args->data = NULL;
    if(bean)
    {
        sandesha2_rm_bean_t *bean_l = NULL;
        bean_l = sandesha2_rm_bean_get_base(bean, env);
        transaction = sandesha2_rm_bean_get_transaction(bean_l, env);
    }
    if(transaction)
    {
        dbconn = (sqlite3 *) sandesha2_permanent_transaction_get_dbconn(transaction, env);
        rc = sqlite3_exec(dbconn, sql_stmt_retrieve, retrieve_func, args,
            &error_msg);
        if(rc == SQLITE_BUSY)
            rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_retrieve, 
                retrieve_func, args, &error_msg, rc);
        if(rc != SQLITE_OK )
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql stmt:%s. sql error: %s",
                sql_stmt_retrieve, error_msg);
            printf("sql_stmt_retrieve:%s\n", sql_stmt_retrieve);
            printf("retrieve error_msg:%s\n", error_msg);
            sqlite3_free(error_msg);
            return AXIS2_FALSE;
        }
        if(args)
            AXIS2_FREE(env->allocator, args);
        retrieve_bean = (sandesha2_rm_bean_t *) args->data;
        if(retrieve_bean)
        {
            rc = sqlite3_exec(dbconn, sql_stmt_update, 0, 0, &error_msg);
            if(rc == SQLITE_BUSY)
                rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_update, 
                0, 0, &error_msg, rc);
            if( rc != SQLITE_OK )
            {
                axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
                AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, 
                    AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql stmt: %s.sql error: %s", 
                    sql_stmt_update, error_msg);
                printf("sql_stmt_update:%s\n", sql_stmt_update);
                printf("update error_msg:%s\n", error_msg);
                sqlite3_free(error_msg);
                return AXIS2_FALSE;
            }
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            return AXIS2_TRUE;
        }
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        axis2_thread_mutex_lock(bean_mgr_impl->mutex);
        rc = sqlite3_exec(dbconn, sql_stmt_insert, 0, 0, &error_msg);
        if(rc == SQLITE_BUSY)
            rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_insert, 
                0, 0, &error_msg, rc);
        if( rc != SQLITE_OK )
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql stmt: %s. sql error: %s", 
                sql_stmt_insert, error_msg);
            printf("sql_stmt_insert:%s\n", sql_stmt_insert);
            printf("insert error_msg:%s\n", error_msg);
            sqlite3_free(error_msg);
            return AXIS2_FALSE;
        }
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_remove(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve,
    axis2_char_t *sql_stmt_remove)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    axis2_char_t *error_msg = NULL;
    sandesha2_rm_bean_t *bean = NULL;
    sandesha2_transaction_t *transaction = NULL;
    sqlite3 *dbconn = NULL;
    int rc = -1;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(!sandesha2_permanent_bean_mgr_get_dbconn(env, bean_mgr_impl->db_name, 
        &(bean_mgr_impl->dbconn)))
        return AXIS2_FALSE;
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = env;
    args->data = NULL;
    rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_retrieve, retrieve_func, args, 
        &error_msg);
    if(rc == SQLITE_BUSY)
        rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_retrieve, 
            retrieve_func, args, &error_msg, rc);
    if(rc != SQLITE_OK )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql stmt: %s. sql error: %s",
            sql_stmt_retrieve, error_msg);
        printf("sql_stmt_retrieve:%s\n", sql_stmt_retrieve);
        printf("retrieve error_msg:%s\n", error_msg);
        sqlite3_free(error_msg);
        return AXIS2_FALSE;
    }
    bean = (sandesha2_rm_bean_t *) args->data;
    if(args)
        AXIS2_FREE(env->allocator, args);
    sqlite3_close(bean_mgr_impl->dbconn);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    if(bean)
    {
        if(sandesha2_rm_bean_get_key(bean, env))
            sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, 
                bean);
    }
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(bean)
    {
        sandesha2_rm_bean_t *bean_l = NULL;
        bean_l = sandesha2_rm_bean_get_base(bean, env);
        transaction = sandesha2_rm_bean_get_transaction(bean_l, env);
    }
    if(transaction)
    {
        dbconn = (sqlite3 *) sandesha2_permanent_transaction_get_dbconn(transaction, env);
        rc = sqlite3_exec(dbconn, sql_stmt_remove, 0, 0, &error_msg);
        if(rc == SQLITE_BUSY)
            rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_remove, 
                0, 0, &error_msg, rc);
        if(rc != SQLITE_OK )
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql stmt: %s. sql error: %s",
                sql_stmt_remove, error_msg);
            printf("sql_stmt_remove:%s\n", sql_stmt_remove);
            printf("remove error_msg:%s\n", error_msg);
            sqlite3_free(error_msg);
            return AXIS2_FALSE;
        }
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_retrieve(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    axis2_char_t *error_msg = NULL;
    sandesha2_rm_bean_t *bean = NULL;
    sandesha2_transaction_t *transaction = NULL;
    sqlite3 *dbconn = NULL;
    int rc = -1;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(!sandesha2_permanent_bean_mgr_get_dbconn(env, bean_mgr_impl->db_name, 
        &(bean_mgr_impl->dbconn)))
        return NULL;
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = (axis2_env_t*)env;
    args->data = NULL;
    rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_retrieve, retrieve_func, args, 
        &error_msg);
    if(rc == SQLITE_BUSY)
        rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_retrieve, 
            retrieve_func, args, &error_msg, rc);
    if(rc != SQLITE_OK )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql stmt: %s. sql error: %s",
            sql_stmt_retrieve, error_msg);
        printf("sql_stmt_retrieve:%s\n", sql_stmt_retrieve);
        printf("retrieve error_msg:%s\n", error_msg);
        sqlite3_free(error_msg);
        return AXIS2_FALSE;
    }
    bean = (sandesha2_rm_bean_t *) args->data;
    if(args)
        AXIS2_FREE(env->allocator, args);
    sqlite3_close(bean_mgr_impl->dbconn);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    if(bean)
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = (axis2_env_t*)env;
    args->data = NULL;
    if(bean)
    {
        sandesha2_rm_bean_t *bean_l = NULL;
        bean_l = sandesha2_rm_bean_get_base(bean, env);
        transaction = sandesha2_rm_bean_get_transaction(bean_l, env);
    }
    if(transaction)
    {
        dbconn = (sqlite3 *) sandesha2_permanent_transaction_get_dbconn(bean, env);
        rc = sqlite3_exec(dbconn, sql_stmt_retrieve, retrieve_func, args, 
            &error_msg);
        if(rc == SQLITE_BUSY)
            rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_retrieve, 
                retrieve_func, args, &error_msg, rc);
        if(rc != SQLITE_OK )
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
                error_msg);
            printf("sql_stmt_retrieve:%s\n", sql_stmt_retrieve);
            printf("retrieve error_msg:%s\n", error_msg);
            sqlite3_free(error_msg);
            return AXIS2_FALSE;
        }
        bean = (sandesha2_rm_bean_t *) args->data;
        if(args)
            AXIS2_FREE(env->allocator, args);
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_update(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve_old_bean,
    axis2_char_t *sql_stmt_update)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    sandesha2_transaction_t *transaction = NULL;
    sqlite3 *dbconn = NULL;
    sandesha2_rm_bean_t *old_bean = NULL;
    axis2_char_t *error_msg = NULL;
    int rc = -1;
    axis2_char_t *key = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    if(bean)
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(bean)
    {
        sandesha2_rm_bean_t *bean_l = NULL;
        bean_l = sandesha2_rm_bean_get_base(bean, env);
        transaction = sandesha2_rm_bean_get_transaction(bean_l, env);
    }
    if(transaction)
    {
        dbconn = (sqlite3 *) sandesha2_permanent_transaction_get_dbconn(
            transaction, env);
        args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
        args->env = (axis2_env_t*)env;
        args->data = NULL;
        rc = sqlite3_exec(dbconn, sql_stmt_retrieve_old_bean, 
           retrieve_func, args, &error_msg);
        if(rc == SQLITE_BUSY)
            rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_retrieve_old_bean, 
                retrieve_func, args, &error_msg, rc);
        if(rc != SQLITE_OK )
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            if(args)
                AXIS2_FREE(env->allocator, args);
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
                error_msg);
            printf("sql_stmt_retrieve_old_bean:%s\n", sql_stmt_retrieve_old_bean);
            printf("retrieve error_msg:%s\n", error_msg);
            sqlite3_free(error_msg);
            return AXIS2_FALSE;
        }
        rc = sqlite3_exec(dbconn, sql_stmt_update, 0, 0, &error_msg);
        if(rc == SQLITE_BUSY)
            rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_update, 
                0, 0, &error_msg, rc);
        if(rc != SQLITE_OK )
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
                error_msg);
            printf("sql_stmt_update:%s\n", sql_stmt_update);
            printf("update error_msg:%s\n", error_msg);
            sqlite3_free(error_msg);
            return AXIS2_FALSE;
        }
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        old_bean = (sandesha2_rm_bean_t *) args->data;
        if(old_bean)
            key = sandesha2_rm_bean_get_key(old_bean, env);
        if(!key)
            return AXIS2_FALSE;
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, old_bean);
        if(args)
            AXIS2_FREE(env->allocator, args);
    }
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
    axis2_array_list_t *data_array = NULL;
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
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = (axis2_env_t*)env;
    args->data = NULL;
    if(!sandesha2_permanent_bean_mgr_get_dbconn(env, bean_mgr_impl->db_name, 
        &(bean_mgr_impl->dbconn)))
        return NULL;
    rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_find, find_func, args, 
        &error_msg);
    if(rc == SQLITE_BUSY)
        rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_find, 
            find_func, args, &error_msg, rc);
    data_array = (axis2_array_list_t *) args->data;
    if(rc != SQLITE_OK )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_array)
            AXIS2_ARRAY_LIST_FREE(data_array, env);
        if(args)
            AXIS2_FREE(env->allocator, args);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
            error_msg);
        printf("sql_stmt_find:%s\n", sql_stmt_find);
        printf("retrieve error_msg:%s\n", error_msg);
        sqlite3_free(error_msg);
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
    if(data_array)
        AXIS2_ARRAY_LIST_FREE(data_array, env);
    if(args)
        AXIS2_FREE(env->allocator, args);
    sqlite3_close(bean_mgr_impl->dbconn);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    /* Now we have a point-in-time view of the beans, lock them all.*/
    size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_rm_bean_t *temp = AXIS2_ARRAY_LIST_GET(beans, env, i);
        if(temp)
            sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, 
                temp);
    }
    /* Finally remove any beans that are no longer in the table */
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    size = AXIS2_ARRAY_LIST_SIZE(beans, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_rm_bean_t *temp = AXIS2_ARRAY_LIST_GET(beans, env, i);
        if(temp)
        {
            int count = -1;
            sandesha2_transaction_t *transaction = NULL;
            sqlite3 *dbconn = NULL;
            sandesha2_rm_bean_t *bean_l = NULL;

            bean_l = sandesha2_rm_bean_get_base(temp, env);
            transaction = sandesha2_rm_bean_get_transaction(bean_l, env);
            dbconn = (sqlite3 *) sandesha2_permanent_transaction_get_dbconn(
                transaction, env);
            rc = sqlite3_exec(dbconn, sql_stmt_count, count_func, &count, 
                &error_msg);
            if(rc == SQLITE_BUSY)
                rc = sandesha2_permanent_bean_mgr_busy_handler(dbconn, sql_stmt_count, 
                    count_func, &count, &error_msg, rc);
            if(rc != SQLITE_OK )
            {
                axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
                AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
                    error_msg);
                printf("sql_stmt_count:%s\n", sql_stmt_count);
                printf("retrieve error_msg:%s\n", error_msg);
                sqlite3_free(error_msg);
                return NULL;
            }
            if(count == 0)
            {
                AXIS2_ARRAY_LIST_REMOVE(beans, env, i);
            }
        }
    }
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

sandesha2_msg_store_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_retrieve_msg_store_bean(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_bean_mgr_args_t *args = NULL;
    axis2_char_t *error_msg = NULL;
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;
    int rc = -1;
    axis2_char_t *sql_stmt_retrieve = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(!sandesha2_permanent_bean_mgr_get_dbconn(env, bean_mgr_impl->db_name, 
        &(bean_mgr_impl->dbconn)))
        return NULL;
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = (axis2_env_t*)env;
    args->data = NULL;
    sql_stmt_retrieve = axis2_strcat(env, "select stored_key, msg_id, soap_env_str,"\
        "soap_version, transport_out, op, svc, svc_grp, op_mep, to_url, "\
        "transport_to, execution_chain_str, flow, msg_recv_str, svr_side, "\
        "in_msg_store_key, prop_str, action from msg where stored_key = '", 
        key, "';", NULL);
    rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_retrieve, 
        sandesha2_msg_store_bean_retrieve_callback, args, &error_msg);
    if(rc == SQLITE_BUSY)
        rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_retrieve, 
            sandesha2_msg_store_bean_retrieve_callback, args, &error_msg, rc);
    if(rc != SQLITE_OK )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        printf("sql_stmt_retrieve:%s\n", sql_stmt_retrieve);
        printf("retrieve error_msg:%s\n", error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(bean_mgr_impl->dbconn);
        return AXIS2_FALSE;
    }
    msg_store_bean = (sandesha2_msg_store_bean_t *) args->data;
    if(args)
        AXIS2_FREE(env->allocator, args);
    sqlite3_close(bean_mgr_impl->dbconn);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return msg_store_bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_insert_msg_store_bean(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    sandesha2_msg_store_bean_t *bean)
{
    axis2_char_t *sql_stmt_retrieve = NULL;
    axis2_char_t *sql_stmt_update = NULL;
    axis2_char_t *sql_stmt_insert = NULL;
    axis2_char_t *error_msg = NULL;
    int rc = -1;
    sandesha2_bean_mgr_args_t *args = NULL;
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;

    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = 
        bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    
    axis2_char_t *msg_id = sandesha2_msg_store_bean_get_msg_id(bean, env);
    axis2_char_t str_msg_id[msg_id?AXIS2_STRLEN(msg_id):0];
    axis2_char_t *stored_key = sandesha2_msg_store_bean_get_stored_key(bean, env);
    axis2_char_t str_stored_key[stored_key?AXIS2_STRLEN(stored_key):0];
    axis2_char_t *soap_env_str = 
        sandesha2_msg_store_bean_get_soap_envelope_str(bean, env);
    axis2_char_t str_soap_env_str[soap_env_str?AXIS2_STRLEN(soap_env_str):0];
    int soap_version = sandesha2_msg_store_bean_get_soap_version(
        bean, env);
    axis2_char_t str_soap_version[32];
    axis2_char_t *transport_out = sandesha2_msg_store_bean_get_transport_out(
        bean, env);
    axis2_char_t str_transport_out[transport_out?AXIS2_STRLEN(transport_out):0];
    axis2_char_t *op = sandesha2_msg_store_bean_get_op(bean, env);
    axis2_char_t str_op[op?AXIS2_STRLEN(op):0];
    axis2_char_t *svc = sandesha2_msg_store_bean_get_svc(bean, env);
    axis2_char_t str_svc[svc?AXIS2_STRLEN(svc):0];
    axis2_char_t *svc_grp = sandesha2_msg_store_bean_get_svc_grp(bean, env);
    axis2_char_t str_svc_grp[svc_grp?AXIS2_STRLEN(svc_grp):0];
    axis2_char_t *op_mep = sandesha2_msg_store_bean_get_op_mep(bean, env);;
    axis2_char_t str_op_mep[op_mep?AXIS2_STRLEN(op_mep):0];
    axis2_char_t *to_url = sandesha2_msg_store_bean_get_to_url(bean, env);
    axis2_char_t str_to_url[to_url?AXIS2_STRLEN(to_url):0];
    axis2_char_t *reply_to = sandesha2_msg_store_bean_get_reply_to(bean, env);
    axis2_char_t str_reply_to[reply_to?AXIS2_STRLEN(reply_to):0];
    axis2_char_t *transport_to = sandesha2_msg_store_bean_get_transport_to(
        bean, env);
    axis2_char_t str_transport_to[transport_to?AXIS2_STRLEN(transport_to):0];
    axis2_char_t *execution_chain_str = 
        sandesha2_msg_store_bean_get_execution_chain_str(bean, env);
    axis2_char_t str_execution_chain_str[execution_chain_str?AXIS2_STRLEN(execution_chain_str):0];
    int flow = sandesha2_msg_store_bean_get_flow(bean, env);
    axis2_char_t str_flow[32];
    axis2_char_t *msg_recv_str = sandesha2_msg_store_bean_get_msg_recv_str(
        bean, env);
    axis2_char_t str_msg_recv_str[msg_recv_str?AXIS2_STRLEN(msg_recv_str):0];
    axis2_bool_t svr_side = sandesha2_msg_store_bean_is_svr_side(bean, env);
    axis2_char_t str_svr_side[32];
    axis2_char_t *in_msg_store_key = 
        sandesha2_msg_store_bean_get_in_msg_store_key(bean, env);
    axis2_char_t str_in_msg_store_key[in_msg_store_key?AXIS2_STRLEN(in_msg_store_key):0];
    axis2_char_t *prop_str = 
        sandesha2_msg_store_bean_get_persistent_property_str(bean, env);
    axis2_char_t str_prop_str[prop_str?AXIS2_STRLEN(prop_str):0];
    axis2_char_t *action = sandesha2_msg_store_bean_get_action(bean, env);
    axis2_char_t str_action[action?AXIS2_STRLEN(action):0];

    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(!sandesha2_permanent_bean_mgr_get_dbconn(env, bean_mgr_impl->db_name, 
        &(bean_mgr_impl->dbconn)))
        return AXIS2_FALSE;
    sprintf(str_stored_key, "'%s'", stored_key);
    sprintf(str_msg_id, msg_id?"'%s'":"NULL", msg_id);
    sprintf(str_soap_env_str, soap_env_str?"'%s'":"NULL", soap_env_str);
    sprintf(str_soap_version, "%d", soap_version);
    sprintf(str_transport_out, transport_out?"'%s'":"NULL", transport_out);
    sprintf(str_op, op?"'%s'":"NULL", op);
    sprintf(str_svc, svc?"'%s'":"NULL", svc);
    sprintf(str_svc_grp, svc_grp?"'%s'":"NULL", svc_grp);
    sprintf(str_op_mep, op_mep?"'%s'":"NULL", op_mep);
    sprintf(str_to_url, to_url?"'%s'":"NULL", to_url);
    sprintf(str_reply_to, reply_to?"'%s'":"NULL", reply_to);
    sprintf(str_transport_to, transport_to?"'%s'":"NULL", transport_to);
    sprintf(str_execution_chain_str, execution_chain_str?"'%s'":"NULL", 
        execution_chain_str);
    sprintf(str_flow, "%d", flow);
    sprintf(str_msg_recv_str, msg_recv_str?"'%s'":"NULL", msg_recv_str);
    sprintf(str_svr_side, "%d", svr_side);
    sprintf(str_in_msg_store_key, in_msg_store_key?"'%s'":"NULL", in_msg_store_key);
    sprintf(str_prop_str, "'%s'", prop_str);
    sprintf(str_action, action?"'%s'":"NULL", action);

    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_bean_mgr_args_t));
    args->env = env;
    args->data = NULL;
    sql_stmt_retrieve = axis2_strcat(env, "select stored_key, msg_id, soap_env_str,"\
        "soap_version, transport_out, op, svc, svc_grp, op_mep, to_url, reply_to, "\
        "transport_to, execution_chain_str, flow, msg_recv_str, svr_side, "\
        "in_msg_store_key, prop_str, action from msg where stored_key = '", 
        key, "';", NULL);
    rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_retrieve,
        sandesha2_msg_store_bean_retrieve_callback, args, &error_msg);
    if(rc == SQLITE_BUSY)
        rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_retrieve, 
            sandesha2_msg_store_bean_retrieve_callback, args, &error_msg, rc);
    if(rc != SQLITE_OK )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        printf("sql_stmt_retrieve:%s\n", sql_stmt_retrieve);
        printf("retrieve error_msg:%s\n", error_msg);
        sqlite3_free(error_msg);
        return AXIS2_FALSE;
    }
    if(args)
        AXIS2_FREE(env->allocator, args);
    msg_store_bean = (sandesha2_msg_store_bean_t *) args->data;
    if(msg_store_bean)
    {
        sql_stmt_update = axis2_strcat(env, "update msg set msg_id=", 
            str_msg_id, ",soap_env_str=", str_soap_env_str, ",soap_version=", 
            str_soap_version, ", transport_out=", str_transport_out, ", op=", 
            str_op, ", svc=", str_svc, ", svc_grp=", str_svc_grp, ", op_mep=", 
            str_op_mep, ", to_url=", str_to_url, ", transport_to=", 
            str_transport_to, ", reply_to=", str_reply_to,  ", execution_chain_str=", 
            str_execution_chain_str, ", flow=", str_flow, ", msg_recv_str=", 
            str_msg_recv_str, ", svr_side=", str_svr_side, ", in_msg_store_key=",
            str_in_msg_store_key, ", prop_str=", str_prop_str, ", action=", str_action,
            " where stored_key='", key, "';", NULL);
        rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_update, 0, 0, &error_msg);
        if(rc == SQLITE_BUSY)
            rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_update, 
                0, 0, &error_msg, rc);
        if( rc != SQLITE_OK )
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, 
                AXIS2_FAILURE);
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", 
                error_msg);
            printf("sql_stmt_update:%s\n", sql_stmt_update);
            printf("update error_msg:%s\n", error_msg);
            sqlite3_free(error_msg);
            return AXIS2_FALSE;
        }
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        return AXIS2_TRUE;
    }
    sql_stmt_insert = axis2_strcat(env, "insert into msg(stored_key, msg_id, "\
        "soap_env_str, soap_version, transport_out, op, svc, svc_grp, op_mep,"\
        "to_url, reply_to,transport_to, execution_chain_str, flow, msg_recv_str, "\
        "svr_side, in_msg_store_key, prop_str, action) values(", str_stored_key, 
        ",", str_msg_id, ",", str_soap_env_str, ",", str_soap_version, ",", 
        str_transport_out, ",", str_op, ",", str_svc, ",", str_svc_grp, ",", 
        str_op_mep, ",", str_to_url, ",", str_reply_to, ",", str_transport_to, ",", 
        str_execution_chain_str, ",", str_flow, ",", str_msg_recv_str, ",", 
        str_svr_side, ",", str_in_msg_store_key, ",", str_prop_str, 
        ",", str_action, ");", NULL);
    rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_insert, 0, 0, &error_msg);
    if(rc == SQLITE_BUSY)
        rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_insert, 
            0, 0, &error_msg, rc);
    if( rc != SQLITE_OK )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s", error_msg);
        printf("sql_stmt_insert:%s\n", sql_stmt_insert);
        printf("insert error_msg:%s\n", error_msg);
        sqlite3_free(error_msg);
        return AXIS2_FALSE;
    }
    sqlite3_close(bean_mgr_impl->dbconn);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_remove_msg_store_bean(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    axis2_char_t *sql_stmt_remove = NULL;
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    axis2_char_t *error_msg = NULL;
    int rc = -1;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(!sandesha2_permanent_bean_mgr_get_dbconn(env, bean_mgr_impl->db_name, 
        &(bean_mgr_impl->dbconn)))
        return AXIS2_FALSE;
    sql_stmt_remove = axis2_strcat(env, "delete from msg where stored_key='",
        key, "';", NULL);
    rc = sqlite3_exec(bean_mgr_impl->dbconn, sql_stmt_remove, 0, 0, &error_msg);
    if(rc == SQLITE_BUSY)
        rc = sandesha2_permanent_bean_mgr_busy_handler(bean_mgr_impl->dbconn, sql_stmt_remove, 
            0, 0, &error_msg, rc);
    if(rc != SQLITE_OK )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "SQL Error %s",
            error_msg);
        printf("sql_stmt_remove:%s\n", sql_stmt_remove);
        printf("remove error_msg:%s\n", error_msg);
        sqlite3_free(error_msg);
        return AXIS2_FALSE;
    }
    sqlite3_close(bean_mgr_impl->dbconn);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

static int
sandesha2_permanent_bean_mgr_busy_handler(
    sqlite3* dbconn,
    char *sql_stmt,
    int (*callback_func)(void *, int, char **, char **),
    void *args,
    char **error_msg,
    int rc)
{
    int counter = 0;
    while(rc == SQLITE_BUSY && counter < 8)
    {
        if(*error_msg)
             sqlite3_free(*error_msg);
        counter++;
        AXIS2_SLEEP(SANDESHA2_BUSY_WAIT_TIME);
        rc = sqlite3_exec(dbconn, sql_stmt, callback_func, args, error_msg);
    }
    return rc;
}

static axis2_status_t sandesha2_permanent_bean_mgr_get_dbconn(
    const axis2_env_t *env,
    axis2_char_t *db_name,
    sqlite3 **dbconn)
{
    int rc = sqlite3_open(db_name, dbconn);
    if(rc != SQLITE_OK)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Can't open database: %s\n", 
            sqlite3_errmsg(*dbconn));
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CANNOT_OPEN_DATABASE, 
            AXIS2_FAILURE);
        sqlite3_close(*dbconn);
        return AXIS2_FAILURE;
    }
    return AXIS2_SUCCESS;
}

