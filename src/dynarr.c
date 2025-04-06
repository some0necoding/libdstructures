#include "dynarr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long long roundnextpow2(long long v);

/**
 * Resize the array of entries to new_size.
 *
 * @param the array whose entries array will be resized
 * @param new_size the new size of the entries array
 * @return 0 no error;
 *         1 memory allocation failed
 *         2 new_size is less than the current number of entries
 */
static uint8_t dynarr_resize(dynarr* arr, size_t new_size);
static uint8_t dynarr_append(dynarr* arr, void* elem, enum elem_type type);

/**
 * Append an entry to a dynamic array.
 *
 * @param arr the array to append to
 * @param entry the entry to append
 * @return 0 no error;
 *         1 memory allocation failed
 *         2 resizing failed
 */
static uint8_t dynarr_append_entry(dynarr* arr, dynarr_entry* entry);
static uint8_t dynarr_set(dynarr* arr, size_t i, void* elem, enum elem_type type);
static uint8_t dynarr_get(dynarr *arr, size_t i, void** elem);

/**
 * Make a copy of an entry.
 *
 * @param entry the entry to copy
 * @return a pointer to the copy if no error occurs; NULL if memory allocation
 *         fails
 */
static dynarr_entry* dynarr_entry_copy(dynarr_entry* entry);

dynarr* dynarr_new(size_t size)
{
    dynarr* arr = calloc(1, sizeof(dynarr));
    if (!arr) return NULL;

    size = roundnextpow2(size);
    arr->entries = calloc(size, sizeof(dynarr_entry*));
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

    return dynarr_append_entry(arr, entry);
}

static uint8_t dynarr_resize(dynarr* arr, size_t new_size)
{
    if (new_size < arr->len) return 2;
    void* old_arr = arr->entries;

    arr->cap = new_size;
    arr->entries = calloc(arr->cap, sizeof(dynarr_entry*));
    if (!arr->entries) return 1;

    memcpy(arr->entries, old_arr, arr->len * sizeof(dynarr_entry*));
    free(old_arr);
    return 0;
}

static uint8_t dynarr_append_entry(dynarr* arr, dynarr_entry* entry)
{
    if (arr->len >= arr->cap - 1) {
        int ret = dynarr_resize(arr, arr->cap * 2);
        if (ret != 0) return ret;
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

uint8_t dynarr_slice(dynarr* arr, size_t i, size_t j, dynarr** slice)
{
    size_t arr_size = dynarr_size(arr);
    if (j > arr_size) j = arr_size;

    size_t amount = (j < i) ? 0 : j - i;
    *slice = dynarr_new(amount);
    if (!*slice) return 1;
    if (amount == 0) return 0;

    while ((*slice)->len < amount) {
        dynarr_entry* entry = arr->entries[(*slice)->len + i];
        dynarr_entry* copy = dynarr_entry_copy(entry);
        if (!copy) return 1;
        (*slice)->entries[(*slice)->len++] = copy;
    }

    return 0;
}

static dynarr_entry* dynarr_entry_copy(dynarr_entry* entry)
{
    dynarr_entry* copy = calloc(1, sizeof(dynarr_entry));
    if (!copy) return NULL;

    copy->type = entry->type;
    switch (copy->type) {
        case PTR:
            copy->elem_ptr = entry->elem_ptr;
            break;
        case BIT_8:
            copy->elem_8 = entry->elem_8;
            break;
        case BIT_16:
            copy->elem_16 = entry->elem_16;
            break;
        case BIT_32:
            copy->elem_32 = entry->elem_32;
            break;
        case BIT_64:
            copy->elem_64 = entry->elem_64;
            break;
    }

    return copy;
}

uint8_t dynarr_remove(dynarr *arr, size_t i)
{
    if (i < 0 || i >= arr->len) return 3;
    free(arr->entries[i]);
    for (i = i + 1; i < arr->len; i++) {
        arr->entries[i - 1] = arr->entries[i];
    }
    arr->len--;
    arr->entries[arr->len] = NULL;
    if (arr->len < arr->cap * .25) {
        uint8_t ret = dynarr_resize(arr, arr->cap / 2);
        if (ret != 0) return ret;
    }
    return 0;
}

size_t dynarr_size(dynarr* arr)
{
    return arr->len;
}

void dynarr_qsort(dynarr *arr, int (*compar)(const void *, const void *)) {}

void dynarr_free(dynarr *arr)
{
    for (int i = 0; i < arr->len; i++) free(arr->entries[i]);
    free(arr->entries);
    free(arr);
}
