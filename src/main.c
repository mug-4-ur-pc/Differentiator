/*!
 * @file
 * @brief Main file of differentiator.
 */


#include "parser/parser.h"
#include "utilities/utilities.h"
#include "differentiator.h"

#include <stdlib.h>


int main (void)
{
	size_t len = 0;
	char* input = read_string(stdin, &len);
	if (!input)
		return 1;

	parser_t parser;
	if (!parser_init(&parser, input))
		return 1;

	bintree_t tree = parse_expr(&parser);
	if (!tree)
	{
		parser_deinit(&parser);
		free(input);
		return 1;
	}
/*
	FILE* tex = start_article(input);
	if (!tex)
	{
		bintree_destroy(tree);
		free(input);
		return 1;
	}

	int ret = 0;
	if (!(tree = differentiate(tree, tex)))
		ret = 1;
	
	fclose(tex);*/
	bintree_dump(tree);
	parser_deinit(&parser);
	bintree_destroy(tree);
	free(input);
	return 0;//ret;
}
