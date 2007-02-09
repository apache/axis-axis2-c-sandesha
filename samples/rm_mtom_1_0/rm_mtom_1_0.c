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

#include "mtom_util.h"
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
    axis2_property_t *property = NULL;
    axis2_listener_manager_t *listener_manager = NULL;
    axis2_char_t *offered_seq_id = NULL;
    int c;
    const axis2_char_t *image_name = "resources/axis2.jpg";
    axiom_node_t *ret_node = NULL;
   
    /* Set up the environment */
    env = axis2_env_create_all("echo_non_blocking_dual.log", 
            AXIS2_LOG_LEVEL_TRACE);

    /* Set end point reference of echo service */
    /*address = "http://127.0.0.1:8888/axis2/services/RMSampleService";*/
    address = "http://127.0.0.1:5555/axis2/services/RMSampleService";
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
    if (AXIS2_STRCMP(address, "-h") == 0)
    {
        printf("Usage : %s [endpoint_url]\n", argv[0]);
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
    AXIS2_OPTIONS_SET_ENABLE_MTOM(options, env, AXIS2_TRUE); 
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
    offered_seq_id = axis2_uuid_gen(env);
    property = axis2_property_create(env);
    if(property)
    {
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(offered_seq_id, env));
        AXIS2_OPTIONS_SET_PROPERTY(options, env, SANDESHA2_CLIENT_OFFERED_SEQ_ID,
            property);
    }
    payload = build_om_payload_for_mtom(env, image_name, "test1.jpg");
    ret_node = AXIS2_SVC_CLIENT_SEND_RECEIVE(svc_client, env, payload);

    payload = build_om_payload_for_mtom(env, image_name, "test2.jpg");
    ret_node = AXIS2_SVC_CLIENT_SEND_RECEIVE(svc_client, env, payload);

    payload = build_om_payload_for_mtom(env, image_name, "test3.jpg");
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_VALUE_TRUE);
    AXIS2_OPTIONS_SET_PROPERTY(options, env, "Sandesha2LastMessage", 
            property);
    ret_node = AXIS2_SVC_CLIENT_SEND_RECEIVE(svc_client, env, payload);
    AXIS2_SLEEP(10); 
    if (svc_client)
    {
        AXIS2_SVC_CLIENT_FREE(svc_client, env);
        svc_client = NULL;
    }
    
    return 0;
}

void 
usage(
    axis2_char_t *prog_name)
{
    fprintf(stdout, "\n Usage : %s", prog_name);
    fprintf(stdout, " [-o ADDRESS]");
    fprintf(stdout, " Options :\n");
    fprintf(stdout, "\t-o ADDRESS \t endpoint address.. The"
        " default is http://127.0.0.1:5555/axis2/services/RMSampleService ../\n");
    fprintf(stdout, " Help :\n\t-h \t display this help screen.\n\n");
}


