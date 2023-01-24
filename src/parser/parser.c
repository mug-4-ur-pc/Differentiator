/*!
 * @file
 * @brief File with recursive descent parser implementation.
 */

#include "../tree/token_specific.h"
#include "../utilities/utilities.h"
#include "lexemes.h"
#include "parser.h"
#include "token.h"

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//TODO: Fix unary operation.


static bintree_t parse_number        (parser_t* parser);
static bintree_t parse_ident         (parser_t* parser);
static bintree_t parse_function      (parser_t* parser);
static bintree_t parse_factor        (parser_t* parser);
static bintree_t parse_term          (parser_t* parser);
static bintree_t parse_expr_of_prior (parser_t* paraer, int prior);



/*!
 * @brief Get operation or key charactes lexeme from given string.
 *
 * @return Success of parsing lexeme.
 */
static bool get_lexeme_op_or_key
(
	lexeme_t*   lexeme, /*!< [out] parsed lexeme.                            */
	const char* str     /*!< [in]  input string.                             */
)
{
	lexeme->length = 1;
	switch (*str)
	{
		case '(':
			lexeme->type = LEXEME_LPAREN;
			return true;

		case ')':
			lexeme->type = LEXEME_RPAREN;
			return true;

		case '+':
		case '-':
		case '*':
		case '/':
		case '^':
			lexeme->type = LEXEME_OP;
			return true;

		default:
			lexeme->type = LEXEME_UNKNOWN;
			fprintf(stderr, "%u:%u: Bad symbol '%c'\n",
			        lexeme->line, lexeme->pos, *str);
			return false;
	}
}

/*!
 * @brief Get identifier lexeme from given string.
 *
 * @return Success of parsing lexeme.
 */
static bool get_lexeme_ident
(
	lexeme_t*   lexeme, /*!< [out] parsed lexeme.                            */
	const char* str     /*!< [in]  input string.                             */
)
{
	unsigned length = 0;
	while (isident(str[++length]))
		continue;

	lexeme->type   = LEXEME_IDENT;
	lexeme->length = length;
	return true;
}

/*!
 * @brief Get number lexeme from given string.
 *
 * @return Success of parsing lexeme.
 */
static bool get_lexeme_number
(
	lexeme_t*   lexeme, /*!< [out] parsed lexeme.                            */
	const char* str     /*!< [in]  input string.                             */
)
{
	unsigned length = 0;
	while (isdigit(str[length]))
		++length;

	if (isident(str[length]))
	{
		while (isident(str[++length]))
			continue;

		lexeme->length = length;
		fprintf(stderr, "%u:%u: %.*s is not a number.\n",
		        lexeme->line, lexeme->pos, length, str);
		return false;
	}

	lexeme->type   = LEXEME_NUMBER;
	lexeme->length = length;
	return true;
}

/*!
 * @brief Get EOF lexeme from given string.
 *
 * @return Success of parsing lexeme.
 */
static bool get_lexeme_eof
(
	lexeme_t*   lexeme, /*!< [out] parsed lexeme.                            */
	const char* str     /*!< [in]  input string.                             */
)
{
	MAYBE_UNUSED(str);
	lexeme->type   = LEXEME_EOF;
	lexeme->length = 0;
	return true;
}

/*!
 * @brief Get lexeme from given string.
 *
 * @return Success of parsing lexeme.
 */
static bool get_lexeme
(
	lexeme_t*   lexeme, /*!< [out] parsed lexeme.                            */
	const char* str     /*!< [in]  input string.                             */
)
{
	lexeme->ptr  = str;
	lexeme->type = LEXEME_UNKNOWN;
	if (*str == '\0')
		return get_lexeme_eof(lexeme, str);

	if (isdigit(*str))
		return get_lexeme_number(lexeme, str);

	if (iskey(*str))
		return get_lexeme_op_or_key(lexeme, str);

	if (isident(*str))
		return get_lexeme_ident(lexeme, str);

	assert ("Unreachable" || false);
	return false;
}

/*!
 * @brief Skip all white-space characters.
 */
static void skip_spaces
(
	const char** lines, /*!< [in]     array of lines.                        */
	unsigned*    line,  /*!< [in,out] number of current line.                */
	unsigned*    pos    /*!< [in,out] current position in line.              */
)
{
	unsigned l  = *line;
	unsigned p  = *pos;
	char     ch = '\0';
	while (isspace(ch = lines[l][p]))
	{
		if (ch == '\n')
		{
			++l;
			p = 0;
		}
		else
			++p;
	}

	*line = l;
	*pos  = p;
}

/*!
 * @brief Split lines to lexemes.
 *
 * @note Don't forget to free memory.
 *
 * @return Array of lexemes. If an error has been occurred it returns NULL.
 */
static lexeme_t* split_to_lexemes
(
	const char** lines /*!< [in] array of lines.                             */
)
{
	const size_t LEX_INCREASE_CAP = 1000;
	size_t       index            = 0;
	unsigned     line             = 0;
	unsigned     pos              = 0;
	lexeme_t*    lexemes          = NULL;

	do
	{
		if (index % LEX_INCREASE_CAP == 0)
		{
			void* check = realloc(lexemes,
			                      (index + LEX_INCREASE_CAP)
			                      * sizeof *lexemes);
			if (!check)
			{
				fputs("Cannot allocate memory for lexemes array.\n\n", stderr);
				free(lexemes);
				return NULL;
			}

			lexemes = (lexeme_t*) check;
		}

		skip_spaces(lines, &line, &pos);
		lexemes[index].line = line;
		lexemes[index].pos  = pos;
		if (!get_lexeme(&lexemes[index], lines[line] + pos))
		{
			// The first part of error report was printed in get_lexeme()
			fprintf(stderr, "%s\n", lines[line]);
			free(lexemes);
			return NULL;
		}

		pos += lexemes[index++].length;
	}
	while (*lines[line]);

	return lexemes;
}

/*!
 * @brief Convert number lexeme to token.
 *
 * @note It is assumed that the given lexeme valid.
 *
 * @return A token.
 */
static token_t lexeme_to_number_token
(
	const lexeme_t* lexeme /*!< [in]  lexeme which will be converted.        */
)
{
	token_t token = {.type = TOKEN_NUMBER, .value.number = 0};
	sscanf(lexeme->ptr, "%lf", &token.value.number);
	return token;
}

/*!
 * @brief Convert identifier lexeme to token.
 *
 * @note It is assumed that the given lexeme valid.
 *
 * @return A token.
 */
static token_t lexeme_to_ident_token
(
	const lexeme_t* lexeme /*!< [in]  lexeme which will be converted.        */
)
{
	token_t token;
	token.type        = TOKEN_VAR;
	token.value.ident = (char*) calloc(lexeme->length + 1,
	                                   sizeof *token.value.ident);
	if (token.value.ident)
	{
		strncpy(token.value.ident, lexeme->ptr, lexeme->length);
	}
	else
	{
		token.type = TOKEN_UNKNOWN;
		fputs("Cannot allocate memory for ident token.\n\n", stderr);
	}

	return token;
}

/*!
 * @brief Convert operand lexeme to token.
 *
 * @note It is assumed that the given lexeme valid.
 *
 * @return A token.
 */
static token_t lexeme_to_op_token
(
	const lexeme_t* lexeme /*!< [in]  lexeme which will be converted.        */
)
{
	token_t token;
	token.type = TOKEN_UNKNOWN;
	for (size_t i = 0; i < LEXEME_OP_AMOUNT; ++i)
	{
		if (!strncmp(LEX_TOK_MATCHING[i].lexeme, lexeme->ptr, lexeme->length))
		{
			token.type            = TOKEN_OP;
			token.value.operation = LEX_TOK_MATCHING[i].token;
			return token;
		}
	}

	return token;
}

/*!
 * @brief Convert lexeme to token.
 *
 * @return A token.
 */
static token_t lexeme_to_token
(
	const lexeme_t* lexeme /*!< [in] lexeme which will be converted.         */
)
{
	assert (lexeme);

	switch (lexeme->type)
	{
		case LEXEME_NUMBER:
			return lexeme_to_number_token(lexeme);

		case LEXEME_IDENT:
			return lexeme_to_ident_token(lexeme);

		case LEXEME_OP:
			return lexeme_to_op_token(lexeme);

		case LEXEME_EOF:
		case LEXEME_LPAREN:
		case LEXEME_RPAREN:
		default:
			return (token_t) {.type = TOKEN_UNKNOWN, .value.ident = NULL};
	}
}

/*
 * @brief Get priority of given lexeme.
 *
 * @return Priority of lexeme or -1 if given lexeme is not an operation.
 */
static int get_priority
(
	const lexeme_t* lexeme /*!< [in] preccossed lexeme.                      */
)
{
	if (lexeme->type != LEXEME_OP)
		return -1;

	for (int prior = 0; prior < PARSER_MAX_PRIOR; ++prior)
	{
		for (int i = 0; OP_PRIORS[prior][i][0]; ++i)
		{
			if (lexeme_equals(lexeme, OP_PRIORS[prior][i]))
				return prior;
		}
	}

	return -1;
}

/*!
 * @brief Parse number.
 *
 * @return Parsed tree.
 */
static bintree_t parse_number
(
	parser_t* parser /*!< [in,out] parser state.                             */
)
{
	size_t          pos    = parser_get_pos(parser);
	const lexeme_t* lexeme = parser_next(parser);
	if (lexeme->type == LEXEME_NUMBER)
	{
		bintree_t root = bintree_create(lexeme_to_token(lexeme));
		if (root)
			return root;

		fputs("Cannot allocate memory for number node.\n\n", stderr);
	}

	parser_restore(parser, pos);
	return NULL;
}

/*!
 * @brief Parse identifier.
 *
 * @return Parsed tree.
 */
static bintree_t parse_ident
(
	parser_t* parser /*!< [in,out] parser state.                             */
)
{
	size_t          pos    = parser_get_pos(parser);
	const lexeme_t* lexeme = parser_next(parser);
	if (lexeme->type == LEXEME_IDENT)
	{
		token_t token = lexeme_to_token(lexeme);
		if (token.value.ident)
		{
			bintree_t root = bintree_create_by_moving(token);
			if (root)
				return root;
		}

		token_destroy(&token);
		fputs("Cannot allocate memory for ident node.\n\n", stderr);
	}

	parser_restore(parser, pos);
	return NULL;
}

/*!
 * @brief Parse function.
 *
 * function ::= ident '(' expression ')'
 *
 * @return Parsed tree.
 */
static bintree_t parse_function
(
	parser_t* parser /*!< [in,out] parsing state.                            */
)
{
	size_t    pos    = parser_get_pos(parser);
	bintree_t func   = parse_ident(parser);
	if (func && lexeme_equals(parser_next(parser), LEX_LPAREN))
	{
		func->value.type = TOKEN_FUNC;
		bintree_t arg = parse_expr_of_prior(parser, 0);
		if (arg)
		{
			if (lexeme_equals(parser_next(parser), LEX_RPAREN))
			{
				bintree_t root = create_func_node(func, arg);
				if (root)
					return root;
			}

			parser_error(parser, "Expected ')'");
		}

		bintree_destroy(arg);
	}

	bintree_destroy(func);
	parser_restore(parser, pos);
	return NULL;
}

/*!
 * @brief Parse factor.
 *
 * factor ::= {'(' expr_0 ')'} | function | ident | number
 *
 * @return Parsed tree.
 */
bintree_t parse_factor
(
	parser_t* parser /*!< [in,out] parser state.                             */
)
{
	size_t          pos    = parser_get_pos(parser);
	const lexeme_t* lexeme = parser_next(parser);
	if (lexeme_equals(lexeme, LEX_LPAREN))
	{
		bintree_t root = parse_expr_of_prior(parser, 0);
		if (root)
		{
			lexeme = parser_next(parser);
			if (lexeme_equals(lexeme, LEX_RPAREN))
				return root;

			parser_error(parser, "Expected ')'");
		}

		bintree_destroy(root);
		parser_restore(parser, pos);
		return NULL;
	}

	parser_restore(parser, pos);
	bintree_t root = parse_function(parser);
	if (root)
		return root;

	parser_restore(parser, pos);
	root = parse_ident(parser);
	if (root)
		return root;

	parser_restore(parser, pos);
	root = parse_number(parser);
	if (root)
		return root;

	parser_error(parser, "Invalid sequence %.*s",
	             (int) lexeme->length, lexeme->ptr);
	parser_restore(parser, pos);
	return NULL;
}

/*!
 * @brief Parse term.
 *
 * term ::= ['+' '-']? factor
 *
 * @return Parsed tree.
 */
static bintree_t parse_term
(
	parser_t* parser /*!< [in,out] parsing state.                            */
)
{
	size_t          pos        = parser_get_pos(parser);
	const lexeme_t* lexeme     = parser_next(parser);
	bool            has_prefix = false;
	if (lexeme_equals(lexeme, LEX_PLUS)
	    || lexeme_equals(lexeme, LEX_MINUS))
		has_prefix = true;
	else
		parser_restore(parser, pos);

	bintree_t root = parse_factor(parser);
	if (has_prefix)
		root = create_prefunop_node(root, lexeme_to_token(lexeme));

	if (!root)
		parser_restore(parser, pos);

	return root;
}

/*!
 * @brief Parse expr_<prior> where <prior> is the priority of operation
 * and move string pointer.
 *
 * expr_0 ::= expr_1 {['+' '-'] expr_1}*
 * expr_1 ::= expr_2 {['*' '/'] expr_2}*
 * expr_2 ::= term {'^' term}*
 *
 * @return Parsed tree.
 */
static bintree_t parse_expr_of_prior
(
	parser_t* parser, /*!< [in,out] parser state.                            */
	int       prior   /*!< [in]     priority of current parsed operations.   */
)
{
	if (prior >= PARSER_MAX_PRIOR)
		return parse_term(parser);

	bintree_t root = parse_expr_of_prior(parser, prior + 1);
	if (!root)
		return NULL;

	size_t          pos    = parser_get_pos(parser);
	const lexeme_t* lexeme;
	while (get_priority(lexeme = parser_next(parser)) == prior)
	{
		root = create_binop_node(root, parse_expr_of_prior(parser, prior + 1),
		                         lexeme_to_token(lexeme));
		if (!root)
		{
			parser_restore(parser, pos);
			return NULL;
		}

		pos = parser_get_pos(parser);
	}

	parser_restore(parser, pos);
	return root;
}




bintree_t parse_expr (parser_t* parser)
{
	assert (parser);

	bintree_t root = parse_expr_of_prior(parser, 0);
	if (!root)
		return NULL;

	if (lexeme_equals(parser_next(parser), LEX_EOF))
		return root;

	parser_error(parser, "Extra lexeme at the end.");
	return bintree_destroy(root);
}


bool parser_init (parser_t* parser, const char* str)
{
	assert (parser);
	assert (str);

	parser->pos   = 0;
	parser->lines = (const char**) split_by_delim(str, '\n');
	if (!parser->lines)
	{
		fputs("Cannot create list of strings.\n\n", stderr);
		return false;
	}

	parser->lexemes = split_to_lexemes(parser->lines);
	if (!parser->lexemes)
	{
		free(parser->lines);
		return false;
	}

	return true;
}


void parser_deinit (parser_t* parser)
{
	assert (parser);

	free(parser->lines);
	free(parser->lexemes);
	parser->lines   = NULL;
	parser->lexemes = NULL;
	parser->pos     = 0;
}


const lexeme_t* parser_next (parser_t* parser)
{
	assert (parser);
	
	lexeme_t* ret = &parser->lexemes[parser->pos];
	if (ret->type != LEXEME_EOF)
		++parser->pos;

	return ret;
}


const lexeme_t* parser_restore (parser_t* parser, size_t pos)
{
	assert (parser);

	parser->pos = (pos <= parser->pos) ? pos : parser->pos;
	return &parser->lexemes[parser->pos];
}


size_t parser_get_pos (const parser_t* parser)
{
	assert (parser);

	return parser->pos;
}


bool lexeme_equals (const lexeme_t* lexeme, const char* str)
{
	assert (lexeme);
	assert (str);

	return !strncmp(lexeme->ptr, str, lexeme->length) && !str[lexeme->length];
}

bool isident (char ch)
{
	return isalnum(ch) || (!isspace(ch) && !iskey(ch));
}


bool iskey (char ch)
{
	return strchr(KEY_CHARS, ch);
}
