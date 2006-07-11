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
 
#include <sandesha2_sequence_report.h>
#include <axis2_log.h>

typedef struct sandesha2_sequence_report_impl sandesha2_sequence_report_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_sequence_report_impl
{
    sandesha2_sequence_report_t report;

    axis2_char_t sequence_status;
	axis2_char_t sequence_direction;
	axis2_char_t *sequence_id;
	axis2_char_t *internal_sequence_id;   /* only for outgoing sequences */
	axis2_array_list_t *completed_msgs; /* no of messages acked (both for incoming and outgoing)*/
};

#define SANDESHA2_INTF_TO_IMPL(report) ((sandesha2_sequence_report_impl_t *) report)

axis2_status_t AXIS2_CALL 
sandesha2_sequence_report_free(
        void *report,
        const axis2_env_t *envv);

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_sequence_status(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t sequence_status);

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_sequence_direction(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t sequence_direction);

axis2_char_t AXIS2_CALL
sandesha2_sequence_report_get_sequence_status(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env);

axis2_char_t AXIS2_CALL
sandesha2_sequence_report_get_sequence_direction(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env);

axis2_char_t *AXIS2_CALL
sandesha2_sequence_report_get_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *sequence_id);

axis2_array_list_t *AXIS2_CALL
sandesha2_sequence_report_get_completed_msgs(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_add_completed_msg(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        long *msg_no);

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_completed_msgs(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_array_list_t *completed_msgs);

axis2_char_t *AXIS2_CALL
sandesha2_sequence_report_get_internal_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        long *msg_no);

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_internal_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *internal_sequence_id);

AXIS2_EXTERN sandesha2_sequence_report_t * AXIS2_CALL
sandesha2_sequence_report_create(
        const axis2_env_t *env)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    report_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_sequence_report_impl_t));

    report_impl->completed_msgs = NULL;
    report_impl->sequence_status = SEQUENCE_STATUS_UNKNOWN;
    report_impl->sequence_direction = SEQUENCE_DIRECTION_UNKNOWN;

    report_impl->report.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_sequence_report_ops_t)); 
    
    report_impl->completed_msgs = axis2_array_list_create(env, 0);
    if(!report_impl->completed_msgs) 
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    report_impl->report.ops->free = sandesha2_sequence_report_free;
    report_impl->report.ops->set_sequence_status = 
        sandesha2_sequence_report_set_sequence_status;
    report_impl->report.ops->set_sequence_direction = 
        sandesha2_sequence_report_set_sequence_direction;
    report_impl->report.ops->get_sequence_status = 
        sandesha2_sequence_report_get_sequence_status;
    report_impl->report.ops->get_sequence_direction = 
        sandesha2_sequence_report_get_sequence_direction;
    report_impl->report.ops->get_sequence_id = 
        sandesha2_sequence_report_get_sequence_id;
    report_impl->report.ops->set_sequence_id = 
        sandesha2_sequence_report_set_sequence_id;
    report_impl->report.ops->get_completed_msgs = 
        sandesha2_sequence_report_get_completed_msgs;
    report_impl->report.ops->add_completed_msg = 
        sandesha2_sequence_report_add_completed_msg;
    report_impl->report.ops->set_completed_msgs = 
        sandesha2_sequence_report_set_completed_msgs;
    report_impl->report.ops->get_internal_sequence_id = 
        sandesha2_sequence_report_get_internal_sequence_id;
    report_impl->report.ops->set_internal_sequence_id = 
        sandesha2_sequence_report_set_internal_sequence_id;

    return &(report_impl->report);
}

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_free(
        void *report,
        const axis2_env_t *env)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->completed_msgs)
    {
        AXIS2_ARRAY_LIST_FREE(report_impl->completed_msgs, env);
        report_impl->completed_msgs = NULL;
    }

    if(report_impl->sequence_id)
    {
        AXIS2_FREE(env->allocator, report_impl->sequence_id);
        report_impl->sequence_id = NULL;
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

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_sequence_status(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t sequence_status)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if (sequence_status >= SEQUENCE_STATUS_UNKNOWN && 
            sequence_status <= MAX_SEQUENCE_STATUS) 
    {
        report_impl->sequence_status = sequence_status;
    }
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_sequence_direction(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t sequence_direction)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if (sequence_direction >= SEQUENCE_DIRECTION_UNKNOWN && 
            sequence_direction <= MAX_SEQUENCE_DIRECTION) 
    {
        report_impl->sequence_direction = sequence_direction;
    }
    return AXIS2_SUCCESS;
}

axis2_char_t AXIS2_CALL
sandesha2_sequence_report_get_sequence_status(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->sequence_status;
}

axis2_char_t AXIS2_CALL
sandesha2_sequence_report_get_sequence_direction(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->sequence_direction;
}

axis2_char_t *AXIS2_CALL
sandesha2_sequence_report_get_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->sequence_id;
}

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *sequence_id)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->sequence_id)
    {
        AXIS2_FREE(env->allocator, report_impl->sequence_id);
        report_impl->sequence_id = NULL;
    }
    report_impl->sequence_id = AXIS2_STRDUP(sequence_id, env);
    if(!report_impl->sequence_id)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    return AXIS2_SUCCESS;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_sequence_report_get_completed_msgs(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->completed_msgs;
}

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_add_completed_msg(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        long *msg_no)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    AXIS2_ARRAY_LIST_ADD(report_impl->completed_msgs, env, msg_no);
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_completed_msgs(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_array_list_t *completed_msgs)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->completed_msgs)
    {
        int i = 0, size = 0;
        
        size = AXIS2_ARRAY_LIST_SIZE(report_impl->completed_msgs, env);
        for(i = 0; i < size; i++)
        {
            long *msg_no = NULL;
        
            msg_no = AXIS2_ARRAY_LIST_GET(report_impl->completed_msgs, env, i);
            AXIS2_FREE(env->allocator, msg_no);
        }
        AXIS2_ARRAY_LIST_FREE(report_impl->completed_msgs, env);
        report_impl->completed_msgs = NULL;
    }
    report_impl->completed_msgs = completed_msgs;
    return AXIS2_SUCCESS;
}

axis2_char_t *AXIS2_CALL
sandesha2_sequence_report_get_internal_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        long *msg_no)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->internal_sequence_id;
}

axis2_status_t AXIS2_CALL
sandesha2_sequence_report_set_internal_sequence_id(
        sandesha2_sequence_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *internal_sequence_id)
{
    sandesha2_sequence_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->internal_sequence_id)
    {
        AXIS2_FREE(env->allocator, report_impl->internal_sequence_id);
        report_impl->internal_sequence_id = NULL;
    }
    report_impl->internal_sequence_id = AXIS2_STRDUP(internal_sequence_id, env);
    if(!report_impl->internal_sequence_id)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    return AXIS2_SUCCESS;
}

