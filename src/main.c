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
    int ulte_num = (int) stack_size / 8 + 1; //approximately
    BtorNode *ulte_exp[ulte_num];
    int ult_count = 0;
    if (exvar_occurs_linearly(btor, ulte_exp, &ult_count)) { //x occurs linearly in every inequation occurring in the formula
        int left_ult_count = 0, right_ult_count = 0; //q and p respectively
        bool only_exvar_left[ult_count], only_exvar_right[ult_count];
        BtorNode *exp;
        int i;
        for (i = 0; i < ult_count; i++) {
            exp = ulte_exp[i];
            only_exvar_left[i] = only_this_var(btor, exp->e[0], exp, exists_var);
            only_exvar_right[i] = only_this_var(btor, exp->e[1], exp, exists_var);
            if (only_exvar_left[i])
                right_ult_count++;
            else
                left_ult_count++;
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
        BtorNode *and_exp, *exp1, *res_exp, *or_exp;
        i = 1, j = 1;
        if (ult_count > 1) {
            if (left_ult_count != 0 && right_ult_count != 0) {
                BtorNode *ulte[ult_count - 1];
                BtorNode *dif;
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
                        and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                        res_exp = i + j == 2 ? and_exp : btor_exp_bv_or(btor, res_exp, and_exp);
                        j++;
                    } while (next_permutation(right_perm, right_ult_count));
                    i++;
                } while (next_permutation(left_perm, left_ult_count));
            } else if (left_ult_count != 0) {
                BtorNode *ulte[ult_count];
                do {
                    exp = ulte_exp[left_perm[0]]->e[0];
                    for (k = left_ult_count - 1; k > 0; k--) {
                        index[0] = left_perm[k];
                        index[1] = left_perm[k - 1];
                        ulte[ult_count - k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[0],
                                                                   ulte_exp[index[1]]->e[0]);
                    }
                    and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                    res_exp = i == 1 ? and_exp : btor_exp_bv_or(btor, res_exp, and_exp);
                    i++;
                } while (next_permutation(left_perm, left_ult_count));
            } else if (right_ult_count != 0) {
                BtorNode *ulte[ult_count];
                do {
                    exp = ulte_exp[right_perm[0]]->e[1];
                    for (k = 1; k < right_ult_count; k++) {
                        index[0] = right_perm[k - 1];
                        index[1] = right_perm[k];
                        ulte[k - 1] = btor_exp_bv_ulte(btor, ulte_exp[index[0]]->e[1], ulte_exp[index[1]]->e[1]);
                    }
                    and_exp = ult_count == 2 ? ulte[0] : btor_exp_bv_and_n(btor, ulte, ult_count - 1);
                    res_exp = j == 1 ? and_exp : btor_exp_bv_or(btor, res_exp, and_exp);
                    j++;
                } while (next_permutation(right_perm, right_ult_count));
            } else
                BTOR_ABORT(true, "The input formula is incorrect");
        }
        else
            res_exp = btor_exp_true(btor);
        btor_dumpsmt_dump_node(btor, fd_out, res_exp, -1);

        if (ult_count > 1)
        {
            for (i = 0; i < ult_count; i++)
                btor_node_release(btor, ulte_exp[i]);
            btor_node_release(btor, and_exp);
        }
        btor_node_release(btor, res_exp);
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
