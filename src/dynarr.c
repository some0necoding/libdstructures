#include "dynarr.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static uint64_t min(uint64_t a, uint64_t b);

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

/**
 * Append an entry to a dynamic array.
 *
 * @param arr the array to append to
 * @param entry the entry to append
 * @return 0 no error;
 *         1 memory allocation failed
 *         2 resizing failed
 */
static uint8_t append_entry(dynarr* arr, struct dynarr_entry* entry);
static void free_entry(struct dynarr_type *type, struct dynarr_entry *entry);
static void merge(struct dynarr_entry** src, size_t left, size_t right, size_t end, struct dynarr_entry** dest, int (*compar)(const void*, const void*));

static void free_entry(struct dynarr_type *type, struct dynarr_entry *entry)
{
    if (type->valFree) type->valFree(entry->val);
    free(entry);
}

dynarr* dynarr_new(size_t size)
{
    dynarr* arr = NULL;

    arr = calloc(1, sizeof(dynarr));
    if (!arr) goto enomem;

    arr->type = calloc(1, sizeof(struct dynarr_type));
    if (!arr->type) goto enomem;

    size = roundnextpow2(size);
    arr->entries = calloc(size, sizeof(struct dynarr_entry*));
    if (!arr->entries) goto enomem;

    arr->cap = size;
    arr->len = 0;
    return arr;

enomem:
    if (arr->entries) free(arr->entries);
    if (arr->type)    free(arr->type);
    if (arr)          free(arr);
    return NULL;
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

uint8_t dynarr_append(dynarr* arr, void* val)
{
    struct dynarr_entry* entry = calloc(1, sizeof(struct dynarr_entry));
    if (!entry) return 1;

    entry->val = (arr->type->valDup) ? arr->type->valDup(val) : val;
    if (!entry->val) {
        free_entry(arr->type, entry);
        return 3;
    }

    return append_entry(arr, entry);
}

static uint8_t dynarr_resize(dynarr* arr, size_t new_size)
{
    if (new_size < arr->len) return 2;
    void* old_arr = arr->entries;

    arr->cap = new_size;
    arr->entries = calloc(arr->cap, sizeof(struct dynarr_entry*));
    if (!arr->entries) {
        arr->entries = old_arr;
        return 1;
    }

    memcpy(arr->entries, old_arr, arr->len * sizeof(struct dynarr_entry*));
    free(old_arr);
    return 0;
}

static uint8_t append_entry(dynarr* arr, struct dynarr_entry* entry)
{
    if (arr->len >= arr->cap - 1) {
        int ret = dynarr_resize(arr, arr->cap * 2);
        if (ret != 0) {
            free_entry(arr->type, entry);
            return ret;
        }
    }

    arr->entries[arr->len++] = entry;
    return 0;
}

uint8_t dynarr_set(dynarr* arr, size_t i, void* val)
{
    if (i < 0 || i >= arr->len) return 1;
    struct dynarr_entry* entry = arr->entries[i];

    void *old = entry->val;
    entry->val = (arr->type->valDup) ? arr->type->valDup(val) : val;
    if (!entry->val) {
        entry->val = old;
        return 3;
    }
    if (arr->type->valFree) arr->type->valFree(old);

    return 0;
}

uint8_t dynarr_get(dynarr *arr, size_t i, void** val)
{
    if (i < 0 || i >= arr->len) return 1;
    struct dynarr_entry* entry = arr->entries[i];

    *val = entry->val;
    return 0;
}

uint8_t dynarr_remove(dynarr *arr, size_t i)
{
    if (i < 0 || i >= arr->len) return 3;

    /**
     * When an element different than the last one is removed all the following
     * are shifted back by 1 in order to keep the array dense. In case of error,
     * if we want to preserve the original dynarr state, we would need to unshift
     * all elements to their original position, requiring a second annoying
     * iteration.
     *
     * To avoid this we initially just swap the i-th element (the one to be
     * removed) with the last one and shrink the array, effectively reducing the
     * size of the dynarr but without shifting anything. Also, a reference to
     * the element to be removed is kept.
     *
     * Now resizing can be performed. If it goes well following elements are
     * shifted by 1, the last element (now at index i) is inserted after them
     * and the element to be removed is free'd; otherwise the last element and
     * the one to be removed are put back in their original positions, the
     * length is increased and the error is returned.
     */

    bool is_last = i == arr->len - 1;
    struct dynarr_entry* rm = arr->entries[i];
    if (!is_last)
        arr->entries[i] = arr->entries[arr->len - 1];
    arr->entries[arr->len - 1] = NULL;
    arr->len--;

    uint8_t ret = 0;

    if (arr->len < arr->cap * .25) {
        ret = dynarr_resize(arr, arr->cap / 2);
        if (ret != 0) goto fail;
    }

    if (!is_last) {
        struct dynarr_entry* last = arr->entries[i];
        for (size_t j = i + 1; j < arr->len; j++)
            arr->entries[j - 1] = arr->entries[j];
        arr->entries[arr->len - 1] = last;
    }

    free_entry(arr->type, rm);
    goto done;

fail:
    arr->len++;
    arr->entries[arr->len - 1] = arr->entries[i];
    arr->entries[i] = rm;

done:
    return ret;
}

size_t dynarr_size(dynarr* arr)
{
    return arr->len;
}

/**
 * The following sort is implemented with a bottom-up mergesort. Not adaptive. It
 * uses O(n) space and O(nlogn) time.
 */
uint8_t dynarr_sort(dynarr *arr)
{
    struct dynarr_entry** temp = calloc(arr->len, sizeof(struct dynarr_entry*));
    if (!temp) return 1;

    if (!arr->type->valCompare) return 2;

    for (size_t width = 1; width < arr->len; width = width * 2) {
        for (int i = 0; i < arr->len; i = i + 2 * width) {
            merge(arr->entries, i, min(i + width, arr->len),
                    min(i + 2 * width, arr->len), temp, arr->type->valCompare);
        }
        memcpy(arr->entries, temp, arr->len * sizeof(struct dynarr_entry*));
    }

    free(temp);
    return 0;
}

static uint64_t min(uint64_t a, uint64_t b) { return (a < b) ? a : b; }

static void merge(
    struct dynarr_entry** src,
	size_t left,
	size_t right,
	size_t end,
	struct dynarr_entry** dest,
	int (*compar)(const void*, const void*)
) {
    size_t i = left;
    size_t j = right;
    for (size_t k = left; k < end; k++) {
        if (i < right && (j >= end || compar(src[i]->val, src[j]->val) <= 0)) {
            dest[k] = src[i++];
        } else {
            dest[k] = src[j++];
        }
    }
}

void dynarr_free(dynarr *arr)
{
    for (int i = 0; i < arr->len; i++)
        free_entry(arr->type, arr->entries[i]);
    free(arr->entries);
    free(arr->type);
    free(arr);
}
