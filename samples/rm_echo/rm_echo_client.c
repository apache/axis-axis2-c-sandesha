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

#include "echo_util.h"
#include <axis2_util.h>
#include <axiom_soap.h>
#include <axis2_client.h>
#include <axis2_svc_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_op_client.h>
#include <axis2_listener_manager.h>
#include <axis2_callback_recv.h>
#include <axis2_svc_client.h>
#include <sandesha2_client_constants.h>
#include <sandesha2_constants.h>

/* on_complete callback function */
axis2_status_t AXIS2_CALL
rm_echo_callback_on_complete(struct axis2_callback *callback,
    const axis2_env_t *env);

/* on_error callback function */
axis2_status_t AXIS2_CALL
rm_echo_callback_on_error(struct axis2_callback *callback,
    const axis2_env_t *env,
    int exception);

void wait_on_callback(
        const axis2_env_t *env,
        axis2_callback_t *callback);

static axis2_status_t
send_non_blocking(
    const axis2_env_t *env,
    axis2_svc_client_t *svc_client,
    axis2_options_t *options,
    axis2_qname_t *op_qname,
    axis2_callback_t *callback,
    axiom_node_t *payload,
    axis2_listener_manager_t *listner_manager);

static axis2_bool_t
fill_soap_envelope(
    const axis2_env_t *env,
    axis2_svc_client_t *svc_client,
    axis2_msg_ctx_t *msg_ctx,
    axis2_options_t *options,
    const axiom_node_t *payload);

void 
usage(
    axis2_char_t *prog_name);

int main(int argc, char** argv)
{
    const axis2_env_t *env = NULL;
    const axis2_char_t *address = NULL;
    axis2_endpoint_ref_t* endpoint_ref = NULL;
    axis2_endpoint_ref_t* reply_to = NULL;
    axis2_options_t *options = NULL;
    const axis2_char_t *client_home = NULL;
    axis2_svc_client_t* svc_client = NULL;
    axiom_node_t *payload = NULL;
    axis2_callback_t *callback = NULL;
    axis2_callback_t *callback2 = NULL;
    axis2_callback_t *callback3 = NULL;
    axis2_property_t *property = NULL;
    axis2_listener_manager_t *listener_manager = NULL;
    axis2_char_t *offered_seq_id = NULL;
    axis2_bool_t offer = AXIS2_FALSE;
    int version = 0;
    int c;
   
    /* Set up the environment */
    env = axis2_env_create_all("echo_non_blocking_dual.log", 
            AXIS2_LOG_LEVEL_TRACE);

    /* Set end point reference of echo service */
    /*address = "http://127.0.0.1:8888/axis2/services/RMSampleService";*/
    address = "http://127.0.0.1:5555/axis2/services/RMSampleService";
    while ((c = AXIS2_GETOPT(argc, argv, ":a:o:v:")) != -1)
    {

        switch (c)
        {
            case 'a':
                address = optarg;
                break;
            case 'o': /* Sequence Offer */
                offer = AXIS2_ATOI(optarg);
                break;
            case 'v': /* RM Version */
                version = AXIS2_ATOI(optarg);
                break;
            case ':':
                fprintf(stderr, "\nOption -%c requires an operand\n", optopt);
                usage(argv[0]);
                return -1;
            case '?':
                if (isprint(optopt))
                    fprintf(stderr, "\nUnknown option `-%c'.\n", optopt);
                usage(argv[0]);
                return -1;
        }
    }
    printf("offer:%d\n", offer);
    if (AXIS2_STRCMP(address, "-h") == 0)
    {
        printf("Usage : %s [endpoint_url] [offer]\n", argv[0]);
        printf("use -h for help\n");
        return 0;
    }
    printf ("Using endpoint : %s\n", address);
    
    /* Create EPR with given address */
    endpoint_ref = axis2_endpoint_ref_create(env, address);

    /* Setup options */
    options = axis2_options_create(env);
    AXIS2_OPTIONS_SET_TO(options, env, endpoint_ref);
    AXIS2_OPTIONS_SET_USE_SEPARATE_LISTENER(options, env, AXIS2_TRUE);
    
    /* Seperate listner needs addressing, hence addressing stuff in options */
    /*AXIS2_OPTIONS_SET_ACTION(options, env,
        "http://127.0.0.1:8080/axis2/services/RMSampleService/anonOutInOp");*/
    reply_to = axis2_endpoint_ref_create(env, 
            "http://localhost:7777/axis2/services/__ANONYMOUS_SERVICE__/"\
                "__OPERATION_OUT_IN__");

    AXIS2_OPTIONS_SET_REPLY_TO(options, env, reply_to);

    /* Set up deploy folder. It is from the deploy folder, the configuration is 
     * picked up using the axis2.xml file.
     * In this sample client_home points to the Axis2/C default deploy folder. 
     * The client_home can be different from this folder on your system. For 
     * example, you may have a different folder (say, my_client_folder) with its 
     * own axis2.xml file. my_client_folder/modules will have the modules that 
     * the client uses
     */
    client_home = AXIS2_GETENV("AXIS2C_HOME");
    if (!client_home)
        client_home = "../../deploy";

    /* Create service client */
    svc_client = axis2_svc_client_create(env, client_home);
    if (!svc_client)
    {
        printf("Error creating service client\n");
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Stub invoke FAILED: Error code:"
                  " %d :: %s", env->error->error_number,
                        AXIS2_ERROR_GET_MESSAGE(env->error));
        return -1;
    }

    /* Set service client options */
    AXIS2_SVC_CLIENT_SET_OPTIONS(svc_client, env, options);    
    
    AXIS2_SVC_CLIENT_ENGAGE_MODULE(svc_client, env, AXIS2_MODULE_ADDRESSING);  
    AXIS2_SVC_CLIENT_ENGAGE_MODULE(svc_client, env, "sandesha2");

    listener_manager = axis2_listener_manager_create(env);
    if (!listener_manager)
    {
        return AXIS2_FAILURE;
    }
    /* Offer sequence */
    if(offer)
    {
        offered_seq_id = axis2_uuid_gen(env);
        property = axis2_property_create(env);
        if(property)
        {
            AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(offered_seq_id, env));
            AXIS2_OPTIONS_SET_PROPERTY(options, env, SANDESHA2_CLIENT_OFFERED_SEQ_ID,
                property);
        }
    }
    /* RM Version 1.1 */
    if(version == 1)
    {
        property = axis2_property_create(env);
        if(property)
        {
            AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                SANDESHA2_SPEC_VERSION_1_1, env));
            AXIS2_OPTIONS_SET_PROPERTY(options, env, 
                SANDESHA2_CLIENT_RM_SPEC_VERSION, property);
        }
    }
    payload = build_om_payload_for_echo_svc(env, "echo1", "sequence1");
    callback = axis2_callback_create(env);
    AXIS2_CALLBACK_SET_ON_COMPLETE(callback, rm_echo_callback_on_complete);
    AXIS2_CALLBACK_SET_ON_ERROR(callback, rm_echo_callback_on_error);
    send_non_blocking(env, svc_client, options, NULL, callback, payload, 
            listener_manager);

    wait_on_callback(env, callback);

    payload = build_om_payload_for_echo_svc(env, "echo2", "sequence1");
    callback2 = axis2_callback_create(env);
    AXIS2_CALLBACK_SET_ON_COMPLETE(callback2, rm_echo_callback_on_complete);
    AXIS2_CALLBACK_SET_ON_ERROR(callback2, rm_echo_callback_on_error);
    send_non_blocking(env, svc_client, options, NULL, callback2, payload, 
            listener_manager);
    wait_on_callback(env, callback2);    

    callback3 = axis2_callback_create(env);
    AXIS2_CALLBACK_SET_ON_COMPLETE(callback3, rm_echo_callback_on_complete);
    AXIS2_CALLBACK_SET_ON_ERROR(callback3, rm_echo_callback_on_error);
    payload = build_om_payload_for_echo_svc(env, "echo3", "sequence1");
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_VALUE_TRUE);
    AXIS2_OPTIONS_SET_PROPERTY(options, env, "Sandesha2LastMessage", 
            property);
    send_non_blocking(env, svc_client, options, NULL, callback3, payload, 
            listener_manager);
    wait_on_callback(env, callback3);
    AXIS2_SLEEP(10); 
    if (svc_client)
    {
        AXIS2_SVC_CLIENT_FREE(svc_client, env);
        svc_client = NULL;
    }
    
    return 0;
}

axis2_status_t AXIS2_CALL
rm_echo_callback_on_complete(
    struct axis2_callback *callback,
    const axis2_env_t *env)
{
   /** SOAP response has arrived here; get the soap envelope 
     from the callback object and do whatever you want to do with it */
   
   axiom_soap_envelope_t *soap_envelope = NULL;
   axiom_node_t *ret_node = NULL;
    axis2_status_t status = AXIS2_SUCCESS;
   
   printf("inside on_complete_callback function\n");
   
   soap_envelope = AXIS2_CALLBACK_GET_ENVELOPE(callback, env);
   
   if (!soap_envelope)
   {
       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Stub invoke FAILED: Error code:"
                     " %d :: %s", env->error->error_number,
                     AXIS2_ERROR_GET_MESSAGE(env->error));
      printf("echo stub invoke FAILED!\n");
      status = AXIS2_FAILURE;
   }
    else
    {
        ret_node = AXIOM_SOAP_ENVELOPE_GET_BASE_NODE(soap_envelope, env);
    
        if(!ret_node)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                    "Stub invoke FAILED: Error code:%d :: %s", 
                    env->error->error_number, 
                    AXIS2_ERROR_GET_MESSAGE(env->error));
            printf("echo stub invoke FAILED!\n");
            status = AXIS2_FAILURE;
        }
        else
        {
            axis2_char_t *om_str = NULL;
            om_str = AXIOM_NODE_TO_STRING(ret_node, env);
            if (om_str)
                printf("\nReceived OM : %s\n", om_str);
            printf("\necho client invoke SUCCESSFUL!\n");
        }
    }    
    return status;
}

axis2_status_t AXIS2_CALL
rm_echo_callback_on_error(
    struct axis2_callback *callback,
    const axis2_env_t *env,
    int exception)
{
   /** take necessary action on error */
   printf("\nEcho client invoke FAILED. Error code:%d ::%s", exception, 
         AXIS2_ERROR_GET_MESSAGE(env->error));
   return AXIS2_SUCCESS;
}

void wait_on_callback(
    const axis2_env_t *env,
    axis2_callback_t *callback)
{
    /** Wait till callback is complete. Simply keep the parent thread running
       until our on_complete or on_error is invoked */
    while(1)
    {
        if (AXIS2_CALLBACK_GET_COMPLETE(callback, env))
        {
            /* We are done with the callback */
            break;
        }
    }
    return;
}

static axis2_status_t
send_non_blocking(
    const axis2_env_t *env,
    axis2_svc_client_t *svc_client,
    axis2_options_t *options,
    axis2_qname_t *op_qname,
    axis2_callback_t *callback,
    axiom_node_t *payload,
    axis2_listener_manager_t *listener_manager)
{
    axis2_op_client_t *op_client = NULL;
    axis2_svc_ctx_t *svc_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_svc_t *svc = NULL;
    axis2_op_t *op = NULL;
    axis2_callback_recv_t *callback_recv = NULL;
    const axis2_char_t *transport_in_protocol = NULL;

    if(!op_qname)
        op_qname = axis2_qname_create(env, AXIS2_ANON_OUT_IN_OP, NULL, NULL);
    svc_ctx = AXIS2_SVC_CLIENT_GET_SVC_CTX(svc_client, env);
    conf_ctx = AXIS2_SVC_CTX_GET_CONF_CTX(svc_ctx, env);
    msg_ctx = axis2_msg_ctx_create(env, conf_ctx, NULL, NULL);
    svc = AXIS2_SVC_CLIENT_GET_AXIS_SERVICE(svc_client, env);
    op = AXIS2_SVC_GET_OP_WITH_QNAME(svc, env,
            op_qname);
    op_client = axis2_op_client_create(env, op, svc_ctx, options);
    if (!op_client)
    {
        return AXIS2_FAILURE;
    }
    if (!fill_soap_envelope(env, svc_client, msg_ctx, options, payload))
        return AXIS2_FAILURE;
    AXIS2_OP_CLIENT_SET_CALLBACK(op_client, env, callback);
    AXIS2_OP_CLIENT_ADD_OUT_MSG_CTX(op_client, env, msg_ctx);
    transport_in_protocol = AXIS2_OPTIONS_GET_TRANSPORT_IN_PROTOCOL(
                options, env);
    if (!transport_in_protocol)
        transport_in_protocol = AXIS2_TRANSPORT_HTTP;
    AXIS2_LISTNER_MANAGER_MAKE_SURE_STARTED(listener_manager, env, 
            transport_in_protocol, conf_ctx);
    callback_recv = axis2_callback_recv_create(env);
    if (!(callback_recv))
    {
        AXIS2_SVC_CLIENT_FREE(svc_client, env);
        return AXIS2_FAILURE;
    }

    AXIS2_OP_SET_MSG_RECV(op, env,
            AXIS2_CALLBACK_RECV_GET_BASE(callback_recv, env));
    AXIS2_OP_CLIENT_SET_CALLBACK_RECV(op_client, env, callback_recv);
    return AXIS2_OP_CLIENT_EXECUTE(op_client, env, AXIS2_FALSE);
}

static axis2_bool_t
fill_soap_envelope(
    const axis2_env_t *env,
    axis2_svc_client_t *svc_client,
    axis2_msg_ctx_t *msg_ctx,
    axis2_options_t *options,
    const axiom_node_t *payload)
{
    const axis2_char_t *soap_version_uri;
    int soap_version;
    axiom_soap_envelope_t *envelope = NULL;

    soap_version_uri = AXIS2_OPTIONS_GET_SOAP_VERSION_URI(options, env);

    if (!soap_version_uri)
    {
        return AXIS2_FALSE;
    }

    if (AXIS2_STRCMP(soap_version_uri,
            AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI) == 0)
        soap_version = AXIOM_SOAP11;
    else
        soap_version = AXIOM_SOAP12;


    envelope = axiom_soap_envelope_create_default_soap_envelope(env, 
            soap_version);
    if (!envelope)
    {
        return AXIS2_FALSE;
    }

    if (payload)
    {
        axiom_soap_body_t *soap_body = NULL;
        soap_body = AXIOM_SOAP_ENVELOPE_GET_BODY(envelope, env);
        if (soap_body)
        {
            axiom_node_t *node = NULL;
            node = AXIOM_SOAP_BODY_GET_BASE_NODE(soap_body, env);
            if (node)
            {
                AXIOM_NODE_ADD_CHILD(node, env, (axiom_node_t *)payload);
            }
        }
    }

    AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(msg_ctx, env, envelope);

    return AXIS2_TRUE;
}

void 
usage(
    axis2_char_t *prog_name)
{
    fprintf(stdout, "\n Usage : %s", prog_name);
    fprintf(stdout, " [-a ADDRESS]");
    fprintf(stdout, " [-o OFFER]");
    fprintf(stdout, " [-v RM VERSION]");
    fprintf(stdout, " Options :\n");
    fprintf(stdout, "\t-v RM VERSION \t rm version.. The"
            " default rm version is 1.0 ../\n");
    fprintf(stdout, "\t-o OFFER \t seq offer value.. The"
            " default offer value is 0(false) ../\n");
    fprintf(stdout, "\t-a ADDRESS \t endpoint address.. The"
            " default is http://127.0.0.1:5555/axis2/services/RMSampleService ../\n");
    fprintf(stdout, " Help :\n\t-h \t display this help screen.\n\n");
}


