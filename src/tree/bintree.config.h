/*!
 * @file
 * @brief File with binary tree configuration.
 *
 * Change it to configure your own binary tree.
 */

#include "../parser/token.h"

/*!
 * @brief Type of binary tree element.
 */
#define BINTREE_VALUE_T token_t

/*!
 * @brief Copy function for binary tree elements.
 */
#define BINTREE_VALUE_COPY(NODE_VALUE_, VALUE_) \
	token_copy(&NODE_VALUE_, &VALUE_)

/*!
 * @brief Move function for binary tree elements.
 */
#define BINTREE_VALUE_MOVE(NODE_VALUE_, VALUE_) \
	token_move(&NODE_VALUE_, &VALUE_)

/*!
 *
 */
#define BINTREE_VALUE_DESTROY(VALUE_) token_destroy(&VALUE_)

/*!
 * @brief Function which prints a tree element.
 */
#define BINTREE_VALUE_PRINT(VALUE_, STREAM_) \
	token_print(&VALUE_, STREAM_)

/*!
 * @brief Function which returns true if elements are equal else false.
 */
#define BINTREE_VALUE_EQUAL(NODE_VALUE_, VALUE_) \
	token_equal(&NODE_VALUE_, &VALUE_)

/*!
 * @brief Function which parses value from input string.
 */
#define BINTREE_VALUE_PARSE(VALUE_, STR_, STRLEN_) \
	token_parse(&VALUE_, STR_, STRLEN_)

/*!
 * @brief Max file name of binary tree dump.
 */
#define BINTREE_MAX_FNAME ((size_t) 8192)
