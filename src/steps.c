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
                kind = (exp->e[0] == exists_var && without_this_var(btor, exp->e[1], exists_var) ||
                       exp->e[1] == exists_var && without_this_var(btor, exp->e[0], exists_var)) ? 1 : 0;
                ulte_exp[*ult_count] = btor_node_copy(btor, exp); //to multiply up to LCM a little bit later
                (*ult_count)++;
            }
            else if (btor_node_is_bv_eq(exp)) //6
                kind = exp->e[0] == exists_var && without_this_var(btor, exp->e[1], exists_var) ||
                        exp->e[1] == exists_var && without_this_var(btor, exp->e[0], exists_var) ? 1 : 0;
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
            if (kind == 0)
                return 0;
        }
    }
    return kind;
}
