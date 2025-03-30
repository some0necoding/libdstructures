#include "dynarr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long long roundnextpow2(long long v);

dynarr* dynarr_new(size_t size)
{
    dynarr* arr = calloc(1, sizeof(dynarr));
    if (!arr) return NULL;

    size = roundnextpow2(size);
    arr->arr = calloc(size, sizeof(void*));
    if (!arr->arr) return NULL;

    arr->cap = size;
    arr->len = 0;
    return arr;
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

int dynarr_add(dynarr* arr, void* elem)
{
    if (arr->len >= arr->cap - 1) {
        size_t old_cap = arr->cap;
        void* old_arr = arr->arr;
        arr->cap *= 2;
        arr->arr = calloc(arr->cap * 2, sizeof(void*));
        if (!arr->arr) return -1;
        memcpy(arr->arr, old_arr, old_cap);
    }

    arr->arr[arr->len++] = elem;
    return 0;
}

int dynarr_set(dynarr* arr, size_t i, void* elem)
{
    if (i < 0 || i >= arr->len) {
        fprintf(stderr, "%s:%d: index %lu out of bound\n", __FILE__, __LINE__, i);
        return -1;
    }

    arr->arr[i] = elem;
    return 0;
}

void* dynarr_get(dynarr *arr, size_t i)
{
    if (i < 0 || i >= arr->len) {
        fprintf(stderr, "%s:%d: index %lu out of bound\n", __FILE__, __LINE__, i);
        return NULL;
    }

    return arr->arr[i];
}

size_t dynarr_size(dynarr* arr)
{
    return arr->len;
}

void dynarr_qsort(dynarr *arr, int (*compar)(const void *, const void *))
{
    qsort(arr->arr, arr->len, sizeof(void*), compar);
}
