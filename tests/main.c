#include "dynarr.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    test_dynarr_add();
    test_dynarr_add_with_reallocation();
    test_dynarr_set();
    test_dynarr_size();
    return EXIT_SUCCESS;
}
