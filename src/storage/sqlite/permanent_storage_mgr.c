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
 
#include <platforms/axutil_platform_auto_sense.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_permanent_storage_mgr.h>
#include "sandesha2_permanent_bean_mgr.h"
#include <sandesha2_msg_store_bean.h>
#include <sandesha2_property_bean.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <sandesha2_rm_bean.h>
#include <axutil_log.h>
#include <axutil_hash.h>
#include <axutil_thread.h>
#include <axutil_property.h>
#include <axis2_msg_ctx.h>
#include <axutil_uuid_gen.h>
#include <axis2_conf_ctx.h>
#include <axis2_const.h>
#include <axis2_svc_ctx.h>
#include <axis2_options.h>
#include <axis2_addr.h>
#include <axis2_http_transport.h>
#include <axiom_soap_const.h>
#include <axis2_transport_out_desc.h>
#include <axis2_http_out_transport_info.h>
#include <axiom_xml_reader.h>
#include <axiom_stax_builder.h>
#include <axiom_soap_builder.h>
#include <axutil_file_handler.h>

/** 
 * @brief Sandesha2 Permanent Storage Manager Struct Impl
 *   Sandesha2 Permanent Storage Manager 
 */ 
typedef struct sandesha2_permanent_storage_mgr
{
	sandesha2_storage_mgr_t storage_mgr;
    sandesha2_permanent_bean_mgr_t *bean_mgr;
} sandesha2_permanent_storage_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(trans) \
    ((sandesha2_permanent_storage_mgr_t *) trans)

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free_void_arg(
    void *storage_mgr,
    const axutil_env_t *env);

axis2_status_t AXIS2_CALL 
sandesha2_permanent_storage_mgr_free(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *envv);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx);
	
axis2_msg_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx,
    const axis2_bool_t persistent);
		
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);
			
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_update_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx,
    int msg_type);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init_storage(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axis2_module_desc_t *module_desc);

axiom_soap_envelope_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_soap_envelope(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_soap_envelope(
    sandesha2_storage_mgr_t *storage,
    const axutil_env_t *env,
    axiom_soap_envelope_t *soap_env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_response(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_char_t *seq_id,
    axiom_soap_envelope_t *response,
    int msg_no,
    int soap_version);

axiom_soap_envelope_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axutil_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axutil_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no);

static sandesha2_msg_store_bean_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_msg_store_bean (
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_msg_ctx_t *msg_ctx);

static axutil_hash_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_map_from_string(
    const axutil_env_t *env,
    axis2_char_t *str);

static axis2_char_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_string(
    const axutil_env_t *env,
    axis2_msg_ctx_t *msg_ctx);

static const sandesha2_storage_mgr_ops_t storage_mgr_ops = 
{
    sandesha2_permanent_storage_mgr_free,
    sandesha2_permanent_storage_mgr_free_void_arg,
    sandesha2_permanent_storage_mgr_init,
    sandesha2_permanent_storage_mgr_retrieve_msg_ctx,
    sandesha2_permanent_storage_mgr_store_msg_ctx,
    sandesha2_permanent_storage_mgr_update_msg_ctx,
    sandesha2_permanent_storage_mgr_remove_msg_ctx,
    sandesha2_permanent_storage_mgr_init_storage,
    sandesha2_permanent_storage_mgr_retrieve_soap_envelope,
    sandesha2_permanent_storage_mgr_store_soap_envelope,
    sandesha2_permanent_storage_mgr_store_response,
    sandesha2_permanent_storage_mgr_retrieve_response,
    sandesha2_permanent_storage_mgr_remove_response
};

AXIS2_EXTERN sandesha2_storage_mgr_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_create(
    const axutil_env_t *env,
    axis2_char_t *dbname)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
   
    storage_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_storage_mgr_t));

    storage_mgr_impl->bean_mgr = NULL;
    storage_mgr_impl->bean_mgr = sandesha2_permanent_bean_mgr_create(env,
        dbname, NULL);
    storage_mgr_impl->storage_mgr.ops = &storage_mgr_ops;

    return &(storage_mgr_impl->storage_mgr);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free_void_arg(
    void *storage_mgr,
    const axutil_env_t *env)
{
    storage_mgr = (sandesha2_permanent_storage_mgr_t *) storage_mgr;
    return sandesha2_permanent_storage_mgr_free(storage_mgr, env);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    if(storage_mgr_impl->bean_mgr)
    {
        sandesha2_permanent_bean_mgr_free(storage_mgr_impl->bean_mgr, env);
        storage_mgr_impl->bean_mgr = NULL;
    }
    if(storage_mgr_impl)
    {
        AXIS2_FREE(env->allocator, storage_mgr_impl);
        storage_mgr_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_conf_ctx_t *conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
    axutil_property_t *property = NULL;
    axis2_ctx_t *ctx = axis2_conf_ctx_get_base(conf_ctx, env);
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    
    property = axis2_ctx_get_property(ctx, env, SANDESHA2_MSG_CTX_MAP);
    axis2_msg_ctx_set_keep_alive(msg_ctx, env, AXIS2_TRUE);
    msg_store_bean = sandesha2_permanent_storage_mgr_get_msg_store_bean(
        storage_mgr, env, msg_ctx);
    sandesha2_msg_store_bean_set_stored_key(msg_store_bean, env, key);
    sandesha2_permanent_bean_mgr_insert_msg_store_bean(storage_mgr_impl->bean_mgr, 
        env, msg_store_bean);
    if(property)
    {
        axutil_hash_t *msg_ctx_map = NULL;
        msg_ctx_map = axutil_property_get_value(property, env);
        axutil_hash_set(msg_ctx_map, axutil_strdup(env, key),
            AXIS2_HASH_KEY_STRING, msg_ctx);
    }
    if(msg_store_bean)
        sandesha2_msg_store_bean_free(msg_store_bean, env);
    return AXIS2_SUCCESS;
}
			
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_update_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx)
{
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    {
        axis2_conf_ctx_t *conf_ctx = axis2_msg_ctx_get_conf_ctx(msg_ctx, env);
        axutil_property_t *property = NULL;
        axis2_ctx_t *ctx = axis2_conf_ctx_get_base(conf_ctx, env);
        axutil_hash_t *msg_ctx_map = NULL;

        property = axis2_ctx_get_property(ctx, env, SANDESHA2_MSG_CTX_MAP);
        if(property)
        {
            msg_ctx_map = axutil_property_get_value(property, env);
            if(msg_ctx_map)
            {
                axutil_hash_set(msg_ctx_map, axutil_strdup(env, key), 
                    AXIS2_HASH_KEY_STRING, msg_ctx);
            }
        }
    }

    axis2_msg_ctx_set_keep_alive(msg_ctx, env, AXIS2_TRUE);
    msg_store_bean = sandesha2_permanent_storage_mgr_get_msg_store_bean(
        storage_mgr, env, msg_ctx);
    sandesha2_msg_store_bean_set_stored_key(msg_store_bean, env, key);
    sandesha2_permanent_bean_mgr_update_msg_store_bean(storage_mgr_impl->bean_mgr, 
        env, msg_store_bean);
    if(msg_store_bean)
        sandesha2_msg_store_bean_free(msg_store_bean, env);
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx,
    int msg_type)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axutil_property_t *property = NULL;
    axis2_ctx_t *ctx = axis2_conf_ctx_get_base(conf_ctx, env);
    axutil_hash_t *msg_ctx_map = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    property = axis2_ctx_get_property(ctx, env, SANDESHA2_MSG_CTX_MAP);
    if(property)
    {
        msg_ctx_map = axutil_property_get_value(property, env);
        if(msg_ctx_map)
        {
            axutil_hash_index_t *i = NULL;
            for (i = axutil_hash_first(msg_ctx_map, env); i; i = 
                axutil_hash_next(env, i))
            {
                const void *k = NULL;
                void *v = NULL;
                axis2_char_t *key_l = NULL;

                axutil_hash_this(i, &k, NULL, &v);
                key_l = (axis2_char_t *) k;
                if(0 == axutil_strcmp(key, key_l))
                {
                    axis2_op_ctx_t *op_ctx = NULL;
                    msg_ctx = (axis2_msg_ctx_t *) v;
                    op_ctx = 
                        axis2_msg_ctx_get_op_ctx(msg_ctx, env);
                    if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
                    {
                        /*axiom_soap_envelope_t *soap_envelope = 
                            axis2_msg_ctx_get_soap_envelope(msg_ctx, env);
                        axis2_op_ctx_set_in_use(op_ctx, env, AXIS2_FALSE);
                        axiom_soap_envelope_free(soap_envelope, env);
                        axis2_op_ctx_free(op_ctx, env);*/
                        axutil_hash_set(msg_ctx_map, key, AXIS2_HASH_KEY_STRING, 
                            NULL);
                        AXIS2_FREE(env->allocator, key_l);
                    }
                    else if(msg_type != SANDESHA2_MSG_TYPE_CREATE_SEQ)
                    {
                        /*AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                            "[sandesha2]Removing msg_ctx with msg_id:%s", 
                            axis2_msg_ctx_get_msg_id(msg_ctx, env));
                        axis2_op_ctx_set_in_use(op_ctx, env, AXIS2_FALSE);
                        axis2_msg_ctx_set_keep_alive(msg_ctx, env, AXIS2_FALSE);
                        axis2_msg_ctx_set_paused(msg_ctx, env, AXIS2_FALSE);
                        axis2_op_ctx_free(op_ctx, env);
                        axutil_hash_set(msg_ctx_map, key, AXIS2_HASH_KEY_STRING, 
                            NULL);
                        AXIS2_FREE(env->allocator, key_l);*/
                    }
                }
            }
        }
    }
    sandesha2_permanent_bean_mgr_remove_msg_store_bean(
        storage_mgr_impl->bean_mgr, env, key);
    return AXIS2_SUCCESS;
}

axis2_msg_ctx_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axutil_env_t *env, 
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx,
    const axis2_bool_t persistent)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_conf_t *conf = NULL;
    axis2_svc_grp_t *svc_grp = NULL;
    axis2_svc_t *svc = NULL;
    AXIS2_TRANSPORT_ENUMS transport_out = -1;
    axis2_char_t *soap_env_str = NULL;
    axis2_char_t *svc_grp_name_str = NULL;
    axis2_char_t *svc_name_str = NULL;
    axis2_char_t *op_name_str = NULL;
    axis2_char_t *op_mep_str = NULL;
    axis2_char_t *to_url_str = NULL;
    axis2_char_t *reply_to_str = NULL;
    axis2_char_t *transport_to_str = NULL;
    axis2_char_t *persistent_prop_str = NULL;
    axis2_transport_out_desc_t *transport_out_desc = NULL;
    axis2_endpoint_ref_t *endpoint_ref = NULL;
    axis2_op_t *op = NULL;
    /*axis2_options_t *options = NULL;*/
    axiom_xml_reader_t *reader = NULL;
    int soap_version = -1;
    axiom_stax_builder_t *om_builder = NULL;
    axiom_soap_builder_t *soap_builder = NULL;
    axiom_soap_envelope_t *soap_envelope = NULL;
    sandesha2_msg_store_bean_t *msg_store_bean = NULL;

    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    if(!persistent)
    {
        axutil_property_t *property = NULL;
        axis2_ctx_t *ctx = axis2_conf_ctx_get_base(conf_ctx, env);
        axutil_hash_t *msg_ctx_map = NULL;

        property = axis2_ctx_get_property(ctx, env, SANDESHA2_MSG_CTX_MAP);
        if(property)
        {
            msg_ctx_map = axutil_property_get_value(property, env);
            msg_ctx = (axis2_msg_ctx_t *) axutil_hash_get(msg_ctx_map, key, 
                AXIS2_HASH_KEY_STRING);
        }
    }
    if(msg_ctx)
        return msg_ctx;
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]retrieved from database");
    msg_store_bean = sandesha2_permanent_bean_mgr_retrieve_msg_store_bean(
        storage_mgr_impl->bean_mgr, env, key);
    if (!msg_store_bean) 
    {
        return NULL;
    }
    msg_ctx = axis2_msg_ctx_create(env, conf_ctx, NULL, NULL);
    soap_env_str = sandesha2_msg_store_bean_get_soap_envelope_str(msg_store_bean, env);
    reader = axiom_xml_reader_create_for_memory(env, soap_env_str, axutil_strlen(
        soap_env_str), NULL, AXIS2_XML_PARSER_TYPE_BUFFER);
    om_builder = axiom_stax_builder_create(env, reader);
    soap_version = sandesha2_msg_store_bean_get_soap_version(msg_store_bean, env);
    if(SANDESHA2_SOAP_VERSION_1_1 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    else if(SANDESHA2_SOAP_VERSION_1_2 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    soap_envelope = axiom_soap_builder_get_soap_envelope(soap_builder, env);
    if (!soap_envelope)
    {
        axiom_soap_builder_free(soap_builder, env);
        return NULL;
    }
    axis2_msg_ctx_set_soap_envelope(msg_ctx, env, soap_envelope);
    axis2_msg_ctx_set_msg_id(msg_ctx, env, sandesha2_msg_store_bean_get_msg_id(
        msg_store_bean, env));

    conf = axis2_conf_ctx_get_conf(conf_ctx, env);
    transport_out = sandesha2_msg_store_bean_get_transport_out(msg_store_bean, 
        env);
    transport_out_desc = axis2_conf_get_transport_out(conf, env, transport_out);
    axis2_msg_ctx_set_transport_out_desc(msg_ctx, env, transport_out_desc);
    svc_grp_name_str = sandesha2_msg_store_bean_get_svc_grp(msg_store_bean, env);
    if(svc_grp_name_str)
    {
        svc_grp = axis2_conf_get_svc_grp(conf, env, svc_grp_name_str);
        if(svc_grp)
            axis2_msg_ctx_set_svc_grp(msg_ctx, env, svc_grp);
    }
    else
    {
        svc_grp = axis2_svc_grp_create_with_conf(env, conf);
        axis2_msg_ctx_set_svc_grp(msg_ctx, env, svc_grp);
    }
    svc_name_str = sandesha2_msg_store_bean_get_svc(msg_store_bean, env);
    if(svc_name_str)
    {
        svc = axis2_conf_get_svc(conf, env, svc_name_str);
        if(svc)
            axis2_msg_ctx_set_svc(msg_ctx, env, svc);
        else
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Cannot build message "\
                 "context since service %s is not present", svc_name_str);
            AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CANNOT_BUILD_MSG_CTX_WHEN_SVC_NOT_PRESENT, 
                AXIS2_FAILURE);
            axiom_soap_builder_free(soap_builder, env);
            return NULL;
        }
    }

    op_name_str = sandesha2_msg_store_bean_get_op(msg_store_bean, env);
    op_mep_str = sandesha2_msg_store_bean_get_op_mep(msg_store_bean, env);
    svc = axis2_msg_ctx_get_svc(msg_ctx, env);
    if((op_name_str || op_mep_str) && svc)
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
            axis2_msg_ctx_set_op(msg_ctx, env, op);
        else
        {
            AXIS2_LOG_WARNING(env->log, AXIS2_LOG_SI, 
                    "[sandesha2] Cannot find a suitable operation for the generated message");
            /*AXIS2_ERROR_SET(env->error, 
                SANDESHA2_ERROR_CANNOT_FIND_OP_FOR_GENERATED_MSG, AXIS2_FAILURE);
            axiom_soap_builder_free(soap_builder, env);
            return NULL;*/
        }
    }
    /* Setting contexts TODO is this necessary? */
    svc_grp = axis2_msg_ctx_get_svc_grp(msg_ctx, env);
    if(svc_grp)
    {
        axis2_svc_grp_ctx_t *svc_grp_ctx = axis2_svc_grp_ctx_create(env, svc_grp, conf_ctx);
        axis2_msg_ctx_set_svc_grp_ctx(msg_ctx, env, svc_grp_ctx);
    }
    svc = axis2_msg_ctx_get_svc(msg_ctx, env);
    if(svc)
    {
        axis2_svc_grp_ctx_t *svc_grp_ctx = axis2_msg_ctx_get_svc_grp_ctx(
            msg_ctx, env);
        axis2_svc_ctx_t *svc_ctx = axis2_svc_ctx_create(env, svc, svc_grp_ctx);
        if(svc_ctx)
        {
            axis2_svc_ctx_set_parent(svc_ctx, env, svc_grp_ctx);
            axis2_msg_ctx_set_svc_ctx(msg_ctx, env, svc_ctx);
        }
        
    }
    op = axis2_msg_ctx_get_op(msg_ctx, env);
    if(op)
    {
		axis2_op_ctx_t *op_ctx = NULL;
        axis2_svc_ctx_t *svc_ctx = axis2_msg_ctx_get_svc_ctx(msg_ctx, env);
		op_ctx = axis2_op_ctx_create(env, op, svc_ctx);
        if(op_ctx)
        {
            axis2_char_t *msg_id = NULL;
             axis2_op_ctx_set_parent(op_ctx, env, svc_ctx);
            axis2_msg_ctx_set_op_ctx(msg_ctx, env, op_ctx);
            msg_id = (axis2_char_t *) axis2_msg_ctx_get_msg_id(msg_ctx, env);
        }
    }
    axis2_msg_ctx_set_server_side(msg_ctx, env, 
        sandesha2_msg_store_bean_is_svr_side(msg_store_bean, env));
    axis2_msg_ctx_set_flow(msg_ctx, env, 
        sandesha2_msg_store_bean_get_flow(msg_store_bean, env));
    transport_to_str = sandesha2_msg_store_bean_get_transport_to(msg_store_bean, 
        env);
    if(transport_to_str)
    {
        axis2_msg_ctx_set_transport_url(msg_ctx, env, transport_to_str);
    }
    to_url_str = sandesha2_msg_store_bean_get_to_url(msg_store_bean, env);
    if(to_url_str)
    {
        endpoint_ref = axis2_endpoint_ref_create(env, to_url_str);
        axis2_msg_ctx_set_to(msg_ctx, env, endpoint_ref);
    }
    reply_to_str = sandesha2_msg_store_bean_get_reply_to(msg_store_bean, env);
    if(reply_to_str)
    {
        endpoint_ref = axis2_endpoint_ref_create(env, reply_to_str);
        axis2_msg_ctx_set_reply_to(msg_ctx, env, endpoint_ref);
    }
    /*options = (axis2_options_t *) axis2_msg_ctx_get_options(msg_ctx, env);
    if(options)
    {
        axis2_char_t *action = sandesha2_msg_store_bean_get_action(
            msg_store_bean, env);
		axis2_options_set_action(options, env, action);
    }*/
    {
        axis2_char_t *action = sandesha2_msg_store_bean_get_action(
            msg_store_bean, env);
        axis2_msg_ctx_set_wsa_action(msg_ctx, env, action);
    }
    persistent_prop_str = 
        sandesha2_msg_store_bean_get_persistent_property_str(msg_store_bean, 
            env);
    if(persistent_prop_str && 0 != axutil_strcmp("", persistent_prop_str))
    {
        axutil_hash_t *map = 
            sandesha2_permanent_storage_mgr_get_property_map_from_string(env, 
                persistent_prop_str);
        axutil_hash_index_t *index = NULL;
        if(map)
            for (index = axutil_hash_first(map, env); index; index = 
                axutil_hash_next(env, index))
            {
                axutil_property_t *property = NULL;
                void *v = NULL;
                const void *k = NULL;
                axis2_char_t *key = NULL;
                axutil_hash_this(index, &k, NULL, &v);
                key = (axis2_char_t *) k;
                property = (axutil_property_t *) v;
                axis2_msg_ctx_set_property(msg_ctx, env, key, property);
            }
    }
    /*if(msg_store_bean)
        sandesha2_msg_store_bean_free(msg_store_bean, env);*/
    return msg_ctx;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init_storage(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_module_desc_t *module_desc)
{
    return AXIS2_SUCCESS;
}

axiom_soap_envelope_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_char_t *key)
{
    /* TODO No real value */
    return NULL;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axiom_soap_envelope_t *soap_env,
    axis2_char_t *key)
{
    /* TODO No real value */
    return AXIS2_SUCCESS;
}

static sandesha2_msg_store_bean_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_msg_store_bean (
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_msg_ctx_t *msg_ctx)
{
    axiom_soap_envelope_t *envelope = NULL;
    axis2_char_t *soap_str = NULL;
    int soap_version = 0;
    axiom_xml_writer_t *xml_writer = NULL;
    axiom_output_t *om_output = NULL;
    axis2_transport_out_desc_t *transport_out_desc = NULL;
    axis2_svc_grp_t *svc_grp = NULL;
    axis2_char_t *transport_to = NULL;
    axis2_svc_t *svc = NULL;
    axis2_op_t *op = NULL;
    axis2_endpoint_ref_t *to = NULL;
    axis2_endpoint_ref_t *reply_to = NULL;
    axis2_char_t *address = NULL;
    axis2_char_t *action = NULL;
    axutil_property_t *property = NULL;
    /*axis2_options_t *options = NULL;*/
    axis2_char_t *prop_str = NULL;
    axis2_op_ctx_t *op_ctx = NULL;
    sandesha2_msg_store_bean_t *bean = NULL;
        
    envelope = axis2_msg_ctx_get_soap_envelope(msg_ctx, env);
    if (!envelope)
    {
        AXIS2_ERROR_SET(env->error,
                AXIS2_ERROR_NULL_SOAP_ENVELOPE_IN_MSG_CTX,
                AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "%s",
                AXIS2_ERROR_GET_MESSAGE(env->error));
        return AXIS2_FAILURE;
    }
    xml_writer = axiom_xml_writer_create_for_memory(env, NULL,
            AXIS2_TRUE, 0, AXIS2_XML_PARSER_TYPE_BUFFER);
    if (!xml_writer)
    {
        return AXIS2_FAILURE;
    }
    om_output = axiom_output_create(env, xml_writer);
    if (!om_output)
    {
        axiom_xml_writer_free(xml_writer, env);
        xml_writer = NULL;
        return AXIS2_FAILURE;
    }
    axiom_output_set_soap11(om_output, env, axis2_msg_ctx_get_is_soap_11(
        msg_ctx, env));
    axiom_soap_envelope_serialize(envelope, env, om_output, AXIS2_FALSE);
    soap_str = (axis2_char_t *)axiom_xml_writer_get_xml(xml_writer, env);
    if (axis2_msg_ctx_get_is_soap_11(msg_ctx, env))
        soap_version = SANDESHA2_SOAP_VERSION_1_1;
    else
        soap_version = SANDESHA2_SOAP_VERSION_1_2;
    bean = sandesha2_msg_store_bean_create(env);
    transport_out_desc = axis2_msg_ctx_get_transport_out_desc(msg_ctx, env);
    svc_grp = axis2_msg_ctx_get_svc_grp(msg_ctx, env);
    svc = axis2_msg_ctx_get_svc(msg_ctx, env);
    op = axis2_msg_ctx_get_op(msg_ctx, env);
    if(transport_out_desc)
    {
        AXIS2_TRANSPORT_ENUMS transport_out = axis2_transport_out_desc_get_enum(
            transport_out_desc, env);
        sandesha2_msg_store_bean_set_transport_out(bean, env, transport_out);
    }
    if(svc_grp)
    {
        axis2_char_t *svc_grp_name = (axis2_char_t *)  axis2_svc_grp_get_name(
            svc_grp, env);
        sandesha2_msg_store_bean_set_svc_grp(bean, env, svc_grp_name);
    }
    if (svc) 
    {
        axis2_char_t *svc_name = (axis2_char_t *) axis2_svc_get_name(svc, env);
        sandesha2_msg_store_bean_set_svc(bean, env, svc_name);
    }
    if(op)
    {
        axutil_qname_t *qname = (axutil_qname_t *) axis2_op_get_qname(op, env);
        axis2_char_t *mep = NULL;
        if(qname)
        {
            axis2_char_t *op_name = axutil_qname_to_string(qname, env);
            sandesha2_msg_store_bean_set_op(bean, env, op_name);
        }
        mep = (axis2_char_t *) axis2_op_get_msg_exchange_pattern(op, env);
        sandesha2_msg_store_bean_set_op_mep(bean, env, mep);
    }
    sandesha2_msg_store_bean_set_flow(bean, env, axis2_msg_ctx_get_flow(msg_ctx, 
        env));
    sandesha2_msg_store_bean_set_svr_side(bean, env, 
        axis2_msg_ctx_get_server_side(msg_ctx, env));
    sandesha2_msg_store_bean_set_soap_envelope_str(bean, env, soap_str);
    if(xml_writer)
        axiom_xml_writer_free(xml_writer, env);
    sandesha2_msg_store_bean_set_soap_version(bean, env, soap_version);
    sandesha2_msg_store_bean_set_msg_id(bean, env, (axis2_char_t *) 
        axis2_msg_ctx_get_msg_id(msg_ctx, env));
    to = axis2_msg_ctx_get_to(msg_ctx, env);
    if(to)
    {
        address = (axis2_char_t *) axis2_endpoint_ref_get_address(to, env);
        sandesha2_msg_store_bean_set_to_url(bean, env, address);
    }
    reply_to = axis2_msg_ctx_get_reply_to(msg_ctx, env);
    if(reply_to)
    {
        address = (axis2_char_t *) axis2_endpoint_ref_get_address(reply_to, env);
        sandesha2_msg_store_bean_set_reply_to(bean, env, address);
    }
    transport_to = axis2_msg_ctx_get_transport_url(msg_ctx, env);
    if(transport_to)
        sandesha2_msg_store_bean_set_transport_to(bean, env, transport_to);

    /*options = (axis2_options_t *) axis2_msg_ctx_get_options(msg_ctx, env);
	action  = (axis2_char_t *) axis2_options_get_action(options, env);*/
    action = (axis2_char_t *) axis2_msg_ctx_get_wsa_action(msg_ctx, env);
    sandesha2_msg_store_bean_set_action(bean, env, action);
    prop_str = sandesha2_permanent_storage_mgr_get_property_string(env, msg_ctx);
    if (prop_str)
    {
        sandesha2_msg_store_bean_set_persistent_property_str(bean, env, 
            prop_str);
        if(prop_str && axutil_strlen(prop_str) > 0)
        {
            AXIS2_FREE(env->allocator, prop_str);
        }
    }
    /* setting the request message if this a response message.*/
    op_ctx = axis2_msg_ctx_get_op_ctx(msg_ctx, env);
    if(op_ctx)
    {
        axis2_msg_ctx_t *in_msg_ctx = NULL;
        in_msg_ctx =  axis2_op_ctx_get_msg_ctx(op_ctx, env, 
            AXIS2_WSDL_MESSAGE_LABEL_IN); 
        if(in_msg_ctx)
        {
            axis2_char_t *in_msg_store_key = NULL;
            axis2_bool_t insert = AXIS2_FALSE;
            property = axis2_msg_ctx_get_property(msg_ctx, env, 
                SANDESHA2_IN_MESSAGE_STORAGE_KEY);
            if(property)
                in_msg_store_key = (axis2_char_t *) axutil_property_get_value(
                    property, env);
            if(!in_msg_store_key)
            {
                in_msg_store_key = (axis2_char_t *) axutil_uuid_gen(env);
                insert = AXIS2_TRUE;
            }
            /*if(insert)
                sandesha2_permanent_storage_mgr_store_msg_ctx(storage_mgr, env,
                    in_msg_store_key, in_msg_ctx);
            else
                sandesha2_permanent_storage_mgr_update_msg_ctx(storage_mgr, env,
                    in_msg_store_key, in_msg_ctx);*/
            sandesha2_msg_store_bean_set_in_msg_store_key(bean, env, 
                in_msg_store_key);
        }
    }
    return bean;
}

static axis2_char_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_string(
    const axutil_env_t *env,
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_char_t *prop_str = "";
    axutil_property_t *property = NULL;
    axutil_hash_index_t *index = NULL;
	axutil_hash_t *properties = NULL;
    /*axis2_options_t *options = (axis2_options_t *) axis2_msg_ctx_get_options(
        msg_ctx, env);
	properties = axis2_options_get_properties(options, env);*/
    properties = axis2_ctx_get_property_map(axis2_msg_ctx_get_base(msg_ctx, 
        env), env);
    property = axis2_msg_ctx_get_property(msg_ctx, env, 
        SANDESHA2_QUALIFIED_FOR_SENDING);
    if(property)
    {
        axis2_char_t *value = axutil_property_get_value(property, env);
        prop_str = axutil_strcat(env, SANDESHA2_QUALIFIED_FOR_SENDING,
            SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, value, NULL);
    }
    property = axis2_msg_ctx_get_property(msg_ctx, env, 
        AXIS2_WSA_VERSION);
    if(property)
    {
        axis2_char_t *temp_str = NULL;
        axis2_char_t *value = axutil_property_get_value(property, env);
        if(value)
        {
            temp_str = prop_str;
            prop_str = axutil_strcat(env, temp_str, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, AXIS2_WSA_VERSION, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, value, NULL);
            if(temp_str && 0 < axutil_strlen(temp_str))
                AXIS2_FREE(env->allocator, temp_str);
        }
    }
    for (index = axutil_hash_first(properties, env); index; index = 
        axutil_hash_next(env, index))
    {
        axis2_char_t *temp_str = NULL;
        void *v = NULL;
        const void *k = NULL;
        axis2_char_t *key = NULL;
        axis2_char_t *value = NULL;
        axutil_hash_this(index, &k, NULL, &v);
        key = (axis2_char_t *) k;
        if(0 == axutil_strcmp(AXIS2_HTTP_OUT_TRANSPORT_INFO, key))
            continue;
        if(0 == axutil_strcmp(AXIS2_TRANSPORT_OUT, key))
            continue;
        if(0 == axutil_strcmp(AXIS2_TRANSPORT_IN, key))
            continue;
        if(0 == axutil_strcmp(AXIS2_TRANSPORT_HEADERS, key))
            continue;
        if(0 == axutil_strcmp(SANDESHA2_ORIGINAL_TRANSPORT_OUT_DESC, key))
            continue;
        if(0 == axutil_strcmp(AXIS2_HTTP_CLIENT, key))
            continue;

        property = (axutil_property_t *) v;
        if(property)
            value = axutil_property_get_value(property, env);
        if(value)
        {
            temp_str = prop_str;
            prop_str = axutil_strcat(env, temp_str, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, key, 
                SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR, 
                value, NULL);
            if(temp_str && axutil_strlen(temp_str) > 0)
                AXIS2_FREE(env->allocator, temp_str);
        }
    }
    return prop_str;
}

static axutil_hash_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_property_map_from_string(
    const axutil_env_t *env,
    axis2_char_t *str)
{
    int i = 0, size = 0;
    axutil_hash_t *map = axutil_hash_make(env);
    axutil_array_list_t *values = sandesha2_utils_split(env, str, 
        SANDESHA2_PERSISTANT_PROPERTY_SEPERATOR);
    if(values)
        size = axutil_array_list_size(values, env);
    if((size % 2 != 0) || (size == 1 && 0 == axutil_strcmp("", 
        axutil_array_list_get(values, env, 0))))
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "Invalid persistence property string");
        AXIS2_ERROR_SET(env->error, 
            SANDESHA2_ERROR_INVALID_PERSISTENCE_PROPERTY_STRING, AXIS2_FAILURE);
        return NULL;
    }
    for(i = 0; i < size; i=i+2)
    {
        axis2_char_t *key = axutil_array_list_get(values, env, i);
        axis2_char_t *value = axutil_array_list_get(values, env, i+1);
        axutil_property_t *property = axutil_property_create_with_args(env, 0, 
            0, 0, value);
        axutil_hash_set(map, key, AXIS2_HASH_KEY_STRING, property);
    }
    return map;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_response(
    sandesha2_storage_mgr_t *storage_mgr,
    const axutil_env_t *env,
    axis2_char_t *seq_id,
    axiom_soap_envelope_t *response,
    int msg_no,
    int soap_version)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    axis2_char_t *response_str = NULL;
    axiom_xml_writer_t *xml_writer = NULL;
    axiom_output_t *om_output = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    if (!response)
    {
        AXIS2_ERROR_SET(env->error,
            AXIS2_ERROR_NULL_SOAP_ENVELOPE_IN_MSG_CTX,
            AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "%s",
            AXIS2_ERROR_GET_MESSAGE(env->error));
        return AXIS2_FAILURE;
    }
    xml_writer = axiom_xml_writer_create_for_memory(env, NULL,
        AXIS2_TRUE, 0, AXIS2_XML_PARSER_TYPE_BUFFER);
    if (!xml_writer)
    {
        return AXIS2_FAILURE;
    }
    om_output = axiom_output_create(env, xml_writer);
    if (!om_output)
    {
        axiom_xml_writer_free(xml_writer, env);
        xml_writer = NULL;
        return AXIS2_FAILURE;
    }
    axiom_soap_envelope_serialize(response, env, om_output, AXIS2_FALSE);
    response_str = (axis2_char_t *)axiom_xml_writer_get_xml(xml_writer, env);

    sandesha2_permanent_bean_mgr_store_response(storage_mgr_impl->bean_mgr, 
        env, seq_id, response_str, msg_no, soap_version);
    if(xml_writer)
        axiom_xml_writer_free(xml_writer, env);
    return AXIS2_SUCCESS;
}
	
axiom_soap_envelope_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axutil_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    sandesha2_response_t *response = NULL;
    axiom_soap_envelope_t *response_envelope = NULL;
    axiom_xml_reader_t *reader = NULL;
    int soap_version = -1;
    axiom_stax_builder_t *om_builder = NULL;
    axiom_soap_builder_t *soap_builder = NULL;

    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    response = sandesha2_permanent_bean_mgr_retrieve_response(
        storage_mgr_impl->bean_mgr, env, seq_id, msg_no);
    if(!response)
    {
        return NULL;
    }
    reader = axiom_xml_reader_create_for_memory(env, response->response_str, 
        axutil_strlen(response->response_str), NULL, AXIS2_XML_PARSER_TYPE_BUFFER);
    om_builder = axiom_stax_builder_create(env, reader);
    soap_version = response->soap_version;
    if(SANDESHA2_SOAP_VERSION_1_1 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    else if(SANDESHA2_SOAP_VERSION_1_2 == soap_version)
    {
        soap_builder = axiom_soap_builder_create(env, om_builder,
            AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI);
    }
    response_envelope = axiom_soap_builder_get_soap_envelope(soap_builder, env);
    if(!response_envelope)
    {
        axiom_soap_builder_free(soap_builder, env);
    }
    return response_envelope;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_response(
    sandesha2_storage_mgr_t *storage_mgr, 
    const axutil_env_t *env, 
    axis2_char_t *seq_id,
    int msg_no)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;

    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return sandesha2_permanent_bean_mgr_remove_response(
        storage_mgr_impl->bean_mgr, env, seq_id, msg_no);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_create_db(
    const axutil_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    int rc = -1;
    axis2_char_t *dbname = NULL;
    sqlite3 *dbconn = NULL;
    axis2_char_t *sql_stmt1 = NULL;
    axis2_char_t *sql_stmt2 = NULL;
    axis2_char_t *sql_stmt3 = NULL;
    axis2_char_t *sql_stmt4 = NULL;
    axis2_char_t *sql_stmt5 = NULL;
    axis2_char_t *sql_stmt6 = NULL;
    axis2_char_t *sql_stmt7 = NULL;
    axis2_char_t *error_msg = NULL;

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Entry:sandesha2_permanent_storage_mgr_create_db");

    dbname = sandesha2_util_get_dbname(env, conf_ctx);
    if(AXIS2_SUCCESS == axutil_file_handler_access(dbname, AXIS2_F_OK))
    {
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
            "[sandesha2]Database %s already created.", dbname);
        return AXIS2_SUCCESS;
    }
    AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2]dbname:%s", dbname);
    dbconn = sandesha2_permanent_bean_mgr_get_dbconn(env, dbname);

    #if !defined(WIN32)
    {
        axis2_char_t permission_str[256];
        sprintf(permission_str, "chmod 777 %s", dbname); 
        system(permission_str);
    }
    #endif

    sql_stmt1 = "create table create_seq("\
        "create_seq_msg_id varchar(100) primary key, "\
        "internal_seq_id varchar(200), seq_id varchar(200), "\
        "create_seq_msg_store_key varchar(100), ref_msg_store_key varchar(100))";
    sql_stmt2 = "create table invoker("\
        "msg_ctx_ref_key varchar(100) primary key,"\
        "msg_no long, seq_id varchar(200), is_invoked boolean)";
    sql_stmt3 = "create table sender("\
        "msg_id varchar(100) primary key, msg_ctx_ref_key varchar(100), "\
        "internal_seq_id varchar(200), sent_count int, msg_no long, "\
        "send boolean, resend boolean, time_to_send long, msg_type int, "\
        "seq_id varchar(200), wsrm_anon_uri varchar(100), "\
        "to_address varchar(100))";
    sql_stmt4 = "create table next_msg("\
        "seq_id varchar(200) primary key, internal_seq_id varchar(200), "\
        "ref_msg_key varchar(100), "\
        "polling_mode boolean, msg_no long)";
    sql_stmt5 = "create table seq_property(id varchar(200) ,"\
        "seq_id varchar(200), name varchar(200), value varchar(200))";
    sql_stmt6 = "create table msg("\
        "stored_key varchar(200) primary key, msg_id varchar(200), "\
        "soap_env_str text, soap_version int, transport_out varchar(100), "\
        "op varchar(100), svc varchar(100), svc_grp varchar(100), "\
        "op_mep varchar(100), to_url varchar(200), reply_to varchar(200), "\
        "transport_to varchar(200), execution_chain_str varchar(200), "\
        "flow int, msg_recv_str varchar(200), svr_side boolean, "\
        "in_msg_store_key varchar(200), prop_str varchar(8192), "\
        "action varchar(200))";
    sql_stmt7 = "create table response(seq_id varchar(200), "\
        "response_str text, msg_no int, soap_version int)";
    if(dbconn)
    {
        rc = sqlite3_exec(dbconn, sql_stmt1, NULL, 0, &error_msg);
        if( rc != SQLITE_OK )
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "[sandesha2]Error creating database table create_seq; "\
                "SQL Error: %s", error_msg);
            sqlite3_free(error_msg);
            sqlite3_close(dbconn);
            return AXIS2_FAILURE;
        }
        rc = sqlite3_exec(dbconn, sql_stmt2, NULL, 0, &error_msg);
        if( rc != SQLITE_OK )
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "[sandesha2]Error creating database table invoker; SQL Error: %s",
                error_msg);
            sqlite3_free(error_msg);
            sqlite3_close(dbconn);
            return AXIS2_FAILURE;
        }
        rc = sqlite3_exec(dbconn, sql_stmt3, NULL, 0, &error_msg);
        if( rc != SQLITE_OK )
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "[sandesha2]Error creating database table sender; SQL Error: %s",
                error_msg);
            sqlite3_free(error_msg);
            sqlite3_close(dbconn);
            return AXIS2_FAILURE;
        }
        rc = sqlite3_exec(dbconn, sql_stmt4, NULL, 0, &error_msg);
        if( rc != SQLITE_OK )
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "[sandesha2]Error creating database table next_msg; SQL Error: %s",
                error_msg);
            sqlite3_free(error_msg);
            sqlite3_close(dbconn);
            return AXIS2_FAILURE;
        }
        rc = sqlite3_exec(dbconn, sql_stmt5, NULL, 0, &error_msg);
        if( rc != SQLITE_OK )
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "[sandesha2]Error creating database table seq_property; "\
                "SQL Error: %s",
                error_msg);
            sqlite3_free(error_msg);
            sqlite3_close(dbconn);
            return AXIS2_FAILURE;
        }
        rc = sqlite3_exec(dbconn, sql_stmt6, NULL, 0, &error_msg);
        if( rc != SQLITE_OK )
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "[sandesha2]Error creating database table msg; SQL Error: %s",
                error_msg);
            sqlite3_free(error_msg);
            sqlite3_close(dbconn);
            return AXIS2_FAILURE;
        }
        rc = sqlite3_exec(dbconn, sql_stmt7, NULL, 0, &error_msg);
        if( rc != SQLITE_OK )
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "[sandesha2]Error creating database table response; SQL Error: %s",
                error_msg);
            sqlite3_free(error_msg);
            sqlite3_close(dbconn);
            return AXIS2_FAILURE;
        }
        sqlite3_close(dbconn);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "[sandesha2] Created the database %s successfully", dbname);
    }
    else
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] Database %s creation failed", dbname);
        return AXIS2_FAILURE;
    }

    AXIS2_LOG_TRACE(env->log, AXIS2_LOG_SI, 
        "[sandesha2]Exit:sandesha2_permanent_storage_mgr_create_db");
    return AXIS2_SUCCESS;
}

