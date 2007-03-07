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
 
#include <sandesha2_report.h>
#include <sandesha2_seq_report.h>
#include <axis2_log.h>

typedef struct sandesha2_report_impl sandesha2_report_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_report_impl
{
    sandesha2_report_t report;

    axis2_array_list_t *incoming_seq_list;
    axis2_array_list_t *outgoing_seq_list;
    axis2_hash_t *seq_status_map;
    axis2_hash_t *no_of_completed_msgs_map;
    axis2_hash_t *outgoing_internal_seq_id_map;
};

#define SANDESHA2_INTF_TO_IMPL(report) ((sandesha2_report_impl_t *) report)

axis2_status_t AXIS2_CALL 
sandesha2_report_free(
        void *report,
        const axis2_env_t *envv);

long AXIS2_CALL
sandesha2_report_get_completed_msgs_count(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *seq_id);

axis2_array_list_t *AXIS2_CALL
sandesha2_report_get_incoming_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env);

axis2_array_list_t *AXIS2_CALL
sandesha2_report_get_outgoing_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env);

axis2_char_t AXIS2_CALL
sandesha2_report_get_seq_status_map(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *seq_id);

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_incoming_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *incoming_seq_id);

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_outgoing_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *out_seq_id);

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_no_of_completed_msgs_map(
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *id,
    long no_of_msgs);
	
axis2_status_t AXIS2_CALL
sandesha2_report_add_to_seq_status_map(
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
sandesha2_report_create(
        const axis2_env_t *env)
{
    sandesha2_report_impl_t *report_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    report_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_report_impl_t));

    report_impl->incoming_seq_list = NULL;
    report_impl->outgoing_seq_list = NULL;
    report_impl->seq_status_map = NULL;
    report_impl->no_of_completed_msgs_map = NULL;
    report_impl->outgoing_internal_seq_id_map = NULL;

    report_impl->report.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_report_ops_t)); 
    
    report_impl->incoming_seq_list = axis2_array_list_create(env, 0);
    report_impl->outgoing_seq_list = axis2_array_list_create(env, 0);
    if(!report_impl->incoming_seq_list ||
       !report_impl->outgoing_seq_list) 
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    report_impl->seq_status_map = axis2_hash_make(env);
    report_impl->no_of_completed_msgs_map = axis2_hash_make(env);
    report_impl->outgoing_internal_seq_id_map = axis2_hash_make(env);
    if(!report_impl->seq_status_map ||
        !report_impl->no_of_completed_msgs_map ||
        !report_impl->outgoing_internal_seq_id_map) 
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }

    report_impl->report.ops->free = sandesha2_report_free;
    report_impl->report.ops->get_completed_msgs_count = 
        sandesha2_report_get_completed_msgs_count;
    report_impl->report.ops->get_incoming_seq_list = 
        sandesha2_report_get_incoming_seq_list;
    report_impl->report.ops->get_outgoing_seq_list = 
        sandesha2_report_get_outgoing_seq_list;
    report_impl->report.ops->get_seq_status_map = 
        sandesha2_report_get_seq_status_map;
    report_impl->report.ops->add_to_incoming_seq_list = 
        sandesha2_report_add_to_incoming_seq_list;
    report_impl->report.ops->add_to_outgoing_seq_list = 
        sandesha2_report_add_to_outgoing_seq_list;
    report_impl->report.ops->add_to_no_of_completed_msgs_map = 
        sandesha2_report_add_to_no_of_completed_msgs_map;
    report_impl->report.ops->add_to_seq_status_map = 
        sandesha2_report_add_to_seq_status_map;
    report_impl->report.ops->get_internal_seq_id_of_out_seq = 
        sandesha2_report_get_internal_seq_id_of_out_seq;
    report_impl->report.ops->add_to_outgoing_internal_seq_map = 
        sandesha2_report_add_to_outgoing_internal_seq_map;

    return &(report_impl->report);
}

axis2_status_t AXIS2_CALL
sandesha2_report_free(
        void *report,
        const axis2_env_t *env)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->incoming_seq_list)
    {
        axis2_array_list_free(report_impl->incoming_seq_list, env);
        report_impl->incoming_seq_list = NULL;
    }

    if(report_impl->outgoing_seq_list)
    {
        axis2_array_list_free(report_impl->outgoing_seq_list, env);
        report_impl->outgoing_seq_list = NULL;
    }

    if(report_impl->seq_status_map)
    {
        axis2_hash_free(report_impl->seq_status_map, env);
        report_impl->seq_status_map = NULL;
    }

    if(report_impl->no_of_completed_msgs_map)
    {
        axis2_hash_free(report_impl->no_of_completed_msgs_map, env);
        report_impl->no_of_completed_msgs_map = NULL;
    }

    if(report_impl->outgoing_internal_seq_id_map)
    {
        axis2_hash_free(report_impl->outgoing_internal_seq_id_map, env);
        report_impl->outgoing_internal_seq_id_map = NULL;
    }
    
    if((&(report_impl->report))->ops)
    {
        AXIS2_FREE(env->allocator, (&(report_impl->report))->ops);
        (&(report_impl->report))->ops = NULL;
    }

    if(report_impl)
    {
        AXIS2_FREE(env->allocator, report_impl);
        report_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

long AXIS2_CALL
sandesha2_report_get_completed_msgs_count(
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *seq_id)
{
    sandesha2_report_impl_t *report_impl = NULL;
    long *lng = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    lng = (long *)axis2_hash_get(report_impl->no_of_completed_msgs_map, seq_id, 
        AXIS2_HASH_KEY_STRING);
    if(!lng)
    {
        return -1;
    }
    return *lng;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_report_get_incoming_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);
    return report_impl->incoming_seq_list;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_report_get_outgoing_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);
    return report_impl->outgoing_seq_list;
}

axis2_char_t AXIS2_CALL
sandesha2_report_get_seq_status_map(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *seq_id)
{
    sandesha2_report_impl_t *report_impl = NULL;
    axis2_char_t *status = NULL;
    

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    status = (axis2_char_t *) axis2_hash_get(report_impl->seq_status_map, 
            seq_id, AXIS2_HASH_KEY_STRING);
    if(!status)
        return SANDESHA2_SEQ_STATUS_UNKNOWN;
    return *status;
}

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_incoming_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *incoming_seq_id)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    axis2_array_list_add(report_impl->incoming_seq_list, env, incoming_seq_id);
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_outgoing_seq_list(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *out_seq_id)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    axis2_array_list_add(report_impl->outgoing_seq_list, env, out_seq_id);
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_no_of_completed_msgs_map(
    sandesha2_report_t *report,
    const axis2_env_t *env,
    axis2_char_t *id,
    long no_of_msgs)
{
    sandesha2_report_impl_t *report_impl = NULL;
    long *no_of_msgs_l = AXIS2_MALLOC(env->allocator, sizeof(long));

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    *no_of_msgs_l = no_of_msgs;
    axis2_hash_set(report_impl->no_of_completed_msgs_map, id, 
        AXIS2_HASH_KEY_STRING, no_of_msgs_l);
    return AXIS2_SUCCESS;
}
	
axis2_status_t AXIS2_CALL
sandesha2_report_add_to_seq_status_map(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *id,
        axis2_char_t status)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    axis2_hash_set(report_impl->seq_status_map, id, 
            AXIS2_HASH_KEY_STRING, &status);
    return AXIS2_SUCCESS;
}

axis2_char_t *AXIS2_CALL
sandesha2_report_get_internal_seq_id_of_out_seq(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *out_seq_id)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);
    return (axis2_char_t *) axis2_hash_get(
            report_impl->outgoing_internal_seq_id_map, out_seq_id, 
            AXIS2_HASH_KEY_STRING);
}

axis2_status_t AXIS2_CALL
sandesha2_report_add_to_outgoing_internal_seq_map(
        sandesha2_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *out_seq_id,
        axis2_char_t *internal_seq_id)
{
    sandesha2_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    axis2_hash_set(report_impl->outgoing_internal_seq_id_map, out_seq_id, 
            AXIS2_HASH_KEY_STRING, internal_seq_id);
    return AXIS2_SUCCESS;
}
	
