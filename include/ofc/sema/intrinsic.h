/* Copyright 2015 Codethink Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ofc_sema_intrinsic_h__
#define __ofc_sema_intrinsic_h__

typedef struct ofc_sema_intrinsic_s ofc_sema_intrinsic_t;

const ofc_sema_intrinsic_t* ofc_sema_intrinsic(
	const ofc_sema_scope_t* scope,
	ofc_str_ref_t name);

/* This takes ownership of args and deletes on failure. */
ofc_sema_expr_list_t* ofc_sema_intrinsic_cast(
	ofc_sparse_ref_t src,
	const ofc_sema_intrinsic_t* intrinsic,
	ofc_sema_expr_list_t* args);

const ofc_sema_type_t* ofc_sema_intrinsic_type(
	const ofc_sema_intrinsic_t* intrinsic,
	ofc_sema_expr_list_t* args);

bool ofc_sema_intrinsic_print(
	ofc_colstr_t* cs,
	const ofc_sema_intrinsic_t* intrinsic);

#endif
