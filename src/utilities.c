#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

long int parseInt(const char *string) {
  char *endptr = NULL;
  long int ret = strtol(string, &endptr, 10);

  if ((errno == ERANGE && (ret == LONG_MAX || ret == LONG_MIN)) ||
      (errno && !ret)) {
    perror("Error while parsing integer value");
    return -1;
  }

  return ret;
}
