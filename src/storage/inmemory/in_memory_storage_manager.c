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
 
#include <sandesha2_in_memory_storage_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

typedef struct sandesha2_in_memory_storage_mgr_impl sandesha2_in_memory_storage_mgr_impl_t;

/** 
 * @brief Sandesha2 Storage Manager Struct Impl
 *   Sandesha2 Storage Manager 
 */ 
struct sandesha2_in_memory_storage_mgr_impl
{
    sandesha2_in_memory_storage_mgr_t storage;
	
	sandesha2_in_memory_storage_mgr_t *instance;
    axis2_char_t *SANDESHA2_MSG_MAP_KEY;
    sandesha2_in_memory_create_seq_mgr_t *create_seq_mgr;
    sandesha2_in_memory_next_msg_mgr_t *next_msg_mgr;
    sandesha2_in_memory_seq_property_mgr_t *seq_property_mgr;
    sandesha2_in_memory_sender_mgr_t *sender_mgr;
    sandesha2_in_memory_invoker_mgr_t *invoker_mgr;

    axis2_conf_ctx_t *conf_ctx = NULL;
};

#define SANDESHA2_INTF_TO_IMPL(storage) ((sandesha2_in_memory_storage_mgr_impl_t *) storage)

axis2_status_t AXIS2_CALL 
sandesha2_in_memory_storage_mgr_free(
        void *storage,
        const axis2_env_t *envv);

AXIS2_EXTERN sandesha2_in_memory_storage_mgr_t * AXIS2_CALL
sandesha2_in_memory_storage_mgr_create(
        const axis2_env_t *env,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;
    axis2_ctx_t *ctx = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    storage_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_in_memory_storage_mgr_impl_t));

    storage_impl->instance = NULL;
    storage_impl->SANDESHA2_MSG_MAP_KEY = AXIS2_STRDUP("Sandesha2MessageMap", env);
    storage_impl->create_seq_mgr = NULL;
    storage_impl->next_msg_mgr = NULL;
    storage_impl->seq_property_mgr = NULL;
    storage_impl->sender_mgr = NULL;
    storage_impl->invoker_mgr = NULL;
    storage_impl->conf_ctx = NULL;

    storage_impl->storage.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_in_memory_storage_mgr_ops_t)); 
   
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    storage_impl->create_seq_mgr = sandesha2_in_memory_create_seq_mgr_create(env, ctx);
    storage_impl->next_msg_mgr = sandesha2_in_memory_next_msg_mgr_create(env, ctx);
    storage_impl->seq_property_mgr = sandesha2_in_memory_seq_property_mgr_create(env, ctx);
    storage_impl->sender_mgr = sandesha2_in_memory_sender_mgr_create(env, ctx);
    storage_impl->invoker_mgr = sandesha2_in_memory_invoker_mgr_create(env, ctx);

    storage_impl->storage.ops->free = sandesha2_in_memory_storage_mgr_free;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;
    storage_impl->storage.ops-> = 
        sandesha2_in_memory_storage_mgr_;

    return &(storage_impl->storage);
}

axis2_status_t AXIS2_CALL
sandesha2_in_memory_storage_mgr_free(
        void *storage,
        const axis2_env_t *env)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);

    if(storage_impl->instance)
    {
        axis2_thread_mutex_destry(storage_impl->instance);
        storage_impl->instance = NULL;
    }
    if(storage_impl->create_seq_mgr)
    {
        SANDESHA2_IN_MEMORY_CREATE_SEQ_MGR_FREE(storage_impl->creat_seq_mgr, env);
        storage_impl->create_seq_mgr = NULL;
    }
    if(storage_impl->next_msg_mgr)
    {
        SANDESHA2_IN_MEMORY_NEXT_MSG_MGR_FREE(storage_impl->next_msg_mgr, env);
        storage_impl->next_msg_mgr = NULL;
    }
    if(storage_impl->sender_mgr)
    {
        SANDESHA2_IN_MEMORY_SENDER_MGR_FREE(storage_impl->sender_mgr, env);
        storage_impl->sender_mgr = NULL;
    }
    if(storage_impl->seq_property_mgr)
    {
        SANDESHA2_IN_MEMORY_SEQ_PROPERTY_MGR_FREE(storage_impl->seq_property_mgr, env);
        storage_impl->seq_property_mgr = NULL;
    }
    if(storage_impl->invoker_mgr)
    {
        SANDESHA2_IN_MEMORY_INVOKER_MGR_FREE(storage_impl->invoker_mgr, env);
        storage_impl->invoker_mgr = NULL;
    }
    if(storage_impl->SANDESHA2_MSG_MAP_KEY)
    {
        AXIS2_FREE(env->allocator, storage_impl->SANDESHA2_MSG_MAP_KEY);
        storage_impl->SANDESHA2_MSG_MAP_KEY = NULL;
    }

    if((&(storage_impl->storage))->ops)
    {
        AXIS2_FREE(env->allocator, (&(storage_impl->storage))->ops);
        (&(storage_impl->storage))->ops = NULL;
    }

    if(storage_impl)
    {
        AXIS2_FREE(env->allocator, storage_impl);
        storage_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

sandesha2_transaction_mgr_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_transaction(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);

    return sandesha2_transaction_mgr_create(env);
}

sandesha2_in_memory_create_seq_mgr_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_create_seq_mgr(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);

    return storage_impl->create_seq_mgr;
}

sandesha2_in_memory_next_msg_mgr_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_next_msg_mgr(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);

    return storage_impl->next_msg_mgr;
}

sandesha2_in_memory_sender_mgr_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_retransmitter_mgr(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);

    return storage_impl->sender_mgr;
}

sandesha2_in_memory_seq_property_mgr_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_seq_property_mgr(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);

    return storage_impl->seq_property_mgr;
}

sandesha2_in_memory_invoker_mgr_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_storage_map_mgr(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);

    return storage_impl->invoker_mgr;
}

axis2_status_t AXIS2_CALL
sandesha2_in_memory_storage_mgr_set_ctx(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);


    storage_impl->conf_ctx = conf_ctx;
    return AXIS2_SUCCESS;
}

axis2_conf_ctx_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_ctx(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);


    return storage_impl->conf_ctx;
}

axis2_status_t AXIS2_CALL
sandesha2_in_memory_storage_mgr_init(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);
    
    sandesha2_in_memory_storage_mgr_set_ctx(storage, env, conf_ctx);

    return AXIS2_SUCCESS;
}

sandesha2_in_memory_storage_mgr_t *AXIS2_CALL
sandesha2_in_memory_storage_mgr_get_instance(
        sandesha2_in_memory_storage_mgr_t *storage,
        const axis2_env_t *env,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_in_memory_storage_mgr_impl_t *storage_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    storage_impl = SANDESHA2_INTF_TO_IMPL(storage);
   
    if(!storage_impl->instance)
    {
        storage_impl->instance = sandesha2_in_memory_storage_mgr_create(env, conf_ctx);
    }

    return storage_impl->instance;
}
	
	public MessageContext retrieveMessageContext(String key,ConfigurationContext context) {
		HashMap storageMap = (HashMap) getContext().getProperty(MESSAGE_MAP_KEY);
		if (storageMap==null)
			return null;
		
		return (MessageContext) storageMap.get(key);
	}

	public void storeMessageContext(String key,MessageContext msgContext) {
		HashMap storageMap = (HashMap) getContext().getProperty(MESSAGE_MAP_KEY);
		
		if (storageMap==null) {
			storageMap = new HashMap ();
			getContext().setProperty(MESSAGE_MAP_KEY,storageMap);
		}
		
		if (key==null)
		    key = SandeshaUtil.getUUID();
		
		storageMap.put(key,msgContext);
		
	}

	public void updateMessageContext(String key,MessageContext msgContext) throws SandeshaStorageException { 
		HashMap storageMap = (HashMap) getContext().getProperty(MESSAGE_MAP_KEY);
		
		if (storageMap==null) {
			throw new SandeshaStorageException ("Storage Map not present");
		}
		
		Object oldEntry = storageMap.get(key);
		if (oldEntry==null)
			throw new SandeshaStorageException ("Entry is not present for updating");
		
		storeMessageContext(key,msgContext);
	}
	
	public void removeMessageContext(String key) throws SandeshaStorageException { 
		HashMap storageMap = (HashMap) getContext().getProperty(MESSAGE_MAP_KEY);
		
		if (storageMap==null) {
			return;
		}
		
		Object entry = storageMap.get(key);
		if (entry!=null)
			storageMap.remove(key);
	}
	
	public void  initStorage (AxisModule moduleDesc) {
		
	}

	public SOAPEnvelope retrieveSOAPEnvelope(String key) throws SandeshaStorageException {
		// TODO no real value
		return null;
	}

	public void storeSOAPEnvelope(SOAPEnvelope envelope, String key) throws SandeshaStorageException {
		// TODO no real value
	}
	
	
}
