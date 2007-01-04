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

}sandesha2_permanent_bean_mgr_impl_t;

#define SANDESHA2_INTF_TO_IMPL(bean_mgr) \
    ((sandesha2_permanent_bean_mgr_impl_t *) bean_mgr)

static int
sandesha2_permanent_bean_mgr_busy_handler(
    sqlite3_stmt* stmt,
    int rc);

static void 
sandesha2_permanent_bean_mgr_handle_sql_error(
    const axis2_env_t *env,
    sqlite3 *dbconn,
    axis2_char_t *sql_stmt);

static axis2_status_t 
sandesha2_msg_retrieve_callback(
    const axis2_env_t *env,
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    void *key,
    axis2_array_list_t *data_list)
{
    int rc = -1;
    sandesha2_msg_store_bean_t *bean = (sandesha2_msg_store_bean_t *) 
        sandesha2_msg_store_bean_create(env);
    axis2_char_t *stored_key = (axis2_char_t *) key;
    if(stored_key)
    {
        if (sqlite3_bind_text(stmt, 1, stored_key, axis2_strlen(stored_key), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
    }
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_BUSY)
    {
        if(busy_handler(stmt, rc) != SQLITE_OK)
        {
            return AXIS2_FAILURE;
        }
    }
    if(rc == SQLITE_ROW)
    {
        sandesha2_msg_store_bean_set_stored_key(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 0));
        sandesha2_msg_store_bean_set_msg_id(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 1));
        sandesha2_msg_store_bean_set_soap_envelope_str(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 2));
        sandesha2_msg_store_bean_set_soap_version(bean, env, 
            sqlite3_column_int(stmt, 3));
        sandesha2_msg_store_bean_set_transport_out(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 4));
        sandesha2_msg_store_bean_set_op(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 5));
        sandesha2_msg_store_bean_set_svc(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 6));
        sandesha2_msg_store_bean_set_svc_grp(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 7));
        sandesha2_msg_store_bean_set_op_mep(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 8));
        sandesha2_msg_store_bean_set_to_url(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 9));
        sandesha2_msg_store_bean_set_reply_to(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 10));
        sandesha2_msg_store_bean_set_transport_to(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 11));
        sandesha2_msg_store_bean_set_execution_chain_str(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 12));
        sandesha2_msg_store_bean_set_flow(bean, env, 
            sqlite3_column_int(stmt, 13));
        sandesha2_msg_store_bean_set_msg_recv_str(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 14));
        sandesha2_msg_store_bean_set_svr_side(bean, env, 
            sqlite3_column_int(stmt, 15));
        sandesha2_msg_store_bean_set_in_msg_store_key(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 16));
        sandesha2_msg_store_bean_set_persistent_property_str(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 17));
        sandesha2_msg_store_bean_set_action(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 18));
        rc = sqlite3_step(stmt);
        if(rc == SQLITE_BUSY)
        {
            if(busy_handler(stmt, rc) != SQLITE_OK)
            {
                return AXIS2_FAILURE;
            }
        }
    }
    axis2_array_list_add(data_list, env, bean);
    return AXIS2_SUCCESS;
}

static axis2_status_t 
sandesha2_msg_update_or_insert_callback(
    const axis2_env_t *env,
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    sandesha2_msg_store_bean_t *bean)
{
    if(!bean)
        return AXIS2_FAILURE;
    {
        int rc = -1;
        axis2_char_t *stored_key = sandesha2_msg_store_bean_get_stored_key(bean, env);
        axis2_char_t *msg_id = sandesha2_msg_store_bean_get_msg_id(bean, env);
        axis2_char_t *soap_env_str = 
            sandesha2_msg_store_bean_get_soap_envelope_str(bean, env);
        int soap_version = sandesha2_msg_store_bean_get_soap_version(
            bean, env);
        axis2_char_t *transport_out = sandesha2_msg_store_bean_get_transport_out(
            bean, env);
        axis2_char_t *op = sandesha2_msg_store_bean_get_op(bean, env);
        axis2_char_t *svc = sandesha2_msg_store_bean_get_svc(bean, env);
        axis2_char_t *svc_grp = sandesha2_msg_store_bean_get_svc_grp(bean, env);
        axis2_char_t *op_mep = sandesha2_msg_store_bean_get_op_mep(bean, env);;
        axis2_char_t *to_url = sandesha2_msg_store_bean_get_to_url(bean, env);
        axis2_char_t *reply_to = sandesha2_msg_store_bean_get_reply_to(bean, env);
        axis2_char_t *transport_to = sandesha2_msg_store_bean_get_transport_to(
            bean, env);
        axis2_char_t *execution_chain_str = 
            sandesha2_msg_store_bean_get_execution_chain_str(bean, env);
        int flow = sandesha2_msg_store_bean_get_flow(bean, env);
        axis2_char_t *msg_recv_str = sandesha2_msg_store_bean_get_msg_recv_str(
            bean, env);
        axis2_bool_t svr_side = sandesha2_msg_store_bean_is_svr_side(bean, env);
        axis2_char_t *in_msg_store_key = 
            sandesha2_msg_store_bean_get_in_msg_store_key(bean, env);
        axis2_char_t *prop_str = 
            sandesha2_msg_store_bean_get_persistent_property_str(bean, env);
        axis2_char_t *action = sandesha2_msg_store_bean_get_action(bean, env);
       
        if (sqlite3_bind_text(stmt, 1, msg_id, axis2_strlen(msg_id), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 2, soap_env_str, axis2_strlen(soap_env_str), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_int(stmt, 3, soap_version))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 4, transport_out, axis2_strlen(transport_out), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 5, op, axis2_strlen(op), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 6, svc, axis2_strlen(svc), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 7, svc_grp, axis2_strlen(svc_grp), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 8, op_mep, axis2_strlen(op_mep), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 9, to_url, axis2_strlen(to_url), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 10, reply_to, axis2_strlen(reply_to), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 11, transport_to, axis2_strlen(transport_to), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 12, execution_chain_str, axis2_strlen(execution_chain_str), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_int(stmt, 13, flow))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 14, msg_recv_str, axis2_strlen(msg_recv_str), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_int(stmt, 15, svr_side))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 16, in_msg_store_key, axis2_strlen(in_msg_store_key), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 17, prop_str, axis2_strlen(prop_str), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 18, action, axis2_strlen(action), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 19, stored_key, axis2_strlen(stored_key), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_BUSY)
        {
            if(busy_handler(stmt, rc) != SQLITE_OK)
            {
                return AXIS2_FAILURE;
            }
        }
        else if (rc == SQLITE_DONE)
            return AXIS2_SUCCESS;
    }
    return AXIS2_FAILURE;
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
    
    AXIS2_ENV_CHECK(env, NULL);
    bean_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_bean_mgr_impl_t));

    bean_mgr_impl->storage_mgr = storage_mgr;
    bean_mgr_impl->mutex = NULL;

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
    void *key,
    sandesha2_rm_bean_t *bean,
    axis2_status_t (*retrieve_callback)(const axis2_env_t *, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), void *, axis2_array_list_t *),
    axis2_status_t (*update_or_insert_callback)(const axis2_env_t *, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), sandesha2_rm_bean_t *),
    axis2_char_t *sql_stmt_retrieve,
    axis2_char_t *sql_stmt_update,
    axis2_char_t *sql_stmt_insert)
{
    int rc = -1;
    sqlite3 *dbconn = NULL;
    sqlite3_stmt *stmt_retrieve = NULL;
    sqlite3_stmt *stmt_update = NULL;
    sqlite3_stmt *stmt_insert = NULL;
    sandesha2_rm_bean_t *retrieve_bean = NULL;
    axis2_array_list_t *data_list = NULL;
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    
    sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
        return AXIS2_FALSE;
    if(sqlite3_prepare(dbconn, sql_stmt_retrieve, AXIS2_STRLEN(sql_stmt_retrieve), 
        &stmt_retrieve, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    data_list = axis2_array_list_create(env, 1);
    rc = retrieve_callback(env, stmt_retrieve, sandesha2_permanent_bean_mgr_busy_handler, key, 
        data_list);
    if(rc != AXIS2_SUCCESS )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    if(0 < AXIS2_ARRAY_LIST_SIZE(data_list, env))
        retrieve_bean = AXIS2_ARRAY_LIST_GET(data_list, env, 0);
    if(data_list)
        AXIS2_ARRAY_LIST_FREE(data_list, env);
    sqlite3_reset(stmt_retrieve);
    if(retrieve_bean)
    {
        if(sqlite3_prepare(dbconn, sql_stmt_update, AXIS2_STRLEN(sql_stmt_update), 
            &stmt_update, NULL))
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
                sql_stmt_update);
            return AXIS2_FALSE;
        }
        rc = update_or_insert_callback(env, stmt_update, sandesha2_permanent_bean_mgr_busy_handler, bean);
        if(rc != AXIS2_SUCCESS)
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
                sql_stmt_update);
            return AXIS2_FALSE;
        }
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        return AXIS2_TRUE;
    }
    sqlite3_reset(stmt_update);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(sqlite3_prepare(dbconn, sql_stmt_insert, AXIS2_STRLEN(sql_stmt_insert), 
        &stmt_insert, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_insert);
        return AXIS2_FALSE;
    }
    rc = update_or_insert_callback(env, stmt_insert, sandesha2_permanent_bean_mgr_busy_handler, bean);
    if(rc != AXIS2_SUCCESS)
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_insert);
        return AXIS2_FALSE;
    }
    sqlite3_reset(stmt_insert);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_remove(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    void *key, 
    axis2_status_t (*retrieve_callback)(const axis2_env_t *, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), void *, axis2_array_list_t *),
    axis2_status_t (*remove_callback)(sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), void *),
    axis2_char_t *sql_stmt_retrieve,
    axis2_char_t *sql_stmt_remove)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_rm_bean_t *bean = NULL;
    sqlite3 *dbconn = NULL;
    int rc = -1;
    sqlite3_stmt *stmt_retrieve = NULL;
    sqlite3_stmt *stmt_remove = NULL;
    axis2_array_list_t *data_list = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
        return AXIS2_FALSE;
    if(sqlite3_prepare(dbconn, sql_stmt_retrieve, AXIS2_STRLEN(sql_stmt_retrieve), 
        &stmt_retrieve, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    data_list = axis2_array_list_create(env, 1);
    rc = retrieve_callback(env, stmt_retrieve, sandesha2_permanent_bean_mgr_busy_handler, key, 
        data_list);
    if(rc != AXIS2_SUCCESS)
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    if(0 < AXIS2_ARRAY_LIST_SIZE(data_list, env))
        bean = (sandesha2_rm_bean_t *) AXIS2_ARRAY_LIST_GET(data_list, env, 0);
    sqlite3_reset(stmt_retrieve);
    if(data_list)
        AXIS2_ARRAY_LIST_FREE(data_list, env);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    if(bean)
    {
        if(sandesha2_rm_bean_get_key(bean, env))
            sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, 
                bean);
    }
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    if(sqlite3_prepare(dbconn, sql_stmt_remove, AXIS2_STRLEN(sql_stmt_remove), 
        &stmt_remove, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_remove);
        return AXIS2_FALSE;
    }
    rc = remove_callback(stmt_remove, sandesha2_permanent_bean_mgr_busy_handler, key);
    if(rc != AXIS2_SUCCESS )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_remove);
        return AXIS2_FALSE;
    }
    sqlite3_reset(stmt_remove);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_retrieve(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    void *key,
    axis2_status_t (*retrieve_callback)(const axis2_env_t *, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), void *, axis2_array_list_t *),
    axis2_char_t *sql_stmt_retrieve)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sandesha2_rm_bean_t *bean = NULL;
    sqlite3 *dbconn = NULL;
    sqlite3_stmt *stmt_retrieve = NULL;
    int rc = -1;
    axis2_array_list_t *data_list = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        return NULL;
    }
    if(sqlite3_prepare(dbconn, sql_stmt_retrieve, AXIS2_STRLEN(sql_stmt_retrieve), 
        &stmt_retrieve, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    data_list = axis2_array_list_create(env, 1);
    rc = retrieve_callback(env, stmt_retrieve, sandesha2_permanent_bean_mgr_busy_handler, key, 
        data_list);
    if(rc != AXIS2_SUCCESS )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    if(0 < AXIS2_ARRAY_LIST_SIZE(data_list, env))
        bean = (sandesha2_rm_bean_t *) AXIS2_ARRAY_LIST_GET(data_list, env, 0);
    sqlite3_reset(stmt_retrieve);
    if(data_list)
        AXIS2_ARRAY_LIST_FREE(data_list, env);
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    if(bean)
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);
    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_update(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    void *key,
    sandesha2_rm_bean_t *bean,
    axis2_status_t (*retrieve_callback)(const axis2_env_t *, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), void *, axis2_array_list_t *),
    axis2_status_t (*update_or_insert_callback)(const axis2_env_t *, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), sandesha2_rm_bean_t *),
    axis2_char_t *sql_stmt_retrieve_old_bean,
    axis2_char_t *sql_stmt_update)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    sqlite3 *dbconn = NULL;
    sandesha2_rm_bean_t *old_bean = NULL;
    sqlite3_stmt *stmt_retrieve_old_bean = NULL;
    sqlite3_stmt *stmt_update = NULL;
    int rc = -1;
    axis2_char_t *key_l = NULL;
    axis2_array_list_t *data_list = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    if(bean)
        sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, bean);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
        return AXIS2_FALSE;
    if(sqlite3_prepare(dbconn, sql_stmt_retrieve_old_bean, AXIS2_STRLEN(sql_stmt_retrieve_old_bean), 
        &stmt_retrieve_old_bean, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve_old_bean);
        return AXIS2_FALSE;
    }
    data_list = axis2_array_list_create(env, 1);
    rc = retrieve_callback(env, stmt_retrieve_old_bean, sandesha2_permanent_bean_mgr_busy_handler, key, 
        data_list);
    if(rc != AXIS2_SUCCESS )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve_old_bean);
        return AXIS2_FALSE;
    }
    sqlite3_reset(stmt_retrieve_old_bean);
    if(sqlite3_prepare(dbconn, sql_stmt_update, AXIS2_STRLEN(sql_stmt_update), 
        &stmt_update, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_update);
        return AXIS2_FALSE;
    }
    rc = update_or_insert_callback(env, stmt_update, sandesha2_permanent_bean_mgr_busy_handler, bean);
    if(rc != AXIS2_SUCCESS)
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_update);
        return AXIS2_FALSE;
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    if(0 < AXIS2_ARRAY_LIST_SIZE(data_list, env))
        old_bean = (sandesha2_rm_bean_t *) AXIS2_ARRAY_LIST_GET(data_list, 
            env, 0);
    if(old_bean)
        key_l = sandesha2_rm_bean_get_key(old_bean, env);
    if(data_list)
        AXIS2_ARRAY_LIST_FREE(data_list, env);
    if(!key_l)
    {
        return AXIS2_FALSE;
    }
    sqlite3_reset(stmt_update);
    sandesha2_storage_mgr_enlist_bean(bean_mgr_impl->storage_mgr, env, old_bean);
    return AXIS2_TRUE;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_find(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    axis2_status_t (*retrieve_callback)(const axis2_env_t *env, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), void *, axis2_array_list_t *),
    int (*count_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_find,
    axis2_char_t *sql_stmt_count)
{
    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = NULL;
    axis2_array_list_t *beans = NULL;
    int i = 0, size = 0, rc = -1;
    sqlite3 *dbconn = NULL;
    sqlite3_stmt *stmt_find = NULL;
    axis2_array_list_t *data_list = NULL;
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
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
        return NULL;
    if(sqlite3_prepare(dbconn, sql_stmt_find, AXIS2_STRLEN(sql_stmt_find), 
        &stmt_find, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_find);
        return AXIS2_FALSE;
    }
    data_list = axis2_array_list_create(env, 0);
    rc = retrieve_callback(env, stmt_find, sandesha2_permanent_bean_mgr_busy_handler, NULL, 
        data_list);
    if(rc != AXIS2_SUCCESS )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_find);
        return NULL;
    }
    sqlite3_reset(stmt_find);
    if(data_list)
        size = AXIS2_ARRAY_LIST_SIZE(data_list, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_rm_bean_t *candidate = NULL;
        candidate = (sandesha2_rm_bean_t *) AXIS2_ARRAY_LIST_GET(data_list, 
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
    if(data_list)
        AXIS2_ARRAY_LIST_FREE(data_list, env);
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

            rc = sqlite3_exec(dbconn, sql_stmt_count, count_func, &count, 
                &error_msg);
            if(rc != SQLITE_OK )
            {
                axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
                AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql error %s", 
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
    axis2_status_t (*retrieve_callback)(const axis2_env_t *, sqlite3_stmt *, int (*busy_handler)(
        sqlite3_stmt*, int), void *, axis2_array_list_t *),
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
    beans = sandesha2_permanent_bean_mgr_find(bean_mgr, env, bean, retrieve_callback, 
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
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;
    int rc = -1;
    sqlite3 *dbconn = NULL;
    sqlite3_stmt *stmt_retrieve = NULL;
    axis2_char_t *sql_stmt_retrieve = NULL;
    axis2_array_list_t *data_list = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
        return NULL;
    sql_stmt_retrieve = axis2_strcat(env, "select stored_key, msg_id, soap_env_str,"\
        "soap_version, transport_out, op, svc, svc_grp, op_mep, to_url, "\
        "transport_to, execution_chain_str, flow, msg_recv_str, svr_side, "\
        "in_msg_store_key, prop_str, action from msg where stored_key = ?", NULL);
    if(sqlite3_prepare(dbconn, sql_stmt_retrieve, AXIS2_STRLEN(sql_stmt_retrieve), 
        &stmt_retrieve, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    data_list = axis2_array_list_create(env, 0);
    rc = sandesha2_msg_retrieve_callback(env, stmt_retrieve, sandesha2_permanent_bean_mgr_busy_handler, key, 
        data_list);
    if(rc != AXIS2_SUCCESS )
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return NULL;
    }
    sqlite3_reset(stmt_retrieve);
    if(0 < AXIS2_ARRAY_LIST_SIZE(data_list, env))
        msg_store_bean = (sandesha2_msg_store_bean_t *) AXIS2_ARRAY_LIST_GET(
            data_list, env, 0);
    if(data_list)
        AXIS2_ARRAY_LIST_FREE(data_list, env);
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
    axis2_array_list_t *data_list = NULL;
    int rc = -1;
    sqlite3 *dbconn = NULL;
    sqlite3_stmt *stmt_retrieve = NULL;
    sqlite3_stmt *stmt_update = NULL;
    sqlite3_stmt *stmt_insert = NULL;
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;

    sandesha2_permanent_bean_mgr_impl_t *bean_mgr_impl = 
        bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    

    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
        return AXIS2_FALSE;

    sql_stmt_retrieve = axis2_strcat(env, "select stored_key, msg_id, soap_env_str,"\
        "soap_version, transport_out, op, svc, svc_grp, op_mep, to_url, reply_to, "\
        "transport_to, execution_chain_str, flow, msg_recv_str, svr_side, "\
        "in_msg_store_key, prop_str, action from msg where stored_key = ?", NULL);
    if(sqlite3_prepare(dbconn, sql_stmt_retrieve, AXIS2_STRLEN(sql_stmt_retrieve), 
        &stmt_retrieve, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    data_list = axis2_array_list_create(env, 0);
    rc = sandesha2_msg_retrieve_callback(env, stmt_retrieve, sandesha2_permanent_bean_mgr_busy_handler, key, 
        data_list);
    if(rc != AXIS2_SUCCESS)
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        if(data_list)
            AXIS2_ARRAY_LIST_FREE(data_list, env);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_retrieve);
        return AXIS2_FALSE;
    }
    sqlite3_reset(stmt_retrieve);
    if(0 < AXIS2_ARRAY_LIST_SIZE(data_list, env))
        msg_store_bean = (sandesha2_msg_store_bean_t *) AXIS2_ARRAY_LIST_GET(
            data_list, env, 0);
    if(data_list)
        AXIS2_ARRAY_LIST_FREE(data_list, env);
    if(msg_store_bean)
    {
        sql_stmt_update = axis2_strcat(env, "update msg set msg_id=?,"\
            "soap_env_str=?,soap_version=?, transport_out=?, op=?,"\
            "svc=?, svc_grp=?, op_mep=?, to_url=?, transport_to=?, reply_to=?,"\
            "execution_chain_str=?, flow=?, msg_recv_str=?, svr_side=?,"\
            "in_msg_store_key=?, prop_str=?, action=?"\
            " where stored_key=?", NULL);
        if(sqlite3_prepare(dbconn, sql_stmt_update, AXIS2_STRLEN(sql_stmt_update), 
            &stmt_update, NULL))
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
                sql_stmt_update);
            return AXIS2_FALSE;
        }
        rc = sandesha2_msg_update_or_insert_callback(env, stmt_update, 
            sandesha2_permanent_bean_mgr_busy_handler, bean);
        if(rc != AXIS2_SUCCESS)
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
                sql_stmt_update);
            return AXIS2_FALSE;
        }
        sqlite3_reset(stmt_update);
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        return AXIS2_TRUE;
    }
    sql_stmt_insert = axis2_strcat(env, "insert into msg(msg_id, "\
        "soap_env_str, soap_version, transport_out, op, svc, svc_grp, op_mep,"\
        "to_url, reply_to,transport_to, execution_chain_str, flow, msg_recv_str, "\
        "svr_side, in_msg_store_key, prop_str, action, stored_key) "\
        "values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", NULL);
    if(sqlite3_prepare(dbconn, sql_stmt_insert, AXIS2_STRLEN(sql_stmt_insert), 
        &stmt_insert, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_insert);
        return AXIS2_FALSE;
    }
    rc = sandesha2_msg_update_or_insert_callback(env, stmt_insert, 
        sandesha2_permanent_bean_mgr_busy_handler, bean);
    if(rc != AXIS2_SUCCESS)
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_insert);
        return AXIS2_FALSE;
    }
    sqlite3_reset(stmt_insert);
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
    int rc = -1;
    sqlite3 *dbconn = NULL;
    sqlite3_stmt *stmt_remove = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    bean_mgr_impl = SANDESHA2_INTF_TO_IMPL(bean_mgr);
    axis2_thread_mutex_lock(bean_mgr_impl->mutex);
    dbconn = (sqlite3 *) sandesha2_permanent_storage_mgr_get_dbconn(
        bean_mgr_impl->storage_mgr, env);
    if(!dbconn)
        return AXIS2_FALSE;
    sql_stmt_remove = "delete from msg where stored_key=?";
    if(sqlite3_prepare(dbconn, sql_stmt_remove, axis2_strlen(sql_stmt_remove), 
        &stmt_remove, NULL))
    {
        axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_remove);
        return AXIS2_FALSE;
    }
    if (sqlite3_bind_text(stmt_remove, 1, key, axis2_strlen(key), SQLITE_STATIC))
    {
        sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
            sql_stmt_remove);
    }
    rc = sqlite3_step(stmt_remove);
    if (rc == SQLITE_BUSY)
    {
        if(sandesha2_permanent_bean_mgr_busy_handler(stmt_remove, rc) != 
            SQLITE_OK)
        {
            axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
            sandesha2_permanent_bean_mgr_handle_sql_error(env, dbconn, 
                sql_stmt_remove);
            return AXIS2_FALSE;
        }
    }
    axis2_thread_mutex_unlock(bean_mgr_impl->mutex);
    return AXIS2_TRUE;
}

static int
sandesha2_permanent_bean_mgr_busy_handler(
    sqlite3_stmt* stmt,
    int rc)
{
    int counter = 0;
    while(rc == SQLITE_BUSY && counter < 8)
    {
        counter++;
        AXIS2_SLEEP(SANDESHA2_BUSY_WAIT_TIME);
        rc = sqlite3_step(stmt);
    }
    return rc;
}

static void 
sandesha2_permanent_bean_mgr_handle_sql_error(
    const axis2_env_t *env,
    sqlite3 *dbconn,
    axis2_char_t *sql_stmt)
{
    const axis2_char_t *error_msg = sqlite3_errmsg(dbconn);
    AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_SQL_ERROR, AXIS2_FAILURE);
    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sql stmt:%s. sql error: %s",
        sql_stmt, error_msg);
    printf("sql_stmt:%s\n", sql_stmt);
    printf("sql error_msg:%s\n", error_msg);
}

