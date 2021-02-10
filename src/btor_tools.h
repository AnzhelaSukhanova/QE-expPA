#ifndef COURSE_BTOR_TOOLS_H
#define COURSE_BTOR_TOOLS_H

#include <stdbool.h>

extern "C" {
#include "btorcore.h"
};

BtorBitVector *bv_GCF(BtorMemMgr *mm, BtorBitVector *bv1, BtorBitVector *bv2); //greatest common factor
BtorBitVector *bv_LKM(BtorMemMgr *mm, BtorBitVector *bv1, BtorBitVector *bv2); //least common multiple
bool only_this_var(BtorNode *exp, BtorNode *parent, BtorNode *var);
bool without_this_var(BtorNode *exp, BtorNode *var);
void restore_symbols(Btor *in_btor, Btor *res_btor, BtorNode *exp);

#endif //COURSE_BTOR_TOOLS_H
