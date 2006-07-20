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

#ifndef SANDESHA2_MSG_INIT_H
#define SANDESHA2_MSG_INIT_H

/**
 * @file sandesha2_msg_init.h
 * @brief Sandesha Message init Interface
 *  This class is used to create an RMMessageContext out of an MessageContext.
 * 
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_msg_ctx.h>
#include <axis2_msg_ctx.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_msg_init sandesha2_msg_init_t;
typedef struct sandesha2_msg_init_ops sandesha2_msg_init_ops_t;

/** @defgroup sandesha2_msg_init In Memory Message init
  * @ingroup sandesha2
  * @{
  */

/**
 * Called to create a rm_msg_ctx out of an message context. Finds out things 
 * like rm version and message type as well.
 * 
 * @param ctx
 * @param assumed_rm_ns
 * this is used for validation (to find out weather the rm_ns of the current 
 * message is equal to the regietered rm_ns of the sequence). 
 * If NULL validation will not happen.
 * 
 * @return
 */
sandesha2_msg_ctx_t *
sandesha2_msg_init_init_msg(
        const axis2_env_t *env,
        axis2_msg_ctx_t *ctx);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_MSG_INIT_H */
