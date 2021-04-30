#include <assert.h>
#include "steps.h"

extern "C" {
#include "boolector.h"
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
	stack_size = get_stack_size(btor);

	exists_var = get_exists_var(btor); //x_0
	if (exists_var == NULL)
	{
		if (btor->inconsistent)
			btor_dumpsmt_dump_node(btor, fd_out, btor_exp_false(btor), -1);
		else
			btor_dumpsmt_dump_node(btor, fd_out, btor_exp_true(btor), -1);
		fprintf(fd_out, "\n");
		boolector_delete(btor);
		fclose(fd_out);
		return 0;
	}
	bv_size = btor_sort_bv_get_width(btor, btor_node_get_sort_id(exists_var));
	//printf_exprs_info(btor);

	//3rd step
	int expr_num = (int)stack_size/4 + 1; //approximately
	BtorNode *lin_expr[expr_num], *exp_expr[expr_num];
	int lin_count = 0, exp_count = 0, i;
	QECaseKind formula_kind = exvar_occurs_kind(btor, lin_expr, &lin_count, exp_expr, &exp_count);
	if (formula_kind != INCORRECT)
	{
		BtorNode *res_expr, *or_expr;
		if (formula_kind==SIMP_EXP)
		{
			BtorNode *ulte_expr1, *ulte_expr2;
			if (exp_count == 1)
				res_expr = qe_simp_exp_case(btor, exp_expr[0], lin_expr, lin_count);
			for (i = 1; i < exp_count; i+=2)
			{
				if (exp_expr[i-1] == exp_expr[i])
					or_expr = qe_simp_exp_case(btor, exp_expr[i], lin_expr, lin_count);
				else
				{
					ulte_expr1 = qe_simp_exp_case(btor, exp_expr[i], lin_expr, lin_count);
					ulte_expr2 = qe_simp_exp_case(btor, exp_expr[i - 1], lin_expr, lin_count);
					or_expr = btor_exp_bv_or(btor, ulte_expr1, ulte_expr2);
				}
				res_expr = i==1 ? or_expr : btor_exp_bv_and(btor, res_expr, or_expr);
			}
		}
		else if (formula_kind==SIMP_LIN)
		{
			res_expr = qe_simp_linear_case(btor, lin_expr, lin_count);
		}
		else if (formula_kind==LINEAR)
		{
			uint64_t LCM = 1;
			BtorNode *mul, *coef;
			BtorBitVector *bv_coef;
			for(i = 0; i < lin_count; i++)
			{
				 mul = without_this_var(btor, lin_expr[i]->e[0], exists_var)? lin_expr[i]->e[1] : lin_expr[i]->e[0];
				 coef = btor_node_is_bv_const(mul->e[0])? btor_node_copy(btor, mul->e[0]) : btor_node_copy(btor, mul->e[1]);
				 bv_coef = btor_node_to_bv(coef);
				 LCM = lcm(LCM, btor_bv_to_uint64(bv_coef));
			}

			res_expr = qe_linear_case(btor, lin_expr, lin_count);
		}
		else
		{
			res_expr = qe_exp_case(btor, exp_expr[0], lin_expr, lin_count);
		}
		btor_dumpsmt_dump_node(btor, fd_out, res_expr, -1);
		fprintf(fd_out, "\n");
		btor_node_release(btor, res_expr);
		if (lin_count)
			for (i = 0; i < lin_count; i++)
				btor_node_release(btor, lin_expr[i]);
	}
	else
		BTOR_ABORT(true, "The formula did not transformed to the required form");
	boolector_delete(btor);
	fclose(fd_out);
	return 0;
}
