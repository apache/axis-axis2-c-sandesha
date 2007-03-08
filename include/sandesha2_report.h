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

#ifndef SANDESHA2_REPORT_H
#define SANDESHA2_REPORT_H

/**
 * @file sandesha2_report.h
 * @brief Sandesha Report Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_hash.h>
#include <axis2_array_list.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_report sandesha2_report_t;

/** @defgroup sandesha2_report Report
  * @ingroup sandesha2
  * @{
  */

/** 
 * Deallocate memory
 * @return status code
 */
axis2_status_t AXIS2_CALL
sandesha2_report_free(
    void *report,
    const axis2_env_t *env);
   
long AXIS2_CALL 
sandesha2_report_get_completed_msgs_count (
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *seq_id);

axis2_array_list_t *AXIS2_CALL
sandesha2_report_get_incoming_seq_list (
    sandesha2_report_t *report,
    const axis2_env_t *env);

axis2_array_list_t *AXIS2_CALL
sandesha2_report_get_outgoing_seq_list (
    sandesha2_report_t *report,
    const axis2_env_t *env);

axis2_char_t AXIS2_CALL
sandesha2_report_get_seq_status_map (
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *seq_id);

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_incoming_seq_list (
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *incoming_seq_id);

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_outgoing_seq_list (
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *out_seq_id);

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_no_of_completed_msgs_map (
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *id,
    long no_of_msgs);
    
axis2_status_t AXIS2_CALL
sandesha2_report_add_to_seq_status_map (
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *id,
    axis2_char_t status);

axis2_char_t *AXIS2_CALL
sandesha2_report_get_internal_seq_id_of_out_seq(
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *out_seq_id);

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_outgoing_internal_seq_map(
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *out_seq_id,
    axis2_char_t *internal_seq_id);

AXIS2_EXTERN sandesha2_report_t * AXIS2_CALL
sandesha2_report_create(const axis2_env_t *env);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_REPORT_H */
