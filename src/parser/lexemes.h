/*!
 * @file
 * @brief File which contains lexemes definitions.
 */

#ifdef LEXEMES_H_
#error lexemes.h can be included only one time.
#endif
#define LEXEMES_H_

#include "parser.h"




#define LEX_EOF    ""
#define LEX_PLUS   "+"
#define LEX_MINUS  "-"
#define LEX_MUL    "*"
#define LEX_DIV    "/"
#define LEX_POW    "^"
#define LEX_LPAREN "("
#define LEX_RPAREN ")"

/*!
 * @brief List of key characters.
 */
static const char* const KEY_CHARS = "+-*/^()";

/*!
 * @brief Max priority of the operation.
 *
 * Priorities started from 1.
 */
#define PARSER_MAX_PRIOR ((int) 3)

/*!
 * @brief Priorities of different operations.
 */
static const char OP_PRIORS[PARSER_MAX_PRIOR][3][2] =
{
	{LEX_PLUS, LEX_MINUS, ""},
	{LEX_MUL,  LEX_DIV,   ""},
	{LEX_POW,  "",        ""},
};

static const struct
{
	const char* lexeme;
	operation_t token;
}
LEX_TOK_MATCHING[] =
{
	{LEX_PLUS,  OP_PLUS},
	{LEX_MINUS, OP_MINUS},
	{LEX_MUL,   OP_MUL},
	{LEX_DIV,   OP_DIV},
	{LEX_POW,   OP_POW}
};

/*!
 * @brief Amount of operation lexemes.
 */
static const size_t LEXEME_OP_AMOUNT = 5;
