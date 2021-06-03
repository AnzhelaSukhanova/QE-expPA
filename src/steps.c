#include "steps.h"

BtorNode *one = NULL;
BtorNode *zero = NULL;

QECaseKind max(QECaseKind a, QECaseKind b)
{
	int int_a = (int)a;
	int int_b = (int)b;
	return (QECaseKind)((int_a > int_b) ? int_a : int_b);
}

void
transform_to_required_form(Btor *btor, BtorNode *formula)
{
    //TODO
	if (formula && !btor_node_is_bv_const(formula) && !btor_node_is_bv_var(formula) &&
		!btor_node_is_param(formula) && !btor_node_is_bv_slice(formula))
	{
		BtorNode *child = btor_node_real_addr(formula->e[0]);
		transform_to_required_form(btor, child);
		child = btor_node_real_addr(formula->e[1]);
		transform_to_required_form(btor, child);
		if (btor_node_is_ternary(formula))
		{
			child = btor_node_real_addr(formula->e[1]);
			transform_to_required_form(btor, child);
		}
		if (btor_node_is_forall(formula))
		{
			btor_exp_exists(btor, formula->e[0], btor_node_invert(formula->e[1]));
		}
	}
}

QECaseKind
exvar_occurs_kind(Btor *btor, BtorNode *formula, BtorNodeArray *lin, BtorNodeArray *exp, BtorNodeArray *free_vars, int inv_depth)
{
	if (one == NULL)
	{
		one = btor_exp_bv_one(btor, BTOR_BV_SORT);
		zero = btor_exp_bv_zero(btor, BTOR_BV_SORT);
	}
	QECaseKind kind = SIMP_LIN;
	if (btor_node_is_inverted(formula)) inv_depth++;
	BtorNode *expr = btor_node_real_addr(formula);
    if (formula && !btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) &&
			!btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
	{
    	kind = exvar_occurs_kind(btor, expr->e[0], lin, exp, free_vars, inv_depth);
		kind = kind==INCORRECT ? INCORRECT : max(kind, exvar_occurs_kind(btor, expr->e[1], lin, exp, free_vars, inv_depth));
		if (btor_node_is_ternary(expr))
			kind = kind==INCORRECT ? INCORRECT : max(kind, exvar_occurs_kind(btor, expr->e[2], lin, exp, free_vars, inv_depth));
		if (kind==INCORRECT)
			return INCORRECT;
		if ((btor_node_is_bv_ult(expr) || btor_node_is_bv_eq(expr))) //10, 6
		{
			if (without_this_var(btor, expr, exists_var))
			{
				free_vars->expr[free_vars->count] = btor_node_copy(btor, expr);
				free_vars->count++;
			}
			else
				kind =
					(without_this_var(btor, expr->e[1], exists_var) && only_this_var(btor, expr->e[0], exists_var))
						||
							(without_this_var(btor, expr->e[0], exists_var)
								&& only_this_var(btor, expr->e[1], exists_var)) ? kind : INCORRECT;
		}
		else if (btor_node_is_bv_add(expr)) //8
		{
			if (with_this_var(btor, expr, exists_var))
			{
				if (kind==SIMP_EXP)
					kind = EXPONENTIAL;
				else if (kind==SIMP_LIN)
					kind = LINEAR;
			}
		}
		else if (btor_node_is_bv_mul(expr)) //9
		{
			kind = without_vars(btor, expr->e[0]) || without_vars(btor, expr->e[1]) ? kind : INCORRECT;
			if (expr->e[0]!=one && expr->e[1]==exists_var ||
				expr->e[1]!=one && expr->e[0]==exists_var)
			{
				if (kind==SIMP_EXP)
					kind = EXPONENTIAL;
				else if (kind==SIMP_LIN)
					kind = LINEAR;
			}
		}
		else if (btor_node_is_bv_sll(expr)) //11
		{
			kind = expr->e[1]==exists_var ? SIMP_EXP : kind;
			kind = btor_node_is_bv_const(expr->e[0]) ? kind : INCORRECT;
			if (expr->e[0]!=one && expr->e[1]==exists_var)
				kind = EXPONENTIAL;
		}
		else if (btor_node_is_bv_and(expr) || btor_node_is_exists(expr)) //5, 18
		{
			if(inv_depth%2==1 && !same_children(expr->e[0], expr->e[1]))
			{
				if (kind < SIMP_EXP)
					lin->count = 0;
				else
					exp->count = 0;
			}
			BtorNode *real_child[2];
			for (int j = 0; j < 2; j++)
			{
				if (inv_depth%2==0 && btor_node_is_inverted(expr->e[j]) &&
					same_children(expr->e[j], expr->e[1 - j]))
				{
					if (btor_node_is_bv_eq(expr->e[j]))
						expr->e[j] = btor->true_exp;
					if (btor_node_is_bv_ult(expr->e[j]))
					{
						real_child[j] = btor_node_real_addr(expr->e[j]);
						BtorNode *e[3];
						e[0] = real_child[j]->e[1];
						e[1] = real_child[j]->e[0];
						e[2] = real_child[j]->e[2];
						expr->e[j] = btor_exp_create(btor, BTOR_BV_ULT_NODE, e, real_child[j]->arity);
					}
				}
				real_child[j] = btor_node_real_addr(expr->e[j]);
			}
			for (int j = 0; j < 2; j++)
			{
				if (btor_node_is_bv_eq(expr->e[j]))
				{
					if (!same_children(expr->e[j], expr->e[1 - j]) || !btor_node_is_bv_ult(expr->e[1 - j]))
					{
						if (is_exvar_exp_term(real_child[j]->e[0]) || is_exvar_exp_term(real_child[j]->e[1]))
						{
							exp->expr[exp->count] = btor_node_copy(btor, expr->e[j]);
							exp->count++;
						}
						else
						{
							int k = with_this_var(btor, real_child[j]->e[0], exists_var) ? 0 : 1;
							lin->expr[lin->count] = expr->e[j];
							lin->left_expr[lin->count] = real_child[j]->e[k];
							lin->right_expr[lin->count] = real_child[j]->e[1 - k];
							lin->count++;
							lin->expr[lin->count] = expr->e[j];
							lin->left_expr[lin->count] = real_child[j]->e[1 - k];
							lin->right_expr[lin->count] = real_child[j]->e[k];
							lin->count++;
						}
					}
				}
				else if (btor_node_is_bv_ult(expr->e[j]))
				{
					bool left_exp_ult = is_exvar_exp_term(real_child[j]->e[0]);
					if (left_exp_ult || is_exvar_exp_term(real_child[j]->e[1]))
					{
						if (!same_children(expr->e[j], expr->e[1 - j]) || !btor_node_is_bv_eq(expr->e[1 - j]))
						{
							if (left_exp_ult)
							{
								BtorNode *sub_expr = btor_exp_bv_sub(btor, real_child[j]->e[1], one);
								free_vars->expr[free_vars->count] = btor_exp_bv_ulte(btor, one, real_child[j]->e[1]);
								free_vars->count++;
								exp->expr[exp->count] = btor_exp_bv_ult(btor, real_child[j]->e[0], sub_expr);
								exp->count++;
								exp->expr[exp->count] = btor_exp_eq(btor, real_child[j]->e[0], sub_expr);
								exp->count++;
							}
							else
							{
								BtorNode *add_expr = btor_exp_bv_add(btor, real_child[j]->e[0], one);
								uint64_t max = pow(2, bv_size - 1);
								free_vars->expr[free_vars->count] = btor_exp_bv_ult(btor,
																		  real_child[j]->e[0],
																		  uint64_to_btornode(btor, max, bv_size));
								free_vars->count++;
								exp->expr[exp->count] = btor_exp_bv_ult(btor, add_expr, real_child[j]->e[1]);
								exp->count++;
								exp->expr[exp->count] = btor_exp_eq(btor, add_expr, real_child[j]->e[1]);
								exp->count++;
							}
						}
						else
						{
							exp->expr[exp->count] = btor_node_copy(btor, real_child[j]);
							exp->count++;
							exp->expr[exp->count] = btor_node_copy(btor, real_child[1 - j]);
							exp->count++;
						}
					}
					else
					{
						if (same_children(expr->e[j], expr->e[1 - j]) && btor_node_is_bv_eq(expr->e[1 - j]))
						{
							lin->expr[lin->count] = btor_node_copy(btor, real_child[j]);
							lin->left_expr[lin->count] = real_child[j]->e[0];
							lin->right_expr[lin->count] = real_child[j]->e[1];
							lin->count++;
						}
						else
						{
							if (with_this_var(btor, real_child[j]->e[0], exists_var))
							{
								BtorNode *sub_expr = btor_exp_bv_sub(btor, real_child[j]->e[1], one);
								lin->expr[lin->count] = btor_exp_bv_ult(btor, real_child[j]->e[0], sub_expr);
								lin->left_expr[lin->count] = real_child[j]->e[0];
								lin->right_expr[lin->count] = sub_expr;
								free_vars->expr[free_vars->count] = btor_exp_bv_ulte(btor, one, real_child[j]->e[1]);
								free_vars->count++;
							}
							else
							{
								BtorNode *add_expr = btor_exp_bv_add(btor, real_child[j]->e[0], one);
								lin->expr[lin->count] = btor_exp_bv_ult(btor, add_expr, real_child[j]->e[1]);
								lin->left_expr[lin->count] = add_expr;
								lin->right_expr[lin->count] = real_child[j]->e[1];
								uint64_t max = pow(2, bv_size) - 1;
								free_vars->expr[free_vars->count] = btor_exp_bv_ult(btor,
																		  real_child[j]->e[0],
																		  uint64_to_btornode(btor, max, bv_size));
								free_vars->count++;
							}
							lin->count++;
						}
					}
				}
			}
		}
		else
			return INCORRECT;
	}
    return (QECaseKind)kind;
}

BtorNode *
qe_linear_case(Btor *btor, BtorNodeArray *ult, uint64_t LCM, int old_bv_size)
{
	int left_ult_count = 0, right_ult_count = 0; //q and p respectively
	int only_exvar_left[ult->count];
	int only_exvar_right[ult->count];
	int i, j = 0, k = 0;
	for (i = 0; i < ult->count; i++)
	{
		if (with_this_var(btor, ult->left_expr[i], exists_var))
		{
			only_exvar_left[j] = i;
			left_ult_count++; //number of expressions where free variables are on the right
			j++;
		}
		else
		{
			only_exvar_right[k] = i;
			right_ult_count++; //number of expressions where free variables are on the left
			k++;
		}
	}
	BtorNode *res_expr = NULL;
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
				ulte[k] = btor_exp_bv_ulte(btor, ult->left_expr[index[1]], ult->right_expr[index[0]]);
				res_expr = i + j == 0 ? ulte[k] : btor_exp_bv_and(btor, res_expr, ulte[k]);
				k++;
			}
		}
	}
	else
		res_expr = btor->true_exp;
	if (LCM != 1)
	{
		//TODO
		BtorNode *expr1, *expr2, *dif, *or_expr, *mod_expr, *mod;
		BtorNode *and_expr = NULL;
		BtorNode *const_expr[LCM + 1], *eq_expr[LCM];
		for (i = 0; i <= LCM; i++)
			const_expr[i] = uint64_to_btornode(btor, i, bv_size);
		if (right_ult_count!=0 && left_ult_count!=0)
		{
			for (i = 0; i < left_ult_count; i++)
			{
				index[0] = only_exvar_left[i];
				for (j = 0; j < right_ult_count; j++)
				{
					index[1] = only_exvar_right[j];
					expr1 = ult->expr[index[0]];
					expr2 = ult->expr[index[1]];
					dif = btor_exp_bv_sub(btor, expr1, expr2);
					or_expr = btor_exp_bv_ulte(btor, dif, const_expr[LCM]);
					mod = btor_exp_bv_urem(btor, expr2, const_expr[LCM]);
					for (k = 0; k < LCM; k++)
					{
						eq_expr[k] = btor_exp_eq(btor, dif, const_expr[k]);
						if (k == 0)
							mod_expr = btor_exp_eq(btor, mod, const_expr[0]);
						else
							mod_expr = btor_exp_bv_or(btor, mod_expr, btor_exp_eq(btor, mod, const_expr[LCM - k]));
						eq_expr[k] = btor_exp_bv_and(btor, eq_expr[k], mod_expr);
						or_expr = btor_exp_bv_or(btor, or_expr, eq_expr[k]);
					}
					and_expr = and_expr == NULL? or_expr : btor_exp_bv_or(btor, and_expr, or_expr);
				}
			}
			res_expr = btor_exp_bv_and(btor, res_expr, and_expr);
		}
	}
	return res_expr;
}

BtorNode *
qe_exp_case(Btor *btor, BtorNode *exp_expr, BtorNodeArray *lin)
{
	BtorNode *free_expr, *or_expr, *res_expr, *subcase;
	BtorNode *coef[3], *l2_coef[3];
	int64_t b, c, N, b_p, b_m, c_p, c_m;
	bool left_ulte = true;
	for (int j = 0; j < 3; j++)
		coef[j] = zero;
	if (with_this_var(btor, exp_expr->e[0], exists_var))
	{
		free_expr = btor_node_copy(btor, exp_expr->e[1]);
		get_coefs(btor, exp_expr->e[0], coef);
	}
	else
	{
		free_expr = btor_node_copy(btor, exp_expr->e[0]);
		get_coefs(btor, exp_expr->e[1], coef);
		left_ulte = false;
	}
	for (int j = 0; j < 3; j++)
		l2_coef[j] = l2(btor, coef[j]);
	b = btornode_to_uint64(btor, l2_coef[1]);
	c = btornode_to_uint64(btor, l2_coef[2]);
	int64_t lim = pow(2, bv_size) - 1;
	if (left_ulte)
	{
		b_p = b==lim? 0 : 2*(b + 3);
		b_m = 0;
		c_p = c==lim? 0 : c + 3;
		c_m = 0;
	}
	else
	{
		b_p = 0;
		b_m = b==lim? 0 : 2*(b + 4);
		c_p = 0;
		c_m = c==lim? 0 : c + 4;
	}
	b = max(b_p, b_m);
	c = max(c_p, c_m);
	N = max3(b, c, 2*bv_size);
	BtorNode *const_expr[N + 1];
	const_expr[0] = zero;
	or_expr = replace_exvar(btor, exp_expr, const_expr[0]);
	if (lin->count)
		or_expr = qe_replacement(btor, or_expr, const_expr[0], lin);
	for (int j = 1; j <= N; j++)
	{
		const_expr[j] = uint64_to_btornode(btor, j, bv_size);
		subcase = replace_exvar(btor, exp_expr, const_expr[j]);
		if (lin->count)
			subcase = qe_replacement(btor, subcase, const_expr[j], lin);
		or_expr = btor_exp_bv_or(btor, or_expr, subcase);
	}
	res_expr = or_expr;
	btor_node_release(btor, free_expr);
	return res_expr;
}

BtorNode *
qe_replacement(Btor *btor, BtorNode *exp_expr, BtorNode *value, BtorNodeArray *lin)
{
	BtorNode *and_expr, *eq_expr, *or_expr;
	for (int j = 0; j < lin->count; j++)
	{
		or_expr = replace_exvar(btor, lin->expr[j], value);
		eq_expr = btor_exp_eq(btor, lin->expr[j]->e[0], lin->expr[j]->e[1]);
		eq_expr = replace_exvar(btor, eq_expr, value);
		or_expr = btor_exp_bv_or(btor, or_expr, eq_expr);
		and_expr = j==0 ? or_expr : btor_exp_bv_and(btor, and_expr, or_expr);
	}
	return btor_exp_bv_and(btor, exp_expr, and_expr);
}
