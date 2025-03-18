#include "hmap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
 * Generic hashmap implementation that uses the murmur3 hash function. Collisions
 * are handled with linear probing, but is meant to be replaced with double hashing.
 * The hashmap are dynamically resized following the doubling-halving schema.
 */

static uint32_t murmur3_32(const void* data, size_t msize);
static long long roundnextpow2(long long v);
static float load_factor(hmap* map);
static int hmap_put_entry(hmap* map, hmap_entry* entry);
static int rehash(hmap* map, size_t newsize);

hmap* hmap_new(size_t size)
{
    size = roundnextpow2(size);

    hmap* map = calloc(1, sizeof(hmap));
    if (!map) {
        fprintf(stderr, "%s:%d: cannot allocate memory [errno: %d]\n", __FILE__,
                __LINE__, errno);
        return NULL;
    }

    map->entries = calloc(size, sizeof(hmap_entry*));
    if (!map->entries) {
        fprintf(stderr, "%s:%d: cannot allocate memory [errno: %d]\n", __FILE__,
                __LINE__, errno);
        return NULL;
    }

    map->cap = size;
    map->len = 0;
    map->hash = murmur3_32;

    return map;
}

static long long roundnextpow2(long long v)
{
    size_t size = sizeof(long long);
    v--;
    for (int power = 1; power < size; power *= 2) {
        v |= v >> power;
    }
    v++;
    return v;
}

static uint8_t hmap_put(hmap* map, void* key, size_t key_size, void* value, enum value_type type)
{
    if (load_factor(map) > .5) {
        if (rehash(map, map->cap * 2) < 0) {
            fprintf(stderr, "%s:%d: cannot rehash table\n", __FILE__, __LINE__);
            return -1;
        }
    }

    hmap_entry* entry = calloc(1, sizeof(hmap_entry));
    if (!entry) {
        fprintf(stderr, "%s:%d: cannot allocate memory [errno: %d]\n", __FILE__,
                __LINE__, errno);
        return -1;
    }

    entry->key_size = key_size;
    entry->key = calloc(entry->key_size, 1);
    if (!entry->key) {
        fprintf(stderr, "%s:%d: cannot allocate memory [errno: %d]\n", __FILE__,
                __LINE__, errno);
        return -1;
    }

    memcpy(entry->key, key, entry->key_size);

    switch (type) {
        case PTR:
            entry->value_ptr = value;
            entry->type = PTR;
            break;
        case BIT_8:
            entry->value_8 = *(uint8_t*) value;
            entry->type = BIT_8;
            break;
        case BIT_16:
            entry->value_16 = *(uint16_t*) value;
            entry->type = BIT_16;
            break;
        case BIT_32:
            entry->value_32 = *(uint32_t*) value;
            entry->type = BIT_32;
            break;
        case BIT_64:
            entry->value_64 = *(uint64_t*) value;
            entry->type = BIT_64;
            break;
    }

    return hmap_put_entry(map, entry);
}

uint8_t hmap_putptr(hmap* map, void* key, size_t key_size, void* value)
{
    return hmap_put(map, key, key_size, value, PTR);
}

uint8_t hmap_put8(hmap* map, void* key, size_t key_size, uint8_t value)
{
    return hmap_put(map, key, key_size, &value, BIT_8);
}

uint8_t hmap_put16(hmap* map, void* key, size_t key_size, uint16_t value)
{
    return hmap_put(map, key, key_size, &value, BIT_16);
}

uint8_t hmap_put32(hmap* map, void* key, size_t key_size, uint32_t value)
{
    return hmap_put(map, key, key_size, &value, BIT_32);
}

uint8_t hmap_put64(hmap* map, void* key, size_t key_size, uint64_t value)
{
    return hmap_put(map, key, key_size, &value, BIT_64);
}

static int hmap_put_entry(hmap* map, hmap_entry* entry)
{
    uint32_t hash = map->hash(entry->key, entry->key_size) % map->cap;
    while (map->entries[hash] && memcmp(map->entries[hash]->key, entry->key, map->entries[hash]->key_size) != 0)
        hash = (hash + 1) % map->cap; // skipping busy buckets (linear probing)

    map->len = (map->entries[hash]) ? map->len : map->len + 1; // if entry is being overridden do not increase length
    map->entries[hash] = entry;
    return 0;
}

static float load_factor(hmap* map)
{
    return (float) map->len / (float) map->cap;
}

static int rehash(hmap* map, size_t newsize)
{
    if (newsize < map->len) {
        fprintf(stderr, "%s:%d: size is less than number of entries\n", __FILE__,
                __LINE__);
        return -1;
    }

    hmap_entry** old_table = map->entries;
    size_t old_cap = map->cap;
    map->entries = calloc(newsize, sizeof(hmap_entry*));
    if (!map->entries) {
        fprintf(stderr, "%s:%d: cannot allocate memory [errno: %d]\n", __FILE__,
                __LINE__, errno);
        return -1;
    }

    map->cap = newsize;
    map->len = 0;

    for (int i = 0; i < old_cap; i++) {
        hmap_entry* entry = old_table[i];
        if (!entry) continue;
        if (hmap_put_entry(map, entry) < 0) {
            fprintf(stderr, "%s:%d: cannot rehash entry\n", __FILE__, __LINE__,
                    errno);
            return -1;
        }
    }

    free(old_table);

    return 0;
}

static uint8_t hmap_get(hmap* map, void* key, size_t key_size, void** value)
{
    uint32_t hash = map->hash(key, key_size) % map->cap;
    while (map->entries[hash] && memcmp(map->entries[hash]->key, key, key_size) != 0) hash = (hash + 1) % map->cap; // skipping busy buckets (linear probing)
    hmap_entry* entry = map->entries[hash];
    if (!entry) return 1;

    switch (entry->type) {
        case PTR:
            *value = map->entries[hash]->value_ptr;
            break;
        case BIT_8:
            *value = &map->entries[hash]->value_8;
            break;
        case BIT_16:
            *value = &map->entries[hash]->value_16;
            break;
        case BIT_32:
            *value = &map->entries[hash]->value_32;
            break;
        case BIT_64:
            *value = &map->entries[hash]->value_64;
            break;
    }
    return 0;
}

uint8_t hmap_getptr(hmap* map, void* key, size_t key_size, void** value)
{
    return hmap_get(map, key, key_size, value);
}

uint8_t hmap_get8(hmap* map, void* key, size_t key_size, uint8_t* value)
{
    // directly passing &value to hmap_get would override value, which
    // is the stack address we need to return the value to the user.
    uint8_t* _value;
    uint8_t ret = hmap_get(map, key, key_size, (void**) &_value);
    *value = *_value;
    return ret;
}

uint8_t hmap_get16(hmap* map, void* key, size_t key_size, uint16_t* value)
{
    // directly passing &value to hmap_get would override value, which
    // is the stack address we need to return the value to the user.
    uint16_t* _value;
    uint8_t ret = hmap_get(map, key, key_size, (void**) &_value);
    *value = *_value;
    return ret;
}

uint8_t hmap_get32(hmap* map, void* key, size_t key_size, uint32_t* value)
{
    // directly passing &value to hmap_get would override value, which
    // is the stack address we need to return the value to the user.
    uint32_t* _value;
    uint8_t ret = hmap_get(map, key, key_size, (void**) &_value);
    *value = *_value;
    return ret;
}

uint8_t hmap_get64(hmap* map, void* key, size_t key_size, uint64_t* value)
{
    // directly passing &value to hmap_get would override value, which
    // is the stack address we need to return the value to the user.
    uint64_t* _value;
    uint8_t ret = hmap_get(map, key, key_size, (void**) &_value);
    *value = *_value;
    return ret;
}

int hmap_remove(hmap* map, void* key, size_t key_size)
{
    uint32_t hash = map->hash(key, key_size) % map->cap;
    while (map->entries[hash] && memcmp(map->entries[hash]->key, key, key_size) != 0) hash = (hash + 1) % map->cap; // skipping busy buckets (linear probing)
    free(map->entries[hash]->key);
    free(map->entries[hash]);
    map->entries[hash] = NULL;
    map->len--;
    return 0;
}

void hmap_free(hmap* map)
{
    free(map->entries);
    free(map);
}

/*
 * MurmurHash3 has been created by Austin Appleby and the original code is
 * available in C++ (https://github.com/aappleby/smhasher/).
 * This C port has been created by Seungyoung Kim and has been published as
 * part of qLibc (https://github.com/wolkykim/qlibc).
 */
static uint32_t murmur3_32(const void* data, size_t dsize)
{
    if (data == NULL || dsize == 0) return 0;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    const int nblocks = dsize / 4;
    const uint32_t *blocks = (const uint32_t *) (data);
    const uint8_t *tail = (const uint8_t *) (data + (nblocks * 4));

    uint32_t h = 0;

    int i;
    uint32_t k;
    for (i = 0; i < nblocks; i++) {
        k = blocks[i];

        k *= c1;
        k = (k << 15) | (k >> (32 - 15));
        k *= c2;

        h ^= k;
        h = (h << 13) | (h >> (32 - 13));
        h = (h * 5) + 0xe6546b64;
    }

    k = 0;
    switch (dsize & 3) {
        case 3:
            k ^= tail[2] << 16;
        case 2:
            k ^= tail[1] << 8;
        case 1:
            k ^= tail[0];
            k *= c1;
            k = (k << 15) | (k >> (32 - 15));
            k *= c2;
            h ^= k;
    };

    h ^= dsize;

    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}
