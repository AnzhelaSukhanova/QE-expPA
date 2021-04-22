#ifndef COURSE_STEPS_H
#define COURSE_STEPS_H

#include "btor_tools.h"

extern "C" {
#include "btorabort.h"
}

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;

//1st step: transform the formula into a disjunction of conjunction of inequations between terms of the specific forms
BtorNode **transform_to_required_form();

//returns 0 if the formula didn't transformed to the required for
//        1 if bound variable occurs linearly in every inequation occurring in the formula
//        2 if there is an inequation where bound var occurs in an exponential term
int exvar_occurs_kind(Btor *btor, BtorNode **lin_expr, int *lin_count, BtorNode **exp_expr, int *exp_count);

//eliminates quantifiers if bound variable occurs linearly in every inequation occurring in the formula
BtorNode *qe_linear_case(Btor *btor, BtorNode **ulte_expr, int ult_count);

//eliminates quantifiers in an inequation where bound var occurs in an exponential term
BtorNode *qe_exp_case(Btor *btor, BtorNode *exp_expr, BtorNode **lin_expr, int lin_count);

BtorNode *qe_replacement(Btor *btor, BtorNode *exp_expr, BtorNode *value, BtorNode **lin_expr, int lin_count);

#endif //COURSE_STEPS_H
