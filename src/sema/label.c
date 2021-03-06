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

static void ofc_sema_label__delete(
	ofc_sema_label_t* label)
{
	if (!label)
		return;

	if (label->type == OFC_SEMA_LABEL_FORMAT)
		ofc_sema_format_delete(label->format);

	free(label);
}

static ofc_sema_label_t* ofc_sema_label__stmt(
	unsigned number, const ofc_sema_stmt_t* stmt)
{
	ofc_sema_label_t* label
		= (ofc_sema_label_t*)malloc(
			sizeof(ofc_sema_label_t));
	if (!label) return NULL;

	label->type   = OFC_SEMA_LABEL_STMT;
	label->number = number;
	label->stmt   = stmt;

	return label;
}

static ofc_sema_label_t* ofc_sema_label__format(
	unsigned number, ofc_sema_format_t* format)
{
	if (!format)
		return NULL;

	ofc_sema_label_t* label
		= (ofc_sema_label_t*)malloc(
			sizeof(ofc_sema_label_t));
	if (!label) return NULL;

	label->type   = OFC_SEMA_LABEL_FORMAT;
	label->number = number;
	label->format = format;

	return label;
}

static const unsigned* ofc_sema_label__number(
	const ofc_sema_label_t* label)
{
	return (label ? &label->number : NULL);
}

static const ofc_sema_stmt_t* ofc_sema_label__stmt_key(
	const ofc_sema_label_t* label)
{
	return (label ? label->stmt : NULL);
}

static bool ofc_sema_label__compare(
	const unsigned* a, const unsigned* b)
{
	if (!a || !b)
		return false;
	return (*a == *b);
}

static bool ofc_sema_label__stmt_ptr_compare(
	const ofc_sema_stmt_t* a,
	const ofc_sema_stmt_t* b)
{
	if (!a || !b)
		return false;
	return (a == b);
}

static uint8_t ofc_sema_label__hash(const unsigned* label)
{
	if (!label)
		return 0;

	unsigned h = *label;
	h ^= (h >> (sizeof(h) * 4));
	h ^= (h >> (sizeof(h) * 2));

	return (h & 0xFF);
}

static uint8_t ofc_sema_label__stmt_hash(
	const ofc_sema_stmt_t* stmt)
{
	uintptr_t p = (uintptr_t)stmt;
	uint8_t h = 0;

	unsigned i;
	for (i = 0; i < sizeof(p); i++)
		h += ((p >> (i * 8)) & 0xFF);
	return h;
}

ofc_sema_label_map_t* ofc_sema_label_map_create(void)
{
	ofc_sema_label_map_t* map
		= (ofc_sema_label_map_t*)malloc(
			sizeof(ofc_sema_label_map_t));
	if (!map) return NULL;

	map->label = ofc_hashmap_create(
		(void*)ofc_sema_label__hash,
		(void*)ofc_sema_label__compare,
		(void*)ofc_sema_label__number,
		NULL);

	map->stmt = ofc_hashmap_create(
		(void*)ofc_sema_label__stmt_hash,
		(void*)ofc_sema_label__stmt_ptr_compare,
		(void*)ofc_sema_label__stmt_key,
		(void*)ofc_sema_label__delete);

	map->end_block = ofc_hashmap_create(
		(void*)ofc_sema_label__stmt_hash,
		(void*)ofc_sema_label__stmt_ptr_compare,
		(void*)ofc_sema_label__stmt_key,
		(void*)ofc_sema_label__delete);

	map->format = ofc_sema_format_label_list_create();

	if (!map->label
		|| !map->stmt
		|| !map->end_block
		|| !map->format)
	{
		ofc_sema_label_map_delete(map);
		return NULL;
	}

	return map;
}

void ofc_sema_label_map_delete(
	ofc_sema_label_map_t* map)
{
	if (!map) return;

	ofc_sema_format_label_list_delete(map->format);
	ofc_hashmap_delete(map->end_block);
	ofc_hashmap_delete(map->stmt);
	ofc_hashmap_delete(map->label);

	free(map);
}

bool ofc_sema_label_map_add_stmt(
	ofc_sema_label_map_t* map, unsigned label,
	const ofc_sema_stmt_t* stmt)
{
	if (!map || !map->label || !map->stmt)
		return false;

	const ofc_sema_label_t* duplicate
		= ofc_hashmap_find(map->label, &label);
	if (duplicate)
	{
		ofc_sparse_ref_error(stmt->src,
			"Re-definition of label %d", label);
		return false;
	}

	if (label == 0)
	{
		ofc_sparse_ref_warning(stmt->src,
			"Label zero isn't supported in standard Fortran");
	}

	ofc_sema_label_t* l
		= ofc_sema_label__stmt(label, stmt);
	if (!l) return false;

	if (!ofc_hashmap_add(
		map->stmt, l))
	{
		ofc_sema_label__delete(l);
		return false;
	}

	if (!ofc_hashmap_add(
		map->label, l))
	{
		/* This should never happen. */
		abort();
	}

	return true;
}

bool ofc_sema_label_map_add_end_block(
	ofc_sema_label_map_t* map, unsigned label,
	const ofc_sema_stmt_t* stmt)
{
	if (!map || !map->label || !map->stmt)
		return false;

	const ofc_sema_label_t* duplicate
		= ofc_hashmap_find(map->label, &label);
	if (duplicate)
	{
		ofc_sparse_ref_error(stmt->src,
			"Re-definition of label %d", label);
		return false;
	}

	if (label == 0)
	{
		ofc_sparse_ref_warning(stmt->src,
			"Label zero isn't supported in standard Fortran");
	}

	ofc_sema_label_t* l
		= ofc_sema_label__stmt(label, stmt);
	if (!l) return false;

	if (!ofc_hashmap_add(
		map->end_block, l))
	{
		ofc_sema_label__delete(l);
		return false;
	}

	if (!ofc_hashmap_add(
		map->label, l))
	{
		/* This should never happen. */
		abort();
	}

	return true;
}

bool ofc_sema_label_map_add_format(
	const ofc_parse_stmt_t* stmt,
	ofc_sema_label_map_t* map, unsigned label,
	ofc_sema_format_t* format)
{
	if (!map || !map->label
		|| !map->format|| !format)
		return false;

	const ofc_sema_label_t* duplicate
		= ofc_hashmap_find(map->label, &label);
	if (duplicate)
	{
		ofc_sparse_ref_error(stmt->src,
			"Re-definition of label %d", label);
		return false;
	}

	if (label == 0)
	{
		ofc_sparse_ref_warning(stmt->src,
			"Label zero isn't supported in standard Fortran");
	}

	ofc_sema_label_t* l
		= ofc_sema_label__format(
			label, format);
	if (!l) return false;

	if (!ofc_sema_format_label_list_add(
		map->format, l))
	{
		/* Don't delete because we don't yet own format. */
		free(l);
		return false;
	}

	if (!ofc_hashmap_add(map->label, l))
	{
		/* This should never happen. */
		abort();
	}

	return true;
}

const ofc_sema_label_t* ofc_sema_label_map_find(
	const ofc_sema_label_map_t* map, unsigned label)
{
	if (!map)
		return NULL;
	return ofc_hashmap_find(
		map->label, &label);
}

const ofc_sema_label_t* ofc_sema_label_map_find_stmt(
	const ofc_sema_label_map_t* map,
	const ofc_sema_stmt_t*      stmt)
{
	if (!map)
		return NULL;
	return ofc_hashmap_find(
		map->stmt, stmt);
}

const ofc_sema_label_t* ofc_sema_label_map_find_end_block(
	const ofc_sema_label_map_t* map,
	const ofc_sema_stmt_t*      stmt)
{
	if (!map)
		return NULL;
	return ofc_hashmap_find(
		map->end_block, stmt);
}

ofc_sema_format_label_list_t*
	ofc_sema_format_label_list_create(void)
{
	ofc_sema_format_label_list_t* list
		= (ofc_sema_format_label_list_t*)malloc(
			sizeof(ofc_sema_format_label_list_t));
	if (!list) return NULL;

	list->count  = 0;
	list->format = NULL;

	return list;
}

void ofc_sema_format_label_list_delete(
	ofc_sema_format_label_list_t* list)
{
	if (!list) return;

	if (list->format)
	{
		unsigned i;
		for (i = 0; i < list->count; i++)
			ofc_sema_label__delete(list->format[i]);
		free(list->format);
	}

	free(list);
}

bool ofc_sema_format_label_list_add(
	ofc_sema_format_label_list_t* list,
	ofc_sema_label_t* format)
{
	if (!list || !format) return false;

	ofc_sema_label_t** nformat
		= (ofc_sema_label_t**)realloc(list->format,
			(sizeof(ofc_sema_label_t*) * (list->count + 1)));
	if (!nformat) return false;
	list->format = nformat;

	list->format[list->count++] = format;
	return true;
}

bool ofc_sema_format_label_list_print(
	ofc_colstr_t* cs, unsigned indent,
	ofc_sema_format_label_list_t* list)
{
	if (!cs || !list)
		return false;

	if (list->count > 0)
	{
		if (!ofc_colstr_newline(cs, indent, NULL))
			return false;
	}

	unsigned i;
	for (i = 0; i < list->count; i++)
	{
		if (!list->format[i]->number) return false;

		unsigned label_num = list->format[i]->number;

		if (!ofc_colstr_newline(cs, indent, &label_num)
			|| !ofc_sema_format_label_print(cs, list->format[i]))
			return false;
	}

	return true;
}

bool ofc_sema_format_label_print(ofc_colstr_t* cs,
	ofc_sema_label_t* label)
{
	if (!cs || (label->type != OFC_SEMA_LABEL_FORMAT))
		return false;

	if (!ofc_sema_format_print(cs, label->format))
		return false;

	return true;
}
