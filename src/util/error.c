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
    sandesha2_error_messages[SANDESHA2_ERROR_TO_ADDRESS_IS_NOT_SET] = 
        "'To' address is not set";
    /* Service Context is NULL */
    sandesha2_error_messages[SANDESHA2_ERROR_SVC_CTX_IS_NULL] = 
        "Service context is NULL";
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
        "this spec version does not define a ackRequest action";
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

