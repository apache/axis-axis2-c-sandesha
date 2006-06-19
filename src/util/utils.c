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


AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_utils_remove_soap_body_part(const axis2_env_t *env, 
                        axis2_soap_envelope_t *envelope, axis2_qname_t *qname)
{
    axis2_soap_body_t *soap_body = NULL;
    axis2_om_node_t *body_node = NULL;
    axis2_om_element_t *body_element = NULL;
        axis2_om_element_t *body_element = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, envelope, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, qname, AXIS2_FAILURE);
    
    soap_body = AXIS2_SOAP_ENVELOPE_GET_BODY(envelope, env);
    if(NULL == soap_body)
    {
        return AXIS2_FAILURE;
    }
    body_node = AXIS2_SOAP_BODY_GET_BASE_NODE(soap_body, env);
    if(NULL == body_node)
    {
        return AXIS2_FAILURE;
    }
    body_element = AXIS2_OM_NODE_GET_DATA_ELEMENT(body_node, env);
    if(NULL == body_element)
    {
        return AXIS2_FAILURE;
    }
    body_rm_element = AXIS2_OM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(body_element,
                            env, qname, body_node, &body_rm_node);
    if(NULL != body_rm_element)
    {
        AXIS2_OM_NODE_DETACH(body_rm_node, env);
    }
    return AXIS2_SUCCESS;
}


