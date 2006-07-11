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

#ifndef SANDESHA2_TRANSACTION_H
#define SANDESHA2_TRANSACTION_H

/**
 * @file sandesha2_transaction.h
 * @brief Sandesha In Memory Transaction  Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_transaction sandesha2_transaction_t;
typedef struct sandesha2_transaction_ops sandesha2_transaction_ops_t;

/** @defgroup sandesha2_transaction In Memory Transaction 
  * @ingroup sandesha2
  * @{
  */

struct sandesha2_transaction_ops
{
   /** 
     * Deallocate memory
     * @return status code
     */
    axis2_status_t (AXIS2_CALL *
    free) (
            void *seq_mgr,
            const axis2_env_t *env);
     
    axis2_status_t (AXIS2_CALL *
    commit) (
            sandesha2_transaction_t *seq_mgr,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    rollback) (
            sandesha2_transaction_t *seq_mgr,
            const axis2_env_t *env);
};

struct sandesha2_transaction
{
    sandesha2_transaction_ops_t *ops;
};

AXIS2_EXTERN sandesha2_transaction_t * AXIS2_CALL
sandesha2_transaction_create(
        const axis2_env_t *env);

#define SANDESHA2_TRANSACTION_FREE(seq_mgr, env) \
      (((sandesha2_transaction_t *) seq_mgr)->ops->free (seq_mgr, env))

#define SANDESHA2_TRANSACTION_COMMIT(seq_mgr, env) \
      (((sandesha2_transaction_t *) seq_mgr)->ops->\
       commit (seq_mgr, env))

#define SANDESHA2_TRANSACTION_ROLLBACK(seq_mgr, env) \
      (((sandesha2_transaction_t *) seq_mgr)->ops->\
       rollback (seq_mgr, env))

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_TRANSACTION_H */
