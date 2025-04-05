#include "../src/dynarr.h"
#include "dynarr.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compare(const void* elem1, const void* elem2);
static void test_dynarr_remove(const uint8_t expected[], size_t expected_size, size_t index);

void test_dynarr_append_ptr()
{
    dynarr* arr = dynarr_new(10);
    char* expected = calloc(5, sizeof(char));
    strncpy(expected, "ciao", 5);
    dynarr_appendptr(arr, expected);
    char* actual;
    uint8_t ret = dynarr_getptr(arr, 0, (void**) &actual);
    assert(ret == 0);
    assert(strcmp(expected, actual) == 0);
}

void test_dynarr_append_8()
{
    dynarr* arr = dynarr_new(10);
    uint8_t expected = 42;
    dynarr_append8(arr, expected);
    uint8_t actual;
    uint8_t ret = dynarr_get8(arr, 0, &actual);
    assert(ret == 0);
    assert(actual == expected);
}

void test_dynarr_append_16()
{
    dynarr* arr = dynarr_new(10);
    uint16_t expected = 42;
    dynarr_append16(arr, expected);
    uint16_t actual;
    uint16_t ret = dynarr_get16(arr, 0, &actual);
    assert(ret == 0);
    assert(actual == expected);
}

void test_dynarr_append_32()
{
    dynarr* arr = dynarr_new(10);
    uint32_t expected = 42;
    dynarr_append32(arr, expected);
    uint32_t actual;
    uint32_t ret = dynarr_get32(arr, 0, &actual);
    assert(ret == 0);
    assert(actual == expected);
}

void test_dynarr_append_64()
{
    dynarr* arr = dynarr_new(10);
    uint64_t expected = 42;
    dynarr_append64(arr, expected);
    uint64_t actual;
    uint64_t ret = dynarr_get64(arr, 0, &actual);
    assert(ret == 0);
    assert(actual == expected);
}

void test_dynarr_append_with_reallocation()
{
    const uint8_t expected[] = { 42, 36 };
    const size_t expected_size = sizeof(expected) / sizeof(uint8_t);

    dynarr* arr = dynarr_new(1);

    for (int i = 0; i < expected_size; i++) {
        int ret = dynarr_append8(arr, expected[i]);
        assert(ret == 0);
    }

    for (int i = 0; i < expected_size; i++) {
        uint8_t actual;
        int ret = dynarr_get8(arr, i, &actual);
        assert(ret == 0);
        assert(actual == expected[i]);
    }
}

void test_dynarr_set_ptr()
{
    dynarr* arr = dynarr_new(10);
    char* first = calloc(6, sizeof(char));
    strncpy(first, "first", 6);
    uint8_t ret = dynarr_appendptr(arr, first);
    assert(ret == 0);
    char* expected = calloc(5, sizeof(char));
    strncpy(expected, "ciao", 5);
    ret = dynarr_setptr(arr, 0, expected);
    assert(ret == 0);
    char* actual;
    ret = dynarr_getptr(arr, 0, (void**) &actual);
    assert(ret == 0);
    assert(strcmp(actual, expected) == 0);
}

void test_dynarr_set_8()
{
    dynarr* arr = dynarr_new(10);
    dynarr_append8(arr, 1);
    dynarr_set8(arr, 0, 42);
    uint8_t n;
    uint8_t ret = dynarr_get8(arr, 0, &n);
    assert(n == 42);
}

void test_dynarr_set_16()
{
    dynarr* arr = dynarr_new(10);
    dynarr_append16(arr, 1);
    dynarr_set16(arr, 0, 42);
    uint16_t n;
    uint16_t ret = dynarr_get16(arr, 0, &n);
    assert(n == 42);
}

void test_dynarr_set_32()
{
    dynarr* arr = dynarr_new(10);
    dynarr_append32(arr, 1);
    dynarr_set32(arr, 0, 42);
    uint32_t n;
    uint32_t ret = dynarr_get32(arr, 0, &n);
    assert(n == 42);
}

void test_dynarr_set_64()
{
    dynarr* arr = dynarr_new(10);
    dynarr_append64(arr, 1);
    dynarr_set64(arr, 0, 42);
    uint64_t n;
    uint64_t ret = dynarr_get64(arr, 0, &n);
    assert(n == 42);
}

void test_dynarr_size()
{
    const uint8_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint8_t);

    dynarr* arr = dynarr_new(10);

    for (int i = 0; i < expected_size; i++) {
        int ret = dynarr_append8(arr, expected[i]);
        assert(ret == 0);
    }

    assert(dynarr_size(arr) == 4);
}

static void test_dynarr_remove(const uint8_t expected[], size_t expected_size, size_t index)
{
    dynarr* arr = dynarr_new(10);

    for (int i = 0; i < expected_size; i++) {
        int ret = dynarr_append8(arr, expected[i]);
        assert(ret == 0);
    }

    int ret = dynarr_remove(arr, index);
    assert(ret == 0);
    assert(dynarr_size(arr) == 3);

    for (int i = 0; i < dynarr_size(arr); i++) {
        uint8_t elem;
        int ret = dynarr_get8(arr, i, &elem);
        assert(ret == 0);
        uint32_t expected_index = (i < index) ? i : i + 1;
        assert(elem == expected[expected_index]);
    }
}

void test_dynarr_remove_start()
{
    const uint8_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint8_t);
    test_dynarr_remove(expected, expected_size, 0);
}

void test_dynarr_remove_end()
{
    const uint8_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint8_t);
    test_dynarr_remove(expected, expected_size, 3);
}

void test_dynarr_remove_middle()
{
    const uint8_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint8_t);
    test_dynarr_remove(expected, expected_size, 2);
}

void test_dynarr_remove_halving()
{
    const uint8_t expected[] = { 42, 36, 95, 135, 53 };
    const size_t expected_size = sizeof(expected) / sizeof(uint8_t);

    dynarr* arr = dynarr_new(4);

    for (int i = 0; i < expected_size; i++) {
        int ret = dynarr_append8(arr, expected[i]);
        assert(ret == 0);
    }

    while (dynarr_size(arr) > 1) {
        int ret = dynarr_remove(arr, 1);
        assert(ret == 0);
    }

    assert(dynarr_size(arr) == 1);
    assert(arr->cap == 4);

    uint8_t elem;
    int ret = dynarr_get8(arr, 0, &elem);
    assert(ret == 0);
    assert(elem == expected[0]);
}

static int compare(const void* elem1, const void* elem2)
{
    uint8_t n1 = *(uint8_t*) elem1;
    uint8_t n2 = *(uint8_t*) elem2;
    if (n1 < n2) return -1;
    if (n1 > n2) return 1;
    return 0;
}

void test_dynarr_qsort()
{
    const uint8_t input[] = { 1, 5, 3, 2, 4 };
    const size_t input_size = sizeof(input) / sizeof(uint8_t);
    const uint8_t expected[] = { 1, 2, 3, 4, 5 };
    const size_t expected_size = sizeof(expected) / sizeof(uint8_t);

    dynarr* arr = dynarr_new(10);
    for (int i = 0; i < input_size; i++) {
        uint8_t ret = dynarr_append8(arr, input[i]);
        assert(ret == 0);
    }

    dynarr_qsort(arr, compare);

    for (int i = 0; i < expected_size; i++) {
        uint8_t elem;
        uint8_t ret = dynarr_get8(arr, i, &elem);
        assert(ret == 0);
        assert(elem == expected[i]);
    }
}
