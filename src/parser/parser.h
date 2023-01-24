/*!
 * @file
 * @brief Header file of parser.
 */

#ifndef PARSER_H_
#define PARSER_H_



#include "../tree/bintree.h"



/*!
 * @brief Enum with possible types of the lexeme.
 */
typedef enum
{
	LEXEME_NUMBER  = 0,
	LEXEME_IDENT   = 1,
	LEXEME_LPAREN  = 2,
	LEXEME_RPAREN  = 3,
	LEXEME_OP      = 4,
	LEXEME_EOF     = 5,
	LEXEME_UNKNOWN = 6,
}
lexeme_type_t;

/*!
 * @brief Type of the parsing lexeme.
 */
typedef struct
{
	lexeme_type_t type;   /*!< type of the lexeme.                           */
	const char*   ptr;    /*!< pointer to the beginning of the lexeme.       */
	unsigned      length; /*!< length of the lexeme..                        */
	unsigned      line;   /*!< number of line where lexeme locates.          */
	unsigned      pos;    /*!< position in line where lexeme locates.        */
}
lexeme_t;

/*!
 * @brief Structure which shows parsing state.
 */
typedef struct
{
	lexeme_t*    lexemes; /*!< array of lexemes                              */
	const char** lines;   /*!< array of lines.                               */
	size_t             pos;     /*!< last parsed lexeme.                     */
}
parser_t;



/*!
 * @brief Parse an expression using recursive descent parsing.
 *
 * expression ::= expr_0 '\0'
 *
 * @return Expression tree or NULL if an error was occurred.
 */
bintree_t parse_expr
(
	parser_t* parser /*!< [in,out] parser state.                             */
);

/*!
 * @brief Initialize parser using input string.
 *
 * @note Don't forget to free memory using parser_deinit().
 *
 * @return Success of initialization.
 */
bool parser_init
(
 	parser_t*   parser, /*!< [out] parser of initialization.                 */
	const char* str     /*!< [in]  initial string.                           */
);

/*!
 * @brief Free memory that parser uses.
 */
void parser_deinit
(
	parser_t* parser /*!< [in,out] parser.                                   */
);

/*!
 * @brief Get next lexeme.
 */
const lexeme_t* parser_next
(
	parser_t* parser /*!< [in,out] parser state.                             */
);

/*!
 * @brief Get prev lexeme.
 */
const lexeme_t* parser_restore
(
	parser_t* parser, /*!< [in,out] parser state.                            */
	size_t    pos     /*!< [in] restored position.                           */
);

/*!
 * @brief Get current lexeme position.
 *
 * @return Lexeme position.
 */
size_t parser_get_pos
(
	const parser_t* parser /*!< [in] parser state.                           */
);

/*!
 * @brief Check lexeme to equality to given string.
 *
 * @return True of lexeme equals to given string else false.
 */
bool lexeme_equals
(
	const lexeme_t* lexeme, /*!< [in] lexeme.                                */
	const char*     str     /*!< [in] given string.                          */
);

/*!
 * @brief Check character to valid identifier symbol.
 *
 * @return Is given character can be in identifier.
 */
bool isident
(
	char ch /*!< [in] character.                                             */
);

/*!
 * @brief Check character to be a key.
 *
 * @return Is given character is key.
 */
bool iskey
(
	char ch /*!< [in] character.                                             */
);

/*!
 * @brief Print syntax error using given format string and lexeme position.
 */
#define parser_error_at(PARSER_, POS_, ...) \
do \
{ \
	const lexeme_t* LEX_ = (PARSER_)->lexemes + (POS_); \
	fprintf(stderr, "%u:%u: ", LEX_->line, LEX_->pos); \
	fprintf(stderr, __VA_ARGS__); \
	fputc('\n', stderr); \
	fputs((PARSER_)->lines[LEX_->line], stderr); \
	fputc('\n', stderr); \
} \
while (false)

/*!
 * @brief Print syntax error using given format string.
 */
#define parser_error(PARSER_, ...) \
	parser_error_at(PARSER_, (PARSER_)->pos, __VA_ARGS__)




#endif // not defined PARSER_H_
