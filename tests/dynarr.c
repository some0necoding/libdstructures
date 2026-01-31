#include "../src/dynarr.h"
#include "../src/types.h"
#include "dynarr.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_dynarr_remove(const uint64_t expected[], size_t expected_size,
        size_t index);
static void test_dynarr_sort_generic(const uint64_t data[], const size_t data_size,
        const uint64_t expected[], size_t expected_size);

void test_dynarr_append()
{
    dynarr* arr = dynarr_new(10);
    arr->type->valFree = str_free;
    arr->type->valDup = str_dup;

    dynarr_append(arr, "ciao");

    char* actual;
    uint8_t ret = dynarr_get(arr, 0, (void**) &actual);
    assert(ret == 0);
    assert(strcmp("ciao", actual) == 0);

    dynarr_free(arr);
}

void test_dynarr_append_with_reallocation()
{
    const uint64_t expected[] = { 42, 36 };
    const size_t expected_size = sizeof(expected) / sizeof(uint64_t);

    dynarr* arr = dynarr_new(1);

    for (uint64_t i = 0; i < expected_size; i++) {
        uint8_t ret = dynarr_append(arr, (void*) expected[i]);
        assert(ret == 0);
    }

    for (uint64_t i = 0; i < expected_size; i++) {
        uint64_t actual;
        uint8_t ret = dynarr_get(arr, i, (void**) &actual);
        assert(ret == 0);
        assert(actual == expected[i]);
    }

    dynarr_free(arr);
}

void test_dynarr_set()
{
    dynarr* arr = dynarr_new(10);
    arr->type->valCompare = str_compare;
    arr->type->valFree = str_free;
    arr->type->valDup = str_dup;

    uint8_t ret = dynarr_append(arr, "first");
    assert(ret == 0);

    ret = dynarr_set(arr, 0, "second");
    assert(ret == 0);

    char* actual;
    ret = dynarr_get(arr, 0, (void**) &actual);
    assert(ret == 0);
    assert(strcmp(actual, "second") == 0);

    dynarr_free(arr);
}

void test_dynarr_size()
{
    const uint64_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint64_t);

    dynarr* arr = dynarr_new(10);

    for (uint64_t i = 0; i < expected_size; i++) {
        uint8_t ret = dynarr_append(arr, (void*) expected[i]);
        assert(ret == 0);
    }

    assert(dynarr_size(arr) == 4);
    dynarr_free(arr);
}

static void test_dynarr_remove(const uint64_t expected[], size_t expected_size,
        size_t index)
{
    dynarr* arr = dynarr_new(10);

    for (uint64_t i = 0; i < expected_size; i++) {
        uint8_t ret = dynarr_append(arr, (void*) expected[i]);
        assert(ret == 0);
    }

    uint8_t ret = dynarr_remove(arr, index);
    assert(ret == 0);
    assert(dynarr_size(arr) == 3);

    for (uint64_t i = 0; i < dynarr_size(arr); i++) {
        uint64_t elem;
        uint8_t ret = dynarr_get(arr, i, (void**) &elem);
        assert(ret == 0);
        uint64_t expected_index = (i < index) ? i : i + 1;
        assert(elem == expected[expected_index]);
    }

    dynarr_free(arr);
}

void test_dynarr_remove_start()
{
    const uint64_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint64_t);
    test_dynarr_remove(expected, expected_size, 0);
}

void test_dynarr_remove_end()
{
    const uint64_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint64_t);
    test_dynarr_remove(expected, expected_size, 3);
}

void test_dynarr_remove_middle()
{
    const uint64_t expected[] = { 42, 36, 95, 135 };
    const size_t expected_size = sizeof(expected) / sizeof(uint64_t);
    test_dynarr_remove(expected, expected_size, 2);
}

void test_dynarr_halving()
{
    const uint64_t expected[] = { 42, 36, 95, 135, 53 };
    const size_t expected_size = sizeof(expected) / sizeof(uint64_t);

    dynarr* arr = dynarr_new(4);

    for (uint64_t i = 0; i < expected_size; i++) {
        uint8_t ret = dynarr_append(arr, (void*) expected[i]);
        assert(ret == 0);
    }

    while (dynarr_size(arr) > 1) {
        uint8_t ret = dynarr_remove(arr, 1);
        assert(ret == 0);
    }

    assert(dynarr_size(arr) == 1);
    assert(arr->cap == 4);

    uint64_t elem;
    uint8_t ret = dynarr_get(arr, 0, (void**) &elem);
    assert(ret == 0);
    assert(elem == expected[0]);

    dynarr_free(arr);
}

static void test_dynarr_sort_generic(
    const uint64_t data[],
	const size_t data_size,
	const uint64_t expected[],
	size_t expected_size
) {
    dynarr* arr = dynarr_new(10);
    arr->type->valCompare = uint64_compare;

    for (uint64_t i = 0; i < data_size; i++) {
        uint8_t ret = dynarr_append(arr, (void*) data[i]);
        assert(ret == 0);
    }

    uint8_t ret = dynarr_sort(arr);
    assert(ret == 0);

    for (uint64_t i = 0; i < expected_size; i++) {
        uint64_t elem;
        uint8_t ret = dynarr_get(arr, i, (void**) &elem);
        assert(ret == 0);
        assert(elem == expected[i]);
    }

    dynarr_free(arr);
}

void test_dynarr_sort()
{
    const uint64_t input[] = { 1, 5, 3, 2, 4 };
    const size_t input_size = sizeof(input) / sizeof(uint64_t);
    const uint64_t expected[] = { 1, 2, 3, 4, 5 };
    const size_t expected_size = sizeof(expected) / sizeof(uint64_t);
    test_dynarr_sort_generic(input, input_size, expected, expected_size);
}
