#ifndef DISK_H
#define DISK_H

#include <sys/types.h>

struct process **disks_create(const size_t);
void disks_destroy(struct process **, const size_t);

void disk_request(struct process **, struct process **, const long int,
                  const char *);
void disk_done(struct process **, struct process **, long int);

void disk_show(const struct process *, const size_t);

#endif // DISK_H
