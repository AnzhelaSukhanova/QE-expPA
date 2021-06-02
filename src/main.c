#include <assert.h>
#include "steps.h"

extern "C" {
#include "btorparse.h"
};

size_t stack_size;
BtorNode *exists_var;
int bv_size;
BtorNode *False;

int
main(int argc, char *argv[])
{
	assert(argc > 1);
	Btor *btor = btor_new();
	btor_opt_set(btor, BTOR_OPT_OUTPUT_NUMBER_FORMAT, BTOR_OUTPUT_BASE_HEX);
	char *error_msg;
	int status;
	FILE *fd_in = fopen(argv[1], "r");
	FILE *fd_out = fopen(argv[2], "w");
	btor_parse_smt2(btor, fd_in, argv[1], fd_out, &error_msg, &status);
	fclose(fd_in);
	BTOR_ABORT(status, error_msg);
	False = btor_exp_false(btor);

	double in_t, work_t;
	in_t = get_time();

	stack_size = get_stack_size(btor);
	BtorNode *formula, *res_expr, *input;
	/*btor_dumpsmt_dump_node(btor, stdout, BTOR_PEEK_STACK(btor->nodes_id_table, stack_size - 1), -1);
	fprintf(stdout, "\n");*/
	if (btor->quantifiers->count > 0)
	{
		input = (BtorNode *)btor->quantifiers->last->key;
		transform_to_required_form(btor, input);
		formula = (BtorNode *)btor->quantifiers->last->key; 	
		exists_var = get_first_exists_var(btor);
	}
	else
	{
		if (btor->inconsistent)
			res_expr = False;
		else
			res_expr = btor->true_exp;
	}
	while(btor->exists_vars->count != 0)
	{
		bv_size = btor_sort_bv_get_width(btor, btor_node_get_sort_id(exists_var));
		//printf_exprs_info(btor);
		uint expr_num = (int)stack_size/4 + 1; //approximately
		BtorNodeArray *lin = btornodearr_new(expr_num);
		BtorNodeArray *exp = btornodearr_new(expr_num);
		BtorNodeArray *free_vars = btornodearr_new(expr_num);
		bool is_inverted = btor_hashptr_table_get(btor->forall_vars, exists_var) != NULL || btor_node_is_inverted(formula);
		QECaseKind formula_kind = exvar_occurs_kind(btor, formula, lin, exp, free_vars, 0);
		if (formula_kind!=INCORRECT)
		{
			res_expr = NULL;
			BtorNode *expr;
			if (formula_kind==SIMP_EXP)
			{
				BtorNode *ulte_expr1, *ulte_expr2;
				if (exp->count==1)
					res_expr = qe_exp_case(btor, exp->expr[0], lin);
				for (int i = 1; i < exp->count; i += 2)
				{
					if (!same_children(exp->expr[i - 1], exp->expr[i]))
					{
						i--;
						expr = qe_exp_case(btor, exp->expr[i], lin);
					}
					else
					{
						ulte_expr1 = qe_exp_case(btor, exp->expr[i], lin);
						ulte_expr2 = qe_exp_case(btor, exp->expr[i - 1], lin);
						expr = btor_exp_bv_or(btor, ulte_expr1, ulte_expr2);
					}
					res_expr = res_expr==NULL ? expr : btor_exp_bv_and(btor, res_expr, expr);
				}
			}
			else if (formula_kind==SIMP_LIN)
			{
				res_expr = qe_linear_case(btor, lin, 1, 0);
			}
			else if (formula_kind==LINEAR)
			{
				uint64_t coef[lin->count];
				uint64_t LCM = find_LCM(btor, lin, coef);
				int old_bv_size = bv_size;
				bv_size = (l2(LCM) + 1) + bv_size;
				for (int i = 0; i < lin->count; i++)
				{
					lin->expr[i] = resize_expr(btor, lin->expr[i], old_bv_size);
					if (coef[i] != LCM)
					{
						BtorNode* coef_expr = uint64_to_btornode(btor, LCM/coef[i], bv_size);
						int j = without_this_var(btor, lin->expr[i]->e[0], exists_var)? 0 : 1;
						lin->expr[i]->e[j] = btor_exp_bv_mul(btor, lin->expr[i]->e[j], coef_expr);
					}
				}
				res_expr = qe_linear_case(btor, lin, LCM, old_bv_size);
			}
			else
			{
				res_expr = qe_exp_case(btor, exp->expr[0], lin);
			}
			for (int i = 0; i < free_vars->count; i++)
			{
				if (is_inverted)
					res_expr = res_expr == NULL? free_vars->expr[i] : btor_exp_bv_or(btor, res_expr, free_vars->expr[i]);
				else
					res_expr = res_expr == NULL? free_vars->expr[i] : btor_exp_bv_and(btor, res_expr, free_vars->expr[i]);
			}
			if (is_inverted)
				res_expr = btor_node_invert(res_expr);
			btor_hashptr_table_forget_first(btor->exists_vars);
			exists_var = get_first_exists_var(btor);
			if (exists_var != NULL)
				formula = btor_exp_exists(btor, exists_var, res_expr);
			stack_size = get_stack_size(btor);
		}
		else
			BTOR_ABORT(true, "The formula did not transformed to the required form");
	}
	work_t = get_time();
	//printf("Work: %.12lf\n", work_t - in_t);
	btor_dumpsmt_dump_node(btor, fd_out, res_expr, -1);
	fprintf(fd_out, "\n");
	btor_node_release(btor, res_expr);
	btor_delete(btor);
	fclose(fd_out);
	return 0;
}
