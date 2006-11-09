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
 
#include <sandesha2_utils.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_property_bean.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_ack_range.h>
#include <sandesha2_spec_specific_consts.h>
#include <axis2_string.h>
#include <axis2_uuid_gen.h>
#include <axis2_addr.h>
#include <axis2_property.h>
#include <axis2_array_list.h>
#include <axis2_engine.h>
#include <sandesha2_msg_creator.h>
#include <axis2_transport_out_desc.h>

axis2_hash_t *sandesha2_terminate_mgr_rcv_side_clean_map = NULL;

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_clean_recv_side_after_terminate_msg(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *seq_id,
    sandesha2_storage_mgr_t *storage_man)
{
    axis2_bool_t in_order_invoke = AXIS2_FALSE;
    sandesha2_property_bean_t *prop_bean = NULL;
    axis2_char_t *clean_status = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    if(NULL == sandesha2_terminate_mgr_rcv_side_clean_map)
    {
        sandesha2_terminate_mgr_rcv_side_clean_map = axis2_hash_make(env);
    }
    
    prop_bean = sandesha2_utils_get_property_bean(env, AXIS2_CONF_CTX_GET_CONF(
                    conf_ctx, env));
    in_order_invoke = sandesha2_property_bean_is_in_order(prop_bean, env);
    if(AXIS2_FALSE == in_order_invoke)
        sandesha2_terminate_mgr_clean_recv_side_after_invocation(env, conf_ctx,
                    seq_id, storage_man);
    clean_status = axis2_hash_get(sandesha2_terminate_mgr_rcv_side_clean_map,
                    seq_id, AXIS2_HASH_KEY_STRING);
    if(NULL != clean_status && 0 == AXIS2_STRCMP(clean_status, 
                    SANDESHA2_CLEANED_AFTER_INVOCATION))
        sandesha2_terminate_mgr_complete_term_on_recv_side(env, conf_ctx, seq_id,
                    storage_man);
    else
        axis2_hash_set(sandesha2_terminate_mgr_rcv_side_clean_map, seq_id,
                    AXIS2_HASH_KEY_STRING, AXIS2_STRDUP(
                    SANDESHA2_CLEANED_ON_TERMINATE_MSG, env));
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_clean_recv_side_after_invocation(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *seq_id,
    sandesha2_storage_mgr_t *storage_man)
{
     sandesha2_invoker_mgr_t *invoker_mgr = NULL;
    sandesha2_invoker_bean_t *find_bean = NULL;
    axis2_array_list_t *found_list = NULL;
    int i = 0;
    axis2_char_t *clean_status = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    if(NULL == sandesha2_terminate_mgr_rcv_side_clean_map)
    {
        sandesha2_terminate_mgr_rcv_side_clean_map = axis2_hash_make(env);
    }
    
    invoker_mgr = sandesha2_storage_mgr_get_storage_map_mgr(storage_man, env);
    find_bean = sandesha2_invoker_bean_create(env);
    
    sandesha2_invoker_bean_set_seq_id(find_bean, env, seq_id);
    sandesha2_invoker_bean_set_invoked(find_bean, env, AXIS2_TRUE);
    
    found_list = sandesha2_invoker_mgr_find(invoker_mgr, env, find_bean);
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(found_list, env); i++)
    {
        sandesha2_invoker_bean_t *map_bean = NULL;
        axis2_char_t *msg_store_key = NULL;
        
        map_bean = AXIS2_ARRAY_LIST_GET(found_list, env, i);
        msg_store_key = sandesha2_invoker_bean_get_msg_ctx_ref_key(map_bean, 
                        env);
        sandesha2_storage_mgr_remove_msg_ctx(storage_man, env, msg_store_key);
        sandesha2_invoker_mgr_remove(invoker_mgr, env, 
                        sandesha2_invoker_bean_get_msg_ctx_ref_key(map_bean, 
                        env));
    }
    clean_status = axis2_hash_get(sandesha2_terminate_mgr_rcv_side_clean_map,
                    seq_id, AXIS2_HASH_KEY_STRING);
                    
    if(NULL != clean_status && 0 == AXIS2_STRCMP(clean_status, 
                    SANDESHA2_CLEANED_ON_TERMINATE_MSG))
        sandesha2_terminate_mgr_complete_term_on_recv_side(env, conf_ctx, seq_id,
                    storage_man);
    else
        axis2_hash_set(sandesha2_terminate_mgr_rcv_side_clean_map, seq_id,
                    AXIS2_HASH_KEY_STRING, AXIS2_STRDUP(
                    SANDESHA2_CLEANED_AFTER_INVOCATION, env));

    
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_complete_term_on_recv_side(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *seq_id,
    sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_next_msg_mgr_t *next_mgr = NULL;
    sandesha2_next_msg_bean_t *find_bean = NULL;
    axis2_array_list_t *found_list = NULL;
    axis2_char_t *highest_in_msg_key = NULL;
    
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    next_mgr = sandesha2_storage_mgr_get_next_msg_mgr(storage_man, env);
    find_bean = sandesha2_next_msg_bean_create(env);
    SANDESHA2_NEXT_MSG_BEAN_SET_SEQ_ID(find_bean, env, seq_id);
    
    found_list = SANDESHA2_NEXT_MSG_MGR_FIND(next_mgr, env, find_bean);
    
    highest_in_msg_key = sandesha2_utils_get_seq_property(env, seq_id,
                        SANDESHA2_SEQ_PROP_HIGHEST_IN_MSG_KEY, storage_man);
    if(NULL != highest_in_msg_key)
        sandesha2_storage_mgr_remove_msg_ctx(storage_man, env, 
                        highest_in_msg_key);
    sandesha2_terminate_mgr_remove_recv_side_propertis(env, conf_ctx, seq_id,
                        storage_man);
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_remove_recv_side_propertis(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *seq_id,
    sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *all_seq_bean = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_man, env);
    all_seq_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        SANDESHA2_SEQ_PROP_ALL_SEQS, 
                        SANDESHA2_SEQ_PROP_INCOMING_SEQ_LIST);
    if(NULL != all_seq_bean)
    {
        axis2_array_list_t *all_seq_list = NULL;
        axis2_char_t *all_seq_str = NULL;
        
        all_seq_list = sandesha2_utils_get_array_list_from_string(env, 
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(all_seq_bean, 
                        env));
        if(NULL != all_seq_list)
        {
            int i = 0;
            for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(all_seq_list, env); i++)
            {
                axis2_char_t *value = AXIS2_ARRAY_LIST_GET(all_seq_list, env, i);
                if(0 == AXIS2_STRCMP(value, seq_id))
                {
                    AXIS2_ARRAY_LIST_REMOVE(all_seq_list, env, i);
                    break;
                }
            }
            all_seq_str = sandesha2_utils_array_list_to_string(env, all_seq_list,
                        SANDESHA2_ARRAY_LIST_STRING);
            SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(all_seq_bean, env, all_seq_str);
            SANDESHA2_SEQ_PROPERTY_MGR_UPDATE(seq_prop_mgr, env, all_seq_bean);
        }
    }
    
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_terminate_sending_side(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *seq_id,
    axis2_bool_t svr_side,
    sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *seq_term_bean = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_man, env);
    seq_term_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id,
                        SANDESHA2_SEQ_PROP_SEQ_TERMINATED, SANDESHA2_VALUE_TRUE);
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, seq_term_bean);
    
    sandesha2_terminate_mgr_clean_sending_side_data(env, conf_ctx, seq_id, 
                        svr_side, storage_man);
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_do_updates_if_needed(
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    sandesha2_seq_property_bean_t *prop_bean,
    sandesha2_seq_property_mgr_t *prop_mgr)
{
    axis2_bool_t add_entry_with_seq_id = AXIS2_FALSE;
    axis2_char_t *name = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, prop_bean, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, prop_mgr, AXIS2_FAILURE);
    
    name = SANDESHA2_SEQ_PROPERTY_BEAN_GET_NAME(prop_bean, env);
    if(NULL == name)
        return AXIS2_FAILURE;
    
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_CLIENT_COMPLETED_MESSAGES))
        add_entry_with_seq_id = AXIS2_TRUE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_SEQ_TERMINATED))
        add_entry_with_seq_id = AXIS2_TRUE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_SEQ_CLOSED))
        add_entry_with_seq_id = AXIS2_TRUE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_SEQ_TIMED_OUT))
        add_entry_with_seq_id = AXIS2_TRUE;
        
    if(AXIS2_TRUE == add_entry_with_seq_id && NULL != seq_id)
    {
        sandesha2_seq_property_bean_t *new_bean = NULL;
        new_bean = sandesha2_seq_property_bean_create(env);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(new_bean, env, seq_id);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(new_bean, env, name);
        SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(new_bean, env, 
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(prop_bean, env));
        SANDESHA2_SEQ_PROPERTY_MGR_INSERT(prop_mgr, env, new_bean);
        SANDESHA2_SEQ_PROPERTY_MGR_REMOVE(prop_mgr, env, 
                    SANDESHA2_SEQ_PROPERTY_BEAN_GET_SEQ_ID(prop_bean, env),
                    name);
    }
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_terminate_mgr_is_property_deletable(
    const axis2_env_t *env,
    axis2_char_t *name)
{
    axis2_bool_t deletable = AXIS2_TRUE;
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, name, AXIS2_FALSE);
        
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_TERMINATE_ADDED))
        deletable = AXIS2_FALSE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_NO_OF_OUTGOING_MSGS_ACKED))
        deletable = AXIS2_FALSE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID))
        deletable = AXIS2_FALSE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_SEQ_TERMINATED))
        deletable = AXIS2_FALSE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_SEQ_CLOSED))
        deletable = AXIS2_FALSE;
    if(0 == AXIS2_STRCMP(name, SANDESHA2_SEQ_PROP_SEQ_TIMED_OUT))
        deletable = AXIS2_FALSE;
        
    return deletable;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_time_out_sending_side_seq(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *seq_id,
    axis2_bool_t svr_side,
    sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *seq_term_bean = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_man, env);
    seq_term_bean = sandesha2_seq_property_bean_create_with_data(env, seq_id,
                        SANDESHA2_SEQ_PROP_SEQ_TIMED_OUT, SANDESHA2_VALUE_TRUE);
    
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, seq_term_bean);
    sandesha2_terminate_mgr_clean_sending_side_data(env, conf_ctx, seq_id,
                        svr_side, storage_man);
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_clean_sending_side_data(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *seq_id,
    axis2_bool_t svr_side,
    sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    axis2_char_t *out_seq_id = NULL;
    axis2_array_list_t *found_list = NULL;
    sandesha2_create_seq_bean_t *find_create_seq_bean = NULL;
    sandesha2_seq_property_bean_t *find_seq_prop_bean = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_man, env);
    retrans_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_man, env);    
    create_seq_mgr = sandesha2_storage_mgr_get_create_seq_mgr(storage_man, env);
    
    out_seq_id = sandesha2_utils_get_seq_property(env, seq_id, 
                        SANDESHA2_SEQ_PROP_OUT_SEQ_ID, storage_man);
    if(AXIS2_TRUE == svr_side)
    {
        sandesha2_seq_property_bean_t *acks_to_bean = NULL;
        axis2_char_t *addr_ns_uri = NULL;
        axis2_char_t *anon_uri = NULL;
        axis2_bool_t stop_listner_for_async = AXIS2_FALSE;
        
        acks_to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env,
                        seq_id, SANDESHA2_SEQ_PROP_ACKS_TO_EPR);
        addr_ns_uri = sandesha2_utils_get_seq_property(env, seq_id, 
                        SANDESHA2_SEQ_PROP_ADDRESSING_NAMESPACE_VALUE, 
                        storage_man);
        anon_uri = sandesha2_spec_specific_consts_get_anon_uri(env, addr_ns_uri);
        if(NULL != acks_to_bean)
        {
            axis2_char_t *acks_to = NULL;
            acks_to = SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(acks_to_bean, env);
            if(NULL != acks_to && 0 == AXIS2_STRCMP(acks_to, anon_uri))
                stop_listner_for_async = AXIS2_TRUE;
        }
        
    }
    found_list = SANDESHA2_SENDER_MGR_FIND_BY_INTERNAL_SEQ_ID(retrans_mgr, env,
                        seq_id);
    if(NULL != found_list)
    {
        int i = 0;
        for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(found_list, env); i++)
        {
            sandesha2_sender_bean_t *retrans_bean = NULL;
            axis2_char_t *msg_store_key = NULL;
            
            retrans_bean = AXIS2_ARRAY_LIST_GET(found_list, env, i);
            msg_store_key = SANDESHA2_SENDER_BEAN_GET_MSG_CONTEXT_REF_KEY(
                        retrans_bean, env);
            sandesha2_storage_mgr_remove_msg_ctx(storage_man, env, msg_store_key);
        }
    }
    
    find_create_seq_bean = sandesha2_create_seq_bean_create(env);
    sandesha2_create_seq_bean_set_internal_seq_id(find_create_seq_bean, env,
                        seq_id);
    found_list = SANDESHA2_CREATE_SEQ_MGR_FIND(create_seq_mgr, env, 
                        find_create_seq_bean);
    if(NULL != found_list)
    {
        int i = 0;
        for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(found_list, env); i++)
        {
            sandesha2_create_seq_bean_t *create_seq_bean = NULL;
            
            create_seq_bean = AXIS2_ARRAY_LIST_GET(found_list, env, i);
            SANDESHA2_CREATE_SEQ_MGR_REMOVE(create_seq_mgr, env,
                        sandesha2_create_seq_bean_get_create_seq_msg_id(
                        create_seq_bean, env));
        }
    }
    
    find_seq_prop_bean = sandesha2_seq_property_bean_create(env);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(find_seq_prop_bean, env, seq_id);
    found_list = SANDESHA2_SEQ_PROPERTY_MGR_FIND(seq_prop_mgr, env, 
                        find_seq_prop_bean);
    if(NULL != found_list)
    {
        int i = 0;
        for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(found_list, env); i++)
        {
            sandesha2_seq_property_bean_t *seq_prop_bean = NULL;
            
            seq_prop_bean = AXIS2_ARRAY_LIST_GET(found_list, env, i);
            sandesha2_terminate_mgr_do_updates_if_needed(env, out_seq_id,
                        seq_prop_bean, seq_prop_mgr);
            if(AXIS2_TRUE == sandesha2_terminate_mgr_is_property_deletable(env,
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_NAME(seq_prop_bean, env)))
            {
                SANDESHA2_SEQ_PROPERTY_MGR_REMOVE(seq_prop_mgr, env,
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_SEQ_ID(seq_prop_bean, 
                        env), SANDESHA2_SEQ_PROPERTY_BEAN_GET_NAME(
                        seq_prop_bean, env));
            }
        }
    }
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_terminate_mgr_add_terminate_seq_msg(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx,
    axis2_char_t *out_seq_id,
    axis2_char_t *int_seq_id,
    sandesha2_storage_mgr_t *storage_man)
{
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_msg_ctx_t *terminate_msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_seq_property_bean_t *terminated = NULL;
    sandesha2_msg_ctx_t *terminate_rm_msg = NULL;
    axis2_property_t *property = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    sandesha2_seq_property_bean_t *to_bean = NULL;
    axis2_char_t *rm_ver = NULL;
    sandesha2_seq_property_bean_t *transport_to_bean = NULL;
    axis2_char_t *key = NULL;
    sandesha2_sender_bean_t *terminate_bean = NULL;
    sandesha2_sender_mgr_t *retrans_mgr = NULL;
    sandesha2_seq_property_bean_t *terminate_added = NULL;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_msg_ctx_t *msg_ctx1 = NULL;
    axis2_engine_t *engine = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, out_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, int_seq_id, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, storage_man, AXIS2_FAILURE);
    
    msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(rm_msg_ctx, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    
    seq_prop_mgr = sandesha2_storage_mgr_get_seq_property_mgr(storage_man, env);
    terminated = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, 
                        out_seq_id, SANDESHA2_SEQ_PROP_TERMINATE_ADDED);
    if(terminated && SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(terminated, env) 
            && 0 == AXIS2_STRCMP(SANDESHA2_VALUE_TRUE, 
            SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(terminated, env)))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Terminate was"
                        " added previously");
    }
    terminate_rm_msg = sandesha2_msg_creator_create_terminate_seq_msg(env,
                        rm_msg_ctx, out_seq_id, int_seq_id, storage_man);
    SANDESHA2_MSG_CTX_SET_FLOW(terminate_rm_msg, env, AXIS2_OUT_FLOW);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE,
                        env));
    SANDESHA2_MSG_CTX_SET_PROPERTY(terminate_rm_msg, env, 
                        SANDESHA2_APPLICATION_PROCESSING_DONE, property);
    
    to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env, int_seq_id,
                        SANDESHA2_SEQ_PROP_TO_EPR);
    to_epr = axis2_endpoint_ref_create(env, 
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(to_bean, env));
    if(!to_epr)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] To EPR has an "
                        "invalid value");
        return AXIS2_FAILURE;
    }
    SANDESHA2_MSG_CTX_SET_TO(rm_msg_ctx, env, to_epr);
    SANDESHA2_MSG_CTX_SET_TO(terminate_rm_msg, env, to_epr);
    rm_ver = sandesha2_utils_get_rm_version(env, int_seq_id, storage_man);
    if(!rm_ver)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Cannot find the "
                        "rm version for msg");
        return AXIS2_FAILURE;
    }
    SANDESHA2_MSG_CTX_SET_WSA_ACTION(terminate_rm_msg, env, 
                        sandesha2_spec_specific_consts_get_terminate_seq_action(
                        env, rm_ver));
    SANDESHA2_MSG_CTX_SET_SOAP_ACTION(terminate_rm_msg, env,
                        sandesha2_spec_specific_consts_get_terminate_seq_soap_action
                        (env, rm_ver));
    transport_to_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr, env,
                        int_seq_id, SANDESHA2_SEQ_PROP_TRANSPORT_TO);
    if(transport_to_bean)
    {
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(transport_to_bean,
                        env), env));
        SANDESHA2_MSG_CTX_SET_PROPERTY(terminate_rm_msg, env, 
                            AXIS2_TRANSPORT_URL, property);
    }
    
    SANDESHA2_MSG_CTX_ADD_SOAP_ENVELOPE(terminate_rm_msg, env);
    
    key = axis2_uuid_gen(env);
    terminate_bean = sandesha2_sender_bean_create(env);
    SANDESHA2_SENDER_BEAN_SET_MSG_CONTEXT_REF_KEY(terminate_bean, env, key);
    terminate_msg_ctx = SANDESHA2_MSG_CTX_GET_MSG_CTX(terminate_rm_msg, env);
    /*AXIS2_MSG_CTX_SET_KEEP_ALIVE(terminate_msg_ctx, env, AXIS2_TRUE);*/
    sandesha2_storage_mgr_store_msg_ctx(storage_man, env, key, terminate_msg_ctx);
    SANDESHA2_SENDER_BEAN_SET_TIME_TO_SEND(terminate_bean, env,
                        sandesha2_utils_get_current_time_in_millis(env) +
                        SANDESHA2_TERMINATE_DELAY);
    SANDESHA2_SENDER_BEAN_SET_MSG_ID(terminate_bean, env, 
                        SANDESHA2_MSG_CTX_GET_MSG_ID(terminate_rm_msg, env));
    SANDESHA2_SENDER_BEAN_SET_SEND(terminate_bean, env, AXIS2_TRUE);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_FALSE,
                        env));
    SANDESHA2_MSG_CTX_SET_PROPERTY(terminate_rm_msg, env, 
                            SANDESHA2_QUALIFIED_FOR_SENDING, property);
                            
    SANDESHA2_SENDER_BEAN_SET_RESEND(terminate_bean, env, AXIS2_FALSE);
    retrans_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_man, env);
    
    SANDESHA2_SENDER_MGR_INSERT(retrans_mgr, env, terminate_bean);
    
    terminate_added = sandesha2_seq_property_bean_create(env);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_NAME(terminate_added, env, 
                        SANDESHA2_SEQ_PROP_TERMINATE_ADDED);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_SEQ_ID(terminate_added, env, out_seq_id);
    SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(terminate_added, env, 
                        SANDESHA2_VALUE_TRUE);
                        
    SANDESHA2_SEQ_PROPERTY_MGR_INSERT(seq_prop_mgr, env, terminate_added);
    
    msg_ctx1 = SANDESHA2_MSG_CTX_GET_MSG_CTX(terminate_rm_msg, env);
    transport_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx1, env);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
    AXIS2_PROPERTY_SET_VALUE(property, env, transport_out);
    AXIS2_PROPERTY_SET_FREE_FUNC(property, env, 
            transport_out->ops->free_void_arg);
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx1, env, 
                        SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, property,
                        AXIS2_FALSE);
    
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(key, env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx1, env, SANDESHA2_MESSAGE_STORE_KEY, 
                        property, AXIS2_FALSE);
                        
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(SANDESHA2_VALUE_TRUE, 
                        env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx1, env, SANDESHA2_SET_SEND_TO_TRUE, 
                        property, AXIS2_FALSE);
                        
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(msg_ctx1, env, 
                        sandesha2_utils_get_transport_out(env));
                        
    engine = axis2_engine_create(env, conf_ctx);
    
    return AXIS2_ENGINE_SEND(engine, env, msg_ctx1);
}

