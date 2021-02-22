#ifndef COURSE_STEPS_H
#define COURSE_STEPS_H

#include "btor_tools.h"

extern size_t stack_size;
extern BtorNode *exists_var;
extern int bv_size;

BtorNode **transform_to_required_form();
void put_in_DNF();
bool exvar_occurs_linearly(Btor *btor, BtorNode **ulte_exp, int *ult_count, BtorBitVector **LCM);
bool exvar_occurs_exponentially();

#endif //COURSE_STEPS_H
