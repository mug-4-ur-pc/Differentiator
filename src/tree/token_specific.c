/*!
 * @file
 * @brief An implementaion of additional functions that are specific for token.
 */

#include "token_specific.h"
#include "bintree.h"




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
