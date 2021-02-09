#include "btor_tools.h"

BtorBitVector *
bv_GCF(BtorMemMgr *mm, BtorBitVector *bv1, BtorBitVector *bv2) //greatest common factor
{
    int32_t compare = btor_bv_compare(bv1, bv2);
    if (compare == 0)
        return bv1;
    BtorBitVector *div = btor_bv_new(mm, btor_bv_get_width(bv1));
    //btor_bv_print(bv1); btor_bv_print(bv2);
    if (compare == -1)
        div = bv_GCF(mm, bv1, btor_bv_sub(mm, bv2, bv1));
    else
        div = bv_GCF(mm, bv2, btor_bv_sub(mm, bv1, bv2));
    return div;
}

BtorBitVector *
bv_LKM(BtorMemMgr *mm, BtorBitVector *bv1, BtorBitVector *bv2) //least common multiple
{
    BtorBitVector *GCF = bv_GCF(mm, bv1, bv2);
    if (btor_bv_is_umulo(mm, bv1, btor_bv_udiv(mm, bv2, GCF)))
        return btor_bv_ones(mm, btor_bv_get_width(bv1));
    else
        return btor_bv_mul(mm, bv1, btor_bv_udiv(mm, bv2, GCF));
}

bool
only_this_var(BtorNode *exp, BtorNode *parent, BtorNode *var)
{
    if (exp->kind > 3) {
        if (!only_this_var(exp->e[0], exp, var) && exp->e[0]->kind != 1) {
            return false;
        }
        return (only_this_var(exp->e[1], exp, var) || exp->e[1]->kind == 1);
    }
    else
        return (exp == var);
}

bool
without_this_var(BtorNode *exp, BtorNode *var)
{
    if (exp->kind > 3) {
        if (!without_this_var(exp->e[0], var))
            return false;
        return without_this_var(exp->e[1], var);
    }
    else
        return (exp != var);
}
