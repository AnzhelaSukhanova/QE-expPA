#ifndef COURSE_STEPS_H
#define COURSE_STEPS_H

#include "btor_tools.h"

extern "C" {
#include "btorabort.h"
}

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;
extern BtorNode *False;

enum QECaseKind
{
	INCORRECT = 0, //the formula didn't transformed to the required form
	SIMP_LIN, //bound variable occurs trivially in every inequation occurring in the formula
	LINEAR, //bound variable occurs linearly in every inequation occurring in the formula
	SIMP_EXP, //bound variable occurs in a trivial exponential term
	EXPONENTIAL, //bound variable occurs in a non-trivial exponential term
};

typedef enum QECaseKind QECaseKind;

QECaseKind max(QECaseKind a, QECaseKind b);

//1st step: transform the formula into a disjunction of conjunction of inequations between terms of the specific forms
void transform_to_required_form(Btor *btor, BtorNode *formula);

QECaseKind exvar_occurs_kind(Btor *btor, BtorNode *formula, BtorNodeArray *lin, BtorNodeArray *exp, BtorNodeArray *free_vars, int inv_depth);

//eliminates quantifiers if bound variable occurs linearly in every inequation occurring in the formula
BtorNode *qe_linear_case(Btor *btor, BtorNodeArray *ult, uint64_t LCM, int old_bv_size);

//eliminates quantifiers in an inequation where bound variable occurs in an exponential term
BtorNode *qe_exp_case(Btor *btor, BtorNode *exp_expr, BtorNodeArray *lin);

//eliminates quantifiers in a linear terms for the exponential case
BtorNode *qe_replacement(Btor *btor, BtorNode *exp_expr, BtorNode *value, BtorNodeArray *lin);

#endif //COURSE_STEPS_H
