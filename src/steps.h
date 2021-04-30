#ifndef COURSE_STEPS_H
#define COURSE_STEPS_H

#include "btor_tools.h"

extern "C" {
#include "btorabort.h"
}

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;

enum QECaseKind
{
	INCORRECT = 0, //the formula didn't transformed to the required form
	SIMP_LIN, //bound variable occurs trivially in every inequation occurring in the formula
	SIMP_EXP, //bound variable occurs in a trivial exponential term
	LINEAR, //bound variable occurs linearly in every inequation occurring in the formula
	EXPONENTIAL, //bound variable occurs in a non-trivial exponential term
};

typedef enum QECaseKind QECaseKind;

//1st step: transform the formula into a disjunction of conjunction of inequations between terms of the specific forms
BtorNode **transform_to_required_form();

QECaseKind exvar_occurs_kind(Btor *btor, BtorNode **lin_expr, int *lin_count, BtorNode **exp_expr, int *exp_count);

//eliminates quantifiers if bound variable occurs trivially in every inequation occurring in the formula
BtorNode *qe_simp_linear_case(Btor *btor, BtorNode **ulte_expr, int ult_count);

//eliminates quantifiers in an inequation where bound variable occurs trivially in an exponential term
BtorNode *qe_simp_exp_case(Btor *btor, BtorNode *exp_expr, BtorNode **lin_expr, int lin_count);

//eliminates quantifiers in a linear terms for the exponential case
BtorNode *qe_replacement(Btor *btor, BtorNode *exp_expr, BtorNode *value, BtorNode **lin_expr, int lin_count);

//eliminates quantifiers if bound variable occurs linearly in every inequation occurring in the formula
BtorNode *qe_linear_case(Btor *btor, BtorNode **ulte_expr, int ult_count);

//eliminates quantifiers in an inequation where bound variable occurs non trivially in an exponential term
BtorNode *qe_exp_case(Btor *btor, BtorNode *exp_expr, BtorNode **lin_expr, int lin_count);


#endif //COURSE_STEPS_H
