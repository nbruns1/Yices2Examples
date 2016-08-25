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

  code = yices_pp_term(stdout, term, 120, 40, 0);
  if (code < 0) {
    // An error occurred
    fprintf(stderr, "Error in print_term: ");
    yices_print_error(stderr);
    exit(1);
  }
}

void t4()
{
    int32_t code;
    context_t *ctx;
    model_t *mdl;
	ctx = yices_new_context(NULL);
    type_t bv_type = yices_bv_type(32);
    term_t a = yices_new_uninterpreted_term(bv_type);
    term_t b = yices_new_uninterpreted_term(bv_type);
    term_t c = yices_new_uninterpreted_term(bv_type);
	term_t add_a_b = yices_bvadd(a,b);

    yices_pp_term(stdout, add_a_b, 120, 2, 6);
	yices_pp_term(stdout, a, 120, 2, 6);

    term_t add_a_b_c = yices_bvadd(add_a_b,c);
    yices_pp_term(stdout, add_a_b_c, 120, 2, 6);

    term_t hundert = yices_parse_bvbin("00000000000000000000000001100100");
    yices_pp_term(stdout, hundert, 120, 2, 6);
	
    printf("size_a:%i\n",yices_term_bitsize(a));
    printf("size_hundert:%i\n",yices_term_bitsize(hundert));
    //yices_redcomp
	//yices_bveq_atom
    term_t t = yices_bveq_atom(add_a_b_c,hundert);
    yices_pp_term(stdout, t, 120, 2, 6);

    code = yices_assert_formula(ctx, t);
    
  if (code < 0) {
    printf("Assert failed: ");
    yices_print_error(stdout);
    fflush(stdout);
    return;
  }

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
