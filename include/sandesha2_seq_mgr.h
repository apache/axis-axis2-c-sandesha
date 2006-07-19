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

#ifndef SANDESHA2_SEQ_MGR_H
#define SANDESHA2_SEQ_MGR_H

/**
 * @file sandesha2_seq_mgr.h
 * @brief Sandesha In Memory Sequence Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_msg_ctx.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_seq_mgr sandesha2_seq_mgr_t;
typedef struct sandesha2_seq_mgr_ops sandesha2_seq_mgr_ops_t;

/** @defgroup sandesha2_seq_mgr In Memory Sequence Manager
  * @ingroup sandesha2
  * @{
  */

AXIS_EXTERN axis2_char_t *AXIS2_CALL
sandesha2_seq_mgr_set_up_new_seq(
        const axis2_env_t *env,
        sandesha2_msg_ctx_t *create_seq_msg, 
        sandehsa2_storage_mgr_t *storage_mgr);
       
/**
 * Takes the internal_seq_id as the param. Not the seq_id
 * @param internal_seq_id
 * @param config_ctx
 */
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_seq_mgr_update_last_activated_time(
        const axis2_env_t *env,
        axis2_char_t *property_key,
        axis2_storage_mgr_t *storage_mgr);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_SEQ_MGR_H */
