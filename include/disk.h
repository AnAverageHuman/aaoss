#ifndef DISK_H
#define DISK_H

#include <sys/types.h>

struct process **disks_create(const size_t);
void disks_destroy(struct process **, const size_t);

void disk_request(struct process **, struct process **, const long int,
                  const char *);
void disk_done(const long int);

#endif // DISK_H
