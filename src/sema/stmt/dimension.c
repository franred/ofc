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

#include "ofc/sema.h"


bool ofc_sema_stmt_dimension(
	ofc_sema_scope_t* scope,
	const ofc_parse_stmt_t* stmt)
{
	if (!scope || !stmt
		|| (stmt->type != OFC_PARSE_STMT_DIMENSION)
		|| !stmt->dimension)
		return false;

	unsigned i;
	for (i = 0; i < stmt->dimension->count; i++)
	{
		ofc_parse_lhs_t* lhs
			= stmt->dimension->lhs[i];
		if (!lhs) continue;

		if (lhs->type != OFC_PARSE_LHS_ARRAY)
		{
			ofc_sparse_ref_error(lhs->src,
				"DIMENSION entry must contain array dimensions.");
			return false;
		}

		if (!lhs->parent
			|| (lhs->parent->type != OFC_PARSE_LHS_VARIABLE))
		{
			ofc_sparse_ref_error(lhs->src,
				"Invalid array layout in DIMENSION");
			return false;
		}

		ofc_sparse_ref_t base_name;
		if (!ofc_parse_lhs_base_name(
			*lhs, &base_name))
			return false;

		const ofc_sema_decl_t* decl
			= ofc_sema_scope_decl_find(
				scope, base_name.string, true);
		if (decl)
		{
			ofc_sparse_ref_error(lhs->src,
				"Can't modify dimensions of declaration after use");
			return false;
		}

		ofc_sema_spec_t* spec
			= ofc_sema_scope_spec_modify(
				scope, base_name);
		if (!spec)
		{
			ofc_sparse_ref_error(lhs->src,
				"No declaration for '%.*s' and no valid IMPLICIT rule.",
				base_name.string.size, base_name.string.base);
			return false;
		}

		ofc_sema_array_t* array
			= ofc_sema_array(
				scope, lhs->array.index);
		if (!array) return false;

		if (spec->array)
		{
			bool conflict = !ofc_sema_array_compare(
				spec->array, array);
			ofc_sema_array_delete(array);

			if (conflict)
			{
				ofc_sparse_ref_error(lhs->src,
					"Conflicting array dimension specifications");
				return false;
			}

			ofc_sparse_ref_warning(lhs->src,
				"Multiple array dimension specifications");
		}
		else
		{
			spec->array = array;
		}
	}

	return true;
}
