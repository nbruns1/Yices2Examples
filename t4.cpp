#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "Yices2.hpp"


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

void t4()
{
	Yices2 solver_backend;
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
	solver_backend.assertion(zehn_kleiner_a);

	term_t a_kleiner_zwanzig = yices_bvle_atom(a,zwanzig);
	solver_backend.assertion(a_kleiner_zwanzig);

	term_t add_a_b = yices_bvadd(a,b);
	term_t add_a_b_gleich_c = yices_redcomp(add_a_b,c);
	term_t e = yices_neq(add_a_b_gleich_c,yices_bvconst_zero(1));
	solver_backend.assertion(e);

    term_t add_a_b_c = yices_bvadd(add_a_b,c);
	term_t add_a_b_c_kleiner_zweihundert = yices_bvle_atom(add_a_b_c,zweihundert);
    term_t t = yices_redcomp(add_a_b_c,hundert);
    term_t t0 = yices_neq(t,yices_bvconst_zero(1));
	solver_backend.assertion(t0);

    term_t b_gleich_constant0 = yices_redcomp(b,zero);
    term_t b_gleich_constant1 = yices_neq(b_gleich_constant0,yices_bvconst_zero(1));

	solver_backend.assumption(b_gleich_constant1);

    term_t c_gleich_constant0 = yices_redcomp(c,zero);
    term_t c_gleich_constant1 = yices_neq(c_gleich_constant0,yices_bvconst_zero(1));

	solver_backend.assumption(c_gleich_constant1);
	solver_backend.solve();
    solver_backend.assumption(c_gleich_constant1);
	solver_backend.solve();
	solver_backend.solve();
}


int main(void) {
  t4();
  return 0;
}
