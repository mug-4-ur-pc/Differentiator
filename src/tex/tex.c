/*!
 * @file
 * @brief FIle with implementation of the functions which
 * works with .tex file.
 */

#include "phrases.h"
#include "../dsl/dsl.h"
#include "../tree/token_specific.h"
#include "../optimization/optimization.h"
#include "../parser/lexemes.h"
#include "../utilities/utilities.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>




/*!
 * @brief Check given token to be a special char.
 *
 * @return Checking result.
 */
static bool spec_word
(
	const char* str /*!< [in] given identifier name.                         */
)
{
	return !strcmp(str, "pi") || !strcmp(str, "e");
}

/*!
 * @brief Get priority number of given operation.
 *
 * @return Priority number.
 */
static int op_prior
(
	operation_t op /*!< [in] an operation.                                   */
)
{
	switch (op)
	{
		case OP_EMPTY: return -1;
		case OP_PLUS:  return 0;
		case OP_MINUS: return 0;
		case OP_MUL:   return 1;
		case OP_DIV:   return 1;
		case OP_POW:   return 2;
		case OP_DERIV: return 3;
	}

	return -1;
}

/*!
 * @brief Print an expression in tex format.
 */
static void print_expr
(
	const bintree_t expression, /*!< [in,out] expression which will
	                                          be printed.                    */
	int             curr_prior, /*!< [in]    priority of current operation.  */
	FILE*           output      /*!< [in,out] output stream.                 */
)
{
	if (D_TYPE == TOKEN_NUMBER)
	{
		if (D_NUMBER < 0 && curr_prior != -1)
				fputs("( ", output);

		fprintf(output, "%.2lf ",
		        double_equal(D_NUMBER, 0) ? 0 : D_NUMBER);
		if (D_NUMBER < 0 && curr_prior != -1)
				fputs(") ", output);
		return;
	}

	if (D_TYPE == TOKEN_VAR)
	{
		if (spec_word(D_IDENT))
			fputc('\\', output);

		fprintf(output, "%s ", D_IDENT);
		return;
	}

	if (D_TYPE == TOKEN_FUNC)
	{
		fprintf(output, "\\operatorname{%s}(", D_IDENT);
		print_expr(D_ARG, -1, output);
		fputs(") ", output);
		return;
	}

	if (D_TYPE == TOKEN_OP)
	{
		int prior = op_prior(D_OP);
		if (D_ISPREFUNARY)
		{
			if (curr_prior != -1)
				fputs("( ", output);

			switch (D_OP)
			{
				case OP_PLUS:
					fputc('+', output);
					break;

				case OP_MINUS:
					fputc('-', output);
					break;

				default:
					break;
			}

			print_expr(D_PREFARG, PARSER_MAX_PRIOR, output);
			if (curr_prior != -1)
				fputs(") ", output);

			return;
		}

		if (D_ISPOSTUNARY)
		{
			print_expr(D_POSTARG, PARSER_MAX_PRIOR, output);
			switch (D_OP)
			{
				case OP_DERIV:
					fputc('\'', output);
					break;

				default:
					break;
			}

			return;
		}

		if (prior < curr_prior)
			fputc('(', output);
		
		switch (D_OP)
		{
			case OP_PLUS:
				print_expr(D_LHS, prior, output);
				fputs("+ ", output);
				print_expr(D_RHS, prior, output);
				break;

			case OP_MINUS:
				print_expr(D_LHS, prior, output);
				fputs("- ", output);
				print_expr(D_RHS, prior, output);
				break;

			case OP_MUL:
				print_expr(D_LHS, prior, output);
				fputs("\\cdot ", output);
				print_expr(D_RHS, prior, output);
				break;

			case OP_DIV:
				fputs("\\frac{", output);
				print_expr(D_LHS, -1, output);
				fputs("}{", output);
				print_expr(D_RHS, -1, output);
				fputs("} ", output);
				break;

			case OP_POW:
				print_expr(D_LHS, prior + 1, output);
				fputs("^{", output);
				print_expr(D_RHS, -1, output);
				fputs("} ", output);
				break;

			default:
				break;
		}

		if (prior < curr_prior)
			fputc(')', output);
	}
}




FILE* start_article (const bintree_t expression, size_t max_deriv)
{
	assert (expression);

	FILE* tex = fopen("Taylor.tex", "w");
	if (!tex)
	{
		perror("Cannot create tex file");
		return NULL;
	}

	fputs(TEX_PREAMBLE, tex);
	print_expression(expression, tex);
	fprintf(tex, "%s%zd%s", TEX_PREAMBLE_1, max_deriv, tEX_PREAMBLE_2);
	return tex;
}


void finish_article (FILE* tex, const bintree_t* derivatives,
                     size_t max_deriv, double val)
{
	assert (tex);
	assert (derivatives);

	fprintf(tex, "%s%.2lf%s\\begin{dmath*}", TEX_TAYLOR, val, TEX_TAYLOR_1);
	print_expression(derivatives[0], tex);
	fputs(" = ", tex);
	print_expression_at(derivatives[0], val, tex);
	for (size_t i = 1; i <= max_deriv; ++i)
	{
		fputs("+ \\frac{", tex);
		print_expression_at(derivatives[i], val, tex);
		fprintf(tex, "}{%zd!} \\cdot (x - %.2lf)^%zd ", i, val, i);
	}

	fprintf(tex, "+ o((x - %.2lf)^%zd)\\end{dmath*}\n\n", val, max_deriv);
	fputs(TEX_FINAL, tex);
	fclose(tex);
	system("pdflatex -interaction=nonstopmode -halt-on-error Taylor.tex "
	       ">/dev/null 2>/dev/null");
}


void print_expression_at (const bintree_t expr,
                          double substitute, FILE* output)
{
	assert (expr);
	assert (output);

	bintree_t sub = expression_substitute(expr, substitute);
	sub = tree_optimize(sub);
	print_expr(sub, -1, output);
	bintree_destroy(sub);
}


void print_expression (const bintree_t expr, FILE* output)
{
	assert (expr);
	assert (output);

	print_expr(expr, -1, output);
}


void print_context (const tex_context_t context, FILE* output)
{
	size_t size = 0;
	while (TEX_PHRASES[context][size])
		++size;

	size_t random = (size_t) rand() % size;
	fputs(TEX_PHRASES[context][random], output);
}
