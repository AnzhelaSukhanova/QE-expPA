#include "btor_tools.h"

bool
only_this_var(Btor *btor, BtorNode *expr, BtorNode *var)
{
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) &&
		!btor_node_is_param(real_expr) && !btor_node_is_bv_slice(real_expr))
	{
		if (!only_this_var(btor, real_expr->e[0], var))
			return false;
		return (only_this_var(btor, real_expr->e[1], var));
	}
	else if (btor_node_is_bv_var(real_expr) || btor_node_is_param(real_expr))
		return (real_expr==var);
	else
		return true;
}

bool
without_this_var(Btor *btor, BtorNode *expr, BtorNode *var)
{
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) &&
		!btor_node_is_param(real_expr) && !btor_node_is_bv_slice(real_expr))
	{
		if (!without_this_var(btor, real_expr->e[0], var))
			return false;
		return without_this_var(btor, real_expr->e[1], var);
		return true;
	}
	else
		return (real_expr!=var);
}

bool
without_vars(Btor *btor, BtorNode *expr)
{
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (btor_node_is_bv_const(real_expr))
		return true;
	else if (!btor_node_is_bv_var(real_expr) && !btor_node_is_param(real_expr)
		&& !btor_node_is_bv_slice(real_expr))
	{
		if (!without_vars(btor, real_expr->e[0]))
			return false;
		return without_vars(btor, real_expr->e[1]);
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

size_t
get_stack_size(Btor *btor)
{
	return BTOR_COUNT_STACK(btor->nodes_id_table);
}

BtorNode *
get_exists_var(Btor *btor)
{
	if (btor->exists_vars->first != NULL)
	{
		return (BtorNode *)btor->exists_vars->first->key;
	}
	return NULL;
}

BtorNode *
replace_exvar(Btor *btor, BtorNode *expr, BtorNode *value)
{
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (real_expr==exists_var)
		return value;
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) &&
		!btor_node_is_param(real_expr) && !btor_node_is_bv_slice(real_expr))
	{
		BtorNode *e[3], *res;
		e[0] = replace_exvar(btor, real_expr->e[0], value);
		e[1] = replace_exvar(btor, real_expr->e[1], value);
		e[2] = real_expr->e[2];
		res = btor_exp_create(btor, real_expr->kind, e, real_expr->arity);
		return res;
	}
	return expr;
}

BtorNode *
l2(Btor *btor,  BtorNode *expr)
{
	BtorBitVector *bv;
	uint64_t power = 1;
	int max = pow(2, bv_size) - 1;
	if (!btor_node_is_bv_const(expr))
	{
		bv = btor_bv_uint64_to_bv(btor->mm, max, bv_size);;
		BtorNode *cond_expr[bv_size + 1], *btor_power;
		cond_expr[0] = btor_exp_bv_const(btor, bv);
		for (int j = 0; j < bv_size; j++)
		{
			bv = btor_bv_uint64_to_bv(btor->mm, power, bv_size);
			btor_power = btor_exp_bv_const(btor, bv);
			bv = btor_bv_uint64_to_bv(btor->mm, j, bv_size);
			cond_expr[j + 1] =
				btor_exp_cond(btor, btor_exp_bv_ulte(btor, btor_power, expr), btor_exp_bv_const(btor, bv), cond_expr[j]);
			power *= 2;
		}
		return cond_expr[bv_size];
	}
	else
	{
		bv = btor_node_to_bv(expr);
		uint64_t num = btor_bv_to_uint64(bv);
		if (num != 0)
			for (int j = 0; j < bv_size; j++)
			{
				if (power*2 > num)
					return btor_exp_bv_const(btor, btor_bv_uint64_to_bv(btor->mm, j, bv_size));
				power *= 2;
			}
		return btor_exp_bv_const(btor, btor_bv_uint64_to_bv(btor->mm, max, bv_size));
	}
}

void
get_coefs(Btor *btor,  BtorNode *expr, BtorNode *coef[3])
{
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) && !btor_node_is_param(real_expr)
		&& !btor_node_is_bv_slice(real_expr))
	{
		if (btor_node_is_bv_sll(real_expr))
		{
			if (btor_node_is_bv_const(real_expr->e[0]))
				coef[0] = btor_node_copy(btor, real_expr->e[0]);
			else if (btor_node_is_bv_const(real_expr->e[1]))
				coef[0] = btor_node_copy(btor, real_expr->e[1]);
			else
				coef[0] = btor_node_copy(btor, one);
		}
		else if (btor_node_is_bv_mul(real_expr) && (real_expr->e[0]==exists_var || real_expr->e[1]==exists_var))
		{
				if (btor_node_is_bv_const(real_expr->e[0]))
					coef[1] = btor_node_copy(btor, real_expr->e[0]);
				else if (btor_node_is_bv_const(real_expr->e[1]))
					coef[1] = btor_node_copy(btor, real_expr->e[1]);
				else
					coef[1] = btor_node_copy(btor, one);
		}
		else if (btor_node_is_bv_add(real_expr))
		{
			if (btor_node_is_bv_const(real_expr->e[0]))
				coef[2] = btor_node_copy(btor, real_expr->e[0]);
			else if (btor_node_is_bv_const(real_expr->e[1]))
				coef[2] = btor_node_copy(btor, real_expr->e[1]);
			if (real_expr->e[0]==exists_var)
				coef[1] = btor_node_copy(btor, one);
			else if (real_expr->e[1]==exists_var)
				coef[1] = btor_node_copy(btor, one);
		}
		get_coefs(btor, real_expr->e[0], coef);
		get_coefs(btor, real_expr->e[1], coef);
	}
}

BtorBitVector *
btor_node_to_bv(BtorNode *expr)
{
	return btor_node_is_inverted(expr) ? btor_node_bv_const_get_invbits(expr) : btor_node_bv_const_get_bits(expr);
}

bool
same_children(BtorNode *expr1, BtorNode *expr2)
{
	BtorNode *real_expr1 = btor_node_real_addr(expr1);
	BtorNode *real_expr2 = btor_node_real_addr(expr2);
	return ((real_expr1->e[0] == real_expr2->e[0] && real_expr1->e[1] == real_expr2->e[1]) ||
		    (real_expr1->e[0] == real_expr2->e[1] && real_expr1->e[1] == real_expr2->e[0]));
}

bool
is_exvar_exp_term(BtorNode *expr)
{
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (btor_node_is_bv_sll(real_expr) && real_expr->e[1] == exists_var)
		return true;
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) && !btor_node_is_param(real_expr)
		&& !btor_node_is_bv_slice(real_expr))
		return (is_exvar_exp_term(real_expr->e[0]) || is_exvar_exp_term(real_expr->e[1]));
	return false;
}
