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
 
#include <sandesha2/sandesha2_property_mgr.h>
#include <sys/timeb.h>
#include <axis2_param.h>
#include <sandesha2/sandesha2_constants.h>
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
#include <axis2_param.h>


AXIS2_EXTERN  sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_mgr_load_properties_from_def_values(const axis2_env_t *env)
{
    sandesha2_property_bean_t *property_bean = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    
    property_bean = sandesha2_property_bean_create(env);
    SANDESHA2_PROPERTY_BEAN_SET_ACK_INTERVAL(property_bean, env, 
                        SANDESHA2_DEF_VAL_ACK_INTERVAL);
    SANDESHA2_PROPERTY_BEAN_SET_EXP_BACKOFF(property_bean, env,
                        SANDESHA2_DEF_VAL_EXP_BACKOFF);
    SANDESHA2_PROPERTY_BEAN_SET_INACTIVE_TIMEOUT_INTERVAL_WITH_UNITS(
                        property_bean, env, SANDESHA2_DEF_VAL_INACTIVETIMEOUT,
                        SANDESHA2_DEF_VAL_INACTIVETIMEOUT_MEASURE);
    SANDESHA2_PROPERTY_BEAN_SET_IN_ORDER(property_bean, env,
                        SANDESHA2_DEF_VAL_INORDER_INVOCATION);
    SANDESHA2_PROPERTY_BEAN_SET_MSG_TYPES_TO_DROP(property_bean, env, NULL);
    SANDESHA2_PROPERTY_BEAN_SET_RETRANS_INTERVAL(property_bean, env,
                        SANDESHA2_DEF_VAL_RETR_COUNT);
    /* will be useful when we are loading libraries */
    SANDESHA2_PROPERTY_BEAN_SET_IN_MEM_STORAGE_MGR(property_bean, env,
                        SANDESHA2_DEF_VAL_IN_MEMORY_STORAGE_MGR);
    SANDESHA2_PROPERTY_BEAN_SET_PERMENENT_STORAGE_MGR(property_bean, env,
                        SANDESHA2_DEF_VAL_PERM_STORAGE_MGR);
    SANDESHA2_PROPERTY_BEAN_SET_MAX_RETRANS_COUNT(property_bean, env,
                        SANDESHA2_DEF_VAL_MAX_RETR_COUNT);
    sandesha2_property_mgr_set_msg_types_to_drop(env, 
                        SANDESHA2_DEF_VAL_MSG_TYPES_TO_DROP, property_bean);
    return property_bean;
}

AXIS2_EXTERN  sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_mgr_load_properties_from_module_desc(const axis2_env_t *env,
                        axis2_module_desc_t *module_desc)
{
    sandesha2_property_bean_t *property_bean = NULL;
    axis2_param_t *exp_backoff_param = NULL;
    axis2_char_t *exp_backoff_str = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, module_desc, NULL);
    
    property_bean = sandesha2_property_bean_create(env);
    
    exp_backoff_param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_EXP_BACKOFF);
    if(NULL != exp_backoff_param)
    {
        exp_backoff_str = AXIS2_PARAM_GET_VALUE(exp_backoff_param, env);
        sandesha2_property_mgr_load_exp_backoff(env, exp_backoff_str, 
                        property_bean);
    }
    
                        
    return property_bean;
}
