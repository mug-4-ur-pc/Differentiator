/*!
 * @file
 * @brief Header of token specific functions for binary tree.
 */

#ifndef BINTREE_TOKEN_SPECIFIC_H_
#define BINTREE_TOKEN_SPECIFIC_H_

#include "bintree.h"



/*!
 * @brief Create node of prefix unary operation.
 *
 * @note Don't forget to free memory.
 *
 * @return Created node. If an error has been occurred it returns NULL.
 */
bintree_t create_prefunop_node
(
	bintree_t operand, /*!< [in] an operand.                                 */
	token_t   op       /*!< [in] token that has operation type.              */
);



/*!
 * @brief Create node of postfix unary operation.
 *
 * @note Don't forget to free memory.
 *
 * @return Created node. If an error has been occurred it returns NULL.
 */
bintree_t create_postunop_node
(
	bintree_t operand, /*!< [in] an operand.                                 */
	token_t   op       /*!< [in] token that has operation type.              */
);



/*!
 * @brief Create node of binary operation.
 *
 * @note Don't forget to free memory.
 *
 * @return Created node. If an error has been occurred it returns NULL.
 */
bintree_t create_binop_node
(
	bintree_t lhs, /*!< [in] left operand.                                   */
	bintree_t rhs, /*!< [in] right opeand.                                   */
	token_t   op   /*!< [in] token that has operation type.                  */
);



/*!
 * @brief Create node of function.
 *
 * @note Don't forget to free memory.
 *
 * @return Created node. If an error has been occurred it returns NULL.
 */
bintree_t create_func_node
(
	bintree_t func, /*!< [in] function name.                                 */
	bintree_t arg   /*!< [in] function's argument.                           */
);

/*!
 * @brief Create node of number type.
 *
 * @return Created node or NULL if an error occurred.
 */
bintree_t create_number
(
	double num /*!< in] given number.                                        */
);

/*!
 * @brief  Find expression value with substitution.
 *
 * @return Expression with substitution.
 */
bintree_t expression_substitute
(
	const bintree_t expr,        /*!< [in] input expression.                 */
	double          substitution /*!< [in] substitution value.               */
);




#endif // not defined BINTREE_TOKEN_SPECIFIC_H_
