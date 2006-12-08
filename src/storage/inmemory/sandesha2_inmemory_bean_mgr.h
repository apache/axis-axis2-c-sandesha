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

#ifndef SANDESHA2_INMEMORY_BEAN_MGR_H
#define SANDESHA2_INMEMORY_BEAN_MGR_H

/**
 * @file sandesha2_inmemory_bean_mgr.h
 * @brief Sandesha In Memory Bean Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_array_list.h>
#include <sandesha2_rm_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_inmemory_bean_mgr sandesha2_inmemory_bean_mgr_t;
typedef struct sandesha2_inmemory_bean_mgr_ops sandesha2_inmemory_bean_mgr_ops_t;
struct sandesha2_storage_mgr;
struct axis2_ctx;

AXIS2_DECLARE_DATA struct sandesha2_inmemory_bean_mgr_ops
{
    axis2_status_t (AXIS2_CALL *
            free) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env);

    axis2_bool_t (AXIS2_CALL *
            insert) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                axis2_char_t *key,
                sandesha2_rm_bean_t *bean);

    axis2_bool_t (AXIS2_CALL *
            remove) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                axis2_char_t *key);

    sandesha2_rm_bean_t *(AXIS2_CALL *
            retrieve) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                axis2_char_t *key);

    axis2_bool_t (AXIS2_CALL *
            update) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                axis2_char_t *key,
                sandesha2_rm_bean_t *bean);

    axis2_array_list_t *(AXIS2_CALL *
            find) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                sandesha2_rm_bean_t *bean);

    sandesha2_rm_bean_t *(AXIS2_CALL *
            find_unique) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                sandesha2_rm_bean_t *bean);
    
    axis2_bool_t (AXIS2_CALL *
            match) (
                sandesha2_inmemory_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                sandesha2_rm_bean_t *bean,
                sandesha2_rm_bean_t *candidate);

};

AXIS2_DECLARE_DATA struct sandesha2_inmemory_bean_mgr
{
    sandesha2_inmemory_bean_mgr_ops_t ops;
};

AXIS2_EXTERN sandesha2_inmemory_bean_mgr_t * AXIS2_CALL
sandesha2_inmemory_bean_mgr_create(
    const axis2_env_t *env,
    struct sandesha2_storage_mgr *storage_mgr,
    struct axis2_ctx *ctx,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_inmemory_bean_mgr_free(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *envv);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_bean_mgr_insert(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    sandesha2_rm_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_bean_mgr_remove(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_inmemory_bean_mgr_retrieve(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_bean_mgr_update(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    sandesha2_rm_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_bean_mgr_find(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean);

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_inmemory_bean_mgr_find_unique(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_bean_mgr_match(
    sandesha2_inmemory_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_INMEMORY_BEAN_MGR_H */
