#ifndef COURSE_BTOR_TOOLS_H
#define COURSE_BTOR_TOOLS_H

#include <stdbool.h>
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
extern BtorNode *zero;
extern BtorNode *False;

struct BtorNodeArray
{
	BtorNode **expr;
	BtorNode **left_expr;
	BtorNode **right_expr;
	uint size;
	uint count;
};
typedef struct BtorNodeArray BtorNodeArray;

BtorNodeArray* btornodearr_new(uint size);

//true if only the specified variable appears in the expression
bool only_this_var(Btor *btor, BtorNode *expr, BtorNode *var);

//true if the specified variable doesn't appear in the expression
bool without_this_var(Btor *btor, BtorNode *expr, BtorNode *var);

//true if the specified variable appears in the expression
bool with_this_var(Btor *btor, BtorNode *expr, BtorNode *var);

//true if the expression haven't variables
bool without_vars(Btor *btor, BtorNode *expr);

//debug output
void printf_exprs_info(Btor *btor);

//returns the size of the formula stack
size_t get_stack_size(Btor *btor);

//returns last variable bound by the existential quantifier
BtorNode *get_first_exists_var(Btor *btor);

//bound variable replacement
BtorNode *replace_exvar(Btor *btor, BtorNode *expr, BtorNode *value);

//integer logarithm base 2
BtorNode *l2(Btor *btor,  BtorNode *expr);

//coefficients in exponential, linear and free terms
void get_coefs(Btor *btor,  BtorNode *expr, BtorNode *coef[3]);

BtorBitVector *btornode_to_bv(Btor *btor, BtorNode *expr);

uint64_t btornode_to_uint64(Btor *btor, BtorNode *expr);

BtorNode *uint64_to_btornode(Btor *btor, uint64_t num, int size);

bool same_children(BtorNode *expr1, BtorNode *expr2);

bool is_exvar_exp_term(BtorNode *expr);

BtorNode *resize_expr(Btor *btor, BtorNode *expr, int old_bv_size);

BtorNode *int_sub(Btor *btor, BtorNode *expr1, BtorNode *expr2);

BtorNode *get_rem(Btor *btor, BtorNode *expr, int old_bv_size, int max_power);

uint64_t find_LCM(Btor *btor, BtorNodeArray *lin, uint64_t *coef);

void btor_hashptr_table_forget_first(BtorPtrHashTable *table);

#endif //COURSE_BTOR_TOOLS_H
