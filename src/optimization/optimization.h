/*!
 * @file
 * @brief File which includes definitions of optimization functions.
 */

#ifndef OPTIMIZATIONS_H_
#define OPTIMIZATIONS_H_

#include "../tree/token_specific.h"



/*!
 * @brief Optimize expression tree.
 *
 * @note It changes the original expression.
 *
 * @return Optimized expression or NULL if an error occurred.
 */
bintree_t tree_optimize
(
	bintree_t root /*!< [in,out] tree for optimization.                      */
);

/*!
 * @brief Check tree to variables absence.
 *
 * @return Result of checking.
 */
bool tree_is_constant
(
	const bintree_t tree /*!< [in] given tree.                               */
);




#endif // not defined OPTIMIZATIONS_H_
