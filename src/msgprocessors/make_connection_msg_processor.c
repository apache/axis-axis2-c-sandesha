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
#include <sandesha2_make_connection_msg_processor.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_fault_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_address.h>
#include <sandesha2_identifier.h>
#include <sandesha2_sender_worker.h>
#include <sandesha2_msg_pending.h>
#include <axis2_msg_ctx.h>
#include <axis2_string.h>
#include <axis2_engine.h>
#include <axiom_soap_const.h>
#include <axis2_msg_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_core_utils.h>
#include <axis2_uuid_gen.h>
#include <axis2_endpoint_ref.h>
#include <axis2_op_ctx.h>
#include <axis2_transport_out_desc.h>

/** 
 * @brief Make Connection Message Processor struct impl
 *	Sandesha2 Make Connection Msg Processor
 */
typedef struct sandesha2_make_connnection_msg_processor_impl 
                        sandesha2_make_connnection_msg_processor_impl_t;  
  
struct sandesha2_make_connnection_msg_processor_impl
{
	sandesha2_msg_processor_t msg_processor;
};

#define SANDESHA2_INTF_TO_IMPL(msg_proc) \
    ((sandesha2_make_connnection_msg_processor_impl_t *)(msg_proc))

/***************************** Function headers *******************************/
static axis2_status_t AXIS2_CALL 
sandesha2_make_connnection_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx);

static void 
add_msg_pending_header(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *return_msg_ctx,
    axis2_bool_t pending);
    
static void
set_transport_properties(
    const axis2_env_t *env,
    axis2_msg_ctx_t *return_msg_ctx,
    sandesha2_msg_ctx_t *make_conn_msg_ctx);

static axis2_status_t AXIS2_CALL 
sandesha2_make_connnection_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
	const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_msg_processor_t* AXIS2_CALL
sandesha2_make_connnection_msg_processor_create(
    const axis2_env_t *env)
{
    sandesha2_make_connnection_msg_processor_impl_t *msg_proc_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
              
    msg_proc_impl =  (sandesha2_make_connnection_msg_processor_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_make_connnection_msg_processor_impl_t));
	
    if(!msg_proc_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_msg_processor_ops_t));
    if(!msg_proc_impl->msg_processor.ops)
	{
		sandesha2_make_connnection_msg_processor_free((sandesha2_msg_processor_t*)
                         msg_proc_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    msg_proc_impl->msg_processor.ops->process_in_msg = 
                        sandesha2_make_connnection_msg_processor_process_in_msg;
    msg_proc_impl->msg_processor.ops->free = sandesha2_make_connnection_msg_processor_free;
                        
	return &(msg_proc_impl->msg_processor);
}


static axis2_status_t AXIS2_CALL 
sandesha2_make_connnection_msg_processor_free (
    sandesha2_msg_processor_t *msg_processor, 
	const axis2_env_t *env)
{
    sandesha2_make_connnection_msg_processor_impl_t *msg_proc_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_proc_impl = SANDESHA2_INTF_TO_IMPL(msg_processor);
    
    if(msg_processor->ops)
        AXIS2_FREE(env->allocator, msg_processor->ops);
    
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(msg_processor));
	return AXIS2_SUCCESS;
}

/**
 * Prosesses incoming MakeConnection request messages.
 * A message is selected by the set of SenderBeans that are waiting to be sent.
 * This is processed using a SenderWorker.
 */
static axis2_status_t AXIS2_CALL 
sandesha2_make_connnection_msg_processor_process_in_msg (
    sandesha2_msg_processor_t *msg_processor,
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    sandesha2_make_connection_t *make_conn = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_sender_mgr_t *sender_bean_mgr = NULL;
    sandesha2_sender_bean_t *find_sender_bean = NULL;
    sandesha2_sender_bean_t *sender_bean = NULL;
    sandesha2_address_t *address = NULL;
    sandesha2_identifier_t *identifier = NULL;
    sandesha2_msg_ctx_t *return_rm_msg_ctx = NULL;
    axis2_property_t *property = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_msg_ctx_t *return_msg_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_array_list_t *retrans_list = NULL;
    axis2_bool_t pending = AXIS2_FALSE;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_char_t *msg_storage_key = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axis2_char_t *msg_id = NULL;
    int size = 0, item_to_pick = -1, item = 0;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    AXIS2_LOG_INFO(env->log, "[sandesha2] sandesha2_make_connnection_msg_" \
            "processor_process_in_msg .........");
    printf("sandesha2_make_connnection_msg_processor_process_in_msg\n");

    make_conn = (sandesha2_make_connnection_t*)
        sandesha2_msg_ctx_get_msg_part(rm_msg_ctx, env, 
                SANDESHA2_MSG_PART_MAKE_CONNECTION);
    if(!make_conn)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]"
            " make_connnection part is null");
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_REQD_MSG_PART_MISSING,
                        AXIS2_FAILURE);
        return AXIS2_FAILURE;        
    }
    address = sandesha2_make_connection_get_address(make_conn, env);
    identifier = sandesha2_seq_make_connnection_get_identifier(
            seq_make_connnection, env);
    if(identifier)
        seq_id = sandesha2_identifier_get_identifier(identifier, env);
    msg_ctx = sandesha2_msg_ctx_get_msg_ctx(rm_msg_ctx, env);
    if(msg_ctx)
        conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
    if(conf_ctx)
        storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(conf_ctx, env));
    if(storage_mgr)
        sender_bean_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
    /* Selecting the set of sender beans that suit the given criteria */
    find_sender_bean = sandesha2_sender_bean_create(env);
    if(find_sender_bean)
        sandesha2_sender_bean_set_send(find_sender_bean, env, AXIS2_TRUE);
    if(address)
    {
        axis2_endpoint_ref_t *epr = NULL;
        const axis2_char_t *epr_address = NULL;
        epr = sandesha2_address_get_epr(address, env);
        if(epr)
            epr_address = sandesha2_endpoint_ref_get_address(epr, env);
        if(epr_address)
            sandesha2_sender_bean_set_wsrm_anon_uri(find_sender_bean, env, 
                epr_address);
    }
    if(seq_id)
        sandesha2_sender_bean_set_seq_id(find_sender_bean, env, seq_id);
    /* Find the beans that go with the criteria of the passed sender bean */
    if(find_sender_bean)
    {
        int i = 0, size = 0;
        retrans_list = axis2_array_list_create(env);
        if(!retrans_list)
            return AXIS2_FAILURE;
        axis2_array_list_t *retrans_list1 = NULL, *retrans_list2 = NULL;
        sandesha2_sender_bean_set_resend(find_sender_bean, env, AXIS2_TRUE);
        retrans_list1 = sandesha2_sender_mgr_find_by_sender_bean(retrans_mgr, 
                env, find_sender_bean);
        sandesha2_sender_bean_set_resend(find_sender_bean, env, AXIS2_FALSE);
        retrans_list2 = sandesha2_sender_mgr_find_by_sender_bean(retrans_mgr, 
                env, find_sender_bean);
        if(retrans_list1)
            size = AXIS2_ARRAY_LIST_SIZE(retrans_list1, env);
        for(i = 0; i < size; i++)
        {
            sandesha2_sender_bean_t *temp_bean = NULL;
            temp_bean = AXIS2_ARRAY_LIST_GET(retrans_list1, env, i);
            AXIS2_ARRAY_LIST_ADD(retrans_list, env, temp_bean);
        }
        if(retrans_list2)
            size = AXIS2_ARRAY_LIST_SIZE(retrans_list2, env);
        for(i = 0; i < size; i++)
        {
            sandesha2_sender_bean_t *temp_bean = NULL;
            temp_bean = AXIS2_ARRAY_LIST_GET(retrans_list2, env, i);
            AXIS2_ARRAY_LIST_ADD(retrans_list, env, temp_bean);
        }
    }
    /* Selecting a bean to send randomly. TODO - Should use a better mechanism */
    if(retrans_list)
        size = AXIS2_ARRAY_LIST_SIZE(retrans_list, env);
    if(size > 0)
    {
       unsigned int rand_var = axis2_rand_get_seed_value_based_on_time(env);
       item_to_pick = axis2_rand_with_range(&rand_var, 1, size);
    }
    if(size > 1)
        pending = AXIS2_TRUE; /*There are more than one message to be delivered 
                               using the makeConnection. So the MessagePending 
                               header should have value true;*/

    for(itemp = 0; item < size; item++)
    {
        sender_bean = (sandesha2_sender_bean_t *) AXIS2_ARRAY_LIST_GET(
                retrans_list, env, item);
        if(item == item_to_pick)
            break;
    }
    if(!sender_bean)
        return AXIS2_SUCCESS;
    transport_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx, env);
    if(!transport_out)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_CANNOT_INFER_TRANSPORT, 
                AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    msg_storage_key = sandesha2_sender_bean_get_msg_ctx_ref_key(sender_bean, 
            env);
    return_msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, 
            msg_storage_key, conf_ctx);
    return_rm_msg_ctx = sandesha2_msg_init_init_msg(env, return_msg_ctx);
    add_msg_pending_header(env, return_rm_msg_ctx, pending);
    set_transport_properties(env, return_msg_ctx, rm_msg_ctx);
    /* Setting that the response gets written. This will be used by transports.*/
    if(msg_ctx)
        op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env);
    if(op_ctx)
    {
        ctx = AXIS2_OP_CTX_GET_BASE(op_ctx, env);
        if (ctx)
        {
            axis2_char_t *value = NULL;
            value = AXIS2_STRDUP("TRUE", env);
            property = axis2_property_create(env);
            AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
            AXIS2_PROPERTY_SET_VALUE(property, env, value);
            AXIS2_CTX_SET_PROPERTY(ctx, env, AXIS2_RESPONSE_WRITTEN,
                                        property, AXIS2_FALSE);
        }
    }
    /*
     *running the make_connection through a sender_worker.
     *This will allow Sandesha2 to consider both of following senarios equally.
     * 1. A message being sent by the Sender thread.
     * 2. A message being sent as a reply to an make_connection.
     */
    msg_id = sandesha2_sender_bean_get_msg_id(sender_bean, env);
    if(msg_id)
    {
        sandesha2_sender_worker_t *sender_worker = NULL;
        /* Start a sender worker which will work on this message */
        sender_worker = sandesha2_sender_worker_create(env, conf_ctx, msg_id);
        sandesha2_sender_worker_set_transport_out(sender_worker, env, transport_out); 
        sandesha2_sender_worker_run(sender_worker, env);
    }
}

static void 
add_msg_pending_header(
    const axis2_env_t *env,
    sandesha2_msg_ctx_t *return_msg_ctx,
    axis2_bool_t pending)
{
    axiom_soap_envelope_t *soap_env = sandesha2_msg_ctx_get_soap_envelope(
            return_msg_ctx, env);
    axis2_char_t *rm_ns = sandesha2_msg_ctx_get_rm_ns_val(return_msg_ctx, env);
    sandesha2_msg_pending_t *msg_pending = sandesha2_msg_pending_create(env, 
            rn_ns);
    sandesha2_msg_pending_set_pending(msg_pending, env, pending);
    sandesha2_msg_pending_to_soap_envelope(msg_pending, env, soap_env);
}

static void
set_transport_properties(
    const axis2_env_t *env,
    axis2_msg_ctx_t *return_msg_ctx,
    sandesha2_msg_ctx_t *make_conn_msg_ctx)
{
    axis2_property_t *property = NULL;
    property = sandesha2_msg_ctx_get_property(make_conn_ctx, env, 
            AXIS2_TRANSPORT_OUT);
    AXIS2_MSG_CTX_SET_PROPERTY(return_msg_ctx, env, AXIS2_TRANSPORT_OUT, 
            property, AXIS2_FALSE);
    property = sandesha2_msg_ctx_get_property(make_conn_ctx, env, 
            AXIS2_HTTP_OUT_TRANSPORT_INFO);
    AXIS2_MSG_CTX_SET_PROPERTY(return_msg_ctx, env, 
            AXIS2_HTTP_OUT_TRANSPORT_INFO, property, AXIS2_FALSE);
}

