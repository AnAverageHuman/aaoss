#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "process.h"

struct process **disks_create(const size_t numdisks) {
  struct process **disks;
  size_t memsize = numdisks * sizeof *disks;
  disks = malloc(memsize);
  return memset(disks, 0, memsize);
}

void disks_destroy(struct process **disks, const size_t numdisks) {
  for (size_t i = 0; i < numdisks; i++) {
    while (disks[i]) {
      process_exit(&disks[i]);
    }
  }

  free(disks);
}

void disk_request(long int disk, const char *filename) {}

void disk_done(long int disk) {}
