#include "btor_tools.h"

bool
only_this_var(BtorNode *exp, BtorNode *parent, BtorNode *var)
{
    if (exp->kind > 3) {
        if (!only_this_var(exp->e[0], exp, var) && !btor_node_is_bv_const(exp->e[0])) {
            return false;
        }
        return (only_this_var(exp->e[1], exp, var) || btor_node_is_bv_const(exp->e[1]));
    } else
        return (exp == var);
}

bool
without_this_var(BtorNode *exp, BtorNode *var)
{
    if (exp->kind > 3) {
        if (!without_this_var(exp->e[0], var))
            return false;
        return without_this_var(exp->e[1], var);
    } else
        return (exp != var);
}

void
printf_exps_info(Btor *btor)
{
    BtorNode *exp;
    for (int i = 1; i < stack_size; i++) {
        exp = btor_node_copy(btor, BTOR_PEEK_STACK(btor->nodes_id_table, i));
        printf("id: %d kind: %d\n", btor_node_get_id(exp), exp->kind);
        if (exp->kind > 3) {
            printf("kinds of children: %d %d\n\n", exp->e[0]->kind, exp->e[1]->kind);
        } else
            printf("\n");
    }
}

BtorNode*
get_exists_var(Btor *btor)
{
    int i = 0;
    bool is_exists_var = 0;
    while (i + 1 < stack_size && !is_exists_var) {
        i++;
        exists_var = btor_node_copy(btor, BTOR_PEEK_STACK(btor->nodes_id_table, i));
        if (btor_node_is_param(exists_var))
            is_exists_var = btor_node_param_is_exists_var(exists_var);
    }
    assert(btor_node_param_is_exists_var(exists_var));
    return exists_var;
}
