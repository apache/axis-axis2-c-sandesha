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
 
#include <platforms/axis2_platform_auto_sense.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_permanent_storage_mgr.h>
#include "sandesha2_permanent_bean_mgr.h"
#include <sandesha2_msg_store_bean.h>
#include <sandesha2_permanent_create_seq_mgr.h>
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_permanent_invoker_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_permanent_next_msg_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_permanent_sender_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_permanent_seq_property_mgr.h>
#include <sandesha2_transaction.h>
#include <sandesha2_property_bean.h>
#include <sandesha2_permanent_transaction.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <sandesha2_rm_bean.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <axis2_msg_ctx.h>
#include <axis2_uuid_gen.h>
#include <axis2_conf_ctx.h>
#include <axis2_const.h>
#include <axis2_svc_ctx.h>
#include <axis2_options.h>
#include <axis2_addr.h>
#include <axis2_http_transport.h>
#include <axiom_soap_const.h>
#include <axis2_transport_out_desc.h>
#include <axis2_http_out_transport_info.h>
#include <axiom_xml_reader.h>
#include <axiom_stax_builder.h>
#include <axiom_soap_builder.h>

/** 
 * @brief Sandesha2 Permanent Storage Manager Struct Impl
 *   Sandesha2 Permanent Storage Manager 
 */ 
typedef struct sandesha2_permanent_storage_mgr
{
	sandesha2_storage_mgr_t storage_mgr;
    axis2_char_t *SANDESHA2_MSG_MAP_KEY;
    sandesha2_create_seq_mgr_t *create_seq_mgr;
    sandesha2_next_msg_mgr_t *next_msg_mgr;
    sandesha2_seq_property_mgr_t *seq_property_mgr;
    sandesha2_sender_mgr_t *sender_mgr;
    sandesha2_invoker_mgr_t *invoker_mgr;
    axis2_conf_ctx_t *conf_ctx;
    axis2_char_t *db_name;
    /*sqlite3 *db;*/
    axis2_hash_t *transactions;
    sandesha2_permanent_bean_mgr_t *bean_mgr;
    axis2_hash_t *msg_ctx_map;
    axis2_thread_mutex_t *mutex;
} sandesha2_permanent_storage_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(trans) \
    ((sandesha2_permanent_storage_mgr_t *) trans)

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free_void_arg(
    void *storage_mgr,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL 
sandesha2_permanent_storage_mgr_free(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *envv);

struct sandesha2_transaction *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_transaction(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

void AXIS2_CALL
sandesha2_permanent_storage_mgr_enlist_bean(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *rm_bean);

sandesha2_create_seq_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_create_seq_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_next_msg_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_next_msg_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_sender_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_retrans_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_seq_property_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_seq_property_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_invoker_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_storage_map_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_set_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx);

axis2_conf_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx);
	
axis2_msg_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx);
		
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);
			
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_update_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init_storage(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_module_desc_t *module_desc);

axiom_soap_envelope_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_soap_envelope(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_soap_envelope(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_response(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axiom_soap_envelope_t *response,
    int msg_no,
    int soap_version);

axiom_soap_envelope_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axis2_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axis2_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no);

static sandesha2_msg_store_bean_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_msg_store_bean (
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx);

static axis2_hash_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_map_from_string(
    const axis2_env_t *env,
    axis2_char_t *str);

static axis2_char_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_string(
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx);

static const sandesha2_storage_mgr_ops_t storage_mgr_ops = 
{
    sandesha2_permanent_storage_mgr_free,
    sandesha2_permanent_storage_mgr_free_void_arg,
    sandesha2_permanent_storage_mgr_get_transaction,
    sandesha2_permanent_storage_mgr_enlist_bean,
    sandesha2_permanent_storage_mgr_get_create_seq_mgr,
    sandesha2_permanent_storage_mgr_get_next_msg_mgr,
    sandesha2_permanent_storage_mgr_get_retrans_mgr,
    sandesha2_permanent_storage_mgr_get_seq_property_mgr,
    sandesha2_permanent_storage_mgr_get_storage_map_mgr,
    sandesha2_permanent_storage_mgr_set_ctx,
    sandesha2_permanent_storage_mgr_get_ctx,
    sandesha2_permanent_storage_mgr_init,
    sandesha2_permanent_storage_mgr_retrieve_msg_ctx,
    sandesha2_permanent_storage_mgr_store_msg_ctx,
    sandesha2_permanent_storage_mgr_update_msg_ctx,
    sandesha2_permanent_storage_mgr_remove_msg_ctx,
    sandesha2_permanent_storage_mgr_init_storage,
    sandesha2_permanent_storage_mgr_retrieve_soap_envelope,
    sandesha2_permanent_storage_mgr_store_soap_envelope,
    sandesha2_permanent_storage_mgr_store_response,
    sandesha2_permanent_storage_mgr_retrieve_response,
    sandesha2_permanent_storage_mgr_remove_response
};

AXIS2_EXTERN sandesha2_storage_mgr_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_create(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    axis2_conf_t *conf = NULL;
   
    AXIS2_ENV_CHECK(env, NULL);
    storage_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_storage_mgr_t));

    storage_mgr_impl->SANDESHA2_MSG_MAP_KEY = AXIS2_STRDUP("Sandesha2MessageMap", 
        env);
    storage_mgr_impl->conf_ctx = conf_ctx;
    axis2_allocator_switch_to_global_pool(env->allocator);
    storage_mgr_impl->transactions = axis2_hash_make(env);
    axis2_allocator_switch_to_local_pool(env->allocator);
    storage_mgr_impl->bean_mgr = NULL;
    storage_mgr_impl->mutex = axis2_thread_mutex_create(env->allocator,
        AXIS2_THREAD_MUTEX_DEFAULT);
    axis2_allocator_switch_to_global_pool(env->allocator);
    storage_mgr_impl->msg_ctx_map = axis2_hash_make(env);
    axis2_allocator_switch_to_local_pool(env->allocator);
    conf = AXIS2_CONF_CTX_GET_CONF((const axis2_conf_ctx_t *) conf_ctx, env);
    storage_mgr_impl->bean_mgr = sandesha2_permanent_bean_mgr_create(env,
        &(storage_mgr_impl->storage_mgr), conf_ctx, NULL);
    storage_mgr_impl->create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(
        env, &(storage_mgr_impl->storage_mgr), conf_ctx);
    storage_mgr_impl->next_msg_mgr = sandesha2_permanent_next_msg_mgr_create(
        env, &(storage_mgr_impl->storage_mgr), conf_ctx);
    storage_mgr_impl->seq_property_mgr = 
        sandesha2_permanent_seq_property_mgr_create(env, 
            &(storage_mgr_impl->storage_mgr), conf_ctx);
    storage_mgr_impl->sender_mgr = sandesha2_permanent_sender_mgr_create(env, 
        &(storage_mgr_impl->storage_mgr), conf_ctx);
    storage_mgr_impl->invoker_mgr = sandesha2_permanent_invoker_mgr_create(env, 
        &(storage_mgr_impl->storage_mgr), conf_ctx);

    storage_mgr_impl->storage_mgr.ops = &storage_mgr_ops;

    return &(storage_mgr_impl->storage_mgr);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free_void_arg(
    void *storage_mgr,
    const axis2_env_t *env)
{
    storage_mgr = (sandesha2_permanent_storage_mgr_t *) storage_mgr;
    return sandesha2_permanent_storage_mgr_free(storage_mgr, env);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    if(storage_mgr_impl->transactions)
    {
        axis2_hash_free(storage_mgr_impl->transactions, env);
    }
    if(storage_mgr_impl->create_seq_mgr)
    {
        sandesha2_create_seq_mgr_free(storage_mgr_impl->create_seq_mgr, env);
        storage_mgr_impl->create_seq_mgr = NULL;
    }
    if(storage_mgr_impl->create_seq_mgr)
    {
        sandesha2_create_seq_mgr_free(storage_mgr_impl->create_seq_mgr, env);
        storage_mgr_impl->create_seq_mgr = NULL;
    }
    if(storage_mgr_impl->next_msg_mgr)
    {
        sandesha2_next_msg_mgr_free(storage_mgr_impl->next_msg_mgr, env);
        storage_mgr_impl->next_msg_mgr = NULL;
    }
    if(storage_mgr_impl->sender_mgr)
    {
        sandesha2_sender_mgr_free(storage_mgr_impl->sender_mgr, env);
        storage_mgr_impl->sender_mgr = NULL;
    }
    if(storage_mgr_impl->seq_property_mgr)
    {
        sandesha2_seq_property_mgr_free(storage_mgr_impl->seq_property_mgr, 
            env);
        storage_mgr_impl->seq_property_mgr = NULL;
    }
    if(storage_mgr_impl->invoker_mgr)
    {
        sandesha2_invoker_mgr_free(storage_mgr_impl->invoker_mgr, env);
        storage_mgr_impl->invoker_mgr = NULL;
    }
    if(storage_mgr_impl->SANDESHA2_MSG_MAP_KEY)
    {
        AXIS2_FREE(env->allocator, storage_mgr_impl->SANDESHA2_MSG_MAP_KEY);
        storage_mgr_impl->SANDESHA2_MSG_MAP_KEY = NULL;
    }

    if(storage_mgr_impl)
    {
        AXIS2_FREE(env->allocator, storage_mgr_impl);
        storage_mgr_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

sandesha2_transaction_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_transaction(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
	sandesha2_transaction_t *transaction = NULL;
    axis2_hash_index_t *index = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    unsigned long int thread_id;
    axis2_char_t *thread_id_key = NULL;

	storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
	thread_id = (unsigned long int ) axis2_os_thread_current();


    axis2_thread_mutex_lock(storage_mgr_impl->mutex);
    axis2_allocator_switch_to_global_pool(env->allocator);
    for (index = axis2_hash_first(storage_mgr_impl->transactions , env); index; 
        index = axis2_hash_next(env, index))
    {
        void *v = NULL;
        sandesha2_transaction_t *temp = NULL;
        axis2_hash_this(index, NULL, NULL, &v);
        temp = (sandesha2_transaction_t *) v;
        while(sandesha2_permanent_transaction_is_active(temp, env))
        {
            AXIS2_SLEEP(1);
        }

    }
    thread_id_key = AXIS2_MALLOC(env->allocator, sizeof(char)*128);
    sprintf(thread_id_key, "%lu", thread_id); 
    transaction = (sandesha2_transaction_t *) axis2_hash_get(
        storage_mgr_impl->transactions, thread_id_key, AXIS2_HASH_KEY_STRING);
    if(!transaction)
    {
        transaction = 
            sandesha2_permanent_transaction_create(env, storage_mgr, thread_id);
        axis2_hash_set(storage_mgr_impl->transactions, thread_id_key, 
            AXIS2_HASH_KEY_STRING, transaction);
    }
    axis2_allocator_switch_to_local_pool(env->allocator);
    axis2_thread_mutex_unlock(storage_mgr_impl->mutex);
    return transaction;
}

void AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_transaction(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    sandesha2_transaction_t *transaction)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    unsigned long int thread_id = -1;
    axis2_char_t *thread_id_key = AXIS2_MALLOC(env->allocator, 128);
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    axis2_thread_mutex_lock(storage_mgr_impl->mutex);
    thread_id = (unsigned long int) sandesha2_permanent_transaction_get_thread_id(
        transaction, env);
    sprintf(thread_id_key, "%lu", thread_id); 
    axis2_allocator_switch_to_global_pool(env->allocator);
    axis2_hash_set(storage_mgr_impl->transactions, thread_id_key, 
        AXIS2_HASH_KEY_STRING, NULL);
    axis2_allocator_switch_to_local_pool(env->allocator);
    axis2_thread_mutex_unlock(storage_mgr_impl->mutex);
}

void AXIS2_CALL
sandesha2_permanent_storage_mgr_enlist_bean(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *rm_bean)
{
    /*sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    sandesha2_transaction_t *transaction = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    transaction = sandesha2_permanent_storage_mgr_get_transaction(storage_mgr, 
        env);
    sandesha2_transaction_enlist(transaction, env, rm_bean);*/
}

sandesha2_create_seq_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_create_seq_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->create_seq_mgr;
}

sandesha2_next_msg_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_next_msg_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->next_msg_mgr;
}

sandesha2_sender_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_retrans_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->sender_mgr;
}

sandesha2_seq_property_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_seq_property_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    return storage_mgr_impl->seq_property_mgr;
}

sandesha2_invoker_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_storage_map_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->invoker_mgr;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_set_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    storage_mgr_impl->conf_ctx = conf_ctx;
    return AXIS2_SUCCESS;
}

axis2_conf_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->conf_ctx;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    
    sandesha2_permanent_storage_mgr_set_ctx(storage_mgr, env, conf_ctx);

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx)
{
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    axis2_allocator_switch_to_global_pool(env->allocator);
    axis2_hash_set(storage_mgr_impl->msg_ctx_map, key, AXIS2_HASH_KEY_STRING, 
        msg_ctx);
    axis2_allocator_switch_to_local_pool(env->allocator);
    AXIS2_MSG_CTX_SET_KEEP_ALIVE(msg_ctx, env, AXIS2_TRUE);
    msg_store_bean = sandesha2_permanent_storage_mgr_get_msg_store_bean(
        storage_mgr, env, msg_ctx);
    sandesha2_msg_store_bean_set_stored_key(msg_store_bean, env, key);
    sandesha2_permanent_bean_mgr_insert_msg_store_bean(storage_mgr_impl->bean_mgr, 
        env, key, msg_store_bean);
    return AXIS2_SUCCESS;
}
			
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_update_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx)
{
    return sandesha2_permanent_storage_mgr_store_msg_ctx(storage_mgr, env, key, 
        msg_ctx);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    void *entry = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    axis2_allocator_switch_to_global_pool(env->allocator);
    if(storage_mgr_impl->msg_ctx_map)
        entry = axis2_hash_get(storage_mgr_impl->msg_ctx_map, key, 
            AXIS2_HASH_KEY_STRING);
    if(entry)
    {
        axis2_hash_set(storage_mgr_impl->msg_ctx_map, key, 
            AXIS2_HASH_KEY_STRING, NULL);
    }
    axis2_allocator_switch_to_local_pool(env->allocator);
    sandesha2_permanent_bean_mgr_remove_msg_store_bean(
        storage_mgr_impl->bean_mgr, env, key);
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init_storage(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_module_desc_t *module_desc)
{
    return AXIS2_SUCCESS;
}

axiom_soap_envelope_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    /* TODO No real value */
    return NULL;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_env,
    axis2_char_t *key)
{
    /* TODO No real value */
    return AXIS2_SUCCESS;
}

static sandesha2_msg_store_bean_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_msg_store_bean (
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx)
{
    axiom_soap_envelope_t *envelope = NULL;
    axis2_char_t *soap_str = NULL;
    int soap_version = 0;
    axiom_xml_writer_t *xml_writer = NULL;
    axiom_output_t *om_output = NULL;
    axis2_transport_out_desc_t *transport_out_desc = NULL;
    axis2_svc_grp_t *svc_grp = NULL;
    axis2_svc_t *svc = NULL;
    axis2_op_t *op = NULL;
    axis2_endpoint_ref_t *to = NULL;
    axis2_endpoint_ref_t *reply_to = NULL;
    axis2_char_t *address = NULL;
    axis2_char_t *action = NULL;
    axis2_property_t *property = NULL;
    axis2_options_t *options = NULL;
    axis2_char_t *prop_str = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    sandesha2_msg_store_bean_t *bean = NULL;
        
    envelope = AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(msg_ctx, env);
    if (!envelope)
    {
        AXIS2_ERROR_SET(env->error,
                AXIS2_ERROR_NULL_SOAP_ENVELOPE_IN_MSG_CTX,
                AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "%s",
                AXIS2_ERROR_GET_MESSAGE(env->error));
        return AXIS2_FAILURE;
    }
    xml_writer = axiom_xml_writer_create_for_memory(env, NULL,
            AXIS2_TRUE, 0, AXIS2_XML_PARSER_TYPE_BUFFER);
    if (!xml_writer)
    {
        return AXIS2_FAILURE;
    }
    om_output = axiom_output_create(env, xml_writer);
    if (!om_output)
    {
        AXIOM_XML_WRITER_FREE(xml_writer, env);
        xml_writer = NULL;
        return AXIS2_FAILURE;
    }
    AXIOM_OUTPUT_SET_SOAP11(om_output, env, AXIS2_MSG_CTX_GET_IS_SOAP_11(
        msg_ctx, env));
    AXIOM_SOAP_ENVELOPE_SERIALIZE(envelope, env, om_output, AXIS2_FALSE);
    soap_str = (axis2_char_t *)AXIOM_XML_WRITER_GET_XML(xml_writer, env);
    if (AXIS2_MSG_CTX_GET_IS_SOAP_11(msg_ctx, env))
        soap_version = SANDESHA2_SOAP_VERSION_1_1;
    else
        soap_version = SANDESHA2_SOAP_VERSION_1_2;
    bean = sandesha2_msg_store_bean_create(env);
    transport_out_desc = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx, env);
    svc_grp = AXIS2_MSG_CTX_GET_SVC_GRP(msg_ctx, env);
    svc = AXIS2_MSG_CTX_GET_SVC(msg_ctx, env);
    op = AXIS2_MSG_CTX_GET_OP(msg_ctx, env);
    if(transport_out_desc)
    {
        AXIS2_TRANSPORT_ENUMS transport_out = AXIS2_TRANSPORT_OUT_DESC_GET_ENUM(
            transport_out_desc, env);
        sandesha2_msg_store_bean_set_transport_out(bean, env, transport_out);
    }
    if(svc_grp)
    {
        axis2_char_t *svc_grp_name = (axis2_char_t *) AXIS2_SVC_GRP_GET_NAME(
            svc_grp, env);
        sandesha2_msg_store_bean_set_svc_grp(bean, env, svc_grp_name);
    }
    if (svc) 
    {
        axis2_char_t *svc_name = (axis2_char_t *) AXIS2_SVC_GET_NAME(svc, env);
        sandesha2_msg_store_bean_set_svc(bean, env, svc_name);
    }
    if(op)
    {
        axis2_qname_t *qname = (axis2_qname_t *) AXIS2_OP_GET_QNAME(op, env);
        axis2_char_t *mep = NULL;
        if(qname)
        {
            axis2_char_t *op_name = AXIS2_QNAME_TO_STRING(qname, env);
            sandesha2_msg_store_bean_set_op(bean, env, op_name);
        }
        mep = (axis2_char_t *) AXIS2_OP_GET_MSG_EXCHANGE_PATTERN(op, env);
        sandesha2_msg_store_bean_set_op_mep(bean, env, mep);
    }
    sandesha2_msg_store_bean_set_flow(bean, env, AXIS2_MSG_CTX_GET_FLOW(msg_ctx, 
        env));
    sandesha2_msg_store_bean_set_svr_side(bean, env, 
        AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env));
    sandesha2_msg_store_bean_set_soap_envelope_str(bean, env, soap_str);
    sandesha2_msg_store_bean_set_soap_version(bean, env, soap_version);
    sandesha2_msg_store_bean_set_msg_id(bean, env, (axis2_char_t *) 
        AXIS2_MSG_CTX_GET_MSG_ID(msg_ctx, env));
    to = AXIS2_MSG_CTX_GET_TO(msg_ctx, env);
    if(to)
    {
        address = (axis2_char_t *) axis2_endpoint_ref_get_address(to, env);
        sandesha2_msg_store_bean_set_to_url(bean, env, address);
    }
    reply_to = AXIS2_MSG_CTX_GET_REPLY_TO(msg_ctx, env);
    if(reply_to)
    {
        address = (axis2_char_t *) axis2_endpoint_ref_get_address(reply_to, env);
        sandesha2_msg_store_bean_set_reply_to(bean, env, address);
    }
    /*property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, AXIS2_TRANSPORT_URL,
        AXIS2_FALSE);
    if(property)
    
    {
        sandesha2_msg_store_bean_set_transport_to(bean, env, transport_to);
    }*/
    {
        axis2_char_t *transport_to = NULL;
        transport_to = axis2_msg_ctx_get_transport_url(msg_ctx, env);
        if(transport_to)
            sandesha2_msg_store_bean_set_transport_to(bean, env, transport_to);
    }

    options = (axis2_options_t *) axis2_msg_ctx_get_options(msg_ctx, env);
    action  = (axis2_char_t *) AXIS2_OPTIONS_GET_ACTION(options, env);
    sandesha2_msg_store_bean_set_action(bean, env, action);
    prop_str = sandesha2_permanent_storage_mgr_get_property_string(env, msg_ctx);
    if (prop_str)
        sandesha2_msg_store_bean_set_persistent_property_str(bean, env, 
            prop_str);
    
    /* setting the request message if this a response message.*/
    op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env);
    if(op_ctx)
    {
        axis2_msg_ctx_t *in_msg_ctx = NULL;
        in_msg_ctx = AXIS2_OP_CTX_GET_MSG_CTX(op_ctx, env, 
            AXIS2_WSDL_MESSAGE_LABEL_IN); 
        if(in_msg_ctx)
        {
            axis2_char_t *in_msg_store_key = NULL;
            axis2_bool_t insert = AXIS2_FALSE;
            property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
                SANDESHA2_IN_MESSAGE_STORAGE_KEY, AXIS2_FALSE);
            if(property)
                in_msg_store_key = (axis2_char_t *) AXIS2_PROPERTY_GET_VALUE(
                    property, env);
            if(!in_msg_store_key)
            {
                in_msg_store_key = (axis2_char_t *) axis2_uuid_gen(env);
                insert = AXIS2_TRUE;
            }
            /*if(insert)
                sandesha2_permanent_storage_mgr_store_msg_ctx(storage_mgr, env,
                    in_msg_store_key, in_msg_ctx);
            else
                sandesha2_permanent_storage_mgr_update_msg_ctx(storage_mgr, env,
                    in_msg_store_key, in_msg_ctx);*/
            sandesha2_msg_store_bean_set_in_msg_store_key(bean, env, 
                in_msg_store_key);
        }
    }
    return bean;
}

axis2_msg_ctx_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axis2_env_t *env, 
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_svc_grp_t *svc_grp = NULL;
    axis2_svc_t *svc = NULL;
    AXIS2_TRANSPORT_ENUMS transport_out = -1;
    axis2_char_t *soap_env_str = NULL;
    axis2_char_t *svc_grp_name_str = NULL;
    axis2_char_t *svc_name_str = NULL;
    axis2_char_t *op_name_str = NULL;
    axis2_char_t *op_mep_str = NULL;
    axis2_char_t *to_url_str = NULL;
    axis2_char_t *reply_to_str = NULL;
    axis2_char_t *transport_to_str = NULL;
    axis2_char_t *persistent_prop_str = NULL;
    axis2_transport_out_desc_t *transport_out_desc = NULL;
    axis2_endpoint_ref_t *endpoint_ref = NULL;
    axis2_op_t *op = NULL;
    axis2_options_t *options = NULL;
    axiom_xml_reader_t *reader = NULL;
    int soap_version = -1;
    axiom_stax_builder_t *om_builder = NULL;
    axiom_soap_builder_t *soap_builder = NULL;
    axiom_soap_envelope_t *soap_envelope = NULL;
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;

    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    msg_ctx = (axis2_msg_ctx_t *) axis2_hash_get(storage_mgr_impl->msg_ctx_map,
        key, AXIS2_HASH_KEY_STRING);
    if(msg_ctx)
        return msg_ctx;
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "retrieved from database");
    msg_store_bean = sandesha2_permanent_bean_mgr_retrieve_msg_store_bean(
        storage_mgr_impl->bean_mgr, env, key);
    if (!msg_store_bean) 
    {
        return NULL;
    }
    msg_ctx = axis2_msg_ctx_create(env, conf_ctx, NULL, NULL);
    soap_env_str = sandesha2_msg_store_bean_get_soap_envelope_str(msg_store_bean, env);
    reader = axiom_xml_reader_create_for_memory(env, soap_env_str, AXIS2_STRLEN(
        soap_env_str), NULL, AXIS2_XML_PARSER_TYPE_BUFFER);
    om_builder = axiom_stax_builder_create(env, reader);
    soap_version = sandesha2_msg_store_bean_get_soap_version(msg_store_bean, env);
    if(SANDESHA2_SOAP_VERSION_1_1 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    else if(SANDESHA2_SOAP_VERSION_1_2 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    soap_envelope = AXIOM_SOAP_BUILDER_GET_SOAP_ENVELOPE(soap_builder, env);
    if (!soap_envelope)
    {
        AXIOM_SOAP_BUILDER_FREE(soap_builder, env);
        return NULL;
    }
    AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(msg_ctx, env, soap_envelope);
    AXIS2_MSG_CTX_SET_MSG_ID(msg_ctx, env, sandesha2_msg_store_bean_get_msg_id(
        msg_store_bean, env));
    /* You cannot set the same message id to a new message context again. So generate
     * a new message id and set it
     * axis2_msg_ctx_set_msg_id(msg_ctx, env, axis2_uuid_gen(env));*/

    conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);
    transport_out = sandesha2_msg_store_bean_get_transport_out(msg_store_bean, 
        env);
    transport_out_desc = AXIS2_CONF_GET_TRANSPORT_OUT(conf, env, transport_out);
    AXIS2_MSG_CTX_SET_TRANSPORT_OUT_DESC(msg_ctx, env, transport_out_desc);
    svc_grp_name_str = sandesha2_msg_store_bean_get_svc_grp(msg_store_bean, env);
    if(svc_grp_name_str)
    {
        svc_grp = AXIS2_CONF_GET_SVC_GRP(conf, env, svc_grp_name_str);
        if(svc_grp)
            AXIS2_MSG_CTX_SET_SVC_GRP(msg_ctx, env, svc_grp);
    }
    else
    {
        svc_grp = axis2_svc_grp_create_with_conf(env, conf);
        AXIS2_MSG_CTX_SET_SVC_GRP(msg_ctx, env, svc_grp);
    }
    svc_name_str = sandesha2_msg_store_bean_get_svc(msg_store_bean, env);
    if(svc_name_str)
    {
        svc = AXIS2_CONF_GET_SVC(conf, env, svc_name_str);
        if(svc)
            AXIS2_MSG_CTX_SET_SVC(msg_ctx, env, svc);
        else
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Cannot build message "\
                 "context since service %s is not present", svc_name_str);
            AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CANNOT_BUILD_MSG_CTX_WHEN_SVC_NOT_PRESENT, 
                AXIS2_FAILURE);
            AXIOM_SOAP_BUILDER_FREE(soap_builder, env);
            return NULL;
        }
    }
    op_name_str = sandesha2_msg_store_bean_get_op(msg_store_bean, env);
    op_mep_str = sandesha2_msg_store_bean_get_op_mep(msg_store_bean, env);
    svc = AXIS2_MSG_CTX_GET_SVC(msg_ctx, env);
    if((op_name_str || op_mep_str) && svc)
    {
        axis2_op_t *op = NULL;
        if(op_name_str)
        {
            axis2_qname_t *op_qname = axis2_qname_create_from_string(env, 
                op_name_str);
            op = AXIS2_SVC_GET_OP_WITH_QNAME(svc, env, op_qname);
        }
        if(!op && op_mep_str && svc)
        {
            axis2_hash_t *all_ops = NULL;
            axis2_hash_index_t *index = NULL;
            /* Finding an operation using the MEP */
            all_ops = AXIS2_SVC_GET_ALL_OPS(svc, env);
            for (index = axis2_hash_first(all_ops, env); index; index = 
                axis2_hash_next(env, index))
            {
                void *v = NULL;
                axis2_char_t *mep = NULL;
                axis2_op_t *temp = NULL;
                axis2_hash_this(index, NULL, NULL, &v);
                temp = (axis2_op_t *) v;
                mep = (axis2_char_t *) AXIS2_OP_GET_MSG_EXCHANGE_PATTERN(temp, 
                    env);
                if(0 == AXIS2_STRCMP(mep, op_mep_str))
                {
                    op = temp;
                    break;
                }
            }
        }
        if(op)
            AXIS2_MSG_CTX_SET_OP(msg_ctx, env, op);
        else
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Cant find a suitable "\
                "operation for the generated message");
            AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CANNOT_FIND_OP_FOR_GENERATED_MSG, AXIS2_FAILURE);
            AXIOM_SOAP_BUILDER_FREE(soap_builder, env);
            return NULL;
        }
    }
    /* Setting contexts TODO is this necessary? */
    svc_grp = AXIS2_MSG_CTX_GET_SVC_GRP(msg_ctx, env);
    if(svc_grp)
    {
        axis2_svc_grp_ctx_t *svc_grp_ctx = axis2_svc_grp_ctx_create(env, svc_grp, conf_ctx);
        AXIS2_MSG_CTX_SET_SVC_GRP_CTX(msg_ctx, env, svc_grp_ctx);
    }
    svc = AXIS2_MSG_CTX_GET_SVC(msg_ctx, env);
    if(svc)
    {
        axis2_svc_grp_ctx_t *svc_grp_ctx = AXIS2_MSG_CTX_GET_SVC_GRP_CTX(
            msg_ctx, env);
        axis2_svc_ctx_t *svc_ctx = axis2_svc_ctx_create(env, svc, svc_grp_ctx);
        if(svc_ctx)
        {
            axis2_svc_ctx_set_parent(svc_ctx, env, svc_grp_ctx);
            axis2_msg_ctx_set_svc_ctx(msg_ctx, env, svc_ctx);
        }
        
    }
    op = AXIS2_MSG_CTX_GET_OP(msg_ctx, env);
    if(op)
    {
		axis2_op_ctx_t *op_ctx = NULL;
        axis2_svc_ctx_t *svc_ctx = AXIS2_MSG_CTX_GET_SVC_CTX(msg_ctx, env);
        axis2_allocator_switch_to_global_pool(env->allocator);
		op_ctx = axis2_op_ctx_create(env, op, svc_ctx);
        if(op_ctx)
        {
            axis2_char_t *msg_id = NULL;
            AXIS2_OP_CTX_SET_PARENT(op_ctx, env, svc_ctx);
            AXIS2_MSG_CTX_SET_OP_CTX(msg_ctx, env, op_ctx);
            /*AXIS2_OP_CTX_ADD_MSG_CTX(op_ctx, env, msg_ctx);*/
            msg_id = (axis2_char_t *) AXIS2_MSG_CTX_GET_MSG_ID(msg_ctx, env);
            /*axis2_conf_ctx_register_op_ctx(conf_ctx, env, msg_id, op_ctx);*/
        }
        axis2_allocator_switch_to_local_pool(env->allocator);
    }
    AXIS2_MSG_CTX_SET_SERVER_SIDE(msg_ctx, env, 
        sandesha2_msg_store_bean_is_svr_side(msg_store_bean, env));
    AXIS2_MSG_CTX_SET_FLOW(msg_ctx, env, 
        sandesha2_msg_store_bean_get_flow(msg_store_bean, env));
    transport_to_str = sandesha2_msg_store_bean_get_transport_to(msg_store_bean, 
        env);
    if(transport_to_str)
    {
        /*property = axis2_property_create_with_args(env, 0, 0, 0, transport_to_str);
        AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, AXIS2_TRANSPORT_URL, property,
        AXIS2_FALSE);*/
        axis2_msg_ctx_set_transport_url(msg_ctx, env, transport_to_str);
    }
    to_url_str = sandesha2_msg_store_bean_get_to_url(msg_store_bean, env);
    if(to_url_str)
    {
        endpoint_ref = axis2_endpoint_ref_create(env, to_url_str);
        AXIS2_MSG_CTX_SET_TO(msg_ctx, env, endpoint_ref);
    }
    reply_to_str = sandesha2_msg_store_bean_get_reply_to(msg_store_bean, env);
    if(reply_to_str)
    {
        endpoint_ref = axis2_endpoint_ref_create(env, reply_to_str);
        AXIS2_MSG_CTX_SET_REPLY_TO(msg_ctx, env, endpoint_ref);
    }
    options = (axis2_options_t *) AXIS2_MSG_CTX_GET_OPTIONS(msg_ctx, env);
    if(options)
    {
        axis2_char_t *action = sandesha2_msg_store_bean_get_action(
            msg_store_bean, env);
        AXIS2_OPTIONS_SET_ACTION(options, env, action);
    }
    persistent_prop_str = 
        sandesha2_msg_store_bean_get_persistent_property_str(msg_store_bean, 
            env);
    if(persistent_prop_str && 0 != AXIS2_STRCMP("", persistent_prop_str))
    {
        axis2_hash_t *map = 
            sandesha2_permanent_storage_mgr_get_property_map_from_string(env, 
                persistent_prop_str);
        axis2_hash_index_t *index = NULL;
        if(map)
            for (index = axis2_hash_first(map, env); index; index = 
                axis2_hash_next(env, index))
            {
                axis2_property_t *property = NULL;
                void *v = NULL;
                const void *k = NULL;
                axis2_char_t *key = NULL;
                axis2_hash_this(index, &k, NULL, &v);
                key = (axis2_char_t *) k;
                property = (axis2_property_t *) v;
                AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, key, property, 
                    AXIS2_FALSE);
            }
    }
    return msg_ctx;
}

static axis2_char_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_string(
    const axis2_env_t *env,
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_char_t *prop_str = "";
    axis2_property_t *property = NULL;
    axis2_options_t *options = (axis2_options_t *) AXIS2_MSG_CTX_GET_OPTIONS(
        msg_ctx, env);
    axis2_hash_t *properties = AXIS2_OPTIONS_GET_PROPERTIES(options, env);
    axis2_hash_index_t *index = NULL;

    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING, AXIS2_FALSE);
    if(property)
    {
        axis2_char_t *value = AXIS2_PROPERTY_GET_VALUE(property, env);
        prop_str = axis2_strcat(env, SANDESHA2_QUALIFIED_FOR_SENDING,
            SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, value, NULL);
    }
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
        AXIS2_WSA_VERSION, AXIS2_FALSE);
    if(property)
    {
        axis2_char_t *temp_str = NULL;
        axis2_char_t *value = AXIS2_PROPERTY_GET_VALUE(property, env);
        if(value)
        {
            temp_str = prop_str;
            prop_str = axis2_strcat(env, temp_str, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, AXIS2_WSA_VERSION, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, value, NULL);
            if(temp_str && 0 < axis2_strlen(temp_str))
                AXIS2_FREE(env->allocator, temp_str);
        }
    }
    for (index = axis2_hash_first(properties, env); index; index = 
        axis2_hash_next(env, index))
    {
        axis2_char_t *temp_str = NULL;
        void *v = NULL;
        const void *k = NULL;
        axis2_char_t *key = NULL;
        axis2_char_t *value = NULL;
        axis2_hash_this(index, &k, NULL, &v);
        key = (axis2_char_t *) k;
        if(0 == axis2_strcmp(AXIS2_HTTP_OUT_TRANSPORT_INFO, key))
            continue;
        if(0 == axis2_strcmp(AXIS2_TRANSPORT_OUT, key))
            continue;
        if(0 == axis2_strcmp(AXIS2_TRANSPORT_IN, key))
            continue;
        if(0 == axis2_strcmp(AXIS2_TRANSPORT_HEADERS, key))
            continue;
        if(0 == axis2_strcmp(SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, key))
            continue;
        if(0 == axis2_strcmp(AXIS2_HTTP_CLIENT, key))
            continue;

        property = (axis2_property_t *) v;
        if(property)
            value = AXIS2_PROPERTY_GET_VALUE(property, env);
        if(value)
        {
            temp_str = prop_str;
            prop_str = axis2_strcat(env, temp_str, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, key, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, 
                value, NULL);
            if(temp_str && axis2_strlen(temp_str)> 0)
                AXIS2_FREE(env->allocator, temp_str);
        }
    }
    return prop_str;
}

static axis2_hash_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_map_from_string(
    const axis2_env_t *env,
    axis2_char_t *str)
{
    int i = 0, size = 0;
    axis2_hash_t *map = axis2_hash_make(env);
    axis2_array_list_t *values = sandesha2_utils_split(env, str, 
        SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR);
    if(values)
        size = axis2_array_list_size(values, env);
    if((size % 2 != 0) || (size == 1 && 0 == AXIS2_STRCMP("", 
        axis2_array_list_get(values, env, 0))))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "Invalid persistence property string");
        AXIS2_ERROR_SET(env->error, 
            SANDESHA2_ERROR_INVALID_PERSISTENCE_PROPERTY_STRING, AXIS2_FAILURE);
        return NULL;
    }
    for(i = 0; i < size; i=i+2)
    {
        axis2_char_t *key = axis2_array_list_get(values, env, i);
        axis2_char_t *value = axis2_array_list_get(values, env, i+1);
        axis2_property_t *property = axis2_property_create_with_args(env, 0, 
            0, 0, value);
        axis2_hash_set(map, key, AXIS2_HASH_KEY_STRING, property);
    }
    return map;
}

sqlite3 * AXIS2_CALL
sandesha2_permanent_storage_mgr_get_dbconn(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axis2_env_t *env)
{
    sqlite3* dbconn = NULL;
    sandesha2_transaction_t *transaction = NULL;
    unsigned long int thread_id = (unsigned long int) axis2_os_thread_current();
    axis2_char_t thread_id_key[128];
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    /*axis2_thread_mutex_lock(storage_mgr_impl->mutex);*/
    sprintf(thread_id_key, "%lu", thread_id);
    axis2_allocator_switch_to_global_pool(env->allocator);
    transaction = (sandesha2_transaction_t *) axis2_hash_get(
        storage_mgr_impl->transactions, thread_id_key, AXIS2_HASH_KEY_STRING);
    axis2_allocator_switch_to_local_pool(env->allocator);
    /*transaction = sandesha2_permanent_storage_mgr_get_transaction(storage_mgr, 
        env);*/
    if(transaction)
    {
        dbconn = (sqlite3 *) sandesha2_permanent_transaction_get_dbconn(transaction, env);
    }
    /*axis2_thread_mutex_unlock(storage_mgr_impl->mutex);*/
    return dbconn;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_response(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *seq_id,
    axiom_soap_envelope_t *response,
    int msg_no,
    int soap_version)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    axis2_char_t *response_str = NULL;
    axiom_xml_writer_t *xml_writer = NULL;
    axiom_output_t *om_output = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    if (!response)
    {
        AXIS2_ERROR_SET(env->error,
            AXIS2_ERROR_NULL_SOAP_ENVELOPE_IN_MSG_CTX,
            AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "%s",
            AXIS2_ERROR_GET_MESSAGE(env->error));
        return AXIS2_FAILURE;
    }
    xml_writer = axiom_xml_writer_create_for_memory(env, NULL,
        AXIS2_TRUE, 0, AXIS2_XML_PARSER_TYPE_BUFFER);
    if (!xml_writer)
    {
        return AXIS2_FAILURE;
    }
    om_output = axiom_output_create(env, xml_writer);
    if (!om_output)
    {
        AXIOM_XML_WRITER_FREE(xml_writer, env);
        xml_writer = NULL;
        return AXIS2_FAILURE;
    }
    AXIOM_SOAP_ENVELOPE_SERIALIZE(response, env, om_output, AXIS2_FALSE);
    response_str = (axis2_char_t *)AXIOM_XML_WRITER_GET_XML(xml_writer, env);

    sandesha2_permanent_bean_mgr_store_response(storage_mgr_impl->bean_mgr, 
        env, seq_id, response_str, msg_no, soap_version);
    return AXIS2_SUCCESS;
}
	
axiom_soap_envelope_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axis2_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    sandesha2_response_t *response = NULL;
    axiom_soap_envelope_t *response_envelope = NULL;
    axiom_xml_reader_t *reader = NULL;
    int soap_version = -1;
    axiom_stax_builder_t *om_builder = NULL;
    axiom_soap_builder_t *soap_builder = NULL;

    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    response = sandesha2_permanent_bean_mgr_retrieve_response(
        storage_mgr_impl->bean_mgr, env, seq_id, msg_no);
    if(!response)
    {
        return NULL;
    }
    reader = axiom_xml_reader_create_for_memory(env, response->response_str, 
        AXIS2_STRLEN(response->response_str), NULL, AXIS2_XML_PARSER_TYPE_BUFFER);
    om_builder = axiom_stax_builder_create(env, reader);
    soap_version = response->soap_version;
    if(SANDESHA2_SOAP_VERSION_1_1 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    else if(SANDESHA2_SOAP_VERSION_1_2 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    response_envelope = AXIOM_SOAP_BUILDER_GET_SOAP_ENVELOPE(soap_builder, env);
    if(!response_envelope)
    {
        AXIOM_SOAP_BUILDER_FREE(soap_builder, env);
    }
    return response_envelope;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axis2_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;

    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return sandesha2_permanent_bean_mgr_remove_response(
        storage_mgr_impl->bean_mgr, env, seq_id, msg_no);
}

