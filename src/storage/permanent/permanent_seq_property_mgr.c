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
#include <sandesha2_permanent_seq_property_mgr.h>
#include "sandesha2_permanent_bean_mgr.h"
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_rm_bean.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <axis2_types.h>

/** 
 * @brief Sandesha Permanent Sequence Property Manager Struct Impl
 *   Sandesha Permanent Sequence Property Manager 
 */ 
typedef struct sandesha2_permanent_seq_property_mgr
{
    sandesha2_seq_property_mgr_t seq_prop_mgr;
    sandesha2_permanent_bean_mgr_t *bean_mgr;
    axis2_array_list_t *values;
} sandesha2_permanent_seq_property_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(seq_property_mgr) \
    ((sandesha2_permanent_seq_property_mgr_t *) seq_property_mgr)

static axis2_char_t *
sandesha2_permanent_seq_property_mgr_get_id_with_bean(
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

axis2_status_t 
sandesha2_seq_property_retrieve_callback(
    const axis2_env_t *env,
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    void *key,
    axis2_array_list_t *data_list)
{
    int rc = -1;
    axis2_char_t *id = (axis2_char_t *) key;
    if(id)
    {
        if (sqlite3_bind_text(stmt, 1, id, 
            axis2_strlen(id), SQLITE_STATIC))
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
    while(rc == SQLITE_ROW)
    {
        sandesha2_seq_property_bean_t *bean = (sandesha2_seq_property_bean_t *) 
            sandesha2_seq_property_bean_create(env);
        sandesha2_seq_property_bean_set_seq_id(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 0));
        sandesha2_seq_property_bean_set_name(bean, env,
            (axis2_char_t *) sqlite3_column_text(stmt, 1));
        sandesha2_seq_property_bean_set_value(bean, env,
            (axis2_char_t *) sqlite3_column_text(stmt, 2));
        rc = sqlite3_step(stmt);
        if(rc == SQLITE_BUSY)
        {
            if(busy_handler(stmt, rc) != SQLITE_OK)
            {
                return AXIS2_FAILURE;
            }
        }
        axis2_array_list_add(data_list, env, bean);
    }
    return AXIS2_SUCCESS;
}

static axis2_status_t
sandesha2_seq_property_remove_callback(
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    void *key)
{
    int rc = -1;
    axis2_char_t *id = (axis2_char_t *) key;
    if (sqlite3_bind_text(stmt, 1, id, 
        axis2_strlen(id), SQLITE_STATIC))
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
    return AXIS2_FAILURE;
}

static axis2_status_t 
sandesha2_seq_property_update_or_insert_callback(
    const axis2_env_t *env,
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    sandesha2_rm_bean_t *seq_prop_bean)
{
    sandesha2_seq_property_bean_t *bean = (sandesha2_seq_property_bean_t *) 
        seq_prop_bean;
    if(!bean)
        return AXIS2_FAILURE;
    {
        int rc = -1;
        axis2_char_t *id = sandesha2_permanent_seq_property_mgr_get_id_with_bean(env, bean);
        axis2_char_t *seq_id = sandesha2_seq_property_bean_get_seq_id(bean, env);
        axis2_char_t *name = sandesha2_seq_property_bean_get_name(bean, env);
        axis2_char_t *value = sandesha2_seq_property_bean_get_value(bean, env);
          
        if (sqlite3_bind_text(stmt, 1, seq_id, axis2_strlen(
            seq_id), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 2, name, axis2_strlen(name), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 3, value, axis2_strlen(value), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 4, id, axis2_strlen(id), SQLITE_STATIC))
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

static int 
sandesha2_seq_property_count_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int *count = (int *) not_used;
    *count = AXIS2_ATOI(argv[0]);
    return 0;
}


void AXIS2_CALL
sandesha2_permanent_seq_property_mgr_free(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_insert(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_remove(
        sandesha2_seq_property_mgr_t *seq_prop_mgr,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name);

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_retrieve(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axis2_char_t *name);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_update(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_find(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_find_unique(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean);

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_match(
    sandesha2_permanent_bean_mgr_t *seq_prop,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_retrieve_all(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env);

static axis2_char_t *
sandesha2_permanent_seq_property_mgr_get_id_with_seq_id_and_name(
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axis2_char_t *name);

static const sandesha2_seq_property_mgr_ops_t seq_property_mgr_ops = 
{
    sandesha2_permanent_seq_property_mgr_free,
    sandesha2_permanent_seq_property_mgr_insert,
    sandesha2_permanent_seq_property_mgr_remove,
    sandesha2_permanent_seq_property_mgr_retrieve,
    sandesha2_permanent_seq_property_mgr_update,
    sandesha2_permanent_seq_property_mgr_find,
    sandesha2_permanent_seq_property_mgr_find_unique,
    sandesha2_permanent_seq_property_mgr_retrieve_all,
};

AXIS2_EXTERN sandesha2_seq_property_mgr_t * AXIS2_CALL
sandesha2_permanent_seq_property_mgr_create(
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_conf_ctx_t *ctx)
{
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    seq_prop_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_seq_property_mgr_t));

    seq_prop_mgr_impl->values = NULL;
    seq_prop_mgr_impl->bean_mgr = sandesha2_permanent_bean_mgr_create(env,
        storage_mgr, ctx, SANDESHA2_BEAN_MAP_SEQ_PROPERTY);
    seq_prop_mgr_impl->bean_mgr->ops.match = sandesha2_permanent_seq_property_mgr_match;
    seq_prop_mgr_impl->seq_prop_mgr.ops = seq_property_mgr_ops;
    return &(seq_prop_mgr_impl->seq_prop_mgr);
}

void AXIS2_CALL
sandesha2_permanent_seq_property_mgr_free(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    if(seq_prop_mgr_impl->values)
    {
        AXIS2_ARRAY_LIST_FREE(seq_prop_mgr_impl->values, env);
        seq_prop_mgr_impl->values = NULL;
    }
    if(seq_prop_mgr_impl->bean_mgr)
    {
        sandesha2_permanent_bean_mgr_free(seq_prop_mgr_impl->bean_mgr, env);
        seq_prop_mgr_impl->bean_mgr = NULL;
    }
    if(seq_prop_mgr_impl)
    {
        AXIS2_FREE(env->allocator, seq_prop_mgr_impl);
        seq_prop_mgr_impl = NULL;
    }
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_insert(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *sql_insert = NULL;
    axis2_char_t *sql_update = NULL;
    axis2_char_t *sql_retrieve = NULL;
    axis2_bool_t ret = AXIS2_FALSE;
    axis2_char_t *id = NULL; 
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_LOG_INFO(env->log, 
        "Entry:[sandesha2]sandesha2_permanent_seq_property_mgr_insert");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    id = sandesha2_permanent_seq_property_mgr_get_id_with_bean(env, bean);
    sql_retrieve = "select seq_id, name, value from seq_property "\
        "where id=?";
    sql_update = "update seq_property set seq_id=?, name=?, value=? where id=?";
    sql_insert = "insert into seq_property(seq_id, name, value, id) values(?,?,?,?)";
    ret = sandesha2_permanent_bean_mgr_insert(seq_prop_mgr_impl->bean_mgr, env, 
        (void *) id, (sandesha2_rm_bean_t *) bean,  
        sandesha2_seq_property_retrieve_callback, 
        sandesha2_seq_property_update_or_insert_callback, 
        sql_retrieve, sql_update, sql_insert);
    AXIS2_LOG_INFO(env->log, 
        "Exit:[sandesha2]sandesha2_permanent_seq_property_mgr_insert:return%d", 
            ret);
    return ret;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_remove(
        sandesha2_seq_property_mgr_t *seq_prop_mgr,
        const axis2_env_t *env,
        axis2_char_t *seq_id,
        axis2_char_t *name)
{
    axis2_char_t *key = NULL;
    axis2_char_t *sql_remove = NULL;
    axis2_char_t *sql_retrieve = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    key = sandesha2_permanent_seq_property_mgr_get_id_with_seq_id_and_name(env, 
        seq_id, name);
    sql_remove = "delete from seq_property where id=?";
    sql_retrieve = "select seq_id, name, value from seq_property "\
        "where id=?";
    status = sandesha2_permanent_bean_mgr_remove(seq_prop_mgr_impl->bean_mgr, 
        env, (void *) key, sandesha2_seq_property_retrieve_callback, 
        sandesha2_seq_property_remove_callback, 
        sql_retrieve, sql_remove);
    AXIS2_FREE(env->allocator, key);
    return status;
}

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_retrieve(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axis2_char_t *name)
{
    axis2_char_t *key = NULL;
    axis2_char_t *sql_retrieve = NULL;
    sandesha2_seq_property_bean_t *ret = NULL;
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_seq_property_mgr_retrieve");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    key = sandesha2_permanent_seq_property_mgr_get_id_with_seq_id_and_name(env, 
        seq_id, name);
    sql_retrieve = "select seq_id, name, value from seq_property "\
        "where id=?";
    ret = (sandesha2_seq_property_bean_t *) sandesha2_permanent_bean_mgr_retrieve(
        seq_prop_mgr_impl->bean_mgr, env, (void *) key, 
        sandesha2_seq_property_retrieve_callback, sql_retrieve);
    AXIS2_FREE(env->allocator, key);
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_seq_property_mgr_retrieve");
    return ret;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_update(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *sql_update = NULL;
    axis2_char_t *sql_retrieve = NULL;
    axis2_bool_t ret = AXIS2_FALSE;
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;

    axis2_char_t *id = sandesha2_permanent_seq_property_mgr_get_id_with_bean(env, bean);

    AXIS2_LOG_INFO(env->log, 
        "Entry:[sandesha2]sandesha2_permanent_seq_property_mgr_update");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);

    sql_retrieve = "select seq_id, name, value from seq_property where id=?";
    sql_update = "update seq_property set seq_id=?,name=?,value=? where id=?";
    ret = sandesha2_permanent_bean_mgr_update(seq_prop_mgr_impl->bean_mgr, env, 
        (void *)id, (sandesha2_rm_bean_t *) bean, 
        sandesha2_seq_property_retrieve_callback, 
        sandesha2_seq_property_update_or_insert_callback, 
        sql_retrieve, sql_update);
    AXIS2_LOG_INFO(env->log, 
        "Exit:[sandesha2]sandesha2_permanent_seq_property_mgr_update:return:%d", ret);
    return ret;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_find(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;
    axis2_array_list_t *ret = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_seq_property_mgr_find");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    sql_find = "select seq_id, name,value from seq_property;";
    sql_count = "select count(*) as no_recs from seq_property;";
    ret = sandesha2_permanent_bean_mgr_find(seq_prop_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_seq_property_retrieve_callback,
        sandesha2_seq_property_count_callback, sql_find, sql_count);
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_seq_property_mgr_find");
    return ret;
}

sandesha2_seq_property_bean_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_find_unique(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    sql_find = "select seq_id, name,value from seq_property;";
    sql_count = "select count(*) as no_recs from seq_property;";
    return (sandesha2_seq_property_bean_t *) 
        sandesha2_permanent_bean_mgr_find_unique(seq_prop_mgr_impl->bean_mgr, 
        env, (sandesha2_rm_bean_t *) bean, sandesha2_seq_property_retrieve_callback,
        sandesha2_seq_property_count_callback, sql_find, sql_count);
}

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_seq_property_mgr_match(
    sandesha2_permanent_bean_mgr_t *seq_prop_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate)
{
    axis2_bool_t equal = AXIS2_TRUE;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *temp_seq_id = NULL;
    axis2_char_t *name = NULL;
    axis2_char_t *temp_name = NULL;
    axis2_char_t *value = NULL;
    axis2_char_t *temp_value = NULL;
    
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_seq_property_mgr_match");
    seq_id = sandesha2_seq_property_bean_get_seq_id(
        (sandesha2_seq_property_bean_t *) bean, env);
    temp_seq_id = sandesha2_seq_property_bean_get_seq_id(
        (sandesha2_seq_property_bean_t *) candidate, env);
    if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
    {
        equal = AXIS2_FALSE;
    }
    name = sandesha2_seq_property_bean_get_name(
        (sandesha2_seq_property_bean_t *) bean, env);
    temp_name = sandesha2_seq_property_bean_get_name(
        (sandesha2_seq_property_bean_t *) candidate, env);
    if(name && temp_name && 0 != AXIS2_STRCMP(name, temp_name))
    {
        equal = AXIS2_FALSE;
    }
    value = sandesha2_seq_property_bean_get_value(
        (sandesha2_seq_property_bean_t *) bean, env);
    temp_value = sandesha2_seq_property_bean_get_value(
        (sandesha2_seq_property_bean_t *) candidate, env);
    if(value && temp_value && 0 != AXIS2_STRCMP(value, temp_value))
    {
        equal = AXIS2_FALSE;
    }
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_seq_property_mgr_match:equal:%d", 
            equal);
    return equal;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_seq_property_mgr_retrieve_all(
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_seq_property_mgr_t *seq_prop_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_prop_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_prop_mgr);
    return sandesha2_seq_property_mgr_find(seq_prop_mgr, env, NULL);
}

static axis2_char_t *
sandesha2_permanent_seq_property_mgr_get_id_with_seq_id_and_name(
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axis2_char_t *name)
{
    axis2_char_t *id = NULL;

    id = axis2_strcat(env, seq_id, ":", name, NULL);
    return id;
}

static axis2_char_t *
sandesha2_permanent_seq_property_mgr_get_id_with_bean(
    const axis2_env_t *env,
    sandesha2_seq_property_bean_t *bean)
{
    axis2_char_t *seq_id = NULL;
    axis2_char_t *name = NULL;
    axis2_char_t *id = NULL;

    seq_id = sandesha2_seq_property_bean_get_seq_id(bean, env);
    name = sandesha2_seq_property_bean_get_name(bean, env);
    id = axis2_strcat(env, seq_id, ":", name, NULL);

    return id;
}

