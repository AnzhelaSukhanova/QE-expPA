#include <assert.h>
#include <stdint.h>
#include "simp_tools.h"
#include "btor_tools.h"

extern "C" {
#include "boolector.h"
#include "btorabort.h"
#include "btorbv.h"
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
    fclose(fd_in);
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
    int bv_size = btor_sort_bv_get_width(btor, btor_node_get_sort_id(exists_var));

    //2nd step: put the formula in DNF
    //TODO

    //3rd step
    //x occurs linearly in every inequation occurring in the formula
    BtorBitVector *coef = btor_bv_one(btor->mm, bv_size);
    BtorBitVector *LCM = btor_bv_one(btor->mm, bv_size); //d
    int ult_count = 0, eq_count = 0;
    int left_ult_count = 0, right_ult_count = 0; //q and p respectively
    int ulte_num = (int) stack_size / 8 + 1; //approximately
    BtorNode *ulte_exp[ulte_num];
    bool only_exists_var_left[ulte_num], only_exists_var_right[ulte_num];
    BtorNode *exp = NULL;
    BtorNode *const_exp = NULL;
    /*for (i = 1; i < stack_size; i++) {
        exp = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        printf("%d %d\n", btor_node_get_id(exp), exp->kind);
        if (exp->kind > 3) {
            printf("%d %d\n\n", exp->e[0]->kind, exp->e[1]->kind);
        }
    }*/
    for (i = 1; i < stack_size; i++) {
        exp = BTOR_PEEK_STACK(btor->nodes_id_table, i);
        if (exp->kind > 3) {
            switch (exp->kind) {
                case 10: //ULT
                    assert(btor_node_is_param(exp->e[0]) || btor_node_is_bv_var(exp->e[0]) || btor_node_is_bv_mul(exp->e[0]) ||
                           btor_node_is_param(exp->e[1]) || btor_node_is_bv_var(exp->e[1]) || btor_node_is_bv_mul(exp->e[1]));
                    only_exists_var_left[ult_count] = only_this_var(exp->e[0], exp, exists_var);
                    only_exists_var_right[ult_count] = only_this_var(exp->e[1], exp, exists_var);
                    if (only_exists_var_left[ult_count])
                        right_ult_count++;
                    else
                        left_ult_count++;
                    assert(only_exists_var_left[ult_count] && without_this_var(exp->e[1], exists_var) ||
                           only_exists_var_right[ult_count] && without_this_var(exp->e[0], exists_var));
                    ulte_exp[ult_count] = exp; //to multiply up to LCM a little bit later
                    ult_count++;
                    break;
                case 6:  //EQ
                    assert(btor_node_is_param(exp->e[0]) || btor_node_is_bv_var(exp->e[0]) || btor_node_is_bv_mul(exp->e[0]) ||
                           btor_node_is_param(exp->e[1]) || btor_node_is_bv_var(exp->e[1]) || btor_node_is_bv_mul(exp->e[1]));
                    assert(only_this_var(exp->e[0], exp, exists_var) && without_this_var(exp->e[1], exists_var) ||
                           only_this_var(exp->e[1], exp, exists_var) && without_this_var(exp->e[0], exists_var));
                    eq_count++;
                    break;
                case 9: //MUL
                    assert(btor_node_is_bv_const(exp->e[0]) && btor_node_is_param(exp->e[1]) ||
                           btor_node_is_param(exp->e[0]) && btor_node_is_bv_const(exp->e[1]) ||
                           btor_node_is_bv_const(exp->e[0]) && btor_node_is_bv_var(exp->e[1]) ||
                           btor_node_is_bv_var(exp->e[0]) && btor_node_is_bv_const(exp->e[1]));
                    const_exp = btor_node_is_bv_const(exp->e[0]) ? exp->e[0] : exp->e[1];
                    coef = btor_node_is_inverted(const_exp) ? btor_node_bv_const_get_invbits(const_exp)
                                                            : btor_node_bv_const_get_bits(const_exp);
                    LCM = bv_LCM(btor->mm, LCM, coef);
                    break;
                case 5:  //AND
                    assert(btor_node_is_param(exp->e[0]) || btor_node_is_bv_and(exp->e[0]) || btor_node_is_bv_ult(exp->e[0]) ||
                           btor_node_is_param(exp->e[1]) || btor_node_is_bv_and(exp->e[1]) || btor_node_is_bv_ult(exp->e[1]));
                    break;
                case 18: //EXISTS
                    assert(btor_node_is_param(exp->e[0]) || btor_node_is_bv_and(exp->e[0]) || btor_node_is_exists(exp->e[0]) ||
                           btor_node_is_param(exp->e[1]) || btor_node_is_bv_and(exp->e[1]) || btor_node_is_exists(exp->e[1]));
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
        exp = ulte_exp[i];
        if (btor_node_is_bv_mul(exp->e[0]) && only_exists_var_left[i] ||
            btor_node_is_bv_mul(exp->e[1]) &&
            only_exists_var_right[i]) { //multiply out each inequation in order to get d as the coefficient of x_0
            BtorNode *mul_exp = NULL;
            mul_exp = only_exists_var_left[i] ? exp->e[0] : exp->e[1];
            const_exp = btor_node_is_bv_const(mul_exp->e[0]) ? mul_exp->e[0] : mul_exp->e[1];
            coef = btor_node_is_inverted(const_exp) ? btor_node_bv_const_get_invbits(const_exp)
                                                    : btor_node_bv_const_get_bits(const_exp);
            multiplier[i] = btor_bv_udiv(btor->mm, LCM, coef);
            if (!btor_bv_is_one(multiplier[i]))
                if (only_exists_var_left[i])
                {
                    ulte_exp[i] = btor_exp_bv_ult(btor,
                                                 ulte_exp[i]->e[0],
                                                 btor_exp_bv_mul(btor, ulte_exp[i]->e[1], btor_exp_bv_const(btor, multiplier[i])));
                }
                else {
                    ulte_exp[i] = btor_exp_bv_ult(btor,
                                                 btor_exp_bv_mul(btor, ulte_exp[i]->e[0], btor_exp_bv_const(btor, multiplier[i])),
                                                 ulte_exp[i]->e[1]);
                }
        }
    }
    if (btor_bv_is_zero(LCM)) {
        fprintf(fd_out, "false");
        fclose(fd_out);
        return 0;
    }
    btor_bv_print(LCM);

    int power = 0; //r
    uint64_t mantis = btor_bv_to_uint64(LCM); //d_0
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
    int left_perm[left_ult_count]; //σ ∈ S_q
    int right_perm[right_ult_count]; //ρ ∈ S_p
    int k = 0, j = 0;
    for (i = 0; i < ult_count; i++) {
        if (only_exists_var_left[i]) {
            right_perm[j] = i;
            j++;
        }
        else {
            left_perm[k] = i;
            k++;
        }
    }

    int index[2];
    uint64_t bound = btor_bv_to_uint64(LCM); //d
    BtorNode *const_exps[bound + 1]; //c
    const_exps[bound] = btor_exp_bv_const(btor, LCM);
    BtorNode *eq[bound + 1];
    BtorNode *ult;
    BtorNode *and_exps;
    BtorNode *or_exp;
    BtorNode *exp1;
    BtorNode *res_exp;
    for (i = 0; i < bound; i++)
        const_exps[i] = btor_exp_bv_const(btor, btor_bv_uint64_to_bv(btor->mm, i, bv_size));
    printf("%d %d\n", left_ult_count, right_ult_count);
    i = 1, j = 1;
    if (left_ult_count != 0 && right_ult_count != 0) {
        BtorNode *ulte[ult_count - 1];
        do {
            do {
                exp = ulte_exp[left_perm[0]]->e[0];
                exp1 = ulte_exp[right_perm[0]]->e[1];
                ulte[0] = btor_exp_bv_ulte(btor, exp, exp1);
                for (k = 1; k < right_ult_count; k++) {
                    index[0] = right_perm[k - 1];
                    index[1] = right_perm[k];
                    ulte[k] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[1], ulte_exp[index[1]]->e[1]);
                }
                for (k = left_ult_count - 1; k > 0; k--) {
                    index[0] = left_perm[k];
                    index[1] = left_perm[k - 1];
                    ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[0], ulte_exp[index[1]]->e[0]);
                }
                BtorNode *dif = btor_exp_bv_sub(btor, exp1, exp);
                for (k = 0; k <= bound; k++)
                    eq[k] = btor_exp_eq(btor, dif, const_exps[k]);
                or_exp = eq[0];
                for (k = 1; k <= bound; k++)
                    or_exp = btor_exp_bv_or(btor, or_exp, eq[k]);
                ult = btor_exp_bv_ult(btor, const_exps[bound], dif);
                or_exp = btor_exp_bv_or(btor, or_exp, ult);
                if (ult_count > 1) {
                    and_exps = ult_count == 2? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                    res_exp = i + j == 2? btor_exp_bv_and(btor, and_exps, or_exp) :
                            btor_exp_bv_or(btor, res_exp, btor_exp_bv_and(btor, and_exps, or_exp));
                }
                else
                    res_exp = i + j == 2? or_exp : btor_exp_bv_or(btor, res_exp, or_exp);
                j++;
            }
            while (next_permutation(right_perm, right_ult_count));
            i++;
        }
        while (next_permutation(left_perm, left_ult_count));
    }
    else if (left_ult_count != 0) {
        BtorNode *ulte[ult_count];
        do {
            exp = ulte_exp[left_perm[0]]->e[0];
            for (k = left_ult_count - 1; k > 0; k--) {
                index[0] = left_perm[k];
                index[1] = left_perm[k - 1];
                ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[0], ulte_exp[index[1]]->e[0]);
            }
            for (k = 0; k <= bound; k++)
                eq[k] = btor_exp_eq(btor, exp, const_exps[k]);
            or_exp = eq[0];
            for (k = 1; k <= bound; k++)
                or_exp = btor_exp_bv_or(btor, or_exp, eq[k]);
            ult = btor_exp_bv_ult(btor, const_exps[bound], exp);
            or_exp = btor_exp_bv_or(btor, ult, or_exp);
            if (ult_count > 1) {
                and_exps = ult_count == 2? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                res_exp = i == 1? btor_exp_bv_and(btor, and_exps, or_exp) :
                          btor_exp_bv_or(btor, res_exp, btor_exp_bv_and(btor, and_exps, or_exp));
            }
            else
                res_exp = i == 1? or_exp : btor_exp_bv_or(btor, res_exp, or_exp);
            i++;
        }
        while (next_permutation(left_perm, left_ult_count));
    }
    else if (right_ult_count != 0) {
        BtorNode *ulte[ult_count];
        do {
            exp = ulte_exp[right_perm[0]]->e[1];
            for (k = 1; k < right_ult_count; k++) {
                index[0] = right_perm[k - 1];
                index[1] = right_perm[k];
                ulte[k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[1], ulte_exp[index[1]]->e[1]);
            }
            for (k = 0; k <= bound; k++)
                eq[k] = btor_exp_eq(btor, exp, const_exps[k]);
            or_exp = eq[0];
            for (k = 1; k <= bound; k++)
                or_exp = btor_exp_bv_or(btor, or_exp, eq[k]);
            ult = btor_exp_bv_ult(btor, const_exps[bound], exp);
            or_exp = btor_exp_bv_or(btor, ult, or_exp);
            if (ult_count > 1) {
                and_exps = ult_count == 2? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                res_exp = j == 1? btor_exp_bv_and(btor, and_exps, or_exp) :
                          btor_exp_bv_or(btor, res_exp, btor_exp_bv_and(btor, and_exps, or_exp));
            }
            else
                res_exp = j == 1? or_exp : btor_exp_bv_or(btor, res_exp, or_exp);
            j++;
        }
        while (next_permutation(right_perm, right_ult_count));
    }
    else
        printf("The input formula is incorrect");
    btor_dumpsmt_dump_node(btor, fd_out, res_exp, -1);

    //there is an inequation where x occurs in an exponential term
    //TODO

    boolector_delete (btor);
    fclose(fd_out);
    return 0;
}
