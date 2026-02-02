#ifndef LIBDSTRUCTURES_HMAP_H
#define LIBDSTRUCTURES_HMAP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>


struct hmap_type {
    void *(*valDup)(void *val);
    void *(*keyDup)(void *key);
    void (*valFree)(void *val);
    void (*keyFree)(void *key);

    /*
     * @param valCompare the comparison function that will be applied to the
     *        values of the map and that must return:
     *          - 0 on equality
     *          - < 0 if val1 < val2
     *          - > 0 if vas1 > val2
     */
    int (*valCompare)(const void *val1, const void *val2);

    /*
     * REQUIRED
     *
     * Compare keys.
     *
     * @param keyCompare the comparison function that will be applied to the
     *        keys of the map and that must return:
     *          - 0 on equality
     *          - < 0 if val1 < val2
     *          - > 0 if vas1 > val2
     */
    int (*keyCompare)(const void *key1, const void *key2);

    /*
     * REQUIRED
     */
    uint32_t (*hash)(const void* key);
};


struct hmap_entry {
    void *key;
    void *val;
};

typedef struct {
    struct hmap_type *type;
    struct hmap_entry** entries;
    size_t len;
    size_t cap;
} hmap;

/**
 * Create a new hmap with the specified initial size. This is the only way to
 * create an hmap instance. All hmap instances must be freed using hmap_free().
 *
 * @param size the initial size of the hmap. If it is not a power of 2 it gets
 *             rounded to the next one.
 * @return     a pointer to an hmap struct with the specified initial size if no
 *             error occurs. NULL is returned and errno set if memory cannot be
 *             allocated.
 */
hmap*    hmap_new(size_t size);

/**
 * Put a new entry in the hmap.
 *
 * @param map the map to update
 * @param key pointer to the key
 * @param value pointer to the value
 * @return 0 if no error occurs;
 *         1 if memory allocation fails;
 *         2 if rehash fails
 *         3 if key/value duplication fails
 */
uint8_t  hmap_put(hmap* map, void* key, void*   value);

/**
 * Retrieve an element from the hmap.
 *
 * @param map the map to lookup in
 * @param key pointer to the key
 * @param value returned value
 * @return 0 no error;
 *         1 key not found
 */
uint8_t  hmap_get(hmap* map, void* key, void** value);

/**
 * Remove an element from the hmap.
 *
 * @param map the map to remove from
 * @param key pointer to the key
 * @return 0 no error;
 *         1 key not found
 */
uint8_t  hmap_remove(hmap* map, void* key);

/**
 * Free an hmap instance.
 *
 * @param map the map to be free'd
 */
void hmap_free(hmap* map);

#endif // !LIBDSTRUCTURES_HMAP_H
