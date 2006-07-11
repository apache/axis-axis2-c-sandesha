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

#ifndef SANDESHA2_IN_MEMORY_SEQ_PROPERTY_MGR_H
#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_MGR_H

/**
 * @file sandesha2_in_memory_seq_property_mgr.h
 * @brief Sandesha In Memory Sequence Property Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_ctx.h>
#include <axis2_array_list.h>
#include <sandesha2_sequence_property_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_in_memory_seq_property_mgr sandesha2_in_memory_seq_property_mgr_t;
typedef struct sandesha2_in_memory_seq_property_mgr_ops sandesha2_in_memory_seq_property_mgr_ops_t;

/** @defgroup sandesha2_in_memory_seq_property_mgr In Memory Sequence Property Manager
  * @ingroup sandesha2
  * @{
  */

struct sandesha2_in_memory_seq_property_mgr_ops
{
   /** 
     * Deallocate memory
     * @return status code
     */
    axis2_status_t (AXIS2_CALL *
    free) (
            void *seq_property,
            const axis2_env_t *env);
     
    axis2_bool_t (AXIS2_CALL *
    insert) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env,
            sandesha2_sequence_property_bean_t *bean);

    axis2_bool_t (AXIS2_CALL *
    remove) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env,
            axis2_char_t *seq_id,
            axis2_char_t *name);

    sandesha2_sequence_property_bean_t *(AXIS2_CALL *
    retrieve) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env,
            axis2_char_t *seq_id,
            axis2_char_t *name);

    axis2_bool_t (AXIS2_CALL *
    update) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env,
            sandesha2_sequence_property_bean_t *bean);

    axis2_array_list_t *(AXIS2_CALL *
    find) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env,
            sandesha2_sequence_property_bean_t *bean);

    sandesha2_sequence_property_bean_t *(AXIS2_CALL *
    find_unique) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env,
            sandesha2_sequence_property_bean_t *bean);

    axis2_array_list_t *(AXIS2_CALL *
    retrieve_all) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env);

    axis2_bool_t (AXIS2_CALL *
    update_or_insert) (
            sandesha2_in_memory_seq_property_mgr_t *seq_property,
            const axis2_env_t *env,
            sandesha2_sequence_property_bean_t *bean);
};

struct sandesha2_in_memory_seq_property_mgr
{
    sandesha2_in_memory_seq_property_mgr_ops_t *ops;
};

AXIS2_EXTERN sandesha2_in_memory_seq_property_mgr_t * AXIS2_CALL
sandesha2_in_memory_seq_property_mgr_create(
        const axis2_env_t *env,
        axis2_ctx_t *ctx);

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_MGR_FREE(seq_property, env) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->free (seq_property, env))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_INSERT(seq_property, env, bean) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       insert (seq_property, env, bean))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_REMOVE(seq_property, env, seq_id, name) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       remove (seq_property, env, seq_id, name))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_RETRIEVE(seq_property, env, seq_id, name) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       retrieve (seq_property, env, seq_id, name))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_UPDATE(seq_property, env, bean) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       update (seq_property, env, bean))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_FIND(seq_property, env, bean) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       find (seq_property, env, bean))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_FIND_UNIQUE(seq_property, env, bean) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       find_unique (seq_property, env, bean))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_UPDATE_OR_INSERT(seq_property, env, bean) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       update_or_insert (seq_property, env, bean))

#define SANDESHA2_IN_MEMORY_SEQ_PROPERTY_RETRIEVE_ALL(seq_property, env) \
      (((sandesha2_in_memory_seq_property_mgr_t *) seq_property)->ops->\
       retrieve_all (seq_property, env))



/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_IN_MEMORY_SEQ_PROPERTY_MGR_H */
