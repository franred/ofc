#include "parse.h"


static ofc_parse_pointer_t* ofc_parse_pointer(
	const ofc_sparse_t* src, const char* ptr,
	ofc_parse_debug_t* debug,
	unsigned* len)
{
	unsigned i = 0;
	if (ptr[i++] != '(')
		return NULL;

	ofc_parse_pointer_t* pointer
		= (ofc_parse_pointer_t*)malloc(
			sizeof(ofc_parse_pointer_t));
	if (!pointer) return NULL;

	unsigned dpos = ofc_parse_debug_position(debug);

	unsigned l = ofc_parse_name(
		src, &ptr[i], debug, &pointer->name);
	if (l == 0)
	{
		free(pointer);
		return NULL;
	}
	i += l;

	if (ptr[i++] != ',')
	{
		free(pointer);
		ofc_parse_debug_rewind(debug, dpos);
		return NULL;
	}

	l = ofc_parse_name(
		src, &ptr[i], debug, &pointer->target);
	if (l == 0)
	{
		free(pointer);
		ofc_parse_debug_rewind(debug, dpos);
		return NULL;
	}
	i += l;

	if (ptr[i++] != ')')
	{
		free(pointer);
		ofc_parse_debug_rewind(debug, dpos);
		return NULL;
	}

	if (len) *len = i;
	return pointer;
}

bool ofc_parse_pointer_print(
	ofc_colstr_t* cs, const ofc_parse_pointer_t* pointer)
{
	if (!pointer)
		return false;

	return (ofc_colstr_atomic_writef(cs, "(")
		&& ofc_str_ref_print(cs, pointer->name)
		&& ofc_colstr_atomic_writef(cs, ", ")
		&& ofc_str_ref_print(cs, pointer->target)
		&& ofc_colstr_atomic_writef(cs, ")"));
}


ofc_parse_pointer_list_t* ofc_parse_pointer_list(
	const ofc_sparse_t* src, const char* ptr,
	ofc_parse_debug_t* debug,
	unsigned* len)
{
	ofc_parse_pointer_list_t* list
		= (ofc_parse_pointer_list_t*)malloc(
			sizeof(ofc_parse_pointer_list_t));
	if (!list) return NULL;

	list->count = 0;
	list->pointer = NULL;

	unsigned i = ofc_parse_list(
		src, ptr, debug, ',',
		&list->count, (void***)&list->pointer,
		(void*)ofc_parse_pointer, free);
	if (i == 0)
	{
		free(list);
		return NULL;
	}

	if (len) *len = i;
	return list;
}

void ofc_parse_pointer_list_delete(
	ofc_parse_pointer_list_t* list)
{
	if (!list)
		return;

	ofc_parse_list_delete(
		list->count, (void**)list->pointer,
		free);
	free(list);
}

bool ofc_parse_pointer_list_print(
	ofc_colstr_t* cs, const ofc_parse_pointer_list_t* list)
{
	return ofc_parse_list_print(
		cs, list->count, (const void**)list->pointer,
		(void*)ofc_parse_pointer_print);
}
