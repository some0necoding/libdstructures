#ifndef TYPES_H
#define TYPES_H

#include "stdint.h"

void*    uint64_dup(void *val);
void     uint64_free(void *val);
int      uint64_compare(const void *val1, const void *val2);
uint32_t uint64_hash(const void* val);

void*    int64_dup(void *val);
void     int64_free(void *val);
int      int64_compare(const void *val1, const void *val2);
uint32_t int64_hash(const void* val);

void*    str_dup(void *val);
void     str_free(void *val);
int      str_compare(const void *val1, const void *val2);

#endif // !TYPES_H
