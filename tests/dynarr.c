#include "../src/dynarr.h"
#include "dynarr.h"
#include <assert.h>
#include <stdint.h>

static int compare(const void* elem1, const void* elem2);

void test_dynarr_add()
{
    dynarr* arr = dynarr_new(10);
    dynarr_add(arr, (void*) 42);
    int n;
    uint8_t ret = dynarr_get(arr, 0, (void**) &n);
    assert(ret == 0);
    assert(n == 42);
}

void test_dynarr_add_with_reallocation()
{
    dynarr* arr = dynarr_new(1);
    dynarr_add(arr, (void*) 42);
    dynarr_add(arr, (void*) 36);
    int n1;
    int ret = dynarr_get(arr, 0, (void**) &n1);
    assert(ret == 0);
    assert(n1 == 42);
    int n2;
    ret = dynarr_get(arr, 1, (void**) &n2);
    assert(ret == 0);
    assert(n2 == 36);
}

void test_dynarr_set()
{
    dynarr* arr = dynarr_new(10);
    dynarr_add(arr, (void*) 1);
    dynarr_set(arr, 0, (void*) 42);
    int n;
    uint8_t ret = dynarr_get(arr, 0, (void**) &n);
    assert(n == 42);
}

void test_dynarr_size()
{
    dynarr* arr = dynarr_new(10);
    dynarr_add(arr, (void*) 1);
    dynarr_add(arr, (void*) 1);
    dynarr_add(arr, (void*) 1);
    dynarr_add(arr, (void*) 1);
    assert(dynarr_size(arr) == 4);
}

static int compare(const void* elem1, const void* elem2)
{
    int* n1 = (int*) elem1;
    int* n2 = (int*) elem2;
    if (*n1 < *n2) return -1;
    if (*n1 > *n2) return 1;
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
        int ret = dynarr_add(arr, (void**) &input[i]);
        assert(ret == 0);
    }

    dynarr_qsort(arr, compare);

    for (int i = 0; i < expected_size; i++) {
        int elem;
        int ret = dynarr_get(arr, i, (void**) &elem);
        assert(ret == 0);
        assert(elem == expected[i]);
    }
}
