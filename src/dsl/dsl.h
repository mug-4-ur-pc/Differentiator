/*!
 * @file
 * @brief Domain specific language for binary tree of tokens.
 */

#ifndef DSL_H_
#define DSL_H_

#define D_NODE    (expression)
#define D_LHS     (D_NODE->left)
#define D_RHS     (D_NODE->right)
#define D_PREFARG (D_LHS)
#define D_POSTARG (D_RHS)
#define D_ARG     (D_RHS)

#define D_TOKEN         (D_NODE->value)
#define D_LHS_TOKEN     (D_LHS->value)
#define D_RHS_TOKEN     (D_RHS->value)
#define D_PREFARG_TOKEN (D_PREFARG->value)
#define D_POSTARG_TOKEN (D_POSTARG->value)
#define D_ARG_TOKEN     (D_ARG->value)

#define D_TYPE         (D_TOKEN.type)
#define D_LHS_TYPE     (D_LHS_TOKEN.type)
#define D_RHS_TYPE     (D_RHS_TOKEN.type)
#define D_PREFARG_TYPE (D_PREFARG_TOKEN.type)
#define D_POSTARG_TYPE (D_POSTARG_TOKEN.type)
#define D_ARG_TYPE     (D_ARG_TOKEN.type)

#define D_NUMBER         (D_TOKEN.value.number)
#define D_LHS_NUMBER     (D_LHS_TOKEN.value.number)
#define D_RHS_NUMBER     (D_RHS_TOKEN.value.number)
#define D_PREFARG_NUMBER (D_PREFARG_TOKEN.value.number)
#define D_POSTARG_NUMBER (D_POSTARG_TOKEN.value.number)
#define D_ARG_NUMBER     (D_ARG_TOKEN.value.number)

#define D_IDENT         (D_TOKEN.value.ident)
#define D_LHS_IDENT     (D_LHG_TOKEN.value.ident)
#define D_RHS_IDENT     (D_RHS_TOKEN.value.ident)
#define D_PREFARG_IDENT (D_PREFARG_TOKEN.value.ident)
#define D_POSTARG_IDENT (D_POSTARG_TOKEN.value.ident)
#define D_ARG_IDENT     (D_ARG_TOKEN.value.ident)

#define D_OP         (D_TOKEN.value.operation)
#define D_LHS_OP     (D_LHS_TOKEN.value.operation)
#define D_RHS_OP     (D_RHS_TOKEN.value.operation)
#define D_PREFARG_OP (D_PREFARG_TOKEN.value.operation)
#define D_POSTARG_OP (D_POSTARG_TOKEN.value.operation)
#define D_ARG_OP     (D_ARG_TOKEN.value.operation)

#define D_ISPREFUNARY (D_LHS && !D_RHS)
#define D_ISPOSTUNARY (!D_LHS && D_RHS)
#define D_ISBINOP     (D_LHS && D_RHS)


#define D_NEW_FUNC(RET_, NAME_, ARG_) \
do \
{ \
	t.type = TOKEN_FUNC; \
	t.value.ident = (NAME_); \
	(RET_) = create_func_node(bintree_create(t), ARG_); \
} \
while (false)

#define D_NEW_PREFUNOP(RET_, OP_, ARG_) \
do \
{ \
	t.type = TOKEN_OP; \
	t.value.operation = (OP_); \
	(RET_) = create_prefunop_node(ARG_, t); \
} \
while (false)

#define D_NEW_POSTUNOP(RET_, OP_, ARG_) \
do \
{ \
	t.type = TOKEN_OP; \
	t.value.operation = (OP_); \
	(RET_) = create_postunop_node(ARG_, t); \
} \
while (false)

#define D_NEW_OP(RET_, OP_, LHS_, RHS_) \
do \
{ \
	t.type = TOKEN_OP; \
	t.value.operation = (OP_); \
	(RET_) = create_binop_node(LHS_, RHS_, t); \
} \
while (false)
#define D_CHANGE_TO_NUMBER(WHAT_, NUM_) \
do \
{ \
	double num = (NUM_); \
	(WHAT_)->left  = bintree_destroy((WHAT_)->left); \
	(WHAT_)->right = bintree_destroy((WHAT_)->right); \
	token_destroy(&(WHAT_)->value); \
	(WHAT_)->value.type         = TOKEN_NUMBER; \
	(WHAT_)->value.value.number = num; \
} \
while (false)




#endif // not defined DSL_H_
