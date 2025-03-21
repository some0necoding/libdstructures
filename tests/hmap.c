#include "hmap.h"
#include "../src/hmap.h"
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    hmap_free(map);
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
    hmap_free(map);
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
    hmap_free(map);
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
    hmap_free(map);
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
    hmap_free(map);
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
    hmap_free(map);
}

void test_hmap_add_str_key()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    char* key = "key";
    uint8_t expected = 57;
    hmap_put8(map, &key, strlen(key), expected);
    uint8_t actual;
    uint8_t ret = hmap_get8(map, &key, strlen(key), &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);
    hmap_free(map);
}

void test_hmap_add_duplicate_key()
{
    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);
    char* key = "key";
    hmap_put8(map, &key, strlen(key), 42);
    uint8_t expected = 57;
    hmap_put8(map, &key, strlen(key), expected);
    uint8_t actual;
    uint8_t ret = hmap_get8(map, &key, strlen(key), &actual);
    assert(ret == 0);
    assert(map->len == 1);
    assert(expected == actual);
    hmap_free(map);
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
    hmap_free(map);
}

void test_hmap_rehash()
{
    struct pair { uint32_t key; uint8_t value; };

    struct pair expected[] = {
        { .key = 42, .value = 15 },
        { .key = 36, .value = 68 },
        { .key = 73, .value = 39 },
    };
    size_t expected_size = sizeof(expected) / sizeof(struct pair);

    hmap* map = hmap_new(2);

    for (int i = 0; i < expected_size; i++) {
        struct pair p = expected[i];
        hmap_put8(map, &p.key, sizeof(uint32_t), p.value);
    }

    assert(map->len == expected_size);
    assert(map->cap == 4);

    for (int i = 0; i < expected_size; i++) {
        struct pair p = expected[i];
        uint8_t actual;
        uint8_t ret = hmap_get8(map, &p.key, sizeof(uint32_t), &actual);
        assert(ret == 0);
        assert(p.value == actual);
    }

    hmap_free(map);
}

void test_hmap_entries()
{
    struct pair { char* key; uint8_t value; };

    struct pair expected[] = {
        { .key = "key1", .value = 15 },
        { .key = "key2", .value = 68 },
        { .key = "key3", .value = 39 },
    };
    size_t expected_size = sizeof(expected) / sizeof(struct pair);

    hmap* map = hmap_new(4096);
    assert(map->cap == 4096);

    for (int i = 0; i < expected_size; i++) {
        struct pair p = expected[i];
        hmap_put8(map, p.key, strlen(p.key), p.value);
    }

    hmap_entry** entries;
    size_t entries_size;
    uint8_t ret = hmap_entries(map, &entries, &entries_size);

    assert(entries_size == expected_size);
    for (int i = 0; i < entries_size; i++) {
        hmap_entry* entry = entries[i];
        bool found = false;
        struct pair exp;
        for (int j = 0; j < expected_size && !found; j++) {
            exp = expected[j];
            if (strlen(exp.key) == entry->key_size && memcmp(exp.key, entry->key, entry->key_size) == 0) found = true;
        }
        assert(found == true);
        assert(exp.value == entry->value_8);
    }

    hmap_free(map);
}
