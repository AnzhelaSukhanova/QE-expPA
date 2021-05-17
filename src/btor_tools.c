#include "btor_tools.h"

BtorNodeArray* btornodearr_new(uint size)
{
	BtorNodeArray *array = NULL;

	BtorNode *expr[size];
	if (!(array = (BtorNodeArray*)malloc(sizeof(BtorNodeArray))))
		exit(3);
	if (!(array->expr = (BtorNode **)malloc(size * sizeof(BtorNode *))))
		exit(3);
	array->size = size;
	array->count = 0;
	return array;
}

bool
only_this_var(Btor *btor, BtorNode *expr, BtorNode *var)
{
	if (expr == btor->true_exp || expr == False) return false;
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) &&
		!btor_node_is_param(real_expr) && !btor_node_is_bv_slice(real_expr))
	{
		if (!only_this_var(btor, real_expr->e[0], var) && !btor_node_is_bv_const(real_expr->e[0]))
			return false;
		return (only_this_var(btor, real_expr->e[1], var) || btor_node_is_bv_const(real_expr->e[1]));
	}
	else return (real_expr==var);
}

bool
without_this_var(Btor *btor, BtorNode *expr, BtorNode *var)
{
	if (expr == btor->true_exp || expr == False) return true;
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) &&
		!btor_node_is_param(real_expr) && !btor_node_is_bv_slice(real_expr))
	{
		if (!without_this_var(btor, real_expr->e[0], var))
			return false;
		return without_this_var(btor, real_expr->e[1], var);
	}
	else
		return (real_expr!=var);
}

bool
with_this_var(Btor *btor, BtorNode *expr, BtorNode *var)
{
	if (expr == btor->true_exp || expr == False) return false;
	BtorNode *real_expr = btor_node_real_addr(expr);
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) &&
		!btor_node_is_param(real_expr) && !btor_node_is_bv_slice(real_expr))
	{
		if (with_this_var(btor, real_expr->e[0], var))
			return true;
		return with_this_var(btor, real_expr->e[1], var);
	}
	else
		return (real_expr==var);
}

bool
without_vars(Btor *btor, BtorNode *expr)
{
	if (expr == btor->true_exp || expr == False) return true;
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
get_first_exists_var(Btor *btor)
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
	uint64_t power = 1;
	uint64_t max = pow(2, bv_size) - 1;
	if (!btor_node_is_bv_const(expr))
	{
		BtorNode *cond_expr[bv_size + 1], *btor_power;
		cond_expr[0] = uint64_to_btornode(btor, max, bv_size);
		for (int j = 0; j < bv_size; j++)
		{
			btor_power = uint64_to_btornode(btor, power, bv_size);
			cond_expr[j + 1] =
				btor_exp_cond(btor, btor_exp_bv_ulte(btor, btor_power, expr), uint64_to_btornode(btor, j, bv_size), cond_expr[j]);
			power *= 2;
		}
		return cond_expr[bv_size];
	}
	else
	{
		uint64_t num = btornode_to_uint64(btor, expr);
		if (num != 0)
			for (int j = 0; j < bv_size; j++)
			{
				if (power*2 > num)
					return uint64_to_btornode(btor, j, bv_size);
				power *= 2;
			}
		return uint64_to_btornode(btor, max, bv_size);
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
btornode_to_bv(Btor *btor, BtorNode *expr)
{
	BtorBitVector *bv = btor_node_is_inverted(expr)? btor_node_bv_const_get_invbits(expr) : btor_node_bv_const_get_bits(expr);
	/*btor_dumpsmt_dump_node(btor, stdout, expr, -1);
	printf(" ");
	btor_bv_print(bv);*/
	return bv;
}

uint64_t
btornode_to_uint64(Btor *btor, BtorNode *expr)
{
	BtorBitVector *bv = btornode_to_bv(btor, expr);
	return btor_bv_to_uint64(bv);
}

BtorNode *
uint64_to_btornode(Btor *btor, uint64_t num, int size)
{
	BtorBitVector *bv = btor_bv_uint64_to_bv(btor->mm, num, size);
	return btor_exp_bv_const(btor, bv);
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

BtorNode *
resize_expr(Btor *btor, BtorNode *expr, int old_bv_size)
{
	if (!expr || expr == btor->true_exp || expr == False) return expr;
	BtorNode *real_expr = btor_node_real_addr(expr);
	BtorNode *res;
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) && !btor_node_is_param(real_expr))
	{
		BtorNode *e[3];
		e[0] = resize_expr(btor, real_expr->e[0], old_bv_size);
		e[1] = resize_expr(btor, real_expr->e[1], old_bv_size);
		e[2] = resize_expr(btor, real_expr->e[1], old_bv_size);
		res = btor_exp_create(btor, real_expr->kind, e, real_expr->arity);
		if (btor_node_is_inverted(expr))
			res = btor_exp_bv_not(btor, res);
	}
	else if (btor_node_is_bv_const(real_expr))
	{
		uint64_t bv_const = btornode_to_uint64(btor, expr);
		res = uint64_to_btornode(btor, bv_const, bv_size);
	}
	else if (btor_node_is_bv_var(real_expr) || btor_node_is_param(real_expr))
	{
		/*BtorSortId sort = btor_sort_bv(btor, bv_size);
		real_expr->sort_id = sort;*/
		BtorNode *zero_con = uint64_to_btornode(btor, 0, bv_size - old_bv_size);
		res = btor_node_create_bv_concat(btor, zero_con, real_expr);
		/*char *symbol = btor_node_get_symbol(btor, real_expr);
		res = btor_node_is_bv_var(real_expr)? btor_node_create_var(btor, sort, symbol) :
			                                  btor_node_create_param(btor, sort, symbol);*/
	}
	return res;
}

BtorNode *
int_sub(Btor *btor, BtorNode *expr1, BtorNode *expr2)
{
	BtorNode *sub = btor_exp_bv_sub(btor, expr1, expr2);
	return btor_exp_cond(btor, btor_exp_bv_ult(btor, expr1, expr2), zero, sub);

}

BtorNode *
get_rem(Btor *btor, BtorNode *expr, int old_bv_size, int max_power)
{
	if (!expr || expr == btor->true_exp || expr == False) return expr;
	BtorNode *real_expr = btor_node_real_addr(expr), *res;
	if (!btor_node_is_bv_const(real_expr) && !btor_node_is_bv_var(real_expr) && !btor_node_is_param(real_expr))
	{
		uint64_t base = pow(2, old_bv_size);
		BtorNode *e[3], *cond, *case_if, *case_else;
		BtorNode *base_expr = uint64_to_btornode(btor, base, bv_size);
		if (btor_node_is_bv_add(real_expr))
		{
			cond = btor_exp_bv_urem(btor, real_expr->e[0], base_expr);
			cond = btor_exp_bv_add(btor, cond, btor_exp_bv_urem(btor, real_expr->e[0], base_expr));
			cond = btor_exp_bv_ult(btor, cond, base_expr);
			case_if = get_rem(btor, real_expr->e[0], old_bv_size, max_power);
			case_if = btor_exp_bv_add(btor, case_if, get_rem(btor, real_expr->e[1], old_bv_size, max_power));
			case_else = btor_exp_bv_sub(btor, case_if, base_expr);
			res = btor_exp_cond(btor, cond, case_if, case_else);
		}
		else if (btor_node_is_bv_sll(expr))
		{
			cond = btor_exp_bv_ult(btor, real_expr->e[0], uint64_to_btornode(btor, old_bv_size, bv_size));
			case_else = zero;
			res = btor_exp_cond(btor, cond, expr, case_else);
		}
		else if (btor_node_is_bv_mul(expr))
		{
			if (btor_node_is_bv_const(real_expr->e[0]))
				case_else = btor_exp_bv_mul(btor, real_expr->e[0], get_rem(btor, real_expr->e[1], old_bv_size, max_power));
			else
				case_else = btor_exp_bv_mul(btor, get_rem(btor, real_expr->e[0], old_bv_size, max_power), real_expr->e[1]);
			res = case_else;
			uint64_t extra = base;
			for (int i = 1; i < pow(2, max_power) - 1; i++)
			{
				cond = btor_exp_bv_urem(btor, real_expr->e[0], base_expr);
				cond = btor_exp_bv_mul(btor, cond, btor_exp_bv_urem(btor, real_expr->e[0], base_expr));
				cond = btor_exp_bv_ult(btor, base_expr, cond);
				case_if = btor_exp_bv_sub(btor, case_else, uint64_to_btornode(btor, extra, bv_size));
				res = btor_exp_cond(btor, cond, case_if, res);
				extra = extra + base;
			}
		}
		else
		{
			e[0] = get_rem(btor, real_expr->e[0], old_bv_size, max_power);
			e[1] = get_rem(btor, real_expr->e[1], old_bv_size, max_power);
			e[2] = get_rem(btor, real_expr->e[1], old_bv_size, max_power);
			res = btor_exp_create(btor, real_expr->kind, e, real_expr->arity);
		}
		if (btor_node_is_inverted(expr))
			res = btor_exp_bv_not(btor, res);
	}
	else res = expr;
	return res;
}

uint64_t
find_LCM(Btor *btor, BtorNodeArray *lin, uint64_t *coef)
{
	uint64_t LCM = 1;
	BtorNode *mul, *coef_expr;
	for (int i = 0; i < lin->count; i++)
	{
		mul = with_this_var(btor, lin->expr[i]->e[0], exists_var) ? lin->expr[i]->e[0] : lin->expr[i]->e[1];
		if (mul!=exists_var)
		{
			coef_expr = btor_node_is_bv_const(mul->e[0]) ? mul->e[0] : mul->e[1];
			coef[i] = btornode_to_uint64(btor, coef_expr);
			LCM = lcm(LCM, coef[i]);
		}
		else
			coef[i] = 1;
	}
	return LCM;
}

//from boolector/src/utils/btorhashptr.c
void
btor_hashptr_table_forget_first(BtorPtrHashTable *table)
{
	BtorPtrHashBucket *bucket = table->first;

	if (bucket->prev)
		bucket->prev->next = bucket->next;
	else
		table->first = bucket->next;

	if (bucket->next)
		bucket->next->prev = bucket->prev;
	else
		table->last = bucket->prev;

	assert (table->count > 0);
	table->count--;
}
