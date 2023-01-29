/*!
 * @file
 * @brief An implementaion of additional functions that are specific for token.
 */

#include "token_specific.h"
#include "bintree.h"

#include <assert.h>
#include <string.h>




/*!
 * @brief Recursively substitute all values.
 *
 * @return Tree with substitution.
 */
static bintree_t expr_substitute
(
	const bintree_t expr,        /*!< [in] input expression.                 */
	double          substitution /*!< [in] substitution value.               */
)
{
	token_t t = expr->value;
	if (t.type == TOKEN_VAR && !strcmp(t.value.ident, "x"))
	{
		t.type         = TOKEN_NUMBER;
		t.value.number = substitution;
	}

	bintree_t root = bintree_create(t);
	if (!root)
		return NULL;

	if (expr->left)
	{
		bintree_hook_left(root, expr_substitute(expr->left, substitution));
		if (!root->left)
			return bintree_destroy(root);
	}

	if (expr->right)
	{
		bintree_hook_right(root, expr_substitute(expr->right, substitution));
		if (!root->right)
			return bintree_destroy(root);
	}

	return root;
}




bintree_t create_binop_node (bintree_t lhs, bintree_t rhs, token_t op)
{
	if (!lhs || !rhs)
		return NULL;

	bintree_t node = bintree_create_by_moving(op);
	if (!node)
		fputs("Cannot allocate memory for operation node.\n\n", stderr);

	if (!node || !lhs || !rhs || op.type == TOKEN_UNKNOWN)
	{
		bintree_destroy(node);
		bintree_destroy(lhs);
		bintree_destroy(rhs);
		token_destroy(&op);
		return NULL;
	}

	bintree_hook_left(node, lhs);
	bintree_hook_right(node, rhs);

	return node;
}


bintree_t create_prefunop_node (bintree_t operand, token_t op)
{
	if (!operand)
		return NULL;

	bintree_t node = bintree_create_by_moving(op);
	if (!node)
		fputs("Cannot allocate memory for node.\n\n", stderr);

	if (!node || !operand || op.type == TOKEN_UNKNOWN)
	{
		bintree_destroy(node);
		bintree_destroy(operand);
		token_destroy(&op);
		return NULL;
	}

	bintree_hook_left(node, operand);

	return node;
}


bintree_t create_postunop_node (bintree_t operand, token_t op)
{
	if (!operand)
		return NULL;

	bintree_t node = bintree_create_by_moving(op);
	if (!node)
		fputs("Cannot allocate memory for operation node.\n\n", stderr);

	if (!node || !operand || op.type == TOKEN_UNKNOWN)
	{
		bintree_destroy(node);
		bintree_destroy(operand);
		token_destroy(&op);
		return NULL;
	}

	bintree_hook_right(node, operand);

	return node;
}


bintree_t create_func_node (bintree_t func, bintree_t arg)
{
	if (!func || !arg)
	{
		bintree_destroy(func);
		bintree_destroy(arg);
		return NULL;
	}

	bintree_hook_right(func, arg);
	return func;
}


bintree_t create_number (double num)
{
	token_t token = {.type = TOKEN_NUMBER, .value.number = num};
	return bintree_create_by_moving(token);
}


bintree_t expression_substitute (const bintree_t expr, double substitution)
{
	assert (expr);

	return expr_substitute(expr, substitution);
}
