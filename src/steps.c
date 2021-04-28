#include "steps.h"

BtorNode *one;

BtorNode **
transform_to_required_form()
{
    //TODO
    return NULL;
}

int
exvar_occurs_kind(Btor *btor, BtorNode **lin_expr, int *lin_count, BtorNode **exp_expr, int *exp_count)
{
	one = btor_exp_bv_one(btor, 2);
    int kind = 1;
    BtorNode *expr;
    for (int i = 1; i < stack_size; i++)
	{
		expr = BTOR_PEEK_STACK(btor->nodes_id_table, i);
		if (expr && !btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) &&
			!btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
		{
			if (btor_node_is_bv_ult(expr) || btor_node_is_bv_eq(expr)) //10, 6
				kind = (without_this_var(btor, expr->e[1], exists_var) && only_this_var(btor, expr->e[0], exists_var)) ||
					   (without_this_var(btor, expr->e[0], exists_var) && only_this_var(btor, expr->e[1], exists_var)) ? kind : 0;
			else if (btor_node_is_bv_add(expr)) //8
				kind = without_this_var(btor, expr->e[0], exists_var) || without_this_var(btor, expr->e[1], exists_var) ||
					   btor_node_is_bv_sll(expr->e[0]) || btor_node_is_bv_sll(expr->e[1]) ? kind : 0;
			else if (btor_node_is_bv_mul(expr)) //9
			{
				kind = without_vars(btor, expr->e[0]) || without_vars(btor, expr->e[1]) ? kind : 0;
				if (kind == 1)
					kind = expr->e[0] != exists_var && expr->e[1] != exists_var;
			}
			else if (btor_node_is_bv_sll(expr)) //11
			{
				kind = only_this_var(btor, expr->e[1], exists_var)? 2 : kind;
				kind = btor_node_is_bv_const(expr->e[0])? kind : 0;
			}
			else if (btor_node_is_bv_and(expr)) //5
			{
				BtorNode *real_child[2];
				for (int j = 0; j < 2; j++)
					real_child[j] = btor_node_real_addr(expr->e[j]);
				for (int j = 0; j < 2; j++)
				{
					if (btor_node_is_bv_eq(expr->e[j]))
					{
						if (!same_children(expr->e[j], expr->e[1-j]) || !btor_node_is_bv_ult(expr->e[1-j]))
						{
							if (real_child[j]->e[0]!=exists_var && real_child[j]->e[1]!=exists_var)
							{
								exp_expr[*exp_count] = btor_node_copy(btor, real_child[j]);
								(*exp_count)++;
								exp_expr[*exp_count] = btor_node_copy(btor, real_child[j]);
								(*exp_count)++;
							}
							else
							{
								BtorNode *sub_expr = btor_exp_bv_sub(btor, real_child[j]->e[1], one);
								BtorNode *add_expr = btor_exp_bv_add(btor, real_child[j]->e[1], one);
								lin_expr[*lin_count] = btor_node_real_addr(btor_exp_bv_ult(btor, real_child[j]->e[0], add_expr));
								(*lin_count)++;
								lin_expr[*lin_count] = btor_node_real_addr(btor_exp_bv_ult(btor, sub_expr, real_child[j]->e[0]));
								(*lin_count)++;
							}
						}
					}
					else if (btor_node_is_bv_ult(expr->e[j]))
					{
						if (real_child[j]->e[0]!=exists_var && real_child[j]->e[1]!=exists_var)
						{
							if (!same_children(expr->e[j], expr->e[1-j]) || !btor_node_is_bv_eq(expr->e[1-j]))
							{
								BtorNode *sub_expr = btor_exp_bv_sub(btor, real_child[j]->e[1], one);
								exp_expr[*exp_count] = btor_node_real_addr(btor_exp_bv_ult(btor, real_child[j]->e[0], sub_expr));
								(*exp_count)++;
								exp_expr[*exp_count] = btor_node_real_addr(btor_exp_eq(btor, real_child[j]->e[0], sub_expr));
								(*exp_count)++;
							}
							else
							{
								exp_expr[*exp_count] = btor_node_copy(btor, real_child[j]);
								(*exp_count)++;
								exp_expr[*exp_count] = btor_node_copy(btor, real_child[1-j]);
								(*exp_count)++;
							}
						}
						else
						{
							if (same_children(expr->e[j], expr->e[1-j]) && btor_node_is_bv_eq(expr->e[1-j]))
							{
								lin_expr[*lin_count] = btor_node_copy(btor, real_child[j]);
								(*lin_count)++;
							}
							else
							{
								BtorNode *sub_expr = btor_exp_bv_sub(btor, real_child[j]->e[1], one);
								lin_expr[*lin_count] = btor_node_real_addr(btor_exp_bv_ult(btor, real_child[j]->e[0], sub_expr));
								(*lin_count)++;
							}
						}
					}
				}
			}
			else if (btor_node_is_exists(expr)) //18
			{
				BtorNode *real_child[2];
				for (int j = 0; j < 2; j++)
					real_child[j] = btor_node_real_addr(expr->e[j]);
				if (real_child[0]->e[0]!=exists_var && real_child[0]->e[1]!=exists_var)
					if (btor_node_is_bv_eq(expr->e[0]))
					{
						BtorNode *sub_expr = btor_exp_bv_sub(btor, real_child[0]->e[1], one);
						BtorNode *add_expr = btor_exp_bv_add(btor, real_child[0]->e[1], one);
						lin_expr[*lin_count] = btor_exp_bv_ult(btor, real_child[0]->e[0], add_expr);
						(*lin_count)++;
						lin_expr[*lin_count] = btor_exp_bv_ult(btor, sub_expr, real_child[0]->e[0]);
						(*lin_count)++;
					}
					else if (btor_node_is_bv_ult(expr->e[0]))
					{
						BtorNode *sub_expr = btor_exp_bv_sub(btor, real_child[0]->e[1], one);
						lin_expr[*lin_count] = btor_exp_bv_ulte(btor, real_child[0]->e[0], sub_expr);
						(*lin_count)++;
					}
			}
			else
				return 0;
			if (kind==0)
			{
				//debug output
				printf("id: %d kind: %d\n", btor_node_get_id(expr), expr->kind);
				return 0;
			}
		}
	}
    return kind;
}

BtorNode *
qe_linear_case(Btor *btor, BtorNode **ulte_expr, int ult_count)
{
	int left_ult_count = 0, right_ult_count = 0; //q and p respectively
	int only_exvar_left[ult_count];
	int only_exvar_right[ult_count];
	BtorNode *res_expr;
	int i, j = 0, k = 0;
	for (i = 0; i < ult_count; i++)
	{
		if (ulte_expr[i]->e[0]==exists_var)
		{
			only_exvar_left[j] = i;
			j++;
			left_ult_count++; //number of expressions where free variables are on the right
		}
		else
		{
			only_exvar_right[k] = i;
			k++;
			right_ult_count++; //number of expressions where free variables are on the left
		}
	}
	int index[2];
	BtorNode *ulte[left_ult_count*right_ult_count];
	if (left_ult_count!=0 && right_ult_count!=0)
	{
		k = 0;
		for (i = 0; i < left_ult_count; i++)
		{
			index[0] = only_exvar_left[i];
			for (j = 0; j < right_ult_count; j++)
			{
				index[1] = only_exvar_right[j];
				ulte[k] = btor_exp_bv_ulte(btor, ulte_expr[index[1]]->e[0], ulte_expr[index[0]]->e[1]);
				res_expr = i + j == 0 ? ulte[k] : btor_exp_bv_and(btor, res_expr, ulte[k]);
				k++;
			}
		}
	}
	else
		res_expr = btor_exp_true(btor);
	return res_expr;
}

BtorNode *
qe_exp_case(Btor *btor, BtorNode *exp_expr, BtorNode **lin_expr, int lin_count)
{
	BtorNode *l2_expr[2], *free_expr, *case_expr[3], *res_expr, *subcase;
	BtorNode *coef[3], *l2_coef[3];
	BtorBitVector *bv = btor_bv_one(btor->mm, bv_size);
	uint64_t b, c, N;
	int left_ulte = 1;
	for(int j = 0; j < 3; j++)
		coef[j] = btor_exp_bv_zero(btor, 2);
	if (only_this_var(btor, exp_expr->e[0], exists_var))
	{
		free_expr = btor_node_copy(btor, exp_expr->e[1]);
		get_coefs(btor, exp_expr->e[0], coef);
	}
	else
	{
		free_expr = btor_node_copy(btor, exp_expr->e[0]);
		get_coefs(btor, exp_expr->e[1], coef);
		left_ulte = 0;
	}
	for (int j = 0; j < 3; j++)
		l2_coef[j] = l2(btor, coef[j]);
	bv = btor_node_to_bv(l2_coef[1]);
	b = 2*btor_bv_to_uint64(bv) + 3;
	bv = btor_node_to_bv(l2_coef[2]);
	c = btor_bv_to_uint64(bv) + 3;
	N = max3(b, c,0);
	BtorNode *const_expr[N + 1];
	l2_expr[0] = btor_exp_bv_sub(btor, l2(btor, free_expr), l2_coef[0]);
	l2_expr[1] = btor_exp_bv_add(btor, l2_expr[0], one);
	case_expr[0] = replace_exvar(btor, exp_expr, l2_expr[0]);
	case_expr[1] = replace_exvar(btor, exp_expr, l2_expr[1]);
	if (lin_count)
		case_expr[0] = qe_replacement(btor, case_expr[0], l2_expr[0], lin_expr, lin_count);
	if (lin_count)
		case_expr[1] = qe_replacement(btor, case_expr[0], l2_expr[0], lin_expr, lin_count);
	const_expr[0] = btor_exp_bv_zero(btor, 2);
	case_expr[2] = replace_exvar(btor, exp_expr, const_expr[0]);
	if (lin_count)
		case_expr[2] = qe_replacement(btor, case_expr[2], const_expr[0], lin_expr, lin_count);
	for (int j = 1; j <= N; j++)
	{
		bv = btor_bv_uint64_to_bv(btor->mm, j, bv_size);
		const_expr[j] = btor_exp_bv_const(btor, bv);
		subcase = replace_exvar(btor, exp_expr, const_expr[j]);
		if (lin_count)
			subcase = qe_replacement(btor, subcase, const_expr[j], lin_expr, lin_count);
		case_expr[2] = btor_exp_bv_or(btor, case_expr[2], subcase);
	}
	res_expr = case_expr[0];
	for (int j = 1; j < 3; j++)
		res_expr = btor_exp_bv_or(btor, res_expr, case_expr[j]);
	for (int j = 0; j < 3; j++)
		btor_node_release(btor, case_expr[j]);
	for (int j = 0; j < 2; j++)
		btor_node_release(btor, l2_expr[j]);
	btor_node_release(btor, free_expr);
	return res_expr;
}

BtorNode *
qe_replacement(Btor *btor, BtorNode *exp_expr, BtorNode *value, BtorNode **lin_expr, int lin_count)
{
	BtorNode *and_expr, *eq_expr, *or_expr;
	for (int j = 0; j < lin_count; j++)
	{
		or_expr = replace_exvar(btor, lin_expr[j], value);
		eq_expr = btor_exp_eq(btor, lin_expr[j]->e[0], lin_expr[j]->e[1]);
		eq_expr = replace_exvar(btor, eq_expr, value);
		or_expr = btor_exp_bv_or(btor, or_expr, eq_expr);
		and_expr = j==0 ? or_expr : btor_exp_bv_and(btor, and_expr, or_expr);
	}
	return btor_exp_bv_and(btor, exp_expr, and_expr);
}
