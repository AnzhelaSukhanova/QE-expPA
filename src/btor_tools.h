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
#include "dumper/btordumpsmt.h"
};

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;
extern BtorNode *one;

//true if only the specified variable appears in the expression
bool only_this_var(Btor *btor, BtorNode *expr, BtorNode *var);

//true if the specified variable doesn't appear in the expression
bool without_this_var(Btor *btor, BtorNode *expr, BtorNode *var);

//true if the expression haven't variables
bool without_vars(Btor *btor, BtorNode *expr);

//debug output
void printf_exprs_info(Btor *btor);

//returns the size of the formula stack
size_t get_stack_size(Btor *btor);

//returns variable bound by the existential quantifier
BtorNode *get_exists_var(Btor *btor);

//bound variable replacement
BtorNode *replace_exvar(Btor *btor, BtorNode *expr, BtorNode *value);

//integer logarithm base 2
BtorNode *l2(Btor *btor,  BtorNode *expr);

//coefficients in exponential, linear and free terms
void get_coefs(Btor *btor,  BtorNode *expr, BtorNode *coef[3]);

BtorBitVector *btor_node_to_bv(BtorNode *expr);

bool same_children(BtorNode *expr1, BtorNode *expr2);

#endif //COURSE_BTOR_TOOLS_H
