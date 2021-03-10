#include <assert.h>
#include "steps.h"

extern "C" {
#include "boolector.h"
#include "btorabort.h"
#include "btorbv.h"
#include "dumper/btordumpsmt.h"
};

size_t stack_size;
BtorNode *exists_var;
int bv_size;

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

    BtorNode **new_vars = transform_to_required_form(); //vector x
    stack_size = BTOR_COUNT_STACK(btor->nodes_id_table);
    exists_var = get_exists_var(btor); //x_0
    bv_size = btor_sort_bv_get_width(btor, btor_node_get_sort_id(exists_var));
    put_in_DNF();
    //printf_exps_info(btor);

    //3rd step
    BtorBitVector *LCM = btor_bv_one(btor->mm, bv_size); //d
    int ulte_num = (int) stack_size / 8 + 1; //approximately
    BtorNode *ulte_exp[ulte_num];
    int ult_count = 0;
    if (exvar_occurs_linearly(btor, ulte_exp, &ult_count, &LCM)) { //x occurs linearly in every inequation occurring in the formula
        int left_ult_count = 0, right_ult_count = 0; //q and p respectively
        bool only_exvar_left[ult_count], only_exvar_right[ult_count];
        BtorBitVector *multiplier[ult_count]; //d = d_k * multiplier_k
        BtorBitVector *coef = btor_bv_one(btor->mm, bv_size);
        BtorNode *exp, *const_exp;
        int i;
        for (i = 0; i < ult_count; i++) {
            exp = ulte_exp[i];
            only_exvar_left[i] = only_this_var(exp->e[0], exp, exists_var);
            only_exvar_right[i] = only_this_var(exp->e[1], exp, exists_var);
            if (only_exvar_left[i])
                right_ult_count++;
            else
                left_ult_count++;
            //multiply out each inequation in order to get d as the coefficient of x_0
            if (btor_node_is_bv_mul(exp->e[0]) && only_exvar_left[i] ||
                btor_node_is_bv_mul(exp->e[1]) && only_exvar_right[i]) {
                BtorNode *mul_exp;
                mul_exp = only_exvar_left[i] ? exp->e[0] : exp->e[1];
                const_exp = btor_node_is_bv_const(mul_exp->e[0]) ? mul_exp->e[0] : mul_exp->e[1];
                coef = btor_node_is_inverted(const_exp) ? btor_node_bv_const_get_invbits(const_exp)
                                                        : btor_node_bv_const_get_bits(const_exp);
                multiplier[i] = btor_bv_udiv(btor->mm, LCM, coef);
            } else
                multiplier[i] = LCM;
            if (!btor_bv_is_one(multiplier[i]))
                if (only_exvar_left[i]) {
                    ulte_exp[i] = btor_exp_bv_ult(btor,
                                                  ulte_exp[i]->e[0],
                                                  btor_exp_bv_mul(btor, ulte_exp[i]->e[1],
                                                                  btor_exp_bv_const(btor, multiplier[i])));
                } else {
                    ulte_exp[i] = btor_exp_bv_ult(btor,
                                                  btor_exp_bv_mul(btor, ulte_exp[i]->e[0],
                                                                  btor_exp_bv_const(btor, multiplier[i])),
                                                  ulte_exp[i]->e[1]);
                }
        }
        //btor_bv_print(LCM);

        int power = 0; //r
        uint64_t mantis = btor_bv_to_uint64(LCM); //d_0
        while (mantis % 2 == 0) { //decompose d as follows: d = 2^r * d_0
            mantis = mantis / 2;
            power++;
        }
        if (new_vars[0] != NULL) { //replace x_i by r + k_i + d.φ(d_0).x'_i or by z with 0 <= z < r
            //TODO
        }
        //consider a S-term
        //TODO

        int left_perm[left_ult_count]; //σ ∈ S_q
        int right_perm[right_ult_count]; //ρ ∈ S_p
        int k = 0, j = 0;
        for (i = 0; i < ult_count; i++) {
            if (only_exvar_left[i]) {
                right_perm[j] = i;
                j++;
            } else {
                left_perm[k] = i;
                k++;
            }
        }

        int index[2];
        uint64_t bound = btor_bv_to_uint64(LCM); //d
        BtorNode *const_exps[bound + 1]; //c
        const_exps[bound] = btor_exp_bv_const(btor, LCM);
        BtorNode *eq_exp[bound];
        BtorNode *ult, *and_exp, *exp1, *res_exp, *or_exp;
        BtorNode *max = btor_exp_bv_ones(btor, btor_node_get_sort_id(const_exps[bound]));
        for (i = 0; i < bound; i++)
            const_exps[i] = btor_exp_bv_const(btor, btor_bv_uint64_to_bv(btor->mm, i, bv_size));
        i = 1, j = 1;
        bool LCM_is_one = btor_bv_is_one(LCM);
        if (left_ult_count != 0 && right_ult_count != 0) {
            BtorNode *ulte[ult_count - 1];
            BtorNode *mod, *mod_exp, *dif;
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
                        ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[0],
                                                                   ulte_exp[index[1]]->e[0]);
                    }
                    dif = btor_exp_bv_sub(btor, exp1, exp);
                    mod = btor_exp_bv_urem(btor, exp, const_exps[bound]);
                    for (k = 0; k < bound; k++) {
                        eq_exp[k] = btor_exp_eq(btor, dif, const_exps[k]);
                        if (k == 0)
                            mod_exp = btor_exp_eq(btor, mod, const_exps[0]);
                        else
                            mod_exp = btor_exp_bv_or(btor, mod_exp, btor_exp_eq(btor, mod, const_exps[bound - k]));
                        eq_exp[k] = btor_exp_bv_and(btor, eq_exp[k], mod_exp);
                    }
                    or_exp = btor_exp_eq(btor, dif, const_exps[bound]);
                    for (k = 0; k < bound; k++)
                        or_exp = btor_exp_bv_or(btor, or_exp, eq_exp[k]);
                    ult = btor_exp_bv_ult(btor, const_exps[bound], dif);
                    or_exp = btor_exp_bv_or(btor, or_exp, ult);
                    if (ult_count > 1) {
                        and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                        res_exp = i + j == 2 ? btor_exp_bv_and(btor, and_exp, or_exp) :
                                  btor_exp_bv_or(btor, res_exp, btor_exp_bv_and(btor, and_exp, or_exp));
                    } else
                        res_exp = i + j == 2 ? or_exp : btor_exp_bv_or(btor, res_exp, or_exp);
                    j++;
                } while (next_permutation(right_perm, right_ult_count));
                i++;
            } while (next_permutation(left_perm, left_ult_count));
            btor_node_release(btor, mod);
            btor_node_release(btor, mod_exp);
            btor_node_release(btor, dif);
        }
        else if (left_ult_count != 0) {
            BtorNode *ulte[ult_count];
            BtorNode *mod_exp;
            do {
                exp = ulte_exp[left_perm[0]]->e[0];
                for (k = left_ult_count - 1; k > 0; k--) {
                    index[0] = left_perm[k];
                    index[1] = left_perm[k - 1];
                    ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[0],
                                                               ulte_exp[index[1]]->e[0]);
                }
                for (k = 0; k < bound; k++) {
                    eq_exp[k] = btor_exp_eq(btor, exp, const_exps[k]);
                    if (k == 0)
                        mod_exp = btor_exp_eq(btor, exp, const_exps[0]);
                    else
                        mod_exp = btor_exp_bv_or(btor, mod_exp, btor_exp_eq(btor, exp, const_exps[bound - k]));
                    eq_exp[k] = btor_exp_bv_and(btor, eq_exp[k], mod_exp);
                }
                or_exp = btor_exp_eq(btor, exp, const_exps[bound]);
                for (k = 0; k < bound; k++)
                    or_exp = btor_exp_bv_or(btor, or_exp, eq_exp[k]);
                ult = btor_exp_bv_ult(btor, const_exps[bound], exp);
                or_exp = btor_exp_bv_or(btor, ult, or_exp);
                if (ult_count > 1) {
                    and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                    res_exp = i == 1 ? btor_exp_bv_and(btor, and_exp, or_exp) :
                              btor_exp_bv_or(btor, res_exp, btor_exp_bv_and(btor, and_exp, or_exp));
                } else
                    res_exp = i == 1 ? or_exp : btor_exp_bv_or(btor, res_exp, or_exp);
                i++;
            } while (next_permutation(left_perm, left_ult_count));
            btor_node_release(btor, mod_exp);
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
                for (k = 0; k < bound; k++)
                    eq_exp[k] = btor_exp_eq(btor, exp, const_exps[k]);
                or_exp = btor_exp_eq(btor, exp, const_exps[bound]);
                for (k = 0; k < bound; k++)
                    or_exp = btor_exp_bv_or(btor, or_exp, eq_exp[k]);
                ult = btor_exp_bv_ulte(btor, const_exps[bound], exp);
                or_exp = btor_exp_bv_or(btor, ult, or_exp);
                if (ult_count > 1) {
                    and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                    res_exp = j == 1 ? btor_exp_bv_and(btor, and_exp, or_exp) :
                              btor_exp_bv_or(btor, res_exp, btor_exp_bv_and(btor, and_exp, or_exp));
                } else
                    res_exp = j == 1 ? or_exp : btor_exp_bv_or(btor, res_exp, or_exp);
                j++;
            } while (next_permutation(right_perm, right_ult_count));
        }
        else
            printf("The input formula is incorrect");
        btor_dumpsmt_dump_node(btor, fd_out, res_exp, -1);

        for (i = 0; i < ult_count; i++)
            btor_node_release(btor, ulte_exp[i]);
        for (i = 0; i < bound; i++) {
            btor_node_release(btor, eq_exp[i]);
            btor_node_release(btor, const_exps[i]);
        }
        btor_node_release(btor, const_exps[bound]);
        btor_node_release(btor, ult);
        btor_node_release(btor, or_exp);
        if (ult_count > 1)
        {
            btor_node_release(btor, and_exp);
            btor_node_release(btor, res_exp);
        }
        btor_node_release(btor, max);
    }
    else if (exvar_occurs_exponentially())
    {
        //there is an inequation where x occurs in an exponential term
        //TODO
    }
    else
        BTOR_ABORT(true, "The formula did not transformed to the required form");

    boolector_delete(btor);
    fclose(fd_out);
    return 0;
}
