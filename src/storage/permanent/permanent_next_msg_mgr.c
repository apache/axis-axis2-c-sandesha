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
 
#include <sandesha2_permanent_next_msg_mgr.h>
#include <sandesha2_permanent_bean_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <sandesha2_storage_mgr.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <axis2_types.h>

/** 
 * @brief Sandesha2 Permanent Next Message Manager Struct Impl
 *   Sandesha Sequence2 Permanent Next Message Manager 
 */ 
typedef struct sandesha2_permanent_next_msg_mgr
{
    sandesha2_next_msg_mgr_t next_msg_mgr;
    sandesha2_permanent_bean_mgr_t *bean_mgr;
    axis2_array_list_t *values;
}sandesha2_permanent_next_msg_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(next_msg_mgr) \
    ((sandesha2_permanent_next_msg_mgr_t *) next_msg_mgr)

axis2_status_t 
sandesha2_next_msg_retrieve_callback(
    const axis2_env_t *env,
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    void *key,
    axis2_array_list_t *data_list)
{
    int rc = -1;
    axis2_char_t *seq_id = (axis2_char_t *) key;
    if(seq_id)
    {
        if (sqlite3_bind_text(stmt, 1, seq_id, 
            axis2_strlen(seq_id), SQLITE_STATIC))
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
        sandesha2_next_msg_bean_t *bean = (sandesha2_next_msg_bean_t *) 
            sandesha2_next_msg_bean_create(env);
        sandesha2_next_msg_bean_set_seq_id(bean, env, 
            (axis2_char_t *) sqlite3_column_text(stmt, 0));
        sandesha2_next_msg_bean_set_ref_msg_key(bean, env,
            (axis2_char_t *) sqlite3_column_text(stmt, 1));
        sandesha2_next_msg_bean_set_polling_mode(bean, env,
            sqlite3_column_int(stmt, 2));
        sandesha2_next_msg_bean_set_next_msg_no_to_process(bean, env,
            sqlite3_column_int64(stmt, 3));
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
sandesha2_next_msg_remove_callback(
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    void *key)
{
    int rc = -1;
    axis2_char_t *seq_id = (axis2_char_t *) key;
    if (sqlite3_bind_text(stmt, 1, seq_id, 
        axis2_strlen(seq_id), SQLITE_STATIC))
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
sandesha2_next_msg_update_or_insert_callback(
    const axis2_env_t *env,
    sqlite3_stmt *stmt,
    int (*busy_handler)(sqlite3_stmt* stmt, int rc),
    sandesha2_rm_bean_t *next_msg_bean)
{
    sandesha2_next_msg_bean_t *bean = (sandesha2_next_msg_bean_t *) next_msg_bean;
    if(!bean)
        return AXIS2_FAILURE;
    {
        int rc = -1;
        axis2_char_t *seq_id = sandesha2_next_msg_bean_get_seq_id((sandesha2_rm_bean_t *) bean, 
            env);
        axis2_char_t *ref_msg_key = sandesha2_next_msg_bean_get_ref_msg_key(bean, env);
        axis2_bool_t polling_mode = sandesha2_next_msg_bean_is_polling_mode(bean, env);
        long msg_no = sandesha2_next_msg_bean_get_next_msg_no_to_process(bean, env);
          
        if (sqlite3_bind_text(stmt, 1, ref_msg_key, 
            axis2_strlen(ref_msg_key), SQLITE_STATIC))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_int(stmt, 2, polling_mode))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_int64(stmt, 3, msg_no))
        {
            return AXIS2_FAILURE;
        }
        if (sqlite3_bind_text(stmt, 4, seq_id, axis2_strlen(seq_id), SQLITE_STATIC))
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
sandesha2_next_msg_count_callback(
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
sandesha2_permanent_next_msg_mgr_free(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_insert(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_remove(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id);

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_retrieve(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_update(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_find(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_find_unique(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_match(
    sandesha2_permanent_bean_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_retrieve_all(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env);

static const sandesha2_next_msg_mgr_ops_t next_msg_mgr_ops = 
{
    sandesha2_permanent_next_msg_mgr_free,
    sandesha2_permanent_next_msg_mgr_insert,
    sandesha2_permanent_next_msg_mgr_remove,
    sandesha2_permanent_next_msg_mgr_retrieve,
    sandesha2_permanent_next_msg_mgr_update,
    sandesha2_permanent_next_msg_mgr_find,
    sandesha2_permanent_next_msg_mgr_find_unique,
    sandesha2_permanent_next_msg_mgr_retrieve_all
};

AXIS2_EXTERN sandesha2_next_msg_mgr_t * AXIS2_CALL
sandesha2_permanent_next_msg_mgr_create(
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_conf_ctx_t *ctx)
{
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    next_msg_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_next_msg_mgr_t));

    next_msg_mgr_impl->values = axis2_array_list_create(env, 0);
    if(!next_msg_mgr_impl->values)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    next_msg_mgr_impl->bean_mgr = sandesha2_permanent_bean_mgr_create(env,
        storage_mgr, ctx, SANDESHA2_BEAN_MAP_NEXT_MESSAGE);
    next_msg_mgr_impl->bean_mgr->ops.match = sandesha2_permanent_next_msg_mgr_match;
    next_msg_mgr_impl->next_msg_mgr.ops = next_msg_mgr_ops;

    return &(next_msg_mgr_impl->next_msg_mgr);
}

void AXIS2_CALL
sandesha2_permanent_next_msg_mgr_free(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);

    if(next_msg_mgr_impl->bean_mgr)
    {
        sandesha2_permanent_bean_mgr_free(next_msg_mgr_impl->bean_mgr, env);
        next_msg_mgr_impl->bean_mgr = NULL;
    }
    if(next_msg_mgr_impl->values)
    {
        AXIS2_ARRAY_LIST_FREE(next_msg_mgr_impl->values, env);
        next_msg_mgr_impl->values = NULL;
    }
    if(next_msg_mgr_impl)
    {
        AXIS2_FREE(env->allocator, next_msg_mgr_impl);
        next_msg_mgr_impl = NULL;
    }
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_insert(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean)
{
    axis2_char_t *sql_insert = NULL;
    axis2_char_t *sql_retrieve = NULL;
    axis2_char_t *sql_update = NULL;
    axis2_char_t *seq_id = NULL; 
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);

    seq_id = sandesha2_next_msg_bean_get_seq_id((sandesha2_rm_bean_t *) bean, 
        env);
    sql_retrieve = "select seq_id, ref_msg_key, polling_mode, msg_no from"\
        " next_msg where seq_id=?";
    sql_update = "update next_msg set ref_msg_key=?, polling_mode=?, msg_no=?"\
        " where seq_id=?";
    sql_insert = "insert into next_msg(ref_msg_key, polling_mode,"\
        "msg_no, seq_id) values(?,?,?,?)";
    return sandesha2_permanent_bean_mgr_insert(next_msg_mgr_impl->bean_mgr, env,
        (void *) seq_id, (sandesha2_rm_bean_t *) bean, 
        sandesha2_next_msg_retrieve_callback, 
        sandesha2_next_msg_update_or_insert_callback, 
        sql_retrieve, sql_update, sql_insert);
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_remove(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id)
{
    axis2_char_t *sql_remove = NULL;
    axis2_char_t *sql_retrieve = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    sql_remove = axis2_strcat(env, "delete from next_msg where seq_id='",
        seq_id, "';", NULL);
    sql_retrieve = "select seq_id, ref_msg_key, "\
        "polling_mode, msg_no from next_msg "\
        "where seq_id=?";
    return sandesha2_permanent_bean_mgr_remove(next_msg_mgr_impl->bean_mgr, env, 
        (void *) seq_id, 
        sandesha2_next_msg_retrieve_callback, 
        sandesha2_next_msg_remove_callback, 
        sql_retrieve, sql_remove);
}

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_retrieve(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id)
{
    axis2_char_t *sql_retrieve = NULL;
    sandesha2_next_msg_bean_t *bean = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    sql_retrieve = "select seq_id, ref_msg_key, "\
        "polling_mode, msg_no from next_msg "\
        "where seq_id=?";
    bean = (sandesha2_next_msg_bean_t *) sandesha2_permanent_bean_mgr_retrieve(
        next_msg_mgr_impl->bean_mgr, env, (void *) seq_id,
        sandesha2_next_msg_retrieve_callback, sql_retrieve);
    return bean;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_update(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean)
{
    axis2_char_t *sql_update = NULL;
    axis2_char_t *sql_retrieve = NULL;
    axis2_bool_t ret = AXIS2_FALSE;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;

    axis2_char_t *seq_id = sandesha2_next_msg_bean_get_seq_id((sandesha2_rm_bean_t *) bean, 
        env);

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);

    sql_retrieve = "select seq_id, ref_msg_key, "\
        "polling_mode, msg_no from next_msg "\
        "where seq_id=?";
    sql_update = "update next_msg set ref_msg_key=?, polling_mode=?, msg_no=?,"\
        " where seq_id=?";
    ret = sandesha2_permanent_bean_mgr_update(next_msg_mgr_impl->bean_mgr, env, 
        (void *) seq_id, (sandesha2_rm_bean_t *) bean, 
        sandesha2_next_msg_retrieve_callback, 
        sandesha2_next_msg_update_or_insert_callback, 
        sql_retrieve, sql_update);
    return ret;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_find(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    axis2_array_list_t *ret = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    sql_find = "select seq_id,ref_msg_key,"\
        "polling_mode,msg_no from next_msg;";
    sql_count = "select count(*) as no_recs from next_msg;";
    ret = sandesha2_permanent_bean_mgr_find(next_msg_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_next_msg_retrieve_callback,
        sandesha2_next_msg_count_callback, sql_find, sql_count);
    return ret;
}

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_find_unique(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_next_msg_bean_t *ret = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    sql_find = "select seq_id,ref_msg_key,"\
        "polling_mode,msg_no from next_msg;";
    sql_count = "select count(*) as no_recs from next_msg;";
    ret = (sandesha2_next_msg_bean_t *) sandesha2_permanent_bean_mgr_find_unique(
        next_msg_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean, 
        sandesha2_next_msg_retrieve_callback, sandesha2_next_msg_count_callback, 
        sql_find, sql_count);
    return ret;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_next_msg_mgr_retrieve_all(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env)
{
    axis2_char_t *sql_find = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    sql_find = axis2_strcat(env, "select seq_id,ref_msg_key,"\
        "polling_mode,msg_no from next_msg;", NULL);
    return (axis2_array_list_t *) sandesha2_next_msg_mgr_find(
        next_msg_mgr, env, NULL);
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_match(
    sandesha2_permanent_bean_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate)
{
    axis2_bool_t equal = AXIS2_TRUE;
    long next_msg_no = 0;
    long temp_next_msg_no = 0;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *temp_seq_id = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_next_msg_mgr_match");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    next_msg_no = sandesha2_next_msg_bean_get_next_msg_no_to_process(
        (sandesha2_next_msg_bean_t *) bean, env);
    temp_next_msg_no = sandesha2_next_msg_bean_get_next_msg_no_to_process(
        (sandesha2_next_msg_bean_t *) candidate, env);
    if(next_msg_no > 0 && (next_msg_no != temp_next_msg_no))
    {
        equal = AXIS2_FALSE;
    }
    seq_id = sandesha2_next_msg_bean_get_seq_id(bean, env);
    temp_seq_id = sandesha2_next_msg_bean_get_seq_id(candidate, env);
    if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
    {
        equal = AXIS2_FALSE;
    }
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_next_msg_mgr_match:equal:%d", 
            equal);
    return equal;
}

