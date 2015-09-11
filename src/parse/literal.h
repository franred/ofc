#ifndef __parse_literal_h__
#define __parse_literal_h__

#include <stdint.h>

typedef enum
{
	PARSE_LITERAL_BINARY,
	PARSE_LITERAL_OCTAL,
	PARSE_LITERAL_HEX,
	PARSE_LITERAL_HOLLERITH,
	PARSE_LITERAL_STRING,
	PARSE_LITERAL_UNSIGNED_INT,
	PARSE_LITERAL_SIGNED_INT,
	PARSE_LITERAL_REAL,
} parse_literal_e;

typedef struct
{
	parse_literal_e type;
	unsigned        kind;

	union
	{
		str_ref_t hollerith;

		uint64_t uint;
		int64_t  sint;

		long double real;
	};
} parse_literal_t;


unsigned parse_literal(
	const sparse_t* src, const char* ptr,
	parse_literal_t* literal);

#endif