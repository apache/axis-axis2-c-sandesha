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
#include <sandesha2_sender_worker.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_seq.h>
#include <axis2_addr.h>
#include <axis2_engine.h>
#include <stdlib.h>
#include <axis2_http_transport.h>
#include <axis2_http_transport_utils.h>
#include <axiom_soap_const.h>
#include <axiom_soap_fault.h>
#include <axiom_soap_body.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_terminate_seq_res.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_msg_retrans_adjuster.h>
#include <platforms/axutil_platform_auto_sense.h>

/** 
 * @brief Sender struct impl
 *	Sandesha2 Sender Invoker
 */
typedef struct sandesha2_sender_worker_args sandesha2_sender_worker_args_t;

struct sandesha2_sender_worker_t
{
	axis2_conf_ctx_t *conf_ctx;
    axis2_char_t *msg_id;
    axis2_msg_ctx_t *msg_ctx;
    axis2_transport_out_desc_t *transport_out;
    axis2_bool_t persistent_msg_ctx;
};

struct sandesha2_sender_worker_args
{
    sandesha2_sender_worker_t *impl;
    axutil_env_t *env;
};

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_piggybackable_msg_type(
    sandesha2_sender_worker_t *sender_worker, 
    const axutil_env_t *env, 
    int msg_type);

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_ack_already_piggybacked(
    sandesha2_sender_worker_t *sender_worker, 
    const axutil_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx);

static axis2_status_t AXIS2_CALL
sandesha2_sender_worker_check_for_sync_res(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr,
    int msg_type);

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_fault_envelope(
    const axutil_env_t *env, 
    axiom_soap_envelope_t *soap_envelope);

axis2_status_t AXIS2_CALL 
sandesha2_sender_worker_free(
    sandesha2_sender_worker_t *sender_worker, 
    const axutil_env_t *env);								


AXIS2_EXTERN sandesha2_sender_worker_t* AXIS2_CALL
sandesha2_sender_worker_create(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id)
{
    sandesha2_sender_worker_t *sender_worker = NULL;
    sender_worker =  (sandesha2_sender_worker_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_sender_worker_t));
    if(!sender_worker)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sender_worker->conf_ctx = conf_ctx;
    sender_worker->msg_id = axutil_strdup(env, msg_id);
    sender_worker->msg_ctx = NULL;
    sender_worker->transport_out = NULL;
    
	return sender_worker;
}

AXIS2_EXTERN sandesha2_sender_worker_t* AXIS2_CALL
sandesha2_sender_worker_create_with_msg_ctx(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id,
    axis2_msg_ctx_t *msg_ctx)
{
    sandesha2_sender_worker_t *sender_worker = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    sender_worker =  (sandesha2_sender_worker_t *)AXIS2_MALLOC 
        (env->allocator, sizeof(sandesha2_sender_worker_t));
	
    if(!sender_worker)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sender_worker->conf_ctx = conf_ctx;
    sender_worker->msg_id = axutil_strdup(env, msg_id);
    sender_worker->msg_ctx = msg_ctx;
    sender_worker->transport_out = NULL;
    
	return sender_worker;
}

axis2_status_t AXIS2_CALL
sandesha2_sender_worker_free_void_arg(
    void *sender_worker,
    const axutil_env_t *env)
{
    sandesha2_sender_worker_t *sender_worker_l = NULL;
    sender_worker_l = (sandesha2_sender_worker_t *) sender_worker;
    return sandesha2_sender_worker_free(sender_worker_l, env);
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_worker_free(
    sandesha2_sender_worker_t *sender_worker, 
    const axutil_env_t *env)
{
    /* Do not free this */
    sender_worker->conf_ctx = NULL;
    
    if(sender_worker->msg_id)
    {
        AXIS2_FREE(env->allocator, sender_worker->msg_id);
        sender_worker->msg_id = NULL;
    }
	AXIS2_FREE(env->allocator, sender_worker);
	return AXIS2_SUCCESS;
}

axis2_status_t
sandesha2_sender_worker_send(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id,
    axis2_bool_t persistent_msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr,
    sandesha2_seq_property_mgr_t *seq_prop_mgr,
    sandesha2_create_seq_mgr_t *create_seq_mgr,
    sandesha2_sender_mgr_t *sender_mgr)
{
    sandesha2_sender_worker_t *sender_worker = NULL;
    sandesha2_sender_bean_t *sender_worker_bean = NULL;
    sandesha2_sender_bean_t *bean1 = NULL;
    axis2_char_t *key = NULL;
    axutil_property_t *property = NULL;
    axis2_bool_t continue_sending = AXIS2_TRUE;
    axis2_char_t *qualified_for_sending = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    sandesha2_property_bean_t *prop_bean = NULL;
    axutil_array_list_t *msgs_not_to_send = NULL;
    int msg_type = -1;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    axis2_bool_t successfully_sent = AXIS2_FALSE;
    axis2_status_t status = AXIS2_SUCCESS;
    axis2_bool_t resend = AXIS2_FALSE;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Entry:sandesha2_sender_worker_send");        
    
    sender_worker_bean = sandesha2_sender_mgr_retrieve(sender_mgr, env, msg_id);
    if(!sender_worker_bean)
    {
        AXIS2_LOG_WARNING(env->log, AXIS2_LOG_SI, 
            "[sandesha2]sender_worker_bean is NULL");
        return AXIS2_FAILURE;
    }
    key = sandesha2_sender_bean_get_msg_ctx_ref_key(sender_worker_bean, env);
    if(!msg_ctx)
    {
        if(persistent_msg_ctx)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Retrieving msg_ctx from database");
            msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, 
                key, conf_ctx, AXIS2_TRUE);
        }
        else
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "[sandesha2]Retrieving msg_ctx from configuration context");
            msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, 
                key, conf_ctx, AXIS2_FALSE);
        }
    }
    if(!msg_ctx)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]msg_ctx is not present in the store yet.");
        /*msg_ctx is still not stored so try again later.*/
        if(sender_worker_bean)
            sandesha2_sender_bean_free(sender_worker_bean, env);
        return AXIS2_SUCCESS;
    }
    continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env,
        sender_worker_bean, conf_ctx, storage_mgr, seq_prop_mgr, create_seq_mgr, 
        sender_mgr);
    sandesha2_sender_mgr_update(sender_mgr, env, sender_worker_bean);
    if(!continue_sending)
    {
        status = AXIS2_FAILURE;
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Do not continue sending the message");
        if(sender_worker_bean)
            sandesha2_sender_bean_free(sender_worker_bean, env);
        return status;
    }
    
    property = axis2_msg_ctx_get_property(msg_ctx, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING);
    if(property)
        qualified_for_sending = axutil_property_get_value(property, env);
    if(qualified_for_sending && 0 != axutil_strcmp(
        qualified_for_sending, AXIS2_VALUE_TRUE))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Message is not qualified for sending");

        if(sender_worker_bean)
            sandesha2_sender_bean_free(sender_worker_bean, env);
        /*return success here to wait till the message is qualified for sending*/
        return AXIS2_SUCCESS;
    }
    rm_msg_ctx = sandesha2_msg_init_init_msg(env, msg_ctx);
    
    prop_bean = sandesha2_utils_get_property_bean(env, 
        axis2_conf_ctx_get_conf(conf_ctx, env));
    if(prop_bean)
        msgs_not_to_send = sandesha2_property_bean_get_msg_types_to_drop(
            prop_bean, env);
    if(msgs_not_to_send)
    {
        int j = 0;
        axis2_bool_t continue_sending = AXIS2_FALSE;

        for(j = 0; j < axutil_array_list_size(msgs_not_to_send, env); j++)
        {
            axis2_char_t *value = NULL;
            int int_val = -1;
            int msg_type = -1;
            
            value = axutil_array_list_get(msgs_not_to_send, env, j);
            int_val = atoi(value);
            msg_type = sandesha2_msg_ctx_get_msg_type(rm_msg_ctx, env);
            if(msg_type == int_val)
                continue_sending = AXIS2_TRUE;
        }
        if(continue_sending)
        {
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]Continue "\
                "Sending is true. So returning from Sender Worker");
            if(sender_worker_bean)
                sandesha2_sender_bean_free(sender_worker_bean, env);
            if(rm_msg_ctx)
                sandesha2_msg_ctx_free(rm_msg_ctx, env);
            return AXIS2_SUCCESS;
        }
    }
    /* 
     *  This method is not implemented yet
     *  update_msg(sender_worker, env, msg_xtx);
     */
    msg_type = sandesha2_msg_ctx_get_msg_type(rm_msg_ctx, env);
    if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
    {
        sandesha2_seq_t *seq = NULL;
        axis2_char_t *seq_id = NULL;
        sandesha2_identifier_t *identifier = NULL;
        
        seq = sandesha2_msg_ctx_get_sequence(rm_msg_ctx, env);
        identifier = sandesha2_seq_get_identifier(seq, env);
        seq_id = sandesha2_identifier_get_identifier(identifier, env);
    }
    if(sandesha2_sender_worker_is_piggybackable_msg_type(sender_worker, env,
        msg_type) && AXIS2_FALSE  == 
        sandesha2_sender_worker_is_ack_already_piggybacked(sender_worker, env,
        rm_msg_ctx))
    {
        sandesha2_ack_mgr_piggyback_acks_if_present(env, rm_msg_ctx, 
            storage_mgr, seq_prop_mgr, sender_mgr);
    }
    
    if(!transport_out) 
        transport_out = axis2_msg_ctx_get_transport_out_desc(msg_ctx, env);
    transport_sender = axis2_transport_out_desc_get_sender(transport_out, env);
    if(transport_sender)
    {
        /* This is neccessary to avoid a double free */
        axis2_msg_ctx_set_property(msg_ctx, env, AXIS2_TRANSPORT_IN, NULL);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "dam_msg_id:%s", msg_id);
        if(AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, msg_ctx))
		{
        	successfully_sent = AXIS2_TRUE;
		}else
		{
        	successfully_sent = AXIS2_FALSE;
		}
    }
    msg_id = sandesha2_sender_bean_get_msg_id(sender_worker_bean, env);
    bean1 = sandesha2_sender_mgr_retrieve(sender_mgr, env, msg_id);
    if(bean1)
    { 
        resend = sandesha2_sender_bean_is_resend(sender_worker_bean, env);
        if(resend)
        {
            sandesha2_sender_bean_set_sent_count(bean1, env, 
                sandesha2_sender_bean_get_sent_count(sender_worker_bean, env));
            sandesha2_sender_bean_set_time_to_send(bean1, env, 
                sandesha2_sender_bean_get_time_to_send(sender_worker_bean, env));
            sandesha2_sender_mgr_update(sender_mgr, env, bean1);
        }
    }
    if(sender_worker_bean)
        sandesha2_sender_bean_free(sender_worker_bean, 
            env);
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]msg_type:%d", msg_type);
    if(successfully_sent)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]message of msg_type:%d successfully sent", msg_type);
        if(AXIS2_FALSE == axis2_msg_ctx_get_server_side(msg_ctx, env) ||
            msg_type == SANDESHA2_MSG_TYPE_ACK)
            sandesha2_sender_worker_check_for_sync_res(env, msg_ctx, 
                storage_mgr, msg_type);
    }
    if(SANDESHA2_MSG_TYPE_TERMINATE_SEQ == msg_type)
    {
        sandesha2_terminate_seq_t *terminate_seq = NULL;
        axis2_char_t *seq_id = NULL;
        axis2_conf_ctx_t *conf_ctx = NULL;
        axis2_char_t *internal_seq_id = NULL;
        
        terminate_seq = sandesha2_msg_ctx_get_terminate_seq(rm_msg_ctx, env);
        seq_id = sandesha2_identifier_get_identifier(
                    sandesha2_terminate_seq_get_identifier(terminate_seq, 
                    env), env);
        conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
        internal_seq_id = sandesha2_utils_get_seq_property(env, seq_id, 
            SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID, seq_prop_mgr);
        sandesha2_terminate_mgr_terminate_sending_side(env, conf_ctx,
            internal_seq_id, axis2_msg_ctx_get_server_side(msg_ctx, env), 
                storage_mgr, seq_prop_mgr, create_seq_mgr, sender_mgr);
        if(internal_seq_id)
            AXIS2_FREE(env->allocator, internal_seq_id);
        /* We have no more messages for this sequence. So continue send 
         * status is false*/
        status = AXIS2_FAILURE;
    }
    if(!resend && msg_type != SANDESHA2_MSG_TYPE_APPLICATION && 
        msg_type != SANDESHA2_MSG_TYPE_CREATE_SEQ)
    {
        axis2_char_t *msg_stored_key = NULL;
        if(bean1)
        {
            msg_id = sandesha2_sender_bean_get_msg_id(bean1, env); 
            sandesha2_sender_mgr_remove(sender_mgr, env, msg_id);
            msg_stored_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
                bean1, env);
            sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
                msg_stored_key, conf_ctx, -1);
        }
    }
    if(bean1)
        sandesha2_sender_bean_free(bean1, env);

    /*if(msg_ctx && msg_type != SANDESHA2_MSG_TYPE_APPLICATION && 
        msg_type != SANDESHA2_MSG_TYPE_CREATE_SEQ)
    {
        axis2_op_ctx_t *op_ctx = NULL;
        op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
        if(op_ctx)
            axis2_op_ctx_free(op_ctx, env);
        axis2_msg_ctx_set_keep_alive(msg_ctx, env, AXIS2_FALSE);
        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_FALSE);
        axis2_msg_ctx_free(msg_ctx, env);
    }*/
    if(rm_msg_ctx)
        sandesha2_msg_ctx_free(rm_msg_ctx, env);
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Exit:sandesha2_sender_worker_send");        
    return status;
}

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_piggybackable_msg_type(
    sandesha2_sender_worker_t *sender_worker, 
    const axutil_env_t *env, 
    int msg_type)
{
    if(SANDESHA2_MSG_TYPE_ACK == msg_type)
        return AXIS2_FALSE;
    
    return AXIS2_TRUE;
}

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_ack_already_piggybacked(
    sandesha2_sender_worker_t *sender_worker, 
    const axutil_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    if(sandesha2_msg_ctx_get_seq_ack(rm_msg_ctx, env))
        return AXIS2_TRUE;
    
    return AXIS2_FALSE;
}

static axis2_status_t AXIS2_CALL
sandesha2_sender_worker_check_for_sync_res(
    const axutil_env_t *env, 
    axis2_msg_ctx_t *msg_ctx,
    sandesha2_storage_mgr_t *storage_mgr,
    int msg_type)
{
    axutil_property_t *property = NULL;
    axis2_msg_ctx_t *res_msg_ctx = NULL;
    axiom_soap_envelope_t *res_envelope = NULL;
    axis2_char_t *soap_ns_uri = NULL;
    axis2_svc_ctx_t *svc_ctx = NULL;
    axis2_svc_grp_t *svc_grp = NULL;
    axis2_svc_t *svc = NULL;
    axis2_op_t *op = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_bool_t free_msg = AXIS2_FALSE;
   
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2]Entry:sandesha2_sender_worker_check_for_sync_res");
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);

    if(!msg_ctx)
        return AXIS2_SUCCESS;
    soap_ns_uri = axis2_msg_ctx_get_is_soap_11(msg_ctx, env) ?
         AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
         AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;

    res_envelope = axis2_msg_ctx_get_response_soap_envelope(msg_ctx, env);
    if(res_envelope && msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
    {
        axiom_soap_envelope_increment_ref(res_envelope, env);
        /* To avoid a second passing through incoming handlers at op_client*/
        property = axutil_property_create_with_args(env, 0, 0, 0, 
            AXIS2_VALUE_TRUE);
        axis2_msg_ctx_set_property(msg_ctx, env, AXIS2_HANDLER_ALREADY_VISITED, 
            property);
    }
    if(!res_envelope)
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Response envelope not found");
        res_envelope = axis2_http_transport_utils_create_soap_msg(env, msg_ctx,
            soap_ns_uri);
        free_msg = AXIS2_TRUE;
        if(!res_envelope)
        {
            return AXIS2_SUCCESS;
        }
    }
    conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    svc_grp = axis2_msg_ctx_get_svc_grp(msg_ctx, env);
    svc = axis2_msg_ctx_get_svc(msg_ctx, env);
    op = axis2_msg_ctx_get_op(msg_ctx, env);
    res_msg_ctx = axis2_msg_ctx_create(env, conf_ctx, 
        axis2_msg_ctx_get_transport_in_desc(msg_ctx, env), 
        axis2_msg_ctx_get_transport_out_desc(msg_ctx, env));
    if(svc_grp)
    {
        axis2_char_t *svc_grp_name = (axis2_char_t *)  axis2_svc_grp_get_name(
            svc_grp, env);
        svc_grp = axis2_conf_get_svc_grp(conf, env, svc_grp_name);
        if(svc_grp)
            axis2_msg_ctx_set_svc_grp(res_msg_ctx, env, svc_grp);
    }
    if (svc) 
    {
        axis2_char_t *svc_name = (axis2_char_t *) axis2_svc_get_name(svc, env);
        svc = axis2_conf_get_svc(conf, env, svc_name);
        if(svc)
            axis2_msg_ctx_set_svc(res_msg_ctx, env, svc);
    }
    if(op)
    {
        axutil_qname_t *qname = (axutil_qname_t *) axis2_op_get_qname(op, env);
        axis2_char_t *op_mep_str = NULL;
        axis2_char_t *op_name_str = NULL; 
        if(qname)
        {
            op_name_str = axutil_qname_to_string(qname, env);
        }
        op_mep_str = (axis2_char_t *) axis2_op_get_msg_exchange_pattern(op, env);
        if((op_name_str || op_mep_str))
        {
            axis2_op_t *op = NULL;
            if(op_name_str)
            {
                axutil_qname_t *op_qname = axutil_qname_create_from_string(env, 
                    op_name_str);
                op = axis2_svc_get_op_with_qname(svc, env, op_qname);
                if(op_qname)
                    axutil_qname_free(op_qname, env);
            }
            if(!op && op_mep_str && svc)
            {
                axutil_hash_t *all_ops = NULL;
                axutil_hash_index_t *index = NULL;
                /* Finding an operation using the MEP */
                all_ops = axis2_svc_get_all_ops(svc, env);
                for (index = axutil_hash_first(all_ops, env); index; index = 
                    axutil_hash_next(env, index))
                {
                    void *v = NULL;
                    axis2_char_t *mep = NULL;
                    axis2_op_t *temp = NULL;
                    axutil_hash_this(index, NULL, NULL, &v);
                    temp = (axis2_op_t *) v;
                    mep = (axis2_char_t *) axis2_op_get_msg_exchange_pattern(temp, 
                        env);
                    if(0 == axutil_strcmp(mep, op_mep_str))
                    {
                        op = temp;
                        break;
                    }
                }
            }
            if(op)
                axis2_msg_ctx_set_op(res_msg_ctx, env, op);
            else
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Cant find a suitable "\
                    "operation for the generated message");
                AXIS2_ERROR_SET(env->error, 
                    SANDESHA2_ERROR_CANNOT_FIND_OP_FOR_GENERATED_MSG, AXIS2_FAILURE);
                return AXIS2_FAILURE;
            }
        }
    }
    /* Setting contexts TODO is this necessary? */
    op = axis2_msg_ctx_get_op(res_msg_ctx, env);
    if(op)
    {
		axis2_op_ctx_t *op_ctx = NULL;
        axis2_svc_ctx_t *svc_ctx = axis2_msg_ctx_get_svc_ctx(res_msg_ctx, env);
		op_ctx = axis2_op_ctx_create(env, op, svc_ctx);
        if(op_ctx)
        {
            axis2_op_ctx_set_parent(op_ctx, env, svc_ctx);
            axis2_msg_ctx_set_op_ctx(res_msg_ctx, env, op_ctx);
        }
    }
    /*
     * Setting the message as serverSide will let it go through the 
     * Message Receiver (may be callback MR).
     */
    axis2_msg_ctx_set_server_side(res_msg_ctx, env, AXIS2_TRUE);

    svc_ctx = axis2_msg_ctx_get_svc_ctx(msg_ctx, env);
    axis2_msg_ctx_set_svc_ctx(res_msg_ctx, env, svc_ctx);
    axis2_msg_ctx_set_svc_grp_ctx(res_msg_ctx, env, 
        axis2_msg_ctx_get_svc_grp_ctx(msg_ctx, env));

    if(res_envelope)
    {
        axis2_engine_t *engine = NULL;
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Response envelope found");
        axis2_msg_ctx_set_soap_envelope(res_msg_ctx, env, res_envelope);
        engine = axis2_engine_create(env, axis2_msg_ctx_get_conf_ctx(msg_ctx, 
            env));
        if(AXIS2_TRUE == sandesha2_sender_worker_is_fault_envelope(env, 
            res_envelope))
            axis2_engine_receive_fault(engine, env, res_msg_ctx);
        else
            axis2_engine_receive(engine, env, res_msg_ctx);
        if(engine)
            axis2_engine_free(engine, env);
    }
    if(free_msg)
    {
        axiom_soap_envelope_free(res_envelope, env);
    }
    if(msg_type == SANDESHA2_MSG_TYPE_CREATE_SEQ)
    {
        axis2_op_ctx_t *op_ctx = axis2_msg_ctx_get_op_ctx(res_msg_ctx, env);
        axis2_op_ctx_free(op_ctx, env);
    }
    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI,
        "[sandesha2]Exit:sandesha2_sender_worker_check_for_sync_res");
    return AXIS2_SUCCESS;
}

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_fault_envelope(
    const axutil_env_t *env, 
    axiom_soap_envelope_t *soap_envelope)
{
    axiom_soap_fault_t *fault = NULL;
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    
    fault = axiom_soap_body_get_fault(axiom_soap_envelope_get_body(soap_envelope,
        env), env);
    if(fault)
        return AXIS2_TRUE;
        
    return AXIS2_FALSE;
}

void sandesha2_sender_worker_set_transport_out(
    sandesha2_sender_worker_t *sender_worker,
    const axutil_env_t *env,
    axis2_transport_out_desc_t *transport_out)
{
    sender_worker->transport_out = transport_out;
}

