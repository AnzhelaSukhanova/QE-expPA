#include "btor_tools.h"

bool
only_this_var(Btor *btor, BtorNode *expr, BtorNode *var)
{
	if (!btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) &&
		!btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
	{
		uint32_t id = abs(btor_node_get_id(expr->e[0]));
		BtorNode *child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
		if (!only_this_var(btor, child_expr, var) && !btor_node_is_bv_const(child_expr))
		{
			return false;
		}
		id = abs(btor_node_get_id(expr->e[1]));
		child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
		return (only_this_var(btor, child_expr, var) || btor_node_is_bv_const(child_expr));
	}
	else
		return (expr==var);
}

bool
without_this_var(Btor *btor, BtorNode *expr, BtorNode *var)
{
	if (!btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) &&
		!btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
	{
		uint32_t id = abs(btor_node_get_id(expr->e[0]));
		BtorNode *child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
		if (!without_this_var(btor, expr->e[0], var))
			return false;
		id = abs(btor_node_get_id(expr->e[1]));
		child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
		return without_this_var(btor, child_expr, var);
	}
	else
		return (expr!=var);
}

bool
without_vars(Btor *btor, BtorNode *expr)
{
    if (btor_node_is_bv_const(expr))
        return true;
    else if (!btor_node_is_bv_var(expr) && !btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
    {
        uint32_t id = abs(btor_node_get_id(expr->e[0]));
        BtorNode *child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
        if (!without_vars(btor, child_expr))
            return false;
        id = abs(btor_node_get_id(expr->e[1]));
        child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
        return without_vars(btor, child_expr);
    }
    else
        return false;
}

void
printf_exprs_info(Btor *btor)
{
	BtorNode *expr;
	for (int i = 1; i < stack_size; i++)
	{
		expr = btor_node_copy(btor, BTOR_PEEK_STACK(btor->nodes_id_table, i));
		assert(expr);
		printf("id: %d kind: %d\n", btor_node_get_id(expr), expr->kind);
		if (!btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) &&
			!btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
			printf("kinds of children: %d %d\n\n", expr->e[0]->kind, expr->e[1]->kind);
		else
			printf("\n");
	}
}

BtorNode *
get_exists_var(Btor *btor)
{
    int i = 0;
    bool is_exists_var = 0;
    while (i + 1 < stack_size && !is_exists_var)
    {
        i++;
        exists_var = btor_node_copy(btor, BTOR_PEEK_STACK(btor->nodes_id_table, i));
        if (btor_node_is_param(exists_var))
            is_exists_var = btor_node_param_is_exists_var(exists_var);
    }
    assert(btor_node_param_is_exists_var(exists_var));
    return exists_var;
}

BtorNode *
replace_exvar(Btor *btor, BtorNode *expr, BtorNode *value)
{
	if (expr==exists_var)
		return value;
	if (!btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) &&
		!btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
	{
		BtorNode *e[3], *res;
		if (expr->e[0]==exists_var)
		{
			e[0] = value;
			e[1] = expr->e[1];
			e[2] = expr->e[2];
			res = btor_exp_create(btor, expr->kind, e, expr->arity);
		}
		else if (expr->e[1]==exists_var)
		{
			e[0] = expr->e[0];
			e[1] = value;
			e[2] = expr->e[2];
			res = btor_exp_create(btor, expr->kind, e, expr->arity);
		}
		else
		{
			uint32_t id = abs(btor_node_get_id(expr->e[0]));
			e[0] = btor_node_copy(btor, BTOR_PEEK_STACK(btor->nodes_id_table, id));
			e[0] = replace_exvar(btor, e[0], value);
			id = abs(btor_node_get_id(expr->e[1]));
			e[1] = btor_node_copy(btor, BTOR_PEEK_STACK(btor->nodes_id_table, id));
			e[1] = replace_exvar(btor, e[1], value);
			e[2] = expr->e[2];
			res = btor_exp_create(btor, expr->kind, e, expr->arity);
		}
		return res;
	}
	return expr;
}

BtorBitVector *
get_exp_coef_sum(Btor *btor, BtorNode **exp_expr, int exp_count)
{
	BtorBitVector *sum = btor_bv_zero(btor->mm, bv_size);
	BtorBitVector *coef;
	BtorNode *expr;
	for (int i = 0; i < exp_count; i++)
	{
		expr = find_exp_by_child_kind(btor, exp_expr[i], (BtorNodeKind) 11);
		if (btor_node_is_bv_const(expr->e[0]))
			coef = btor_node_is_inverted(expr->e[0]) ? btor_node_bv_const_get_invbits(expr->e[0])
													 : btor_node_bv_const_get_bits(expr->e[0]);
		else if (btor_node_is_bv_const(expr->e[1]))
			coef = btor_node_is_inverted(expr->e[1]) ? btor_node_bv_const_get_invbits(expr->e[1])
													 : btor_node_bv_const_get_bits(expr->e[1]);
		else
			coef = btor_bv_one(btor->mm, bv_size);
		sum = btor_bv_add(btor->mm, sum, coef);
	}
	return sum;
}

BtorNode *
find_exp_by_child_kind(Btor *btor, BtorNode *expr, BtorNodeKind kind)
{
	assert(expr && !btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) && !btor_node_is_param(expr)
			   && !btor_node_is_bv_slice(expr));
	if (expr->e[0]->kind!=kind)
	{
		if (expr->e[1]->kind!=kind)
		{
			uint32_t id = abs(btor_node_get_id(expr->e[0]));
			BtorNode *child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
			child_expr = find_exp_by_child_kind(btor, child_expr, kind);
			if (child_expr != NULL) return child_expr;
			id = abs(btor_node_get_id(expr->e[0]));
			child_expr = BTOR_PEEK_STACK(btor->nodes_id_table, id);
			child_expr = find_exp_by_child_kind(btor, child_expr, kind);
			if (child_expr != NULL) return child_expr;
			return NULL;
		}
		else
			return expr;
	}
	else
		return expr;
}

BtorNode *
l2(Btor *btor,  BtorNode *expr)
{
	BtorBitVector *bv;
	BtorNode *cond_expr[bv_size], *power;
	cond_expr[0] = btor_exp_bv_zero(btor, 2);
	for (int j = 1; j < bv_size; j++)
	{
		bv = btor_bv_uint64_to_bv(btor->mm, pow(2, j), bv_size);
		power = btor_exp_bv_const(btor, bv);
		bv = btor_bv_uint64_to_bv(btor->mm, j, bv_size);
		cond_expr[j] = btor_exp_cond(btor, btor_exp_bv_ulte(btor, power, expr), btor_exp_bv_const(btor, bv), cond_expr[j-1]);
	}
	return cond_expr[bv_size - 1];
}
