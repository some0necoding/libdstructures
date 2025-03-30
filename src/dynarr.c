#include "dynarr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long long roundnextpow2(long long v);
static uint8_t dynarr_append(dynarr* arr, void* elem, enum elem_type type);
static uint8_t dynarr_set(dynarr* arr, size_t i, void* elem, enum elem_type type);
static uint8_t dynarr_get(dynarr *arr, size_t i, void** elem);

dynarr* dynarr_new(size_t size)
{
    dynarr* arr = calloc(1, sizeof(dynarr));
    if (!arr) return NULL;

    size = roundnextpow2(size);
    arr->entries = calloc(size, sizeof(void*));
    if (!arr->entries) return NULL;

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

uint8_t dynarr_appendptr(dynarr* arr, void* elem)
{
    return dynarr_append(arr, elem, PTR);
}

uint8_t dynarr_append8  (dynarr* arr, uint8_t elem)
{
    return dynarr_append(arr, &elem, BIT_8);
}

uint8_t dynarr_append16 (dynarr* arr, uint16_t elem)
{
    return dynarr_append(arr, &elem, BIT_16);
}

uint8_t dynarr_append32 (dynarr* arr, uint32_t elem)
{
    return dynarr_append(arr, &elem, BIT_32);
}

uint8_t dynarr_append64 (dynarr* arr, uint64_t elem)
{
    return dynarr_append(arr, &elem, BIT_64);
}

static uint8_t dynarr_append(dynarr* arr, void* elem, enum elem_type type)
{
    if (arr->len >= arr->cap - 1) {
        size_t old_cap = arr->cap;
        void* old_arr = arr->entries;
        arr->cap *= 2;
        arr->entries = calloc(arr->cap * 2, sizeof(void*));
        if (!arr->entries) return 1;
        memcpy(arr->entries, old_arr, old_cap * sizeof(dynarr_entry*));
    }

    dynarr_entry* entry = calloc(1, sizeof(dynarr_entry));
    if (!entry) return 1;

    entry->type = type;
    switch (type) {
        case PTR:
            entry->elem_ptr = elem;
            break;
        case BIT_8:
            entry->elem_8 = *(uint8_t*) elem;
            break;
        case BIT_16:
            entry->elem_16 = *(uint16_t*) elem;
            break;
        case BIT_32:
            entry->elem_32 = *(uint32_t*) elem;
            break;
        case BIT_64:
            entry->elem_64 = *(uint64_t*) elem;
            break;
    }
    arr->entries[arr->len++] = entry;

    return 0;
}

uint8_t dynarr_setptr(dynarr* arr, size_t i, void* elem)
{
    return dynarr_set(arr, i, elem, PTR);
}

uint8_t dynarr_set8  (dynarr* arr, size_t i, uint8_t elem)
{
    return dynarr_set(arr, i, &elem, BIT_8);
}

uint8_t dynarr_set16 (dynarr* arr, size_t i, uint16_t elem)
{
    return dynarr_set(arr, i, &elem, BIT_16);
}

uint8_t dynarr_set32 (dynarr* arr, size_t i, uint32_t elem)
{
    return dynarr_set(arr, i, &elem, BIT_32);
}

uint8_t dynarr_set64 (dynarr* arr, size_t i, uint64_t elem)
{
    return dynarr_set(arr, i, &elem, BIT_64);
}

static uint8_t dynarr_set(dynarr* arr, size_t i, void* elem, enum elem_type type)
{
    if (i < 0 || i >= arr->len) return 1;
    dynarr_entry* entry = arr->entries[i];

    entry->type = type;
    switch (type) {
        case PTR:
            entry->elem_ptr = elem;
            break;
        case BIT_8:
            entry->elem_8 = *(uint8_t*) elem;
            break;
        case BIT_16:
            entry->elem_16 = *(uint16_t*) elem;
            break;
        case BIT_32:
            entry->elem_32 = *(uint32_t*) elem;
            break;
        case BIT_64:
            entry->elem_64 = *(uint64_t*) elem;
            break;
    }

    return 0;
}

uint8_t dynarr_getptr(dynarr* arr, size_t i, void** elem)
{
    return dynarr_get(arr, i, elem);
}

uint8_t dynarr_get8(dynarr* arr, size_t i, uint8_t* elem)
{
    uint8_t* _elem;
    int ret = dynarr_get(arr, i, (void**) &_elem);
    if (ret == 0) *elem = *_elem;
    return ret;
}

uint8_t dynarr_get16(dynarr* arr, size_t i, uint16_t* elem)
{
    uint16_t* _elem;
    int ret = dynarr_get(arr, i, (void**) &_elem);
    if (ret == 0) *elem = *_elem;
    return ret;
}

uint8_t dynarr_get32(dynarr* arr, size_t i, uint32_t* elem)
{
    uint32_t* _elem;
    int ret = dynarr_get(arr, i, (void**) &_elem);
    if (ret == 0) *elem = *_elem;
    return ret;
}

uint8_t dynarr_get64(dynarr* arr, size_t i, uint64_t* elem)
{
    uint64_t* _elem;
    int ret = dynarr_get(arr, i, (void**) &_elem);
    if (ret == 0) *elem = *_elem;
    return ret;
}

static uint8_t dynarr_get(dynarr *arr, size_t i, void** elem)
{
    if (i < 0 || i >= arr->len) return 1;
    dynarr_entry* entry = arr->entries[i];

    switch (entry->type) {
        case PTR:
            *elem = entry->elem_ptr;
            break;
        case BIT_8:
            *elem = &entry->elem_8;
            break;
        case BIT_16:
            *elem = &entry->elem_16;
            break;
        case BIT_32:
            *elem = &entry->elem_32;
            break;
        case BIT_64:
            *elem = &entry->elem_64;
            break;
    }

    return 0;
}

size_t dynarr_size(dynarr* arr)
{
    return arr->len;
}

void dynarr_qsort(dynarr *arr, int (*compar)(const void *, const void *)) {}
