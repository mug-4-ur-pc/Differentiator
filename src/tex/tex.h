/*!
 * @file
 * @brief File includes function prototypes which work with tex file.
 */

#ifndef TEX_H_
#define TEX_H_

#include "../tree/bintree.h"

#include <stdio.h>



/*!
 * @brief Current context that shows which phrase should use.
 */
typedef int tex_context_t;

/*!
 * @brief List of contest.
 */
enum contexts
{
	CONTEXT_BEGIN_DIFF,
	CONTEXT_BEGIN_DIFF_1,
	CONTEXT_END_DIFF,
	CONTEXT_DIFF_NUMBER,
	CONTEXT_DIFF_VAR,
	CONTEXT_DIFF_FUNC,
	CONTEXT_DIFF_OP,
	CONTEXT_OPTIMIZE,
	TEX_CONTEXTS_AMOUNT
};

/*!
 * @brief Start article about Taylor's series in tex file.
 *
 * @return File with started article or NULL.
 */
FILE* start_article
(
	const bintree_t expression, /*!< [in] an initial expression.              */
	size_t          max_deriv   /*!< [in] maximal derivative.                */
);

/*!
 *
 */
void finish_article
(
	FILE*            tex,         /*!< [in,out] tex file stream.             */
	const bintree_t* derivatives, /*!< [in]     array with derivatives.      */
	size_t           max_deriv,   /*!< [in]     amount of derivatives.       */
	double           val          /*!< [in]     value for substitution.	     */
);

/*!
 * @brief Print an expression in tex format.
 */
void print_expression_at
(
	const bintree_t expression,   /*!< [in]     expression which will
	                                            be printed.                  */
	double          substitution, /*!< [in]     value for substitution.      */
	FILE*           output        /*!< [in,out] output stream.               */
);

/*!
 * @brief Print an expression in tex format.
 */
void print_expression
(
	const bintree_t expression, /*!< [in]     expression which will
	                                          be printed.                    */
	FILE*           output      /*!< [in,out] output stream.                 */
);

/*!
 * @brief Print random phrase of given context.
 *
 * @note Set random seed using srand() before callong this function.
 */
void print_context
(
	const tex_context_t context, /*!< [in]     given context.                */
	FILE*               output   /*!< [in,out] tex file.                     */
);




#endif // not defined TEX_H_
