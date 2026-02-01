#include "hmap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long long roundnextpow2(long long v);
static float load_factor(hmap* map);

static void hmap_put_entry(hmap* map, struct hmap_entry* entry);
static void free_entry(struct hmap_type *type, struct hmap_entry *entry);

/**
 * Rehash map to reset its size to newsize. On error the hashmap remains unchanged.
 *
 * @param map the map to rehash
 * @param newsize the new size of the map
 * @return 0 if no error occurs;
 *         1 if memory allocation fails;
 *         2 if newsize is less than the current number of entries in the map.
 */
static uint8_t rehash(hmap* map, size_t newsize);

static void compact_cluster(hmap* map, uint32_t empty_slot);
static bool overflow(uint32_t start, uint32_t end);
static bool different_cluster(uint32_t empty_slot, uint32_t curr_slot, uint32_t hash);
static uint32_t find_slot(hmap* map, const void* key);

static void free_entry(struct hmap_type *type, struct hmap_entry *entry)
{
    if (type->keyFree) type->keyFree(entry->key);
    if (type->valFree) type->valFree(entry->val);
    free(entry);
}

hmap* hmap_new(size_t size)
{
    hmap* map = NULL;

    map = calloc(1, sizeof(hmap));
    if (!map) goto enomem;

    map->type = calloc(1, sizeof(struct hmap_type));
    if (!map->type) goto enomem;

    size = roundnextpow2(size);
    map->entries = calloc(size, sizeof(struct hmap_entry*));
    if (!map->entries) goto enomem;

    map->cap = size;
    map->len = 0;

    return map;

enomem:
    if (map->entries) free(map->entries);
    if (map->type)    free(map->type);
    if (map)          free(map);
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

uint8_t hmap_put(hmap* map, void* key, void* value)
{
    if (load_factor(map) > .5) {
        int ret = rehash(map, map->cap * 2);
        if (ret > 0) return ret;
    }

    struct hmap_entry* entry = calloc(1, sizeof(struct hmap_entry));
    if (!entry) return 1;

    entry->key = (map->type->keyDup) ? map->type->keyDup(key) : key;
    if (!entry->key) {
        free_entry(map->type, entry);
        return 3;
    }

    entry->val = (map->type->valDup) ? map->type->valDup(value) : value;
    if (!entry->val) {
        free_entry(map->type, entry);
        return 3;
    }

    hmap_put_entry(map, entry);
    return 0;
}

static void hmap_put_entry(hmap* map, struct hmap_entry* entry)
{
    const uint32_t hash = find_slot(map, entry->key);

    // if entry is being overridden do not increase length and free the old entry
    if (map->entries[hash]) free_entry(map->type, map->entries[hash]);
    else                    map->len++;

    map->entries[hash] = entry;
}

static uint32_t find_slot(hmap* map, const void* key)
{
    uint32_t hash = map->type->hash(key) % map->cap;
    while (map->entries[hash] && map->type->keyCompare(map->entries[hash]->key, key) != 0)
        hash = (hash + 1) % map->cap; // probe linearly
    return hash;
}

static float load_factor(hmap* map)
{
    return (float) map->len / (float) map->cap;
}

static uint8_t rehash(hmap* map, size_t newsize)
{
    if (newsize < map->len) return 2;

    struct hmap_entry** old_table = map->entries;
    size_t old_cap = map->cap;
    map->entries = calloc(newsize, sizeof(struct hmap_entry*));
    if (!map->entries) {
        map->entries = old_table;
        return 1;
    }

    map->cap = newsize;
    map->len = 0;

    for (int i = 0; i < old_cap; i++) {
        struct hmap_entry* entry = old_table[i];
        if (entry) hmap_put_entry(map, entry);
    }

    free(old_table);
    return 0;
}

uint8_t hmap_get(hmap* map, void* key, void** value)
{
    const uint32_t hash = find_slot(map, key);
    struct hmap_entry* entry = map->entries[hash];
    if (!entry) return 1;

    *value = entry->val;
    return 0;
}

uint8_t hmap_remove(hmap* map, void* key)
{
    const uint32_t hash = find_slot(map, key);
    struct hmap_entry* entry = map->entries[hash];
    if (!entry) return 1;
    if (map->type->keyFree) map->type->keyFree(entry->key);
    if (map->type->valFree) map->type->valFree(entry->val);
    free_entry(map->type, entry);
    map->entries[hash] = NULL;
    map->len--;
    compact_cluster(map, hash);
    return 0;
}

/**
 * If an entry inside a cluster (probe sequence of collided entries, i.e. with
 * the same hash) is deleted, it causes the probe to stop even if beyond it
 * there are other elements of the same cluster.
 *
 * e.g. (linear probing)
 *
 * given a1 a2 a3 entries with different keys but same hash:
 *
 * ... a1 a2 a3 ...
 *
 * after deleting a2:
 *
 * ... a1 xx a3 ...
 *        ^
 * probe stops here and a3 is unreachable
 *
 * For this reason the cluster should be compacted relocating each entry after
 * the empty slot, in order. An entry is already in the right position - and
 * consequently is part of a different cluster - if its hash lies between the
 * currently empty slot index (excluded) and its current slot index (also
 * considering index overflow).
 * If an entry is not in the right position - and thus is part of this same
 * cluster - it is moved in the currently empty slot and its slot is marked as
 * the new empty.
 *
 * right position:
 *  if (empty <= slot) |           empty..hash..slot            |
 *  if (slot < empty)  |.hash..slot                 empty.......| or
 *                     |.......slot                 empty..hash.|
 */
static void compact_cluster(hmap* map, uint32_t empty_slot)
{
    for (uint32_t slot = (empty_slot + 1) % map->cap; map->entries[slot]; slot = (slot + 1) % map->cap) {
        struct hmap_entry* entry = map->entries[slot];
        uint32_t hash = map->type->hash(entry->key);
        if (different_cluster(empty_slot, slot, hash)) break;
        map->entries[empty_slot] = entry;
        map->entries[slot] = NULL;
        empty_slot = slot;
    }
}

static bool overflow(uint32_t start, uint32_t end) { return start > end; }

static bool different_cluster(uint32_t empty_slot, uint32_t curr_slot, uint32_t hash)
{
    if (!overflow(empty_slot, curr_slot))
        return empty_slot < hash && hash <= curr_slot;
    return hash <= curr_slot || empty_slot < hash;
}

void hmap_free(hmap* map)
{
    for (uint32_t i = 0; i < map->cap; i++) {
        struct hmap_entry* entry = map->entries[i];
        if (entry) free_entry(map->type, entry);
    }
    free(map->entries);
    free(map->type);
    free(map);
}
