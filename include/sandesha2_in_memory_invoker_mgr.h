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

#ifndef SANDESHA2_INVOKER_MGR_H
#define SANDESHA2_INVOKER_MGR_H

/**
 * @file sandesha2_invoker_mgr.h
 * @brief Sandesha In Memory Invoker Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_ctx.h>
#include <axis2_array_list.h>
#include <sandesha2_invoker_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_invoker_mgr sandesha2_invoker_mgr_t;
typedef struct sandesha2_invoker_mgr_ops sandesha2_invoker_mgr_ops_t;

/** @defgroup sandesha2_invoker_mgr In Memory Invoker Manager
  * @ingroup sandesha2
  * @{
  */

struct sandesha2_invoker_mgr_ops
{
   /** 
     * Deallocate memory
     * @return status code
     */
    axis2_status_t (AXIS2_CALL *
    free) (
            void *invoker,
            const axis2_env_t *env);
     
    axis2_bool_t (AXIS2_CALL *
    insert) (
            sandesha2_invoker_mgr_t *invoker,
            const axis2_env_t *env,
            sandesha2_invoker_bean_t *bean);

    axis2_bool_t (AXIS2_CALL *
    remove) (
            sandesha2_invoker_mgr_t *invoker,
            const axis2_env_t *env,
            axis2_char_t *ref_key);

    sandesha2_invoker_bean_t *(AXIS2_CALL *
    retrieve) (
            sandesha2_invoker_mgr_t *invoker,
            const axis2_env_t *env,
            axis2_char_t *ref_key);

    axis2_bool_t (AXIS2_CALL *
    update) (
            sandesha2_invoker_mgr_t *invoker,
            const axis2_env_t *env,
            sandesha2_invoker_bean_t *bean);

    axis2_array_list_t *(AXIS2_CALL *
    find) (
            sandesha2_invoker_mgr_t *invoker,
            const axis2_env_t *env,
            sandesha2_invoker_bean_t *bean);

    sandesha2_invoker_bean_t *(AXIS2_CALL *
    find_unique) (
            sandesha2_invoker_mgr_t *invoker,
            const axis2_env_t *env,
            sandesha2_invoker_bean_t *bean);


};

struct sandesha2_invoker_mgr
{
    sandesha2_invoker_mgr_ops_t *ops;
};

AXIS2_EXTERN sandesha2_invoker_mgr_t * AXIS2_CALL
sandesha2_invoker_mgr_create(
        const axis2_env_t *env,
        axis2_ctx_t *ctx);

#define SANDESHA2_INVOKER_MGR_FREE(invoker, env) \
      (((sandesha2_invoker_mgr_t *) invoker)->ops->free (invoker, env))

#define SANDESHA2_INVOKER_MGR_INSERT(invoker, env, bean) \
      (((sandesha2_invoker_mgr_t *) invoker)->ops->\
       insert (invoker, env, bean))

#define SANDESHA2_INVOKER_MGR_REMOVE(invoker, env, ref_key) \
      (((sandesha2_invoker_mgr_t *) invoker)->ops->\
       remove (invoker, env, ref_key))

#define SANDESHA2_INVOKER_MGR_RETRIEVE(invoker, env, ref_key) \
      (((sandesha2_invoker_mgr_t *) invoker)->ops->\
       retrieve (invoker, env, ref_key))

#define SANDESHA2_INVOKER_MGR_UPDATE(invoker, env, bean) \
      (((sandesha2_invoker_mgr_t *) invoker)->ops->\
       update (invoker, env, bean))

#define SANDESHA2_INVOKER_MGR_FIND(invoker, env, bean) \
      (((sandesha2_invoker_mgr_t *) invoker)->ops->\
       find (invoker, env, bean))

#define SANDESHA2_INVOKER_MGR_FIND_UNIQUE(invoker, env, bean) \
      (((sandesha2_invoker_mgr_t *) invoker)->ops->\
       find_unique (invoker, env, bean))


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_INVOKER_MGR_H */
