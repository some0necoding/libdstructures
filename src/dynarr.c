/**
 *
 * Author: me
 *
 * What this file implements:
 *
 *      nothing.
 *
 *      Everything is implemented as macros in its header file because C does
 *      not support generics but I still want it to do so. For this reason
 *      you should remember that this implementation is not really type safe,
 *      so be careful.
 *
 *      dynarr is a simple implementation of a dynamic array in C (i.e. an
 *      array of non-fixed size). A dynarr is stretchable but not shrinkable.
 *      This is to reduce overhead of memory allocation.
 *
 *      The dynarr itself is defined as an anonymous struct containing a
 *      header (dynarr_header) followed by an array of data. Because of this
 *      be aware that a dynarr WILL EXIST ONLY INSIDE THE SCOPE IT IS DEFINED
 *      IN. If you need to pass a dynarr between functions you will need to
 *      transform it in a fixed-size array and then to retransform it back
 *      in a dynarr using respectively the functions dynarr_to_arr() and
 *      arr_to_dynarr() (See header file for their specifications).
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
