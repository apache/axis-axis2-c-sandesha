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

#ifndef SANDESHA2_SENDER_MGR_H
#define SANDESHA2_SENDER_MGR_H

/**
 * @file sandesha2_sender_mgr.h
 * @brief Sandesha In Memory Sender Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_ctx.h>
#include <axis2_array_list.h>
#include <sandesha2_sender_bean.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_sender_mgr_t sandesha2_sender_mgr_t;

AXIS2_EXTERN sandesha2_sender_mgr_t * AXIS2_CALL
sandesha2_sender_mgr_create(
    const axis2_env_t *env,
    axis2_ctx_t *ctx);

axis2_status_t AXIS2_CALL 
sandesha2_sender_mgr_free(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *envv);

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_insert(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_remove(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    const axis2_char_t *msg_id);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_retrieve(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    const axis2_char_t *msg_id);

axis2_bool_t AXIS2_CALL
sandesha2_sender_mgr_update(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

axis2_array_list_t *AXIS2_CALL
sandesha2_sender_mgr_find_by_internal_seq_id(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id);

axis2_array_list_t *AXIS2_CALL
sandesha2_sender_mgr_find_by_sender_bean(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_find_unique(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    sandesha2_sender_bean_t *bean);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_get_next_msg_to_send(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env);

sandesha2_sender_bean_t *AXIS2_CALL
sandesha2_sender_mgr_retrieve_from_msg_ref_key(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    axis2_char_t *msg_ctx_ref_key);

/*
static axis2_array_list_t *
sandesha2_sender_mgr_find_beans_with_msg_no(
    sandesha2_sender_mgr_t *sender,
    const axis2_env_t *env,
    axis2_array_list_t *list,
    long msg_no);
*/


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_SENDER_MGR_H */
