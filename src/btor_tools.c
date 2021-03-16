#include "btor_tools.h"

bool
only_this_var(Btor *btor, BtorNode *exp, BtorNode *parent, BtorNode *var)
{
    if (!btor_node_is_bv_const(exp) && !btor_node_is_bv_var(exp) && !btor_node_is_param(exp))
    {
        uint32_t id = btor_node_get_id(exp->e[0]);
        BtorNode *child_exp = BTOR_PEEK_STACK(btor->nodes_id_table, id);
        if (!only_this_var(btor, child_exp, exp, var) && !btor_node_is_bv_const(child_exp)) {
            return false;
        }
        id = btor_node_get_id(exp->e[1]);
        child_exp = BTOR_PEEK_STACK(btor->nodes_id_table, id);
        return (only_this_var(btor, child_exp, exp, var) || btor_node_is_bv_const(child_exp));
    }
    else
        return (exp == var);
}

bool
without_this_var(Btor *btor, BtorNode *exp, BtorNode *var)
{
    if (!btor_node_is_bv_const(exp) && !btor_node_is_bv_var(exp) && !btor_node_is_param(exp))
    {
        uint32_t id = btor_node_get_id(exp->e[0]);
        BtorNode *child_exp = BTOR_PEEK_STACK(btor->nodes_id_table, id);
        if (!without_this_var(btor, exp->e[0], var))
            return false;
        /*id = btor_node_get_id(exp->e[1]); ??
        child_exp = BTOR_PEEK_STACK(btor->nodes_id_table, id); ??*/
        return without_this_var(btor, exp->e[1], var);
    } else
        return (exp != var);
}

bool
without_vars(Btor *btor, BtorNode *exp)
{
    if (btor_node_is_bv_const(exp))
        return true;
    else if (!btor_node_is_bv_var(exp) && !btor_node_is_param(exp))
    {
        uint32_t id = btor_node_get_id(exp->e[0]);
        BtorNode *child_exp = BTOR_PEEK_STACK(btor->nodes_id_table, id);
        if (!without_vars(btor, child_exp))
            return false;
        id = btor_node_get_id(exp->e[1]);
        child_exp = BTOR_PEEK_STACK(btor->nodes_id_table, id);
        return without_vars(btor, child_exp);
    }
    else
        return false;
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
