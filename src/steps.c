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

bool
exvar_occurs_linearly(Btor *btor, BtorNode **ulte_exp, int *ult_count)
{
    bool is_linearly = true;
    int eq_count = 0;
    BtorNode *exp;
    for (int i = 1; i < stack_size; i++) {
        exp = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        if (!btor_node_is_bv_const(exp) && !btor_node_is_bv_var(exp) && !btor_node_is_param(exp)) {
            if (btor_node_is_bv_ult(exp)) {
                is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_var(exp->e[0]) ||
                              btor_node_is_bv_mul(exp->e[0]) || btor_node_is_bv_add(exp->e[0]) ||
                              btor_node_is_param(exp->e[1]) || btor_node_is_bv_var(exp->e[1]) ||
                              btor_node_is_bv_mul(exp->e[1]) || btor_node_is_bv_add(exp->e[1]);
                is_linearly =
                        only_this_var(exp->e[0], exp, exists_var) && without_this_var(exp->e[1], exists_var) ||
                        only_this_var(exp->e[1], exp, exists_var) && without_this_var(exp->e[0], exists_var);
                ulte_exp[*ult_count] = btor_node_copy(btor, exp); //to multiply up to LCM a little bit later
                (*ult_count)++;
            } else if (btor_node_is_bv_eq(exp)) {
                is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_var(exp->e[0]) ||
                              btor_node_is_bv_mul(exp->e[0]) || btor_node_is_bv_add(exp->e[0]) ||
                              btor_node_is_param(exp->e[1]) || btor_node_is_bv_var(exp->e[1]) ||
                              btor_node_is_bv_mul(exp->e[1]) || btor_node_is_bv_add(exp->e[1]);
                is_linearly =
                        only_this_var(exp->e[0], exp, exists_var) && without_this_var(exp->e[1], exists_var) ||
                        only_this_var(exp->e[1], exp, exists_var) && without_this_var(exp->e[0], exists_var);
            } else if (btor_node_is_bv_add(exp)) {
                is_linearly = (btor_node_is_bv_const(exp->e[0]) || btor_node_is_bv_var(exp->e[0]) ||
                              btor_node_is_bv_mul(exp->e[0])) && without_this_var(exp->e[0], exists_var) ||
                              btor_node_is_bv_const(exp->e[1]) || btor_node_is_bv_var(exp->e[1]) ||
                              btor_node_is_bv_mul(exp->e[1]) && without_this_var(exp->e[0], exists_var);
            } else if (btor_node_is_bv_mul(exp)) {
                is_linearly = btor_node_is_bv_const(exp->e[0]) && btor_node_is_bv_var(exp->e[1]) ||
                              btor_node_is_bv_var(exp->e[0]) && btor_node_is_bv_const(exp->e[1]);
            } else if (btor_node_is_bv_and(exp)) {
                is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_and(exp->e[0]) ||
                              btor_node_is_bv_ult(exp->e[0]) ||
                              btor_node_is_param(exp->e[1]) || btor_node_is_bv_and(exp->e[1]) ||
                              btor_node_is_bv_ult(exp->e[1]);
            } else if (btor_node_is_exists(exp)) {
                is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_and(exp->e[0]) ||
                              btor_node_is_exists(exp->e[0]) ||
                              btor_node_is_param(exp->e[1]) || btor_node_is_bv_and(exp->e[1]) ||
                              btor_node_is_exists(exp->e[1]);
            } else
                return false;
        }
        if (!is_linearly)
            return false;
    }
    return is_linearly;
}

bool
exvar_occurs_exponentially()
{
    //TODO
    return false;
}
