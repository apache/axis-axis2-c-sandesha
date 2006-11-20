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

#include <stdlib.h>
#include <sandesha2_error.h>
#include <axis2_error_default.h>

const axis2_char_t * AXIS2_CALL 
sandesha2_error_impl_get_message (const axis2_error_t *error);
      
axis2_status_t AXIS2_CALL
sandesha2_error_impl_set_error_number (axis2_error_t *error, axis2_error_codes_t error_number);

axis2_status_t AXIS2_CALL
sandesha2_error_impl_set_status_code (axis2_error_t *error, axis2_status_codes_t status_code);

axis2_status_t AXIS2_CALL
sandesha2_error_impl_get_status_code (axis2_error_t *error);

/* array to hold error messages */
const axis2_char_t* sandesha2_error_messages[AXIS2_ERROR_LAST];

axis2_status_t AXIS2_CALL
sandesha2_error_init()
{
    int i = 0;
    for (i = 0; i < AXIS2_ERROR_LAST; i++)
    {
        sandesha2_error_messages[i] = "Unknown Error :(";
    }
    sandesha2_error_messages[AXIS2_ERROR_NONE] = "No Error";
        /* A namespace that is not supported by Sandesha2 */
    sandesha2_error_messages[SANDESHA2_ERROR_UNSUPPORTED_NS] = 
    "A namespace that is not supported by Sandesha2";
    /* Object cannot generated since the om_element corresponding to
     * that is NULL 
     */
    sandesha2_error_messages[SANDESHA2_ERROR_NULL_OM_ELEMENT] = 
    "Object cannot generated since the om_element corresponding to \
        that is NULL";
    /* The om element has no child elements or attributes */
    sandesha2_error_messages[SANDESHA2_ERROR_EMPTY_OM_ELEMENT] = 
    "The om element has no child elements or attributes";
    /*  om_element cannot be generated since the element corresponding 
     * to that is NULL
     */
    sandesha2_error_messages[SANDESHA2_ERROR_TO_OM_NULL_ELEMENT] = 
    "om_element cannot be generated since the element corresponding \
        to that is NULL";
    /* Required OM attribute is NULL */
    sandesha2_error_messages[SANDESHA2_ERROR_NULL_OM_ATTRIBUTE] = 
    "Required OM attribute is NULL";
    /* An invalid number found during object to om conversion  */
    sandesha2_error_messages[SANDESHA2_ERROR_TO_OM_INVALID_NUMBER] = 
    "An invalid number found during object to om conversion";
    /* Ack Final now allowed in the spec  */
    sandesha2_error_messages[SANDESHA2_ERROR_ACK_FINAL_NOT_ALLOWED] = 
    "Ack Final now allowed in the spec";
    /* Ack None now allowed in the spec  */
    sandesha2_error_messages[SANDESHA2_ERROR_ACK_NONE_NOT_ALLOWED] = 
    "Ack None now allowed in the spec";

    /* 'To' Address is not set */
    sandesha2_error_messages[SANDESHA2_ERROR_TO_ADDRESS_NOT_SET] = 
        "'To' address is not set";
    /* Service Context is NULL */
    sandesha2_error_messages[SANDESHA2_ERROR_SVC_CTX_NULL] = 
        "Service context is NULL";
    sandesha2_error_messages[SANDESHA2_ERROR_SVC_NULL]=
        "Service is NULL";
    /* Non Unique Result */
    sandesha2_error_messages[SANDESHA2_ERROR_NON_UNIQUE_RESULT] = 
        "Result is Not Unique";
    /* Key is NULL. Cannot insert */
    sandesha2_error_messages[SANDESHA2_ERROR_KEY_IS_NULL] = 
        "Key is NULL, Cannot insert";
    /* Storage Map not present */
    sandesha2_error_messages[SANDESHA2_ERROR_STORAGE_MAP_NOT_PRESENT] = 
        "Storage Map not present";
    sandesha2_error_messages[SANDESHA2_ERROR_ENTRY_IS_NOT_PRESENT_FOR_UPDATING] =
        "Entry is not present for updating";
    sandesha2_error_messages[SANDESHA2_ERROR_INVALID_NS_URI] = 
        "Invalid namespace uri";
    sandesha2_error_messages[SANDESHA2_ERROR_INVALID_SPEC_VERSION] = 
        "Invalid Spec Version";
    sandesha2_error_messages[
        SANDESHA2_ERROR_INCOMING_SEQ_REPORT_NOT_PRESENT_FOR_GIVEN_SEQ_ID] = 
        "An incoming seq report is not present for the given seqID";
    sandesha2_error_messages[SANDESHA2_ERROR_TO_EPR_NOT_SET] = 
        "To EPR is not set";   
    sandesha2_error_messages[SANDESHA2_ERROR_OPTIONS_OBJECT_NOT_SET] = 
        "Options object is not set";
    sandesha2_error_messages[SANDESHA2_ERROR_UNKNOWN_SPEC_ERROR_MESSAGE] = 
        "Unknown specification error message"; 
    sandesha2_error_messages[SANDESHA2_ERROR_COULD_NOT_SEND_TERMINATE_MESSAGE] = 
        "Could not send the terminate message";
    sandesha2_error_messages[SANDESHA2_ERROR_RM_SPEC_VERSION_DOES_NOT_DEFINE_A_SEQ_CLOSE_ACTION] =
        "This rm spec version does not define a sequence close action";
    sandesha2_error_messages[SANDESHA2_ERROR_COULD_NOT_SEND_THE_CLOSE_SEQ_MESSAGE] =
        "Could not send the close sequence message";
    sandesha2_error_messages[SANDESHA2_ERROR_CLOSE_SEQ_FEATURE_ONLY_AVAILABLE_FOR_WSRM1_1] =
        "Close Sequence feature is only available for WSRM 1.1";
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_FIND_SEQ_REPORT_FOR_GIVEN_DATA] = 
        "Cannnot find a seq report for the given data";   
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_FIND_SEQ_REPORT_FOR_GIVEN_SEQ_ID] = 
        "Cannnot find a seq report for the given sequence id";
    sandesha2_error_messages[SANDESHA2_ERROR_SEQ_NOT_IN_ACTIVE_STATE] = 
        "Sequence is not in a active state. Either create seq response has not \
            being received or seq has been terminated, cannot get seqID";
    sandesha2_error_messages[SANDESHA2_ERROR_SEQ_ID_BEAN_NOT_SET] =
        "Sequence id bean is not set";
    sandesha2_error_messages[SANDESHA2_ERROR_EMPTY_ACK_REQUEST_MESSAGE] =
        "Empty AckRequest messages can only be sent with the v1_1 spec"; 
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_GENERATE_SEQ_REPORT_FOR_GIVEN_INTERNAL_SEQ_ID] =
        "Cannot generate the sequence report for the given internalSequenceID";
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_SEND_ACK_REQUEST_MESSAGE_SINCE_IS_NOT_ACTIVE] =
        "Canot send the ackRequest message since it is not active";
    sandesha2_error_messages[SANDESHA2_ERROR_SPEC_VERSION_DOES_NOT_DEFINE_A_ACK_REQUEST_ACTION] =
        "This spec version does not define a ackRequest action";
    sandesha2_error_messages[SANDESHA2_ERROR_SPEC_VERSION_DOES_NOT_DEFINE_A_ACK_REQUEST_SOAP_ACTION] = 
        "This spec version does not define a ackRequest soap action";
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_FIND_SEQ_ID] =
        "Cannot find the sequence id";
    sandesha2_error_messages[SANDESHA2_ERROR_RM_VERSION_DOES_NOT_ALLOW_SEQ_CLOSING] =
        "This RM version does not allow sequence closing";
    sandesha2_error_messages[NOT_A_VALID_TERMINATED_SEQ] = 
        "Not a valid terminated sequence. Internal sequence Bean is not \
         available for the given sequence";
    sandesha2_error_messages[NOT_A_VALID_TIMEDOUT_SEQ] =
        "Not a valid timedout sequence. Internal sequence Bean is not \
         available for the given sequence";
    sandesha2_error_messages[SANDESHA2_ERROR_COMPLETED_MSGS_BEAN_IS_NULL] =
        "Completed messages bean is null, for the sequence"; 
    sandesha2_error_messages[SANDESHA2_ERROR_UNRECORDED_SEQ_ID] =
        "Sequence id is not recoreded"; 
    sandesha2_error_messages[SANDESHA2_ERROR_TO_IS_NULL] =
        "To is NULL";
    sandesha2_error_messages[SANDESHA2_ERROR_CREATE_SEQ_PART_IS_NULL] =
        "Create Sequence Part is NULL";
    sandesha2_error_messages[SANDESHA2_ERROR_ACKS_TO_IS_NULL] =
        "AcksTo is NULL";
    sandesha2_error_messages[SANDESHA2_ERROR_CREATE_SEQ_MSG_DOES_NOT_HAVE_WSA_ACTION_VALUE] = 
        "Create sequence message does not have the WSA:Action value";
    sandesha2_error_messages[SANDESHA2_ERROR_CREATE_SEQ_MSG_DOES_NOT_HAVE_VALID_RM_NS_VALUE]=
        "Create sequence message does not has a valid RM namespace value. \
        Can't decide the RM version";
    sandesha2_error_messages[SANDESHA2_ERROR_APPLICATION_MSG_NULL]=
        "Application message is NULL";
    sandesha2_error_messages[SANDESHA2_ERROR_CONF_CTX_NULL]=
        "Configuration Context is NULL";
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_FIND_RM_VERSION_OF_GIVEN_MSG]=
        "Cannot find the rmVersion of the given message";
    sandesha2_error_messages[SANDESHA2_ERROR_RM_NS_VALUE_IS_DIFFERENT_FROM_REGISTERED_NS_FOR_SEQ]=
        "Given message has rmNamespace value, which is different from the \
                        reqistered namespace for the sequence";
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_INIT_MSG]=
        "Cannot initialize the message";
    sandesha2_error_messages[SANDESHA2_ERROR_CANNOT_PROCESS_MSG]=
        "Error in processing the message";
    sandesha2_error_messages[SANDESHA2_ERROR_INVALID_STRING_ARRAY]=
        "String array is invalid";
    sandesha2_error_messages[SANDESHA2_ERROR_PENDING_HEADER_MUST_HAVE_ATTRIBUTE_PENDING]=
        "MessagePending header must have an attribute named 'pending'";
    sandesha2_error_messages[SANDESHA2_ERROR_ATTRIBUTE_PENDING_MUST_HAVE_VALUE_TRUE_OR_FALSE]=
        "Attribute 'pending' must have value 'true' or 'false'";
    sandesha2_error_messages[SANDESHA2_ERROR_MAKE_CONNECTION_ELEMENT_SHOULD_HAVE_AT_LEAST_ADDRESS_OR_IDENTIFIER]=
        "MakeConnection element should have at lease one of Address and Identifier subelements";
    /* Invalid MakeConnection object. Both Identifier and Address are null */
    sandesha2_error_messages[SANDESHA2_ERROR_INVALID_MAKE_CONNECTION_BOTH_IDENTIFER_AND_ADDRESS_NULL]=
        "Invalid MakeConnection object. Both Identifier and Address are null";
    sandesha2_error_messages[SANDESHA2_ERROR_MAKE_CONNECTION_IS_NOT_SUPPORTED_IN_THIS_RM_VERSION]=
        "MakeConnection is not supported in this RM version";
    sandesha2_error_messages[SANDESHA2_ERROR_INVALID_MAKE_CONNECTION_MSG]=
        "Invalid MakeConnection message. Either Address or Identifier must be present";

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_error_impl_free (axis2_error_t *error)
{
    if (NULL != error && NULL != error->ops)
    {
        free (error->ops);
    }
    if (NULL != error)
    {
        free (error); 
    }
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_error_t* AXIS2_CALL
sandesha2_error_create (axis2_allocator_t * allocator)
{
    axis2_error_t *error = NULL;
    if (!allocator)
        return NULL;

    error = axis2_error_create(allocator);

    if (!error)
        return NULL;

    /*error->ops->get_extended_message = sandesha2_error_impl_get_message;*/
    error->ops->set_error_number = sandesha2_error_impl_set_error_number;
    error->ops->set_status_code = sandesha2_error_impl_set_status_code;
    error->ops->get_status_code = sandesha2_error_impl_get_status_code;
    error->ops->free            = sandesha2_error_impl_free;

    return error;
}

const axis2_char_t * AXIS2_CALL
sandesha2_error_impl_get_message (
        const axis2_error_t *error)
{
    if (error && error->error_number >= AXIS2_ERROR_NONE && error->error_number 
            < AXIS2_ERROR_LAST)
    {
        return AXIS2_ERROR_GET_MESSAGE(error);
    }
    else if(error && error->error_number >= SANDESHA2_ERROR_NONE && error->
            error_number < SANDESHA2_ERROR_LAST)
    {
        return sandesha2_error_messages[error->error_number];
    }
    
    return "Invalid Error Number";
}

axis2_status_t AXIS2_CALL
sandesha2_error_impl_set_error_number (
        axis2_error_t *error, 
        axis2_error_codes_t error_number)
{
    error->error_number = error_number; 
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_error_impl_set_status_code (axis2_error_t *error, axis2_status_codes_t status_code)
{
    error->status_code = status_code; 
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_error_impl_get_status_code (
        axis2_error_t *error)
{
    return error->status_code;
}

