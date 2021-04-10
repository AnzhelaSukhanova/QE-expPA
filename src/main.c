#include <assert.h>
#include "steps.h"

extern "C" {
#include "boolector.h"
#include "dumper/btordumpsmt.h"
};

size_t stack_size;
BtorNode *exists_var;
int bv_size;

int
main(int argc, char *argv[])
{
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
	//printf_exprs_info(btor);

	//3rd step
	int ulte_num = (int)stack_size/8 + 1; //approximately
	BtorNode *lin_expr[ulte_num], *exp_expr[ulte_num];
	int lin_count = 0, exp_count = 0, i;
	int formula_kind = exvar_occurs_kind(btor, lin_expr, &lin_count, exp_expr, &exp_count);
	if (formula_kind!=0)
	{
		BtorNode *res_expr;
		if (formula_kind==2)
		{
			int N = 0; //N = max{b+ , c+ , b− , c−} but now exp-expr hasn't linear and free terms
			BtorNode *l2_expr[2], *free_expr, *one, *case_expr[4], *const_expr[N + 1], *or_expr;
			BtorBitVector *one_bv = btor_bv_one(btor->mm, bv_size);
			BtorBitVector *exp_coef_sum = get_exp_coef_sum(btor, exp_expr, exp_count);
			uint64_t power = btor_bv_to_uint64(btor_bv_srl(btor->mm, exp_coef_sum, one_bv)) + 3; //δ = l2(Sum_i(|ai|)) + 3
			one = btor_exp_bv_one(btor, 2);
			for (i = 0; i < exp_count; i++)
			{
				if (only_this_var(btor, exp_expr[i]->e[0], exists_var))
					free_expr = btor_node_copy(btor, exp_expr[i]->e[1]);
				else
					free_expr = btor_node_copy(btor, exp_expr[i]->e[0]);
				l2_expr[0] = l2(btor, free_expr);
				btor_dumpsmt_dump_node(btor, fd_out, l2_expr[0], -1);
				fprintf(fd_out, "\n");
				l2_expr[1] = btor_exp_bv_add(btor, l2_expr[0], one);
				case_expr[0] = replace_exvar(btor, exp_expr[i], l2_expr[0]);
				case_expr[1] = replace_exvar(btor, exp_expr[i], l2_expr[1]);
				const_expr[0] = btor_exp_bv_zero(btor, 2);
				case_expr[2] = replace_exvar(btor, exp_expr[i], const_expr[0]);
				BtorBitVector *bv;
				for (int j = 1; j <= N; j++)
				{
					bv = btor_bv_uint64_to_bv(btor->mm, j, bv_size);
					const_expr[j] = btor_exp_bv_const(btor, bv);
					case_expr[2] = btor_exp_bv_or(btor, case_expr[2], replace_exvar(btor, exp_expr[i], const_expr[j]));
				}
				exp_expr[i] = case_expr[0];
				for (int j = 1; j < 3; j++)
					exp_expr[i] = btor_exp_bv_or(btor, exp_expr[i], case_expr[j]);
				if (i%2==1)
				{
					or_expr = btor_exp_bv_or(btor, exp_expr[i], exp_expr[i - 1]);
					res_expr = i==1? or_expr : btor_exp_bv_and(btor, res_expr, or_expr);
				}
				else if (i!=0)
					res_expr = btor_exp_bv_and(btor, res_expr, or_expr);
				for (int j = 1; j < 3; j++)
					btor_node_release(btor, case_expr[j]);
				btor_node_release(btor, l2_expr[1]);
				btor_node_release(btor, l2_expr[0]);
				btor_node_release(btor, free_expr);
			}
			if (lin_count)
				res_expr = btor_exp_bv_or(btor, res_expr, qe_linear_case(btor, lin_expr, lin_count));
		}
		else
		{
			//x occurs linearly in every inequation occurring in the formula
			res_expr = qe_linear_case(btor, lin_expr, lin_count);
		}
		btor_dumpsmt_dump_node(btor, fd_out, res_expr, -1);
		btor_node_release(btor, res_expr);
		if (lin_count)
			for (i = 0; i < lin_count; i++)
				btor_node_release(btor, lin_expr[i]);
		if (exp_count)
			for (i = 0; i < exp_count; i++)
				btor_node_release(btor, exp_expr[i]);
	}
	else
		BTOR_ABORT(true, "The formula did not transformed to the required form");

	boolector_delete(btor);
	fclose(fd_out);
	return 0;
}
