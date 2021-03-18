#ifndef COURSE_STEPS_H
#define COURSE_STEPS_H

#include "btor_tools.h"

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;

BtorNode **transform_to_required_form();
void put_in_DNF();
int exvar_occurs_kind(Btor *btor, BtorNode **ulte_exp, int *ult_count);

#endif //COURSE_STEPS_H
