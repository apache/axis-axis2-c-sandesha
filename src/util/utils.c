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
 
#include <sandesha2/sandesha2_utils.h>
#include <sys/timeb.h>
#include <axis2_param.h>
#include <sandesha2/sandesha2_constants.h>
#include <sandesha2/sandesha2_in_order_invoker.h>
#include <sandesha2/sandesha2_transport_sender.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2/sandesha2_error.h>
#include <sandesha2_sender.h>
#include <axis2_string.h>
#include <axis2_conf.h>
#include <axis2_property.h>
#include <axiom_soap_body.h>
#include <axis2_options.h>
#include <axis2_msg_ctx.h>
#include <axis2_transport_out_desc.h>
#include <axis2_transport_in_desc.h>
#include <axis2_qname.h>
#include <axis2_http_transport.h>
#include <axis2_addr.h>
#include <axiom_soap_header.h>
#include <sandesha2/sandesha2_seq.h>
#include <sandesha2/sandesha2_seq_ack.h>
#include <sandesha2/sandesha2_ack_requested.h>
#include <sandesha2/sandesha2_close_seq.h>
#include <sandesha2/sandesha2_close_seq_res.h>

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_utils_remove_soap_body_part(const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope, axis2_qname_t *qname)
{
    axiom_soap_body_t *soap_body = NULL;
    axiom_node_t *body_node = NULL;
    axiom_node_t *body_rm_node = NULL;
    axiom_element_t *body_element = NULL;
    axiom_element_t *body_rm_element = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, envelope, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, qname, AXIS2_FAILURE);
    
    soap_body = AXIOM_SOAP_ENVELOPE_GET_BODY(envelope, env);
    if(NULL == soap_body)
    {
        return AXIS2_FAILURE;
    }
    body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(soap_body, env);
    if(NULL == body_node)
    {
        return AXIS2_FAILURE;
    }
    body_element = AXIOM_NODE_GET_DATA_ELEMENT(body_node, env);
    if(NULL == body_element)
    {
        return AXIS2_FAILURE;
    }
    body_rm_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(body_element,
                            env, qname, body_node, &body_rm_node);
    if(NULL != body_rm_element)
    {
        AXIOM_NODE_DETACH(body_rm_node, env);
    }
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN long AXIS2_CALL
sandesha2_utils_get_current_time_in_millis(
        const axis2_env_t *env)
{
    const long fixed_time = 1153918446;
    long seconds = -1;
    long millis = -1;
    struct timeb *tp = AXIS2_MALLOC(env->allocator, sizeof(struct timeb));
    ftime(tp);
    /* To prevent an overflow we substract a contstant from seconds value
     * This value is taken as 18.23.xx seconds on 26 Jul 2006
     *
     */
    seconds = tp->time;
    seconds -= fixed_time;
    seconds *= 1000;
    millis = tp->millitm;
    millis = millis + seconds;

    return millis;
}

AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_rm_version(const axis2_env_t *env,
                        axis2_char_t *key,
                        sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_property_mgr_t *seq_prop_man = NULL;
    sandesha2_seq_property_bean_t *rm_version_bean = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, key, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    
    seq_prop_man = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_man, env);
    rm_version_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_man, 
                        env, key, SANDESHA2_SEQ_PROP_RM_SPEC_VERSION);
    if(NULL == rm_version_bean)
        return NULL;
    return SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(rm_version_bean, env);
}

AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_utils_get_storage_mgr(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_conf_t *conf)
{
    axis2_param_t *parameter = NULL;
    axis2_char_t *value = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);
    AXIS2_PARAM_CHECK(env->error, conf, NULL);
    
    parameter = AXIS2_CONF_GET_PARAM(conf, env, 
                        SANDESHA2_STORAGE_MGR_PARAMETER);
    if(NULL == parameter)
        parameter = axis2_param_create(env, SANDESHA2_STORAGE_MGR_PARAMETER,
                        SANDESHA2_DEFAULT_STORAGE_MGR);
    value = AXIS2_PARAM_GET_VALUE(parameter, env);
    if(0 == AXIS2_STRCMP(value, SANDESHA2_INMEMORY_STORAGE_MGR))
        return sandesha2_utils_get_inmemory_storage_mgr(env, conf_ctx);
    else if (0 == AXIS2_STRCMP(value, SANDESHA2_PERMANENT_STORAGE_MGR))
        return sandesha2_utils_get_permanent_storage_mgr(env, conf_ctx);
    else
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNKNOWN_STORAGE_MGR,
                        AXIS2_FAILURE);
        return NULL;
    }
    return NULL;
}
                        
AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_seq_property(const axis2_env_t *env,
                        axis2_char_t *id,
                        axis2_char_t *name,
                        sandesha2_storage_mgr_t *storage_man)
{
    sandesha2_seq_property_mgr_t *seq_prop_man = NULL;
    sandesha2_seq_property_bean_t *seq_prop_bean = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, id, NULL);
    AXIS2_PARAM_CHECK(env->error, name, NULL);
    AXIS2_PARAM_CHECK(env->error, storage_man, NULL);
    
    seq_prop_man = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_man, env);
    seq_prop_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_man,
                        env, id, name);
    if(NULL == seq_prop_bean)
        return NULL;
    return  SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(seq_prop_bean, env);
    return NULL;
}

AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_utils_get_property_bean(const axis2_env_t *env,
                        axis2_conf_t *conf)
{
    axis2_param_t *param = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, conf, NULL);
    
    param = AXIS2_CONF_GET_PARAM(conf, env, SANDESHA2_SANDESHA_PROPERTY_BEAN);
    if(NULL == param)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONFIGURATION_NOT_SET,
                        AXIS2_FAILURE);
        return NULL;
    }
    return (sandesha2_property_bean_t*)AXIS2_PARAM_GET_VALUE(param, env);
    
}

AXIS2_EXTERN axis2_array_list_t* AXIS2_CALL
sandesha2_utils_get_array_list_from_string(const axis2_env_t *env,
                        axis2_char_t *string)
{
    axis2_char_t *dup_string = NULL;
    axis2_char_t *temp_str = NULL;
    axis2_bool_t added = AXIS2_FALSE;
    axis2_array_list_t *ret = axis2_array_list_create(env, 
                        AXIS2_ARRAY_LIST_DEFAULT_CAPACITY);
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, string, NULL);
    /* remove the array markers - []  if present */
    if('[' == *string)
        temp_str = string + sizeof(axis2_char_t);
    else
        temp_str = string;
    dup_string = AXIS2_STRDUP(temp_str, env);
    if(']' == dup_string[AXIS2_STRLEN(dup_string)])
        dup_string[AXIS2_STRLEN(dup_string)] = '\0';
        
    temp_str = NULL;
    temp_str = strtok(dup_string, ",");
    while(NULL != temp_str)
    {
        axis2_char_t *temp_element = AXIS2_STRDUP(temp_str, env);
        added = AXIS2_TRUE;
        AXIS2_ARRAY_LIST_ADD(ret, env, temp_element);
    }
    AXIS2_FREE(env->allocator, dup_string);
    if(AXIS2_FALSE == added)
    {
        AXIS2_ARRAY_LIST_FREE(ret, env);
        return NULL;
    }
    return ret;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_utils_array_list_contains(const axis2_env_t *env,
                        axis2_array_list_t *list,
                        axis2_char_t *string)
{
    int i = 0;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, list, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, string, AXIS2_FALSE);
    
    for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(list, env); i++)
    {
        axis2_char_t *element = AXIS2_ARRAY_LIST_GET(list, env, i);
        if(NULL != element && 0 == AXIS2_STRCMP(element, string))
            return AXIS2_TRUE;
    }
    return AXIS2_FAILURE;
}

AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_array_list_to_string(const axis2_env_t *env,
                        axis2_array_list_t *list, int type)
{
    axis2_char_t *list_string = NULL;
    int i = 0;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, list, NULL);
    
    list_string = AXIS2_STRDUP("[", env);
    for(i = 0; i < AXIS2_ARRY_LIST_SIZE(list, env); i++)
    {
             
        if(SANDESHA2_ARRAY_LIST_STRING == type)
        {
            axis2_char_t *element = AXIS2_ARRAY_LIST_GET(list, env, i);
            list_string = axis2_strcat(env, list_string, ",", element, NULL);
        }
        else if(SANDESHA2_ARRAY_LIST_LONG == type)
        {
            long *element = AXIS2_ARRAY_LIST_GET(list, env, i);
            axis2_char_t value[32];
            sprintf(value, "%ld", *element);
            list_string = axis2_strcat(env, list_string, ",", value, NULL);
        } 
    }
    list_string = axis2_strcat(env, list_string, "]", NULL);
    
    return list_string;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL                        
sandesha2_utils_start_invoker_for_seq(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id)
{
    sandesha2_in_order_invoker_t *invoker = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    
    property = AXIS2_CTX_GET_PROPERTY(AXIS2_CONF_CTX_GET_BASE(conf_ctx, env),
                        env, SANDESHA2_INVOKER, AXIS2_FALSE);
    if(NULL == property)
        return AXIS2_FAILURE;
    invoker = AXIS2_PROPERTY_GET_VALUE(property, env);
    if(NULL == invoker)
    {
        invoker = sandesha2_in_order_invoker_create(env);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, invoker);
        AXIS2_CTX_SET_PROPERTY(AXIS2_CONF_CTX_GET_BASE(conf_ctx, env),
                        env, SANDESHA2_INVOKER, property, AXIS2_FALSE);
    }
    SANDESHA2_IN_ORDER_INVOKER_RUN_FOR_SEQ(invoker, env, conf_ctx, seq_id);
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL                        
sandesha2_utils_start_sender_for_seq(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx,
                        axis2_char_t *seq_id)
{
    sandesha2_sender_t *sender = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    
    property = AXIS2_CTX_GET_PROPERTY(AXIS2_CONF_CTX_GET_BASE(conf_ctx, env),
                        env, SANDESHA2_SENDER, AXIS2_FALSE);
    if(NULL != property)
        sender = AXIS2_PROPERTY_GET_VALUE(property, env);
        
    if(NULL == sender)
    {
        sender = sandesha2_sender_create(env);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, sender);
        AXIS2_CTX_SET_PROPERTY(AXIS2_CONF_CTX_GET_BASE(conf_ctx, env),
                        env, SANDESHA2_SENDER, property, AXIS2_FALSE);
    }
    SANDESHA2_SENDER_RUN_FOR_SEQ(sender, env, conf_ctx, seq_id);
    return AXIS2_SUCCESS;
}
               
                        
AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_outgoing_internal_seq_id(const axis2_env_t *env,
                        axis2_char_t *seq_id)
{
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, seq_id, NULL);
    
    return axis2_strcat(env, SANDESHA2_INTERNAL_SEQ_PREFIX, ":",
                        seq_id, NULL);
}

AXIS2_EXTERN axis2_transport_out_desc_t* AXIS2_CALL
sandesha2_utils_get_transport_out(const axis2_env_t *env)
{
    axis2_transport_out_desc_t *out_desc = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    axis2_qname_t *qname = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    
    transport_sender = sandesha2_transport_sender_create(env);
    qname = axis2_qname_create(env, "Sandesha2TransportOutDesc", NULL, NULL);
    out_desc = axis2_transport_out_desc_create_with_qname(env, qname);
    AXIS2_TRANSPORT_OUT_DESC_SET_SENDER(out_desc, env, transport_sender);
    return out_desc;
}

AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_utils_get_inmemory_storage_mgr(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx)
{
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);
    
    property = AXIS2_CTX_GET_PROPERTY(AXIS2_CONF_CTX_GET_BASE(conf_ctx, env),
                        env, SANDESHA2_INMEMORY_STORAGE_MGR, AXIS2_FALSE);

    if(NULL != property && NULL != AXIS2_PROPERTY_GET_VALUE(property, env))
        return (sandesha2_storage_mgr_t*)AXIS2_PROPERTY_GET_VALUE(property, 
                        env);
    else
    {
        /* TODO we need to class load the proper storage mgr */
        sandesha2_storage_mgr_t *storage_man = 
                        sandesha2_storage_mgr_create(env, conf_ctx);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, storage_man);
        AXIS2_CTX_SET_PROPERTY(AXIS2_CONF_CTX_GET_BASE(conf_ctx, env),
                        env, SANDESHA2_INMEMORY_STORAGE_MGR, property, 
                        AXIS2_FALSE);
        return storage_man;
    }
    return NULL;    
}

AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_utils_get_permanent_storage_mgr(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx)
{
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);
    
    /*TODO implement when the persistent storage is avalable */
    return NULL;
}

AXIS2_EXTERN axis2_char_t* AXIS2_CALL                       
sandesha2_utils_get_svr_side_incoming_seq_id(const axis2_env_t *env,
                        axis2_char_t *incoming_seq_id)
{
    axis2_char_t *start_str = NULL;
    axis2_char_t *ret = NULL;
    int start_len = 0;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, incoming_seq_id, NULL);
    
    start_str = axis2_strcat(env, SANDESHA2_INTERNAL_SEQ_PREFIX, ":", NULL);
    start_len = AXIS2_STRLEN(start_str);
    if(0 != AXIS2_STRNCMP(incoming_seq_id, start_str, start_len))
        return NULL;
    ret = AXIS2_STRDUP((incoming_seq_id + start_len * sizeof(axis2_char_t)), env);
    
    return ret;    
}


AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_utils_get_property_bean_from_op(const axis2_env_t *env,
                        axis2_op_t *op)
{
        axis2_param_t *param = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, op, NULL);
    
    param = AXIS2_OP_GET_PARAM(op, env, SANDESHA2_SANDESHA_PROPERTY_BEAN);
    if(NULL == param)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_CONFIGURATION_NOT_SET,
                        AXIS2_FAILURE);
        return NULL;
    }
    return (sandesha2_property_bean_t*)AXIS2_PARAM_GET_VALUE(param, env);

}

AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_get_internal_seq_id(
        const axis2_env_t *env,
        axis2_char_t *to,
        axis2_char_t *seq_key)
{
    axis2_char_t *ret = NULL;

    AXIS2_ENV_CHECK(env, NULL);

    if(!to && !seq_key)
    {
        return NULL;
    }
    else if(!to)
    {
        return AXIS2_STRDUP(seq_key, env);
    }
    else if(!seq_key)
    {
        return AXIS2_STRDUP(to, env);
    }
    else
    {
        ret = axis2_strcat(env, SANDESHA2_INTERNAL_SEQ_PREFIX, ":", to, ":", 
                        seq_key, NULL);
        return ret;
    }
    return NULL;
}

AXIS2_EXTERN axis2_msg_ctx_t *AXIS2_CALL
sandesha2_utils_create_new_related_msg_ctx(const axis2_env_t *env,
                        sandesha2_msg_ctx_t *ref_rm_msg,
                        axis2_op_t *op)
{
    axis2_msg_ctx_t *ref_msg = NULL;
    axis2_msg_ctx_t *new_msg = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_transport_out_desc_t *out_desc = NULL;
    axis2_transport_in_desc_t *in_desc = NULL;
    axis2_options_t *options = NULL;
    axis2_svc_t *svc = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    axiom_soap_envelope_t *soap_env = NULL;
    axis2_property_t *property = NULL;
    axis2_char_t *addr_ver = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ref_rm_msg, NULL);
    AXIS2_PARAM_CHECK(env->error, op, NULL);
    
    ref_msg = SANDESHA2_MSG_CTX_GET_MSG_CTX(ref_rm_msg, env);
    conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(ref_msg, env);
    conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);
    
    out_desc = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(ref_msg, env);
    in_desc = AXIS2_MSG_CTX_GET_TRANSPORT_IN_DESC(ref_msg, env);
    
    new_msg = axis2_msg_ctx_create(env, conf_ctx, in_desc, out_desc);
        
    options = axis2_options_create(env);
    AXIS2_MSG_CTX_SET_OPTIONS(new_msg, env, options);
    
    if(NULL != AXIS2_MSG_CTX_GET_SVC_GRP(ref_msg, env))
    {
        AXIS2_MSG_CTX_SET_SVC_GRP(new_msg, env, 
                        AXIS2_MSG_CTX_GET_SVC_GRP(ref_msg, env));
        if(NULL != AXIS2_MSG_CTX_GET_SVC_GRP_CTX(ref_msg, env))
        {
            AXIS2_MSG_CTX_SET_SVC_GRP_CTX(new_msg, env, 
                    AXIS2_MSG_CTX_GET_SVC_GRP_CTX(ref_msg, env));
            AXIS2_MSG_CTX_SET_SVC_GRP_CTX_ID(new_msg, env, 
                    AXIS2_MSG_CTX_GET_SVC_GRP_CTX_ID(ref_msg, env));
        }
        else
        {
            axis2_svc_grp_ctx_t *svc_grp_ctx = NULL;
            svc_grp_ctx = axis2_svc_grp_ctx_create(env, 
                        AXIS2_MSG_CTX_GET_SVC_GRP(ref_msg, env), conf_ctx);
            AXIS2_MSG_CTX_SET_SVC_GRP_CTX(new_msg, env, svc_grp_ctx);
        }
    }
    else
    {
        axis2_svc_grp_t *svc_grp = NULL;
        axis2_svc_grp_ctx_t *svc_grp_ctx = NULL;
        
        svc_grp = axis2_svc_grp_create_with_conf(env, conf);
        svc_grp_ctx = axis2_svc_grp_ctx_create(env, svc_grp, conf_ctx);
        AXIS2_MSG_CTX_SET_SVC_GRP(new_msg, env, svc_grp);
        AXIS2_MSG_CTX_SET_SVC_GRP_CTX(new_msg, env, svc_grp_ctx);
    }
    if(NULL != AXIS2_MSG_CTX_GET_SVC(ref_msg, env))
    {
        AXIS2_MSG_CTX_SET_SVC(new_msg, env, AXIS2_MSG_CTX_GET_SVC(ref_msg, env));
        if(NULL != AXIS2_MSG_CTX_GET_SVC_CTX(ref_msg, env))
        {
            AXIS2_MSG_CTX_SET_SVC_CTX(new_msg, env, 
                        AXIS2_MSG_CTX_GET_SVC_CTX(ref_msg, env));
            AXIS2_MSG_CTX_SET_SVC_CTX_ID(new_msg, env, 
                        AXIS2_MSG_CTX_GET_SVC_CTX_ID(ref_msg, env));
        }
        else
        {
            axis2_svc_ctx_t *svc_ctx =  NULL;
            svc_ctx = axis2_svc_ctx_create(env, 
                        AXIS2_MSG_CTX_GET_SVC(ref_msg, env),
                        AXIS2_MSG_CTX_GET_SVC_GRP_CTX(new_msg, env));
            AXIS2_MSG_CTX_SET_SVC_CTX(new_msg, env, svc_ctx);
        }
    }
    else
    {
        axis2_svc_t *axis_svc = NULL;
        axis2_qname_t *svc_qname = NULL;
        axis2_svc_grp_t *svc_grp = NULL;
        axis2_svc_ctx_t *svc_ctx = NULL;
        axis2_svc_grp_ctx_t *svc_grp_ctx = NULL;
        
        svc_qname = axis2_qname_create(env, "AnonymousRMService", NULL, NULL);
        axis_svc = axis2_svc_create_with_qname(env, svc_qname);
        
        svc_grp = AXIS2_MSG_CTX_GET_SVC_GRP(new_msg, env);
        AXIS2_SVC_SET_PARENT(axis_svc, env, AXIS2_MSG_CTX_GET_SVC_GRP(new_msg,
                        env));
        AXIS2_SVC_GRP_ADD_SVC(svc_grp, env, axis_svc);
        svc_grp_ctx = AXIS2_MSG_CTX_GET_SVC_GRP_CTX(new_msg, env);
        svc_ctx = axis2_svc_ctx_create(env, axis_svc, svc_grp_ctx);
    }
    AXIS2_MSG_CTX_SET_OP(new_msg, env, op);
    svc = AXIS2_MSG_CTX_GET_SVC(new_msg, env);
    
    if(NULL != svc && NULL != op)
    {
        AXIS2_SVC_ADD_OP(svc, env, op);
        AXIS2_OP_SET_PARENT(op, env, svc);
    }
    
    op_ctx = axis2_op_ctx_create(env, op, AXIS2_MSG_CTX_GET_SVC_CTX(new_msg, 
                        env));
    AXIS2_MSG_CTX_SET_OP_CTX(new_msg, env, op_ctx);
    AXIS2_OP_CTX_ADD_MSG_CTX(op_ctx, env, new_msg);
    
    soap_env = axiom_soap_envelope_create_default_soap_envelope(env, 
                        sandesha2_utils_get_soap_version(env, 
                        AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(ref_msg, env)));
    AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(new_msg, env, soap_env);
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, AXIS2_TRANSPORT_URL,
                        AXIS2_FALSE);
    if(NULL != property && NULL != AXIS2_PROPERTY_GET_VALUE(property, env))
    {
        axis2_char_t *val = AXIS2_PROPERTY_GET_VALUE(property, env);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(val, env));
        AXIS2_CTX_SET_PROPERTY(new_msg, env, AXIS2_TRANSPORT_URL, property,
                        AXIS2_FALSE);
    }
    property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, AXIS2_WSA_VERSION,
                        AXIS2_FALSE);
    if(NULL == property)
    {
        axis2_msg_ctx_t *req_msg = NULL;
        axis2_op_ctx_t *op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(ref_msg, env);
        
        req_msg = AXIS2_OP_CTX_GET_MSG_CTX(op_ctx, env, 
                        AXIS2_WSDL_MESSAGE_LABEL_IN_VALUE);
        if(NULL != req_msg)
        {
            property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, 
                        AXIS2_TRANSPORT_URL, AXIS2_FALSE);
            if(NULL != property)
                addr_ver = AXIS2_PROPERTY_GET_VALUE(property, env);
        }
    }
    else
    {
        addr_ver = AXIS2_PROPERTY_GET_VALUE(property, env);
    }
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(addr_ver, env));
    AXIS2_CTX_SET_PROPERTY(new_msg, env, AXIS2_WSA_VERSION, property,
                    AXIS2_FALSE);
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, AXIS2_TRANSPORT_OUT, 
                        AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_PROPERTY(new_msg, env, AXIS2_TRANSPORT_OUT, property, 
                        AXIS2_FALSE);
    property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, 
                        AXIS2_TRANSPORT_IN, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_PROPERTY(new_msg, env, AXIS2_TRANSPORT_IN, 
                        property, AXIS2_FALSE);
    property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, 
                        AXIS2_HTTP_OUT_TRANSPORT_INFO, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_PROPERTY(new_msg, env, AXIS2_HTTP_OUT_TRANSPORT_INFO, 
                        property, AXIS2_FALSE);
    property = AXIS2_MSG_CTX_GET_PROPERTY(ref_msg, env, 
                        AXIS2_TRANSPORT_HEADERS, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_PROPERTY(new_msg, env, AXIS2_TRANSPORT_HEADERS, 
                        property, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_EXECUTION_CHAIN(new_msg, env, 
                        AXIS2_MSG_CTX_GET_EXECUTION_CHAIN(ref_msg, env));
                        
    return new_msg;
}

AXIS2_EXTERN  int AXIS2_CALL
sandesha2_utils_get_soap_version(const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope)
{
    AXIS2_ENV_CHECK(env, -1);
    AXIS2_PARAM_CHECK(env->error, envelope, -1);
    
    return AXIOM_SOAP_ENVELOPE_GET_SOAP_VERSION(envelope, env);
}

AXIS2_EXTERN axis2_char_t* AXIS2_CALL
sandesha2_utils_trim_string(const axis2_env_t *env, 
                        axis2_char_t *orig_string)
{
    axis2_char_t *tmp = NULL;
    axis2_char_t *tmp2 = NULL;
    axis2_char_t *ret = NULL;
    int len = 0;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, orig_string, NULL);
    
    tmp = orig_string;
    while(' ' == *tmp)
        tmp++;
        
    tmp2 = orig_string + AXIS2_STRLEN(orig_string);
    while(' ' == *tmp2 && tmp2 != orig_string)
        tmp2--;
        
    len = tmp2 - tmp;
    if(len > 0)
        ret = AXIS2_MALLOC(env->allocator, len + sizeof(axis2_char_t));
        
    memcpy(ret, tmp, len);
    ret[len] = '\0';
    return ret;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL                        
sandesha2_utils_is_retriable_on_faults(const axis2_env_t *env,
                        axis2_msg_ctx_t *msg_ctx)
{
    axis2_bool_t ret = AXIS2_FALSE;
    axis2_char_t *action = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FALSE);
    
    action =  AXIS2_MSG_CTX_GET_WSA_ACTION(msg_ctx, env);
    if(NULL == action)
        return AXIS2_FALSE;
        
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_02_ACTION_CREATE_SEQ))
        ret = AXIS2_TRUE;
    else if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_CREATE_SEQ))
        ret = AXIS2_TRUE;
        
    return ret;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
sandesha2_utils_is_rm_global_msg(const axis2_env_t *env,
                        axis2_msg_ctx_t *msg_ctx)
{
    axis2_bool_t is_global_msg = AXIS2_FALSE;
    axis2_char_t *action = NULL;
    axiom_soap_envelope_t *soap_env = NULL;
    axiom_soap_header_t *soap_header = NULL;
    axiom_element_t *header_element = NULL;
    axiom_node_t *header_node = NULL;
    axiom_element_t *seq_element = NULL;
    axiom_node_t *seq_node = NULL;
    axis2_qname_t *qname = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FALSE);
    
    action =  AXIS2_MSG_CTX_GET_WSA_ACTION(msg_ctx, env);
    soap_env = AXIS2_MSG_CTX_GET_SOAP_ENVELOPE(msg_ctx, env);
    
    if(NULL == soap_env)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] SOAP envelope is"
                        " null");
        return AXIS2_FALSE;
    }
    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(soap_env, env);
    
    if(NULL != soap_header)
    {
        header_node = AXIOM_SOAP_HEADER_GET_BASE_NODE(soap_header, env);
        header_element = AXIOM_NODE_GET_DATA_ELEMENT(header_node, env);
    
        qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQ,
                        SANDESHA2_SPEC_2005_02_NS_URI, NULL);
        seq_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(header_element, 
                        env, qname, header_node, &seq_node);
        if(NULL == seq_element)
        {
            qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQ,
                        SANDESHA2_SPEC_2005_10_NS_URI, NULL);
            seq_element = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(
                        header_element, env, qname, header_node, &seq_node);
        }
    }
    if(NULL != seq_element)
        is_global_msg = AXIS2_TRUE;
        
    if(0 == AXIS2_STRCMP(action, 
                        SANDESHA2_SPEC_2005_02_ACTION_SEQ_ACKNOWLEDGEMENT))
        is_global_msg = AXIS2_TRUE;
        
    if(0 == AXIS2_STRCMP(action, 
                        SANDESHA2_SPEC_2005_02_ACTION_CREATE_SEQ_RESPONSE))
        is_global_msg = AXIS2_TRUE;
        
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_02_ACTION_TERMINATE_SEQ))
        is_global_msg = AXIS2_TRUE;    
        
    if(0 == AXIS2_STRCMP(action, 
                        SANDESHA2_SPEC_2005_10_ACTION_SEQ_ACKNOWLEDGEMENT))
        is_global_msg = AXIS2_TRUE;
        
    if(0 == AXIS2_STRCMP(action, 
                        SANDESHA2_SPEC_2005_10_ACTION_CREATE_SEQ_RESPONSE))
        is_global_msg = AXIS2_TRUE;
        
    if(0 == AXIS2_STRCMP(action, SANDESHA2_SPEC_2005_10_ACTION_TERMINATE_SEQ))
        is_global_msg = AXIS2_TRUE;    
    
    return is_global_msg;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
sandesha2_utils_get_seq_id_from_rm_msg_ctx(const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    int msg_type = -1;
    axis2_char_t *seq_id = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, NULL);
    
    msg_type = SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, env);
    
    if(SANDESHA2_MSG_TYPE_APPLICATION == msg_type)
    {
        sandesha2_seq_t *seq = NULL;
        seq = (sandesha2_seq_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env,
                        SANDESHA2_MSG_PART_SEQ);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_GET_IDENTIFIER(seq, env), env);
    }
    else if(SANDESHA2_MSG_TYPE_ACK == msg_type)
    {
        sandesha2_seq_ack_t *seq_ack = NULL;
        seq_ack = (sandesha2_seq_ack_t*)SANDESHA2_MSG_CTX_GET_MSG_PART(
                        rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_SEQ_ACK_GET_IDENTIFIER(seq_ack, env), env);
    }
    else if(SANDESHA2_MSG_TYPE_ACK_REQUEST == msg_type)
    {
        sandesha2_ack_requested_t *ack_requested = NULL;
        ack_requested = (sandesha2_ack_requested_t*)
                        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_ACK_REQUEST);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_ACK_REQUESTED_GET_IDENTIFIER(ack_requested, 
                        env), env);
    }
    else if(SANDESHA2_MSG_TYPE_CLOSE_SEQ == msg_type)
    {
        sandesha2_close_seq_t *close_seq = NULL;
        close_seq = (sandesha2_close_seq_t*)
                        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_CLOSE_SEQ);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_CLOSE_SEQ_GET_IDENTIFIER(close_seq, 
                        env), env);
    }
    else if(SANDESHA2_MSG_TYPE_CLOSE_SEQ_RESPONSE == msg_type)
    {
        sandesha2_close_seq_res_t *close_seq_res = NULL;
        close_seq_res = (sandesha2_close_seq_res_t*)
                        SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_CLOSE_SEQ_RESPONSE);
        seq_id = SANDESHA2_IDENTIFIER_GET_IDENTIFIER(
                        SANDESHA2_CLOSE_SEQ_RES_GET_IDENTIFIER(close_seq_res, 
                        env), env);
    }
    return seq_id;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_utils_stop_invoker(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    
    /*TODO */
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_utils_stop_sender(const axis2_env_t *env,
                        axis2_conf_ctx_t *conf_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    
    /*TODO */
    return AXIS2_SUCCESS;
}
