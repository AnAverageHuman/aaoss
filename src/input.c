#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "aaoss.h"

/* print a simple prompt to indicate that we are ready to receive input
 */
void print_prompt() { printf(">> "); }

/* prompts for and returns a line of input from stdin, excluding empty lines
 */
char *get_input() {
  char *input = NULL;
  while (!input) {
    print_prompt();

    if (scanf("%m[^\n]", &input) == EOF) {
      free(input);
      return NULL;
    }

    assert(!scanf("%*c")); // discard newline
  }

  return input;
}
