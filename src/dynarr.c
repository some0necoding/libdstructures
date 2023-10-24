/**
 *
 * Author: me
 *
 * What this file implements:
 *
 *      nothing.
 *
 *      Everything is implemented as macros in his header file because C does
 *      not support generics but I still want it to do so.
 *
 *      Remember that this implementation is not really type safe (for obvious
 *      reasons i'd say), so be careful.
 *
 *      dynarr is a simple implementation of a dynamic array in C (i.e. an
 *      array of non-fixed size).
 *
 *      The dynarr itself is defined as an anonymous struct containing a
 *      header (dynarr_header) followed by an array of data.
 *
 *      A dynarr can be defined (NOT initialized) as follows:
 *
 *          DYNARR(int) dynarr_of_ints;
 *          DYNARR(char *) dynarr_of_strings;
 *          DYNARR(double) dynarr_of_doubles;
 *
 *      The function DYNARR(T) simply defines an anonymous structure with
 *      no data. The dynarr can be initialized as follows:
 *
 *          dynarr_init(dynarr);
 *
 *      Now your dynarr has been allocated and you can perform some operations
 *      on it (e.g. add(), remove(), set(), etc.). Single functions' specifi-
 *      cations can be read in the header file.
 *
 */

#include "dynarr.h"
