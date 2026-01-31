#ifndef HASH_H
#define HASH_H

#include "stdint.h"
#include "stdlib.h"

uint32_t murmur3_32(const void* data, size_t dsize);

#endif // !HMAP_H
