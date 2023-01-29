/*!
 * @file
 * @brief File which includes optimization functions implementation.
 */

#include "optimization.h"
#include "../tree/token_specific.h"
#include "../dsl/dsl.h"
#include "../utilities/utilities.h"
#include "../tex/tex.h"

#include <assert.h>
#include <string.h>
#include <math.h>




/*!
 * @brief Use pre-calculation optimization.
 *
 * @return True if optimization was used else false.
 */
static bool precalc_optimization
(
	bintree_t expression /*!< [in,out] tree for optimization.                */
)
{
	if (!D_NODE)
		return false;

	bool ret = false;
	ret |= precalc_optimization(D_LHS);
	ret |= precalc_optimization(D_RHS);
	if (D_TYPE == TOKEN_OP)
	{
		if (D_ISPREFUNARY)
		{
			switch (D_OP)
			{
				case OP_PLUS:
					D_NODE = bintree_replace(D_NODE, D_PREFARG);
					return true;

				case OP_MINUS:
					if (D_PREFARG_TYPE == TOKEN_NUMBER)
					{
						D_CHANGE_TO_NUMBER(D_NODE, -D_PREFARG_NUMBER);
						return true;
					}
					else if (D_PREFARG_TYPE == TOKEN_OP
					         && D_PREFARG_OP == OP_PLUS)
					{
						bintree_replace(D_NODE, D_PREFARG);
						D_OP = OP_MINUS;
						return true;
					}
					else if (D_PREFARG_TYPE == TOKEN_OP
					         && D_PREFARG_OP == OP_MINUS)
					{
						bintree_replace(D_NODE, D_PREFARG);
						D_OP = OP_PLUS;
						return true;
					}

					return ret;

				case OP_EMPTY:
				case OP_MUL:
				case OP_DIV:
				case OP_POW:
				case OP_DERIV:
				default:
					assert ("UNREACHABLE" && false);
					return ret;
			}
		}

		switch (D_OP)
		{
			case OP_EMPTY:
			case OP_DERIV:
				break;

			case OP_PLUS:
				if (D_LHS_TYPE == TOKEN_NUMBER
				    && double_equal(D_LHS_NUMBER, 0))
				{
					D_NODE = bintree_replace(D_NODE, D_RHS);
					return true;
				}
				else if (D_RHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_RHS_NUMBER, 0))
				{
					D_NODE = bintree_replace(D_NODE, D_LHS);
					return true;
				}

				break;

			case OP_MINUS:
				if (D_LHS_TYPE == TOKEN_NUMBER
				    && double_equal(D_LHS_NUMBER, 0))
				{
					D_LHS = bintree_replace(D_LHS, D_RHS);
					return true;
				}
				else if (D_RHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_RHS_NUMBER, 0))
				{
					D_NODE = bintree_replace(D_NODE, D_LHS);
					return true;
				}
				else if (bintree_equal(D_LHS, D_RHS))
				{
					D_CHANGE_TO_NUMBER(D_NODE, 0);
					return true;
				}

				break;
			
			case OP_MUL:
				if (D_LHS_TYPE == TOKEN_NUMBER
				    && double_equal(D_LHS_NUMBER, 1))
				{
					D_NODE = bintree_replace(D_NODE, D_RHS);
					return true;
				}
				else if (D_RHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_RHS_NUMBER, 1))
				{
					D_NODE = bintree_replace(D_NODE, D_LHS);
					return true;
				}
				else if ((D_LHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_LHS_NUMBER, 0))
				         || (D_RHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_RHS_NUMBER, 0)))
				{
					D_CHANGE_TO_NUMBER(D_NODE, 0);
					return true;
				}

				break;

			case OP_DIV:
				if (D_LHS_TYPE == TOKEN_NUMBER
				    && double_equal(D_LHS_NUMBER, 0))
				{
					D_CHANGE_TO_NUMBER(D_NODE, 0);
					return true;
				}
				else if (D_RHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_RHS_NUMBER, 1))
				{
					D_NODE = bintree_replace(D_NODE, D_LHS);
					return true;
				}
				else if (bintree_equal(D_LHS, D_RHS))
				{
					D_CHANGE_TO_NUMBER(D_NODE, 1);
					return true;
				}

				break;

			case OP_POW:
				if (D_LHS_TYPE == TOKEN_NUMBER
				    && double_equal(D_LHS_NUMBER, 0))
				{
					D_CHANGE_TO_NUMBER(D_NODE, 0);
					return true;
				}
				else if (D_LHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_LHS_NUMBER, 1))
				{
					D_CHANGE_TO_NUMBER(D_NODE, 1);
					return true;
				}
				else if (D_RHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_RHS_NUMBER, 0))
				{
					D_CHANGE_TO_NUMBER(D_NODE, 1);
					return true;
				}
				else if (D_RHS_TYPE == TOKEN_NUMBER
				         && double_equal(D_RHS_NUMBER, 1))
				{
					D_NODE = bintree_replace(D_NODE, D_LHS);
					return true;
				}

				break;
			default:
				assert ("UNREACHABLE" && false);
		}
	}
	else if (D_TYPE == TOKEN_FUNC)
	{
		if (!strcmp(D_IDENT, "sin") || !strcmp(D_IDENT, "tg"))
		{
			if (D_ARG_TYPE == TOKEN_NUMBER && double_equal(D_ARG_NUMBER, 0))
			{
				D_CHANGE_TO_NUMBER(D_NODE, 0);
			}
		}
		else if (!strcmp(D_IDENT, "cos"))
		{
			if (D_ARG_TYPE == TOKEN_NUMBER && double_equal(D_ARG_NUMBER, 0))
			{
				D_CHANGE_TO_NUMBER(D_NODE, 1);
			}
		}
		else if (!strcmp(D_IDENT, "ln"))
		{
			if (D_ARG_TYPE == TOKEN_NUMBER && double_equal(D_ARG_NUMBER, 1))
			{
				D_CHANGE_TO_NUMBER(D_NODE, 0);
			}
			else if (D_ARG_TYPE == TOKEN_VAR && !strcmp(D_ARG_IDENT, "e"))
			{
				D_CHANGE_TO_NUMBER(D_NODE, 1);
			}
		}
	}

	return ret;
}

/*!
 * @brief Use folding constants optimization.
 *
 * @return True if optimization was used else false.
 */
static bool fold_const_optimization
(
	bintree_t expression /*!< [in,out] tree for optimization.                */
)
{
	if (!D_NODE)
		return false;

	bool ret = false;
	ret |= fold_const_optimization(D_LHS);
	ret |= fold_const_optimization(D_RHS);
	if (D_TYPE != TOKEN_OP || !D_ISBINOP)
		return ret;

	if (D_LHS_TYPE == TOKEN_NUMBER && D_RHS_TYPE == TOKEN_NUMBER)
	{
		double val = 0;
		switch (D_OP)
		{
			case OP_EMPTY:
			case OP_DERIV:
				break;

			case OP_PLUS:
				val = D_LHS_NUMBER + D_RHS_NUMBER;
				break;

			case OP_MINUS:
				val = D_LHS_NUMBER - D_RHS_NUMBER;
				break;

			case OP_MUL:
				val = D_LHS_NUMBER * D_RHS_NUMBER;
				break;

			case OP_DIV:
				val = D_LHS_NUMBER / D_RHS_NUMBER;
				break;

			case OP_POW:
				val = pow(D_LHS_NUMBER, D_RHS_NUMBER);
				break;

			default:
				assert ("UNREACHABLE" && false);
		}

		D_CHANGE_TO_NUMBER(D_NODE, val);
		return true;
	}

	return ret;
}




bintree_t tree_optimize (bintree_t root)
{
	assert (root);

	bool has_optimization = false;
	do
	{
		has_optimization = false;
		has_optimization |= fold_const_optimization(root);
		has_optimization |= precalc_optimization(root);
	}
	while (has_optimization);

	return root;
}


bool tree_is_constant (const bintree_t expression)
{
	if (!D_NODE)
		return true;

	bool ret  = D_TYPE != TOKEN_VAR || strcmp(D_IDENT, "x");
	ret      &= tree_is_constant(D_LHS);
	ret      &= tree_is_constant(D_RHS);

	return ret;
}
