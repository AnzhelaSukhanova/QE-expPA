#include "steps.h"

//1st step: transform the formula into a disjunction of conjunction of inequations between terms of the specific forms
BtorNode **
transform_to_required_form()
{
    //TODO
    return NULL;
}

//2nd step: put the formula in DNF
void
put_in_DNF()
{
    //TODO
}

int
exvar_occurs_kind(Btor *btor, BtorNode **ulte_exp, int *ult_count)
{
    int kind = 1;
    int eq_count = 0;
    BtorNode *exp;
    for (int i = 1; i < stack_size; i++) {
        exp = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        if (exp && !btor_node_is_bv_const(exp) && !btor_node_is_bv_var(exp) && !btor_node_is_param(exp)) {
            if (btor_node_is_bv_ult(exp)) //10
            {
                kind = without_this_var(btor, exp->e[1], exists_var) || without_this_var(btor, exp->e[0], exists_var);
                if (kind) {
                    if (exp->e[0] == exists_var || exp->e[1] == exists_var)
                    {
                        kind = 1;
                        ulte_exp[*ult_count] = btor_node_copy(btor, exp);
                        (*ult_count)++;
                    }
                    else if (only_this_var(btor, exp->e[0], exists_var) || only_this_var(btor, exp->e[1], exists_var))
                        kind = 2;
                    else kind = 0;
                }
            }
            else if (btor_node_is_bv_eq(exp)) //6
            {
                kind = without_this_var(btor, exp->e[1], exists_var) || without_this_var(btor, exp->e[0], exists_var);
                if (kind) {
                    if (exp->e[0] == exists_var || exp->e[1] == exists_var)
                        kind = 1;
                    else if (only_this_var(btor, exp->e[0], exists_var) || only_this_var(btor, exp->e[1], exists_var))
                        kind = 2;
                    else kind = 0;
                }
            }
            else if (btor_node_is_bv_add(exp)) //8
                kind = without_this_var(btor, exp->e[0], exists_var) && without_this_var(btor, exp->e[1], exists_var) ? kind : 0;
            else if (btor_node_is_bv_mul(exp)) //9
                kind = without_vars(btor, exp->e[0]) && without_this_var(btor, exp->e[1], exists_var) ||
                       without_this_var(btor, exp->e[0], exists_var) && without_vars(btor, exp->e[1]) ? kind : 0;
            else if (btor_node_is_bv_sll(exp)) //11
                kind = only_this_var(btor, exp->e[0], exists_var) && without_this_var(btor, exp->e[1], exists_var) ||
                       only_this_var(btor, exp->e[1], exists_var) && without_this_var(btor, exp->e[0], exists_var) ? 2 : 0;
            else if (btor_node_is_bv_and(exp)) //5
                kind = btor_node_is_param(exp->e[0]) || btor_node_is_bv_and(exp->e[0]) ||
                       btor_node_is_bv_ult(exp->e[0]) ||
                       btor_node_is_param(exp->e[1]) || btor_node_is_bv_and(exp->e[1]) ||
                       btor_node_is_bv_ult(exp->e[1]) ? kind : 0;
            else if (btor_node_is_exists(exp)) //18
                kind = btor_node_is_param(exp->e[0]) && (btor_node_is_param(exp->e[1]) ||
                        btor_node_is_bv_and(exp->e[1]) || btor_node_is_exists(exp->e[1])) ? kind : 0;
            else
                return 0;
            if (kind == 0) {
                printf("id: %d kind: %d\n", btor_node_get_id(exp), exp->kind);
                return 0;
            }
        }
    }
    return kind;
}

BtorNode *
qe_linear_case(Btor *btor, int ult_count, BtorNode **ulte_exp)
{
    int left_ult_count = 0, right_ult_count = 0; //q and p respectively
    bool only_exvar_left[ult_count], only_exvar_right[ult_count];
    BtorNode *exp1;
    int i;
    for (i = 0; i < ult_count; i++) {
        exp1 = ulte_exp[i];
        only_exvar_left[i] = exp1->e[0] == exists_var;
        only_exvar_right[i] = exp1->e[1] == exists_var;
        if (only_exvar_left[i])
            right_ult_count++;
        else
            left_ult_count++;
    }
    int left_perm[left_ult_count]; //σ ∈ S_q
    int right_perm[right_ult_count]; //ρ ∈ S_p
    int k = 0, j = 0;
    for (i = 0; i < ult_count; i++) {
        if (only_exvar_left[i]) {
            right_perm[j] = i;
            j++;
        } else {
            left_perm[k] = i;
            k++;
        }
    }
    int index[2];
    BtorNode *and_exp, *exp2, *res_exp;
    i = 1, j = 1;
    if (ult_count > 1) {
        if (left_ult_count != 0 && right_ult_count != 0) {
            BtorNode *ulte[ult_count - 1];
            BtorNode *dif;
            do {
                do {
                    exp1 = ulte_exp[left_perm[0]]->e[0];
                    exp2 = ulte_exp[right_perm[0]]->e[1];
                    ulte[0] = btor_exp_bv_ulte(btor, exp1, exp2);
                    for (k = 1; k < right_ult_count; k++) {
                        index[0] = right_perm[k - 1];
                        index[1] = right_perm[k];
                        ulte[k] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[1], ulte_exp[index[1]]->e[1]);
                    }
                    for (k = left_ult_count - 1; k > 0; k--) {
                        index[0] = left_perm[k];
                        index[1] = left_perm[k - 1];
                        ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[0],
                                                                   ulte_exp[index[1]]->e[0]);
                    }
                    and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                    res_exp = i + j == 2 ? and_exp : btor_exp_bv_or(btor, res_exp, and_exp);
                    j++;
                } while (next_permutation(right_perm, right_ult_count));
                i++;
            } while (next_permutation(left_perm, left_ult_count));
        } else if (left_ult_count != 0) {
            BtorNode *ulte[ult_count];
            do {
                exp1 = ulte_exp[left_perm[0]]->e[0];
                for (k = left_ult_count - 1; k > 0; k--) {
                    index[0] = left_perm[k];
                    index[1] = left_perm[k - 1];
                    ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[0],
                                                               ulte_exp[index[1]]->e[0]);
                }
                and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                res_exp = i == 1 ? and_exp : btor_exp_bv_or(btor, res_exp, and_exp);
                i++;
            } while (next_permutation(left_perm, left_ult_count));
        } else if (right_ult_count != 0) {
            BtorNode *ulte[ult_count];
            do {
                exp1 = ulte_exp[right_perm[0]]->e[1];
                for (k = 1; k < right_ult_count; k++) {
                    index[0] = right_perm[k - 1];
                    index[1] = right_perm[k];
                    ulte[k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[1], ulte_exp[index[1]]->e[1]);
                }
                and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                res_exp = j == 1 ? and_exp : btor_exp_bv_or(btor, res_exp, and_exp);
                j++;
            } while (next_permutation(right_perm, right_ult_count));
        } else
            BTOR_ABORT(true, "The input formula is incorrect");
    }
    else
        res_exp = btor_exp_true(btor);
    return res_exp;
}
