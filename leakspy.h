#ifndef LEAKSPY_H
#define LEAKSPY_H

#include <stddef.h>

void *leakspy_malloc(size_t size);
int leakspy_free_by_id(int id);
void leakspy_show_allocations(void);
void leakspy_report(void);
void leakspy_cleanup(void);

#endif
