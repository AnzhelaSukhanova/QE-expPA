#include <assert.h>
#include "steps.h"

extern "C" {
#include "boolector.h"
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
    int formula_kind = exvar_occurs_kind(btor, ulte_exp, &ult_count);
    if (formula_kind != 0)
    {
        if (formula_kind == 2) {
            //there is an inequation where x occurs in an exponential term
        }

        //x occurs linearly in every inequation occurring in the formula
        if (new_vars[0] != NULL) { //replace x_i by r + k_i + d.φ(d_0).x'_i or by z with 0 <= z < r
            //TODO
        }
        //consider a S-term
        //TODO

        BtorNode *res_exp = qe_linear_case(btor, ult_count, ulte_exp);
        btor_dumpsmt_dump_node(btor, fd_out, res_exp, -1);

        if (ult_count > 1)
            for (int i = 0; i < ult_count; i++)
                btor_node_release(btor, ulte_exp[i]);
        btor_node_release(btor, res_exp);
    }
    else
        BTOR_ABORT(true, "The formula did not transformed to the required form");

    boolector_delete(btor);
    fclose(fd_out);
    return 0;
}
