/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
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
 
#ifndef SANDESHA2_ERROR_H
#define SANDESHA2_ERROR_H

#include <axis2_error.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup sandesha2_error
 * @ingroup Sandesha2 Error
 * @{
 */
   /**
    * Since we use the same env->error from Axis2 we need to start from
    * a AXIS2_ERROR_LAST + some number to prevent ambiguity 
    */  
    #define SANDESHA2_ERROR_CODES_START (AXIS2_ERROR_LAST + 1000)
	/**
    * \brief Sandesha2 error codes
    *
    * Set of error codes for Sandesha2
    */
    enum sandesha2_error_codes
    { 
        /* No error */
        SANDESHA2_ERROR_NONE = SANDESHA2_ERROR_CODES_START,
        /* A namespace that is not supported by Sandesha2 */
        SANDESHA2_ERROR_UNSUPPORTED_NS,
        /* Object cannot generated since the om_element corresponding to
         * that is NULL */
        SANDESHA2_ERROR_NULL_OM_ELEMENT,
        /* The om element has no child elements or attributes */
        SANDESHA2_ERROR_EMPTY_OM_ELEMENT,
        /* om_element cannot be generated since the element corresponding 
         * to that is NULL*/
        SANDESHA2_ERROR_TO_OM_NULL_ELEMENT,
        /* Required OM attribute is NULL */
        SANDESHA2_ERROR_NULL_OM_ATTRIBUTE,
        /* An invalid number found during object to om conversion*/
        SANDESHA2_ERROR_TO_OM_INVALID_NUMBER,
        /* Ack Final now allowed in the spec */
        SANDESHA2_ERROR_ACK_FINAL_NOT_ALLOWED,
        /* 'To' address is not set */
        SANDESHA2_ERROR_TO_ADDRESS_IS_NOT_SET,
        /* Service Context is NULL */
        SANDESHA2_ERROR_SVC_CTX_IS_NULL,
        /* Result is not unique */
        SANDESHA2_ERROR_NON_UNIQUE_RESULT,
        /* Key is NULL. Cannot insert */
        SANDESHA2_ERROR_KEY_IS_NULL,
        /* Ack None now allowed in the spec */
        SANDESHA2_ERROR_ACK_NONE_NOT_ALLOWED,
        /* unknown storage mgr */
        SANDESHA2_ERROR_UNKNOWN_STORAGE_MGR,
        /* Sandesha configuration is not set in axis2 conf*/
        SANDESHA2_ERROR_CONFIGURATION_NOT_SET,
        /* NULL msg context */
        SANDESHA2_ERROR_NULL_MSG_CTX,
        /* Error occured while sending fault */
        SANDESHA2_ERROR_SENDING_FAULT,
        /* Invalid message number specified */
        SANDESHA2_ERROR_INVALID_MSG_NUM,
        /* Specified seq does not exist */
        SANDESHA2_ERROR_SEQ_NOT_EXIST,
        /* RM Spec version is invalid */
        SANDESHA2_ERROR_INVALID_SPEC_VERSION,
        /* NULL axis2 configuration context */
        SANDESHA2_ERROR_NULL_CONF_CTX,
        /* Error occured while sending ack */
        SANDESHA2_ERROR_SENDING_ACK,
        /* NULL to epr is specified */
        SANDESHA2_ERROR_NULL_TO,
        /* NULL sequnce given for processing */
        SANDESHA2_ERROR_NULL_SEQ,
        /* Storage Map not present */
        SANDESHA2_ERROR_STORAGE_MAP_NOT_PRESENT,
        /* Entry is not present for updating */
        SANDESHA2_ERROR_ENTRY_IS_NOT_PRESENT_FOR_UPDATING,
        /* Invalid namespace uri */
        SANDESHA2_ERROR_INVALID_NS_URI,
        /** The following has to be the last error value all the time.
            All other error codes should appear above this.
            SANDESHA_ERROR_LAST is used to track the number of error codes present
            for the purpose of sizing the error message array.
          */
        SANDESHA2_ERROR_LAST
    };
        
/** @} */
#ifdef __cplusplus
}
#endif
 
#endif /*SANDESHA2_ERROR_H*/
