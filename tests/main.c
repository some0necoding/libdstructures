#include "dynarr.h"
#include "hmap.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    test_dynarr_append_ptr();
    test_dynarr_append_8();
    test_dynarr_append_16();
    test_dynarr_append_32();
    test_dynarr_append_64();
    test_dynarr_append_with_reallocation();
    test_dynarr_set_ptr();
    test_dynarr_set_8();
    test_dynarr_set_16();
    test_dynarr_set_32();
    test_dynarr_set_64();
    test_dynarr_remove_start();
    test_dynarr_remove_end();
    test_dynarr_remove_middle();
    test_dynarr_remove_halving();
    test_dynarr_slice_valid();
    test_dynarr_slice_j_less_than_i();
    test_dynarr_slice_j_greater_than_arrlen();
    test_dynarr_size();

    test_hmap_add_heap_ptr();
    test_hmap_add_stack_ptr();
    test_hmap_add_8();
    test_hmap_add_16();
    test_hmap_add_32();
    test_hmap_add_64();
    test_hmap_add_str_key();
    test_hmap_remove();
    test_hmap_rehash();
    test_hmap_add_duplicate_key();
    test_hmap_entries();
    test_hmap_remove_unexistent_key();
    test_hmap_remove_with_collisions();

    return EXIT_SUCCESS;
}
