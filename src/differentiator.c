/*!
 * @file
 * @brief File witch includes differentiator functions.
 */

#include "differentiator.h"
#include "dsl/dsl.h"
#include "parser/token.h"
#include "tree/bintree.h"
#include "tree/token_specific.h"
#include "tex/tex.h"
#include "optimization/optimization.h"
#include "utilities/utilities.h"

#include <assert.h>
#include <string.h>




static bintree_t differentiate_node   (const bintree_t expression, FILE* tex);
static bintree_t differentiate_number (const bintree_t expression, FILE* tex);
static bintree_t differentiate_var    (const bintree_t expression, FILE* tex);
static bintree_t differentiate_func   (const bintree_t expression, FILE* tex);
static bintree_t differentiate_op     (const bintree_t expression, FILE* tex);



/*!
 * @brief Print one step of finding derivative.
 */
static void print_step
(
	const tex_context_t context,    /*!< [in]     type of differentiation.   */
	const bintree_t     deriv,      /*!< [in]     derivate of
	                                              given expression.          */
	const bintree_t     expression, /*!< [in]     given expression.          */
	FILE*               output      /*!< [in,out] tex file stream.           */
)
{
	print_context(context, output);
	fputs("\n\\begin{dmath*}\n(", output);
	print_expression(expression, output);
	fputs(")' = ", output);
	print_expression(deriv, output);
	fputs(".\n\\end{dmath*}\n", output);
}

/*!
 * @brief Differentiate number.
 *
 * @return Found derivative.
 */
static bintree_t differentiate_number
(
	const bintree_t expression, /*!< [in,out] given expression.              */
	FILE*           output      /*!< [in,out] tex file stream.               */
)
{
	bintree_t root = create_number(0);
	if (!root)
		fputs("Cannot allocate memory for number node.\n\n", stderr);
	else
		print_step(CONTEXT_DIFF_NUMBER, root, expression, output);

	return root;
}

/*!
 * @brief Differentiate variable.
 *
 * @return Found derivative.
 */
static bintree_t differentiate_var
(
	const bintree_t expression, /*!< [in,out] given expression.              */
	FILE*           output      /*!< [in,out] tex file stream.               */
)
{
	bintree_t root = create_number((strcmp(D_IDENT, "x")) ? 0 : 1);
	if (!root)
		fputs("Cannot allocate memory for ident node.\n\n", stderr);
	else
		print_step(CONTEXT_DIFF_VAR, root, expression, output);

	return root;
}

/*!
 * @brief Differentiate function.
 *
 * @return Found derivative.
 */
static bintree_t differentiate_func
(
	const bintree_t expression, /*!< [in,out] given expression.              */
	FILE*           output      /*!< [in,out] tex file stream.               */
)
{
	bintree_t root = NULL;
	bintree_t arg  = bintree_copy(D_ARG);
	if (!arg)
	{
		fputs("Cannot create copy of function argument.\n\n", stderr);
		return NULL;
	}

	token_t t = {.type = TOKEN_FUNC, .value.ident = NULL};
	if (strcmp(D_IDENT, "sin") == 0)
	{
		D_NEW_FUNC(root, "cos", arg);
	}
	else if (strcmp(D_IDENT, "cos") == 0)
	{
		D_NEW_FUNC(root, "sin", arg);
		D_NEW_PREFUNOP(root, OP_MINUS, root);
	}
	else if (strcmp(D_IDENT, "tg") == 0)
	{
		D_NEW_FUNC(root, "cos", arg);
		D_NEW_OP(root, OP_POW, root, create_number(2));
		D_NEW_OP(root, OP_DIV, create_number(1), root);
	}
	else if (strcmp(D_IDENT, "ctg") == 0)
	{
		D_NEW_FUNC(root, "sin", arg);
		D_NEW_OP(root, OP_POW, root, create_number(2));
		D_NEW_PREFUNOP(root, OP_MINUS, root);
		D_NEW_OP(root, OP_DIV, create_number(1), root);
	}
	else if (strcmp(D_IDENT, "ln") == 0)
	{
		D_NEW_OP(root, OP_DIV, create_number(1), arg);
	}
	else
	{
		D_NEW_FUNC(root, D_IDENT, arg);
		D_NEW_POSTUNOP(root, OP_DERIV, root);
	}

	if (!root)
	{
		fputs("Cannot create node.\n\n", stderr);
		return NULL;
	}

	D_NEW_OP(root, OP_MUL, root, differentiate_node(D_ARG, output));
	if (!root)
		fputs("Cannot allocate memory for function node.\n\n", stderr);
	else
		print_step(CONTEXT_DIFF_FUNC, root, expression, output);

	return root;
}

/*!
 * @brief Differentiate operation.
 *
 * @return Found derivative.
 */
static bintree_t differentiate_op
(
	const bintree_t expression, /*!< [in,out] given expression.              */
	FILE*           output      /*!< [in,out] tex file stream.               */
)
{
	bintree_t root = NULL;
	bintree_t tmp1 = NULL;
	bintree_t tmp2 = NULL;
	token_t t;
	if (D_ISPREFUNARY)
	{
		D_NEW_PREFUNOP(root, D_OP, differentiate_node(D_PREFARG, output));
	}
	else if (D_ISPOSTUNARY)
	{
		// There are no another postfix operations except OP_DERIV
		bintree_t arg = D_POSTARG;
		while (arg && arg->value.type == TOKEN_OP
		       && arg->value.value.operation == OP_DERIV)
			arg = arg->right;

		if (!arg)
		{
			fputs("Function hasn't arguments.\n\n", stderr);
			return NULL;
		}

		if (arg->value.type == TOKEN_FUNC && arg->right)
		{
			arg = arg->right;
		}
		else
		{
			fputs("Tree has wrong format.\n\n", stderr);
			return NULL;
		}
		
		D_NEW_POSTUNOP(root, OP_DERIV, bintree_copy(D_POSTARG));
		D_NEW_POSTUNOP(root, OP_DERIV, root);
		D_NEW_OP(root, OP_MUL, root, differentiate_node(arg, output));
	}
	else
	{
		bintree_t arg1 = bintree_copy(D_LHS);
		bintree_t arg2 = bintree_copy(D_RHS);
		if (!arg1 || !arg2)
		{
			bintree_destroy(arg1);
			bintree_destroy(arg2);
			fputs("Cannot copy operation arguments.\n\n", stderr);
			return NULL;
		}

		switch (D_OP)
		{
			case OP_EMPTY:
				break;
	
			case OP_DERIV:
				break;

			case OP_PLUS:
				bintree_destroy(arg1);
				bintree_destroy(arg2);
				D_NEW_OP(root, OP_PLUS, differentiate_node(D_LHS, output),
				         differentiate_node(D_RHS, output));
				break;

			case OP_MINUS:
				bintree_destroy(arg1);
				bintree_destroy(arg2);
				D_NEW_OP(root, OP_MINUS, differentiate_node(D_LHS, output),
				         differentiate_node(D_RHS, output));
				break;

			case OP_MUL:
				D_NEW_OP(tmp1, OP_MUL, arg1, differentiate_node(D_RHS, output));
				D_NEW_OP(tmp2, OP_MUL, differentiate_node(D_LHS, output), arg2);
				D_NEW_OP(root, OP_PLUS, tmp1, tmp2);
				break;

			case OP_DIV:
				D_NEW_OP(tmp1, OP_MUL, differentiate_node(D_LHS, output), arg2);
				D_NEW_OP(tmp2, OP_MUL, arg1, differentiate_node(D_RHS, output));
				D_NEW_OP(tmp1, OP_MINUS, tmp1, tmp2);
				arg2 = bintree_copy(D_RHS);
				D_NEW_OP(tmp2, OP_POW, arg2, create_number(2));
				D_NEW_OP(root, OP_DIV, tmp1, tmp2);
				break;

			case OP_POW:
				if (tree_is_constant(arg2))
				{
					root = tree_optimize(arg2);
					if (!root)
					{
						bintree_destroy(arg1);
						bintree_destroy(arg2);
						fputs("Cannot optimize subtree.\n\n", stderr);
						return NULL;
					}

					if (root->value.type == TOKEN_NUMBER
					    && double_equal(root->value.value.number, 0))
					{
						bintree_destroy(arg1);
						bintree_destroy(root);
						root = create_number(0);
						break;
					}

					D_NEW_OP(arg2, OP_MINUS, root, create_number(1));
					D_NEW_OP(root, OP_POW, arg1, arg2);
					D_NEW_OP(root, OP_MUL, bintree_copy(arg2), root);
					D_NEW_OP(root, OP_MUL, root, differentiate_node(D_LHS, output));
					break;
				}

				D_NEW_OP(root, D_OP, arg1, arg2);
				if (tree_is_constant(arg1))
				{
					D_NEW_FUNC(tmp1, "ln", bintree_copy(D_LHS));
					D_NEW_OP(root, OP_MUL, root, tmp1);
					D_NEW_OP(root, OP_MUL, root,
					         differentiate_node(D_RHS, output));
					break;
				}

				D_NEW_OP(tmp1, OP_MUL, differentiate_node(D_LHS, output),
				         bintree_copy(D_RHS));
				D_NEW_OP(tmp1, OP_DIV, tmp1, bintree_copy(D_LHS));
				D_NEW_FUNC(tmp2, "ln", bintree_copy(D_LHS));
				D_NEW_OP(tmp2, OP_MUL, differentiate_node(D_RHS, output), tmp2);
				D_NEW_OP(tmp1, OP_PLUS, tmp1, tmp2);
				D_NEW_OP(root, OP_MUL, root, tmp1);
				break;
		}
	}

	if (!root)
		fputs("Cannot allocate memory for operation node.\n\n", stderr);
	else
		print_step(CONTEXT_DIFF_OP, root, expression, output);

	return root;
}

/*!
 * @brief Differentiate node.
 *
 * @return Derivative of given node.
 */
static bintree_t differentiate_node
(
	const bintree_t expression, /*!< [in] node of the expression tree.       */
	FILE*           tex         /*!< [in] tex file stream.                   */
)
{
	if (D_TYPE == TOKEN_NUMBER)
		return differentiate_number(expression, tex);

	if (D_TYPE == TOKEN_VAR)
		return differentiate_var(expression, tex);

	if (D_TYPE == TOKEN_FUNC)
		return differentiate_func(expression, tex);

	if (D_TYPE == TOKEN_OP)
		return differentiate_op(expression, tex);

	fputs("Token has unknown type.\n\n", stderr);
	return NULL;
}




bintree_t differentiate (const bintree_t root, FILE* tex)
{
	assert (root);
	assert (tex);

	print_context(CONTEXT_BEGIN_DIFF, tex);
	print_expression(root, tex);
	print_context(CONTEXT_BEGIN_DIFF_1, tex);
	bintree_t deriv = differentiate_node(root, tex);
	if (!deriv)
		return NULL;

	print_context(CONTEXT_OPTIMIZE, tex);
	fputs(" \\begin{dmath*}\n", tex);
	print_expression(deriv, tex);
	fputs(" = ", tex);
	bintree_t ret = tree_optimize(deriv);
	if (!ret)
		return bintree_destroy(deriv);
	
	print_expression(ret, tex);
	fputs(" .\n\\end{dmath*}\n\n", tex);
	print_context(CONTEXT_END_DIFF, tex);
	return ret;
}
