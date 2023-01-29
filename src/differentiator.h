/*!
 * @file
 * @brief A header file with differentiator functions.
 */

#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_


#include "tree/bintree.h"
#include "tree/token_specific.h"



/*!
 * @brief Differentiate expression and writing it to the tex file.
 *
 * @return Derivative. If an error has been occurred it returns NULL.
 */
bintree_t differentiate
(
	 const bintree_t root, /*!< [in]     input expression.                   */
	 FILE*           tex   /*!< [in,out] output tex file.                    */
);




#endif // not defined DIFFERENTIATOR_H_
