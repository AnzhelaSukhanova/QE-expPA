#include <assert.h>
#include <stdint.h>
#include "simp_tools.h"
#include "btor_tools.h"

extern "C" {
#include "boolector.h"
#include "btorabort.h"
#include "btorbv.h"
#include "btorexp.h"
#include "dumper/btordumpsmt.h"
};

//#include <typeinfo>
//printf("%s\n", typeid(?).name());

int
main(int argc, char *argv[]) {
    assert(argc > 1);
    Btor *btor = boolector_new();
    char *error_msg;
    int status;
    FILE *fd_in = fopen(argv[1], "r");
    FILE *fd_out = fopen(argv[2], "w");
    boolector_parse_smt2(btor, fd_in, argv[1], fd_out, &error_msg, &status);
    BTOR_ABORT(status, error_msg);

    //1st step: transform the formula into a disjunction of conjunction of inequations between terms of the specific forms
    //TODO
    BtorNode *new_vars = NULL; //vector x
    size_t stack_size = BTOR_COUNT_STACK(btor->nodes_id_table);
    BtorNode *exists_var; //x_0
    int i = 0;
    bool is_exists_var = 0;
    while (i + 1 < stack_size && !is_exists_var)
    {
        i++;
        exists_var = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        if(btor_node_is_param(exists_var))
            is_exists_var = btor_node_param_is_exists_var(exists_var);
    }
    assert(btor_node_param_is_exists_var(exists_var));
    int bv_size = btor_sort_bv_get_width(btor, exists_var->sort_id);

    //2nd step: put the formula in DNF
    //TODO

    //3rd step
    //x occurs linearly in every inequation occurring in the formula
    BtorBitVector *coef = btor_bv_one(btor->mm, bv_size);
    BtorBitVector *LKM = btor_bv_one(btor->mm, bv_size); //d
    int ult_count = 0, eq_count = 0;
    int left_ult_count = 0, right_ult_count = 0; //q and p respectively
    int ulte_qty = (int) stack_size / 8; //approximately
    BtorNode *ult_exp[ulte_qty];
    BtorNode *eq_exp[ulte_qty];
    bool only_exists_var_left[ulte_qty], only_exists_var_right[ulte_qty];
    BtorNode *exp = NULL;
    BtorNode *const_exp = NULL;
    /*for (i = 1; i < stack_size; i++) {
        exp = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        printf("%d %d\n", exp->id, exp->kind);
    }*/
    for (i = 1; i < stack_size; i++) {
        exp = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        //printf("%d %d\n", exp->id, exp->kind);
        if (exp->kind > 3) {
            switch (exp->kind) {
                case 10: //ULT
                    assert(exp->e[0]->kind == 3 || exp->e[0]->kind == 2 || exp->e[0]->kind == 9 ||
                           exp->e[1]->kind == 3 || exp->e[1]->kind == 2 || exp->e[1]->kind == 9);
                    only_exists_var_left[ult_count] = only_this_var(exp->e[0], exp, exists_var);
                    only_exists_var_right[ult_count] = only_this_var(exp->e[1], exp, exists_var);
                    if (only_exists_var_left[ult_count])
                        right_ult_count++;
                    else
                        left_ult_count++;
                    assert(only_exists_var_left[ult_count] && without_this_var(exp->e[1], exists_var) ||
                           only_exists_var_right[ult_count] && without_this_var(exp->e[0], exists_var));
                    ult_exp[ult_count] = exp; //to multiply up to LKM a little bit later
                    ult_count++;
                    break;
                case 6:  //EQ
                    assert(exp->e[0]->kind == 3 || exp->e[0]->kind == 2 || exp->e[0]->kind == 9 ||
                           exp->e[1]->kind == 3 || exp->e[1]->kind == 2 || exp->e[1]->kind == 9);
                    assert(only_this_var(exp->e[0], exp, exists_var) && without_this_var(exp->e[1], exists_var) ||
                           only_this_var(exp->e[1], exp, exists_var) && without_this_var(exp->e[0], exists_var));
                    eq_exp[eq_count] = exp; //to multiply up to LKM a little bit later
                    eq_count++;
                    break;
                case 9: //MUL
                    //printf("%d %d\n", exp->e[0]->kind, exp->e[1]->kind);
                    assert(exp->e[0]->kind == 1 && exp->e[1]->kind == 3 ||
                           exp->e[0]->kind == 3 && exp->e[1]->kind == 1 ||
                           exp->e[0]->kind == 1 && exp->e[1]->kind == 2 ||
                           exp->e[0]->kind == 2 && exp->e[1]->kind == 1);
                    const_exp = exp->e[0]->kind == 1 ? exp->e[0] : exp->e[1];
                    coef = btor_node_is_inverted(const_exp) ? btor_node_bv_const_get_invbits(const_exp)
                                                            : btor_node_bv_const_get_bits(const_exp);
                    LKM = bv_LKM(btor->mm, LKM, coef);
                    break;
                case 5:  //AND
                    assert(exp->e[0]->kind == 3 || exp->e[0]->kind == 10 ||
                           exp->e[1]->kind == 3 || exp->e[1]->kind == 10);
                    break;
                case 18: //EXISTS
                    assert(exp->e[0]->kind == 3 || exp->e[0]->kind == 5 || exp->e[0]->kind == 18 ||
                           exp->e[1]->kind == 3 || exp->e[1]->kind == 5 || exp->e[1]->kind == 18);
                    break;
                default:
                    BTOR_ABORT(true, "The formula did not transformed to the required form");
                    break;
            }
        }
    }
    assert(ult_count == eq_count); //we work with non-strict inequalities
    BtorBitVector *multiplier[ult_count]; //d = d_k * multiplier_k
    for (i = 0; i < ult_count; i++) {
        exp = ult_exp[i];
        if (exp->e[0]->kind == 9 && only_exists_var_left[i] ||
            exp->e[1]->kind == 9 &&
            only_exists_var_right[i]) { //multiply out each inequation in order to get d as the coefficient of x_0
            BtorNode *mul_exp = NULL;
            mul_exp = only_exists_var_left[i] ? exp->e[0] : exp->e[1];
            const_exp = mul_exp->e[0]->kind == 1 ? mul_exp->e[0] : mul_exp->e[1];
            coef = btor_node_is_inverted(const_exp) ? btor_node_bv_const_get_invbits(const_exp)
                                                    : btor_node_bv_const_get_bits(const_exp);
            multiplier[i] = btor_bv_udiv(btor->mm, LKM, coef);
        }
    }
    btor_bv_print(LKM);

    int power = 0; //r
    uint64_t mantis = btor_bv_to_uint64(LKM); //d_0
    while (mantis % 2 == 0) { //decompose d as follows: d = 2^r * d_0
        mantis = mantis / 2;
        power++;
    }
    //printf("%d %ld\n", power, mantis);
    if (new_vars != NULL) { //replace x_i by r + k_i + d.φ(d_0).x'_i or by z with 0 <= z < r
        //TODO
    }
    //consider a S-term
    //TODO
    Btor *res_btor = boolector_new();
    int left_perm_qty = factorial(left_ult_count); //|S_q|
    int left_perm[left_perm_qty][left_ult_count]; //S_q
    int right_perm_qty = factorial(right_ult_count); //|S_p|
    int right_perm[right_perm_qty][right_ult_count]; //S_p
    int j = 0, k = 0;
    for (i = 0; i < ult_count; i++) {
        if (only_exists_var_left[i]) {
            right_perm[0][j] = i;
            j++;
        }
        else {
            left_perm[0][k] = i;
            k++;
        }
    }
    permute(0, left_ult_count, 0, (int *)left_perm);
    permute(0, right_ult_count, 0, (int *)right_perm);

    int index1, index2;
    uint64_t bound = btor_bv_to_uint64(LKM); //d
    BtorNode *const_exps[bound + 1]; //c
    const_exps[bound] = btor_exp_bv_const(res_btor, LKM);
    BtorNode *eq[bound];
    BtorNode *and_exps;
    BtorNode *or_exp;
    BtorNode *exp1;
    BtorNode *res_exps[left_perm_qty][right_perm_qty];
    for (i = 0; i < bound; i++)
        const_exps[i] = btor_exp_bv_const(res_btor, btor_bv_uint64_to_bv(res_btor->mm, i, bv_size));
    printf("%d %d\n", left_ult_count, right_ult_count);
    if (left_ult_count != 0 && right_ult_count != 0) {
        BtorNode *ulte[ult_count + 1];
        for (i = 0; i < left_perm_qty; i++) {
            for (j = 0; j < right_perm_qty; j++) {
                exp = ult_exp[*left_perm[i]]->e[0];
                exp1 = ult_exp[*right_perm[j]]->e[1];
                ulte[0] = btor_node_create_bv_ult(res_btor, exp, exp1);
                for (k = 1; k < right_ult_count; k++) {
                    index1 = right_perm[j][k - 1];
                    index2 = right_perm[j][k];
                    ulte[k] = btor_exp_bv_ulte(res_btor, ult_exp[index1]->e[1], ult_exp[index2]->e[1]);
                }
                for (k = left_ult_count - 1; k > 0; k--) {
                    index1 = left_perm[i][k];
                    index2 = left_perm[i][k - 1];
                    ulte[ult_count + 1 - k] = btor_exp_bv_ulte(res_btor, ult_exp[index1]->e[0], ult_exp[index2]->e[0]);
                }
                BtorNode *dif = btor_exp_bv_sub(res_btor, exp1, exp);
                for (k = 0; k < bound; k++)
                    eq[k] = btor_exp_eq(res_btor, dif, const_exps[k]);
                or_exp = eq[0];
                if (bound != 1) {
                    for (k = 1; k < bound; k++)
                        or_exp = btor_exp_bv_or(res_btor, or_exp, eq[k]);
                }
                or_exp = btor_exp_bv_or(res_btor, btor_exp_bv_ulte(res_btor, const_exps[bound], exp), or_exp);
                if (ult_count > 1) {
                    and_exps = ult_count == 2? ulte[0] : btor_exp_bv_and_n(res_btor, ulte, ult_count);
                    res_exps[i][j] = btor_exp_bv_and(res_btor, and_exps, or_exp);
                }
                else
                    res_exps[i][j] = or_exp;
            }
        }
    }
    else if (left_ult_count != 0) {
        BtorNode *ulte[ult_count];
        for (i = 0; i < left_perm_qty; i++) {
            exp = ult_exp[*left_perm[i]]->e[0];
            for (k = left_ult_count - 1; k > 0; k--) {
                index1 = left_perm[i][k];
                index2 = left_perm[i][k - 1];
                ulte[ult_count - k - 1] = btor_exp_bv_ulte(res_btor, ult_exp[index1]->e[0], ult_exp[index2]->e[0]);
            }
            for (k = 0; k < bound; k++)
                eq[k] = btor_exp_eq(res_btor, exp, const_exps[k]);
            or_exp = eq[0];
            if (bound != 1) {
                for (k = 1; k < bound; k++)
                    or_exp = btor_exp_bv_or(res_btor, or_exp, eq[k]);
            }
            or_exp = btor_exp_bv_or(res_btor, btor_exp_bv_ulte(res_btor, const_exps[bound], exp), or_exp);
            if (ult_count > 1) {
                and_exps = ult_count == 2? ulte[0] : btor_exp_bv_and_n(res_btor, ulte, ult_count - 1);
                res_exps[i][0] = btor_exp_bv_and(res_btor, and_exps, or_exp);
            }
            else
                res_exps[i][0] = or_exp;
        }
    }
    else if (right_ult_count != 0) {
        BtorNode *ulte[ult_count];
        for (j = 0; j < right_ult_count; j++) {
            exp = ult_exp[*right_perm[j]]->e[1];
            //btor_exp_var(res_btor, btor_node_get_sort_id(exp), btor_node_get_symbol(res_btor, exp));
            for (k = 1; k < right_ult_count; k++) {
                index1 = right_perm[j][k - 1];
                index2 = right_perm[j][k];
                ulte[k - 1] = btor_exp_bv_ulte(res_btor, ult_exp[index1]->e[1], ult_exp[index2]->e[1]);
            }
            for (k = 0; k < bound; k++)
                eq[k] = btor_exp_eq(res_btor, exp, const_exps[k]);
            or_exp = eq[0];
            if (bound != 1) {
                for (k = 1; k < bound; k++)
                    or_exp = btor_exp_bv_or(res_btor, or_exp, eq[k]);
            }
            or_exp = btor_exp_bv_or(res_btor, btor_exp_bv_ulte(res_btor, const_exps[bound], exp), or_exp);
            if (ult_count > 1) {
                and_exps = ult_count == 2? ulte[0] : btor_exp_bv_and_n(res_btor, ulte, ult_count - 1);
                res_exps[0][j] = btor_exp_bv_and(res_btor, and_exps, or_exp);
            }
            else
                res_exps[0][j] = or_exp;
        }
    }
    else
        printf("The input formula is incorrect");
    BtorNode *res_exp = res_exps[0][0];
    for (i = 0; i < left_perm_qty; i++) {
        for (j = 0; j < right_perm_qty; j++) {
            res_exp = btor_exp_bv_or(res_btor, res_exp, res_exps[i][j]);
        }
    }
    //btor_simplify_exp(res_btor, res_exp);
    btor_dumpsmt_dump_node(res_btor, fd_out, res_exp, -1);
    //boolector_dump_smt2 (res_btor, fd_out);

    //there is an inequation where x occurs in an exponential term
    //TODO

    boolector_delete (res_btor);
    boolector_delete (btor);
    return 0;
}
