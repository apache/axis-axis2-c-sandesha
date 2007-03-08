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

#ifndef SANDESHA2_SEQ_REPORT_H
#define SANDESHA2_SEQ_REPORT_H

/**
 * @file sandesha2_seq_report.h
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

typedef struct sandesha2_seq_report sandesha2_seq_report_t;

#define SANDESHA2_SEQ_STATUS_UNKNOWN 0
#define SANDESHA2_SEQ_STATUS_INITIAL 1
#define SANDESHA2_SEQ_STATUS_ESTABLISHED 2
#define SANDESHA2_SEQ_STATUS_TERMINATED 3
#define SANDESHA2_SEQ_STATUS_TIMED_OUT 4
#define SANDESHA2_MAX_SEQ_STATUS 4

#define SANDESHA2_SEQ_DIRECTION_UNKNOWN 0
#define SANDESHA2_SEQ_DIRECTION_IN 1
#define SANDESHA2_SEQ_DIRECTION_OUT 2
#define SANDESHA2_MAX_SEQ_DIRECTION 2


/** @defgroup sandesha2_seq_report Sequence Report
  * @ingroup sandesha2
  * @{
  */
axis2_status_t AXIS2_CALL 
sandesha2_seq_report_free(
    void *report,
    const axis2_env_t *envv);

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_seq_status(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env,
    axis2_char_t seq_status);

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_seq_direction(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env,
    axis2_char_t seq_direction);

axis2_char_t AXIS2_CALL
sandesha2_seq_report_get_seq_status(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env);

axis2_char_t AXIS2_CALL
sandesha2_seq_report_get_seq_direction(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env);

axis2_char_t *AXIS2_CALL
sandesha2_seq_report_get_seq_id(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_seq_id(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *seq_id);

axis2_array_list_t *AXIS2_CALL
sandesha2_seq_report_get_completed_msgs(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_seq_report_add_completed_msg(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env,
    long *msg_no);

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_completed_msgs(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env,
    axis2_array_list_t *completed_msgs);

axis2_char_t *AXIS2_CALL
sandesha2_seq_report_get_internal_seq_id(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env,
    long *msg_no);

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_internal_seq_id(
    sandesha2_seq_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *internal_seq_id);

AXIS2_EXTERN sandesha2_seq_report_t * AXIS2_CALL
sandesha2_seq_report_create(const axis2_env_t *env);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_SEQ_REPORT_H */
