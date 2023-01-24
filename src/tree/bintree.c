/*!
 * @file
 * @brief Binary tree implementation.
 */


#include "bintree.h"
#include "../utilities/utilities.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>




/*!
 * @brief Deserialize binary tree recursively.
 *
 * @return Binary tree or NULL if an error occurred during processing this
 * function.
 */
static bintree_t bintree_deserialize_
(
	const char* str,       /*!< [in]     input string.                       */
	size_t*     curr_index /*!< [in,out] index of processing character.      */
)
{
	const char* curr_ptr = strpbrk(str + *curr_index, "{\"}");
	if (!curr_ptr || *curr_ptr != '{')
		return NULL;

	++curr_ptr;
	bintree_t node = (bintree_t) calloc(1, sizeof *node);
	if (!node)
		return NULL;

	*curr_index = curr_ptr - str;
	node->left = bintree_deserialize_(str, curr_index);

	curr_ptr = strchr(str + *curr_index, '\"');
	if (!curr_ptr)
		return bintree_destroy(node);

	char* node_end = (char*) curr_ptr;
	do
	{
		node_end = strchr(node_end + 1, '\"');
		if (!node_end)
			return bintree_destroy(node);
	}
	while (node_end[-1] == '\\');

	*node_end = '\0';
	++curr_ptr;
	size_t node_len = node_end - curr_ptr;
	BINTREE_VALUE_PARSE(node->value, curr_ptr, node_len);
	*node_end   = '\"';
	curr_ptr    = node_end + 1;

	*curr_index = curr_ptr - str;
	node->right = bintree_deserialize_(str, curr_index);
	curr_ptr = strchr(str + *curr_index, '}');
	if (!curr_ptr)
		return bintree_destroy(node);

	*curr_index = curr_ptr - str + 1;

	node->parent = NULL;
	if (node->left)
		node->left->parent = node;
	if (node->right)
		node->right->parent = node;

	return node;
}

/*!
 * @brief Print binary tree.
 */
static void bintree_print_
(
 	const bintree_t node,   /*!< [in]     binary tree.                       */
	FILE*           output, /*!< [in,out] output stream.                     */
	size_t          depth   /*!< [in]     recursion depth.                   */
)
{
	if (!node)
		return;

	print_n_chars('\t', depth, output);
	fputs("{\n", output);

	bintree_print_(node->left, output, depth + 1);

	print_n_chars('\t', depth + 1, output);
	putc('\"', output);
	BINTREE_VALUE_PRINT(node->value, output);
	fputs("\"\n", output);

	bintree_print_(node->right, output, depth + 1);

	print_n_chars('\t', depth, output);
	fputs("}\n", output);
}

/*!
 * @brief Recursively write all binary tree edges.
 *
 * @note If edge equals to NULL segmentation fault will be thrown.
 *
 * @return Amount of nodes in subtree.
 */
static size_t bintree_dump_write_edges
(
	const bintree_t node,      /*!< [in]     current node.                   */
	FILE*           dump,      /*!< [in,out] dump file stream.               */
	size_t          curr_index /*!< [in]     index of current node.          */
)
{
	assert (node);

	// Left size includes current node.
	size_t left_size  = 1;
	size_t right_size = 0;

	fprintf(dump, "\tN%zd [label = \"<NL%zd>|", curr_index, curr_index);
	BINTREE_VALUE_PRINT(node->value, dump);
	fprintf(dump, "|<NR%zd>\"];\n", curr_index);

	if (node->left)
	{
		fprintf(dump, "\tN%zd:<NL%zd> -> N%zd;\n",
			curr_index, curr_index, curr_index + 1);

		left_size += bintree_dump_write_edges(node->left, dump,
		                                      curr_index + 1);
	}

	if (node->right)
	{
		fprintf(dump, "\tN%zd:<NR%zd> -> N%zd;\n",
			curr_index, curr_index, curr_index + left_size);

		right_size = bintree_dump_write_edges(node->right, dump,
		                                      curr_index + left_size);
	}

	return left_size + right_size;
}

/*!
 * @brief Write dump of binary tree to .dot file.
 */
static void bintree_write_dump_to_dot
(
 	const bintree_t head,      /*!< [in]     binary tree.                    */
	FILE*           dump,      /*!< [in,out] dump file stream.               */
	const char*     tree_name, /*!< [in]     name of the binary tree,        */
	size_t          line,      /*!< [in]     line where dump function was
	                                         called.                         */
	const char*     func,      /*!< [in]     function where the dump function
	                                         was called.                     */
	const char*     file       /*!< [in]     file where dump function
	                                         was called.                     */
)
{
	fprintf(dump, "digraph G {\n"
		"\tbgcolor = \"darkblue\";\n"
		"\tnode [color = \"yellow\", fontcolor = \"indigo\","
			"style = \"filled\", shape = \"record\"];\n"
		"\tedge [color = \"blue\"];\n"
		"\tfontcolor = \"white\";\n"
		"\tlabel = \"Binary tree dump:\\n%s from %zd:%s:%s\";\n\n",
		tree_name, line, func, file);

	if (head)
		bintree_dump_write_edges(head, dump, 0);
	
	fputc('}', dump);
}

/*!
 * @brief Recursively find an element in binary tree.
 *
 * @return Node with given element
 * or NULL if tree doesn't contain given element.
 */
static bintree_t bintree_find_
(
	const bintree_t node, /*!< [in] current node.                            */
	BINTREE_VALUE_T elem  /*!< [in] given element.                           */
)
{
	if (BINTREE_VALUE_EQUAL(node->value, elem))
		return node;

	bintree_t ret = NULL;
	if (node->left && (ret = bintree_find_(node->left, elem)))
		return ret;

	if (node->right && (ret = bintree_find_(node->right, elem)))
		return ret;

	return NULL;
}




bintree_t bintree_create (const BINTREE_VALUE_T value)
{
	bintree_t node = (bintree_t) calloc(1, sizeof *node);
	if (node)
	{
		node->left   = NULL;
		node->right  = NULL;
		node->parent = NULL;
		BINTREE_VALUE_COPY(node->value, value);
	}

	return node;
}


bintree_t bintree_create_by_moving (BINTREE_VALUE_T value)
{
	bintree_t node = (bintree_t) calloc(1, sizeof *node);
	if (node)
	{
		node->left   = NULL;
		node->right  = NULL;
		node->parent = NULL;
		BINTREE_VALUE_MOVE(node->value, value);
	}

	return node;
}


bintree_t bintree_destroy (bintree_t head)
{
	if (!head)
		return NULL;

	BINTREE_VALUE_DESTROY(head->value);
	bintree_destroy(head->left);
	bintree_destroy(head->right);
	free(head);

	return NULL;
}


bintree_t bintree_add_left (bintree_t node, BINTREE_VALUE_T value)
{
	assert (node);

	return bintree_hook_left(node, bintree_create(value));
}


bintree_t bintree_hook_left (bintree_t node, bintree_t for_hooking)
{
	assert (node);
	assert (for_hooking);

	bintree_t for_hooking_parent = for_hooking->parent;
	if (for_hooking_parent)
	{
		if (for_hooking_parent->left == for_hooking)
			for_hooking_parent->left = NULL;
		else
			for_hooking_parent->right = NULL;
	}

	bintree_destroy(node->left);
	node->left = for_hooking;
	if (for_hooking)
		for_hooking->parent = node;

	return node->left;

}



bintree_t bintree_unhook_left (bintree_t node)
{
	assert (node);

	bintree_t left = node->left;
	node->left     = NULL;
	if (left)
		left->parent = NULL;

	return left;
}


bintree_t bintree_add_right (bintree_t node, BINTREE_VALUE_T value)
{
	assert (node);

	return bintree_hook_right(node, bintree_create(value));
}


bintree_t bintree_hook_right (bintree_t node, bintree_t for_hooking)
{
	assert (node);
	assert (for_hooking);

	bintree_t for_hooking_parent = for_hooking->parent;
	if (for_hooking_parent)
	{
		if (for_hooking_parent->left == for_hooking)
			for_hooking_parent->left = NULL;
		else
			for_hooking_parent->right = NULL;
	}

	bintree_destroy(node->right);
	node->right = for_hooking;
	if (for_hooking)
		for_hooking->parent = node;

	return node->right;

}


bintree_t bintree_unhook_right (bintree_t node)
{
	assert (node);

	bintree_t right = node->right;
	node->right     = NULL;
	if (right)
		right->parent = NULL;

	return right;
}


void bintree_print (const bintree_t head, FILE* output)
{
	assert (output);

	bintree_print_(head, output, 0);
}


void bintree_dump_func_ (const bintree_t head, const char* bintree_name,
                         size_t line, const char* func, const char* file)
{
	assert (bintree_name);
	assert (func);
	assert (file);

	char dump_fname[BINTREE_MAX_FNAME] = {0};
	sprintf(dump_fname, "%s_%zd_%s_%s.dot",
	        bintree_name, line, func, file);

	FILE* dump = fopen(dump_fname, "w");
	if (!dump)
		perror("Binary tree dump failed");
	
	bintree_write_dump_to_dot(head, dump, bintree_name, line, func, file);
	fclose(dump);

	char command[BINTREE_MAX_FNAME * 2] = {0};
	sprintf(command, "dot %s_%zd_%s_%s.dot -Tpng -o %s_%zd_%s_%s.png",
	        bintree_name, line, func, file,
	        bintree_name, line, func, file);

	system(command);
}


bintree_t bintree_read (FILE* input)
{
	assert (input);

	size_t len = 0;
	char* data = read_string(input, &len);
	if (!data)
		return NULL;

	bintree_t head = bintree_deserialize(data);
	free(data);

	return head;
}


bintree_t bintree_deserialize (const char* str)
{
	assert (str);

	size_t index = 0;
	return bintree_deserialize_(str, &index);
}


bintree_t bintree_find (const bintree_t head, BINTREE_VALUE_T elem)
{
	assert (head);

	return bintree_find_(head, elem);
}


bintree_direction_t* bintree_get_way (const bintree_t node, size_t* len)
{
	assert (node);
	assert (len);

	size_t length = bintree_get_height(node);
	if (length == 1)
	{
		*len = 0;
		return NULL;
	}

	bintree_direction_t* way = (bintree_direction_t*)
	                           calloc(length--, sizeof *way);
	if (!way)
		return NULL;

	*len = length;
	bintree_t curr_node = node;
	for (size_t i = 1; i <= length; ++i)
	{
		way[length - i] = (curr_node->parent->right == curr_node)
		                  ? BINTREE_RIGHT : BINTREE_LEFT;
		curr_node = curr_node->parent;
	}

	return way;
}


size_t bintree_get_height (const bintree_t node)
{
	size_t height = 0;
	for (bintree_t curr_node = node; curr_node; curr_node = curr_node->parent)
		++height;

	return height;
}


bintree_t bintree_next (const bintree_t node, bintree_direction_t dir)
{
	assert (node);
	
	switch (dir)
	{
		case BINTREE_STAY:  return node;
		case BINTREE_LEFT:  return node->left;
		case BINTREE_RIGHT: return node->right;
		default:            return NULL;
	}
}


bintree_t bintree_insert_left (bintree_t* root,
                               bintree_t where, bintree_t node)
{
	assert (where);
	assert (node);

	bintree_t parent = where->parent;
	bintree_destroy(node->left);
	bintree_destroy(node->parent);
	node->left    = where;
	node->parent  = parent;
	where->parent = node;
	if (!parent)
	{
		*root = node;
		return node;
	}

	if (parent->left == where)
		parent->left = node;
	else
		parent->right = node;

	return node;
}


bintree_t bintree_insert_right (bintree_t* root,
                                bintree_t where, bintree_t node)
{
	assert (where);
	assert (node);

	bintree_t parent = where->parent;
	bintree_destroy(node->right);
	bintree_destroy(node->parent);
	node->right   = where;
	node->parent  = parent;
	where->parent = node;
	if (!parent)
	{
		*root = node;
		return node;
	}

	if (parent->left == where)
		parent->left = node;
	else
		parent->right = node;

	return node;
}


