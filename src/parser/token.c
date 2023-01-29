/*!
 * @file
 * @brief File with token implementation.
 */

#include "token.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




static const char TOKEN_TYPES[][16] =
{
	"NUMBER",
	"VAR",
	"OP",
	"FUNC",
	"UNKNOWN"
};


static const size_t TOKEN_TYPES_LEN[] =
{
	sizeof "NUMBER",
	sizeof "VAR",
	sizeof "OP",
	sizeof "FUNC",
	sizeof "UNKNOWN"
};



/*!
 * @brief Parse value of token type.
 
 @note Function may use memory allocation.
 *
 * @return Success of the parsing process.
 */
static bool token_parse_value
(
	token_t*    token, /*!< [in,out[ parsed token.                           */
	const char* str,   /*!< [in]     parsed string.                          */
	size_t      len    /*!< [in]     length of parsed data.                  */
)
{
	switch (token->type)
	{
		case TOKEN_NUMBER:
			return sscanf(str, "%lf", &token->value.number) == 1;

		case TOKEN_OP:
			token->value.operation = str[0];
			return len == 1;

		case TOKEN_VAR:
		case TOKEN_FUNC:
			token->value.ident = (char*) calloc(len + 1, 1);
			if (!token->value.ident)
				return false;

			memcpy(token->value.ident, str, len);
			return true;

		case TOKEN_UNKNOWN:
		default:
			return false;
	}
}




bool token_parse (token_t* token, const char* str, size_t len)
{
	assert (token);
	assert (str);

	for (size_t type = 0; type < TOKEN_UNKNOWN; ++type)
	{
		// - 1 because of the \0 char at the end of every string.
		size_t type_len = TOKEN_TYPES_LEN[type] - 1;
		if (len >= type_len + 2
		    && !memcmp(str, TOKEN_TYPES[type], sizeof type_len)
		    && str[type_len] == '('
			&& str[len - 1] == ')')
		{
			str += + type_len + 1;
			len -= type_len + 2;
			token->type = type;
			return token_parse_value(token, str, len);
		}
	}

	token->type = TOKEN_UNKNOWN;
	return false;
}


bool token_equal (const token_t* t1, const token_t* t2)
{
	assert (t1);
	assert (t2);

	if (t1->type != t2->type)
		return false;

	switch (t1->type)
	{
		case TOKEN_NUMBER:
			return t1->value.number == t2->value.number;

		case TOKEN_OP:
			return t1->value.operation == t2->value.operation;

		case TOKEN_FUNC:
		case TOKEN_VAR:
			return strcmp(t1->value.ident, t2->value.ident) == 0;

		case TOKEN_UNKNOWN:
		default:
			return true;
	}
}


void token_print (const token_t* t, FILE* output)
{
	assert (t);
	assert (output);

	fputs(TOKEN_TYPES[t->type], output);
	fputc('(', output);

	switch (t->type)
	{
		case TOKEN_NUMBER:
			fprintf(output, "%lf", t->value.number);
			break;

		case TOKEN_OP:
			fputc(t->value.operation, output);
			break;

		case TOKEN_FUNC:
		case TOKEN_VAR:
			fputs(t->value.ident, output);
			break;

		case TOKEN_UNKNOWN:
		default:
			break;
	}

	fputc(')', output);
}


void token_destroy (token_t* t)
{
	if (!t)
		return;

	switch (t->type)
	{
		case TOKEN_FUNC:
		case TOKEN_VAR:
			free(t->value.ident);
			break;

		case TOKEN_NUMBER:
		case TOKEN_OP:
		case TOKEN_UNKNOWN:
		default:
			break;
	}

	t->value.ident = NULL;
}


bool token_copy (token_t* dest, const token_t* src)
{
	assert (dest);
	assert (src);

	switch (dest->type = src->type)
	{
		case TOKEN_NUMBER:
			dest->value.number = src->value.number;
			return true;

		case TOKEN_OP:
			dest->value.operation = src->value.operation;
			return true;

		case TOKEN_FUNC:
		case TOKEN_VAR:
			dest->value.ident = (char*) realloc(dest->value.ident,
			                                    strlen(src->value.ident) + 1);
			if (!dest->value.ident)
				return false;

			strcpy(dest->value.ident, src->value.ident);
			return true;

		case TOKEN_UNKNOWN:
		default:
			return true;
	}
}


bool token_move (token_t* dest, token_t* src)
{
	assert (dest);
	assert (src);

	token_destroy(dest);
	dest->type       = src->type;
	dest->value      = src->value;
	src->type        = TOKEN_UNKNOWN;
	src->value.ident = NULL;

	return true;
}
