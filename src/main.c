/*!
 * @file
 * @brief Main file of differentiator.
 */


#include "parser/parser.h"
#include "tex/tex.h"
#include "utilities/utilities.h"
#include "differentiator.h"

#include <stdlib.h>
#include <time.h>


int main (void)
{
	srand(time(NULL));

	size_t len          = 0;
	size_t max_deriv    = 0;
	double substitution = 0;
	if (scanf("%zu %lf", &max_deriv, &substitution) != 2)
	{
		fputs("Input format: "
		      "<max derivative> <substitution value> <expression>\n", stderr);
		return 1;
	}

	char* input = read_string(stdin, &len);
	if (!input)
		return 1;

	parser_t parser;
	if (!parser_init(&parser, input))
		return 1;

	bintree_t derivatives[max_deriv + 1];
	for (size_t i = 0; i <= max_deriv; ++i)
		derivatives[i] = NULL;

	derivatives[0] = parse_expr(&parser);
	if (!derivatives[0])
	{
		parser_deinit(&parser);
		free(input);
		return 1;
	}

	FILE* tex = start_article(derivatives[0], max_deriv);
	if (!tex)
	{
		bintree_destroy(derivatives[0]);
		free(input);
		return 1;
	}

	int ret = 0;
	for (size_t i = 1; i <= max_deriv; ++i)
	{
		derivatives[i] = differentiate(derivatives[i - 1], tex);
		if (!derivatives[i])
		{
			break;
			ret = 1;
		}
	}
	
	if (ret == 0)
		finish_article(tex, derivatives, max_deriv, substitution);
	else
		fclose(tex);

	parser_deinit(&parser);
	free(input);
	
	for (size_t i = 0; i <= max_deriv; ++i)
		derivatives[i] = bintree_destroy(derivatives[i]);
	
	return ret;
}
