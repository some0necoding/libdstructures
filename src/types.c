#include "hash.h"
#include "types.h"
#include "stdint.h"
#include "string.h"

static uint64_t min(uint64_t a, uint64_t b);
static uint64_t min(uint64_t a, uint64_t b) { return (a < b) ? a : b; }

void*    uint64_dup(void *val) { return val; }
void     uint64_free(void *val) {}
int      uint64_compare(const void *val1, const void *val2)
{
    return val1 - val2;
}
uint32_t uint64_hash(const void* val)
{
    return murmur3_32(&val, sizeof(uint64_t));
}

void*    int64_dup(void *val) { return val; }
void     int64_free(void *val) {}
int      int64_compare(const void *val1, const void *val2)
{
    return val1 - val2;
}
uint32_t int64_hash(const void* val)
{
    return murmur3_32(&val, sizeof(int64_t));
}

void*    str_dup(void *val) { return strdup(val); }
void     str_free(void *val) { free(val); }
int      str_compare(const void *val1, const void *val2)
{
    size_t n = min(strlen(val1), strlen(val2));
    return strncmp(val1, val2, n);
}
