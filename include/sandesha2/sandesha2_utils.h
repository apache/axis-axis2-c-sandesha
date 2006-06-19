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
 
#ifndef SANDESHA2_UTILS_H
#define SANDESHA2_UTILS_H

/**
  * @file sandesha2_utils.h
  * @brief 
  */



#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup sandesha2_utils
 * @ingroup sandesha2_util
 * @{
 */
AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_utils_remove_soap_body_part(const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope, axis2_qname_t *qname);    
/** @} */
#ifdef __cplusplus
}
#endif

#endif                          /* SANDESHA2_UTILS_H */

