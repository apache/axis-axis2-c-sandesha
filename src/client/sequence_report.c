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
 
#include <sandesha2_seq_report.h>
#include <axis2_log.h>

typedef struct sandesha2_seq_report_impl sandesha2_seq_report_impl_t;

/** 
 * @brief Sandesha Sequence Report Struct Impl
 *   Sandesha Sequence Report 
 */ 
struct sandesha2_seq_report_impl
{
    sandesha2_seq_report_t report;

    axis2_char_t seq_status;
	axis2_char_t seq_direction;
	axis2_char_t *seq_id;
	axis2_char_t *internal_seq_id;   /* only for outgoing seqs */
	axis2_array_list_t *completed_msgs; /* no of messages acked (both for incoming and outgoing)*/
};

#define SANDESHA2_INTF_TO_IMPL(report) ((sandesha2_seq_report_impl_t *) report)

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
sandesha2_seq_report_create(
        const axis2_env_t *env)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    report_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_seq_report_impl_t));

    report_impl->completed_msgs = NULL;
    report_impl->seq_status = SANDESHA2_SEQ_STATUS_UNKNOWN;
    report_impl->seq_direction = SANDESHA2_SEQ_DIRECTION_UNKNOWN;

    report_impl->report.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_seq_report_ops_t)); 
    
    report_impl->completed_msgs = axis2_array_list_create(env, 0);
    if(!report_impl->completed_msgs) 
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    report_impl->report.ops->free = sandesha2_seq_report_free;
    report_impl->report.ops->set_seq_status = 
        sandesha2_seq_report_set_seq_status;
    report_impl->report.ops->set_seq_direction = 
        sandesha2_seq_report_set_seq_direction;
    report_impl->report.ops->get_seq_status = 
        sandesha2_seq_report_get_seq_status;
    report_impl->report.ops->get_seq_direction = 
        sandesha2_seq_report_get_seq_direction;
    report_impl->report.ops->get_seq_id = 
        sandesha2_seq_report_get_seq_id;
    report_impl->report.ops->set_seq_id = 
        sandesha2_seq_report_set_seq_id;
    report_impl->report.ops->get_completed_msgs = 
        sandesha2_seq_report_get_completed_msgs;
    report_impl->report.ops->add_completed_msg = 
        sandesha2_seq_report_add_completed_msg;
    report_impl->report.ops->set_completed_msgs = 
        sandesha2_seq_report_set_completed_msgs;
    report_impl->report.ops->get_internal_seq_id = 
        sandesha2_seq_report_get_internal_seq_id;
    report_impl->report.ops->set_internal_seq_id = 
        sandesha2_seq_report_set_internal_seq_id;

    return &(report_impl->report);
}

axis2_status_t AXIS2_CALL
sandesha2_seq_report_free(
        void *report,
        const axis2_env_t *env)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->completed_msgs)
    {
        AXIS2_ARRAY_LIST_FREE(report_impl->completed_msgs, env);
        report_impl->completed_msgs = NULL;
    }

    if(report_impl->seq_id)
    {
        AXIS2_FREE(env->allocator, report_impl->seq_id);
        report_impl->seq_id = NULL;
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
sandesha2_seq_report_set_seq_status(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env,
        axis2_char_t seq_status)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if (seq_status >= SANDESHA2_SEQ_STATUS_UNKNOWN && 
            seq_status <= SANDESHA2_MAX_SEQ_STATUS) 
    {
        report_impl->seq_status = seq_status;
    }
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_seq_direction(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env,
        axis2_char_t seq_direction)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if (seq_direction >= SANDESHA2_SEQ_DIRECTION_UNKNOWN && 
            seq_direction <= SANDESHA2_MAX_SEQ_DIRECTION) 
    {
        report_impl->seq_direction = seq_direction;
    }
    return AXIS2_SUCCESS;
}

axis2_char_t AXIS2_CALL
sandesha2_seq_report_get_seq_status(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->seq_status;
}

axis2_char_t AXIS2_CALL
sandesha2_seq_report_get_seq_direction(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->seq_direction;
}

axis2_char_t *AXIS2_CALL
sandesha2_seq_report_get_seq_id(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->seq_id;
}

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_seq_id(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *seq_id)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->seq_id)
    {
        AXIS2_FREE(env->allocator, report_impl->seq_id);
        report_impl->seq_id = NULL;
    }
    report_impl->seq_id = AXIS2_STRDUP(seq_id, env);
    if(!report_impl->seq_id)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    return AXIS2_SUCCESS;
}

axis2_array_list_t *AXIS2_CALL
sandesha2_seq_report_get_completed_msgs(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->completed_msgs;
}

axis2_status_t AXIS2_CALL
sandesha2_seq_report_add_completed_msg(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env,
        long *msg_no)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    AXIS2_ARRAY_LIST_ADD(report_impl->completed_msgs, env, msg_no);
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_completed_msgs(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env,
        axis2_array_list_t *completed_msgs)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

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
sandesha2_seq_report_get_internal_seq_id(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env,
        long *msg_no)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    return report_impl->internal_seq_id;
}

axis2_status_t AXIS2_CALL
sandesha2_seq_report_set_internal_seq_id(
        sandesha2_seq_report_t *report,
        const axis2_env_t *env,
        axis2_char_t *internal_seq_id)
{
    sandesha2_seq_report_impl_t *report_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    report_impl = SANDESHA2_INTF_TO_IMPL(report);

    if(report_impl->internal_seq_id)
    {
        AXIS2_FREE(env->allocator, report_impl->internal_seq_id);
        report_impl->internal_seq_id = NULL;
    }
    report_impl->internal_seq_id = AXIS2_STRDUP(internal_seq_id, env);
    if(!report_impl->internal_seq_id)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    return AXIS2_SUCCESS;
}

