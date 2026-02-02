#include "hmap.h"
#include "../src/hmap.h"
#include "../src/types.h"
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void test_hmap_add()
{
    hmap* map = hmap_new(4096);
    map->type->keyCompare = uint64_compare;
    map->type->hash = uint64_hash;
    map->type->valDup = str_dup;
    map->type->valFree = str_free;

    assert(map->cap == 4096);

    uint64_t key = 15;
    char* expected = "ciao";

    uint8_t ret = hmap_put(map, (void*) key, expected);
    assert(ret == 0);

    char* actual;
    ret = hmap_get(map, (void*) key, (void**) &actual);

    assert(ret == 0);
    assert(map->len == 1);
    assert(strncmp(expected, actual, strlen(expected)) == 0);

    hmap_free(map);
}

void test_hmap_add_str_key()
{
    hmap* map = hmap_new(4096);
    map->type->keyDup = str_dup;
    map->type->keyFree = str_free;
    map->type->keyCompare = str_compare;
    map->type->hash = str_hash;

    assert(map->cap == 4096);

    char *key = "key";
    uint64_t expected = 15;

    uint8_t ret = hmap_put(map, (void*) key, (void*) expected);
    assert(ret == 0);

    uint64_t actual;
    ret = hmap_get(map, (void*) key, (void**) &actual);

    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);

    hmap_free(map);
}

void test_hmap_add_duplicate_key()
{
    hmap* map = hmap_new(4096);
    map->type->keyDup = str_dup;
    map->type->keyFree = str_free;
    map->type->keyCompare = str_compare;
    map->type->hash = str_hash;

    assert(map->cap == 4096);

    char* key = "key";
    uint8_t ret = hmap_put(map, key, (void*) 42);
    assert(ret == 0);

    uint64_t expected = 57;
    ret = hmap_put(map, key, (void*) expected);
    assert(ret == 0);

    uint64_t actual;
    ret = hmap_get(map, key, (void**) &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);

    hmap_free(map);
}

void test_hmap_remove()
{
    hmap* map = hmap_new(4096);
    map->type->keyCompare = uint64_compare;
    map->type->hash = uint64_hash;

    assert(map->cap == 4096);

    uint64_t key = 42;
    uint64_t expected = 1844674407370;

    uint8_t ret = hmap_put(map, (void*) key, (void*) expected);
    assert(ret == 0);

    ret = hmap_remove(map, (void*) key);
    assert(ret == 0);

    uint64_t actual;
    ret = hmap_get(map, (void*) key, (void**) &actual);
    assert(ret != 0);
    assert(map->len == 0);

    hmap_free(map);
}

uint32_t collision_generator(const void* data)
{
    uint64_t key = (uint64_t) data;
    if (key == 42) return 1;
    if (key == 36) return 1;
    if (key == 73) return 2;
}

void test_hmap_remove_with_collisions()
{
    struct pair { uint64_t key; uint64_t value; };

    struct pair expected[] = {
        { .key = 42, .value = 15 },
        { .key = 36, .value = 68 },
        { .key = 73, .value = 39 },
    };

    size_t expected_size = sizeof(expected) / sizeof(struct pair);

    hmap* map = hmap_new(4096);
    map->type->keyCompare = uint64_compare;
    map->type->hash = collision_generator;

    for (int i = 0; i < expected_size; i++) {
        struct pair p = expected[i];
        uint8_t ret = hmap_put(map, (void*) p.key, (void*) p.value);
        assert(ret == 0);
    }

    for (int i = 0; i < expected_size; i++) {
        struct pair p = expected[i];
        uint8_t ret = hmap_remove(map, (void*) p.key);
        assert(ret == 0);
        assert(map->len == expected_size - i - 1);
    }

    hmap_free(map);
}

void test_hmap_remove_unexistent_key()
{
    hmap* map = hmap_new(4096);
    map->type->keyCompare = uint64_compare;
    map->type->hash = uint64_hash;

    assert(map->cap == 4096);

    uint64_t key = 42;
    uint8_t ret = hmap_remove(map, (void*) key);
    assert(ret != 0);
    assert(map->len == 0);

    hmap_free(map);
}

void test_hmap_rehash()
{
    struct pair { uint64_t key; uint64_t value; };

    struct pair expected[] = {
        { .key = 42, .value = 15 },
        { .key = 36, .value = 68 },
        { .key = 73, .value = 39 },
    };
    size_t expected_size = sizeof(expected) / sizeof(struct pair);

    hmap* map = hmap_new(2);
    map->type->keyCompare = uint64_compare;
    map->type->hash = uint64_hash;

    for (int i = 0; i < expected_size; i++) {
        struct pair p = expected[i];
        hmap_put(map, (void*) p.key, (void*) p.value);
    }

    assert(map->len == expected_size);
    assert(map->cap == 4);

    for (int i = 0; i < expected_size; i++) {
        struct pair p = expected[i];
        uint64_t actual;
        uint8_t ret = hmap_get(map, (void*) p.key, (void**) &actual);
        assert(ret == 0);
        assert(p.value == actual);
    }

    hmap_free(map);
}
