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
 
#ifndef SANDESHA2_IN_ORDER_INVOKER_H
#define SANDESHA2_IN_ORDER_INVOKER_H

/**
  * @file sandesha2_in_order_invoker.h
  * @brief 
  */

#include <axis2_utils_defines.h>
#include <axis2_env.h>
#include <axis2_conf_ctx.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @ingroup sandesha2_in_order_invoker
 * @{
 */
 
 typedef struct sandesha2_in_order_invoker_ops sandesha2_in_order_invoker_ops_t;
 typedef struct sandesha2_in_order_invoker sandesha2_in_order_invoker_t;
 /**
 * @brief Sandesha2 InOrder Invoker ops struct
 * Encapsulator struct for ops of sandesha2_in_order_invoker
 */
AXIS2_DECLARE_DATA struct sandesha2_in_order_invoker_ops
{
    
    axis2_status_t (AXIS2_CALL *
        stop_invoker_for_seq) 
            (sandesha2_in_order_invoker_t *invoker,
            const axis2_env_t *env,
            axis2_char_t *seq_id);
            
    axis2_status_t (AXIS2_CALL *
        stop_invoking) 
            (sandesha2_in_order_invoker_t *invoker,
            const axis2_env_t *env);
            
    axis2_bool_t (AXIS2_CALL *
        is_invoker_started) 
            (sandesha2_in_order_invoker_t *invoker,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        run_invoker_for_seq) 
            (sandesha2_in_order_invoker_t *invoker,
            const axis2_env_t *env,
            axis2_conf_ctx_t *conf_ctx,
            axis2_char_t *seq_id);
            
    axis2_status_t (AXIS2_CALL *
        run) 
            (sandesha2_in_order_invoker_t *invoker,
            const axis2_env_t *env);
            
    axis2_status_t (AXIS2_CALL *
        free) 
            (sandesha2_in_order_invoker_t *invoker,
            const axis2_env_t *env);
};

/**
 * @brief sandesha2_in_order_invoker_ops
 *    sandesha2_in_order_invoker_ops
 */
AXIS2_DECLARE_DATA struct sandesha2_in_order_invoker
{
    sandesha2_in_order_invoker_ops_t *ops;
};

AXIS2_EXTERN sandesha2_in_order_invoker_t* AXIS2_CALL
sandesha2_in_order_invoker_create(
						const axis2_env_t *env);
                        
/************************** Start of function macros **************************/
#define SANDESHA2_IN_ORDER_INVOKER_FREE(invoker, env) \
    ((invoker)->ops->free (invoker, env))
    
#define SANDESHA2_IN_ORDER_INVOKER_STOP_FOR_SEQ(invoker, env, id) \
    ((invoker)->ops->stop_invoker_for_seq(invoker, env, id))
    
#define SANDESHA2_IN_ORDER_INVOKER_STOP_INVOKING(invoker, env) \
    ((invoker)->ops->stop_invoking(invoker, env))
    
#define SANDESHA2_IN_ORDER_INVOKER_IS_INVOKER_STARTED(invoker, env) \
    ((invoker)->ops->is_invoker_started(invoker, env))
    
#define SANDESHA2_IN_ORDER_INVOKER_RUN_FOR_SEQ(invoker, env, \
    conf_ctx, id) \
    ((invoker)->ops->run_invoker_for_seq(invoker, env, conf_ctx, id))

#define SANDESHA2_IN_ORDER_INVOKER_RUN(invoker, env) \
    ((invoker)->ops->run(invoker, env))
/************************** End of function macros ****************************/

/** @} */
#ifdef __cplusplus
}
#endif

#endif /*SANDESHA2_IN_ORDER_INVOKER_H*/
