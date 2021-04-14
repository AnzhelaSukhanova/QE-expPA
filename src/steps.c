#include "steps.h"

BtorNode **
transform_to_required_form()
{
    //TODO
    return NULL;
}

void
put_in_DNF()
{
    //TODO
}

int
exvar_occurs_kind(Btor *btor, BtorNode **lin_expr, int *lin_count, BtorNode **exp_expr, int *exp_count)
{
    int kind = 1;
    int eq_count = 0;
    BtorNode *expr;
    for (int i = 1; i < stack_size; i++)
	{
		expr = BTOR_PEEK_STACK(btor->nodes_id_table, i);
		if (expr && !btor_node_is_bv_const(expr) && !btor_node_is_bv_var(expr) &&
			!btor_node_is_param(expr) && !btor_node_is_bv_slice(expr))
		{
			if (btor_node_is_bv_ult(expr)) //10
			{
				if (without_this_var(btor, expr->e[1], exists_var) || without_this_var(btor, expr->e[0], exists_var))
				{
					if (expr->e[0]==exists_var || expr->e[1]==exists_var)
					{
						lin_expr[*lin_count] = btor_node_copy(btor, expr);
						(*lin_count)++;
					}
					else if (only_this_var(btor, expr->e[0], exists_var) || only_this_var(btor, expr->e[1], exists_var))
					{
						exp_expr[*exp_count] = btor_node_copy(btor, expr);
						(*exp_count)++;
					}
					else
						kind = 0;
				}
			}
			else if (btor_node_is_bv_eq(expr)) //6
			{
				if (without_this_var(btor, expr->e[1], exists_var) || without_this_var(btor, expr->e[0], exists_var))
				{
					if (only_this_var(btor, expr->e[0], exists_var) || only_this_var(btor, expr->e[1], exists_var))
					{
						exp_expr[*exp_count] = btor_node_copy(btor, expr);
						(*exp_count)++;
					}
					else if (expr->e[0]!=exists_var && expr->e[1]!=exists_var)
						kind = 0;
				}
			}
			else if (btor_node_is_bv_add(expr)) //8
				kind = without_this_var(btor, expr->e[0], exists_var) && without_this_var(btor, expr->e[1], exists_var) ? kind : 0;
			else if (btor_node_is_bv_mul(expr)) //9
				kind = without_vars(btor, expr->e[0]) && without_this_var(btor, expr->e[1], exists_var) ||
					without_this_var(btor, expr->e[0], exists_var) && without_vars(btor, expr->e[1]) ? kind : 0;
			else if (btor_node_is_bv_sll(expr)) //11
			{
				kind = only_this_var(btor, expr->e[1], exists_var)? 2 : kind;
				kind = btor_node_is_bv_const(expr->e[0])? kind : 0;
			}
			else if (btor_node_is_bv_and(expr)) //5
				kind = btor_node_is_param(expr->e[0]) || btor_node_is_bv_and(expr->e[0]) ||
					btor_node_is_bv_ult(expr->e[0]) ||
					btor_node_is_param(expr->e[1]) || btor_node_is_bv_and(expr->e[1]) ||
					btor_node_is_bv_ult(expr->e[1]) ? kind : 0;
			else if (!btor_node_is_exists(expr)) //18
				return 0;
			if (kind==0)
				return 0;
		}
	}
    return kind;
}

BtorNode *
qe_linear_case(Btor *btor, BtorNode **ulte_expr, int ult_count)
{
	int left_ult_count = 0, right_ult_count = 0; //q and p respectively
	bool only_exvar_left[ult_count];
	BtorNode *expr1;
	int i;
	for (i = 0; i < ult_count; i++)
	{
		expr1 = ulte_expr[i];
		only_exvar_left[i] = expr1->e[0]==exists_var;
		if (only_exvar_left[i])
			right_ult_count++; //number of expressions where free variables are on the right
		else
			left_ult_count++; //number of expressions where free variables are on the left
	}
	int left_perm[left_ult_count]; //σ ∈ S_q
	int right_perm[right_ult_count]; //ρ ∈ S_p
	int k = 0, j = 0;
	for (i = 0; i < ult_count; i++)
	{
		if (only_exvar_left[i])
		{
			right_perm[j] = i;
			j++;
		}
		else
		{
			left_perm[k] = i;
			k++;
		}
	}
	int index[2];
	BtorNode *and_expr, *expr2, *res_expr;
	i = 1, j = 1;
	if (ult_count > 1)
	{
		if (left_ult_count!=0 && right_ult_count!=0)
		{
			BtorNode *ulte[ult_count - 1];
			do
			{
				do
				{
					expr1 = ulte_expr[left_perm[0]]->e[0];
					expr2 = ulte_expr[right_perm[0]]->e[1];
					ulte[0] = btor_exp_bv_ulte(btor, expr1, expr2);
					for (k = 1; k < right_ult_count; k++)
					{
						index[0] = right_perm[k - 1];
						index[1] = right_perm[k];
						ulte[k] = btor_exp_bv_ulte(btor, ulte_expr[index[0]]->e[1], ulte_expr[index[1]]->e[1]);
					}
					for (k = left_ult_count - 1; k > 0; k--)
					{
						index[0] = left_perm[k];
						index[1] = left_perm[k - 1];
						ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_expr[index[0]]->e[0],
																   ulte_expr[index[1]]->e[0]);
					}
					and_expr = ult_count==2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
					res_expr = i + j==2 ? and_expr : btor_exp_bv_or(btor, res_expr, and_expr);
					j++;
				}
				while (next_permutation(right_perm, right_ult_count));
				i++;
			}
			while (next_permutation(left_perm, left_ult_count));
		}
		else if (left_ult_count!=0)
		{
			BtorNode *ulte[ult_count];
			do
			{
				expr1 = ulte_expr[left_perm[0]]->e[0];
				for (k = left_ult_count - 1; k > 0; k--)
				{
					index[0] = left_perm[k];
					index[1] = left_perm[k - 1];
					ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_expr[index[0]]->e[0],
															   ulte_expr[index[1]]->e[0]);
				}
				and_expr = ult_count==2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
				res_expr = i==1 ? and_expr : btor_exp_bv_or(btor, res_expr, and_expr);
				i++;
			}
			while (next_permutation(left_perm, left_ult_count));
		}
		else if (right_ult_count!=0)
		{
			BtorNode *ulte[ult_count];
			do
			{
				expr1 = ulte_expr[right_perm[0]]->e[1];
				for (k = 1; k < right_ult_count; k++)
				{
					index[0] = right_perm[k - 1];
					index[1] = right_perm[k];
					ulte[k - 1] = btor_exp_bv_ulte(btor, ulte_expr[index[0]]->e[1], ulte_expr[index[1]]->e[1]);
				}
				and_expr = ult_count==2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
				res_expr = j==1 ? and_expr : btor_exp_bv_or(btor, res_expr, and_expr);
				j++;
			}
			while (next_permutation(right_perm, right_ult_count));
		}
		else
			BTOR_ABORT(true, "The input formula is incorrect");
	}
	else
		res_expr = btor_exp_true(btor);
	return res_expr;
}
