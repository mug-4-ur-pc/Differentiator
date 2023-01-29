/*!
 * @file
 * @brief File which includes definition of the token type.
 */

#ifndef TOKEN_H_
#define TOKEN_H_




#include <stdio.h>
#include <stdbool.h>



/*!
 * @brief Possible types of the token.
 */
typedef enum
{
	TOKEN_NUMBER  = 0, //!< number type.
	TOKEN_VAR     = 1, //!< variable type.
	TOKEN_OP      = 2, //!< operation type.
	TOKEN_FUNC    = 3, //!< function type.
	TOKEN_UNKNOWN = 4, //!< unknown tyoe.
}
token_type_t;

/*!
 * @brief List of operations.
 */
typedef enum
{
	OP_EMPTY = 'E',
	OP_PLUS  = '+',
	OP_MINUS = '-',
	OP_MUL   = '*',
	OP_DIV   = '/',
	OP_POW   = '^',
	OP_DERIV = '`',
}
operation_t;

/*!
 * @brief Possible values of token.
 */
typedef union
{
	double      number;    //!< number field.
	char*       ident;     //!< identifier field.
	operation_t operation; //!< operation field.
}
token_value_t;

/*!
 * @brief Token type.
 */
typedef struct
{
	token_type_t  type;  /*!< type of the token.                             */
	token_value_t value; /*!< value of the token.                            */
}
token_t;



/*!
 * @brief Parse token from string which has format TYPE(VALUE)
 *
 * @return Success of parsing.
 */
bool token_parse
(
	token_t*    token, /*!< [out] token where parsed value will be written.  */
	const char* str,   /*!< [in]  string with serealized token.              */
	size_t      len    /*!< [in]  length of input string.                    */
);

/*!
 * @brief Check tokens to equality.
 *
 * @return true if tokens are wqual else false.
 */
bool token_equal
(
	const token_t* t1, /*!< [in] first token.                                */
	const token_t* t2  /*!< [in] second token.                               */
);

/*!
 * @brief Print token.
 */
void token_print
(
	const token_t* t,     /*!< [in]     token which will be printed.         */
	FILE*         output /*!< [in,out] output stream.                        */
);

/*!
 * @brief Destroy token and free memory.
 */
void token_destroy
(
	token_t* t /*!< [in,out] token.                                          */
);

/*!
 * @brief Copy token.
 *
 * @return If an error occurred it returns false else true.
 */
bool token_copy
(
	token_t*       dest, /*!< [out[ destination token.                       */
	const token_t* src   /*!< [in]  source token.                            */
);

/*!
 * @brief Move token.
 *
 * @note After moving src becomes empty.
 *
 * @return If an error occurred it returns false else true.
 */
bool token_move
(
	token_t* dest, /*!< [out[     destination token.                         */
	token_t* src   /*!< [in,out]  source token.                              */
);




#endif // not defined TOKEN_H_
