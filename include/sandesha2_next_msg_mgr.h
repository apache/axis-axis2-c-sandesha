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

typedef struct sandesha2_next_msg_mgr_t sandesha2_next_msg_mgr_t;

AXIS2_EXTERN sandesha2_next_msg_mgr_t * AXIS2_CALL
sandesha2_next_msg_mgr_create(
    const axis2_env_t *env,
    axis2_ctx_t *ctx);

axis2_status_t AXIS2_CALL 
sandesha2_next_msg_mgr_free(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_insert(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_remove(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env,
    axis2_char_t *seq_id);

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_next_msg_mgr_retrieve(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env,
    axis2_char_t *seq_id);

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_mgr_update(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_next_msg_mgr_find(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

sandesha2_next_msg_bean_t *AXIS2_CALL
sandesha2_next_msg_mgr_find_unique(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env,
    sandesha2_next_msg_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_next_msg_mgr_retrieve_all(
    sandesha2_next_msg_mgr_t *next_msg,
    const axis2_env_t *env);


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_NEXT_MSG_MGR_H */
