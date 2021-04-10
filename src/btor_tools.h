#ifndef COURSE_BTOR_TOOLS_H
#define COURSE_BTOR_TOOLS_H

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "simp_tools.h"

extern "C" {
#include "btorcore.h"
#include "btorexp.h"
#include "btorbv.h"
};

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;

bool only_this_var(Btor *btor, BtorNode *expr, BtorNode *var);
bool without_this_var(Btor *btor, BtorNode *expr, BtorNode *var);
bool without_vars(Btor *btor, BtorNode *expr);
void printf_exprs_info(Btor *btor);
BtorNode *get_exists_var(Btor *btor);
BtorNode *replace_exvar(Btor *btor, BtorNode *expr, BtorNode *value);
BtorBitVector *get_exp_coef_sum(Btor *btor, BtorNode **exp_expr, int exp_count);
BtorNode *find_exp_by_child_kind(Btor *btor, BtorNode *expr, BtorNodeKind kind);
BtorNode *l2(Btor *btor,  BtorNode *expr);

#endif //COURSE_BTOR_TOOLS_H
