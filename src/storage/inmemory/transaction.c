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
 
#include <sandesha2_transaction.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

typedef struct sandesha2_transaction_impl sandesha2_transaction_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_transaction_impl
{
    sandesha2_transaction_t transaction;

};

#define SANDESHA2_INTF_TO_IMPL(transaction) ((sandesha2_transaction_impl_t *) transaction)

axis2_status_t AXIS2_CALL 
sandesha2_transaction_free(
        void *transaction,
        const axis2_env_t *envv);

axis2_status_t AXIS2_CALL
sandesha2_transaction_commit(
        sandesha2_transaction_t *transaction,
        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_transaction_rollback(
        sandesha2_transaction_t *transaction,
        const axis2_env_t *env);

sandesha2_transaction_t *
sandesha2_transaction_create(
        const axis2_env_t *env)
{
    sandesha2_transaction_impl_t *transaction_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    transaction_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_transaction_impl_t));

    transaction_impl->transaction.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_transaction_ops_t)); 
    
    transaction_impl->transaction.ops->free = sandesha2_transaction_free;
    transaction_impl->transaction.ops->commit = 
        sandesha2_transaction_commit;
    transaction_impl->transaction.ops->rollback = 
        sandesha2_transaction_rollback;

    return &(transaction_impl->transaction);
}

axis2_status_t AXIS2_CALL
sandesha2_transaction_free(
        void *transaction,
        const axis2_env_t *env)
{
    sandesha2_transaction_impl_t *transaction_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    transaction_impl = SANDESHA2_INTF_TO_IMPL(transaction);

    if((&(transaction_impl->transaction))->ops)
    {
        AXIS2_FREE(env->allocator, (&(transaction_impl->transaction))->ops);
        (&(transaction_impl->transaction))->ops = NULL;
    }

    if(transaction_impl)
    {
        AXIS2_FREE(env->allocator, transaction_impl);
        transaction_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_transaction_commit(
        sandesha2_transaction_t *transaction,
        const axis2_env_t *env)
{
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_transaction_rollback(
        sandesha2_transaction_t *transaction,
        const axis2_env_t *env)
{
    return AXIS2_SUCCESS;
}


