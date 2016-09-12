#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "yices.h"


/*
 * How to call the pretty printer
 * - this also shows how to check for errors
 *   and print an error message if something goes wrong
 */
static void print_term(term_t term) {
  int32_t code;

  code = yices_pp_term(stdout, term, 8000, 1, 0);
  if (code < 0) {
    // An error occurred
    fprintf(stderr, "Error in print_term: ");
    yices_print_error(stderr);
    exit(1);
  }
}

void solve(context_t *ctx)
{
int32_t code;
model_t *mdl;
switch (yices_check_context(ctx, NULL)) { // NULL means default heuristics
  case STATUS_SAT:
    // build the model and print it
    printf("Satisfiable\n");
    mdl = yices_get_model(ctx, true);
    code = yices_pp_model(stdout, mdl, 120, 100, 0);
    if (code < 0) {
      printf("Print model failed: ");
      yices_print_error(stdout);
      fflush(stdout);
      return;
    }
    // cleanup: delete the model
    yices_free_model(mdl);
    break;

  case STATUS_UNSAT:
    printf("Unsatisfiable\n");
    break;

  case STATUS_UNKNOWN:
    printf("Status is unknown\n");
    break;

  case STATUS_IDLE:
  case STATUS_SEARCHING:
  case STATUS_INTERRUPTED:
  case STATUS_ERROR:
    // these codes should not be returned
    printf("Bug: unexpected status returned\n");
    break;
  }
}

void t4()
{
    int32_t code;
    context_t *ctx;
	ctx = yices_new_context(NULL);
    type_t bv_type = yices_bv_type(32);
    term_t a = yices_new_uninterpreted_term(bv_type);
    term_t b = yices_new_uninterpreted_term(bv_type);
    term_t c = yices_new_uninterpreted_term(bv_type);
    term_t zero = yices_bvconst_int32(32, 0);
	term_t zehn = yices_bvconst_int32(32, 10);
	term_t zwanzig = yices_bvconst_int32(32, 20);
	term_t zweihundert = yices_bvconst_int32(32, 200);
	term_t hundert = yices_bvconst_int32(32, 100);

	term_t zehn_kleiner_a = yices_bvle_atom(zehn,a);
	code = yices_assert_formula(ctx, zehn_kleiner_a);

    if (code < 0) {
    printf("Assert0 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

	term_t a_kleiner_zwanzig = yices_bvle_atom(a,zwanzig);
	code = yices_assert_formula(ctx, a_kleiner_zwanzig);

    if (code < 0) {
    printf("Assert1 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

	term_t add_a_b = yices_bvadd(a,b);
	term_t add_a_b_gleich_c = yices_redcomp(add_a_b,c);
	term_t e = yices_neq(add_a_b_gleich_c,yices_bvconst_zero(1));
	code = yices_assert_formula(ctx, e);

    if (code < 0) {
    printf("Assert2 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }
    term_t add_a_b_c = yices_bvadd(add_a_b,c);
	term_t add_a_b_c_kleiner_zweihundert = yices_bvle_atom(add_a_b_c,zweihundert);
    term_t t = yices_redcomp(add_a_b_c,hundert);
    term_t t0 = yices_neq(t,yices_bvconst_zero(1));
    code = yices_assert_formula(ctx, t0);
    
    if (code < 0) {
    printf("Assert3 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

	if(yices_push(ctx) != 0)
	{
    printf("push0 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

    term_t b_gleich_constant0 = yices_redcomp(b,zero);
    term_t b_gleich_constant1 = yices_neq(b_gleich_constant0,yices_bvconst_zero(1));

    code = yices_assert_formula(ctx, b_gleich_constant1);
    
    if (code < 0) {
    printf("Assert4 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

    term_t c_gleich_constant0 = yices_redcomp(b,zero);
    term_t c_gleich_constant1 = yices_neq(b_gleich_constant0,yices_bvconst_zero(1));

    code = yices_assert_formula(ctx, c_gleich_constant1);
    
    if (code < 0) {
    printf("Assert5 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

	solve(ctx);
	
	if(yices_pop(ctx) != 0)
	{
    printf("pop0 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

	if(yices_push(ctx) != 0)
	{
    printf("push1 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

    code = yices_assert_formula(ctx, c_gleich_constant1);
    
    if (code < 0) {
    printf("Assert6 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

	solve(ctx);

	if(yices_pop(ctx) != 0)
	{
    printf("pop1 failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
    }

	solve(ctx);
	

  /*
   * Delete the context: this is optional since yices_exit
   * would do it anyway.
   */
  yices_free_context(ctx);
}


int main(void) {
  yices_init();    // Always call this first
  t4();
  yices_exit();    // Cleanup 

  return 0;
}
