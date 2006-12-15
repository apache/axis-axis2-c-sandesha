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

static int 
sandesha2_next_msg_find_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int i;
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    axis2_array_list_t *data_list = (axis2_array_list_t *) args->data;
    const axis2_env_t *env = args->env;
    for(i = 0; i < argc; i++)
    {
        sandesha2_next_msg_bean_t *bean = NULL;
        if(0 == AXIS2_STRCMP(col_name[i], "seq_id"))
            sandesha2_next_msg_bean_set_seq_id(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "ref_msg_key"))
            sandesha2_next_msg_bean_set_ref_msg_key(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "polling_mode"))
            sandesha2_next_msg_bean_set_polling_mode(bean, env, 
                argv[i] ? AXIS2_ATOI(argv[i]) : 0);
        if(0 == AXIS2_STRCMP(col_name[i], "msg_no"))
            sandesha2_next_msg_bean_set_next_msg_no_to_process(bean, env, 
                argv[i] ? atol(argv[i]) : 0);
        axis2_array_list_add(data_list, env, bean);
    }
    return 0;
}

static int 
sandesha2_next_msg_retrieve_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    const axis2_env_t *env = args->env;
    sandesha2_next_msg_bean_t *bean = (sandesha2_next_msg_bean_t *) args->data;
    sandesha2_next_msg_bean_set_seq_id(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_next_msg_bean_set_ref_msg_key(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_next_msg_bean_set_polling_mode(bean, env, 
        argv[0] ? AXIS2_ATOI(argv[0]) : 0);
    sandesha2_next_msg_bean_set_next_msg_no_to_process(bean, env, 
        argv[0] ? atol(argv[0]) : 0);
    return 0;
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
    axis2_char_t *insert_sql = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *ref_msg_key = NULL;
    axis2_bool_t polling_mode = AXIS2_FALSE;
    long msg_no = -1;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);

    seq_id = sandesha2_next_msg_bean_get_seq_id((sandesha2_rm_bean_t *) bean, 
        env);
    ref_msg_key = sandesha2_next_msg_bean_get_ref_msg_key(bean, env);
    polling_mode = sandesha2_next_msg_bean_is_polling_mode(bean, env);
    msg_no = sandesha2_next_msg_bean_get_next_msg_no_to_process(bean, env);
    insert_sql = axis2_strcat(env, "insert into next_msg(seq_id, "\
        "ref_msg_key, polling_mode, msg_no) values('", seq_id, 
        "','", ref_msg_key, "','", polling_mode, "','", msg_no, "');", NULL);
    return sandesha2_permanent_bean_mgr_insert(next_msg_mgr_impl->bean_mgr, env,
        insert_sql);
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_next_msg_mgr_remove(
    sandesha2_next_msg_mgr_t *next_msg_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id)
{
    axis2_char_t *sql_remove = NULL;
    axis2_char_t *sql_retrieve = NULL;
    sandesha2_next_msg_bean_t *bean = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    sql_remove = axis2_strcat(env, "delete from next_msg where seq_id='",
        seq_id, "';", NULL);
    sql_retrieve = axis2_strcat(env, "select seq_id, ref_msg_key, "\
        "polling_mode, msg_no from next_msg "\
        "where seq_id='", seq_id, "';", NULL);
    bean = sandesha2_next_msg_bean_create(env);
    return sandesha2_permanent_bean_mgr_remove(next_msg_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_next_msg_retrieve_callback, 
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
    sql_retrieve = axis2_strcat(env, "select seq_id, ref_msg_key, "\
        "polling_mode, msg_no from next_msg "\
        "where seq_id='", seq_id, "';", NULL);
    bean = sandesha2_next_msg_bean_create(env);
    sandesha2_permanent_bean_mgr_retrieve(
        next_msg_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean,
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
    axis2_char_t *seq_id = NULL;
    axis2_char_t *ref_msg_key = NULL;
    axis2_bool_t polling_mode = AXIS2_FALSE;
    long msg_no = -1;
    axis2_bool_t ret = AXIS2_FALSE;
    sandesha2_next_msg_bean_t *old_bean = NULL;
    sandesha2_permanent_next_msg_mgr_t *next_msg_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    next_msg_mgr_impl = SANDESHA2_INTF_TO_IMPL(next_msg_mgr);
    seq_id = sandesha2_next_msg_bean_get_seq_id((sandesha2_rm_bean_t *) bean, 
        env);
    ref_msg_key = sandesha2_next_msg_bean_get_ref_msg_key(bean, env);
    polling_mode = sandesha2_next_msg_bean_is_polling_mode(bean, env);
    msg_no = sandesha2_next_msg_bean_get_next_msg_no_to_process(bean, env);
    sql_retrieve = axis2_strcat(env, "select seq_id, ref_msg_key, "\
        "polling_mode, msg_no from next_msg "\
        "where seq_id='", seq_id, "';", NULL);
    sql_update = axis2_strcat(env, "update next_msg set ref_msg_key='", 
        ref_msg_key, "', polling_mode='", polling_mode, "', msg_no='", 
            msg_no, "' where seq_id='", seq_id, "';", NULL);
    old_bean = sandesha2_next_msg_bean_create(env);
    ret = sandesha2_permanent_bean_mgr_update(next_msg_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) old_bean, sandesha2_next_msg_retrieve_callback, 
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
    sql_find = axis2_strcat(env, "select seq_id,ref_msg_key,"\
        "polling_mode,msg_no from next_msg;", NULL);
    sql_count = "select count(*) as no_recs from next_msg;";
    ret = sandesha2_permanent_bean_mgr_find(next_msg_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_next_msg_find_callback,
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
    sql_find = axis2_strcat(env, "select seq_id,ref_msg_key,"\
        "polling_mode,msg_no from next_msg;", NULL);
    sql_count = "select count(*) as no_recs from next_msg;";
    ret = (sandesha2_next_msg_bean_t *) sandesha2_permanent_bean_mgr_find_unique(
        next_msg_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean, 
        sandesha2_next_msg_find_callback, sandesha2_next_msg_count_callback, 
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

