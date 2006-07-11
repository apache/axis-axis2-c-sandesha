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

#ifndef SANDESHA2_NEXT_MSG_MGR_H
#define SANDESHA2_NEXT_MSG_MGR_H

/**
 * @file sandesha2_next_msg_mgr.h
 * @brief Sandesha In Memory Next Message Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_ctx.h>
#include <axis2_array_list.h>
#include <sandesha2_next_msg_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_next_msg_mgr sandesha2_next_msg_mgr_t;
typedef struct sandesha2_next_msg_mgr_ops sandesha2_next_msg_mgr_ops_t;

/** @defgroup sandesha2_next_msg_mgr In Memory Next Message Manager
  * @ingroup sandesha2
  * @{
  */

struct sandesha2_next_msg_mgr_ops
{
   /** 
     * Deallocate memory
     * @return status code
     */
    axis2_status_t (AXIS2_CALL *
    free) (
            void *next_msg,
            const axis2_env_t *env);
     
    axis2_bool_t (AXIS2_CALL *
    insert) (
            sandesha2_next_msg_mgr_t *next_msg,
            const axis2_env_t *env,
            sandesha2_next_msg_bean_t *bean);

    axis2_bool_t (AXIS2_CALL *
    remove) (
            sandesha2_next_msg_mgr_t *next_msg,
            const axis2_env_t *env,
            axis2_char_t *msg_id);

    sandesha2_next_msg_bean_t *(AXIS2_CALL *
    retrieve) (
            sandesha2_next_msg_mgr_t *next_msg,
            const axis2_env_t *env,
            axis2_char_t *msg_id);

    axis2_bool_t (AXIS2_CALL *
    update) (
            sandesha2_next_msg_mgr_t *next_msg,
            const axis2_env_t *env,
            sandesha2_next_msg_bean_t *bean);

    axis2_array_list_t *(AXIS2_CALL *
    find) (
            sandesha2_next_msg_mgr_t *next_msg,
            const axis2_env_t *env,
            sandesha2_next_msg_bean_t *bean);

    sandesha2_next_msg_bean_t *(AXIS2_CALL *
    find_unique) (
            sandesha2_next_msg_mgr_t *next_msg,
            const axis2_env_t *env,
            sandesha2_next_msg_bean_t *bean);

    axis2_array_list_t *(AXIS2_CALL *
    retrieve_all) (
            sandesha2_next_msg_mgr_t *next_msg,
            const axis2_env_t *env);
};

struct sandesha2_next_msg_mgr
{
    sandesha2_next_msg_mgr_ops_t *ops;
};

AXIS2_EXTERN sandesha2_next_msg_mgr_t * AXIS2_CALL
sandesha2_next_msg_mgr_create(
        const axis2_env_t *env,
        axis2_ctx_t *ctx);

#define SANDESHA2_NEXT_MSG_MGR_FREE(next_msg, env) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->free (next_msg, env))

#define SANDESHA2_NEXT_MSG_INSERT(next_msg, env, bean) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->\
       insert (next_msg, env, bean))

#define SANDESHA2_NEXT_MSG_REMOVE(next_msg, env, msg_id) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->\
       remove (next_msg, env, msg_id))

#define SANDESHA2_NEXT_MSG_RETRIEVE(next_msg, env, msg_id) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->\
       retrieve (next_msg, env, msg_id))

#define SANDESHA2_NEXT_MSG_UPDATE(next_msg, env, bean) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->\
       update (next_msg, env, bean))

#define SANDESHA2_NEXT_MSG_FIND(next_msg, env, bean) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->\
       find_by_next_msg_bean (next_msg, env, bean))

#define SANDESHA2_NEXT_MSG_FIND_UNIQUE(next_msg, env, bean) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->\
       find_unique (next_msg, env, bean))

#define SANDESHA2_NEXT_MSG_RETRIEVE_ALL(next_msg, env) \
      (((sandesha2_next_msg_mgr_t *) next_msg)->ops->\
       retrieve_all (next_msg, env))



/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_NEXT_MSG_MGR_H */
