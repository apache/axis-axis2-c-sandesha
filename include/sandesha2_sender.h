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

#ifndef SANDESHA2_SENDER_H
#define SANDESHA2_SENDER_H

/**
 * @file sandesha2_sender.h
 * @brief Sandesha Sender Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_conf_ctx.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_sender sandesha2_sender_t;
typedef struct sandesha2_sender_ops sandesha2_sender_ops_t;

/** @defgroup sandesha2_sender Sender
  * @ingroup sandesha2
  * @{
  */

struct sandesha2_sender_ops
{
   /** 
     * Deallocate memory
     * @return status code
     */
    axis2_status_t (AXIS2_CALL *
    free) (
            sandesha2_sender_t *sender,
            const axis2_env_t *env);
     
    axis2_status_t (AXIS2_CALL *
    stop_sender_for_seq) (
            sandesha2_sender_t *sender,
            const axis2_env_t *env,
            axis2_char_t *seq_id);

    axis2_status_t (AXIS2_CALL *
    stop_sending) (
            sandesha2_sender_t *sender,
            const axis2_env_t *env);

    axis2_bool_t (AXIS2_CALL *
    is_sender_started) (
            sandesha2_sender_t *sender,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    run) (
            sandesha2_sender_t *sender,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    run_for_seq) (
            sandesha2_sender_t *sender,
            const axis2_env_t *env,
            axis2_conf_ctx_t *conf_ctx,
            axis2_char_t *seq_id);
};

struct sandesha2_sender
{
    sandesha2_sender_ops_t *ops;
};

AXIS2_EXTERN sandesha2_sender_t * AXIS2_CALL
sandesha2_sender_create(
        const axis2_env_t *env);

#define SANDESHA2_SENDER_FREE(sender, env) \
      (((sandesha2_sender_t *) sender)->ops->free (sender, env))

#define SANDESHA2_SENDER_STOP_FOR_SEQ(sender, env, \
        seq_id) \
      (((sandesha2_sender_t *) sender)->ops->\
      stop_sender_for_seq (sender, env, seq_id))

#define SANDESHA2_SENDER_STOP_SENDING(sender, env) \
      (((sandesha2_sender_t *) sender)->ops->\
      stop_sending (sender, env))

#define SANDESHA2_SENDER_IS_SENDER_STARTED(sender, env) \
      (((sandesha2_sender_t *) sender)->ops->\
      is_sender_started (sender, env))

#define SANDESHA2_SENDER_RUN(sender, env) \
      (((sandesha2_sender_t *) sender)->ops->\
      run (sender, env))

#define SANDESHA2_SENDER_RUN_FOR_SEQ(sender, env, conf_ctx, seq_id) \
      (((sandesha2_sender_t *) sender)->ops->\
      run_sender_for_seq (sender, env, conf_ctx, seq_id))

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_SENDER_H */
