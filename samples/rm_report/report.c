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

#include "report_util.h"
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
#include <sandesha2_client.h>
#include <sandesha2_report.h>
#include <axis2_addr.h>
#include <axis2_options.h>
#include <ctype.h>

#define SANDESHA2_MAX_COUNT 4

/* on_complete callback function */
axis2_status_t AXIS2_CALL
rm_report_callback_on_complete(
    struct axis2_callback *callback,
    const axis2_env_t *env);

/* on_error callback function */
axis2_status_t AXIS2_CALL
rm_report_callback_on_error(
    struct axis2_callback *callback,
    const axis2_env_t *env,
    int exception);

void wait_on_callback(
    const axis2_env_t *env,
    axis2_callback_t *callback);

static void 
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
    axis2_svc_ctx_t *svc_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axiom_node_t *payload = NULL;
    axis2_property_t *property = NULL;
    axis2_listener_manager_t *listener_manager = NULL;
    axis2_char_t *offered_seq_id = NULL;
    axiom_soap_envelope_t *result = NULL;
    sandesha2_seq_report_t *report = NULL;
    int c;
    int i = 0, size = 0;
   
    /* Set up the environment */
    /*env = axis2_env_create_all("report_non_blocking_dual.log", 
            AXIS2_LOG_LEVEL_DEBUG);*/
    /*env = axis2_env_create_all("report_non_blocking_dual.log", 
            AXIS2_LOG_LEVEL_ERROR);*/
    env = axis2_env_create_all("rm_report.log", 
            AXIS2_LOG_LEVEL_DEBUG);

    /* Set end point reference of report service */
    /*address = "http://127.0.0.1:8888/axis2/services/RMSampleService";*/
    address = "http://127.0.0.1:8888/axis2/services/RMSampleService";
    while ((c = AXIS2_GETOPT(argc, argv, ":a:")) != -1)
    {

        switch (c)
        {
            case 'a':
                address = optarg;
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
    if (axis2_strcmp(address, "-h") == 0)
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
    
    /* Seperate listner needs addressing, hence addressing stuff in options */
    AXIS2_OPTIONS_SET_ACTION(options, env,
        "http://127.0.0.1:8080/axis2/services/RMSampleService/anonOutInOp");
    reply_to = axis2_endpoint_ref_create(env, AXIS2_WSA_ANONYMOUS_URL);
    /*AXIS2_OPTIONS_SET_REPLY_TO(options, env, reply_to);*/

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
    offered_seq_id = axis2_uuid_gen(env);
    property = axis2_property_create(env);
    if(property)
    {
        axis2_property_set_value(property, env, axis2_strdup(offered_seq_id, 
            env));
        AXIS2_OPTIONS_SET_PROPERTY(options, env, 
            SANDESHA2_CLIENT_OFFERED_SEQ_ID, property);
    }
    /* RM Version 1.1 */
    property = axis2_property_create_with_args(env, 3, 0, 0, 
        SANDESHA2_SPEC_VERSION_1_0);
    if(property)
    {
        AXIS2_OPTIONS_SET_PROPERTY(options, env, 
            SANDESHA2_CLIENT_RM_SPEC_VERSION, property);
    }
    property = axis2_property_create_with_args(env, 3, 0, 0, "sequence1");
    if(property)
    {
        AXIS2_OPTIONS_SET_PROPERTY(options, env, SANDESHA2_CLIENT_SEQ_KEY, 
            property);
    }
     
    svc_ctx = AXIS2_SVC_CLIENT_GET_SVC_CTX(svc_client, env);
    conf_ctx = axis2_svc_ctx_get_conf_ctx(svc_ctx, env);
   
    property = axis2_property_create_with_args(env, 0, 0, 0, "4");
    if(property)
    {
        AXIS2_OPTIONS_SET_PROPERTY(options, env, AXIS2_TIMEOUT_IN_SECONDS, 
            property);
    }
    payload = build_om_payload_for_echo_svc(env, "echo1", "sequence1");
    result = AXIS2_SVC_CLIENT_SEND_RECEIVE(svc_client, env, payload);

    if(result)
    {
        axis2_char_t *om_str = NULL;
        om_str = AXIOM_NODE_TO_STRING(result, env);
        if (om_str)
            printf("\nReceived OM : %s\n", om_str);
        printf("\necho client two way single channel invoke SUCCESSFUL!\n");

        AXIS2_FREE(env->allocator, om_str);
        result = NULL;
    }
    else
    {
        printf("\necho client two way single channel invoke FAILED!\n");
    }
    payload = NULL;
    report = sandesha2_client_get_report(env, conf_ctx);
    if(report)
    {
        int i = 0, size = 0;
        axis2_array_list_t *incoming_seq_list = NULL;
        axis2_array_list_t *outgoing_seq_list = NULL;
        incoming_seq_list = sandesha2_report_get_incoming_seq_list(report, env);
        if(incoming_seq_list)
            size = axis2_array_list_size(incoming_seq_list, env);
        for(i = 0; i < size; i++)
        {
            axis2_char_t *seq_id = axis2_array_list_get(incoming_seq_list, env, i);
            if(seq_id)
            {
                printf("incoming seq_id %d:%s\n", i+1, seq_id);
            }
        }
        size = 0;
        outgoing_seq_list = sandesha2_report_get_outgoing_seq_list(report, env);
        if(outgoing_seq_list)
            size = axis2_array_list_size(outgoing_seq_list, env);
        for(i = 0; i < size; i++)
        {
            axis2_char_t *seq_id = axis2_array_list_get(outgoing_seq_list, env, i);
            if(seq_id)
            {
                long completed_msgs_count = -1; 
                printf("outgoing seq_id %d:%s\n", i+1, seq_id);
                completed_msgs_count = sandesha2_report_get_completed_msgs_count(
                    report, env, seq_id);
                printf("completed_msgs_count:%d\n", completed_msgs_count);
            }
        }
    }
    property = axis2_property_create_with_args(env, 0, 0, 0, AXIS2_VALUE_TRUE);
    AXIS2_OPTIONS_SET_PROPERTY(options, env, "Sandesha2LastMessage", property);
    payload = build_om_payload_for_echo_svc(env, "echo2", "sequence1");
    result = AXIS2_SVC_CLIENT_SEND_RECEIVE(svc_client, env, payload);
    if(result)
    {
        axis2_char_t *om_str = NULL;
        om_str = AXIOM_NODE_TO_STRING(result, env);
        if (om_str)
            printf("\nReceived OM : %s\n", om_str);
        printf("\necho client two way single channel invoke SUCCESSFUL!\n");

        AXIS2_FREE(env->allocator, om_str);
        result = NULL;
    }
    else
    {
        printf("\necho client two way single channel invoke FAILED!\n");
    }
    payload = NULL;
    AXIS2_SLEEP(2 * SANDESHA2_MAX_COUNT);

    if (svc_client)
    {
        /*AXIS2_SVC_CLIENT_FREE(svc_client, env);*/
        svc_client = NULL;
    }
    
    return 0;
}

axis2_status_t AXIS2_CALL
rm_report_callback_on_complete(
    struct axis2_callback *callback,
    const axis2_env_t *env)
{
   /** SOAP response has arrived here; get the soap envelope 
     from the callback object and do whatever you want to do with it */
   
   axiom_soap_envelope_t *soap_envelope = NULL;
   axiom_node_t *ret_node = NULL;
   axis2_status_t status = AXIS2_SUCCESS;
   
   soap_envelope = AXIS2_CALLBACK_GET_ENVELOPE(callback, env);
   
   if (!soap_envelope)
   {
       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Stub invoke FAILED: Error code:"
            " %d :: %s", env->error->error_number,
            AXIS2_ERROR_GET_MESSAGE(env->error));
      printf("report stub invoke FAILED!\n");
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
            printf("report stub invoke FAILED!\n");
            status = AXIS2_FAILURE;
        }
        else
        {
            axis2_char_t *om_str = NULL;
            om_str = AXIOM_NODE_TO_STRING(ret_node, env);
            if (om_str)
                printf("\nReceived OM : %s\n", om_str);
            printf("\nreport client invoke SUCCESSFUL!\n");
        }
    }    
    return status;
}

axis2_status_t AXIS2_CALL
rm_report_callback_on_error(
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

static void 
usage(
    axis2_char_t *prog_name)
{
    fprintf(stdout, "\n Usage : %s", prog_name);
    fprintf(stdout, " [-a ADDRESS]");
    fprintf(stdout, " Options :\n");
    fprintf(stdout, "\t-a ADDRESS \t endpoint address.. The" \
        " default is http://127.0.0.1:8888/axis2/services/RMSampleService \n");
    fprintf(stdout, " Help :\n\t-h \t display this help screen.\n\n");
}


