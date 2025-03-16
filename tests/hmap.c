#include "hmap.h"
#include "../src/hmap.h"
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void test_hmap_add_heap_ptr()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    uint32_t key = 15;
    char* expected = calloc(5, sizeof(char));
    strncpy(expected, "ciao", 5);
    hmap_putptr(map, &key, sizeof(uint32_t), expected);
    char* actual;
    uint8_t ret = hmap_getptr(map, &key, sizeof(uint32_t), (void**) &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);
    assert(strcmp(expected, actual) == 0);
}

void test_hmap_add_stack_ptr()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    uint32_t key = 90;
    hmap_putptr(map, &key, sizeof(uint32_t), "ciao");
    char* actual;
    uint8_t ret = hmap_getptr(map, &key, sizeof(uint32_t), (void**) &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(strcmp("ciao", actual) == 0);
}

void test_hmap_add_8()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    uint32_t key = 42;
    uint8_t expected = 57;
    hmap_put8(map, &key, sizeof(uint32_t), expected);
    uint8_t actual;
    uint8_t ret = hmap_get8(map, &key, sizeof(uint32_t), &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);
}

void test_hmap_add_16()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    uint32_t key = 42;
    uint16_t expected = 65535;
    hmap_put16(map, &key, sizeof(uint32_t), expected);
    uint16_t actual;
    uint8_t ret = hmap_get16(map, &key, sizeof(uint32_t), &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);
}

void test_hmap_add_32()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    uint32_t key = 42;
    uint32_t expected = 4294967295;
    hmap_put32(map, &key, sizeof(uint32_t), expected);
    uint32_t actual;
    uint8_t ret = hmap_get32(map, &key, sizeof(uint32_t), &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);
}

void test_hmap_add_64()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    uint32_t key = 42;
    uint64_t expected = 1844674407370;
    hmap_put64(map, &key, sizeof(uint32_t), expected);
    uint64_t actual;
    uint8_t ret = hmap_get64(map, &key, sizeof(uint32_t), &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);
}

void test_hmap_remove()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    uint32_t key = 42;
    uint64_t expected = 1844674407370;
    hmap_put64(map, &key, sizeof(uint32_t), expected);
    hmap_remove(map, &key, sizeof(uint32_t));
    uint64_t actual;
    uint8_t ret = hmap_get64(map, &key, sizeof(uint32_t), &actual);
    assert(ret != 0);
    assert(map->len == 0);
}

void test_hmap_rehash()
{
    struct pair { uint32_t key; uint8_t value; };

    struct pair expected[] = {
        { .key = 42, .value = 15 },
        { .key = 36, .value = 68 },
        { .key = 73, .value = 39 },
    };

    hmap* map = hmap_new(2);

    for (int i = 0; i < 3; i++) {
        struct pair p = expected[i];
        hmap_put8(map, &p.key, sizeof(uint32_t), p.value);
    }

    assert(map->len == 3);
    assert(map->cap == 4);

    for (int i = 0; i < 3; i++) {
        struct pair p = expected[i];
        uint8_t actual;
        uint8_t ret = hmap_get8(map, &p.key, sizeof(uint32_t), &actual);
        assert(ret == 0);
        assert(p.value == actual);
    }
}

