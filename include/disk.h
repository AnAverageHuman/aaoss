#ifndef DISK_H
#define DISK_H

#include <sys/types.h>

struct disks {
  size_t numdisks; // this is really const
  struct process **queues;
};

struct disks *disks_create(const size_t);
void disks_destroy(struct process *, struct disks *);

void disk_request(struct process *, struct disks *, const long int,
                  const char *);
void disk_done(struct process *, struct disks *, long int);

void disk_show(const struct process *, const size_t);

#endif // DISK_H
