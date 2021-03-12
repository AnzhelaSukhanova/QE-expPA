#ifndef COURSE_BTOR_TOOLS_H
#define COURSE_BTOR_TOOLS_H

#include <stdbool.h>
#include "simp_tools.h"

extern "C" {
#include "btorcore.h"
#include "btorexp.h"
};

extern size_t stack_size;
extern BtorNode *exists_var;

bool only_this_var(BtorNode *exp, BtorNode *parent, BtorNode *var);
bool without_this_var(BtorNode *exp, BtorNode *var);
void printf_exps_info(Btor *btor);
BtorNode *get_exists_var(Btor *btor);

#endif //COURSE_BTOR_TOOLS_H
