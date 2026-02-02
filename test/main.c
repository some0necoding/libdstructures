#include "dynarr.h"
#include "hmap.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    test_dynarr_append();
    test_dynarr_append_with_reallocation();
    test_dynarr_set();
    test_dynarr_size();
    test_dynarr_remove_start();
    test_dynarr_remove_end();
    test_dynarr_remove_middle();
    test_dynarr_halving();
    test_dynarr_sort();

    test_hmap_add();
    test_hmap_add_str_key();
    test_hmap_add_duplicate_key();
    test_hmap_remove();
    test_hmap_remove_with_collisions();
    test_hmap_remove_unexistent_key();
    test_hmap_rehash();

    printf("All tests succeeded!\n");
    return EXIT_SUCCESS;
}
