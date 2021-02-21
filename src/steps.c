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
is_linearly (Btor *btor, BtorNode **ulte_exp, int *ult_count, BtorBitVector **LCM)
{
    bool is_linearly = true;
    int eq_count = 0;
    BtorNode *exp, *const_exp;
    BtorBitVector *coef = btor_bv_one(btor->mm, bv_size);
    for (int i = 1; i < stack_size; i++) {
        exp = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        if (exp->kind > 3) {
            switch (exp->kind) {
                case 10: //ULT
                    is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_var(exp->e[0]) ||
                                  btor_node_is_bv_mul(exp->e[0]) ||
                                  btor_node_is_param(exp->e[1]) || btor_node_is_bv_var(exp->e[1]) ||
                                  btor_node_is_bv_mul(exp->e[1]);
                    is_linearly =
                            only_this_var(exp->e[0], exp, exists_var) && without_this_var(exp->e[1], exists_var) ||
                            only_this_var(exp->e[1], exp, exists_var) && without_this_var(exp->e[0], exists_var);
                    ulte_exp[*ult_count] = btor_node_copy(btor, exp); //to multiply up to LCM a little bit later
                    (*ult_count)++;
                    break;
                case 6:  //EQ
                    is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_var(exp->e[0]) ||
                                  btor_node_is_bv_mul(exp->e[0]) ||
                                  btor_node_is_param(exp->e[1]) || btor_node_is_bv_var(exp->e[1]) ||
                                  btor_node_is_bv_mul(exp->e[1]);
                    is_linearly =
                            only_this_var(exp->e[0], exp, exists_var) && without_this_var(exp->e[1], exists_var) ||
                            only_this_var(exp->e[1], exp, exists_var) && without_this_var(exp->e[0], exists_var);
                    break;
                case 9: //MUL
                    is_linearly = btor_node_is_bv_const(exp->e[0]) && btor_node_is_param(exp->e[1]) ||
                                  btor_node_is_param(exp->e[0]) && btor_node_is_bv_const(exp->e[1]) ||
                                  btor_node_is_bv_const(exp->e[0]) && btor_node_is_bv_var(exp->e[1]) ||
                                  btor_node_is_bv_var(exp->e[0]) && btor_node_is_bv_const(exp->e[1]);
                    const_exp = btor_node_copy(btor, btor_node_is_bv_const(exp->e[0]) ? exp->e[0] : exp->e[1]);
                    if (exp->e[0] == exists_var || exp->e[1] == exists_var)
                    {
                        coef = btor_node_is_inverted(const_exp) ? btor_node_bv_const_get_invbits(const_exp)
                                                                : btor_node_bv_const_get_bits(const_exp);
                        *LCM = uint_LCM(btor->mm, *LCM, coef);
                    }
                    break;
                case 5:  //AND
                    is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_and(exp->e[0]) ||
                                  btor_node_is_bv_ult(exp->e[0]) ||
                                  btor_node_is_param(exp->e[1]) || btor_node_is_bv_and(exp->e[1]) ||
                                  btor_node_is_bv_ult(exp->e[1]);
                    break;
                case 18: //EXISTS
                    is_linearly = btor_node_is_param(exp->e[0]) || btor_node_is_bv_and(exp->e[0]) ||
                                  btor_node_is_exists(exp->e[0]) ||
                                  btor_node_is_param(exp->e[1]) || btor_node_is_bv_and(exp->e[1]) ||
                                  btor_node_is_exists(exp->e[1]);
                    break;
                default:
                    return false;
                    break;
            }
        }
        if (!is_linearly)
            return false;
    }
    return is_linearly;
}

bool
with_exponential()
{
    //TODO
    return false;
}
