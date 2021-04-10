#ifndef COURSE_STEPS_H
#define COURSE_STEPS_H

#include "btor_tools.h"
extern "C" {
#include "btorabort.h"
}

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;

BtorNode **transform_to_required_form();
void put_in_DNF();
int exvar_occurs_kind(Btor *btor, BtorNode **lin_expr, int *lin_count, BtorNode **exp_expr, int *exp_count);
BtorNode *qe_linear_case(Btor *btor, BtorNode **ulte_expr, int ult_count);

#endif //COURSE_STEPS_H
