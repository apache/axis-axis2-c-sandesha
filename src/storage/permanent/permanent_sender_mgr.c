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
 
#include <sandesha2_permanent_sender_mgr.h>
#include <sandesha2_permanent_bean_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <sandesha2_sender_bean.h>
#include <sandesha2_storage_mgr.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <axis2_types.h>

/** 
 * @brief Sandesha2 Permanent Sender Manager Struct Impl
 *   Sandesha2 Permanent Sender Manager 
 */ 
typedef struct sandesha2_permanent_sender_mgr
{
    sandesha2_sender_mgr_t sender_mgr;
    sandesha2_permanent_bean_mgr_t *bean_mgr;
} sandesha2_permanent_sender_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(sender_mgr) \
    ((sandesha2_permanent_sender_mgr_t *) sender_mgr)

static int 
sandesha2_sender_find_callback(
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
        sandesha2_sender_bean_t *bean = NULL;
        if(0 == AXIS2_STRCMP(col_name[i], "msg_id"))
            sandesha2_sender_bean_set_msg_id(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "msg_ctx_ref_key"))
            sandesha2_sender_bean_set_msg_ctx_ref_key(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "internal_seq_ID"))
            sandesha2_sender_bean_set_internal_seq_id(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "sent_count"))
            sandesha2_sender_bean_set_sent_count(bean, env, 
                argv[i] ? AXIS2_ATOI(argv[i]) : 0);
        if(0 == AXIS2_STRCMP(col_name[i], "msg_no"))
            sandesha2_sender_bean_set_msg_no(bean, env, 
                argv[i] ? atol(argv[i]) : 0);
        if(0 == AXIS2_STRCMP(col_name[i], "send"))
            sandesha2_sender_bean_set_send(bean, env, 
                argv[i] ? AXIS2_ATOI(argv[i]) : 0);
        if(0 == AXIS2_STRCMP(col_name[i], "resend"))
            sandesha2_sender_bean_set_resend(bean, env, 
                argv[i] ? AXIS2_ATOI(argv[i]) : 0);
        if(0 == AXIS2_STRCMP(col_name[i], "time_to_send"))
            sandesha2_sender_bean_set_time_to_send(bean, env, 
                argv[i] ? atol(argv[i]) : 0);
        if(0 == AXIS2_STRCMP(col_name[i], "msg_type"))
            sandesha2_sender_bean_set_msg_type(bean, env, 
                argv[i] ? AXIS2_ATOI(argv[i]) : 0);
        if(0 == AXIS2_STRCMP(col_name[i], "seq_id"))
            sandesha2_sender_bean_set_seq_id(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "wsrm_anon_uri"))
            sandesha2_sender_bean_set_wsrm_anon_uri(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "to_address"))
            sandesha2_sender_bean_set_to_address(bean, env, 
                argv[i] ? argv[i] : "NULL");
        axis2_array_list_add(data_list, env, bean);
    }
    return 0;
}

static int 
sandesha2_sender_retrieve_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    const axis2_env_t *env = args->env;
    sandesha2_sender_bean_t *bean = (sandesha2_sender_bean_t *) args->data;
    sandesha2_sender_bean_set_msg_id(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_sender_bean_set_msg_ctx_ref_key(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_sender_bean_set_internal_seq_id(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_sender_bean_set_sent_count(bean, env, 
        argv[0] ? AXIS2_ATOI(argv[0]) : 0);
    sandesha2_sender_bean_set_msg_no(bean, env, 
        argv[0] ? atol(argv[0]) : 0);
    sandesha2_sender_bean_set_send(bean, env, 
        argv[0] ? AXIS2_ATOI(argv[0]) : 0);
    sandesha2_sender_bean_set_resend(bean, env, 
        argv[0] ? AXIS2_ATOI(argv[0]) : 0);
    sandesha2_sender_bean_set_time_to_send(bean, env, 
        argv[0] ? atol(argv[0]) : 0);
    sandesha2_sender_bean_set_msg_type(bean, env, 
        argv[0] ? AXIS2_ATOI(argv[0]) : 0);
    sandesha2_sender_bean_set_seq_id(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_sender_bean_set_wsrm_anon_uri(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_sender_bean_set_to_address(bean, env, 
        argv[0] ? argv[0] : "NULL");
    return 0;
}

static int 
sandesha2_sender_count_callback(
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
sandesha2_permanent_sender_mgr_free(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_insert(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_remove(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_retrieve(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_update(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_internal_seq_id(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id);

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_sender_bean(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_unique(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_match(
    sandesha2_permanent_bean_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_get_next_msg_to_send(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env);

static const sandesha2_sender_mgr_ops_t sender_mgr_ops = 
{
    sandesha2_permanent_sender_mgr_free,
    sandesha2_permanent_sender_mgr_insert,
    sandesha2_permanent_sender_mgr_remove,
    sandesha2_permanent_sender_mgr_retrieve,
    sandesha2_permanent_sender_mgr_update,
    sandesha2_permanent_sender_mgr_find_by_internal_seq_id,
    sandesha2_permanent_sender_mgr_find_by_sender_bean,
    sandesha2_permanent_sender_mgr_find_unique,
    sandesha2_permanent_sender_mgr_get_next_msg_to_send,
};

AXIS2_EXTERN sandesha2_sender_mgr_t * AXIS2_CALL
sandesha2_permanent_sender_mgr_create(
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_conf_ctx_t *ctx)
{
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    sender_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_sender_mgr_t));

    sender_mgr_impl->bean_mgr = sandesha2_permanent_bean_mgr_create(env,
        storage_mgr, ctx, SANDESHA2_BEAN_MAP_RETRANSMITTER);
    sender_mgr_impl->bean_mgr->ops.match = sandesha2_permanent_sender_mgr_match;
    sender_mgr_impl->sender_mgr.ops = sender_mgr_ops;
    return &(sender_mgr_impl->sender_mgr);
}

void AXIS2_CALL
sandesha2_permanent_sender_mgr_free(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    if(sender_mgr_impl->bean_mgr)
    {
        sandesha2_permanent_bean_mgr_free(sender_mgr_impl->bean_mgr, env);
        sender_mgr_impl->bean_mgr = NULL;
    }
    if(sender_mgr_impl)
    {
        AXIS2_FREE(env->allocator, sender_mgr_impl);
        sender_mgr_impl = NULL;
    }
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_insert(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    axis2_char_t *insert_sql = NULL;
    axis2_char_t *msg_id = NULL;
    axis2_char_t *msg_ctx_ref_key = NULL;
    axis2_char_t *internal_seq_id = NULL;
    int sent_count = -1;
    long msg_no = -1;
    axis2_bool_t send = AXIS2_FALSE;
    axis2_bool_t resend = AXIS2_FALSE;
    long time_to_send = -1;
    int msg_type = -1;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *wsrm_anon_uri = NULL;
    axis2_char_t *to_address = NULL;
    axis2_bool_t ret = AXIS2_FALSE;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;

    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_sender_mgr_insert");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    msg_id = sandesha2_sender_bean_get_msg_id((sandesha2_rm_bean_t *) bean, 
        env);
    msg_ctx_ref_key = sandesha2_sender_bean_get_msg_ctx_ref_key(bean, env);
    internal_seq_id = sandesha2_sender_bean_get_internal_seq_id(bean, env);
    sent_count = sandesha2_sender_bean_get_sent_count(bean, env);
    msg_no = sandesha2_sender_bean_get_msg_no(bean, env);
    send = sandesha2_sender_bean_is_send(bean, env);
    resend = sandesha2_sender_bean_is_resend(bean, env);
    time_to_send = sandesha2_sender_bean_get_time_to_send(bean, env);
    msg_type = sandesha2_sender_bean_get_msg_type(bean, env);
    seq_id = sandesha2_sender_bean_get_seq_id(bean, env);
    wsrm_anon_uri = sandesha2_sender_bean_get_wsrm_anon_uri(bean, env);
    to_address = sandesha2_sender_bean_get_to_address(bean, env);
    insert_sql = axis2_strcat(env, "insert into sender(msg_id, msg_ctx_ref_key, "\
        "internal_seq_id, sent_count, msg_no, send, resend, time_to_send,"\
        "msg_type, seq_id, wsrm_anon_uri, to_address) values('", msg_id, "','",
        msg_ctx_ref_key, "','", internal_seq_id, "','", sent_count, "','", 
        msg_no, "','", send, "','", resend, "','", time_to_send, "','", 
        msg_type, "','", seq_id, "','", wsrm_anon_uri, "','", to_address, "');", 
            NULL);
    ret = sandesha2_permanent_bean_mgr_insert(sender_mgr_impl->bean_mgr, env,
        insert_sql);
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_sender_mgr_insert:return:%d", ret);
    return ret;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_remove(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id)
{
    axis2_char_t *sql_retrieve = NULL;
    axis2_char_t *sql_remove = NULL;
    sandesha2_sender_bean_t *bean = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    sql_remove = axis2_strcat(env, "delete from sender where msg_id='",
        msg_id, "';", NULL);
    sql_retrieve = axis2_strcat(env, "select msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender "\
        "where msg_id='", msg_id, "';", NULL);
    bean = sandesha2_sender_bean_create(env);
    return sandesha2_permanent_bean_mgr_remove(sender_mgr_impl->bean_mgr, env,
        (sandesha2_rm_bean_t *) bean, sandesha2_sender_retrieve_callback,
            sql_retrieve, sql_remove);
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_retrieve(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id)
{
    axis2_char_t *sql_retrieve = NULL;
    sandesha2_sender_bean_t *bean = NULL;
    sandesha2_sender_bean_t *ret = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_sender_mgr_retrieve");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    sql_retrieve = axis2_strcat(env, "select msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender "\
        "where msg_id='", msg_id, "';", NULL);
    bean = sandesha2_sender_bean_create(env);
    ret = (sandesha2_sender_bean_t *) sandesha2_permanent_bean_mgr_retrieve(
        sender_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean, 
        sandesha2_sender_retrieve_callback, sql_retrieve);

    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_sender_mgr_retrieve");
    return ret;
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_update(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;

    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_sender_mgr_update");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    /* No need to update. Being a reference does the job. */
    return AXIS2_SUCCESS;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_internal_seq_id(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_sender_bean_t *bean = NULL;
    axis2_array_list_t *ret = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_sender_mgr_find_by_internal_seq_id");
    AXIS2_ENV_CHECK(env, NULL);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    
    bean = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_set_internal_seq_id(bean, env, 
        internal_seq_id);
    sql_find = axis2_strcat(env, "select msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender;", NULL);
    sql_count = "select count(*) as no_recs from sender;";
    ret = sandesha2_permanent_bean_mgr_find(sender_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_sender_find_callback,
        sandesha2_sender_count_callback, sql_find, sql_count);
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_sender_mgr_find_by_internal_seq_id");
    return ret;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_by_sender_bean(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    axis2_array_list_t *ret = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_sender_mgr_find_by_sender_bean");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    sql_find = axis2_strcat(env, "select msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender;", NULL);
    sql_count = "select count(*) as no_recs from sender;";
    ret = sandesha2_permanent_bean_mgr_find(sender_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_sender_find_callback,
        sandesha2_sender_count_callback, sql_find, sql_count);
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_sender_mgr_find_by_sender_bean");
    return ret;
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_find_unique(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);
    sql_find = axis2_strcat(env, "select msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender;", NULL);
    sql_count = "select count(*) as no_recs from sender;";
    return (sandesha2_sender_bean_t *) sandesha2_permanent_bean_mgr_find(
        sender_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean, 
        sandesha2_sender_find_callback, sandesha2_sender_count_callback, 
        sql_find, sql_count);
}

axis2_bool_t AXIS2_CALL
sandesha2_permanent_sender_mgr_match(
    sandesha2_permanent_bean_mgr_t *sender_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate)
{
    axis2_bool_t add = AXIS2_TRUE;
    axis2_char_t *ref_key = NULL;
    axis2_char_t *temp_ref_key = NULL;
    long time_to_send = 0;
    long temp_time_to_send = 0;
    axis2_char_t *msg_id = NULL;
    axis2_char_t *temp_msg_id = NULL;
    axis2_char_t *internal_seq_id = NULL;
    axis2_char_t *temp_internal_seq_id = NULL;
    long msg_no = 0;
    long temp_msg_no = 0;
    int msg_type = 0;
    int temp_msg_type = 0;
    axis2_bool_t is_send = AXIS2_FALSE;
    axis2_bool_t temp_is_send = AXIS2_FALSE;
    
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_sender_mgr_match");
    ref_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
        (sandesha2_sender_bean_t *) bean, env);
    temp_ref_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
        (sandesha2_sender_bean_t *) candidate, env);
    if(ref_key && temp_ref_key && 0 != AXIS2_STRCMP(ref_key, temp_ref_key))
    {
        add = AXIS2_FALSE;
    }
    time_to_send = sandesha2_sender_bean_get_time_to_send(
        (sandesha2_sender_bean_t *) bean, env);
    temp_time_to_send = sandesha2_sender_bean_get_time_to_send(
        (sandesha2_sender_bean_t *) candidate, env);
    /*if(time_to_send > 0 && (time_to_send < temp_time_to_send))*/
    if(time_to_send > 0 && (time_to_send != temp_time_to_send))
    {
        add = AXIS2_FALSE;
    }
    msg_id = sandesha2_sender_bean_get_msg_id(bean, env);
    temp_msg_id = sandesha2_sender_bean_get_msg_id(candidate, env);
    if(msg_id && temp_msg_id && 0 != AXIS2_STRCMP(msg_id, temp_msg_id))
    {
        add = AXIS2_FALSE;
    }
    internal_seq_id = sandesha2_sender_bean_get_internal_seq_id(
        (sandesha2_sender_bean_t *) bean, env);
    temp_internal_seq_id = sandesha2_sender_bean_get_internal_seq_id(
        (sandesha2_sender_bean_t *) candidate, 
            env);
    if(internal_seq_id && temp_internal_seq_id && 0 != AXIS2_STRCMP(
                internal_seq_id, temp_internal_seq_id))
    {
        add = AXIS2_FALSE;
    }
    msg_no = sandesha2_sender_bean_get_msg_no(
        (sandesha2_sender_bean_t *) bean, env);
    temp_msg_no = sandesha2_sender_bean_get_msg_no(
        (sandesha2_sender_bean_t *) candidate, env);
    if(msg_no > 0 && (msg_no != temp_msg_no))
    {
        add = AXIS2_FALSE;
    }
    msg_type = sandesha2_sender_bean_get_msg_type(
        (sandesha2_sender_bean_t *) bean, env);
    temp_msg_type = sandesha2_sender_bean_get_msg_type(
        (sandesha2_sender_bean_t *) candidate, env);
    if(msg_type != SANDESHA2_MSG_TYPE_UNKNOWN  && (msg_type != temp_msg_type))
    {
        add = AXIS2_FALSE;
    }
    is_send = sandesha2_sender_bean_is_send(
        (sandesha2_sender_bean_t *) bean, env);
    temp_is_send = sandesha2_sender_bean_is_send(
        (sandesha2_sender_bean_t *) candidate, env);
    if(is_send != temp_is_send)
    {
        add = AXIS2_FALSE;
    }
    /* Do not use the is_resend flag to match messages, as it can stop us from
     * detecting RM messages during 'get_next_msg_to_send'*/
    /*is_resend = sandesha2_sender_bean_is_resend(
        (sandesha2_sender_bean_t *) bean, env);
    temp_is_resend = sandesha2_sender_bean_is_resend(
        (sandesha2_sender_bean_t *) candidate, env);
    if(is_resend != temp_is_resend)
    {
        add = AXIS2_FALSE;
    }*/
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_sender_mgr_match:add:%d", 
            add);
    return add;
}

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_permanent_sender_mgr_get_next_msg_to_send(
    sandesha2_sender_mgr_t *sender_mgr,
    const axis2_env_t *env)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_sender_bean_t *matcher = sandesha2_sender_bean_create(env);
    sandesha2_sender_bean_t *result = NULL;
    long time_now = 0;
    int i = 0, size = 0;
    axis2_array_list_t *match_list = NULL;
    sandesha2_permanent_sender_mgr_t *sender_mgr_impl = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_sender_mgr_get_next_msg_to_send");
    AXIS2_ENV_CHECK(env, NULL);
    sender_mgr_impl = SANDESHA2_INTF_TO_IMPL(sender_mgr);

    sandesha2_sender_bean_set_send(matcher, env, AXIS2_TRUE);
    time_now = sandesha2_utils_get_current_time_in_millis(env);
    sandesha2_sender_bean_set_time_to_send(matcher, env, time_now);
    sql_find = axis2_strcat(env, "select msg_ctx_ref_key, internal_seq_id, "\
        "sent_count, msg_no, send, resend, time_to_send, msg_type, seq_id, "\
        "wsrm_anon_uri, to_address from sender;", NULL);
    sql_count = "select count(*) as no_recs from sender;";
    match_list = sandesha2_permanent_bean_mgr_find(sender_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) matcher, sandesha2_sender_find_callback,
        sandesha2_sender_count_callback, sql_find, sql_count);

    /*
     * We either return an application message or an RM message. If we find
     * an application message first then we carry on through the list to be
     * sure that we send the lowest app message avaliable. If we hit a RM
     * message first then we are done.
     */
    if(match_list)
        size = AXIS2_ARRAY_LIST_SIZE(match_list, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_sender_bean_t *bean = NULL;
        int msg_type = -1;
        bean = (sandesha2_sender_bean_t *) AXIS2_ARRAY_LIST_GET(match_list, 
            env, i);
        msg_type = sandesha2_sender_bean_get_msg_type(bean, env);
        if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
        {
            long msg_no = sandesha2_sender_bean_get_msg_no(bean, env);
            long result_msg_no = -1;
            if(result)
                result_msg_no = sandesha2_sender_bean_get_msg_no(result, env);
            if(result == NULL || result_msg_no > msg_no)
                result = bean;
        }
        else if(!result)
        {
            result = bean;
            break;
        }
    }
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_sender_mgr_get_next_msg_to_send");
    return result;
}

