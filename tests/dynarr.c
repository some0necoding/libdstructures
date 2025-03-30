#include "../src/dynarr.h"
#include "dynarr.h"
#include <assert.h>

static int compare(const void* elem1, const void* elem2);

void test_dynarr_add()
{
    dynarr* arr = dynarr_new(10);
    dynarr_add(arr, (void*) 1);
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    int n = (int) dynarr_get(arr, 0);
    #pragma GCC diagnostic warning "-Wpointer-to-int-cast"
    assert(n == 1);
}

void test_dynarr_add_with_reallocation()
{
    dynarr* arr = dynarr_new(1);
    dynarr_add(arr, (void*) 1);
    dynarr_add(arr, (void*) 1);
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    int n1 = (int) dynarr_get(arr, 0);
    assert(n1 == 1);
    int n2 = (int) dynarr_get(arr, 1);
    #pragma GCC diagnostic warning "-Wpointer-to-int-cast"
    assert(n2 == 1);
}

void test_dynarr_set()
{
    dynarr* arr = dynarr_new(10);
    dynarr_add(arr, (void*) 1);
    dynarr_set(arr, 0, (void*) 10);
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    int n = (int) dynarr_get(arr, 0);
    #pragma GCC diagnostic warning "-Wpointer-to-int-cast"
    assert(n == 10);
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
    dynarr* arr = dynarr_new(10);
    dynarr_add(arr, (void*) 1);
    dynarr_add(arr, (void*) 5);
    dynarr_add(arr, (void*) 3);
    dynarr_add(arr, (void*) 2);
    dynarr_add(arr, (void*) 4);
    dynarr_qsort(arr, compare);
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    assert((int) dynarr_get(arr, 0) == 1);
    assert((int) dynarr_get(arr, 1) == 2);
    assert((int) dynarr_get(arr, 2) == 3);
    assert((int) dynarr_get(arr, 3) == 4);
    assert((int) dynarr_get(arr, 4) == 5);
    #pragma GCC diagnostic warning "-Wpointer-to-int-cast"
}
