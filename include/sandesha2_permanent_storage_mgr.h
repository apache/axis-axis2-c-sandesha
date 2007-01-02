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
 
#ifndef SANDESHA2_PERMANENT_STORAGE_MGR_H
#define SANDESHA2_PERMANENT_STORAGE_MGR_H

/**
  * @file sandesha2_permanent_storage_mgr.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_conf_ctx.h>
#include <sqlite3.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct sandesha2_storage_mgr;

/** 
 * @ingroup sandesha2_storage
 * @{
 */

AXIS2_EXTERN sandesha2_storage_mgr_t* AXIS2_CALL
sandesha2_permanent_storage_mgr_create(
    const axis2_env_t *env, 
    axis2_conf_ctx_t *conf_ctx);

/*sqlite3 * AXIS2_CALL
sandesha2_permanent_storage_mgr_get_db(
    struct sandesha2_storage_mgr *storage_mgr,
    const axis2_env_t *env);*/

unsigned long *AXIS2_CALL
sandesha2_permanent_transaction_get_thread_id(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_PERMANENT_STORAGE_MGR_H*/
