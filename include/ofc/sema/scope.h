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

#ifndef __ofc_sema_scope_h__
#define __ofc_sema_scope_h__

typedef enum
{
	OFC_SEMA_SCOPE_GLOBAL = 0,
	OFC_SEMA_SCOPE_PROGRAM,
	OFC_SEMA_SCOPE_STMT_FUNC,
	OFC_SEMA_SCOPE_SUBROUTINE,
	OFC_SEMA_SCOPE_FUNCTION,
	OFC_SEMA_SCOPE_BLOCK_DATA,

	OFC_SEMA_SCOPE_COUNT
} ofc_sema_scope_e;

typedef struct
{
	unsigned count;
	ofc_sema_scope_t**       scope;
} ofc_sema_scope_list_t;


struct ofc_sema_scope_s
{
	ofc_sema_scope_t* parent;
	ofc_sema_scope_list_t*    child;

	const ofc_lang_opts_t* lang_opts;

	ofc_sema_scope_e     type;
	ofc_str_ref_t        name;
	ofc_sema_arg_list_t* args;

	bool external;
	bool intrinsic;

	ofc_sema_implicit_t*  implicit;

	ofc_sema_common_map_t* common;
	ofc_sema_spec_map_t*   spec;
	ofc_sema_decl_list_t*  decl;
	ofc_sema_equiv_list_t* equiv;
	ofc_sema_label_map_t*  label;
	/* namelist_list_map */

	ofc_sema_structure_list_t* structure;
	ofc_sema_structure_list_t* derived_type;

	union
	{
		ofc_sema_stmt_list_t* stmt;
		ofc_sema_expr_t*      expr;
	};
};


ofc_sema_scope_t* ofc_sema_scope_global(
	const ofc_lang_opts_t* lang_opts,
	const ofc_parse_stmt_list_t* list);

ofc_sema_scope_t* ofc_sema_scope_program(
	ofc_sema_scope_t* scope,
	const ofc_parse_stmt_t* stmt);
ofc_sema_scope_t* ofc_sema_scope_stmt_func(
	ofc_sema_scope_t* scope,
	const ofc_parse_stmt_t* stmt);
ofc_sema_scope_t* ofc_sema_scope_block_data(
	ofc_sema_scope_t* scope,
	const ofc_parse_stmt_t* stmt);

bool ofc_sema_scope_subroutine(
	ofc_sema_scope_t* scope,
	const ofc_parse_stmt_t* stmt);
bool ofc_sema_scope_function(
	ofc_sema_scope_t* scope,
	const ofc_parse_stmt_t* stmt);

bool ofc_sema_scope_is_root(
	const ofc_sema_scope_t* scope);
ofc_sema_scope_t* ofc_sema_scope_root(
	ofc_sema_scope_t* scope);

bool ofc_sema_scope_is_procedure(
	const ofc_sema_scope_t* scope);

void ofc_sema_scope_delete(
	ofc_sema_scope_t* scope);

const ofc_str_ref_t* ofc_sema_scope_get_name(
	const ofc_sema_scope_t* scope);
ofc_lang_opts_t ofc_sema_scope_get_lang_opts(
	const ofc_sema_scope_t* scope);

const ofc_sema_label_t* ofc_sema_scope_label_find(
	const ofc_sema_scope_t* scope, unsigned label);

ofc_sema_spec_t* ofc_sema_scope_spec_modify(
	ofc_sema_scope_t* scope, ofc_sparse_ref_t name);
ofc_sema_spec_t* ofc_sema_scope_spec_find_final(
	const ofc_sema_scope_t* scope, ofc_sparse_ref_t name);

bool ofc_sema_scope_equiv_add(
	ofc_sema_scope_t* scope, ofc_sema_equiv_t* equiv);

bool ofc_sema_scope_decl_add(
	ofc_sema_scope_t* scope, ofc_sema_decl_t* decl);
const ofc_sema_decl_t* ofc_sema_scope_decl_find(
	const ofc_sema_scope_t* scope, ofc_str_ref_t name, bool local);
ofc_sema_decl_t* ofc_sema_scope_decl_find_modify(
	ofc_sema_scope_t* scope, ofc_str_ref_t name, bool local);

bool ofc_sema_scope_derived_type_add(
	ofc_sema_scope_t* scope,
	ofc_sema_structure_t* structure);
ofc_sema_structure_t* ofc_sema_scope_derived_type_find(
	ofc_sema_scope_t* scope, ofc_str_ref_t name);

bool ofc_sema_scope_structure_add(
	ofc_sema_scope_t* scope,
	ofc_sema_structure_t* structure);
ofc_sema_structure_t* ofc_sema_scope_structure_find(
	ofc_sema_scope_t* scope, ofc_str_ref_t name);

const ofc_sema_scope_t* ofc_sema_scope_child_find(
	const ofc_sema_scope_t* scope, ofc_str_ref_t name);
ofc_sema_scope_t* ofc_sema_scope_child_find_modify(
	ofc_sema_scope_t* scope, ofc_str_ref_t name);

ofc_sema_common_t* ofc_sema_scope_common_find_create(
	ofc_sema_scope_t* scope, ofc_str_ref_t name);

bool ofc_sema_scope_print(
	ofc_colstr_t* cs, unsigned indent,
	const ofc_sema_scope_t* scope);

ofc_sema_scope_list_t* ofc_sema_scope_list_create(void);

bool ofc_sema_scope_list_add(
	ofc_sema_scope_list_t* list,
	ofc_sema_scope_t* scope);
bool ofc_sema_scope_list_print(
	ofc_colstr_t* cs, unsigned indent,
	const ofc_sema_scope_list_t* list);
void ofc_sema_scope_list_delete(
	ofc_sema_scope_list_t* list);

#endif
