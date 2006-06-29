/*
 * copyright 1999-2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef SANDESHA2_SEQUENCE_PROPERTY_BEAN_H
#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_H

#include <axis2_utils_defines.h>
#include <axis2_env.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_sequence_property_bean sandesha2_sequence_property_bean_t ;
typedef struct sandesha2_sequence_property_bean_ops sandesha2_sequence_property_bean_ops_t;

AXIS2_DECLARE_DATA struct sandesha2_sequence_property_bean_ops
{
	axis2_status_t (AXIS2_CALL *
		free) (struct sandesha2_sequence_property_bean *seq_property,
			const axis2_env_t *env);
	
	axis2_char_t *(AXIS2_CALL *
		get_name) (struct sandesha2_sequence_property_bean *seq_property,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_name) (struct sandesha2_sequence_property_bean *seq_property,
			const axis2_env_t *env,
			axis2_char_t *name);

	axis2_char_t *(AXIS2_CALL *
		get_sequence_id) (struct sandesha2_sequence_property_bean *seq_property,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_sequence_id) (struct sandesha2_sequence_property_bean *seq_property,
			const axis2_env_t *env,
			axis2_char_t *seq_id);
	
	axis2_char_t *(AXIS2_CALL *
		get_value) (struct sandesha2_sequence_property_bean *seq_property,
			const axis2_env_t *env);

	void (AXIS2_CALL *
		set_value) (struct sandesha2_sequence_property_bean *seq_property,
			const axis2_env_t *env,
			axis2_char_t *value);

};

AXIS2_DECLARE_DATA struct sandesha2_sequence_property_bean{
	sandesha2_sequence_property_bean_ops_t *ops;
};

/* constructors 
 */

AXIS2_EXTERN sandesha2_sequence_property_bean_t* AXIS2_CALL
	sandesha2_sequence_property_bean_create(const axis2_env_t *env);

AXIS2_EXTERN sandesha2_sequence_property_bean_t* AXIS2_CALL
	sandesha2_sequence_property_bean_create_with_data(const axis2_env_t *env,
			axis2_char_t *seq_id,
			axis2_char_t *prop_name,
			axis2_char_t *value);


#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_FREE(seq_prop_bean, env) \
	((seq_prop_bean)->ops->free(seq_prop_bean, env)

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_GET_NAME(seq_prop_bean, env) \
	((seq_prop_bean)->ops->get_name(seq_prop_bean, env)

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_SET_NAME(seq_prop_bean, env, name) \
	((seq_prop_bean)->ops->set_name(seq_prop_bean, env, name)

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_GET_SEQUENCE_ID(seq_prop_bean, env) \
	((seq_prop_bean)->ops->get_sequence_id(seq_prop_bean, env)

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_SET_SEQUENCE_ID(seq_prop_bean, env, seq_id) \
	((seq_prop_bean)->ops->set_sequence_id(seq_prop_bean, env, seq_id)

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_GET_VALUE(seq_prop_bean, env) \
	((seq_prop_bean)->ops->get_value(seq_prop_bean, env)

#define SANDESHA2_SEQUENCE_PROPERTY_BEAN_SET_VALUE(seq_prop_bean, env, value) \
	((seq_prop_bean)->ops->set_value(seq_prop_bean, env, value)

#ifdef __cplusplus
}

#endif
	
#endif /* End of SANDESHA2_SEQUENCE_PROPERTY_BEAN_H */
