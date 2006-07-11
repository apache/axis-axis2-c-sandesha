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

#ifndef SANDESHA2_SEQUENCE_REPORT_H
#define SANDESHA2_SEQUENCE_REPORT_H

/**
 * @file sandesha2_sequence_report.h
 * @brief Sandesha Sequence Report Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_hash.h>
#include <axis2_uri.h>
#include <axis2_array_list.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_sequence_report sandesha2_sequence_report_t;
typedef struct sandesha2_sequence_report_ops sandesha2_sequence_report_ops_t;

#define SEQUENCE_STATUS_UNKNOWN 0
#define SEQUENCE_STATUS_INITIAL 1
#define SEQUENCE_STATUS_ESTABLISHED 2
#define SEQUENCE_STATUS_TERMINATED 3
#define SEQUENCE_STATUS_TIMED_OUT 4
#define MAX_SEQUENCE_STATUS 4

#define SEQUENCE_DIRECTION_UNKNOWN 0
#define SEQUENCE_DIRECTION_IN 1
#define SEQUENCE_DIRECTION_OUT 2
#define MAX_SEQUENCE_DIRECTION 2


/** @defgroup sandesha2_sequence_report Sequence Report
  * @ingroup sandesha2
  * @{
  */

struct sandesha2_sequence_report_ops
{
   /** 
     * Deallocate memory
     * @return status code
     */
    axis2_status_t (AXIS2_CALL *
    free) (
            void *seq_report,
            const axis2_env_t *env);
     
    axis2_status_t (AXIS2_CALL *
    set_sequence_status) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env,
            axis2_char_t sequence_status);

    axis2_status_t (AXIS2_CALL *
    set_sequence_direction) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env,
            axis2_char_t sequence_direction);

    axis2_char_t (AXIS2_CALL *
    get_sequence_status) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env);

    axis2_char_t (AXIS2_CALL *
    get_sequence_direction) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env);

    axis2_char_t *(AXIS2_CALL *
    get_sequence_id) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    set_sequence_id) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env,
            axis2_char_t *sequence_id);

    axis2_array_list_t *(AXIS2_CALL *
    get_completed_msgs) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env);

    axis2_status_t (AXIS2_CALL *
    add_completed_msg) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env,
            long *msg_no);

    axis2_status_t (AXIS2_CALL *
    set_completed_msgs) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env,
            axis2_array_list_t *completed_msgs);

    axis2_char_t *(AXIS2_CALL *
    get_internal_sequence_id) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env,
            long *msg_no);

    axis2_status_t (AXIS2_CALL *
    set_internal_sequence_id) (
            sandesha2_sequence_report_t *report,
            const axis2_env_t *env,
            axis2_char_t *internal_sequence_id);
   
};

struct sandesha2_sequence_report
{
    sandesha2_sequence_report_ops_t *ops;
};

AXIS2_EXTERN sandesha2_sequence_report_t * AXIS2_CALL
sandesha2_sequence_report_create(const axis2_env_t *env);

#define SANDESHA2_SEQUENCE_REPORT_FREE(seq_report, env) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->free (seq_report, env))

#define SANDESHA2_SEQUENCE_REPORT_SET_SEQUENCE_STATUS(seq_report, env, \
        sequence_status) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      set_sequence_status (seq_report, env, sequence_status))

#define SANDESHA2_SEQUENCE_REPORT_SET_SEQUENCE_DIRECTION(seq_report, env, \
        sequence_direction) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      set_sequence_direction (seq_report, env, sequence_direction))

#define SANDESHA2_SEQUENCE_REPORT_GET_SEQUENCE_STATUS(seq_report, env) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      get_sequence_status (seq_report, env))

#define SANDESHA2_SEQUENCE_REPORT_GET_SEQUENCE_DIRECTION(seq_report, env) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      get_sequence_direction (seq_report, env))

#define SANDESHA2_SEQUENCE_REPORT_GET_SEQUENCE_ID(seq_report, env) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      get_sequence_id (seq_report, env))

#define SANDESHA2_SEQUENCE_REPORT_SET_SEQUENCE_ID(seq_report, env, sequence_id) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      set_sequence_id (seq_report, env, sequence_id))

#define SANDESHA2_SEQUENCE_REPORT_GET_COMPLETED_MSGS(seq_report, env) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      get_completed_msgs (seq_report, env))

#define SANDESHA2_SEQUENCE_REPORT_ADD_COMPLETED_MSG(seq_report, env, msg_no) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      add_completed_msg (seq_report, env, msg_no))

#define SANDESHA2_SEQUENCE_REPORT_SET_COMPLETED_MSGS(seq_report, env, completed_msgs) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      set_completed_msgs (seq_report, env, completed_msgs))

#define SANDESHA2_SEQUENCE_REPORT_GET_INTERNAL_SEQUENCE_ID(seq_report, env) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      get_internal_sequence_id (seq_report, env))

#define SANDESHA2_SEQUENCE_REPORT_SET_INTERNAL_SEQUENCE_ID(seq_report, env, \
        sequence_id) \
      (((sandesha2_sequence_report_t *) seq_report)->ops->\
      set_internal_sequence_id (seq_report, env, sequence_id))


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_SEQUENCE_REPORT_H */
