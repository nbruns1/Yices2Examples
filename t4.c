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

  code = yices_pp_term(stdout, term, 80, 20, 0);
  if (code < 0) {
    // An error occurred
    fprintf(stderr, "Error in print_term: ");
    yices_print_error(stderr);
    exit(1);
  }
}


int main(void) {
  yices_init();    // Always call this first
  yices_exit();    // Cleanup 

  return 0;
}
